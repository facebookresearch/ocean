/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/DepthBufferRenderer.h"

#include "ocean/cv/FrameConverterColorMap.h"

#include "ocean/media/Manager.h"

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
	const char* DepthBufferRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* DepthBufferRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* DepthBufferRenderer::programVertexShader_ =
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

const char* DepthBufferRenderer::programFragmentShaderDownsampling_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_HIGHP float fragColor;

		void main()
		{
			ivec2 primaryTextureSize = textureSize(primaryTexture, 0);

			ivec2 coordinate = ivec2(vTextureCoordinate * vec2(primaryTextureSize));

			ivec2 coordinateOffset;
			coordinateOffset.x = min(coordinate.x + 1, primaryTextureSize.x - 1);
			coordinateOffset.y = min(coordinate.y + 1, primaryTextureSize.y - 1);

			OCEAN_HIGHP vec4 value;
			value.x = texelFetch(primaryTexture, coordinate, 0).r;
			value.y = texelFetch(primaryTexture, ivec2(coordinateOffset.x, coordinate.y), 0).r;
			value.z = texelFetch(primaryTexture, ivec2(coordinate.x, coordinateOffset.y), 0).r;
			value.w = texelFetch(primaryTexture, coordinateOffset, 0).r;

			fragColor = min(min(value.x, value.y), min(value.z, value.w));

			gl_FragDepth = fragColor;
		}
	)SHADER";

const char* DepthBufferRenderer::programFragmentShaderMinFiltering_ =
	R"SHADER(
		// Input texture coordinate
		in OCEAN_LOWP vec2 vTextureCoordinate;

		// Texture sampler object
		uniform sampler2D primaryTexture;

		// The out fragment color
		out OCEAN_HIGHP float fragColor;

		void main()
		{
			ivec2 primaryTextureSize = textureSize(primaryTexture, 0);

			ivec2 coordinate = ivec2(vTextureCoordinate * vec2(primaryTextureSize));

			OCEAN_HIGHP float colorValue = texelFetch(primaryTexture, coordinate, 0).r;

			ivec2 offsetCoordinate;

			for (int y = -1; y <= 1; ++y)
			{
				offsetCoordinate.y = clamp(coordinate.y + y, 0, primaryTextureSize.y - 1);

				for (int x = -1; x <= 1; ++x)
				{
					offsetCoordinate.x = clamp(coordinate.x + x, 0, primaryTextureSize.x - 1);

					colorValue = min(colorValue, texelFetch(primaryTexture, offsetCoordinate, 0).r);
				}
			}

			fragColor = colorValue;

			gl_FragDepth = fragColor;
		}
	)SHADER";

DepthBufferRenderer::~DepthBufferRenderer()
{
	release();
}

bool DepthBufferRenderer::downsampleAndFilterDepthBuffer(const Rendering::Engine& engine, const Rendering::TextureFramebufferRef& sourceFramebuffer, const unsigned int downsampleIterations)
{
	if (shaderProgramDownsampling_.isNull())
	{
		shaderProgramDownsampling_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgramDownsampling_);

		const std::vector<const char*> vertexShader = {partPlatform_, programVertexShader_};

		std::string errorMessage;
		if (!shaderProgramDownsampling_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, {partPlatform_, programFragmentShaderDownsampling_}, errorMessage))
		{
			release();
			return false;
		}

		shaderProgramMinFiltering_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgramMinFiltering_);

		if (!shaderProgramMinFiltering_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, {partPlatform_, programFragmentShaderMinFiltering_}, errorMessage))
		{
			release();
			return false;
		}

		vertexSet_ = engine.factory().createVertexSet();

		const Vectors3 vertices =
		{
			Vector3(-1, 1, 0),
			Vector3(-1, -1, 0),
			Vector3(1, -1, 0),
			Vector3(1, 1, 0)
		};

		const Vectors2 textureCoordinates =
		{
			Vector2(0, 1),
			Vector2(0, 0),
			Vector2(1, 0),
			Vector2(1, 1)
		};

		vertexSet_->setVertices(vertices);
		vertexSet_->setTextureCoordinates(textureCoordinates, 0u);

		triangles_ = engine.factory().createTriangles();

		const Rendering::TriangleFaces triangleFaces =
		{
			Rendering::TriangleFace(0u, 1u, 2u),
			Rendering::TriangleFace(0u, 2u, 3u)
		};

		triangles_->setFaces(triangleFaces);
		triangles_->setVertexSet(vertexSet_);
	}

	Rendering::TextureFramebufferRef previousFramebuffer = sourceFramebuffer;

	for (unsigned int nIteration = 0u; nIteration < downsampleIterations; ++nIteration)
	{
		if (textureFramebuffers_.size() <= size_t(nIteration))
		{
			textureFramebuffers_.emplace_back(engine.factory().createTextureFramebuffer());
		}

		ocean_assert(nIteration < textureFramebuffers_.size());

		downsampleFramebuffer(previousFramebuffer, textureFramebuffers_[nIteration]);
		previousFramebuffer = textureFramebuffers_[nIteration];
	}

	if (textureFramebuffers_.size() <= size_t(downsampleIterations))
	{
		textureFramebuffers_.emplace_back(engine.factory().createTextureFramebuffer());
	}

	minFilterFramebuffer(previousFramebuffer, textureFramebuffers_[textureFramebuffers_.size() - 1]);

	return true;
}

