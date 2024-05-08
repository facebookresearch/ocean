/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TexturedTrianglesRenderer.h"

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
	const char* TexturedTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* TexturedTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* TexturedTrianglesRenderer::programVertexShader_ =
	R"SHADER(
		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Vertex attribute
		in vec4 aVertex;

		// The resulting color id.
		flat out OCEAN_HIGHP uint vTriangleId;

		void main(void)
		{
			vec4 vertexInView = modelViewMatrix * aVertex;

			gl_Position = vertexInView;

			vTriangleId = uint(gl_VertexID) / 3u;
		}
	)SHADER";


const char* TexturedTrianglesRenderer::partTextureAtlas_ =
	R"SHADER(
		uniform uint textureAtlasDimensionPixels;
		uniform uint textureAtlasTrianglePixels;

		uniform vec3 outerBorderBarycentricEven[3];
		uniform vec3 outerBorderBarycentricOdd[3];

		void triangleTextureAtlasVertices(in uint triangleId, out vec2 borderVertex0, out vec2 borderVertex1, out vec2 borderVertex2)
		{
			uint bins = textureAtlasDimensionPixels / textureAtlasTrianglePixels;
			uint numberTrianglesInTextureAtlas = bins * bins * 2u;

			uint atlasTriangleId = triangleId % numberTrianglesInTextureAtlas;

			uint binId = atlasTriangleId / 2u;
			uint xBin = binId % bins;
			uint yBin = binId / bins;

			uint leftBinPixels = xBin * textureAtlasTrianglePixels;
			uint topBinPixels = yBin * textureAtlasTrianglePixels;

			OCEAN_HIGHP float leftBinTexture = 2.0 * float(leftBinPixels) / float(textureAtlasDimensionPixels) - 1.0;
			OCEAN_HIGHP float rightBinTexture = 2.0 * float(leftBinPixels + textureAtlasTrianglePixels) / float(textureAtlasDimensionPixels) - 1.0;

			OCEAN_HIGHP float topBinTexture = 1.0 - 2.0 * float(topBinPixels) / float(textureAtlasDimensionPixels);
			OCEAN_HIGHP float bottomBinTexture = 1.0 - 2.0 * float(topBinPixels + textureAtlasTrianglePixels) / float(textureAtlasDimensionPixels);

			if (atlasTriangleId % 2u == 0u)
			{
				borderVertex0 = vec2(leftBinTexture, topBinTexture);
				borderVertex1 = vec2(leftBinTexture, bottomBinTexture);
				borderVertex2 = vec2(rightBinTexture, bottomBinTexture);
			}
			else
			{
				borderVertex0 = vec2(leftBinTexture, topBinTexture);
				borderVertex1 = vec2(rightBinTexture, bottomBinTexture);
				borderVertex2 = vec2(rightBinTexture, topBinTexture);
			}
		}

		vec2 barycentric2cartesian(vec2 triangle0, vec2 triangle1, vec2 triangle2, vec3 barycentric)
		{
			return vec2(triangle0.x * barycentric.x + triangle1.x * barycentric.y + triangle2.x * barycentric.z,
							triangle0.y * barycentric.x + triangle1.y * barycentric.y + triangle2.y * barycentric.z);
		}

		void triangleCameraTextureCoordinatesWithBorderEven(in vec2 textureCoordinate0, in vec2 textureCoordinate1, in vec2 textureCoordinate2, out vec2 borderTextureCoordinate0, out vec2 borderTextureCoordinate1, out vec2 borderTextureCoordinate2)
		{
			borderTextureCoordinate0 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricEven[0]);
			borderTextureCoordinate1 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricEven[1]);
			borderTextureCoordinate2 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricEven[2]);
		}

		void triangleCameraTextureCoordinatesWithBorderOdd(in vec2 textureCoordinate0, in vec2 textureCoordinate1, in vec2 textureCoordinate2, out vec2 borderTextureCoordinate0, out vec2 borderTextureCoordinate1, out vec2 borderTextureCoordinate2)
		{
			borderTextureCoordinate0 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricOdd[0]);
			borderTextureCoordinate1 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricOdd[1]);
			borderTextureCoordinate2 = barycentric2cartesian(textureCoordinate0, textureCoordinate1, textureCoordinate2, outerBorderBarycentricOdd[2]);
		}

	)SHADER";

