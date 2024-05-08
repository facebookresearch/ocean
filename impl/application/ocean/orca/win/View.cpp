/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/orca/win/View.h"
#include "application/ocean/orca/win/Application.h"
#include "application/ocean/orca/win/MainWindow.h"

#include "application/ocean/orca/ContentManager.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBicubic.h"

#include "ocean/interaction/Manager.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/media/ExplicitRecorder.h"
#include "ocean/media/FileRecorder.h"
#include "ocean/media/ImageRecorder.h"
#include "ocean/media/ImageSequenceRecorder.h"
#include "ocean/media/ImplicitRecorder.h"
#include "ocean/media/FrameRecorder.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/System.h"

#include "ocean/rendering/ParallelView.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/StereoView.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/View.h"

#include "ocean/scenedescription/Manager.h"

#include "ocean/platform/win/Utilities.h"

namespace Ocean
{

namespace Orca
{

namespace Win
{

BEGIN_MESSAGE_MAP(View, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_MENU_RECORDER_STARTRECORDER, onStartRecorder)
	ON_COMMAND(ID_MENU_RECORDER_STOPRECORDER, onStopRecorder)
	ON_COMMAND(ID_MENU_RECORDER_ENABLERECORDER, onEnableRecorder)
	ON_COMMAND(ID_MENU_RECORDER_DISABLERECORDER, onDisableRecorder)
END_MESSAGE_MAP()

View::View()
{
	renderFrameCounterTimestamp_ = highPerformanceTimer_.seconds();
}

View::~View()
{
	ContentManager::get().release();
}

View::ViewType View::type() const
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		const Rendering::StereoViewRef stereoView(renderingFramebuffer_->view());
		if (stereoView)
		{
			if (stereoView->stereoType() == Rendering::StereoView::STEREO_TYPE_QUADBUFFERED)
				return TYPE_STEREO_VIEW_QUADBUFFERED;

			return TYPE_STEREO_VIEW_ANAGLYPH;
		}

		const Rendering::ParallelViewRef parallelView(renderingFramebuffer_->view());
		if (stereoView)
			return TYPE_PARALLEL_VIEW;

		const Rendering::PerspectiveViewRef perspectiveView(renderingFramebuffer_->view());
		if (perspectiveView)
			return TYPE_MONO_VIEW;
	}

	return TYPE_UNKNOWN;
}

std::string View::renderer() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingEngine_)
			return renderingEngine_->engineName();
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return std::string();
}

bool View::supportsQuadbufferedStereo() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
			return renderingFramebuffer_->isQuadbufferedStereoSupported();
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

bool View::eyesReversed() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::StereoViewRef stereoView(renderingFramebuffer_->view());

			if (stereoView)
				return stereoView->eyesReversed();
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

bool View::supportsAntialiasing(const unsigned int buffers) const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
			return renderingFramebuffer_->isAntialiasingSupported(buffers);
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

bool View::antialiasing() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
			return renderingFramebuffer_->isAntialiasing();
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

double View::horizontalFieldOfView() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::PerspectiveViewRef perspectiveView(renderingFramebuffer_->view());

			if (perspectiveView)
				return float(perspectiveView->fovX());
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return NumericD::deg2rad(45);
}

double View::nearDistance() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
			return float(renderingFramebuffer_->view()->nearDistance());
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return 0.01;
}

double View::farDistance() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
			return float(renderingFramebuffer_->view()->farDistance());
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return 10000.0;
}

double View::focusDistance() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::StereoViewRef stereoView(renderingFramebuffer_->view());

			if (stereoView)
				return float(stereoView->focusDistance());
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return 1.0;
}

float View::preferredFramerate() const
{
	return float(preferredFramerate_);
}

bool View::useHeadlight() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			return renderingFramebuffer_->view()->useHeadlight();
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

unsigned int View::backgroundColor() const
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			const RGBAColor color(renderingFramebuffer_->view()->backgroundColor());
			return (unsigned int)(color.red() * 255) + ((unsigned int)(color.green() * 255) << 8) + ((unsigned int)(color.blue() * 255) << 16);
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return 0u;
}

Media::FrameMediumRef View::backgroundMedium()
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			const Rendering::UndistortedBackgroundRef background(renderingFramebuffer_->view()->background());

			if (background)
				return background->medium();
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return Media::FrameMediumRef();
}

std::string View::recorderEncoder() const
{
	const Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
		return frameRecorder->frameEncoder();

	return std::string();
}

bool View::recorderEncoderConfiguration()
{
	const Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
		frameRecorder->frameEncoderConfiguration(long long(m_hWnd));

	return false;
}

double View::recorderFramerate() const
{
	const Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
		return frameRecorder->frameFrequency();

	return 25.0;
}

View::EncoderNames View::recorderEncoderNames() const
{
	const Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
		return frameRecorder->frameEncoders();

	return EncoderNames();
}

