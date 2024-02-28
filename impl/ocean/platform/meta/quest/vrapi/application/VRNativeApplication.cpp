// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/VRNativeApplication.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/platform/meta/quest/vrapi/Utilities.h"

#include "ocean/rendering/Manager.h"

#include "ocean/rendering/glescenegraph/quest/Quest.h"

#include <stdio.h>
#include <unistd.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

using namespace Application;

namespace VrApi
{

namespace Application
{

VRNativeApplication::VRNativeApplication(struct android_app* androidApp) :
	NativeApplication(androidApp)
{
	// nothing to do here
}

VRNativeApplication::~VRNativeApplication()
{
	// nothing to do here
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

	framebuffer_ = engine_->createFramebuffer(Rendering::Framebuffer::FramebufferType::FRAMEBUFFER_WINDOW, Rendering::Framebuffer::FramebufferConfig{.useStencilBuffer = useStencilBuffer_});
	ocean_assert(framebuffer_);

	if (framebuffer_.isNull())
	{
		Log::error() << "Failed to create framebuffer!";
		return;
	}

	if (!framebuffer_->initializeById(size_t(&ovrJava_)))
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
	questFramebuffer_ = &framebuffer_.force<Rendering::GLESceneGraph::Quest::VrApi::GLESWindowFramebuffer>();
	ocean_assert(questFramebuffer_ != nullptr);

	// we ensure that the engine has the correct timestamp already
	engine_->update(Timestamp(vrapi_GetTimeInSeconds()));

	onFramebufferInitialized();

	// now, we proceed with the normal application loop

	NativeApplication::applicationLoop();

	onFramebufferReleasing();

	onReleaseResources();

	questFramebuffer_ = nullptr;
	stereoView_.release();
	framebuffer_.release();
	engine_.release();

	Rendering::GLESceneGraph::GLESEngine::unregisterEngine();
}

void VRNativeApplication::render(const double predictedDisplayTime)
{
	ocean_assert(stereoView_);
	ocean_assert(engine_);
	ocean_assert(framebuffer_ && questFramebuffer_ != nullptr);
	ocean_assert(ovrMobile_ != nullptr);

	const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovrMobile_, predictedDisplayTime);

	const HomogenousMatrix4 world_T_device = Utilities::toHomogenousMatrix4<Scalar>(tracking.HeadPose.Pose);

	const HomogenousMatrix4 leftView_T_world(Utilities::toHomogenousMatrix4<Scalar>(tracking.Eye[0].ViewMatrix));
	const HomogenousMatrix4 rightView_T_world(Utilities::toHomogenousMatrix4<Scalar>(tracking.Eye[1].ViewMatrix));

	stereoView_->setTransformation(world_T_device);
	stereoView_->setLeftTransformation(leftView_T_world.inverted());
	stereoView_->setRightTransformation(rightView_T_world.inverted());

	stereoView_->setLeftProjectionMatrix(Utilities::toSquareMatrix4<Scalar>(tracking.Eye[0].ProjectionMatrix));
	stereoView_->setRightProjectionMatrix(Utilities::toSquareMatrix4<Scalar>(tracking.Eye[1].ProjectionMatrix));

	// render

	ovrLayerProjection2 worldLayer = vrapi_DefaultLayerProjection2();
	worldLayer.HeadPose = tracking.HeadPose;

	for (size_t eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye)
	{
		worldLayer.Textures[eye].ColorSwapChain = questFramebuffer_->colorTextureSwapChain(eye);
		worldLayer.Textures[eye].SwapChainIndex = questFramebuffer_->textureSwapChainIndex(eye);
		worldLayer.Textures[eye].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection(&tracking.Eye[eye].ProjectionMatrix);
	}

	onPreRender(Timestamp(predictedDisplayTime));

	// If present, custom layers have been set by now; now set flags accordingly
	if (customPreLayers_.empty() && customPostLayers_.empty())
	{
		stereoView_->setBackgroundColor(RGBAColor(0.0f, 0.0f, 0.0f)); // fully opaque black

		worldLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
	}
	else
	{
		stereoView_->setBackgroundColor(RGBAColor(0.0f, 0.0f, 0.0f, 0.0f)); // fully transparent black

		worldLayer.Header.ColorScale = {1.0f, 1.0f, 1.0f, 1.0f};
		worldLayer.Header.SrcBlend = VRAPI_FRAME_LAYER_BLEND_SRC_ALPHA;
		worldLayer.Header.DstBlend = VRAPI_FRAME_LAYER_BLEND_ONE_MINUS_SRC_ALPHA;
		worldLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_INHIBIT_SRGB_FRAMEBUFFER | VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
	}

	engine_->update(Timestamp(predictedDisplayTime));
	framebuffer_->render();

	std::vector<const ovrLayerHeader2*> layers;
	layers.reserve(customPreLayers_.size() + customPostLayers_.size() + 1);

	for (CustomLayer& customLayer : customPreLayers_)
	{
		layers.push_back(customLayer.layerHeader());
	}

	layers.push_back(&worldLayer.Header);

	for (CustomLayer& customLayer : customPostLayers_)
	{
		layers.push_back(customLayer.layerHeader());
	}

