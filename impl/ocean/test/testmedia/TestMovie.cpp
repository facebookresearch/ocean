/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testmedia/TestMovie.h"

#include "ocean/base/String.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieRecorder.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE)
	#include "ocean/media/avfoundation/AVFoundation.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_LINUX) and !defined(OCEAN_DISABLE_FFMPEG_IN_CMAKE)
	#include "ocean/media/ffmpeg/FFmpeg.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#include "ocean/media/android/Android.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/media/mediafoundation/MediaFoundation.h"
#endif

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

TestMovie::MovieVerifier::MovieVerifier(const unsigned int expectedWidth, const unsigned int expectedHeight, const unsigned int expectedNumberFrames, const double expectedFps) :
	expectedWidth_(expectedWidth),
	expectedHeight_(expectedHeight),
	expectedNumberFrames_(expectedNumberFrames),
	expectedFps_(expectedFps)
{
	lastUpdateTimestamp_.toNow();
}

void TestMovie::MovieVerifier::onFrame(const Frame& frame, const SharedAnyCamera& /*camera*/)
{
	ocean_assert(frame.isValid());

	const ScopedLock scopedLock(lock_);

	lastUpdateTimestamp_.toNow();

	if (frame.isValid())
	{
		if (frame.width() == expectedWidth_ && frame.height() == expectedHeight_)
		{
			Frame rgbFrame;
			if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
			{
				ocean_assert(false && "This should never happen!");
				hasError_ = true;

				return;
			}

			unsigned int frameIndex = (unsigned int)(-1);
			if (!parseFrameIndex(rgbFrame, frameIndex) || frameIndex != numberReceivedFrames_)
			{
				Log::debug() << "Failed to parse frame index " << numberReceivedFrames_ << ", got " << frameIndex;
				hasError_ = true;
			}

			if (expectedHeight_ >= yColorStrip_ + bitSize_)
			{
				if (!verifyColor(rgbFrame, numberReceivedFrames_))
				{
					Log::debug() << "Verifying color failed in frame " << numberReceivedFrames_;
					hasError_ = true;
				}
			}

			const double relativeTimestamp = double(frame.relativeTimestamp());

			const double expectedRelativeTimestamp = double(numberReceivedFrames_) / expectedFps_;

			constexpr double tolerance = 0.01; // 10ms
			if (!NumericD::isEqual(relativeTimestamp, expectedRelativeTimestamp, tolerance))
			{
				Log::debug() << "Invalid relative timestamp " << expectedRelativeTimestamp << ", got " << relativeTimestamp;
				hasError_ = true;
			}

			++numberReceivedFrames_;
		}
		else
		{
			Log::debug() << "Invalid frame resolution " << expectedWidth_ << "x" << expectedHeight_ << ", got " << frame.width() << "x" << frame.height();
			hasError_ = true;
		}
	}
	else
	{
		hasError_ = true;
	}
}

Timestamp TestMovie::MovieVerifier::lastUpdateTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	return lastUpdateTimestamp_;
}

bool TestMovie::MovieVerifier::succeeded() const
{
	const ScopedLock scopedLock(lock_);

	if (hasError_)
	{
		return false;
	}

	if (expectedNumberFrames_ == numberReceivedFrames_)
	{
		return true;
	}

	Log::debug() << "Invalid number of frames " << expectedNumberFrames_ << ", got " << numberReceivedFrames_;

	return false;
}

bool TestMovie::MovieVerifier::parseFrameIndex(const Frame& rgbFrame, unsigned int& frameIndex)
{
	ocean_assert(rgbFrame.isValid() && rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	if (rgbFrame.width() < numberBits_ * bitSize_)
	{
		return false;
	}

	constexpr unsigned int bitSize_2 = bitSize_ / 2u;

	unsigned int value = 0u;

	for (unsigned int bitIndex = 0u; bitIndex < numberBits_; ++bitIndex)
	{
		const unsigned int x = bitIndex * bitSize_ + bitSize_2;
		constexpr unsigned int y = bitSize_2;

		const uint8_t* pixel = rgbFrame.constpixel<uint8_t>(x, y);

		constexpr int tolerance = 20;

		constexpr uint8_t black = 0;
		constexpr uint8_t white = 255;

		unsigned int whiteCounter = 0u;
		unsigned int blackCounter = 0u;

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			if (abs(white - int(pixel[0])) < tolerance)
			{
				++whiteCounter;
			}
			else if (abs(black - int(pixel[0])) < tolerance)
			{
				++blackCounter;
			}
		}

		if (blackCounter == 3u)
		{
			value |= 1u << bitIndex;
		}
		else if (whiteCounter == 3u)
		{
			// nothing to do here
		}
		else
		{
			return false;
		}
	}

	frameIndex = value;

	return true;
}

