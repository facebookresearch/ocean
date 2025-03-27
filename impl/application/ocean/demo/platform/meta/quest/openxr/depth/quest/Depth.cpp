/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/depth/quest/Depth.h"

#include "ocean/cv/FrameConverterColorMap.h"

#include "ocean/platform/openxr/Utilities.h"

#include "ocean/rendering/glescenegraph/GLESVertexSet.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"

using namespace Platform::Meta;

Depth::Depth(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	requestAndroidPermission("com.oculus.permission.USE_SCENE");
}

Depth::StringSet Depth::necessaryOpenXRExtensionNames() const
{
	StringSet extensionNames = VRNativeApplicationAdvanced::necessaryOpenXRExtensionNames();

	extensionNames.emplace(XR_META_ENVIRONMENT_DEPTH_EXTENSION_NAME);

	return extensionNames;
}

void Depth::onAndroidPermissionGranted(const std::string& permission)
{
	VRNativeApplicationAdvanced::onAndroidPermissionGranted(permission);

	if (permission == "com.oculus.permission.USE_SCENE")
	{
		Log::info() << "OpenXR Depth: USE_SCENE permission granted";
	}
}

void Depth::onOpenXRSessionReady()
{
	VRNativeApplicationAdvanced::onOpenXRSessionReady();

	if (initializeDepth())
	{
		isDepthProviderRunning_ = true;

		Log::debug() << "OpenXR Depth: Initialized";
	}
	else
	{
		Log::error() << "OpenXR Depth: Failed to initialize";
	}
}

void Depth::onOpenXRSessionStopping()
{
	if (isDepthProviderRunning_)
	{
		ocean_assert(xrStopEnvironmentDepthProviderMETA_ != nullptr);
		xrStopEnvironmentDepthProviderMETA_(xrEnvironmentDepthProvider_);

		isDepthProviderRunning_ = false;
	}

	if (xrEnvironmentDepthSwapchainMETA_ != XR_NULL_HANDLE)
	{
		ocean_assert(xrDestroyEnvironmentDepthSwapchainMETA_ != nullptr);
		xrDestroyEnvironmentDepthSwapchainMETA_(xrEnvironmentDepthSwapchainMETA_);
		xrEnvironmentDepthSwapchainMETA_ = XR_NULL_HANDLE;
	}

	if (xrEnvironmentDepthProvider_ != XR_NULL_HANDLE)
	{
		ocean_assert(xrDestroyEnvironmentDepthProviderMETA_ != nullptr);
		xrDestroyEnvironmentDepthProviderMETA_(xrEnvironmentDepthProvider_);

		xrEnvironmentDepthProvider_ = XR_NULL_HANDLE;
	}

	VRNativeApplicationAdvanced::onOpenXRSessionStopping();
}

void Depth::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	// we create a new scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();

	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// let's create a group in which we place all rendering objects (invisible by default)
	renderingGroup_ = engine_->factory().createGroup();
	renderingGroup_->setVisible(false);
	scene->addChild(renderingGroup_);
}

