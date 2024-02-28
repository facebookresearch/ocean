// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/network/PackagedTCPClient.h"
#include "ocean/network/PackagedUDPClient.h"
#include "ocean/network/Resolver.h"

#include "ocean/media/Utilities.h"
#include "ocean/media/Manager.h"
#include "ocean/media/FrameMedium.h"
#include "ocean/media/PixelImage.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Text.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace XRPlayground
{
/**
 * This experience shows how to access the cameras on Oculus platforms.
 * @ingroup xrplayground
 */
class OculusCameraTuringFeed : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~OculusCameraTuringFeed() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:
		/// The rendering Scene object holding the experience.
		Rendering::TransformRef renderingScene_;

		/// The rendering Text node providing user feedback.
		Rendering::TextRef renderingText_;

		/// The rendering Transform objects holding the individual cameras.
		std::vector<Rendering::TransformRef> renderingTransforms_;

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		Rendering::TransformRef renderingTransform_;
		/// Experience's lock.
		Lock lock_;

		/// Thead that sends frame data to host
		class NetThread: protected Thread {
			constexpr static double kFrameIntervalOverhead_ = 0.013;
#ifdef TETRIS_FPS
	#if (TETRIS_FPS == 10)
			constexpr static double kFrameInterval_ = 0.100 - kFrameIntervalOverhead_;
	#elif (TETRIS_FPS == 20)
			constexpr static double kFrameInterval_ = 0.050 - kFrameIntervalOverhead_;
	#elif (TETRIS_FPS == 30)
			constexpr static double kFrameInterval_ = 0.0333 - kFrameIntervalOverhead_;
	#elif (TETRIS_FPS == 60)
			constexpr static double kFrameInterval_ = 0.0167 - kFrameIntervalOverhead_;
	#else
			constexpr static double kFrameInterval_ = 0.100 - kFrameIntervalOverhead_;
	#endif
#else
			constexpr static double kFrameInterval_ = 0.100 - kFrameIntervalOverhead_;
#endif
			constexpr static size_t kMaxBufferSize = 1024 * 1024;

			Timestamp lastTimestamp_; // The timestamp of the last camera frame which has been streamed
			bool isFrameReady_;
			uint8_t frameBuffer_[kMaxBufferSize];
			size_t frameSize_;
			Lock lock_; // mutex to protect variables amon threads
			Network::PackagedTCPClient tcpClient_;

			bool isStop_;
			// main function
			void threadRun() override;

			size_t fpsNumFrame_;
			double fpsLastTimestamp_;

		public:
			// start the thread
			void start();

			// stop the thread
			void stop();

			// send
			void newFrame(const void* data, const size_t size, const Timestamp timestamp);

			// callback when received
			void onReceiveFromHostTCP(const void* data, const size_t size);
			Timestamp lastRecvTimestamp_; // The timestamp of the last camera frame which has been received
			volatile bool isRecvFrameReady_;
			uint8_t recvFrameBuffer_[kMaxBufferSize];
			size_t recvSoFar;
			Frame *recentRecvFrame_;
			/// The pixel image which will hold the received camera stream.
			Media::PixelImageRef pixelImage_;
		};

		/// A separate thread for network operations
		std::shared_ptr<NetThread> netThread_;

};

}

}