const char* TexturedTrianglesRenderer::programGeometryShader_ =
	R"SHADER(
		layout (triangles) in;
		layout (triangle_strip, max_vertices = 3) out;

		// Projection matrix
		uniform mat4 projectionMatrix;

		uniform mat3 cameraMatrix;

		// Texture sampler object
		uniform sampler2D visibilityTexture;

		uniform uint visibilityTextureWidth;
		uniform uint visibilityTextureHeight;

		flat in OCEAN_HIGHP uint vTriangleId[];

		out OCEAN_HIGHP vec2 oCameraTextureCoordinate;

		vec2 determineCameraTextureCoordinate(vec4 vertexInView)
		{
			vec4 vertexInClip = projectionMatrix * vertexInView; // with range [-1, 1]
			vec2 normalizedVertexInClip = vertexInClip.xy / vertexInClip.w;

			return vec2(normalizedVertexInClip.x * 0.5 + 0.5, 0.5 - normalizedVertexInClip.y * 0.5); // with range [0, 1]
		}

		void main(void)
		{
			// first, we determine whether the triangle is actually visible

			OCEAN_HIGHP uint textureId = vTriangleId[0];

			OCEAN_HIGHP uint xId = textureId % visibilityTextureWidth;
			OCEAN_HIGHP uint yId = textureId / visibilityTextureWidth;

			vec2 visibilityTextureCoordinate;
			visibilityTextureCoordinate.x = (float(xId) + 0.5) / float(visibilityTextureWidth); // with range [0, 1]
			visibilityTextureCoordinate.y = (float(yId) + 0.5) / float(visibilityTextureHeight);

			float visibility = texture(visibilityTexture, visibilityTextureCoordinate).r;

			if (visibility < 0.9)
			{
				return;
			}

			vec2 atlasVertex0;
			vec2 atlasVertex1;
			vec2 atlasVertex2;

			triangleTextureAtlasVertices(textureId, atlasVertex0, atlasVertex1, atlasVertex2);

			vec2 cameraTextureCoordinate0;
			vec2 cameraTextureCoordinate1;
			vec2 cameraTextureCoordinate2;

			if (textureId % 2u == 0u)
			{
				triangleCameraTextureCoordinatesWithBorderEven(determineCameraTextureCoordinate(gl_in[0].gl_Position), determineCameraTextureCoordinate(gl_in[1].gl_Position), determineCameraTextureCoordinate(gl_in[2].gl_Position), cameraTextureCoordinate0, cameraTextureCoordinate1, cameraTextureCoordinate2);
			}
			else
			{
				triangleCameraTextureCoordinatesWithBorderOdd(determineCameraTextureCoordinate(gl_in[0].gl_Position), determineCameraTextureCoordinate(gl_in[1].gl_Position), determineCameraTextureCoordinate(gl_in[2].gl_Position), cameraTextureCoordinate0, cameraTextureCoordinate1, cameraTextureCoordinate2);
			}

			gl_Position = vec4(atlasVertex0.x, atlasVertex0.y, 0.0, 1.0);
			oCameraTextureCoordinate = cameraTextureCoordinate0;
			EmitVertex();

			gl_Position = vec4(atlasVertex1.x, atlasVertex1.y, 0.0, 1.0);
			oCameraTextureCoordinate = cameraTextureCoordinate1;
			EmitVertex();

			gl_Position = vec4(atlasVertex2.x, atlasVertex2.y, 0.0, 1.0);
			oCameraTextureCoordinate = cameraTextureCoordinate2;
			EmitVertex();

			EndPrimitive();
		}
	)SHADER";

const char* TexturedTrianglesRenderer::programFragmentShader_ =
	R"SHADER(
		// Texture sampler object
		uniform sampler2D cameraTexture;

		in OCEAN_HIGHP vec2 oCameraTextureCoordinate;

		// The out fragment color
		out vec4 fragColor;

		void main()
		{
			if (gl_FrontFacing)
			{
				fragColor = texture(cameraTexture, oCameraTextureCoordinate);
			}
			else
			{
				discard;
			}
		}
	)SHADER";

TexturedTrianglesRenderer::~TexturedTrianglesRenderer()
{
	release();
}

bool TexturedTrianglesRenderer::update(const Rendering::Engine& engine, Frame&& rgbFrame)
{
	if (shaderProgram_.isNull())
	{
		shaderProgram_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgram_);

		Rendering::ShaderProgram::CodePairs codePairs =
		{
			Rendering::ShaderProgram::CodePair({partPlatform_, programVertexShader_}, Rendering::ShaderProgram::ST_VERTEX),
			Rendering::ShaderProgram::CodePair({partPlatform_, partTextureAtlas_, programGeometryShader_}, Rendering::ShaderProgram::ST_GEOMETRY),
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

	if (texture_.isNull())
	{
		texture_ = engine.factory().createFrameTexture2D();
		ocean_assert(texture_);

		texture_->setTextureName("cameraTexture");
		texture_->setMagnificationFilterMode(Rendering::Texture::MAG_MODE_LINEAR);
		texture_->setMinificationFilterMode(Rendering::Texture::MIN_MODE_LINEAR_MIPMAP_LINEAR);
		texture_->setUseMipmaps(true);
	}

	texture_->setTexture(std::move(rgbFrame));

	return true;
}

bool TexturedTrianglesRenderer::render(const Rendering::Engine& engine, const unsigned int numberTriangles, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const Rendering::TextureFramebufferRef& visibleTrianglesFramebuffer, const TextureAtlas& textureAtlas)
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

	const Index32 maximalMeshId = numberTriangles / textureAtlas.numberTriangles();
	for (Index32 meshId = 0u; meshId <= maximalMeshId; ++meshId)
	{
		const unsigned int trianglesInMesh = std::min(numberTriangles - meshId * textureAtlas.numberTriangles(), textureAtlas.numberTriangles());

		if (!renderMesh(engine, meshId, trianglesInMesh, projectionMatrix, world_T_camera, vertexSet, triangles, visibleTrianglesFramebuffer, textureAtlas))
		{
			return false;
		}
	}

	return true;
}