	ovrSubmitFrameDescription2 frameDescription = {0};
	frameDescription.Flags = 0;
	frameDescription.SwapInterval = 1;
	frameDescription.FrameIndex = frameIndex_;
	frameDescription.DisplayTime = predictedDisplayTime;
	frameDescription.LayerCount = uint32_t(layers.size());
	frameDescription.Layers = layers.data();

	// Hand over the eye images to the time warp.
	vrapi_SubmitFrame2(ovrMobile_, &frameDescription);

	customPreLayers_.clear();
	customPostLayers_.clear();
}

bool VRNativeApplication::isVRModeEntered() const
{
	return ovrMobile_ != nullptr;
}

void VRNativeApplication::onIdle()
{
	ocean_assert(questFramebuffer_ != nullptr);

	if (applicationResumed_ && androidNativeWindow_ != nullptr)
	{
		if (ovrMobile_ == nullptr)
		{
			// we are ready to enter the vr mode

			ovrModeParms ovrParameters = vrapi_DefaultModeParms(&ovrJava_);

			// no need to reset the FLAG_FULLSCREEN window flag when using a View
			ovrParameters.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
			ovrParameters.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
			ovrParameters.Display = size_t(questFramebuffer_->eglContext().display());
			ovrParameters.ShareContext = size_t(questFramebuffer_->eglContext().context());
			ovrParameters.WindowSurface = size_t(androidNativeWindow_);

			ovrMobile_ = vrapi_EnterVrMode(&ovrParameters);

			if (ovrMobile_ != nullptr)
			{
				onVrModeEntered();
			}
			else
			{
				Log::error() << "vrapi_EnterVrMode() failed!";

				// if entering VR mode failed then the ANativeWindow was not valid.
				androidNativeWindow_ = nullptr;
			}
		}
	}
	else
	{
		if (ovrMobile_ != nullptr)
		{
			vrapi_LeaveVrMode(ovrMobile_);
			ovrMobile_ = nullptr;

			onVrModeLeft();
		}
	}

	if (ovrMobile_ == nullptr)
	{
		// vr mode not yet entered

		Thread::sleep(0u);
		return;
	}

	// This is the only place the frame index is incremented, right before
	// calling vrapi_GetPredictedDisplayTime().
	frameIndex_++;

	// Get the HMD pose, predicted for the middle of the time period during which
	// the new eye images will be displayed. The number of frames predicted ahead
	// depends on the pipeline depth of the engine and the synthesis rate.
	// The better the prediction, the less black will be pulled in at the edges.
	const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(ovrMobile_, frameIndex_);

	trackedRemoteDevice_.update(Timestamp(predictedDisplayTime));

	const uint32_t buttonsLeftPressed = trackedRemoteDevice_.buttonsPressed(TrackedRemoteDevice::RT_LEFT);
	const uint32_t buttonsRightPressed = trackedRemoteDevice_.buttonsPressed(TrackedRemoteDevice::RT_RIGHT);

	if (buttonsLeftPressed != 0u || buttonsRightPressed != 0u)
	{
		onButtonPressed(buttonsLeftPressed | buttonsRightPressed, buttonsLeftPressed, buttonsRightPressed, Timestamp(predictedDisplayTime));
	}

	const uint32_t buttonsLeftReleased = trackedRemoteDevice_.buttonsReleased(TrackedRemoteDevice::RT_LEFT);
	const uint32_t buttonsRightReleased = trackedRemoteDevice_.buttonsReleased(TrackedRemoteDevice::RT_RIGHT);

	if (buttonsLeftReleased != 0u || buttonsRightReleased != 0u)
	{
		onButtonReleased(buttonsLeftReleased | buttonsRightReleased, buttonsLeftReleased, buttonsRightReleased, Timestamp(predictedDisplayTime));
	}

	// we are ready to render the frame

	render(predictedDisplayTime);
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

	// let's release all resources we are responsible for

	vrTextVisualizer_ = VRTextVisualizer();
	vrImageVisualizer_ = VRImageVisualizer();
}

void VRNativeApplication::onVrModeEntered()
{
	Log::debug() << "VRNativeApplication::onVrModeEntered()";

	ocean_assert(ovrMobile_ != nullptr);

	constexpr int32_t cpuLevel = 2;
	constexpr int32_t gpuLevel = 3;
	vrapi_SetClockLevels(ovrMobile_, cpuLevel, gpuLevel);
	vrapi_SetPerfThread(ovrMobile_, VRAPI_PERF_THREAD_TYPE_MAIN, gettid());

	trackedRemoteDevice_ = TrackedRemoteDevice(ovrMobile_, deviceType());
}

void VRNativeApplication::onVrModeLeft()
{
	Log::debug() << "VRNativeApplication::onVrModeLeft()";

	trackedRemoteDevice_ = TrackedRemoteDevice();
}

void VRNativeApplication::onPreRender(const Timestamp& renderTimestamp)
{
	ocean_assert(renderTimestamp.isValid());
}

void VRNativeApplication::onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	ocean_assert(buttons != 0u);

	if (buttons & ovrButton_B)
	{
		showSystemConfirmQuitMenu();
	}
}

void VRNativeApplication::onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	ocean_assert(buttons != 0u);
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

}

}

}

}

}

}
