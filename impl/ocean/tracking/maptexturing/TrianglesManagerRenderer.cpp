/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TrianglesManagerRenderer.h"

#include "ocean/rendering/glescenegraph/GLESPoints.h"
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
	const char* TrianglesManagerRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 300 es

			#define OCEAN_LOWP lowp
			#define OCEAN_HIGHP highp
		)SHADER";
#else
	const char* TrianglesManagerRenderer::partPlatform_ =
		// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
		R"SHADER(#version 330

			#define OCEAN_LOWP // does not exist
			#define OCEAN_HIGHP
		)SHADER";
#endif

const char* TrianglesManagerRenderer::programVertexShaderSetTriangles_ =
	R"SHADER(
		// Vertex attribute
		in OCEAN_HIGHP uint aTriangleId;

		uniform uint uFramebufferWidth;
		uniform uint uFramebufferHeight;

		void main(void)
		{
			OCEAN_HIGHP uint xId = aTriangleId % uFramebufferWidth;
			OCEAN_HIGHP uint yId = aTriangleId / uFramebufferWidth;

			OCEAN_HIGHP float xOutputPosition = (float(xId * 2u) + 0.5) / float(uFramebufferWidth) - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = (float(yId * 2u) + 0.5) / float(uFramebufferHeight) - 1.0;

			gl_Position = vec4(xOutputPosition, yOutputPosition, 0.0, 1.0);
			gl_PointSize = 1.0;
		}
	)SHADER";

const char* TrianglesManagerRenderer::programVertexShaderPlanarTexturedTriangles_ =
	R"SHADER(
		// Vertex attribute
		in OCEAN_HIGHP vec4 aVertex;
		in OCEAN_HIGHP vec2 aTextureCoordinate;

		out OCEAN_HIGHP vec2 vTextureCoordinate;

		void main(void)
		{
			gl_Position = aVertex;

			vTextureCoordinate = aTextureCoordinate;
		}
	)SHADER";

const char* TrianglesManagerRenderer::programVertexShaderRetiredTriangles_ =
	R"SHADER(
		// Vertex attribute
		in OCEAN_HIGHP uint aTriangleId;

		uniform uint uFramebufferWidth;
		uniform uint uFramebufferHeight;

		out OCEAN_HIGHP vec2 vTrianglesStateTextureCoordinate;

		void main(void)
		{
			OCEAN_HIGHP uint xId = aTriangleId % uFramebufferWidth;
			OCEAN_HIGHP uint yId = aTriangleId / uFramebufferWidth;

			OCEAN_HIGHP float xOutputPosition = (float(xId * 2u) + 0.5) / float(uFramebufferWidth) - 1.0; // with range [-1, 1]
			OCEAN_HIGHP float yOutputPosition = (float(yId * 2u) + 0.5) / float(uFramebufferHeight) - 1.0;

			vTrianglesStateTextureCoordinate = vec2((xOutputPosition + 1.0) / 2.0, (yOutputPosition + 1.0) / 2.0); // with range [0, 1]

			gl_Position = vec4(xOutputPosition, yOutputPosition, 0.0, 1.0);
			gl_PointSize = 1.0;
		}
	)SHADER";

const char* TrianglesManagerRenderer::programFragmentShaderSetTriangles_ =
	R"SHADER(
		uniform OCEAN_LOWP float uTriangleColor;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			fragColor = uTriangleColor;
		}
	)SHADER";

const char* TrianglesManagerRenderer::programFragmentShaderActiveTriangles_ =
	R"SHADER(
		// The out fragment color
		uniform sampler2D visibilityTexture;

		in OCEAN_HIGHP vec2 vTextureCoordinate;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			OCEAN_LOWP float visibility = texture(visibilityTexture, vTextureCoordinate).r;

			if (visibility >= 0.9)
			{
				fragColor = 1.0;
			}
			else
			{
				discard;
			}
		}
	)SHADER";

const char* TrianglesManagerRenderer::programFragmentShaderRetiredTriangles_ =
	R"SHADER(
		uniform sampler2D trianglesStateTexture;

		in OCEAN_HIGHP vec2 vTrianglesStateTextureCoordinate;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			OCEAN_LOWP float triangleState = texture(trianglesStateTexture, vTrianglesStateTextureCoordinate).r;

			if (triangleState >= 127.0 / 255.0 && triangleState <= 129.0 / 255.0)
			{
				// the triangle has not been textured, so we can remove it immediately
				fragColor = 32.0 / 255.0;
			}
			else
			{
				// the triangle is retired
				fragColor = 64.0 / 255.0;
			}
		}
	)SHADER";