bool TestMovie::MovieVerifier::verifyColor(const Frame& rgbFrame, const unsigned int frameIndex)
{
	ocean_assert(rgbFrame.isValid() && rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

	if (rgbFrame.height() < yColorStrip_ + bitSize_)
	{
		return false;
	}

	const uint8_t* const pixel = rgbFrame.constpixel<uint8_t>(rgbFrame.width() / 2u, yColorStrip_ + bitSize_ / 2u);

	const uint8_t* expectedColor = TestMovie::uniqueColor(frameIndex);
	ocean_assert(expectedColor != nullptr);

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		const unsigned int error = (unsigned int)(abs(int(pixel[n]) - int(expectedColor[n])));

		if (error > 30u)
		{
			return false;
		}
	}

	return true;
}

bool TestMovie::test(const double testDuration)
{
	ocean_assert_and_suppress_unused(testDuration > 0.0, testDuration);

	registerMediaLibraries();

	Log::info() << "Movie test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testEncodeDecode() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLoop() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPause() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire Movie test succeeded.";
	}
	else
	{
		Log::info() << "Movie test FAILED!";
	}

	unregisterMediaLibraries();

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

/**
 * This class implements a simple instance for the GTest ensuring that
 * the OpenImageLibrary plugin is registered during startup and unregistered before shutdown.
 */
class TestMovieGTestInstance : public ::testing::Test
{
	protected:

		/**
		 * Default constructor.
		 */
  		TestMovieGTestInstance()
  		{
  			// nothing to do here
		}

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
			TestMovie::registerMediaLibraries();
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
			TestMovie::unregisterMediaLibraries();
		}
};

TEST_F(TestMovieGTestInstance, EncodeDecode)
{
	EXPECT_TRUE(TestMovie::testEncodeDecode());
}

TEST_F(TestMovieGTestInstance, Loop)
{
	EXPECT_TRUE(TestMovie::testLoop());
}

#endif // OCEAN_USE_GTEST

bool TestMovie::testEncodeDecode()
{
	Log::info() << "Encode/decode tests:";
	Log::info() << " ";

	const std::vector<std::string> encoderNames(libraryNamesEncoder());
	const std::vector<std::string> decoderNames(libraryNamesDecoder());

	bool allSucceeded = true;

	if (encoderNames.empty())
	{
		Log::info() << "Skipping encode/decode test as no encoder exists.";
		return true;
	}

	const IO::ScopedDirectory directory(IO::Directory::createTemporaryDirectory());

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	constexpr unsigned int numberFrames = 100u;
	constexpr double fps = 30.0;

	if (directory.exists())
	{
		for (const std::string& encoderName : encoderNames)
		{
			const IO::File file = writeMovie(directory, width, height, numberFrames, fps, encoderName);

			for (const std::string& decoderName : decoderNames)
			{
				Log::info() << "Using '" << encoderName << "' to encode, and '" << decoderName << "' to decode the movie:";

				if (file.exists() && readMovie(file, width, height, numberFrames, fps, decoderName))
				{
					Log::info() << "Validation: succeeded";
				}
				else
				{
					Log::info() << "Validation: FAILED!";

					allSucceeded = false;
				}

				Log::info() << " ";
			}
		}
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "All encode/decode tests succeeded.";
	}
	else
	{
		Log::info() << "All encode/decode tests FAILED!";
	}

	return allSucceeded;
}

