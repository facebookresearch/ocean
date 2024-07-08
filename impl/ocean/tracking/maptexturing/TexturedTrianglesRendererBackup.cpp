/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TexturedTrianglesRendererBackup.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/rendering/glescenegraph/GLESFrameTexture2D.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

#ifdef OCEAN_RENDERING_GLES_USE_ES
	const char* TexturedTrianglesRendererBackup::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
		)SHADER";
#else
	const char* TexturedTrianglesRendererBackup::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
		)SHADER";
#endif

const char* TexturedTrianglesRendererBackup::programVertexShader_ =
	R"SHADER(
		// Vertex attribute
		in vec4 aVertex;

		// Texture coordinate attribute
		in vec4 aTextureCoordinate;

		// Resulting texture coordinate
		out OCEAN_LOWP vec2 vTextureCoordinate;

		void main(void)
		{
			gl_Position = aVertex;

			vTextureCoordinate = aTextureCoordinate.xy;
		}
	)SHADER";

const char* TexturedTrianglesRendererBackup::programFragmentShader_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			fragColor = texture(primaryTexture, vTextureCoordinate).rgba;
		}
	)SHADER";

TexturedTrianglesRendererBackup::~TexturedTrianglesRendererBackup()
{
	release();
}

bool TexturedTrianglesRendererBackup::update(const Rendering::Engine& engine, Frame&& rgbFrame)
{
	if (shaderProgram_.isNull())
	{
		shaderProgram_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgram_);

		Rendering::ShaderProgram::CodePairs codePairs =
		{
			Rendering::ShaderProgram::CodePair({partPlatform_, programVertexShader_}, Rendering::ShaderProgram::ST_VERTEX),
			Rendering::ShaderProgram::CodePair({partPlatform_, programFragmentShader_}, Rendering::ShaderProgram::ST_FRAGMENT)
		};

		std::string errorMessage;
		if (!shaderProgram_->setShader(Rendering::ShaderProgram::SL_GLSL, codePairs, errorMessage))
		{
			release();
			return false;
		}

		textureFramebufferMap_.reserve(32);
	}

	if (vertexSet_.isNull())
	{
		vertexSet_ = engine.factory().createVertexSet();
	}

	if (triangles_.isNull())
	{
		triangles_ = engine.factory().createTriangles();
		triangles_->setVertexSet(vertexSet_);
	}

	if (texture_.isNull())
	{
		texture_ = engine.factory().createFrameTexture2D();
		ocean_assert(texture_);

		texture_->setTextureName("cameraTexture");
		texture_->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
		texture_->setMinificationFilterMode(Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		texture_->setUseMipmaps(true);
	}

	if (rgbFrame.isValid())
	{
		texture_->setTexture(std::move(rgbFrame));
	}

	return true;
}

