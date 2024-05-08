/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/RetiredTrianglesRenderer.h"

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
	const char* RetiredTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* RetiredTrianglesRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* RetiredTrianglesRenderer::programVertexShaderRetiredTriangles_ =
	R"SHADER(
		// Model view matrix
		uniform mat4 modelViewMatrix;

		uniform uint framebufferWidth;
		uniform uint framebufferHeight;

		// Vertex attribute
		in vec4 aVertex;

		// The resulting color id.
		flat out OCEAN_HIGHP uint vTriangleId;

		out OCEAN_LOWP vec4 vVertexInView;
		out OCEAN_HIGHP vec2 triangleStateTextureInputPosition;

		void main(void)
		{
			vVertexInView = modelViewMatrix * aVertex;

			vTriangleId = uint(gl_VertexID) / 3u;

			OCEAN_HIGHP uint xId = vTriangleId % framebufferWidth;
			OCEAN_HIGHP uint yId = vTriangleId / framebufferWidth;

			triangleStateTextureInputPosition.x = (float(xId) + 0.5) / float(framebufferWidth); // with range [0, 1]
			triangleStateTextureInputPosition.y = (float(yId) + 0.5) / float(framebufferHeight);

			OCEAN_HIGHP float xOutputPosition = triangleStateTextureInputPosition.x * 2.0 - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = triangleStateTextureInputPosition.y * 2.0 - 1.0;

			OCEAN_HIGHP uint vertexIndex = uint(gl_VertexID) % 3u;

			if (vertexIndex == 0u)
			{
				gl_Position = vec4(xOutputPosition - 1.0 / float(framebufferWidth), yOutputPosition, 0.0, 1.0);
			}
			else if (vertexIndex == 1u)
			{
				gl_Position = vec4(xOutputPosition + 1.0 / float(framebufferWidth), yOutputPosition, 0.0, 1.0);
			}
			else
			{
				gl_Position = vec4(xOutputPosition, yOutputPosition + 1.0 / float(framebufferHeight), 0.0, 1.0);
			}
		}
	)SHADER";

const char* RetiredTrianglesRenderer::programFragmentShaderRetiredTriangles_ =
	R"SHADER(
		// Projection matrix
		uniform OCEAN_LOWP mat4 projectionMatrix;

		// Texture sampler object
		uniform sampler2D trianglesStateTexture;
		uniform sampler2D filteredDepthTexture;

		uniform OCEAN_HIGHP vec2 depthRange;

		in OCEAN_LOWP vec4 vVertexInView;
		in OCEAN_HIGHP vec2 triangleStateTextureInputPosition;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			OCEAN_LOWP float triangleState = texture(trianglesStateTexture, triangleStateTextureInputPosition).r;

			if (triangleState >= 31.0 / 255.0 && triangleState <= 33.0 / 255.0)
			{
				// the triangle is retired and not textured, we use an own color to simplify debugging
				fragColor = 0.5;
			}
			else
			{
				bool triangleMustNotBeDeleted = false;

				if (triangleState >= 62.0 / 255.0 && triangleState <= 66.0 / 255.0)
				{
					// the triangle is a textured but retired triangle

					OCEAN_LOWP vec4 vertexInClip = projectionMatrix * vVertexInView;
					OCEAN_LOWP vec3 normalizedVertexInClip = vertexInClip.xyz / vertexInClip.w;

					OCEAN_LOWP vec2 normalizedVertexInTexture = (normalizedVertexInClip.xy + 1.0) / 2.0;

					const OCEAN_LOWP float border = 0.05;

					if (normalizedVertexInTexture.x <= border || normalizedVertexInTexture.y <= border || normalizedVertexInTexture.x >= (1.0 - border) || normalizedVertexInTexture.y >= (1.0 - border) || normalizedVertexInClip.z >= 1.0 || normalizedVertexInClip.z <= -1.0)
					{
						// the fragment lies outside of the camera, so we cannot delete the triangle

						triangleMustNotBeDeleted = true;
					}
					else
					{
						OCEAN_LOWP float linearOriginalDepth = -vVertexInView.z / vVertexInView.w; // space in front of view is negative, depth is positive

						OCEAN_LOWP float filteredDepth = texture(filteredDepthTexture, normalizedVertexInTexture).r * 2.0 - 1.0;
						OCEAN_LOWP float linearFilteredDepth = (2.0 * depthRange.x * depthRange.y) / (depthRange.y + depthRange.x - filteredDepth * (depthRange.y - depthRange.x));

						if (linearOriginalDepth > linearFilteredDepth + 0.5) // 0.5 meter
						{
							// the fragment may be occluded, so we cannot delete the triangle

							triangleMustNotBeDeleted = true;
						}
					}
				}
				else
				{
					// triangle is not of interests, so we keep it
					triangleMustNotBeDeleted = true;
				}

				if (triangleMustNotBeDeleted)
				{
					fragColor = 1.0;
				}
				else
				{
					discard;
				}
			}
		}
	)SHADER";