bool TestMovie::testLoop()
{
	Log::info() << "Loop test:";
	Log::info() << " ";

	const std::vector<std::string> encoderNames(libraryNamesEncoder());
	const std::vector<std::string> decoderNames(libraryNamesDecoder());

	bool allSucceeded = true;

	if (encoderNames.empty())
	{
		Log::info() << "Skipping encode/decode test as no encoder exists.";
		return true;
	}

	const IO::ScopedDirectory directory(IO::Directory::createTemporaryDirectory());

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	constexpr unsigned int numberFrames = 10u;
	constexpr double fps = 30.0;

	if (directory.exists())
	{
		const IO::File file = writeMovie(directory, width, height, numberFrames, fps, encoderNames.front());

		if (file.exists())
		{
			for (const std::string& decoderName : decoderNames)
			{
				Log::info() << "Testing library '" << decoderName << "'";

				for (const bool useLoop : {false, true})
				{
					const Media::MovieRef movie = Media::Manager::get().newMedium(file(), decoderName, Media::Medium::MOVIE, true /*useExclusive*/);

					if (movie.isNull())
					{
						allSucceeded = false;

						continue;
					}

					if (movie->library() != decoderName)
					{
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;

						continue;
					}

					// we need to ensure that we are guaranteed to receive each individual frame
					if (!movie->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE))
					{
						allSucceeded = false;

						continue;
					}

					if (!movie->setLoop(useLoop))
					{
						allSucceeded = false;

						continue;
					}

					Media::FrameMedium::FrameReceiver frameReceiver;

					const Media::Movie::FrameCallbackScopedSubscription scopedFrameCallbackSubscription = movie->addFrameCallback(std::bind(&Media::FrameMedium::FrameReceiver::onFrame, &frameReceiver, std::placeholders::_1, std::placeholders::_2));

					if (!movie->start())
					{
						allSucceeded = false;

						continue;
					}

					unsigned int frameCounter = 0u;

					Timestamp lastFrameTimestamp(true);
					bool movieTaskFinished = false;
					
					while (!movieTaskFinished)
					{
						// If playback is finished, set loop to break after checking for frame that may have been generated after last check for new frames.
						movieTaskFinished = movie->taskFinished();

						Frame frame;
						if (frameReceiver.latestFrameAndReset(frame))
						{
							lastFrameTimestamp.toNow();

							const unsigned int expectedFrameIndex = frameCounter % numberFrames;

							const Timestamp expectedRelativeTimestamp(double(expectedFrameIndex) / fps);

							if (NumericD::isNotEqual(double(expectedRelativeTimestamp), double(frame.relativeTimestamp()), 0.01))
							{
								allSucceeded = false;
							}

							Frame rgbFrame;
							if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
							{
								ocean_assert(false && "This should never happen!");

								allSucceeded = false;
							}

							unsigned int parsedFrameIndex = (unsigned int)(-1);
							if (MovieVerifier::parseFrameIndex(rgbFrame, parsedFrameIndex))
							{
								if (expectedFrameIndex != parsedFrameIndex)
								{
									allSucceeded = false;
								}
							}
							else
							{
								allSucceeded = false;
							}

							if (frameCounter / numberFrames >= 3u)
							{
								// we have seen enough loops

								break;
							}

							++frameCounter;
						}
						else
						{
							constexpr double timeout = 5.0;

							if (lastFrameTimestamp + timeout < Timestamp(true))
							{
								// we did not receive any frames anymore

								allSucceeded = false;
							}
						}
					}

					if (movieTaskFinished)
					{
						if (useLoop)
						{
							// the movie was supposed to loop, so we don't expect it to ever finish
							allSucceeded = false;
						}
					}
					else
					{
						if (!useLoop && frameCounter != numberFrames)
						{
							// not using loop so number of frame retrieved should match number of frames in file
							allSucceeded = false;
						}
					}

					movie->stop();
					frameReceiver.reset();
				}
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "All encode/decode tests succeeded.";
	}
	else
	{
		Log::info() << "All encode/decode tests FAILED!";
	}

	return allSucceeded;
}

bool TestMovie::testPause()
{
	Log::info() << "Pause test:";
	Log::info() << " ";

	const std::vector<std::string> encoderNames(libraryNamesEncoder());
	const std::vector<std::string> decoderNames(libraryNamesDecoder());

	bool allSucceeded = true;

	if (encoderNames.empty())
	{
		Log::info() << "Skipping encode/decode test as no encoder exists.";
		return true;
	}

	const IO::ScopedDirectory directory(IO::Directory::createTemporaryDirectory());

	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	constexpr unsigned int numberFrames = 40u;
	constexpr double fps = 30.0;

	if (directory.exists())
	{
		const IO::File file = writeMovie(directory, width, height, numberFrames, fps, encoderNames.front());

		if (file.exists())
		{
			for (const std::string& decoderName : decoderNames)
			{
				Log::info() << "Testing library '" << decoderName << "'";

				const Media::MovieRef movie = Media::Manager::get().newMedium(file(), decoderName, Media::Medium::MOVIE, true /*useExclusive*/);

				if (movie.isNull())
				{
					allSucceeded = false;

					continue;
				}

				if (movie->library() != decoderName)
				{
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;

					continue;
				}

				// we need to ensure that we are guaranteed to receive each individual frame
				if (!movie->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE))
				{
					allSucceeded = false;

					continue;
				}

				if (!movie->setLoop(false))
				{
					allSucceeded = false;

					continue;
				}

				Media::FrameMedium::FrameReceiver frameReceiver;

				const Media::Movie::FrameCallbackScopedSubscription scopedFrameCallbackSubscription = movie->addFrameCallback(std::bind(&Media::FrameMedium::FrameReceiver::onFrame, &frameReceiver, std::placeholders::_1, std::placeholders::_2));

				if (!movie->start())
				{
					allSucceeded = false;

					continue;
				}

				unsigned int frameIndex = 0u;

				bool movieWasPaused = false;
				unsigned int pausedFrameIndex = (unsigned int)(-1);

				Timestamp lastFrameTimestamp(true);

				while (true)
				{
					if (frameIndex == numberFrames)
					{
						break;
					}

					if (!movieWasPaused && frameIndex == numberFrames / 2u)
					{
						// we are in the middle of the movie, so we pause it

						if (movie->startTimestamp().isInvalid() || movie->pauseTimestamp().isValid() || movie->stopTimestamp().isValid())
						{
							allSucceeded = false;
						}

						if (movie->pause())
						{
							movieWasPaused = true;
							pausedFrameIndex = frameIndex;

							if (movie->startTimestamp().isValid() || movie->pauseTimestamp().isInvalid() || movie->stopTimestamp().isValid())
							{
								allSucceeded = false;
							}
						}
						else
						{
							allSucceeded = false;
						}
					}

					Frame frame;
					if (frameReceiver.latestFrameAndReset(frame))
					{
						if (pausedFrameIndex != (unsigned int)(-1) && frameIndex > pausedFrameIndex)
						{
							// we allow to receive some few additional frame after pausing the movie - to drain the pipeline

							constexpr unsigned int frameTolerance = 10u;

							if (frameIndex > pausedFrameIndex + frameTolerance)
							{
								allSucceeded = false;
								break;
							}
						}

						lastFrameTimestamp.toNow();

						const Timestamp expectedRelativeTimestamp(double(frameIndex) / fps);

						if (NumericD::isNotEqual(double(expectedRelativeTimestamp), double(frame.relativeTimestamp()), 0.01))
						{
							Log::debug() << "Invalid relative timestamp: " << double(frame.relativeTimestamp()) << ", expected: " << double(expectedRelativeTimestamp);

							allSucceeded = false;
						}

						Frame rgbFrame;
						if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
						{
							ocean_assert(false && "This should never happen!");

							allSucceeded = false;
						}

						unsigned int parsedFrameIndex = (unsigned int)(-1);
						if (MovieVerifier::parseFrameIndex(rgbFrame, parsedFrameIndex))
						{
							if (frameIndex != parsedFrameIndex)
							{
								Log::debug() << "Invalid frame index: " << parsedFrameIndex << ", expected: " << frameIndex;

								allSucceeded = false;
							}
						}
						else
						{
							allSucceeded = false;
						}

						++frameIndex;
					}
					else
					{
						constexpr double timeout = 5.0;

						if (lastFrameTimestamp + timeout < Timestamp(true))
						{
							// we did not receive any frames anymore

							if (movieWasPaused)
							{
								if (pausedFrameIndex == (unsigned int)(-1))
								{
									allSucceeded = false;
								}

								if (movie->start())
								{
									pausedFrameIndex = (unsigned int)(-1);

									if (movie->startTimestamp().isInvalid() || movie->pauseTimestamp().isValid() || movie->stopTimestamp().isValid())
									{
										allSucceeded = false;
									}
								}
								else
								{
									allSucceeded = false;
								}

								lastFrameTimestamp.toNow();
							}
							else
							{
								allSucceeded = false;
							}
						}
					}
				}
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "All encode/decode tests succeeded.";
	}
	else
	{
		Log::info() << "All encode/decode tests FAILED!";
	}

	return allSucceeded;
}

void TestMovie::registerMediaLibraries()
{
#ifndef OCEAN_RUNTIME_SHARED
	#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
		Media::AVFoundation::registerAVFLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		Media::AVFoundation::registerAVFLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_LINUX) and !defined(OCEAN_DISABLE_FFMPEG_IN_CMAKE)
		Media::FFmpeg::registerFFmpegLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
		Media::Android::registerAndroidLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)
		Media::MediaFoundation::registerMediaFoundationLibrary();
	#endif
#endif // OCEAN_RUNTIME_SHARED
}