bool TexturedTrianglesRendererBackup::render(const Rendering::Engine& engine, const HashableTriangle* triangles, const unsigned int numberTriangles, const AnyCamera& anyCamera, const SquareMatrix4& /*projectionMatrix*/, const HomogenousMatrix4& world_T_camera, const Rendering::TextureFramebufferRef& visibleTrianglesFramebuffer, const TextureAtlas& textureAtlas)
{
	if (numberTriangles == 0u)
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	ocean_assert(visibleTrianglesFramebuffer);

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	ocean_assert(triangles != nullptr);

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesVisibleTrianglesFramebuffer = visibleTrianglesFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const unsigned int frameHeight = (numberTriangles + glesVisibleTrianglesFramebuffer.width() - 1u) / glesVisibleTrianglesFramebuffer.width();

	if (!glesVisibleTrianglesFramebuffer.copyColorTextureToFrame(stateFrame_, CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), glesVisibleTrianglesFramebuffer.width(), frameHeight)))
	{
		return false;
	}

	ocean_assert(stateFrame_.isContinuous());
	const uint8_t* const triangleStates = stateFrame_.constdata<uint8_t>();

	Vector2 textureCoordinates[3];

	const Index32 maximalMeshId = numberTriangles / textureAtlas.numberTriangles();
	for (Index32 meshId = 0u; meshId <= maximalMeshId; ++meshId)
	{
		const unsigned int trianglesInMesh = std::min(numberTriangles - meshId * textureAtlas.numberTriangles(), textureAtlas.numberTriangles());
		const unsigned int triangleIdOffset = meshId * textureAtlas.numberTriangles();

		reusableVertices_.clear();
		reusableTextureCoordinates_.clear();

		for (unsigned int meshTriangleId = 0u; meshTriangleId < trianglesInMesh; ++meshTriangleId)
		{
			const Index32 triangleId = meshTriangleId + triangleIdOffset;

			if (triangleStates[triangleId] >= 254u)
			{
				textureAtlas.triangleAtlasTextureCoordinates<false>(meshTriangleId, textureCoordinates);

				reusableVertices_.emplace_back(textureCoordinates[0] * Scalar(2) - Vector2(1, 1)); // the texture coordinates in the texture atlas will be the vertex coordinates when rendering the triangles into the atlas
				reusableVertices_.emplace_back(textureCoordinates[1] * Scalar(2) - Vector2(1, 1));
				reusableVertices_.emplace_back(textureCoordinates[2] * Scalar(2) - Vector2(1, 1));

				constexpr bool respectBorder = true;

				textureAtlas.triangleCameraTextureCoordiantes(anyCamera, flippedCamera_T_world, meshTriangleId, triangles[triangleId].vertices(), textureCoordinates, respectBorder);

				reusableTextureCoordinates_.emplace_back(textureCoordinates[0]);
				reusableTextureCoordinates_.emplace_back(textureCoordinates[1]);
				reusableTextureCoordinates_.emplace_back(textureCoordinates[2]);
			}
		}

		if (!reusableVertices_.empty() && !renderMesh(engine, meshId, reusableVertices_, reusableTextureCoordinates_, texture_, textureAtlas))
		{
			return false;
		}
	}

	return true;
}