RetiredTrianglesRenderer::~RetiredTrianglesRenderer()
{
	release();
}

bool RetiredTrianglesRenderer::render(const Rendering::Engine& engine, const Rendering::VertexSetRef& vertexSet, const Rendering::TrianglesRef& triangles, const unsigned int numberTriangles, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& world_T_camera, const Rendering::TextureFramebufferRef& trianglesStateFramebuffer, const Rendering::TextureFramebufferRef& downsampledDepthFramebuffer, const Scalar nearDistance, const Scalar farDistance, Indices32& deletedTriangleIds)
{
	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTrianglesStateFrameBuffer = trianglesStateFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (shaderProgramRetiredTriangles_.isNull())
	{
		shaderProgramRetiredTriangles_ = engine.factory().createShaderProgram();
		ocean_assert(shaderProgramRetiredTriangles_);

		std::vector<const char*> vertexShader = {partPlatform_, programVertexShaderRetiredTriangles_};
		std::vector<const char*> fragmentShader = {partPlatform_, programFragmentShaderRetiredTriangles_};

		std::string errorMessage;
		if (!shaderProgramRetiredTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
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
	}

	ocean_assert(shaderProgramRetiredTriangles_ && textureFramebuffer_);

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const unsigned int framebufferWidth = glesTrianglesStateFrameBuffer.width();
	const unsigned int framebufferHeight = glesTrianglesStateFrameBuffer.height();

	if (!glesTextureFramebuffer.resize(framebufferWidth, framebufferHeight) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramRetiredTriangles = shaderProgramRetiredTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	glViewport(0, 0, framebufferWidth, framebufferHeight);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	const HomogenousMatrix4 camera_T_world = world_T_camera.inverted();

	glesShaderProgramRetiredTriangles.bind(projectionMatrix, camera_T_world, camera_T_world, SquareMatrix3(false));

	const GLint framebufferWidthLocation = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "framebufferWidth");
	ocean_assert(framebufferWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferWidthLocation, framebufferWidth);

	const GLint framebufferHeightLocation = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "framebufferHeight");
	ocean_assert(framebufferHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferHeightLocation, framebufferHeight);

	const GLint depthRangeLocation = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "depthRange");
	ocean_assert(depthRangeLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(depthRangeLocation, Vector2(nearDistance, farDistance));


	const GLuint trianglesStateTextureId = trianglesStateFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().colorTextureId();

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

	const GLint locationTrianglesStateTexture = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "trianglesStateTexture");
	ocean_assert(locationTrianglesStateTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationTrianglesStateTexture, 0);


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

	const GLint locationFilteredDepthTexture = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "filteredDepthTexture");
	ocean_assert(locationFilteredDepthTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationFilteredDepthTexture, 1);


	vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramRetiredTriangles.id());

	triangles.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTextureFramebuffer.unbindFramebuffer();

	deletedTriangleIds.clear();

	const unsigned int frameHeight = (numberTriangles + framebufferWidth - 1u) / framebufferWidth;

	if (!glesTextureFramebuffer.copyColorTextureToFrame(stateFrame_, CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), framebufferWidth, frameHeight)))
	{
		return false;
	}

	ocean_assert(stateFrame_.isContinuous());
	const uint8_t* trianglesState = stateFrame_.constdata<uint8_t>();

	for (unsigned int triangleId = 0u; triangleId < numberTriangles; ++triangleId)
	{
		if (trianglesState[triangleId] != 255u)
		{
			deletedTriangleIds.emplace_back(triangleId);
		}
	}

	return true;
}

void RetiredTrianglesRenderer::release()
{
	textureFramebuffer_.release();
	shaderProgramRetiredTriangles_.release();
	stateFrame_.release();
}

inline bool RetiredTrianglesRenderer::isValid() const
{
	return bool(shaderProgramRetiredTriangles_);
}

}

}

}
