// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/GLESNativeApplication.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/platform/meta/quest/vrapi/Utilities.h"

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

namespace VrApi
{

namespace Application
{

GLESNativeApplication::GLESNativeApplication(struct android_app* androidApp) :
	NativeApplication(androidApp),
	ovrMobile_(nullptr),
	frameIndex_(0u)
{
	// nothing to do here
}

GLESNativeApplication::~GLESNativeApplication()
{
	// nothing to do here
}

void GLESNativeApplication::applicationLoop()
{
	registerSystemFonts();

	Platform::GLES::EGLContext::ConfigAttributePairs configAttributePairs;
	if (!onPreConfigureEGLContext(configAttributePairs))
	{
		Log::error() << "No valid EGL configiguration attributes provided!";
		return;
	}

	if (!eglContext_.initialize(configAttributePairs))
	{
		Log::error() << "Failed to initialize an EGL context!";
		return;
	}

	Log::info() << "EGLContext initialized.";

	static_assert(VRAPI_FRAME_LAYER_EYE_MAX == 2, "Invalid stereo frame number!");
	glesFramebuffers_.resize(VRAPI_FRAME_LAYER_EYE_MAX);

	GLenum colorFormat = 0u;
	unsigned int multisamples = 0u;

	if (!onPreConfigureGLESFramebuffer(colorFormat, multisamples))
	{
		Log::error() << "No valid GLES framebuffer configiguration provided!";
		return;
	}

	const int framebufferWidth = vrapi_GetSystemPropertyInt(&java(), VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
	const int framebufferHeight = vrapi_GetSystemPropertyInt(&java(), VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);

	if (framebufferWidth <= 0 || framebufferHeight <= 0)
	{
		Log::error() << "Failed to determine the viewport!";
		return;
	}

	for (GLESFramebuffer& framebuffer : glesFramebuffers_)
	{
		if (!framebuffer.initialize(colorFormat, (unsigned int)(framebufferWidth), (unsigned int)(framebufferHeight), multisamples))
		{
			Log::error() << "Failed to initialize framebuffer!";
			return;
		}
	}

	onFramebufferInitialized();

	NativeApplication::applicationLoop();

	for (GLESFramebuffer& framebuffer : glesFramebuffers_)
	{
		framebuffer.release();
	}

	eglContext_.release();
}

bool GLESNativeApplication::onPreConfigureEGLContext(Platform::GLES::EGLContext::ConfigAttributePairs& configAttributePairs)
{
	// can be overwritten in derived classes

	configAttributePairs =
	{
		{EGL_RED_SIZE, 8},
		{EGL_GREEN_SIZE, 8},
		{EGL_BLUE_SIZE, 8},
		{EGL_ALPHA_SIZE, 8},
		{EGL_DEPTH_SIZE, 0},
		{EGL_STENCIL_SIZE, 0},
		{EGL_SAMPLES, 0}
	};

	return true;
}

bool GLESNativeApplication::onPreConfigureGLESFramebuffer(GLenum& colorFormat, unsigned int& multisamples)
{
	// can be overwritten in derived classes

	colorFormat = GL_RGBA8;
	multisamples = 4u;

	return true;
}

void GLESNativeApplication::onFramebufferInitialized()
{
	// can be implemented in derived classes to initialize graphic objects
}

void GLESNativeApplication::onIdle()
{
	ocean_assert(eglContext_.isValid());

	if (applicationResumed_ && androidNativeWindow_ != nullptr)
	{
		if (ovrMobile_ == nullptr)
		{
			// we are ready to enter the vr mode

			ovrModeParms ovrParameters = vrapi_DefaultModeParms(&ovrJava_);

			// no need to reset the FLAG_FULLSCREEN window flag when using a View
			ovrParameters.Flags &= ~VRAPI_MODE_FLAG_RESET_WINDOW_FULLSCREEN;
			ovrParameters.Flags |= VRAPI_MODE_FLAG_NATIVE_WINDOW;
			ovrParameters.Display = size_t(eglContext_.display());
			ovrParameters.ShareContext = size_t(eglContext_.context());
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

	const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovrMobile_, predictedDisplayTime);

	ocean_assert(glesFramebuffers_.size() == VRAPI_FRAME_LAYER_EYE_MAX);

	ovrLayerProjection2 worldLayer = vrapi_DefaultLayerProjection2();
	worldLayer.HeadPose = tracking.HeadPose;

	for (size_t eyeIndex = 0; eyeIndex < glesFramebuffers_.size(); ++eyeIndex)
	{
		GLESFramebuffer& glesFramebuffer = glesFramebuffers_[eyeIndex];

		const HomogenousMatrix4 world_T_view(Utilities::toHomogenousMatrix4<Scalar>(tracking.Eye[eyeIndex].ViewMatrix));

		const SquareMatrix4 projection(Utilities::toSquareMatrix4<Scalar>(tracking.Eye[eyeIndex].ProjectionMatrix));

		worldLayer.Textures[eyeIndex].ColorSwapChain = glesFramebuffer.colorTextureSwapChain();
		worldLayer.Textures[eyeIndex].SwapChainIndex = glesFramebuffer.textureSwapChainIndex();
		worldLayer.Textures[eyeIndex].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection(&tracking.Eye[eyeIndex].ProjectionMatrix);

		glesFramebuffer.bind();

		glViewport(0, 0, glesFramebuffer.width(), glesFramebuffer.height());

		renderFramebuffer(eyeIndex, world_T_view, projection, Timestamp(predictedDisplayTime));

		glesFramebuffer.invalidateDepthBuffer();
		glesFramebuffer.swap();

		GLESFramebuffer::unbind();
	}

	worldLayer.Header.Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;

	const ovrLayerHeader2* layers[] =
	{
		&worldLayer.Header
	};

	ovrSubmitFrameDescription2 frameDescription = {0};
	frameDescription.Flags = 0;
	frameDescription.SwapInterval = 1;
	frameDescription.FrameIndex = frameIndex_;
	frameDescription.DisplayTime = predictedDisplayTime;
	frameDescription.LayerCount = 1;
	frameDescription.Layers = layers;

	// Hand over the eye images to the time warp.
	vrapi_SubmitFrame2(ovrMobile_, &frameDescription);
}

void GLESNativeApplication::renderFramebuffer(const size_t /*eyeIndex*/, const HomogenousMatrix4& /*view_T_world*/, const SquareMatrix4& /*projection*/, const Timestamp& /*predictedDisplayTime*/)
{
	// needs to be implemented in derived class
}

bool GLESNativeApplication::isVRModeEntered() const
{
	return ovrMobile_ != nullptr;
}


void GLESNativeApplication::onVrModeEntered()
{
	ocean_assert(ovrMobile_ != nullptr);

	constexpr int32_t cpuLevel = 2;
	constexpr int32_t gpuLevel = 3;
	vrapi_SetClockLevels(ovrMobile_, cpuLevel, gpuLevel);
	vrapi_SetPerfThread(ovrMobile_, VRAPI_PERF_THREAD_TYPE_MAIN, gettid());

	trackedRemoteDevice_ = TrackedRemoteDevice(ovrMobile_, deviceType());
}

void GLESNativeApplication::onVrModeLeft()
{
	trackedRemoteDevice_ = TrackedRemoteDevice();
}

void GLESNativeApplication::onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	ocean_assert(buttons != 0u);

	if (buttons & ovrButton_B)
	{
		showSystemConfirmQuitMenu();
	}
}

void GLESNativeApplication::onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	ocean_assert(buttons != 0u);
}

void GLESNativeApplication::registerSystemFonts()
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
