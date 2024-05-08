/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/VisibleTrianglesRenderer.h"

#include "ocean/rendering/glescenegraph/GLESPoints.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"
#include "ocean/rendering/glescenegraph/GLESTextureFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

#ifdef OCEAN_RENDERING_GLES_USE_ES
	const char* VisibleTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* VisibleTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* VisibleTrianglesRenderer::programVertexShaderVisibleTriangles_ =
	R"SHADER(
		// Vertex attribute
		in OCEAN_HIGHP uint aId;

		uniform uint uFramebufferWidth;
		uniform uint uFramebufferHeight;

		void main(void)
		{
			OCEAN_HIGHP uint xId = aId % uFramebufferWidth;
			OCEAN_HIGHP uint yId = aId / uFramebufferWidth;

			OCEAN_HIGHP float xOutputPosition = (float(xId * 2u) + 0.5) / float(uFramebufferWidth) - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = (float(yId * 2u) + 0.5) / float(uFramebufferHeight) - 1.0;

			gl_Position = vec4(xOutputPosition, yOutputPosition, 0.0, 1.0);
			gl_PointSize = 1.0;
		}
	)SHADER";

const char* VisibleTrianglesRenderer::programVertexShaderOccludedTriangles_ =
	R"SHADER(
		// Vertex attribute
		in OCEAN_HIGHP uint aId;

		uniform uint uInputWidth;
		uniform uint uInputHeight;

		uniform uint uFramebufferWidth;
		uniform uint uFramebufferHeight;

		out OCEAN_HIGHP vec2 inputPosition;

		void main(void)
		{
			OCEAN_HIGHP uint xId = aId % uFramebufferWidth;
			OCEAN_HIGHP uint yId = aId / uFramebufferWidth;

			OCEAN_HIGHP uint xCoordinate = uint(gl_VertexID) % uInputWidth;
			OCEAN_HIGHP uint yCoordinate = uint(gl_VertexID) / uInputWidth;

			inputPosition.x = (float(xCoordinate) + 0.5) / float(uInputWidth); // with range [0, 1]
			inputPosition.y = (float(yCoordinate) + 0.5) / float(uInputHeight);

			OCEAN_HIGHP float xOutputPosition = (float(xId * 2u) + 0.5) / float(uFramebufferWidth) - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = (float(yId * 2u) + 0.5) / float(uFramebufferHeight) - 1.0;

			gl_Position = vec4(xOutputPosition, yOutputPosition, 0.0, 1.0);
			gl_PointSize = 1.0;
		}
	)SHADER";

const char* VisibleTrianglesRenderer::programFragmentShaderVisibleTriangles_ =
	R"SHADER(
		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			fragColor = 1.0;
		}
	)SHADER";

const char* VisibleTrianglesRenderer::programFragmentShaderOccludedTriangles_ =
	R"SHADER(
		in OCEAN_HIGHP vec2 inputPosition;

		// Texture sampler object
		uniform sampler2D originalDepthTexture;
		uniform sampler2D filteredDepthTexture;

		uniform OCEAN_HIGHP vec2 depthRange;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			const OCEAN_HIGHP float border = 0.005; // 0.5% of image resolution, due to value range [0, 1]

			if (inputPosition.x >= border && inputPosition.x <= (1.0 - border) && inputPosition.y >= border && inputPosition.y <= (1.0 - border))
			{
				OCEAN_LOWP float originalDepth = texture(originalDepthTexture, inputPosition).r * 2.0 - 1.0;
				OCEAN_LOWP float filteredDepth = texture(filteredDepthTexture, inputPosition).r * 2.0 - 1.0;

				OCEAN_LOWP float linearOriginalDepth = (2.0 * depthRange.x * depthRange.y) / (depthRange.y + depthRange.x - originalDepth * (depthRange.y - depthRange.x));
				OCEAN_LOWP float linearFilteredDepth = (2.0 * depthRange.x * depthRange.y) / (depthRange.y + depthRange.x - filteredDepth * (depthRange.y - depthRange.x));

				if (linearOriginalDepth < linearFilteredDepth + 0.5) // 0.5 meter
				{
					discard;
				}
			}

			// the triangle is partial occluded or too close to the frame border
			fragColor = 0.0;
		}
	)SHADER";

VisibleTrianglesRenderer::~VisibleTrianglesRenderer()
{
	release();
}

