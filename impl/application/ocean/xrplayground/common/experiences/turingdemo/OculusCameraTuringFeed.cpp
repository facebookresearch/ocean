// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/turingdemo/OculusCameraTuringFeed.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/Lock.h"
#include "ocean/media/Utilities.h"

#include "ocean/network/Resolver.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

OculusCameraTuringFeed::~OculusCameraTuringFeed()
{
	// nothing to do here
}

bool OculusCameraTuringFeed::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	Log::info() << "> [TETRIS] OculusCameraTuringFeed::load()";

	renderingScene_ = engine->factory().createScene();

	engine->framebuffers().front()->addScene(renderingScene_);

	bool allCamerasAccessed = true;

	Log::info() << "* [TETRIS] OculusCameraTuringFeed::load(): create thread for network transfer";
	netThread_ = std::make_shared<NetThread>();
	netThread_->start();

	for (unsigned int cameraIndex = 0u; cameraIndex < 4u; ++cameraIndex)
	{
		const std::string mediumUrl = "LiveVideoId:" + String::toAString(cameraIndex);

		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium)
		{
			frameMedium->start();

			Rendering::TransformRef renderingTransform = Rendering::Utilities::createBox(engine, Vector3(Scalar(1), Scalar(1), 0), frameMedium);
			renderingTransform->setVisible(false);

			renderingScene_->addChild(renderingTransform);

			renderingTransforms_.emplace_back(std::move(renderingTransform));
			frameMediums_.emplace_back(std::move(frameMedium));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	if (!allCamerasAccessed)
	{
		std::string message = " Failed to access all cameras \n see https://fburl.com/access_cameras ";

		const Rendering::TransformRef transform = Rendering::Utilities::createText(*engine, message, RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
		transform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));

		renderingScene_->addChild(transform);
	}

	// This creates the 1 box for return. (1 rendering transform created here; we do this only once at the beginning)
	{
		// Create a pixelImage (subclass of FrameMedium) to use as a box texture for the returned
		// camera data.
		netThread_->pixelImage_ = Media::Manager::get().newMedium("PIXEL_IMAGE", Media::Medium::PIXEL_IMAGE);
		if(!netThread_->pixelImage_) {
			Log::error() << "* [TETRIS] pixelImage_ not allocated";
			ocean_assert(false);
		}
		// Create identity matrix for pixelImage transform (PixelImage.cpp says it must be set)
		HomogenousMatrixD4 device_T_camera2 = HomogenousMatrixD4(true);
		// Try various translations to see if the Box appears...
		device_T_camera2.setTranslation(VectorD3(0.0,0.0,-2.0));
		netThread_->pixelImage_->setDevice_T_camera(device_T_camera2);
		// Note that we don't actually use the pixelImage_->getDevice_T_camera below in the rendering,
		// but instead create a totally static transform matrix out of thin air.  That one works with
		// the plain untextured box, but the textured box with pixelimage is still not visible.

		const std::string mediumUrl = "LiveReturnVideoId:";

		netThread_->pixelImage_->start();

		// Create the recentRecvFrame_ so we can jam returned network data into it, then
		// set it as the pixelImage's frame
		netThread_->recentRecvFrame_ = new Frame(FrameType(640,480,FrameType::FORMAT_Y8,FrameType::ORIGIN_UPPER_LEFT),netThread_->recvFrameBuffer_,Frame::CM_USE_KEEP_LAYOUT);
		if(!netThread_->recentRecvFrame_) {
			Log::error() << "* [TETRIS] recentRecvFrame_ not allocated";
			ocean_assert(false);
		}
		// VERY IMPORTANT - the pixelImage won't accept a frame unless it has a valid timestamp.
		netThread_->recentRecvFrame_->setTimestamp(timestamp);
		if(!netThread_->pixelImage_->setPixelImage(*netThread_->recentRecvFrame_)) {
			Log::info() << "< [TETRIS] OculusCameraTuringFeed::load() - failed setPixelImage";
			ocean_assert(false);
		}
		// This plain untextured box is used to test the transforms in the preUpdate() rendering pass
		// below.  The plain box appears exactly where expected.
		//renderingTransform_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(1), Scalar(1), 0));
		// This textured box doesn't seem to appear at all...
		renderingTransform_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(1), Scalar(1), 0), netThread_->pixelImage_);
		if(!renderingTransform_) {
			Log::info() << "< [TETRIS] OculusCameraTuringFeed::load() - renderingTransform_";
			ocean_assert(false);
		}
		renderingTransform_->setVisible(false);
		renderingScene_->addChild(renderingTransform_);  // add transform box to the scene
	}

	Log::info() << "< [TETRIS] OculusCameraTuringFeed::load()";
	return true;
}