bool View::setType(const ViewType newType)
{
	if (newType == TYPE_UNKNOWN)
		return false;

	const ScopedLock scopedLock(lock_);

	if (newType == type())
		return true;

	if (renderingEngine_ && renderingFramebuffer_)
	{
		try
		{
			Config::Value& display = Application::get().config()["view"]["display"];
			Rendering::ViewRef view;

			switch (newType)
			{
				case TYPE_MONO_VIEW:
					view = renderingEngine_->factory().createPerspectiveView();
					break;

				case TYPE_PARALLEL_VIEW:
					view = renderingEngine_->factory().createParallelView();
					break;

				case TYPE_STEREO_VIEW_QUADBUFFERED:
				{
					Rendering::StereoViewRef stereoView(renderingEngine_->factory().createStereoView());
					stereoView->setStereoType(Rendering::StereoView::STEREO_TYPE_QUADBUFFERED);
					view = stereoView;
					break;
				}

				case TYPE_STEREO_VIEW_ANAGLYPH:
				{
					Rendering::StereoViewRef stereoView(renderingEngine_->factory().createStereoView());
					stereoView->setStereoType(Rendering::StereoView::STEREO_TYPE_ANAGLYPH);
					view = stereoView;
					break;
				}

				case TYPE_UNKNOWN:
					break;
			}

			if (view)
			{
				view->setNearDistance(Scalar(display["near"](0.01f)));
				view->setFarDistance(Scalar(display["far"](10000.0f)));

				if (renderingFramebuffer_->view())
				{
					view->setTransformation(renderingFramebuffer_->view()->transformation());
				}
			}

			Rendering::PerspectiveViewRef perspectiveView(view);
			if (perspectiveView)
			{
				perspectiveView->setFovX(Scalar(NumericD::deg2rad(display["fovx"](45.0))));
			}

			Rendering::StereoViewRef stereoView(view);
			if (stereoView)
			{
				stereoView->setFocusDistance(Scalar(display["focus"](1.0)));
			}

			if (renderingFramebuffer_->view().isNull() && Application::get().config()["view"]["navigation"]["storeposition"](false))
			{
				const double positionX = Application::get().config()["view"]["navigation"]["positionX"](0.0);
				const double positionY = Application::get().config()["view"]["navigation"]["positionY"](0.0);
				const double positionZ = Application::get().config()["view"]["navigation"]["positionZ"](0.0);

				const double orientationYaw = NumericD::deg2rad(Application::get().config()["view"]["navigation"]["orientationYaw"](0.0));
				const double orientationPitch = NumericD::deg2rad(Application::get().config()["view"]["navigation"]["orientationPitch"](0.0));
				const double orientationRoll = NumericD::deg2rad(Application::get().config()["view"]["navigation"]["orientationRoll"](0.0));

				const Vector3 viewingPosition = Vector3(Scalar(positionX), Scalar(positionY), Scalar(positionZ));
				const Euler viewingEuler = Euler(Scalar(orientationYaw), Scalar(orientationPitch), Scalar(orientationRoll));

				if (viewingEuler.isValid())
				{
					view->setTransformation(HomogenousMatrix4(viewingPosition, viewingEuler));
				}
			}
			else
			{
				view->setTransformation(HomogenousMatrix4(Vector3(0, 0, 20)));
			}

			renderingFramebuffer_->setView(view);
			OnSize(0, 0, 0);

		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	return true;
}

bool View::setRenderer(const std::string& engineName, const Rendering::Engine::GraphicAPI preferredGraphicAPI)
{
	if (m_hWnd == nullptr)
	{
		return false;
	}

	const std::string oldEngineName = renderingEngine_.isNull() ? std::string() : renderingEngine_->engineName();
	ContentManager::get().release();

	{
		const ScopedLock scopedLock(lock_);
		renderingFramebuffer_.release();
		renderingEngine_.release();
	}

	while (!oldEngineName.empty() && Rendering::ObjectRefManager::get().hasEngineObject(oldEngineName, false))
	{
		Sleep(1);
	}

	const ScopedLock scopedLock(lock_);

	try
	{
		if (engineName == "None")
		{
			return true;
		}

		if (engineName == "Default")
		{
			renderingEngine_ = Rendering::Manager::get().engine();
		}
		else
		{
			renderingEngine_ = Rendering::Manager::get().engine(engineName, preferredGraphicAPI);
		}

		if (renderingEngine_.isNull())
		{
			return false;
		}

		initializeFramebuffer();
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return bool(renderingEngine_);
}

bool View::setSupportQuadbufferedStereo(const bool support)
{
	if (support == framebufferSupportsQuadbufferedStereo_)
	{
		return true;
	}

	framebufferSupportsQuadbufferedStereo_ = support;
	return initializeFramebuffer();
}

bool View::setEyesReversed(const bool reverse)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::StereoViewRef stereoView(renderingFramebuffer_->view());

			if (stereoView)
			{
				stereoView->setEyesReversed(reverse);
				return true;
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

bool View::setSupportAntialiasing(const unsigned int buffers)
{
	const ScopedLock scopedLock(lock_);

	/*if (framebuffer)
		try
		{
			return framebuffer->setSupportAntialiasing(buffers);
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}

	return false;*/

	framebufferAntialiasingBuffers_ = buffers;
	return true;
}

bool View::setAntialiasing(const bool antialiasing)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		try
		{
			return renderingFramebuffer_->setAntialiasing(antialiasing);
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	return false;
}

bool View::setRendererFaceMode(const Rendering::Framebuffer::FaceMode mode)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		try
		{
			renderingFramebuffer_->setFaceMode(mode);
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	return true;
}

bool View::setRendererCullingMode(const Rendering::Framebuffer::CullingMode mode)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		try
		{
			renderingFramebuffer_->setCullingMode(mode);

			switch (mode)
			{
				case Rendering::PrimitiveAttribute::CULLING_DEFAULT:
					renderingFramebuffer_->setLightingMode(Rendering::PrimitiveAttribute::LM_DEFAULT);
					break;

				case Rendering::PrimitiveAttribute::CULLING_NONE:
				case Rendering::PrimitiveAttribute::CULLING_FRONT:
					renderingFramebuffer_->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
					break;

				case Rendering::PrimitiveAttribute::CULLING_BACK:
				case Rendering::PrimitiveAttribute::CULLING_BOTH:
					renderingFramebuffer_->setLightingMode(Rendering::PrimitiveAttribute::LM_SINGLE_SIDE_LIGHTING);
					break;
			}

		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	return true;
}

bool View::setRendererTechnique(const Rendering::Framebuffer::RenderTechnique technique)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		try
		{
			renderingFramebuffer_->setRenderTechnique(technique);
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
			return false;
		}
	}

	return true;
}

bool View::setPhantomMode(const Rendering::PhantomAttribute::PhantomMode mode)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_)
	{
		try
		{
			if (renderingFramebuffer_->view())
			{
				renderingFramebuffer_->view()->setPhantomMode(mode);
			}
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
			return false;
		}
	}

	return true;
}