const char* TrianglesManagerRenderer::programFragmentShaderCopyFramebuffer_ =
	R"SHADER(
		// The out fragment color
		uniform sampler2D framebufferTexture;

		in OCEAN_HIGHP vec2 vTextureCoordinate;

		// The out fragment color
		out OCEAN_LOWP float fragColor;

		void main()
		{
			fragColor = texture(framebufferTexture, vTextureCoordinate).r;
		}
	)SHADER";

TrianglesManagerRenderer::~TrianglesManagerRenderer()
{
	release();
}

bool TrianglesManagerRenderer::initialize(const Rendering::Engine& engine, unsigned int framebufferWidth, unsigned int framebufferHeight)
{
	if (shaderProgramSetTriangles_)
	{
		return true;
	}

	ocean_assert(framebufferWidth_ == 0u && framebufferHeight_ == 0u);

	framebufferWidth_ = framebufferWidth;
	framebufferHeight_ = framebufferHeight;

	shaderProgramSetTriangles_ = engine.factory().createShaderProgram();
	ocean_assert(shaderProgramSetTriangles_);

	std::vector<const char*> vertexShader = {partPlatform_, programVertexShaderSetTriangles_};
	std::vector<const char*> fragmentShader = {partPlatform_, programFragmentShaderSetTriangles_};

	std::string errorMessage;
	if (!shaderProgramSetTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
	{
		release();
		return false;
	}

	ocean_assert(!shaderProgramTexturedTriangles_);
	shaderProgramTexturedTriangles_ = engine.factory().createShaderProgram();
	ocean_assert(shaderProgramTexturedTriangles_);

	vertexShader = {partPlatform_, programVertexShaderPlanarTexturedTriangles_};
	fragmentShader = {partPlatform_, programFragmentShaderActiveTriangles_};

	if (!shaderProgramTexturedTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
	{
		release();
		return false;
	}

	ocean_assert(!shaderProgramRetiredTriangles_);
	shaderProgramRetiredTriangles_ = engine.factory().createShaderProgram();
	ocean_assert(shaderProgramRetiredTriangles_);

	vertexShader = {partPlatform_, programVertexShaderRetiredTriangles_};
	fragmentShader = {partPlatform_, programFragmentShaderRetiredTriangles_};

	if (!shaderProgramRetiredTriangles_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
	{
		release();
		return false;
	}

	ocean_assert(!shaderProgramCopyFramebuffer_);
	shaderProgramCopyFramebuffer_ = engine.factory().createShaderProgram();
	ocean_assert(shaderProgramCopyFramebuffer_);

	vertexShader = {partPlatform_, programVertexShaderPlanarTexturedTriangles_};
	fragmentShader = {partPlatform_, programFragmentShaderCopyFramebuffer_};

	if (!shaderProgramCopyFramebuffer_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, fragmentShader, errorMessage))
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

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesTextureFramebuffer.resize(framebufferWidth_, framebufferHeight_) || !glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, framebufferWidth_, framebufferHeight_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	glesTextureFramebuffer.unbindFramebuffer();

	copyTextureFramebuffer_ = engine.factory().createTextureFramebuffer();
	ocean_assert(copyTextureFramebuffer_);

	if (!copyTextureFramebuffer_->setPixelFormat(FrameType::FORMAT_Y8))
	{
		release();
		return false;
	}

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesCopyTextureFramebuffer = copyTextureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesCopyTextureFramebuffer.resize(framebufferWidth_, framebufferHeight_))
	{
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

	const Rendering::TriangleFaces triangleFaces =
	{
		Rendering::TriangleFace(0u, 1u, 2u),
		Rendering::TriangleFace(0u, 2u, 3u)
	};

	triangles_ = engine.factory().createTriangles();
	triangles_->setVertexSet(vertexSet_);
	triangles_->setFaces(triangleFaces);

	points_ = engine.factory().createPoints();
	points_->setVertexSet(vertexSet_);

	ocean_assert(shaderProgramSetTriangles_ && shaderProgramTexturedTriangles_ && shaderProgramCopyFramebuffer_ && textureFramebuffer_ && copyTextureFramebuffer_);

	return true;
}

bool TrianglesManagerRenderer::updateNewTriangles(const Indices32& newTriangleIds, Frame* stateFrame)
{
	if (newTriangleIds.empty())
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Invalid renderer!");
		return false;
	}

	hasChanged_ = true;

	constexpr float newTriangleColor = 0.5f; // for not yet textured triangles

	if (!setTriangles(newTriangleIds, newTriangleColor))
	{
		return false;
	}

	if (stateFrame != nullptr)
	{
		textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(*stateFrame);
	}

	return true;
}

bool TrianglesManagerRenderer::updateTexturedTriangles(const Rendering::TextureFramebufferRef& triangleIdFramebuffer, Frame* stateFrame)
{
	if (!isValid())
	{
		ocean_assert(false && "Invalid renderer!");
		return false;
	}

	hasChanged_ = true;

	ocean_assert(shaderProgramTexturedTriangles_ && textureFramebuffer_);

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	// rendering the ids of all active triangles

	if (!glesTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramActiveTriangles = shaderProgramTexturedTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	glesShaderProgramActiveTriangles.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTriangleIdFrameBuffer = triangleIdFramebuffer.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const GLuint visibleTriangleTextureId = glesTriangleIdFrameBuffer.colorTextureId();

	if (visibleTriangleTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, visibleTriangleTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationOriginalDepthTexture = glGetUniformLocation(glesShaderProgramActiveTriangles.id(), "visibilityTexture");
	ocean_assert(locationOriginalDepthTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationOriginalDepthTexture, 0);

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramActiveTriangles.id());

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesTextureFramebuffer.unbindFramebuffer();

	if (stateFrame != nullptr)
	{
		glesTextureFramebuffer.copyColorTextureToFrame(*stateFrame);
	}

	return true;
}

bool TrianglesManagerRenderer::updateTexturedTriangles(const Indices32& texturedTriangleIds, Frame* stateFrame)
{
	if (texturedTriangleIds.empty())
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Invalid renderer!");
		return false;
	}

	hasChanged_ = true;

	constexpr float newTriangleColor = 1.0f; // for textured triangles

	if (!setTriangles(texturedTriangleIds, newTriangleColor))
	{
		return false;
	}

	if (stateFrame != nullptr)
	{
		textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(*stateFrame);
	}

	return true;
}

bool TrianglesManagerRenderer::updateRetiredTriangles(const Indices32& retiredTriangleIds, Frame* stateFrame)
{
	if (retiredTriangleIds.empty())
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Invalid renderer!");
		return false;
	}

	hasChanged_ = true;

	if (!copyFramebuffer())
	{
		return false;
	}

	if (!textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().bindFramebuffer())
	{
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramRetiredTriangles = shaderProgramRetiredTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	ocean_assert(points_);

	glViewport(0, 0, framebufferWidth_, framebufferHeight_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	glesShaderProgramRetiredTriangles.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	const GLint framebufferWidthLocation = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "uFramebufferWidth");
	ocean_assert(framebufferWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferWidthLocation, framebufferWidth_);

	const GLint framebufferHeightLocation = glGetUniformLocation(glesShaderProgramRetiredTriangles.id(), "uFramebufferHeight");
	ocean_assert(framebufferHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferHeightLocation, framebufferHeight_);

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute("aTriangleId", retiredTriangleIds.data(), retiredTriangleIds.size());

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramRetiredTriangles.id());

	points_->setIndices((unsigned int)(retiredTriangleIds.size()));
	points_.force<Rendering::GLESceneGraph::GLESPoints>().drawPoints();

	textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().unbindFramebuffer();

	if (stateFrame != nullptr)
	{
		textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(*stateFrame);
	}

	return true;
}

