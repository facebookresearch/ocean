/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/handtracker/android/GLMainView.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/android/Bitmap.h"
#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Utilities.h"

using namespace Ocean;

const bool GLMainView::instanceRegistered_ = GLMainView::registerInstanceFunction(GLMainView::createInstance);

GLMainView::GLMainView()
{
	pixelImage_ = Media::Manager::get().newMedium("PixelImageForRenderer", Media::Medium::PIXEL_IMAGE);

	const size_t registeredFonts = CV::Fonts::FontManager::get().registerFonts("/system/fonts");
	Log::debug() << "Registered " << registeredFonts << " fonts";
}

GLMainView::~GLMainView()
{
	stopThread();

	release();
}

void GLMainView::initializeHandTracker(const std::string& inputMedium, const std::string& resolution)
{
	inputFrameMedium_ = Media::Manager::get().newMedium(inputMedium);

	if (inputFrameMedium_)
	{
		if (resolution == "640x480")
		{
			inputFrameMedium_->setPreferredFrameDimension(640u, 480u);
		}
		else if (resolution == "1280x720")
		{
			inputFrameMedium_->setPreferredFrameDimension(1280u, 720u);
		}
		else if (resolution == "1920x1080")
		{
			inputFrameMedium_->setPreferredFrameDimension(1920u, 1080u);
		}

		inputFrameMedium_->start();

		pixelImage_->setDevice_T_camera(inputFrameMedium_->device_T_camera());
	}

	if (!setBackgroundMedium(pixelImage_, true /*adjustFov*/))
	{
		Log::error() << "Failed to set the background medium A";
	}

	JNIEnv* env = Platform::Android::NativeInterfaceManager::get().environment();

	if (env != nullptr)
	{
		Log::debug() << "Valid JNI environment";

		jActivityClass_ = Platform::Android::ScopedJClass(*env, env->FindClass("com/meta/ocean/app/demo/tracking/handtracker/android/HandTrackerActivity"));

		if (jActivityClass_)
		{
			jMethodId_ = env->GetStaticMethodID(jActivityClass_, "processImage", "(Landroid/graphics/Bitmap;)Ljava/lang/String;");

			if (jMethodId_ == nullptr)
			{
				Log::error() << "Failed to find Java method";
			}
			else
			{
				jActivityClass_.makeGlobal();
			}
		}
		else
		{
			Log::error() << "Failed to find Java class";
		}
	}
	else
	{
		Log::error() << "Failed to access JNI environment, no hand tracking possible";
	}

	if (inputFrameMedium_)
	{
		startThread();
	}
}

bool GLMainView::initialize()
{
	if (!GLFrameView::initialize())
	{
		return false;
	}

	Rendering::SceneRef scene = engine_->factory().createScene();
	framebuffer_->addScene(scene);

	Rendering::AbsoluteTransformRef absoluteTransformation = engine_->factory().createAbsoluteTransform();
	absoluteTransformation->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP);
	absoluteTransformation->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine_, "TEXT", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.0005), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);
	absoluteTransformation->addChild(textTransform);

	scene->addChild(absoluteTransformation);

	return true;
}

bool GLMainView::release()
{
	renderingText_.release();

	return GLFrameView::release();
}

void GLMainView::threadRun()
{
	JNIEnv* env = Platform::Android::NativeInterfaceManager::get().environment();

	int supportsDirectConversion = -1;

	Frame rgbaFrame;
	Frame intermediateFrame;

	const CV::FrameConverter::Options frameConverterOptions(uint8_t(0xFFu));

	Timestamp previousTimestamp(false);

	while (shouldThreadStop() == false)
	{
		FrameRef frame = inputFrameMedium_->frame();

		if (frame && frame->timestamp() > previousTimestamp)
		{
			previousTimestamp = frame->timestamp();

			if (supportsDirectConversion < 0)
			{
				if (CV::FrameConverter::Comfort::isSupported(*frame, FrameType::FORMAT_RGBA32))
				{
					supportsDirectConversion = 1;
				}
				else
				{
					supportsDirectConversion = 0;
				}
			}

			if (supportsDirectConversion == 1)
			{
				if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGBA32, rgbaFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, frameConverterOptions))
				{
					Log::error() << "Failed to convert frame " << FrameType::translatePixelFormat(frame->pixelFormat()) << " to RGBA32";
				}
			}
			else
			{
				if (CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGB24, intermediateFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
				{
					if (!CV::FrameConverter::Comfort::convert(intermediateFrame, FrameType::FORMAT_RGBA32, rgbaFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, frameConverterOptions))
					{
						Log::error() << "Failed to convert frame " << FrameType::translatePixelFormat(intermediateFrame.pixelFormat()) << " to RGBA32";
					}
				}
				else
				{
					Log::error() << "Failed to convert frame " << FrameType::translatePixelFormat(frame->pixelFormat()) << " to RGB24";
				}
			}

			if (rgbaFrame.isValid())
			{
				CV::FrameTransposer::Comfort::rotate(rgbaFrame, 90);

				if (env != nullptr && jActivityClass_ && jMethodId_)
				{
					Platform::Android::ScopedJObject bitmap = Platform::Android::Bitmap::toBitmap(env, rgbaFrame);

					if (bitmap)
					{
						jstring result = jstring(env->CallStaticObjectMethod(jActivityClass_, jMethodId_, *bitmap));

						if (result != nullptr)
						{
							const std::string cResult(Platform::Android::Utilities::toAString(env, result));

							Log::debug() << "Successfully processed image: " << cResult;

							if (renderingText_)
							{
								if (cResult.empty())
								{
									renderingText_->setText("None");
								}
								else
								{
									renderingText_->setText(cResult);
								}
							}
						}
						else
						{
							Log::error() << "Failed to process image";
						}
					}
				}
			}
			else
			{
				Log::error() << "Failed to convert frame";
			}

			pixelImage_->setPixelImage(*frame);
		}

		Thread::sleep(1u);
	}
}

jboolean Java_com_meta_ocean_app_demo_tracking_handtracker_android_HandTrackerActivity_initializeHandTracker(JNIEnv* env, jobject javaThis, jstring inputMedium, jstring resolution)
{
	const std::string inputMediumValue(Platform::Android::Utilities::toAString(env, inputMedium));
	const std::string resolutionValue(Platform::Android::Utilities::toAString(env, resolution));

	GLMainView::get<GLMainView>().initializeHandTracker(inputMediumValue, resolutionValue);

	return true;
}