bool OculusCameraTuringFeed::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	Log::info() << "> [TETRIS] OculusCameraTuringFeed::unload()";

	ocean_assert(engine);

	engine->framebuffers().front()->removeScene(renderingScene_);

	frameMediums_.clear();

	renderingTransforms_.clear();
	renderingScene_.release();

	Log::info() << "* [TETRIS] OculusCameraTuringFeed::unload() stop netThread";
	netThread_->stop();

	Log::info() << "< [TETRIS] OculusCameraTuringFeed::unload()";
	return true;
}

Timestamp OculusCameraTuringFeed::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	Log::debug() << "> [TETRIS] OculusCameraTuringFeed::preUpdate() Line: "<<  __LINE__ << "  " << frameMediums_.size();

	HomogenousMatrixD4 device_T_camera;

	FrameRef frame;
	for (size_t n = 0; n < renderingTransforms_.size(); ++n)
	{
		const Rendering::TransformRef& renderingTransform = renderingTransforms_[n];
		const Media::FrameMediumRef& frameMedium = frameMediums_[n];

		frame = frameMedium->frame(); // latest frame

		if (frame)
		{
			device_T_camera = frameMedium->device_T_camera();   // set camera
			device_T_camera = HomogenousMatrixD4(device_T_camera.translation() * 12.5, device_T_camera.rotationMatrix()); // increasing the translational part

			const Scalar factor = Scalar(0.9) / std::max(frame->width(), frame->height());  // set scale factor

			const Scalar scaleX = Scalar(frame->width()) * factor;		// set scale x and y
			const Scalar scaleY = Scalar(frame->height()) * factor;

			HomogenousMatrix4 transformation = HomogenousMatrix4(Vector3(0, 0, -1)) * HomogenousMatrix4(device_T_camera);  // Create a transform Matrix
			transformation.applyScale(Vector3(scaleX, scaleY, Scalar(1)));                // modidy transform matrix according to scale

			renderingTransform->setTransformation(transformation);		// Use the transform matrix
			renderingTransform->setVisible(true);						// make it visible

			Log::debug() << "* [TETRIS] OculusCameraTuringFeed::preUpdate(): Line: "<<  __LINE__<< " size: " << frame->size() << " n=" << n;
			if (n == 0) {
				netThread_->newFrame(frame->constdata<void>(), frame->size(), timestamp);  // copies frame and sets flag to copy to data structure which is sent via TCP port
			}
		}
	}

	frame = netThread_->pixelImage_->frame();
	if (netThread_->isRecvFrameReady_)
	{ // for output frame
		Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ << "isRecvFrameReady: " << netThread_->isRecvFrameReady_ ;

		// Grab the renderingTransform_ for box textured with the image returned from the network
		const Rendering::TransformRef& renderingTransform = renderingTransform_;
		Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ ;

		if (netThread_->pixelImage_)
		{
			Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ ;

	        // We'll skip getting the pixelImage's device_T_camera and instead create a fixed transform here
			//returnImageT = netThread_->pixelImage_->device_T_camera();   // set camera
			HomogenousMatrix4 returnImageT = HomogenousMatrix4(Vector3(0, 1, -2));

			Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ ;

			// Reshape the box to fit the image size
			const Scalar factor = Scalar(0.9) / std::max(frame->width(), frame->height());
			const Scalar scaleX = Scalar(frame->width()) * factor;
			const Scalar scaleY = Scalar(frame->height()) * factor;
			returnImageT.applyScale(Vector3(scaleX, scaleY, Scalar(1)));
			Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ ;

			renderingTransform->setTransformation(returnImageT);

			// VERY IMPORTANT: Update the timestamp...
			netThread_->recentRecvFrame_->setTimestamp(timestamp);

			Log::debug() << "* [TETRIS] OculusCameraTuringFeed::preUpdate(): set the frame to the internally stored buffer. ";
			// Unclear whether forcing the new frame back into the pixelImage's frame member
			// does anything.  One theory: doing so invalidates the texture so the GPU
			// would reload it?
			netThread_->pixelImage_->setPixelImage(*netThread_->recentRecvFrame_);
			//const ScopedLock scopedLock(lock_);
			renderingTransform->setVisible(true);

			netThread_->isRecvFrameReady_ = false;
			Log::debug() << "[TETRIS] OculusCameraTuringFeed::preUpdate(): line "<< __LINE__ << " isRecvFrameReady: " << netThread_->isRecvFrameReady_ ;

		}
	}


	Log::debug() << "< [TETRIS] OculusCameraTuringFeed::preUpdate() line:" << __LINE__ ;
	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> OculusCameraTuringFeed::createExperience()
{
	Log::info() << "> [TETRIS] OculusCameraTuringFeed::createExperience()";
	return std::unique_ptr<XRPlaygroundExperience>(new OculusCameraTuringFeed());
}