bool TrianglesManagerRenderer::updateDeletedTriangles(const Indices32& deletedTriangleIds, Frame* stateFrame)
{
	if (deletedTriangleIds.empty())
	{
		return true;
	}

	if (!isValid())
	{
		ocean_assert(false && "Invalid renderer!");
		return false;
	}

	hasChanged_ = true;

	constexpr float deletedTriangleColor = 0.0f; // for not yet textured triangles

	if (!setTriangles(deletedTriangleIds, deletedTriangleColor))
	{
		return false;
	}

	if (stateFrame != nullptr)
	{
		textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(*stateFrame);
	}

	return true;
}

ConstArrayAccessor<uint8_t> TrianglesManagerRenderer::triangleStates(const unsigned int numberTriangles)
{
	ocean_assert(numberTriangles <= framebufferWidth_ * framebufferHeight_);

	if (hasChanged_)
	{
		const unsigned int activeTriangleIdsFrameHeight = (numberTriangles + framebufferWidth_ - 1u) / framebufferWidth_;

		textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().copyColorTextureToFrame(triangleStatesFrame_, CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), framebufferWidth_, activeTriangleIdsFrameHeight));

		hasChanged_ = false;
	}

	ocean_assert(triangleStatesFrame_.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(numberTriangles <= triangleStatesFrame_.pixels());
	return ConstArrayAccessor<uint8_t>(triangleStatesFrame_.constdata<uint8_t>(), numberTriangles);
}