void Depth::onFramebufferReleasing()
{
	renderingGroup_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void Depth::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (passthrough_.isValid() && !passthrough_.isStarted())
	{
		passthrough_.start();
	}

	if (!isDepthProviderRunning_)
	{
		return;
	}

	ocean_assert(xrAcquireEnvironmentDepthImageMETA_ != nullptr);

	const XrEnvironmentDepthImageAcquireInfoMETA xrEnvironmentDepthImageAcquireInfoMETA{XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_ACQUIRE_INFO_META, nullptr, baseSpace(), xrPredictedDisplayTime};

	XrEnvironmentDepthImageMETA xrEnvironmentDepthImageMETA{XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_META};
	xrEnvironmentDepthImageMETA.views[0] = {XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_VIEW_META};
	xrEnvironmentDepthImageMETA.views[1] = {XR_TYPE_ENVIRONMENT_DEPTH_IMAGE_VIEW_META};

	XrResult xrResult = xrAcquireEnvironmentDepthImageMETA_(xrEnvironmentDepthProvider_, &xrEnvironmentDepthImageAcquireInfoMETA, &xrEnvironmentDepthImageMETA);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to acquire depth image: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return;
	}

	ocean_assert(xrEnvironmentDepthImageMETA.swapchainIndex < swapChainImages_.size());

	if (xrEnvironmentDepthImageMETA.swapchainIndex < swapChainImages_.size())
	{
		const XrSwapchainImageOpenGLESKHR& swapChainImageHeader = swapChainImages_[xrEnvironmentDepthImageMETA.swapchainIndex];

		if (swapChainImageHeader.image != 0u)
		{
			Frame depthFrameA;
			Frame depthFrameB;
			if (extract(swapChainImageHeader.image, xrEnvironmentDepthSwapchainStateMETA_.width, xrEnvironmentDepthSwapchainStateMETA_.height, depthFrameA, depthFrameB))
			{
				const Quest::Application::VRImageVisualizer::ObjectSize objectSize(1);

				Frame colorFrameA;
				if (CV::FrameConverterColorMap::Comfort::convert1ChannelToRGB24(depthFrameA, colorFrameA, CV::FrameConverterColorMap::CM_TURBO))
				{
					constexpr unsigned int imageVisualizationId = 0u;

					vrImageVisualizer_.visualizeImageInView(imageVisualizationId, HomogenousMatrix4(Vector3(Scalar(-0.6), 0, -2)), std::move(colorFrameA), objectSize);
				}

				Frame colorFrameB;
				if (CV::FrameConverterColorMap::Comfort::convert1ChannelToRGB24(depthFrameB, colorFrameB, CV::FrameConverterColorMap::CM_TURBO))
				{
					constexpr unsigned int imageVisualizationId = 1u;

					vrImageVisualizer_.visualizeImageInView(imageVisualizationId, HomogenousMatrix4(Vector3(Scalar(0.6), 0, -2)), std::move(colorFrameB), objectSize);
				}
			}
		}
	}
}