bool DepthBufferRenderer::downsampleFramebuffer(const Rendering::TextureFramebufferRef& sourceFramebuffer, Rendering::TextureFramebufferRef& targetFramebuffer)
{
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesSourceTextureFramebuffer = sourceFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTargetTextureFramebuffer = targetFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	ocean_assert(shaderProgramDownsampling_);
	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgramDownsampling_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	const unsigned int sourceWidth = glesSourceTextureFramebuffer.width();
	const unsigned int sourceHeight = glesSourceTextureFramebuffer.height();

	unsigned int targetWidth = sourceWidth / 2u;
	unsigned int targetHeight = sourceHeight / 2u;

	glesTargetTextureFramebuffer.setPixelFormat(FrameType::FORMAT_Y8); // we are only interested in the depth buffer only

	if (!glesTargetTextureFramebuffer.resize(targetWidth, targetHeight) || !glesTargetTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, targetWidth, targetHeight);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	glesShaderProgram.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));
	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	const GLuint sourceTextureId = glesSourceTextureFramebuffer.depthTextureId();

	if (sourceTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, sourceTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationTexture = glGetUniformLocation(glesShaderProgram.id(), "primaryTexture");
	ocean_assert(locationTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationTexture, 0);

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTargetTextureFramebuffer.unbindFramebuffer();

	return true;
}

bool DepthBufferRenderer::minFilterFramebuffer(const Rendering::TextureFramebufferRef& sourceFramebuffer, Rendering::TextureFramebufferRef& targetFramebuffer)
{
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesSourceTextureFramebuffer = sourceFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTargetTextureFramebuffer = targetFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	ocean_assert(shaderProgramMinFiltering_);
	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgramMinFiltering_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	const unsigned int width = glesSourceTextureFramebuffer.width();
	const unsigned int height = glesSourceTextureFramebuffer.height();

	glesTargetTextureFramebuffer.setPixelFormat(FrameType::FORMAT_Y8); // we are only interested in the depth buffer only

	if (!glesTargetTextureFramebuffer.resize(width, height) || !glesTargetTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, width, height);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	glesShaderProgram.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));
	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgram.id());

	const GLuint sourceTextureId = glesSourceTextureFramebuffer.depthTextureId();

	if (sourceTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, sourceTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationTexture = glGetUniformLocation(glesShaderProgram.id(), "primaryTexture");
	ocean_assert(locationTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationTexture, 0);

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTargetTextureFramebuffer.unbindFramebuffer();

	return true;
}

void DepthBufferRenderer::release()
{
	triangles_.release();
	vertexSet_.release();
	textureFramebuffers_.clear();
	shaderProgramDownsampling_.release();
	shaderProgramMinFiltering_.release();
}

inline bool DepthBufferRenderer::isValid() const
{
	return bool(shaderProgramDownsampling_);
}

}

}

}