bool VisibleTrianglesRenderer::render(const Rendering::Engine& engine, const Rendering::TextureFramebufferRef& trianglesIdFramebuffer, const Rendering::TextureFramebufferRef& downsampledDepthFramebuffer, const Scalar nearDistance, const Scalar farDistance)
{
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTrianglesIdFrameBuffer = trianglesIdFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesTrianglesIdFrameBuffer.copyColorTextureToFrame(idFrame_) || idFrame_.pixelFormat() != FrameType::FORMAT_Y32)
	{
		return false;
	}

	const size_t numberIds = size_t(idFrame_.pixels());

	if (shaderProgramVisibleTriangles_.isNull())
	{
		shaderProgramVisibleTriangles_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgramVisibleTriangles_);

		std::vector<const char*> vertexShader = {partPlatform_, programVertexShaderVisibleTriangles_};
		std::vector<const char*> fragmentShader = {partPlatform_, programFragmentShaderVisibleTriangles_};

		std::string errorMessage;
		if (!shaderProgramVisibleTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
		{
			release();
			return false;
		}

		ocean_assert(!shaderProgramOccludedTriangles_);
		shaderProgramOccludedTriangles_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgramOccludedTriangles_);

		vertexShader = {partPlatform_, programVertexShaderOccludedTriangles_};
		fragmentShader = {partPlatform_, programFragmentShaderOccludedTriangles_};

		if (!shaderProgramOccludedTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
		{
			release();
			return false;
		}

		textureFramebuffer_ = engine.factory().createTextureFramebuffer();
		ocean_assert(textureFramebuffer_);

		if (!textureFramebuffer_->setPixelFormat(FrameType::FORMAT_Y8))
		{
			release();
			return false;
		}

		vertexSet_ = engine.factory().createVertexSet();

		points_ = engine.factory().createPoints();
		points_->setIndices((unsigned int)(numberIds));
	}

	ocean_assert(shaderProgramVisibleTriangles_ && shaderProgramOccludedTriangles_ && textureFramebuffer_);

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	constexpr unsigned int framebufferWidth = 1024u; // **TODO**
	constexpr unsigned int framebufferHeight = 1024u;

	{
		// rendering the ids of all visible triangles

		if (!glesTextureFramebuffer.resize(framebufferWidth, framebufferHeight) || !glesTextureFramebuffer.bindFramebuffer())
		{
			return false;
		}

		Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramVisibleTriangles = shaderProgramVisibleTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

		ocean_assert(vertexSet_ && points_);

		vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute<uint32_t>("aId", idFrame_.data<uint32_t>(), numberIds);

		glViewport(0, 0, framebufferWidth, framebufferHeight);
		ocean_assert(GL_NO_ERROR == glGetError());

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		ocean_assert(GL_NO_ERROR == glGetError());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

		glesShaderProgramVisibleTriangles.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

		const GLint framebufferWidthLocation = glGetUniformLocation(glesShaderProgramVisibleTriangles.id(), "uFramebufferWidth");
		ocean_assert(framebufferWidthLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(framebufferWidthLocation, framebufferWidth);

		const GLint framebufferHeightLocation = glGetUniformLocation(glesShaderProgramVisibleTriangles.id(), "uFramebufferHeight");
		ocean_assert(framebufferHeightLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(framebufferHeightLocation, framebufferHeight);

		vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramVisibleTriangles.id());

		points_.force<Rendering::GLESceneGraph::GLESPoints>().drawPoints();

		glesTextureFramebuffer.unbindFramebuffer();
	}

	{
		// removing the ids of all (partial) occluded triangles

		if (!glesTextureFramebuffer.bindFramebuffer())
		{
			return false;
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

		Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramOccludedTriangles = shaderProgramOccludedTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

		const unsigned int inputWidth = glesTrianglesIdFrameBuffer.width();
		const unsigned int inputHeight = glesTrianglesIdFrameBuffer.height();

		glesShaderProgramOccludedTriangles.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

		const GLint inputWidthLocation = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "uInputWidth");
		ocean_assert(inputWidthLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(inputWidthLocation, inputWidth);

		const GLint inputHeightLocation = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "uInputHeight");
		ocean_assert(inputHeightLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(inputHeightLocation, inputHeight);

		const GLint framebufferWidthLocation = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "uFramebufferWidth");
		ocean_assert(framebufferWidthLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(framebufferWidthLocation, framebufferWidth);

		const GLint framebufferHeightLocation = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "uFramebufferHeight");
		ocean_assert(framebufferHeightLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(framebufferHeightLocation, framebufferHeight);

		const GLint depthRangeLocation = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "depthRange");
		ocean_assert(depthRangeLocation != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(depthRangeLocation, Vector2(nearDistance, farDistance));

		const GLuint originalDepthTextureId = glesTrianglesIdFrameBuffer.depthTextureId();

		if (originalDepthTextureId == 0u)
		{
			return false;
		}

		glActiveTexture(GL_TEXTURE0);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(GL_TEXTURE_2D, originalDepthTextureId);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		const GLint locationOriginalDepthTexture = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "originalDepthTexture");
		ocean_assert(locationOriginalDepthTexture != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(locationOriginalDepthTexture, 0);

		const GLuint filteredDepthTextureId = downsampledDepthFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().depthTextureId();

		if (filteredDepthTextureId == 0u)
		{
			return false;
		}

		glActiveTexture(GL_TEXTURE1);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(GL_TEXTURE_2D, filteredDepthTextureId);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		const GLint locationFilteredDepthTexture = glGetUniformLocation(glesShaderProgramOccludedTriangles.id(), "filteredDepthTexture");
		ocean_assert(locationFilteredDepthTexture != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(locationFilteredDepthTexture, 1);

		vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramOccludedTriangles.id());

		points_.force<Rendering::GLESceneGraph::GLESPoints>().drawPoints();

		glesTextureFramebuffer.unbindFramebuffer();
	}

	return true;
}

void VisibleTrianglesRenderer::release()
{
	points_.release();
	vertexSet_.release();
	textureFramebuffer_.release();
	shaderProgramVisibleTriangles_.release();
	shaderProgramOccludedTriangles_.release();
	idFrame_.release();
}

inline bool VisibleTrianglesRenderer::isValid() const
{
	return bool(shaderProgramVisibleTriangles_);
}

}

}

}