bool Depth::initializeDepth()
{
	XrSystemEnvironmentDepthPropertiesMETA xrSystemEnvironmentDepthPropertiesMETA{XR_TYPE_SYSTEM_ENVIRONMENT_DEPTH_PROPERTIES_META};
	XrSystemProperties xrSystemProperties{XR_TYPE_SYSTEM_PROPERTIES, &xrSystemEnvironmentDepthPropertiesMETA};

	XrResult xrResult = xrGetSystemProperties(xrInstance_, xrSession_.xrSystemId(), &xrSystemProperties);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to determine system properties: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	bool allSucceeded = true;

	allSucceeded = initializeFunctionPointer(xrInstance_, "xrCreateEnvironmentDepthProviderMETA", xrCreateEnvironmentDepthProviderMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrDestroyEnvironmentDepthProviderMETA", xrDestroyEnvironmentDepthProviderMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrStartEnvironmentDepthProviderMETA", xrStartEnvironmentDepthProviderMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrStopEnvironmentDepthProviderMETA", xrStopEnvironmentDepthProviderMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrCreateEnvironmentDepthSwapchainMETA", xrCreateEnvironmentDepthSwapchainMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrDestroyEnvironmentDepthSwapchainMETA", xrDestroyEnvironmentDepthSwapchainMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrEnumerateEnvironmentDepthSwapchainImagesMETA", xrEnumerateEnvironmentDepthSwapchainImagesMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrGetEnvironmentDepthSwapchainStateMETA", xrGetEnvironmentDepthSwapchainStateMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrAcquireEnvironmentDepthImageMETA", xrAcquireEnvironmentDepthImageMETA_) && allSucceeded;
	allSucceeded = initializeFunctionPointer(xrInstance_, "xrSetEnvironmentDepthHandRemovalMETA", xrSetEnvironmentDepthHandRemovalMETA_) && allSucceeded;

	if (!allSucceeded)
	{
		Log::error() << "OpenXR Depth: Failed to initialize OpenXR extension functions";
		return false;
	}

	constexpr XrEnvironmentDepthProviderCreateFlagsMETA createFlags = 0;
	const XrEnvironmentDepthProviderCreateInfoMETA providerInfo{XR_TYPE_ENVIRONMENT_DEPTH_PROVIDER_CREATE_INFO_META, nullptr, createFlags};

	xrResult = xrCreateEnvironmentDepthProviderMETA_(xrSession_, &providerInfo, &xrEnvironmentDepthProvider_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to create depth provider: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	constexpr bool handRemovalEnabled = false;
	const XrEnvironmentDepthHandRemovalSetInfoMETA handRemovalInfo{XR_TYPE_ENVIRONMENT_DEPTH_HAND_REMOVAL_SET_INFO_META, nullptr, handRemovalEnabled};

	xrResult = xrSetEnvironmentDepthHandRemovalMETA_(xrEnvironmentDepthProvider_, &handRemovalInfo);
	ocean_assert(xrResult == XR_SUCCESS);

	if (xrResult != XR_SUCCESS)
	{
		return false;
	}

	ocean_assert(xrEnvironmentDepthProvider_ != XR_NULL_HANDLE);

	const XrEnvironmentDepthSwapchainCreateInfoMETA swapChainCreateInfo{XR_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_CREATE_INFO_META, nullptr, 0u};

	ocean_assert(xrEnvironmentDepthSwapchainMETA_ == XR_NULL_HANDLE);

	ocean_assert(xrCreateEnvironmentDepthSwapchainMETA_ != nullptr);
	xrResult = xrCreateEnvironmentDepthSwapchainMETA_(xrEnvironmentDepthProvider_, &swapChainCreateInfo, &xrEnvironmentDepthSwapchainMETA_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to create depth swapchain: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	xrEnvironmentDepthSwapchainStateMETA_ = {XR_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_STATE_META};

	ocean_assert(xrGetEnvironmentDepthSwapchainStateMETA_ != nullptr);
	xrResult = xrGetEnvironmentDepthSwapchainStateMETA_(xrEnvironmentDepthSwapchainMETA_, &xrEnvironmentDepthSwapchainStateMETA_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to get depth swapchain state: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	Log::debug() << "Swapchain dimensions: " << xrEnvironmentDepthSwapchainStateMETA_.width << "x" << xrEnvironmentDepthSwapchainStateMETA_.height;

	ocean_assert(xrEnumerateEnvironmentDepthSwapchainImagesMETA_ != nullptr);

	uint32_t imageCountOutput = 0u;
	xrResult = xrEnumerateEnvironmentDepthSwapchainImagesMETA_(xrEnvironmentDepthSwapchainMETA_, 0u, &imageCountOutput, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR Depth: Failed to enumerate depth swapchain images: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
		return false;
	}

	if (imageCountOutput != 0u)
	{
		Log::debug() << "OpenXR Depth: " << imageCountOutput << " depth swapchain images";

		ocean_assert(swapChainImages_.empty());
		swapChainImages_.resize(imageCountOutput, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});

		imageCountOutput = 0u;

		xrResult = xrEnumerateEnvironmentDepthSwapchainImagesMETA_(xrEnvironmentDepthSwapchainMETA_, uint32_t(swapChainImages_.size()), &imageCountOutput, (XrSwapchainImageBaseHeader*)(swapChainImages_.data()));

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "OpenXR Depth: Failed to determine depth swapchain images: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
			return false;
		}
	}

	if (!swapChainImages_.empty())
	{
		ocean_assert(xrStartEnvironmentDepthProviderMETA_ != nullptr);
		xrResult = xrStartEnvironmentDepthProviderMETA_(xrEnvironmentDepthProvider_);

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "OpenXR Depth: Failed to start depth provider: " << Platform::OpenXR::Utilities::translateResult(xrInstance_, xrResult);
			return false;
		}

		isDepthProviderRunning_ = true;
	}

	return true;
}