bool TexturedTrianglesRenderer::latestTexture(const Index32 meshId, Frame& textureFrame) const
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

Indices32 TexturedTrianglesRenderer::meshIds() const
{
	Indices32 result;
	result.reserve(textureFramebufferMap_.size());

	for (TextureFramebufferMap::const_iterator iFramebuffer = textureFramebufferMap_.cbegin(); iFramebuffer != textureFramebufferMap_.cend(); ++iFramebuffer)
	{
		result.emplace_back(iFramebuffer->first);
	}

	return result;
}

void TexturedTrianglesRenderer::release()
{
	texture_.release();
	textureFramebufferMap_.clear();
	shaderProgram_.release();
}

inline bool TexturedTrianglesRenderer::isValid() const
{
	return bool(shaderProgram_);
}

bool TexturedTrianglesRenderer::renderMesh(const Rendering::Engine& engine, const Index32 meshId, const unsigned int trianglesInMesh, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const Rendering::TextureFramebufferRef& visibleTrianglesFramebuffer, const TextureAtlas& textureAtlas)
{
	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	ocean_assert(shaderProgram_);

	TextureFramebufferMap::iterator iFramebuffer = textureFramebufferMap_.find(meshId);

	bool clearFramebuffer = false;

	if (iFramebuffer == textureFramebufferMap_.cend())
	{
		iFramebuffer = textureFramebufferMap_.emplace(meshId, engine.factory().createTextureFramebuffer()).first;
		clearFramebuffer = true;
	}

	const Rendering::TextureFramebufferRef& textureFramebuffer = iFramebuffer->second;
	ocean_assert(textureFramebuffer);

	if (!textureFramebuffer->setPixelFormat(FrameType::FORMAT_RGB24))
	{
		release();
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

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


	glesShaderProgram.bind(projectionMatrix, camera_T_world, camera_T_world, SquareMatrix3(false));

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesVisibleTrianglesFramebuffer = visibleTrianglesFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const GLint visibilityTextureWidthLocation = glGetUniformLocation(glesShaderProgram.id(), "visibilityTextureWidth");
	ocean_assert(visibilityTextureWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(visibilityTextureWidthLocation, glesVisibleTrianglesFramebuffer.width());

	const GLint visibilityTextureHeightLocation = glGetUniformLocation(glesShaderProgram.id(), "visibilityTextureHeight");
	ocean_assert(visibilityTextureHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(visibilityTextureHeightLocation, glesVisibleTrianglesFramebuffer.height());


	const GLint textureAtlasDimensionPixelsLocation = glGetUniformLocation(glesShaderProgram.id(), "textureAtlasDimensionPixels");
	ocean_assert(textureAtlasDimensionPixelsLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(textureAtlasDimensionPixelsLocation, textureAtlas.textureSizePixels());

	const GLint textureAtlasTrianglePixelsLocation = glGetUniformLocation(glesShaderProgram.id(), "textureAtlasTrianglePixels");
	ocean_assert(textureAtlasTrianglePixelsLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(textureAtlasTrianglePixelsLocation, textureAtlas.triangleSizePixels());


	const Vectors3 outerBorderBarycentricEven =
	{
		textureAtlas.outerBorderBarycentric(0u, 0u),
		textureAtlas.outerBorderBarycentric(0u, 1u),
		textureAtlas.outerBorderBarycentric(0u, 2u)
	};

	const GLint outerBorderBarycentricEvenLocation = glGetUniformLocation(glesShaderProgram.id(), "outerBorderBarycentricEven");
	ocean_assert(outerBorderBarycentricEvenLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(outerBorderBarycentricEvenLocation, outerBorderBarycentricEven);

	const Vectors3 outerBorderBarycentricOdd =
	{
		textureAtlas.outerBorderBarycentric(1u, 0u),
		textureAtlas.outerBorderBarycentric(1u, 1u),
		textureAtlas.outerBorderBarycentric(1u, 2u)
	};

	const GLint outerBorderBarycentricOddLocation = glGetUniformLocation(glesShaderProgram.id(), "outerBorderBarycentricOdd");
	ocean_assert(outerBorderBarycentricOddLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(outerBorderBarycentricOddLocation, outerBorderBarycentricOdd);


	const GLuint visibilityTextureTextureId = glesVisibleTrianglesFramebuffer.colorTextureId();

	if (visibilityTextureTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, visibilityTextureTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationVisibilityTexture = glGetUniformLocation(glesShaderProgram.id(), "visibilityTexture");
	ocean_assert(locationVisibilityTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationVisibilityTexture, 0);

	vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	texture_.force<Rendering::GLESceneGraph::GLESFrameTexture2D>().bindTexture(glesShaderProgram, 1u);

	const unsigned int firstTriangleId = meshId * textureAtlas.numberTriangles();

	triangles.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles(firstTriangleId, trianglesInMesh);

	glesTextureFramebuffer.unbindFramebuffer();

	return true;
}

}

}

}