void OculusCameraTuringFeed::NetThread::threadRun()
{
	Log::info() << "> [TETRIS] NetThread::threadRun()";

	while (true) {
		if (isStop_)
		{
			Log::info() << "* [TETRIS] NetThread::threadRun(): stop";
			break;
		}

		if (!isFrameReady_)
		{
			// Log::debug() << "* [TETRIS] NetThread::threadRun(): frame is NOT ready, sleep 1ms";
			Thread::sleep(1);
			continue;
		}

		if (!tcpClient_.isConnected())
		{
			Log::error() << "* [TETRIS] NetThread::threadRun(): TCP is NOT connected";
			break;
		}

		if (tcpClient_.send(frameBuffer_, frameSize_) == Network::Socket::SR_SUCCEEDED)
		{
			isFrameReady_ = false;
			Log::debug() << "* [TETRIS] NetThread::threadRun(): TCP send SUCCESS";
		}
		else
		{
			Log::error() << "* [TETRIS] NetThread::threadRun(): TCP send FAIL";
			break;
		}
	}

	Log::info() << "< [TETRIS] NetThread::threadRun()";
}

void OculusCameraTuringFeed::NetThread::start()
{
	Log::info() << "> [TETRIS] NetThread::start()";

	TemporaryScopedLock scopedLock(lock_);
	lastTimestamp_ = Timestamp(0.0);
	isFrameReady_ = false;
	isStop_ = false;
	scopedLock.release();
	fpsNumFrame_ = 0;
	fpsLastTimestamp_ = 0.0;

	if (tcpClient_.isConnected())
	{
		Log::warning() << "* [TETRIS] NetThread::start(): tcpClient_ is already connected, try to disconnect it";
		tcpClient_.disconnect();
		ocean_assert(!tcpClient_.isConnected());
	}

	const auto& address = Network::Resolver::resolveFirstIp4("localhost");
	//const auto& address = Network::Resolver::resolveFirstIp4("192.168.0.165");
	const auto& port = Network::Port(6000, Network::Port::TYPE_READABLE);
	Log::info() << "* [TETRIS] NetThread::start(): connecting to server address=" << address.readable() << " port=" << port.readable();

	Log::info() << "* [TETRIS] NetThread::start(): set receive callback";
	const Network::Resolver::Addresses4 localAddresses = Network::Resolver::get().localAddresses();
	if(!localAddresses.empty()) {
		tcpClient_.setReceiveCallback(Network::TCPClient::ReceiveCallback::create(*this, &OculusCameraTuringFeed::NetThread::onReceiveFromHostTCP));
		Log::info() << "* [TETRIS] NetThread::start(): set receive callback - done";
	}

	if (tcpClient_.connect(address, port))
	{
		Log::info() << "* [TETRIS] NetThread::start(): connect SUCCESS";
	}
	else
	{
		Log::error() << "* [TETRIS] NetThread::start(): connect FAIL";
		ocean_assert(false);
	}

	Log::info() << "* [TETRIS] NetThread::start(): start thread";
	startThread();

	Log::info() << "< [TETRIS] NetThread::start()";
}