bool Depth::extract(const GLuint depthTexture, const uint32_t width, const uint32_t height, Frame& depthFrameA, Frame& depthFrameB)
{
	ocean_assert(depthTexture != 0u);
	ocean_assert(width != 0u && height != 0u);

	const GLboolean isTexture = glIsTexture(depthTexture);
	ocean_assert(glGetError() == GL_NO_ERROR);
	ocean_assert_and_suppress_unused(isTexture, isTexture);

	if (!textureFramebuffer_)
	{
		textureFramebuffer_ = engine_->factory().createTextureFramebuffer();
	}

	if (!textureFramebuffer_->setPixelFormat(FrameType::FORMAT_F32) || !textureFramebuffer_->resize(width, height))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!shaderProgram_)
	{
		shaderProgram_ = engine_->factory().createShaderProgram();

		static const char* partPlatform =
				// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
				R"SHADER(#version 300 es
					#define OCEAN_LOWP lowp
					#define OCEAN_HIGHP highp
				)SHADER";

		static const char* programVertexShader =
			R"SHADER(
				// Vertex attribute
				in vec4 aVertex;

				// Texture coordinate attribute
				in vec4 aTextureCoordinate;

				// Resulting texture coordinate
				out OCEAN_HIGHP vec2 vTextureCoordinate;

				void main(void)
				{
					gl_Position = aVertex;
					vTextureCoordinate = aTextureCoordinate.xy;
				}
			)SHADER";

		static const char* programFragmentShader =
			R"SHADER(
				// Input texture coordinate
				in OCEAN_HIGHP vec2 vTextureCoordinate;

				// Texture sampler object
				uniform sampler2DArray primaryTexture;

				// The index of the texture array to use
				uniform int primaryTextureIndex;

				// The out fragment color
				out OCEAN_HIGHP float fragColor;

				void main()
				{
					float depthValue = texture(primaryTexture, vec3(vTextureCoordinate, float(primaryTextureIndex))).r;

					fragColor = depthValue;
				}
			)SHADER";

		const std::vector<const char*> vertexShader = {partPlatform, programVertexShader};

		std::string errorMessage;
		if (!shaderProgram_->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShader, {partPlatform, programFragmentShader}, errorMessage))
		{
			Log::error() << "OpenXR Depth: Failed to create shader program: " << errorMessage;

			shaderProgram_.release();
			return false;
		}

		vertexSet_ = engine_->factory().createVertexSet();

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

		triangles_ = engine_->factory().createTriangles();

		const Rendering::TriangleFaces triangleFaces =
		{
			Rendering::TriangleFace(0u, 1u, 2u),
			Rendering::TriangleFace(0u, 2u, 3u)
		};

		triangles_->setFaces(triangleFaces);
		triangles_->setVertexSet(vertexSet_);
	}

	// now let's render the depth texture into the intermediate texture framebuffer

	Rendering::GLESceneGraph::GLESTextureFramebuffer& glesTextureFramebuffer = textureFramebuffer_.force<Rendering::GLESceneGraph::GLESTextureFramebuffer>();
	Rendering::GLESceneGraph::GLESShaderProgram& glesShaderProgram = shaderProgram_.force<Rendering::GLESceneGraph::GLESShaderProgram>();
	Rendering::GLESceneGraph::GLESVertexSet& glesVertexSet = vertexSet_.force<Rendering::GLESceneGraph::GLESVertexSet>();
	Rendering::GLESceneGraph::GLESTriangles& glesTriangles = triangles_.force<Rendering::GLESceneGraph::GLESTriangles>();

	for (size_t nTextureIndex = 0; nTextureIndex < 2; ++nTextureIndex)
	{
		if (!glesTextureFramebuffer.bindFramebuffer())
		{
			return false;
		}

		glViewport(0, 0, width, height);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		ocean_assert(glGetError() == GL_NO_ERROR);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ocean_assert(GL_NO_ERROR == glGetError());

		glesShaderProgram.bind(SquareMatrix4(false), HomogenousMatrix4(false), HomogenousMatrix4(false), SquareMatrix3(false));
		glesVertexSet.bindVertexSet(glesShaderProgram.id());

		glActiveTexture(GL_TEXTURE0);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindTexture(GL_TEXTURE_2D_ARRAY, depthTexture);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		ocean_assert(GL_NO_ERROR == glGetError());

		const GLint locationTexture = glGetUniformLocation(glesShaderProgram.id(), "primaryTexture");
		ocean_assert(locationTexture != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(locationTexture, 0);

		const GLint locationTextureIndex = glGetUniformLocation(glesShaderProgram.id(), "primaryTextureIndex");
		ocean_assert(locationTextureIndex != -1);
		Rendering::GLESceneGraph::GLESObject::setUniform(locationTextureIndex, int(nTextureIndex));

		glesTriangles.drawTriangles();

		Frame& depthFrame = nTextureIndex == 0 ? depthFrameA : depthFrameB;

		if (!textureFramebuffer_->copyColorTextureToFrame(depthFrame))
		{
			return false;
		}

		glesTextureFramebuffer.unbindFramebuffer();
	}

	return true;
}

template <typename T>
bool Depth::initializeFunctionPointer(const XrInstance& xrInstance, const char* functionName, T& function)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(functionName != nullptr);
	ocean_assert(function == nullptr);

	const XrResult xrResult = xrGetInstanceProcAddr(xrInstance, functionName, (PFN_xrVoidFunction*)(&function));

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "CaptureApplication: Failed to access '" << functionName << "' function: " << Platform::OpenXR::Utilities::translateResult(xrInstance, xrResult);

		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(function != nullptr);

	return true;
}