bool View::setHorizontalFieldOfView(const double fovx)
{
	if (fovx <= 0.0 || fovx >= NumericD::pi())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::PerspectiveViewRef perspectiveView(renderingFramebuffer_->view());

			if (perspectiveView)
			{
				perspectiveView->setFovX(Scalar(fovx));
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setNearDistance(const double nearDistance)
{
	if (nearDistance <= 0.0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			if (nearDistance >= renderingFramebuffer_->view()->farDistance())
			{
				return false;
			}

			renderingFramebuffer_->view()->setNearDistance(Scalar(nearDistance));
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setFarDistance(const double farDistance)
{
	if (farDistance <= 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			if (farDistance <= renderingFramebuffer_->view()->nearDistance())
			{
				return false;
			}

			renderingFramebuffer_->view()->setFarDistance(Scalar(farDistance));
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setFocusDistance(const double focusDistance)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_)
		{
			const Rendering::StereoViewRef stereoView(renderingFramebuffer_->view());

			if (stereoView)
			{
				stereoView->setFocusDistance(Scalar(focusDistance));
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setPreferredFramerate(const float fps)
{
	if (fps < 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	preferredFramerate_ = fps;

	return true;
}

bool View::setUseHeadlight(const bool state)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			renderingFramebuffer_->view()->setUseHeadlight(state);
			return true;
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return false;
}

bool View::setBackgroundColor(const unsigned int color)
{
	if (color > 0xFFFFFFu)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			const float red = float(color & 0xFF);
			const float green = float((color & 0xFF00) >> 8);
			const float blue = float((color & 0xFF0000) >> 16);

			renderingFramebuffer_->view()->setBackgroundColor(RGBAColor(red / 255.0f, green / 255.0f, blue / 255.0f));
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setBackgroundMedium(const Media::FrameMediumRef& medium)
{
	const ScopedLock scopedLock(lock_);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			renderingFramebuffer_->view()->removeBackground(renderingBackground_);
			renderingBackground_.release();

			if (medium)
			{
				const Rendering::UndistortedBackgroundRef background(renderingEngine_->factory().createUndistortedBackground());

				if (background)
				{
					background->setDisplayType(renderingBackgroundType_);

					medium->start();
					background->setMedium(medium);

					renderingBackground_ = background;
					renderingFramebuffer_->view()->addBackground(renderingBackground_);

					adjustFovXToBackground_ = false;
				}
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setBackgroundDisplayType(const Rendering::UndistortedBackground::DisplayType type)
{
	const ScopedLock scopedLock(lock_);

	renderingBackgroundType_ = type;

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			Rendering::UndistortedBackgroundRef background(renderingFramebuffer_->view()->background());

			if (background)
			{
				background->setDisplayType(type);
			}
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	return true;
}

bool View::setCursorMode(const CursorMode mode)
{
	if (mode == CM_VISIBLE)
	{
		if (cursorVisible_ == false)
		{
			ShowCursor(TRUE);
		}

		cursorVisible_ = true;
	}
	else if (mode == CM_HIDE_INACTIVITY)
	{
		// nothing to do here
	}
	else if (mode == CM_HIDE_ALWAYS)
	{
		if (cursorVisible_)
		{
			ShowCursor(FALSE);
		}

		cursorVisible_ = false;
	}
	else
	{
		return false;
	}

	cursorMode_ = mode;
	return true;
}

bool View::setStorePosition(const bool store)
{
	storeViewingPosition_ = store;
	return true;
}

bool View::setRecorderType(const RecorderType type)
{
	if (type == TYPE_NONE && recorder_.isNull())
	{
		return true;
	}

	if (type == TYPE_NONE)
	{
		recorder_ = Media::RecorderRef();
		return true;
	}

	switch (type)
	{
		case TYPE_IMAGE:
		{
			recorder_ = Media::Manager::get().newRecorder(Media::Recorder::IMAGE_RECORDER);
			break;
		}

		case TYPE_IMAGE_SEQUENCE:
		{
			recorder_ = Media::Manager::get().newRecorder(Media::Recorder::IMAGE_SEQUENCE_RECORDER);

			const Media::ImageSequenceRecorderRef imageSequenceRecorder(recorder_);
			if (imageSequenceRecorder)
				imageSequenceRecorder->setMode(Media::ImageSequenceRecorder::RM_PARALLEL);

			break;
		}

		case TYPE_MOVIE:
		{
			recorder_ = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);
			break;
		}

		case TYPE_MEMORY:
		{
			recorder_ = Media::Manager::get().newRecorder(Media::Recorder::FRAME_MEMORY_RECORDER);
			break;
		}

		default:
		{
			ocean_assert(false && "Invalid recorder type.");
			break;
		}
	}

	Media::FrameRecorderRef frameRecorder(recorder_);
	if (frameRecorder)
	{
		RECT rect;
		GetWindowRect(&rect);

		unsigned int width = rect.right - rect.left;
		unsigned int height = rect.bottom - rect.top;

		frameRecorder->setPreferredFrameType(FrameType(width, height, FrameType::FORMAT_BGR24, FrameType::ORIGIN_LOWER_LEFT));
	}

	return bool(recorder_);
}

bool View::setRecorderEncoder(const std::string& encoder)
{
	const Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
	{
		if (encoder == "None")
		{
			return frameRecorder->setFrameEncoder(std::string());
		}
		else
		{
			return frameRecorder->setFrameEncoder(encoder);
		}
	}

	return false;
}

bool View::setRecorderFrameRate(const double fps)
{
	Media::FrameRecorderRef frameRecorder(recorder_);

	if (frameRecorder)
	{
		return frameRecorder->setFrameFrequency(fps);
	}

	return false;
}

bool View::setRecorderFilename(const std::string& filename)
{
	const Media::FileRecorderRef fileRecorder(recorder_);

	if (fileRecorder)
	{
		return fileRecorder->setFilename(filename);
	}

	return false;
}

bool View::setRecorderExtendedFilename(const bool extend)
{
	const Media::FileRecorderRef fileRecorder(recorder_);

	if (fileRecorder)
	{
		return fileRecorder->setFilenameSuffixed(extend);
	}

	return false;
}

void View::fitSceneToScreen()
{
	const ScopedLock scopedLock(lock_);

	if (renderingEngine_.isNull())
	{
		return;
	}

	if (renderingFramebuffer_)
	{
		try
		{
			if (renderingFramebuffer_->view())
				renderingFramebuffer_->view()->fitCamera();
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}
}

void View::onIdle()
{
	render();
}

void View::applyConfiguration()
{
	configurationApplied_ = true;
}

void View::storeConfiguration()
{
	ocean_assert(configurationApplied_);

	if (renderingFramebuffer_ && renderingFramebuffer_->view())
	{
		const HomogenousMatrix4 transformation = renderingFramebuffer_->view()->transformation();
		const Vector3 position = transformation.translation();
		const Euler euler(transformation.rotation());

		Application::get().config()["view"]["navigation"]["positionX"] = float(position.x());
		Application::get().config()["view"]["navigation"]["positionY"] = float(position.y());
		Application::get().config()["view"]["navigation"]["positionZ"] = float(position.z());

		Application::get().config()["view"]["navigation"]["orientationYaw"] = float(Numeric::rad2deg(euler.yaw()));
		Application::get().config()["view"]["navigation"]["orientationPitch"] = float(Numeric::rad2deg(euler.pitch()));
		Application::get().config()["view"]["navigation"]["orientationRoll"] = float(Numeric::rad2deg(euler.roll()));
	}
}

View& View::mainView()
{
	return MainWindow::mainWindow().mainView();
}

BOOL View::PreCreateWindow(CREATESTRUCT& createStruct)
{
	if (!CWnd::PreCreateWindow(createStruct))
	{
		return FALSE;
	}

	//createStruct.dwExStyle |= WS_EX_CLIENTEDGE;
	createStruct.style &= ~WS_BORDER;
	createStruct.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, ::LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr);

	return TRUE;
}

bool View::initializeFramebuffer()
{
	const ScopedLock scopedLock(lock_);

	if (renderingEngine_.isNull())
	{
		return false;
	}

	Rendering::ViewRef oldView;

	if (renderingFramebuffer_)
	{
		oldView = renderingFramebuffer_->view();
		renderingFramebuffer_ = Rendering::FramebufferRef();
	}

	renderingFramebuffer_ = renderingEngine_->createFramebuffer(Rendering::Framebuffer::FRAMEBUFFER_WINDOW);
	ocean_assert(renderingFramebuffer_);

	try
	{
		renderingFramebuffer_->setSupportQuadbufferedStereo(framebufferSupportsQuadbufferedStereo_);
	}
	catch (...)
	{
		// nothing to do here
	}

	try
	{
		renderingFramebuffer_->setSupportAntialiasing(framebufferAntialiasingBuffers_);
	}
	catch (...)
	{
		// nothing to do here
	}

	try
	{
		renderingFramebuffer_->initializeById(size_t(m_hWnd));
	}
	catch (...)
	{
		// nothing to do here
	}

	if (oldView)
	{
		renderingFramebuffer_->setView(oldView);
	}
	else
	{
		setType(TYPE_MONO_VIEW);
	}

	// Inform the viewEngine about the client window size
	OnSize(0, 0, 0);

	renderingFramebuffer_->view()->setBackgroundColor(RGBAColor(0, 0, 0));

	MainWindow::mainWindow().propertiesWindow().onFramebufferChanged();

	return true;
}

void View::render()
{
	const double startTimestamp = highPerformanceTimer_.seconds();

	if (preferredFramerate_ > 0 && startTimestamp < nextUpdateTimestamp_)
	{
		if (nextUpdateTimestamp_ - startTimestamp >= 0.01)
			Sleep(10);
		else
			Sleep(0);

		return;
	}

	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	checkCursorFullscreen();

	if (renderingEngine_ && renderingFramebuffer_)
	{
		const ScopedLock scopedManagerLock(Interaction::Manager::get().lock());
		const ScopedLock scopedFramebufferLock(renderingFramebuffer_->lock());

		if (renderingFramebuffer_->view())
		{
			try
			{
				if (adjustFovXToBackground_)
				{

					const Rendering::PerspectiveViewRef perspectiveView(renderingFramebuffer_->view());
					if (perspectiveView)
					{
						bool validCamera = false;
						const Scalar idealFovX = perspectiveView->idealFovX(&validCamera);

						if (validCamera)
						{
							perspectiveView->setFovX(idealFovX);
							adjustFovXToBackground_ = false;

							Log::info() << "Adjusting the view's field of view to the background's field of view: " << Numeric::rad2deg(idealFovX) << "deg";
						}
					}
				}

				Media::FrameRecorderRef localFrameRecorder = recorder_;

				if (sizeTimestamp_.isValid())
				{
					if (Timestamp(true) > sizeTimestamp_ + 1.0)
					{
						Media::FrameRecorderRef frameRecorder(recorder_);
						if (frameRecorder)
						{
							unsigned int left, top, width, height;
							renderingFramebuffer_->viewport(left, top, width, height);
							frameRecorder->setPreferredFrameType(FrameType(width, height, FrameType::FORMAT_BGR24, FrameType::ORIGIN_LOWER_LEFT));
						}

						sizeTimestamp_.toInvalid();
					}
					else
					{
						localFrameRecorder.release();
					}
				}

				// Applying specific pre-updates on all scene descriptions necessary before the interaction plugins are applied
				Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(renderingFramebuffer_->view(), Timestamp(true));

				// Applying the pre update function of the interaction plugins
				updateTimestamp = Interaction::Manager::get().preUpdate(userInterface, renderingEngine_, renderingFramebuffer_->view(), updateTimestamp);

				// Applying updates on all scene descriptions
				SceneDescription::Manager::get().update(renderingFramebuffer_->view(), updateTimestamp);

				// Updates the rendering engine
				renderingEngine_->update(updateTimestamp);

				// Applying post update function of the interaction plugins
				Interaction::Manager::get().postUpdate(userInterface, renderingEngine_, renderingFramebuffer_->view(), updateTimestamp);

				// Rendering the current frame

				if (localFrameRecorder)
				{
					Frame frame;
					if (renderingFramebuffer_->render(frame))
					{
						Frame recorderFrame;
						if (localFrameRecorder->lockBufferToFill(recorderFrame, false))
						{
							const bool result = CV::FrameConverter::Comfort::convertAndCopy(frame, recorderFrame);
							ocean_assert_and_suppress_unused(result, result);

							localFrameRecorder->unlockBufferToFill();
						}
					}
				}
				else
				{
					renderingFramebuffer_->render();
				}

				const double stopTimestamp = highPerformanceTimer_.seconds();
				const double recentUpdateAndRenderDuration = stopTimestamp - startTimestamp;

				if (preferredFramerate_ > 0)
					nextUpdateTimestamp_ = stopTimestamp + 1.0 / preferredFramerate_ - recentUpdateAndRenderDuration;

				++renderFrameCounter_;

				ocean_assert(stopTimestamp - renderFrameCounterTimestamp_ >= 0);
				const double elapsedTime = stopTimestamp - renderFrameCounterTimestamp_;

				if (elapsedTime > 0.2)
				{
					const float reachedFps = float(renderFrameCounter_ / elapsedTime);

					const HomogenousMatrix4 transformation = renderingFramebuffer_->view()->transformation();

					MainWindow::mainWindow().setStatusBarPosition(transformation.translation(), transformation.rotation());
					MainWindow::mainWindow().setStatusBarFramerate(reachedFps);
					MainWindow::mainWindow().setStatusBarProgress(ContentManager::get().progress());

					renderFrameCounterTimestamp_ = stopTimestamp;
					renderFrameCounter_ = 0;
				}
			}
			catch (const Exception& exception)
			{
				Log::error() << exception.what();
			}
			catch (...)
			{
				Log::error() << "Unknown error during rendering.";
			}
		}
	}
	else
	{
		CDC* dc = GetDC();
		ocean_assert(dc != nullptr);

		RECT clientRect;
		GetClientRect(&clientRect);

		dc->Rectangle(&clientRect);
		dc->TextOutW(50, 50, L"No valid rendering viewEngine", 25);

		ReleaseDC(dc);

		Sleep(1);
	}
}

void View::checkCursorFullscreen()
{
	POINT point;
	RECT rect;

	// The client rect is not suitable for multi display environments
	GetWindowRect(&rect);
	GetCursorPos(&point);

	bool hideCursor = point.x >= rect.left && point.x < rect.right && point.y >= rect.top && point.y < rect.bottom && MainWindow::mainWindow().fullscreen()
				&& (cursorMode_ == CM_HIDE_ALWAYS || cursorMode_ == CM_HIDE_INACTIVITY && Timestamp(true) > recentNavigationTimestamp_ + 1);

	if (hideCursor && cursorVisible_)
	{
		ShowCursor(FALSE);
		cursorVisible_ = false;
	}
	else if (!hideCursor && !cursorVisible_)
	{
		ShowCursor(TRUE);
		cursorVisible_ = true;
	}
}

void View::OnPaint()
{
	render();
	CWnd::OnPaint();
}

void View::OnSize(UINT type, int width, int height)
{
	const ScopedLock scopedLock(lock_);

	if (renderingFramebuffer_.isNull())
		return;

	try
	{
		RECT clientRect;
		GetClientRect(&clientRect);
		ocean_assert(clientRect.left == 0 && clientRect.top == 0);

		if (clientRect.right > 0 && clientRect.bottom > 0)
		{
			const Rendering::ViewRef& view = renderingFramebuffer_->view();

			renderingFramebuffer_->setViewport(0, 0, clientRect.right, clientRect.bottom);

			if (clientRect.bottom != 0 && view)
			{
				view->setAspectRatio(float(clientRect.right) / float(clientRect.bottom));
				MainWindow::mainWindow().setStatusBarSize(clientRect.right, clientRect.bottom);
			}

			sizeTimestamp_.toNow();
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
	}

	CWnd::OnSize(type, width, height);
}

void View::OnLButtonDblClk(UINT flags, CPoint point)
{
	recentNavigationTimestamp_.toNow();

	// Sets the focus to enable mouse wheel messages
	SetFocus();

	if (Application::get().config()["application"]["doubleclickfullscreen"](true))
		MainWindow::mainWindow().onToggleFullscreen();

	CWnd::OnLButtonDblClk(flags, point);
}

void View::OnLButtonDown(UINT flags, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	recentNavigationTimestamp_.toNow();

	// Sets the focus to enable mouse wheel messages
	SetFocus();

	navigationLastLeftMouseX_ = point.x;
	navigationLastLeftMouseY_ = point.y;

	if (engine() && (SceneDescription::Manager::get().handlesMouseEvents() || Interaction::Manager::get().handlesMouseEvents()))
	{
		Rendering::ObjectId objectId = Rendering::invalidObjectId;
		std::string objectName;
		Vector3 objectPosition;

		const Timestamp timestamp(true);

		Line3 ray;
		pickObject((unsigned int)(point.x), (unsigned int)(point.y), ray, objectId, objectPosition);

		Rendering::ObjectRef o(engine()->object(objectId));
		SceneDescription::Manager::get().mouseEvent(SceneDescription::BUTTON_LEFT, SceneDescription::EVENT_PRESS, Vector2(Scalar(point.x), Scalar(point.y)), objectPosition, objectId, timestamp);


		Interaction::Manager::get().onMousePress(userInterface, engine(), "LEFT", Vector2(Scalar(point.x), Scalar(point.y)), ray, objectId, objectPosition, timestamp);
	}

	CWnd::OnLButtonDown(flags, point);

	SetCapture();
}

void View::OnLButtonUp(UINT flags, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	recentNavigationTimestamp_.toNow();

	navigationLastLeftMouseX_ = invalidMouse_;
	navigationLastLeftMouseY_ = invalidMouse_;

	if (engine() && (SceneDescription::Manager::get().handlesMouseEvents() || Interaction::Manager::get().handlesMouseEvents()))
	{
		Rendering::ObjectId objectId = Rendering::invalidObjectId;
		std::string objectName;
		Vector3 objectPosition;

		const Timestamp timestamp(true);

		Line3 ray;
		pickObject((unsigned int)(point.x), (unsigned int)(point.y), ray, objectId, objectPosition);

		SceneDescription::Manager::get().mouseEvent(SceneDescription::BUTTON_LEFT, SceneDescription::EVENT_RELEASE, Vector2(Scalar(point.x), Scalar(point.y)), objectPosition, objectId, timestamp);
		Interaction::Manager::get().onMouseRelease(userInterface, engine(), "LEFT", Vector2(Scalar(point.x), Scalar(point.y)), ray, objectId, objectPosition, timestamp);
	}

	CWnd::OnLButtonUp(flags, point);

	ReleaseCapture();
}

void View::OnRButtonDown(UINT flags, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	recentNavigationTimestamp_.toNow();

	// Sets the focus to enable mouse wheel messages
	SetFocus();

	navigationLastRightMouseX_ = point.x;
	navigationLastRightMouseY_ = point.y;

	if (engine() && (SceneDescription::Manager::get().handlesMouseEvents() || Interaction::Manager::get().handlesMouseEvents()))
	{
		Rendering::ObjectId objectId = Rendering::invalidObjectId;
		std::string objectName;
		Vector3 objectPosition;

		const Timestamp timestamp(true);

		Line3 ray;
		pickObject((unsigned int)(point.x), (unsigned int)(point.y), ray, objectId, objectPosition);

		SceneDescription::Manager::get().mouseEvent(SceneDescription::BUTTON_RIGHT, SceneDescription::EVENT_PRESS, Vector2(Scalar(point.x), Scalar(point.y)), objectPosition, objectId, timestamp);
		Interaction::Manager::get().onMousePress(userInterface, engine(), "RIGHT", Vector2(Scalar(point.x), Scalar(point.y)), ray, objectId, objectPosition, timestamp);
	}

	CWnd::OnRButtonDown(flags, point);

	SetCapture();
}

void View::OnRButtonUp(UINT flags, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	recentNavigationTimestamp_.toNow();

	navigationLastRightMouseX_ = invalidMouse_;
	navigationLastRightMouseY_ = invalidMouse_;

	if (engine() && (SceneDescription::Manager::get().handlesMouseEvents() || Interaction::Manager::get().handlesMouseEvents()))
	{
		Rendering::ObjectId objectId = Rendering::invalidObjectId;
		std::string objectName;
		Vector3 objectPosition;

		const Timestamp timestamp(true);

		Line3 ray;
		pickObject((unsigned int)(point.x), (unsigned int)(point.y), ray, objectId, objectPosition);

		SceneDescription::Manager::get().mouseEvent(SceneDescription::BUTTON_RIGHT, SceneDescription::EVENT_RELEASE, Vector2(Scalar(point.x), Scalar(point.y)), objectPosition, objectId, timestamp);
		Interaction::Manager::get().onMouseRelease(userInterface, engine(), "RIGHT", Vector2(Scalar(point.x), Scalar(point.y)), ray, objectId, objectPosition, timestamp);
	}

	CWnd::OnRButtonUp(flags, point);

	ReleaseCapture();
}

void View::OnMouseMove(UINT flags, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	const Interaction::UserInterface userInterface;

	recentNavigationTimestamp_.toNow();

	if (renderingFramebuffer_)
	{
		try
		{
			//switch (viewNavigationMode)
			{
				//case MODE_USER:
					if ((flags & MK_LBUTTON) && navigationLastLeftMouseX_ != invalidMouse_ && navigationLastLeftMouseY_ != invalidMouse_)
					{
						const Quaternion orientation = renderingFramebuffer_->view()->transformation().rotation();

						const Vector3 xAxis(1, 0, 0);
						const Vector3 yAxis(0, 1, 0);

						Scalar factor = 0.5;
						if (flags & MK_CONTROL)
						{
							factor *= navigationControlFactor_;
						}
						else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
						{
							factor *= navigationAlternativeFactor_;
						}

						const Quaternion xRotation(orientation * xAxis, Numeric::deg2rad(Scalar(navigationLastLeftMouseY_ - point.y)) * factor);
						const Quaternion yRotation(orientation * yAxis, Numeric::deg2rad(Scalar(navigationLastLeftMouseX_ - point.x)) * factor);

						Quaternion rotation(xRotation * yRotation);
						rotation.normalize();

						const HomogenousMatrix4 newTransform(HomogenousMatrix4(rotation) * renderingFramebuffer_->view()->transformation());

						renderingFramebuffer_->view()->setTransformation(newTransform);

						navigationLastLeftMouseX_ = point.x;
						navigationLastLeftMouseY_ = point.y;
					}

					if ((flags & MK_RBUTTON) && navigationLastRightMouseX_ != invalidMouse_ && navigationLastRightMouseY_ != invalidMouse_)
					{
						Vector3 offset(Scalar(navigationLastRightMouseX_ - point.x), Scalar(point.y - navigationLastRightMouseY_), 0);

						Scalar factor = Scalar(0.01);
						if (flags & MK_CONTROL)
						{
							factor *= navigationControlFactor_;
						}
						else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
						{
							factor *= navigationAlternativeFactor_;
						}
						offset *= factor;

						HomogenousMatrix4 transformation = renderingFramebuffer_->view()->transformation();

						const Vector3 position = transformation.translation();
						const Quaternion orientation = transformation.rotation();

						transformation.setTranslation(position + orientation * offset);

						renderingFramebuffer_->view()->setTransformation(transformation);

						navigationLastRightMouseX_ = point.x;
						navigationLastRightMouseY_ = point.y;
					}

					//break;

				//default:
				//	ocean_assert(false && "Invalid navigation mode.");
			}
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	if (engine() && (SceneDescription::Manager::get().handlesMouseEvents() || Interaction::Manager::get().handlesMouseEvents()))
	{
		Rendering::ObjectId objectId = Rendering::invalidObjectId;
		std::string objectName;
		Vector3 objectPosition;

		const Timestamp timestamp(true);

		Line3 ray;
		pickObject((unsigned int)(point.x), (unsigned int)(point.y), ray, objectId, objectPosition);

		std::string buttonString;
		SceneDescription::ButtonType buttonType = SceneDescription::BUTTON_NONE;

		if (flags & MK_LBUTTON)
		{
			buttonString = "LEFT";
			buttonType = SceneDescription::BUTTON_LEFT;
		}
		else if (flags & MK_RBUTTON)
		{
			buttonString = "RIGHT";
			buttonType = SceneDescription::BUTTON_RIGHT;
		}
		else if (flags & MK_MBUTTON)
		{
			buttonString = "MIDDLE";
			buttonType = SceneDescription::BUTTON_MIDDLE;
		}

		SceneDescription::Manager::get().mouseEvent(buttonType, SceneDescription::EVENT_HOLD, Vector2(Scalar(point.x), Scalar(point.y)), objectPosition, objectId, timestamp);
		Interaction::Manager::get().onMouseMove(userInterface, engine(), buttonString, Vector2(Scalar(point.x), Scalar(point.y)), ray, objectId, objectPosition, timestamp);
	}

	return CWnd::OnMouseMove(flags, point);
}

BOOL View::OnMouseWheel(UINT flags, short delta, CPoint point)
{
	const ScopedLock scopedLock(lock_);

	recentNavigationTimestamp_.toNow();

	if (renderingFramebuffer_)
	{
		try
		{
			Scalar factor = Scalar(0.01);
			if (flags & MK_CONTROL)
			{
				factor *= navigationControlFactor_;
			}
			else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
			{
				factor *= navigationAlternativeFactor_;
			}

			HomogenousMatrix4 transformation = renderingFramebuffer_->view()->transformation();

			const Vector3 direction(transformation.rotation() * Vector3(0, 0, -1));
			const Vector3 position(transformation.translation());

			transformation.setTranslation(position + direction * (Scalar(delta) * factor));

			renderingFramebuffer_->view()->setTransformation(transformation);
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
	}

	return CWnd::OnMouseWheel(flags, delta, point);
}

void View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	std::string key;

	if ((nFlags & 0x4000) != 0x4000 && engine() && Platform::Win::Keyboard::translateVirtualkey(nChar, key))
	{
		const Interaction::UserInterface userInterface;

		Interaction::Manager::get().onKeyPress(userInterface, engine(), key, Timestamp(true));
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	std::string key;

	if ((nFlags & 0x4000) == 0x4000 && engine() && Platform::Win::Keyboard::translateVirtualkey(nChar, key))
	{
		const Interaction::UserInterface userInterface;

		Interaction::Manager::get().onKeyRelease(userInterface, engine(), key, Timestamp(true));
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void View::onStartRecorder()
{
	const Media::ExplicitRecorderRef explicitRecorder(recorder_);

	if (explicitRecorder)
	{
		if (explicitRecorder->isRecording() == false && explicitRecorder->start() == false)
		{
			Log::error() << "Could not start the recorder.";
		}
		else
		{
			Log::info() << "Recorder successfully started.";
		}
	}
	else
	{
		Log::error() << "No valid recorder selected.";
	}
}

void View::onStopRecorder()
{
	const Media::ExplicitRecorderRef explicitRecorder(recorder_);

	if (explicitRecorder)
	{
		if (explicitRecorder->isRecording() && explicitRecorder->stop() == false)
		{
			Log::error() << "Could not stop the file recorder.";
		}
		else
		{
			Log::info() << "Recorder successfully stopped.";
		}
	}
	else
	{
		Log::error() << "No valid recorder selected.";
	}
}

void View::onEnableRecorder()
{
	const Media::ImplicitRecorderRef implicitRecorder(recorder_);

	if (implicitRecorder)
	{
		if (implicitRecorder->isEnabled() == false && implicitRecorder->enable())
		{
			Log::info() << "Recorder successfully enabled.";
		}
		else
		{
			Log::error() << "Could not enable the recorder.";
		}
	}
	else
	{
		Log::error() << "No valid recorder selected.";
	}
}

void View::onDisableRecorder()
{
	const Media::ImplicitRecorderRef implicitRecorder(recorder_);

	if (implicitRecorder)
	{
		if (implicitRecorder->isEnabled() && implicitRecorder->disable() == false)
		{
			Log::error() << "Could not disable the recorder.";
		}
		else
		{
			Log::info() << "Recorder successfully disabled.";
		}
	}
	else
	{
		Log::error() << "No recorder selected.";
	}
}

bool View::pickObject(const unsigned int mouseX, const unsigned int mouseY, Line3& ray, Rendering::ObjectId& objectId, Vector3& objectPosition)
{
	objectId = Rendering::invalidObjectId;
	objectPosition = Vector3(0, 0, 0);

	try
	{
		if (renderingFramebuffer_ && renderingFramebuffer_->view())
		{
			RECT rect;
			GetClientRect(&rect);

			ray = Line3(renderingFramebuffer_->view()->viewingRay(Scalar(mouseX), Scalar(mouseY), rect.right - rect.left, rect.bottom - rect.top));

			Rendering::RenderableRef renderable;

			if (renderingFramebuffer_->intersection(ray, renderable, objectPosition))
			{
				const Rendering::ObjectRefSet geometryNodes(renderable->parentNodes());
				ocean_assert(geometryNodes.empty() == false);

				objectId = (*geometryNodes.begin())->id();
			}
		}
	}
	catch(...)
	{
		// nothing to do here
	}

	if (engine())
	{
		const Rendering::ObjectRef object = engine()->object(objectId);

		Rendering::BackgroundRef background(object);
		if (background)
		{
			// no background picking
			objectId = Rendering::invalidObjectId;
		}
	}

	return objectId != Rendering::invalidObjectId;
}

}

}

}
