/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/MeshRenderer.h"

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
	const char* MeshRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* MeshRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* MeshRenderer::partTextureAtlas_ =
	R"SHADER(
		uniform uint textureAtlasDimensionPixels;
		uniform uint textureAtlasTrianglePixels;

		uniform vec2 innerBorderTextureCoordinatesEven[3];
		uniform vec2 innerBorderTextureCoordinatesOdd[3];

		OCEAN_HIGHP vec2 textureCoordinateInAtlas(in uint vertexIndex)
		{
			OCEAN_HIGHP uint bins = textureAtlasDimensionPixels / textureAtlasTrianglePixels;
			OCEAN_HIGHP uint numberTrianglesInTextureAtlas = bins * bins * 2u;

			OCEAN_HIGHP uint triangleId = vertexIndex / 3u;
			OCEAN_HIGHP uint triangleVertexIndex = vertexIndex % 3u;

			OCEAN_HIGHP uint atlasTriangleId = triangleId % numberTrianglesInTextureAtlas;

			uint binId = atlasTriangleId / 2u;
			uint xBin = binId % bins;
			uint yBin = binId / bins;

			uint leftBinPixels = xBin * textureAtlasTrianglePixels;
			uint topBinPixels = yBin * textureAtlasTrianglePixels;

			OCEAN_HIGHP vec2 innerBorderTextureCoordinate;

			if (atlasTriangleId % 2u == 0u)
			{
				innerBorderTextureCoordinate = innerBorderTextureCoordinatesEven[triangleVertexIndex];
			}
			else
			{
				innerBorderTextureCoordinate = innerBorderTextureCoordinatesOdd[triangleVertexIndex];
			}

			innerBorderTextureCoordinate.x += float(leftBinPixels) / float(textureAtlasDimensionPixels);
			innerBorderTextureCoordinate.y -= float(topBinPixels) / float(textureAtlasDimensionPixels);

			return innerBorderTextureCoordinate;
		}
	)SHADER";

const char* MeshRenderer::programVertexShader_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		uniform uint uTriangleStateTextureWidth;
		uniform uint uTriangleStateTextureHeight;

		// Vertex attribute
		in vec4 aVertex;

		// Resulting texture coordinate
		out OCEAN_HIGHP vec2 vTextureCoordinate;

		out OCEAN_HIGHP vec2 vTrianglesStateTextureCoordinate;

		void main(void)
		{
			OCEAN_HIGHP uint vertexIndex = uint(gl_VertexID);
			OCEAN_HIGHP uint triangleId = vertexIndex / 3u;

			OCEAN_HIGHP uint xId = triangleId % uTriangleStateTextureWidth;
			OCEAN_HIGHP uint yId = triangleId / uTriangleStateTextureWidth;

			OCEAN_HIGHP float xOutputPosition = (float(xId * 2u) + 0.5) / float(uTriangleStateTextureWidth) - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = (float(yId * 2u) + 0.5) / float(uTriangleStateTextureHeight) - 1.0;

			vTrianglesStateTextureCoordinate = vec2((xOutputPosition + 1.0) / 2.0, (yOutputPosition + 1.0) / 2.0); // with range [0, 1]

			gl_Position = projectionMatrix * modelViewMatrix * aVertex;

			vTextureCoordinate = textureCoordinateInAtlas(vertexIndex);
		}
	)SHADER";

const char* MeshRenderer::programFragmentShader_ =
	R"SHADER(
		in OCEAN_HIGHP vec2 vTrianglesStateTextureCoordinate;

		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		uniform sampler2D trianglesStateTexture;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			OCEAN_LOWP float triangleState = texture(trianglesStateTexture, vTrianglesStateTextureCoordinate).r;

			if (triangleState >= 0.9)
			{
				fragColor = texture(primaryTexture, vTextureCoordinate).rgba;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

MeshRenderer::~MeshRenderer()
{
	release();
}

bool MeshRenderer::initialize(const Rendering::Engine& engine)
{
	if (shaderProgram_.isNull())
	{
		shaderProgram_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgram_);

		Rendering::ShaderProgram::CodePairs codePairs =
		{
			Rendering::ShaderProgram::CodePair({partPlatform_, partTextureAtlas_, programVertexShader_}, Rendering::ShaderProgram::ST_VERTEX),
			Rendering::ShaderProgram::CodePair({partPlatform_, programFragmentShader_}, Rendering::ShaderProgram::ST_FRAGMENT)
		};

		std::string errorMessage;
		if (!shaderProgram_->setShader(Rendering::ShaderProgram::SL_GLSL, codePairs, errorMessage))
		{
			release();
			return false;
		}

		textureFramebuffer_ = engine.factory().createTextureFramebuffer();

		if (!textureFramebuffer_->setPixelFormat(FrameType::FORMAT_RGBA32))
		{
			release();
			return false;
		}
	}

	return true;
}

