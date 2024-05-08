/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TrianglesIdRenderer.h"

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
	const char* TrianglesIdRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* TrianglesIdRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* TrianglesIdRenderer::programVertexShaderColorId_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		uniform sampler2D trianglesStateTexture;

		uniform uint trianglesStateTextureWidth;
		uniform uint trianglesStateTextureHeight;

		// Vertex attribute
		in vec4 aVertex;

		// The resulting color id.
		flat out OCEAN_HIGHP uint vTriangleId;
		out OCEAN_LOWP float vTriangleState;

		void main(void)
		{
			gl_Position = projectionMatrix * modelViewMatrix * aVertex;

			vTriangleId = uint(gl_VertexID) / 3u;

			OCEAN_HIGHP uint xId = vTriangleId % trianglesStateTextureWidth;
			OCEAN_HIGHP uint yId = vTriangleId / trianglesStateTextureWidth;

			vec2 textureCoordinate;
			textureCoordinate.x = (float(xId) + 0.5) / float(trianglesStateTextureWidth); // with range [0, 1]
			textureCoordinate.y = (float(yId) + 0.5) / float(trianglesStateTextureHeight);

			vTriangleState = texture(trianglesStateTexture, textureCoordinate).r;
		}
	)SHADER";

const char* TrianglesIdRenderer::programFragmentShaderColorId_ =
	R"SHADER(
		uniform OCEAN_LOWP float uMinimalTriangleState;

		// The color id for the fragment
		flat in OCEAN_HIGHP uint vTriangleId;

		in OCEAN_LOWP float vTriangleState;

		// The out fragment color
		out OCEAN_HIGHP uint fragColor;

		void main()
		{
			if (vTriangleState >= uMinimalTriangleState)
			{
				fragColor = vTriangleId;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

TrianglesIdRenderer::~TrianglesIdRenderer()
{
	release();
}

bool TrianglesIdRenderer::initialize(const Rendering::Engine& engine)
{
	if (shaderProgram_)
	{
		return true;
	}

	shaderProgram_ = engine.factory().createShaderProgram();
	ocean_assert(shaderProgram_);

	const std::vector<const char*> vertexShader = {partPlatform_, programVertexShaderColorId_};
	const std::vector<const char*> fragmentShader = {partPlatform_, programFragmentShaderColorId_};

	std::string errorMessage;
	if (!shaderProgram_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
	{
		release();
		return false;
	}

	textureFramebuffer_ = engine.factory().createTextureFramebuffer();
	ocean_assert(textureFramebuffer_);

	if (!textureFramebuffer_->setPixelFormat(FrameType::FORMAT_Y32))
	{
		release();
		return false;
	}

	return true;
}

bool TrianglesIdRenderer::render(const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const unsigned int width, const unsigned int height, const uint8_t minimalTriangleState, Frame* idFrame, Frame* depthFrame)
{
	ocean_assert(vertexSet && triangles);
	ocean_assert(numberTriangles >= 1u);
	ocean_assert(trianglesStateFramebuffer);

	if (!isValid())
	{
		ocean_assert(false && "Not initialized!");
		return false;
	}

	ocean_assert(shaderProgram_ && textureFramebuffer_);

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

	if (!glesTextureFramebuffer.resize(width, height) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, width, height);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());


	glesShaderProgram.bind(projectionMatrix, camera_T_world, camera_T_world, SquareMatrix3(false));

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTrianglesStateFramebuffer = trianglesStateFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const GLint trianglesStateTextureWidthLocation = glGetUniformLocation(glesShaderProgram.id(), "trianglesStateTextureWidth");
	ocean_assert(trianglesStateTextureWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(trianglesStateTextureWidthLocation, glesTrianglesStateFramebuffer.width());

	const GLint trianglesStateHeightLocation = glGetUniformLocation(glesShaderProgram.id(), "trianglesStateTextureHeight");
	ocean_assert(trianglesStateHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(trianglesStateHeightLocation, glesTrianglesStateFramebuffer.height());

	const GLint minimalTriangleStateLocation = glGetUniformLocation(glesShaderProgram.id(), "uMinimalTriangleState");
	ocean_assert(minimalTriangleStateLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(minimalTriangleStateLocation, float(minimalTriangleState) / 255.0f);

	const GLuint trianglesStateTextureTextureId = glesTrianglesStateFramebuffer.colorTextureId();

	if (trianglesStateTextureTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, trianglesStateTextureTextureId);
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

	vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	triangles.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles(0u, numberTriangles);

	glesTextureFramebuffer.unbindFramebuffer();

	if (idFrame != nullptr)
	{
		if (!glesTextureFramebuffer.copyColorTextureToFrame(*idFrame))
		{
			return false;
		}
	}

	if (depthFrame != nullptr)
	{
		if (!glesTextureFramebuffer.copyDepthTextureToFrame(*depthFrame))
		{
			return false;
		}
	}

	return true;
}

void TrianglesIdRenderer::release()
{
	textureFramebuffer_.release();
	shaderProgram_.release();
}

inline bool TrianglesIdRenderer::isValid() const
{
	return bool(shaderProgram_);
}

}

}

}