bool TexturedTrianglesRendererBackup::render(const Rendering::Engine& engine, const HashableTriangle* triangles, const unsigned int numberMeshes, const Indices32& triangleIds, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const TextureFramebufferMap& textureFramebufferMap, const Rendering::TextureFramebufferRef& renderedMeshFramebuffer, const TextureAtlas& textureAtlas)
{
	if (triangleIds.empty())
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	ocean_assert(triangles != nullptr);
	ocean_assert(renderedMeshFramebuffer);

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

	ocean_assert(triangles != nullptr);

	Vector2 textureCoordinates[3];

	std::vector<Vectors3> vertexGroups(numberMeshes);
	std::vector<Vectors2> textureCoordinateGroups(numberMeshes);

	for (const Index32& triangleId : triangleIds)
	{
		const Index32 meshId = triangleId / textureAtlas.numberTriangles();
		ocean_assert(meshId < numberMeshes);

		Vectors3& meshVertices = vertexGroups[meshId];
		Vectors2& meshTextureCoordinates = textureCoordinateGroups[meshId];

		const unsigned int meshTriangleId = triangleId - meshId * textureAtlas.numberTriangles();

		textureAtlas.triangleAtlasTextureCoordinates<false>(meshTriangleId, textureCoordinates);

		meshVertices.emplace_back(textureCoordinates[0] * Scalar(2) - Vector2(1, 1)); // the texture coordinates in the texture atlas will be the vertex coordinates when rendering the triangles into the atlas
		meshVertices.emplace_back(textureCoordinates[1] * Scalar(2) - Vector2(1, 1));
		meshVertices.emplace_back(textureCoordinates[2] * Scalar(2) - Vector2(1, 1));

		constexpr bool respectBorder = true;

		textureAtlas.triangleCameraTextureCoordiantes(anyCamera, flippedCamera_T_world, meshTriangleId, triangles[triangleId].vertices(), textureCoordinates, respectBorder);

		meshTextureCoordinates.emplace_back(textureCoordinates[0].x(), Scalar(1) - textureCoordinates[0].y());
		meshTextureCoordinates.emplace_back(textureCoordinates[1].x(), Scalar(1) - textureCoordinates[1].y());
		meshTextureCoordinates.emplace_back(textureCoordinates[2].x(), Scalar(1) - textureCoordinates[2].y());
	}

	for (unsigned int meshId = 0u; meshId < numberMeshes; ++meshId)
	{
		if (!vertexGroups[meshId].empty())
		{
			const TextureFramebufferMap::const_iterator iFramebuffer = textureFramebufferMap.find(meshId);

			if (iFramebuffer != textureFramebufferMap.cend())
			{
				if (!renderMesh(engine, meshId, vertexGroups[meshId], textureCoordinateGroups[meshId], iFramebuffer->second, renderedMeshFramebuffer, textureAtlas))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool TexturedTrianglesRendererBackup::latestTexture(const Index32 meshId, Frame& textureFrame) const
{
	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	const TextureFramebufferMap::const_iterator iFramebuffer = textureFramebufferMap_.find(meshId);

	if (iFramebuffer == textureFramebufferMap_.cend())
	{
		return false;
	}

	return iFramebuffer->second->copyColorTextureToFrame(textureFrame);
}

Indices32 TexturedTrianglesRendererBackup::meshIds() const
{
	Indices32 result;
	result.reserve(textureFramebufferMap_.size());

	for (TextureFramebufferMap::const_iterator iFramebuffer = textureFramebufferMap_.cbegin(); iFramebuffer != textureFramebufferMap_.cend(); ++iFramebuffer)
	{
		result.emplace_back(iFramebuffer->first);
	}

	return result;
}

void TexturedTrianglesRendererBackup::release()
{
	texture_.release();
	textureFramebufferMap_.clear();
	shaderProgram_.release();
}

inline bool TexturedTrianglesRendererBackup::isValid() const
{
	return bool(shaderProgram_);
}

bool TexturedTrianglesRendererBackup::renderMesh(const Rendering::Engine& engine, const Index32 meshId, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TextureRef& texture, const TextureAtlas& textureAtlas)
{
	ocean_assert(shaderProgram_);

	TextureFramebufferMap::iterator iFramebuffer = textureFramebufferMap_.find(meshId);

	bool clearFramebuffer = false;

	if (iFramebuffer == textureFramebufferMap_.cend())
	{
		iFramebuffer = textureFramebufferMap_.emplace(meshId, engine.factory().createTextureFramebuffer()).first;
	}

	const Rendering::TextureFramebufferRef& textureFramebuffer = iFramebuffer->second;
	ocean_assert(textureFramebuffer);

	if (!textureFramebuffer->setPixelFormat(FrameType::FORMAT_RGBA32))
	{
		release();
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesTextureFramebuffer.resize(textureAtlas.textureSizePixels(), textureAtlas.textureSizePixels()) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, textureAtlas.textureSizePixels(), textureAtlas.textureSizePixels());
	ocean_assert(GL_NO_ERROR == glGetError());

	if (clearFramebuffer)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		ocean_assert(GL_NO_ERROR == glGetError());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	vertexSet_->setVertices(vertices);
	vertexSet_->setTextureCoordinates(textureCoordinates, 0u);

	ocean_assert(vertices.size() % 3 == 0);
	triangles_->setFaces((unsigned int)(vertices.size() / 3));

	glesShaderProgram.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	texture.force<Rendering::GLESceneGraph::GLESTexture>().bindTexture(glesShaderProgram, 0u);

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTextureFramebuffer.unbindFramebuffer();

	return true;
}

bool TexturedTrianglesRendererBackup::renderMesh(const Rendering::Engine& /*engine*/, const Index32 /*meshId*/, const Vectors3& vertices, const Vectors2& textureCoordinates, const Rendering::TextureFramebufferRef& textureFramebuffer, const Rendering::TextureRef& texture, const TextureAtlas& textureAtlas)
{
	ocean_assert(shaderProgram_);
	ocean_assert(textureFramebuffer);

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesTextureFramebuffer.resize(textureAtlas.textureSizePixels(), textureAtlas.textureSizePixels()) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, textureAtlas.textureSizePixels(), textureAtlas.textureSizePixels());
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	vertexSet_->setVertices(vertices);
	vertexSet_->setTextureCoordinates(textureCoordinates, 0u);

	ocean_assert(vertices.size() % 3 == 0);
	triangles_->setFaces((unsigned int)(vertices.size() / 3));

	glesShaderProgram.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	texture.force<Rendering::GLESceneGraph::GLESTexture>().bindTexture(glesShaderProgram, 0u);

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTextureFramebuffer.unbindFramebuffer();

	return true;
}

}

}

}