bool MeshRenderer::render(const Rendering::Engine& /*engine*/, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const TextureFramebufferMap& textureFramebufferMap, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const TextureAtlas& textureAtlas, Frame* renderedFrame)
{
	if (numberTriangles == 0u)
	{
		return true;
	}

	ocean_assert(shaderProgram_);
	ocean_assert(textureFramebuffer_);
	ocean_assert(anyCamera.isValid());

	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesTextureFramebuffer.resize(anyCamera.width(), anyCamera.height()) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, anyCamera.width(), anyCamera.height());
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	const HomogenousMatrix4 camera_T_world(world_T_camera.inverted());

	constexpr Scalar nearDistance = Scalar(0.01);
	constexpr Scalar farDistance = Scalar(1000);
	const SquareMatrix4 projectionMatrix = SquareMatrix4::projectionMatrix(anyCamera, nearDistance, farDistance);

	glesShaderProgram.bind(projectionMatrix, camera_T_world, camera_T_world, SquareMatrix3(false));

	const GLint textureAtlasDimensionPixelsLocation = glGetUniformLocation(glesShaderProgram.id(), "textureAtlasDimensionPixels");
	ocean_assert(textureAtlasDimensionPixelsLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(textureAtlasDimensionPixelsLocation, textureAtlas.textureSizePixels());

	const GLint textureAtlasTrianglePixelsLocation = glGetUniformLocation(glesShaderProgram.id(), "textureAtlasTrianglePixels");
	ocean_assert(textureAtlasTrianglePixelsLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(textureAtlasTrianglePixelsLocation, textureAtlas.triangleSizePixels());

	Vectors2 innerBorderTextureCoordinatesEven(3);
	textureAtlas.triangleAtlasTextureCoordinates<true>(0u, innerBorderTextureCoordinatesEven.data());

	Vectors2 innerBorderTextureCoordinatesOdd(3);
	textureAtlas.triangleAtlasTextureCoordinates<true>(1u, innerBorderTextureCoordinatesOdd.data());

	const GLint innerBorderTextureCoordinatesEvenLocation = glGetUniformLocation(glesShaderProgram.id(), "innerBorderTextureCoordinatesEven");
	ocean_assert(innerBorderTextureCoordinatesEvenLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(innerBorderTextureCoordinatesEvenLocation, innerBorderTextureCoordinatesEven);

	const GLint innerBorderTextureCoordinatesOddLocation = glGetUniformLocation(glesShaderProgram.id(), "innerBorderTextureCoordinatesOdd");
	ocean_assert(innerBorderTextureCoordinatesOddLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(innerBorderTextureCoordinatesOddLocation, innerBorderTextureCoordinatesOdd);

	vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());


	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTriangleStateTextureFramebuffer = trianglesStateFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const GLuint trianglesStateTextureId = glesTriangleStateTextureFramebuffer.colorTextureId();

	if (trianglesStateTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, trianglesStateTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationTrianglesStateTexture = glGetUniformLocation(glesShaderProgram.id(), "trianglesStateTexture");
	ocean_assert(locationTrianglesStateTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationTrianglesStateTexture, 0);


	const GLint triangleStateTextureWidthLocation = glGetUniformLocation(glesShaderProgram.id(), "uTriangleStateTextureWidth");
	ocean_assert(triangleStateTextureWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(triangleStateTextureWidthLocation, glesTriangleStateTextureFramebuffer.width());

	const GLint triangleStateTextureHeightLocation = glGetUniformLocation(glesShaderProgram.id(), "uTriangleStateTextureHeight");
	ocean_assert(triangleStateTextureHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(triangleStateTextureHeightLocation, glesTriangleStateTextureFramebuffer.height());


	for (TextureFramebufferMap::const_iterator iTextureFramebuffer = textureFramebufferMap.cbegin(); iTextureFramebuffer != textureFramebufferMap.cend(); ++iTextureFramebuffer)
	{
		const Index32& meshId = iTextureFramebuffer->first;

		const unsigned int firstTriangleIdInMesh = meshId * textureAtlas.numberTriangles();
		const unsigned int trianglesInMesh = std::min(numberTriangles - meshId * textureAtlas.numberTriangles(), textureAtlas.numberTriangles());

		iTextureFramebuffer->second.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().bindTexture(glesShaderProgram, 1u);
		triangles.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles(firstTriangleIdInMesh, trianglesInMesh);
	}

	glesTextureFramebuffer.unbindFramebuffer();

	if (renderedFrame != nullptr)
	{
		glesTextureFramebuffer.copyColorTextureToFrame(*renderedFrame);
	}

	return true;
}

void MeshRenderer::release()
{
	textureFramebuffer_.release();
	shaderProgram_.release();
}

inline bool MeshRenderer::isValid() const
{
	return bool(shaderProgram_);
}

}

}

}