void OculusCameraTuringFeed::NetThread::stop()
{
	Log::info() << "> [TETRIS] NetThread::stop()";

	TemporaryScopedLock scopedLock(lock_);
	isStop_ = true;
	if (tcpClient_.isConnected())
	{
		tcpClient_.disconnect();
	}
	stopThread();
	scopedLock.release();

	Log::info() << "< [TETRIS] NetThread::stop()";
}

void OculusCameraTuringFeed::NetThread::newFrame(const void* data, const size_t size, const Timestamp timestamp)
{
	Log::debug() << "> [TETRIS] NetThread::newFrame() size=" << size << " timestamp=" << double(timestamp);
	ocean_assert(size <= kMaxBufferSize);

	double interval = double(timestamp - lastTimestamp_);

	if (interval >= kFrameInterval_)
	{
		Log::debug() << "* [TETRIS] NetThread::newFrame(): new frame, interval=" << interval;

		if (isFrameReady_)
		{
			Log::warning() << "* [TETRIS] NetThread::newFrame(): last frame is still there";
			return;
		}

		Log::debug() << "* [TETRIS] NetThread::newFrame(): memcpy";
		TemporaryScopedLock scopedLock(lock_);
		std::memcpy(frameBuffer_, data, size);
		frameSize_ = size;
		isFrameReady_ = true;
		lastTimestamp_ = timestamp;
		scopedLock.release();

		++fpsNumFrame_;
		if (fpsNumFrame_ % 32 == 0)
		{
			if (fpsLastTimestamp_ != 0.0)
			{
				const double fps = 32.0 / double(timestamp - fpsLastTimestamp_);
				Log::info() << "* [TETIRS] NetThread::newFrame(): fps=" << fps;
			}
			fpsLastTimestamp_ = double(timestamp);
		}
	}
	else
	{
		Log::debug() << "* [TETRIS] NetThread::newFrame(): skip frame because lastTimestamp=" << double(lastTimestamp_) << " interval=" << double(timestamp - lastTimestamp_) << " < " << kFrameInterval_;
	}

	Log::debug() << "< [TETRIS] NetThread::newFrame()";
}

// This is the callback for when we receicve data, we want to receive a frame's worth, and then display back.
// Debug learing.  This guy only gets called, when the Packaged TCP bundle is complete.

void OculusCameraTuringFeed::NetThread::onReceiveFromHostTCP(const void* data, const size_t size)
{
	Log::debug() << "> [TETRIS] NetThread::onReceiveFromHostTCP size=" << size;
	if (size == 0)
	{
		return;
	}
	TemporaryScopedLock scopedLock(lock_);

	// When the data for the frame has arrived, copy it into the recentRecvFrame_ buffer
	uint8_t *frameBuf = recentRecvFrame_->data<uint8_t>();
	std::memcpy(frameBuf, data, size);
	recvSoFar += 1;

	isRecvFrameReady_ = true;

	scopedLock.release();
	Log::debug() << "> [TETRIS] NetThread::onReceiveFromHostTCP: isRecvFrameReady_ =" << isRecvFrameReady_  << "    ";

	return;
}


} // namespace XRPlayground

} // namespace Ocean