void TestMovie::unregisterMediaLibraries()
{
#ifndef OCEAN_RUNTIME_SHARED
	#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
		Media::AVFoundation::unregisterAVFLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		Media::AVFoundation::unregisterAVFLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_LINUX) and !defined(OCEAN_DISABLE_FFMPEG_IN_CMAKE)
		Media::FFmpeg::unregisterFFmpegLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
		Media::Android::unregisterAndroidLibrary();
	#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
	#endif
#endif // OCEAN_RUNTIME_SHARED
}

std::vector<std::string> TestMovie::libraryNamesEncoder()
{
	std::vector<std::string> encoderNames;

#if defined(OCEAN_PLATFORM_BUILD_APPLE)
	encoderNames.emplace_back(Media::AVFoundation::nameAVFLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	encoderNames.emplace_back(Media::MediaFoundation::nameMediaFoundationLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID) && defined(__ANDROID_API__) && __ANDROID_API__ >= 24
	encoderNames.emplace_back(Media::Android::nameAndroidLibrary());
#endif

	return encoderNames;
}

std::vector<std::string> TestMovie::libraryNamesDecoder()
{
	std::vector<std::string> decoderNames;

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
	decoderNames.emplace_back(Media::AVFoundation::nameAVFLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
	decoderNames.emplace_back(Media::AVFoundation::nameAVFLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_LINUX) and !defined(OCEAN_DISABLE_FFMPEG_IN_CMAKE)
	decoderNames.emplace_back(Media::FFmpeg::nameFFmpegLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	decoderNames.emplace_back(Media::MediaFoundation::nameMediaFoundationLibrary());
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24
		decoderNames.emplace_back(Media::Android::nameAndroidLibrary());
	#endif
#endif

	return decoderNames;
}

IO::File TestMovie::writeMovie(const IO::Directory& directory, const unsigned int width, const unsigned int height, const unsigned int numberFrames, const double fps, const std::string& libraryName)
{
	ocean_assert(directory.exists());
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(numberFrames > 0u);
	ocean_assert(fps > 0.0);

	const Media::MovieRecorderRef movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER, libraryName);

	if (movieRecorder.isNull())
	{
		return IO::File();
	}

	std::string filename = "movie_" + libraryName + "_" + String::toAString(width) + "x" + String::toAString(height) + "_" + String::toAString(fps, 2u) + "_" + String::toAString(uniqueId(), 5u) + ".mp4";

	IO::File file(directory + IO::File("movie.mp4"));

	if (!movieRecorder->setFilename(file()))
	{
		return IO::File();
	}

	constexpr unsigned int maximalNumberFrames = 1u << (numberBits_ - 1u);

	ocean_assert(numberFrames <= maximalNumberFrames);
	if (numberFrames > maximalNumberFrames)
	{
		return IO::File();
	}

	assert(numberBits_ * bitSize_ <= width && bitSize_ <= height);
	if (numberBits_ * bitSize_ > width || bitSize_ > height)
	{
		return IO::File();
	}

	const FrameType frameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

	if (!movieRecorder->setPreferredFrameType(frameType))
	{
		return IO::File();
	}

	if (!movieRecorder->setFrameFrequency(fps))
	{
		return IO::File();
	}

	if (!movieRecorder->start())
	{
		return IO::File();
	}

	Frame inputFrame(frameType);
	Frame recorderFrame;

	for (unsigned int frameIndex = 0u; frameIndex < numberFrames; ++frameIndex)
	{
		constexpr bool respectFrameFrequency = false;

		if (!movieRecorder->lockBufferToFill(recorderFrame, respectFrameFrequency))
		{
			return IO::File();
		}

		ocean_assert(recorderFrame.isValid());

		inputFrame.setValue(0xFF);

		for (unsigned int bitIndex = 0u; bitIndex < numberBits_; ++bitIndex)
		{
			if (frameIndex & (1u << bitIndex))
			{
				constexpr uint8_t color = 0x00;

				inputFrame.subFrame(bitIndex * bitSize_, 0u, bitSize_, bitSize_).setValue(color);
			}
		}

		int y = int(bitSize_) + 25;

		CV::Canvas::drawText(inputFrame, "Encoder: " + libraryName, 5, y, CV::Canvas::red());
		CV::Canvas::drawText(inputFrame, "Frame index: " + String::toAString(frameIndex), 5, y += 20, CV::Canvas::green());
		CV::Canvas::drawText(inputFrame, "Frames per second: " + String::toAString(fps, 2u), 5, y += 20, CV::Canvas::blue());

		if (height >= yColorStrip_ + bitSize_)
		{
			inputFrame.subFrame(0u, yColorStrip_, width, bitSize_).setValue(uniqueColor(frameIndex), 3u);
		}

		if (!CV::FrameConverter::Comfort::convertAndCopy(inputFrame, recorderFrame))
		{
			ocean_assert(false && "This should never happen!");

			return IO::File();
		}

		movieRecorder->unlockBufferToFill();
	}

	if (!movieRecorder->stop())
	{
		return IO::File();
	}

	return file;
}

bool TestMovie::readMovie(const IO::File& file, const unsigned int width, const unsigned int height, const unsigned int numberFrames, const double fps, const std::string& libraryName)
{
	ocean_assert(file.exists());
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(numberFrames > 0u);
	ocean_assert(fps > 0.0);

	const Media::MovieRef movie = Media::Manager::get().newMedium(file(), libraryName, Media::Medium::MOVIE, true /*useExclusive*/);

	if (movie.isNull())
	{
		return false;
	}

	if (movie->library() != libraryName)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!movie->setSpeed(Media::Movie::AS_FAST_AS_POSSIBLE))
	{
		return false;
	}

	MovieVerifier movieVerifier(width, height, numberFrames, fps);

	const Media::Movie::FrameCallbackScopedSubscription scopedFrameCallbackSubscription = movie->addFrameCallback(std::bind(&MovieVerifier::onFrame, &movieVerifier, std::placeholders::_1, std::placeholders::_2));

	if (!movie->start())
	{
		return false;
	}

	while (true)
	{
		if (movie->taskFinished())
		{
			break;
		}

		constexpr double timeout = 5.0;

		if (Timestamp(true) > movieVerifier.lastUpdateTimestamp() + timeout)
		{
			return false;
		}
	}

	if (!movieVerifier.succeeded())
	{
		return false;
	}

	return true;
}

unsigned int TestMovie::uniqueId()
{
	static Lock lock;

	const ScopedLock scopedLock(lock);

	static unsigned int idCounter = 0u;

	return ++idCounter;
}

const uint8_t* TestMovie::uniqueColor(const unsigned int frameIndex)
{
	constexpr unsigned int numberColors = 3u;

	static constexpr uint8_t colors[numberColors][3] =
	{
		{0xFF, 0x00, 0x00},
		{0x00, 0xFF, 0x00},
		{0x00, 0x00, 0xFF}
	};

	return colors[frameIndex % numberColors];
}

}

}

}