void TrianglesManagerRenderer::release()
{
	points_.release();
	triangles_.release();
	vertexSet_.release();
	textureFramebuffer_.release();
	copyTextureFramebuffer_.release();

	shaderProgramTexturedTriangles_.release();
	shaderProgramSetTriangles_.release();
	shaderProgramCopyFramebuffer_.release();

	framebufferWidth_ = 0u;
	framebufferHeight_ = 0u;

	hasChanged_ = false;
	triangleStatesFrame_.release();
}

inline bool TrianglesManagerRenderer::isValid() const
{
	return bool(shaderProgramSetTriangles_);
}

bool TrianglesManagerRenderer::setTriangles(const Indices32& triangleIds, const float triangleColor)
{
	if (triangleIds.empty())
	{
		return true;
	}

	if (!textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().bindFramebuffer())
	{
		return false;
	}

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramSetTriangles = shaderProgramSetTriangles_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	ocean_assert(points_);

	glViewport(0, 0, framebufferWidth_, framebufferHeight_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	glesShaderProgramSetTriangles.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	const GLint framebufferWidthLocation = glGetUniformLocation(glesShaderProgramSetTriangles.id(), "uFramebufferWidth");
	ocean_assert(framebufferWidthLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferWidthLocation, framebufferWidth_);

	const GLint framebufferHeightLocation = glGetUniformLocation(glesShaderProgramSetTriangles.id(), "uFramebufferHeight");
	ocean_assert(framebufferHeightLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(framebufferHeightLocation, framebufferHeight_);

	const GLint triangleColorLocation = glGetUniformLocation(glesShaderProgramSetTriangles.id(), "uTriangleColor");
	ocean_assert(triangleColorLocation != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(triangleColorLocation, triangleColor);

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute("aTriangleId", triangleIds.data(), triangleIds.size());

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramSetTriangles.id());

	points_->setIndices((unsigned int)(triangleIds.size()));
	points_.force<Rendering::GLESceneGraph::GLESPoints>().drawPoints();

	textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>().unbindFramebuffer();

	return true;
}

bool TrianglesManagerRenderer::copyFramebuffer()
{
	ocean_assert(isValid());
	ocean_assert(shaderProgramCopyFramebuffer_ && textureFramebuffer_ && copyTextureFramebuffer_);

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesCopyTextureFramebuffer = copyTextureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	if (!glesCopyTextureFramebuffer.bindFramebuffer())
	{
		return false;
	}

	glViewport(0, 0, framebufferWidth_, framebufferHeight_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ocean_assert(GL_NO_ERROR == glGetError());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ocean_assert(GL_NO_ERROR == glGetError());

	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgramCopyFramebuffer = shaderProgramCopyFramebuffer_.force<Rendering::GLESceneGraph::GLESShaderProgram>();

	glesShaderProgramCopyFramebuffer.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();

	const GLuint framebufferTextureId = glesTextureFramebuffer.colorTextureId();

	if (framebufferTextureId == 0u)
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	ocean_assert(GL_NO_ERROR == glGetError());

	glBindTexture(GL_TEXTURE_2D, framebufferTextureId);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint locationFramebufferTexture = glGetUniformLocation(glesShaderProgramCopyFramebuffer.id(), "framebufferTexture");
	ocean_assert(locationFramebufferTexture != -1);
	Rendering::GLESceneGraph::GLESObject::setUniform(locationFramebufferTexture, 0);

	vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>().bindVertexSet(glesShaderProgramCopyFramebuffer.id());

	triangles_.force<Rendering::GLESceneGraph::GLESTriangles>().drawTriangles();

	glesCopyTextureFramebuffer.unbindFramebuffer();

	return true;
}

}

}

}
