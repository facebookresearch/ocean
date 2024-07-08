/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplication.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/platform/openxr/Utilities.h"

#include "ocean/rendering/Manager.h"

#include "ocean/rendering/glescenegraph/quest/Quest.h"

#ifndef XR_USE_GRAPHICS_API_OPENGL_ES
	#define XR_USE_GRAPHICS_API_OPENGL_ES
#endif

#ifndef XR_USE_PLATFORM_ANDROID
	#define XR_USE_PLATFORM_ANDROID
#endif

#include <openxr/openxr_platform.h>

#include <sys/sysinfo.h>

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

using namespace Application;

namespace OpenXR
{

namespace Application
{

#ifdef OCEAN_PLATFORM_BUILD_ANDROID

VRNativeApplication::VRNativeApplication(struct android_app* androidApp) :
	NativeApplication(androidApp)
{
	// nothing to do here
}

#endif // OCEAN_PLATFORM_BUILD_ANDROID

VRNativeApplication::~VRNativeApplication()
{
	// nothing to do here
}

VRNativeApplication::StringSet VRNativeApplication::necessaryOpenXRExtensionNames() const
{
	StringSet extensionNames = NativeApplication::necessaryOpenXRExtensionNames();

	extensionNames.emplace(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
	extensionNames.emplace(XR_FB_COLOR_SPACE_EXTENSION_NAME);

	return extensionNames;
}

bool VRNativeApplication::createOpenXRSession(const XrViewConfigurationViews& xrViewConfigurationViews)
{
	Log::debug() << "VRNativeApplication::createOpenXRSession()";

	ocean_assert(!xrViewConfigurationViews.empty());

	PFN_xrGetOpenGLESGraphicsRequirementsKHR pfnGetOpenGLESGraphicsRequirementsKHR = nullptr;

	XrResult result = xrGetInstanceProcAddr(xrInstance_, "xrGetOpenGLESGraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&pfnGetOpenGLESGraphicsRequirementsKHR));

	if (result != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine OpenGLES requirements function: " << xrInstance_.translateResult(result);
		return false;
	}

	XrGraphicsRequirementsOpenGLESKHR xrGraphicsRequirementsOpenGLESKHR = {XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};
	result = pfnGetOpenGLESGraphicsRequirementsKHR(xrInstance_, xrInstance_.xrSystemId(), &xrGraphicsRequirementsOpenGLESKHR);

	if (result != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine OpenGLES requirements: " << xrInstance_.translateResult(result);
		return false;
	}

	Log::debug() << "Minimal OpenGLES version: " << XR_VERSION_MAJOR(xrGraphicsRequirementsOpenGLESKHR.minApiVersionSupported) << "." << XR_VERSION_MINOR(xrGraphicsRequirementsOpenGLESKHR.minApiVersionSupported);
	Log::debug() << "Maximal OpenGLES version: " << XR_VERSION_MAJOR(xrGraphicsRequirementsOpenGLESKHR.maxApiVersionSupported) << "." << XR_VERSION_MINOR(xrGraphicsRequirementsOpenGLESKHR.maxApiVersionSupported);

	const Platform::GLES::EGLContext::ConfigAttributePairs configAttributePairs =
	{
		{EGL_RED_SIZE, 8},
		{EGL_GREEN_SIZE, 8},
		{EGL_BLUE_SIZE, 8},
		{EGL_ALPHA_SIZE, 8},
		{EGL_DEPTH_SIZE, 0},
		{EGL_STENCIL_SIZE, useStencilBuffer_ ? 8 : 0},
		{EGL_SAMPLES, 0}
	};

	if (!eglContext_.initialize(configAttributePairs))
	{
		Log::error() << "Failed to initialize an EGL context!";
		return false;
	}

	Log::debug() << "OpenGLES initialized";

	XrGraphicsBindingOpenGLESAndroidKHR xrGraphicsBindingAndroidGLES = {XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR};
	xrGraphicsBindingAndroidGLES.display = eglContext_.display();
	xrGraphicsBindingAndroidGLES.config = eglContext_.config();
	xrGraphicsBindingAndroidGLES.context = eglContext_.context();

	ocean_assert(xrViewConfigurationViews.size() == 2);
	ocean_assert(xrViewConfigurationViews.front().recommendedImageRectWidth == xrViewConfigurationViews.back().recommendedImageRectWidth);
	ocean_assert(xrViewConfigurationViews.front().recommendedImageRectHeight == xrViewConfigurationViews.back().recommendedImageRectHeight);

	if (!xrSession_.initialize(xrInstance_, &xrGraphicsBindingAndroidGLES, xrViewConfigurationViews.front().recommendedImageRectWidth, xrViewConfigurationViews.front().recommendedImageRectHeight))
	{
		Log::error() << "Failed to initialize an OpenXR session!";
		return false;
	}

	Log::debug() << "OpenXR session initialized";

	xrSpaceView_ = xrSession_.createSpace(XR_REFERENCE_SPACE_TYPE_VIEW);
	xrSpaceLocal_ = xrSession_.createSpace(XR_REFERENCE_SPACE_TYPE_LOCAL);
	xrSpaceStage_ = xrSession_.createSpace(XR_REFERENCE_SPACE_TYPE_STAGE);

	return true;
}

void VRNativeApplication::releaseOpenXRSession()
{
	Log::debug() << "VRNativeApplication::releaseOpenXRSession()";

	onReleaseActionSets();

	xrSpaceView_.release();
	xrSpaceLocal_.release();
	xrSpaceStage_.release();

	xrSession_.release();
}

void VRNativeApplication::applicationLoop()
{
	registerSystemFonts();

	// we setup and initialize the rendering engine (EGL context, framebuffers, etc.)

	Rendering::GLESceneGraph::Quest::registerGLESceneGraphEngine();

	engine_ = Rendering::Manager::get().engine();
	ocean_assert(engine_);

	if (engine_.isNull())
	{
		Log::error() << "Rendering engine does not exist!";
		return;
	}

	Rendering::Framebuffer::FramebufferConfig framebufferConfiguration;
	framebufferConfiguration.useStencilBuffer = useStencilBuffer_;

	framebuffer_ = engine_->createFramebuffer(Rendering::Framebuffer::FramebufferType::FRAMEBUFFER_WINDOW, framebufferConfiguration);
	ocean_assert(framebuffer_);

	if (framebuffer_.isNull())
	{
		Log::error() << "Failed to create framebuffer!";
		return;
	}

	if (!framebuffer_->initializeById(size_t(&xrSession_)))
	{
		Log::error() << "Failed to initialize framebuffer!";
		return;
	}

	stereoView_ = engine_->factory().createStereoView();

	if (stereoView_.isNull())
	{
		Log::error() << "Failed to create stereo view!";
		return;
	}

	framebuffer_->setView(stereoView_);

	ocean_assert(questFramebuffer_ == nullptr);
	questFramebuffer_ = &framebuffer_.force<Rendering::GLESceneGraph::Quest::OpenXR::GLESWindowFramebuffer>();
	ocean_assert(questFramebuffer_ != nullptr);

	// we need to initialize the rendering engine, however as the session is not yet running, we cannot use the session time
	// on Quest OpenXR is using the uptime since boot, so we determine the time manually

	Timestamp initializationTimestamp(0.0);

	struct sysinfo systemInformation;
	if (sysinfo(&systemInformation) == 0)
	{
		initializationTimestamp = Timestamp(double(systemInformation.uptime));
	}
	else
	{
		Log::error() << "OpenXR VRNativeApplication: Failed to determine system up time";
	}

	engine_->update(initializationTimestamp);

	onFramebufferInitialized();

	// now, we proceed with the normal application loop

	NativeApplication::applicationLoop();

	onFramebufferReleasing();

#ifdef OCEAN_DEBUG
	{
		if (!framebuffer_->scenes().empty())
		{
			Log::debug() << "VRNativeApplication: The rendering framebuffer still holds " << framebuffer_->scenes().size() << " scene(s)";

			for (const Rendering::SceneRef& scene : framebuffer_->scenes())
			{
				if (scene->name().empty())
				{
					Log::debug() << scene->name();
				}
			}
		}
	}
#endif

	onReleaseResources();

	questFramebuffer_ = nullptr;
	stereoView_.release();
	framebuffer_.release();
	engine_.release();

	Rendering::GLESceneGraph::GLESEngine::unregisterEngine();
}

void VRNativeApplication::registerSystemFonts()
{
	const HighPerformanceTimer timer;

	const size_t registeredFonts = CV::Fonts::FontManager::get().registerFonts("/system/fonts");

	const double time = timer.seconds();

	if (registeredFonts == 0)
	{
		Log::warning() << "Failed to register fonts from '/system/fonts'";
	}
	else
	{
		Log::info() << "Registered " << registeredFonts << " fonts from '/system/fonts' in " << time << " seconds";
	}
}

void VRNativeApplication::render(const bool shouldRender, const XrTime& xrPredictedDisplayTime, const Timestamp& renderTimestamp)
{
	ocean_assert(stereoView_);
	ocean_assert(engine_);
	ocean_assert(framebuffer_ && questFramebuffer_ != nullptr);
	ocean_assert(xrSession_.isValid());

	XrSpaceLocation xrSpaceLocation = {XR_TYPE_SPACE_LOCATION};
	XrResult xrResult = xrLocateSpace(xrSpaceView_.object(), baseSpace(), xrPredictedDisplayTime, &xrSpaceLocation);
	ocean_assert_and_suppress_unused(xrResult == XR_SUCCESS, xrResult);

	const HomogenousMatrix4 world_T_device(Utilities::toHomogenousMatrix4<Scalar>(xrSpaceLocation.pose));

	XrFrameBeginInfo xrFrameBeginInfo = {XR_TYPE_FRAME_BEGIN_INFO};
	xrResult = xrBeginFrame(xrSession_, &xrFrameBeginInfo);
	ocean_assert(xrResult == XR_SUCCESS);

	XrViewLocateInfo xrViewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
	xrViewLocateInfo.viewConfigurationType = xrViewConfigurationType_;
	xrViewLocateInfo.displayTime = xrPredictedDisplayTime;
	xrViewLocateInfo.space = xrSpaceView_.object();

	XrViewState xrViewState = {XR_TYPE_VIEW_STATE};

  /// The OpenXR views for both eyes.
	XrView xrViews[maximalNumberEyes_] = {{XR_TYPE_VIEW}, {XR_TYPE_VIEW}};

	uint32_t viewCountOutput = 0u;
	xrResult = xrLocateViews(xrSession_, &xrViewLocateInfo, &xrViewState, uint32_t(maximalNumberEyes_), &viewCountOutput, xrViews);
	ocean_assert(xrResult == XR_SUCCESS);

	if (viewCountOutput != uint32_t(maximalNumberEyes_))
	{
		Log::error() << "Invalid number of views: " << viewCountOutput << " expected " << maximalNumberEyes_;
		return;
	}

	const HomogenousMatrix4 device_T_leftView(Utilities::toHomogenousMatrix4<Scalar>(xrViews[0].pose));
	const HomogenousMatrix4 device_T_rightView(Utilities::toHomogenousMatrix4<Scalar>(xrViews[1].pose));

	ocean_assert(nearDistance_ > 0.0f && nearDistance_ < farDistance_);
	const SquareMatrix4 leftClip_T_leftView(Utilities::toProjectionMatrix4(xrViews[0].fov, nearDistance_, farDistance_));
	const SquareMatrix4 rightClip_T_rightView(Utilities::toProjectionMatrix4(xrViews[1].fov, nearDistance_, farDistance_));

	const HomogenousMatrix4 world_T_views[2] =
	{
		world_T_device * device_T_leftView,
		world_T_device * device_T_rightView
	};

	stereoView_->setTransformation(world_T_device);
	stereoView_->setLeftTransformation(world_T_views[0]);
	stereoView_->setRightTransformation(world_T_views[1]);

	stereoView_->setLeftProjectionMatrix(leftClip_T_leftView);
	stereoView_->setRightProjectionMatrix(rightClip_T_rightView);

	stereoView_->setBackgroundColor(RGBAColor(0.0f, 0.0f, 0.0f, 0.0f)); // fully transparent black

	// render

	xrCompositorLayerUnions_.clear();

	onAddCompositorBackLayers(xrCompositorLayerUnions_);

	xrCompositorLayerUnions_.emplace_back();

	XrCompositionLayerProjection& xrCompositionLayerProjection = xrCompositorLayerUnions_.back().xrCompositionLayerProjection_;
	memset(&xrCompositionLayerProjection, 0, sizeof(XrCompositionLayerProjection));

	XrCompositionLayerProjectionView xrCompositionLayerProjectionViews[maximalNumberEyes_] = {{XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW}};

	xrCompositionLayerProjection.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
	xrCompositionLayerProjection.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
	xrCompositionLayerProjection.space = baseSpace();
	xrCompositionLayerProjection.viewCount = uint32_t(maximalNumberEyes_);
	xrCompositionLayerProjection.views = xrCompositionLayerProjectionViews;

	for (size_t eyeIndex = 0; eyeIndex < maximalNumberEyes_; ++eyeIndex)
	{
		XrCompositionLayerProjectionView& xrCompositionLayerProjectionView = xrCompositionLayerProjectionViews[eyeIndex];
		memset(&xrCompositionLayerProjectionView, 0, sizeof(XrCompositionLayerProjectionView));

		xrCompositionLayerProjectionView.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
		xrCompositionLayerProjectionView.pose = Utilities::toXrPose(world_T_views[eyeIndex]);
		xrCompositionLayerProjectionView.fov = xrViews[eyeIndex].fov;

		XrSwapchainSubImage& xrSwapchainSubImage = xrCompositionLayerProjectionView.subImage;

		memset(&xrSwapchainSubImage, 0, sizeof(XrSwapchainSubImage));
		xrSwapchainSubImage.swapchain = questFramebuffer_->xrSwapchain(eyeIndex);
		xrSwapchainSubImage.imageRect.offset.x = 0;
		xrSwapchainSubImage.imageRect.offset.y = 0;
		xrSwapchainSubImage.imageRect.extent.width = questFramebuffer_->width(eyeIndex);
		xrSwapchainSubImage.imageRect.extent.height = questFramebuffer_->height(eyeIndex);
		xrSwapchainSubImage.imageArrayIndex = 0;
	}

	onPreRender(xrPredictedDisplayTime, renderTimestamp);

	engine_->update(renderTimestamp);
	framebuffer_->render();

	onAddCompositorFrontLayers(xrCompositorLayerUnions_);

	xrCompositionLayerBaseHeaders_.clear();

	for (const XrCompositorLayerUnion& xrCompositorLayerUnion : xrCompositorLayerUnions_)
	{
		xrCompositionLayerBaseHeaders_.emplace_back((const XrCompositionLayerBaseHeader*)(&xrCompositorLayerUnion));
	}

	XrFrameEndInfo xrFrameEndInfo = {XR_TYPE_FRAME_END_INFO};
	xrFrameEndInfo.displayTime = xrPredictedDisplayTime;
	xrFrameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	xrFrameEndInfo.layerCount = uint32_t(xrCompositionLayerBaseHeaders_.size());
	xrFrameEndInfo.layers = xrCompositionLayerBaseHeaders_.data();

	xrResult = xrEndFrame(xrSession_, &xrFrameEndInfo);
	ocean_assert(xrResult == XR_SUCCESS);
}

XrSpace VRNativeApplication::baseSpace() const
{
	// by default, we use the stage space, this behavior can be customized in derived classes

	return xrSpaceStage_.object();
}

void VRNativeApplication::onOpenXRSessionReady()
{
	NativeApplication::onOpenXRSessionReady();

	ocean_assert(xrSession_.isValid());

	ocean_assert(!xrSessionIsRunning_);

	if (xrSession_.begin(xrViewConfigurationType_))
	{
		xrSessionIsRunning_ = true;
	}

	if (actionSets_.empty())
	{
		// the OpenXR session can become ready for the second+ time after a session was ended and must not be initialized/attached again for the same session
		onConfigureActionSets(actionSets_);

		if (!actionSets_.empty())
		{
			XrActionSets xrActionSets;
			xrActionSets.reserve(actionSets_.size());

			for (const SharedActionSet& actionSet : actionSets_)
			{
				if (actionSet && actionSet->actionBindingsSuggested())
				{
					xrActionSets.emplace_back(*actionSet);
				}
				else
				{
					Log::error() << "OpenXR VRNativeApplication: No suggested action bindings in action set";
				}
			}

			XrSessionActionSetsAttachInfo xrSessionActionSetsAttachInfo{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
			xrSessionActionSetsAttachInfo.countActionSets = uint32_t(xrActionSets.size());
			xrSessionActionSetsAttachInfo.actionSets = xrActionSets.data();

			const XrResult xrResult = xrAttachSessionActionSets(xrSession_, &xrSessionActionSetsAttachInfo);

			if (xrResult == XR_SUCCESS)
			{
				Log::debug() << "OpenXR VRNativeApplication: Action sets attached to session";
			}
			else
			{
				Log::error() << "OpenXR VRNativeApplication: Failed to attach action sets: " << xrInstance_.translateResult(xrResult);
			}
		}
	}
}

void VRNativeApplication::onOpenXRSessionStopping()
{
	ocean_assert(xrSession_.isValid());

	ocean_assert(xrSessionIsRunning_);

	if (xrSession_.end())
	{
		xrSessionIsRunning_ = false;
	}

	NativeApplication::onOpenXRSessionStopping();
}

void VRNativeApplication::onConfigureActionSets(Platform::OpenXR::SharedActionSets& actionSets)
{
	ocean_assert(xrSession_.isValid());

	if (trackedController_.initialize(xrSession_))
	{
		actionSets.emplace_back(trackedController_.actionSet());
	}
	else
	{
		Log::error() << "OpenXR: Failed to initialize TrackedController";
	}
}

void VRNativeApplication::onReleaseActionSets()
{
	Log::debug() << "VRNativeApplication::onReleaseActionSets()";

	trackedController_.release();

	actionSets_.clear();
}

void VRNativeApplication::onIdle()
{
	if (!xrSessionIsRunning_)
	{
		// the session is either not running or not in focus (e.g., the Quest menu is shown)
		return;
	}

	ocean_assert(xrSession_.isValid());

	XrTime xrPredictedDisplayTime = 0ull;
	const bool shouldRender = xrSession_.nextFrame(xrPredictedDisplayTime);

	const Timestamp renderTimestamp(Timestamp::nanoseconds2seconds(xrPredictedDisplayTime));

	if (xrSessionState() == XR_SESSION_STATE_FOCUSED)
	{
		// action sets cannot be synced when the session is not in focus

		xrActiveActionSets_.resize(actionSets_.size());

		for (size_t n = 0; n < actionSets_.size(); ++n)
		{
			ocean_assert(*actionSets_[n]);

			if (*actionSets_[n])
			{
				xrActiveActionSets_[n] = XrActiveActionSet{*actionSets_[n], XR_NULL_PATH};
			}
		}

		XrActionsSyncInfo xrActionsSyncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
		xrActionsSyncInfo.countActiveActionSets = uint32_t(xrActiveActionSets_.size());
		xrActionsSyncInfo.activeActionSets = xrActiveActionSets_.data();

		XrResult xrResult = xrSyncActions(xrSession_, &xrActionsSyncInfo);

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "Failed to sync actions: " << xrInstance_.translateResult(xrResult);
		}

		if (trackedController_.isValid() && !trackedController_.update(baseSpace(), xrPredictedDisplayTime))
		{
			ocean_assert(false && "This should never happen!");
		}

		const TrackedController::ButtonType buttonsPressed = trackedController_.buttonsPressed();

		if (buttonsPressed != TrackedController::BT_NONE)
		{
			onButtonPressed(buttonsPressed, renderTimestamp);
		}

		const TrackedController::ButtonType buttonsReleased = trackedController_.buttonsReleased();

		if (buttonsReleased != TrackedController::BT_NONE)
		{
			onButtonReleased(buttonsReleased, renderTimestamp);
		}
	}

	// we are ready to render the frame

	render(shouldRender, xrPredictedDisplayTime, renderTimestamp);
}

void VRNativeApplication::onReleaseResources()
{
	Log::debug() << "VRNativeApplication::onReleaseResources()";
}

void VRNativeApplication::onFramebufferInitialized()
{
	Log::debug() << "VRNativeApplication::onFramebufferInitialized()";

	vrImageVisualizer_ = VRImageVisualizer(engine_, framebuffer_);
	vrTextVisualizer_ = VRTextVisualizer(engine_, framebuffer_);
}

void VRNativeApplication::onFramebufferReleasing()
{
	Log::debug() << "VRNativeApplication::onFramebufferReleasing()";

	vrImageVisualizer_ = VRImageVisualizer();
	vrTextVisualizer_ = VRTextVisualizer();
}

void VRNativeApplication::onAddCompositorBackLayers(XrCompositorLayerUnions& /*xrCompositorLayerUnions*/)
{
	// can be implemented in derived classes
}

void VRNativeApplication::onAddCompositorFrontLayers(XrCompositorLayerUnions& /*xrCompositorLayerUnions*/)
{
	// can be implemented in derived classes
}

void VRNativeApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	ocean_assert(predictedDisplayTime.isValid());
}

void VRNativeApplication::onButtonPressed(const TrackedController::ButtonType buttons, const Timestamp& /*timestamp*/)
{
	ocean_assert_and_suppress_unused(buttons != TrackedController::BT_NONE, buttons);
}

void VRNativeApplication::onButtonReleased(const TrackedController::ButtonType buttons, const Timestamp& /*timestamp*/)
{
	ocean_assert_and_suppress_unused(buttons != TrackedController::BT_NONE, buttons);
}

}

}

}

}

}

}
