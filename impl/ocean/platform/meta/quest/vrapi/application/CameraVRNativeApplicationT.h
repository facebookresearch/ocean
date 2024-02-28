// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/base/Frame.h"
#include "ocean/base/Thread.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SophusUtilities.h"

#include "ocean/platform/meta/quest/sensors/FrameProvider.h"

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

/**
 * This class implements a basic Quest (VR) application which also has access to the cameras of the Quest device.
 * @ingroup platformmetaquestvrapiapplication
 */
template <typename TBaseApplication>
class CameraVRNativeApplicationT : public TBaseApplication
{
	friend class CameraThread;

	public:

		using FrameMetadata = Sensors::FrameProvider::FrameMetadata;
		using FrameMetadatas = Sensors::FrameProvider::FrameMetadatas;
		using CameraType = Sensors::FrameProvider::CameraType;

		/**
		 * Definition of individual methods to handle the camera images.
		 */
		enum CameraHandling : uint32_t
		{
			/// The camera handling is unknown.
			CH_UNKNOWN = 0u,
			/// The camera handling is synchronous (will happen in the main loop).
			CH_SYNCRONOUS,
			/// The camera handling is asynchronous (will happen in a dedicated thread).
			CH_ASYNCRONOUS
		};

	protected:

		/**
		 * The thread object in case `CH_ASYNCRONOUS` is used.
		 */
		class CameraThread : private Thread
		{
			public:

				/**
				 * Starts the thread.
				 * @param owner The owner of the new thread, i.e. the `CameraVRNativeApplicationT` object, must be valid
				 */
				void startThread(CameraVRNativeApplicationT* owner);

			protected:

				/**
				 * The thread function in which the camera frames will be handled, if `desiredCameraHandling()` returns CH_ASYNCRONOUS.
				 * @see Thread::threadRun().
				 */
				void threadRun() override;

			protected:

				/// The owner of the thread.
				CameraVRNativeApplicationT* owner_ = nullptr;
		};

		/**
		 * This class implements a container for the camera image data from multiple cameras.
		 */
		class MultiCameraData
		{
			public:

				/**
				 * Default constructor.
				 */
				MultiCameraData();

				/**
				 * Updates the camera data, image data will be copied.
				 * @param cameras The camera profiles of the cameras
				 * @param frames The camera images
				 * @param world_T_device The transformation from device to world
				 * @param device_T_cameras The transformation from each camera to the device
				 */
				void update(SharedAnyCamerasD&& cameras, Frames&& frames, const HomogenousMatrixD4& world_T_device, HomogenousMatricesD4&& device_T_cameras);

				/**
				 * Returns the last camera data.
				 * @param cameras Optional resulting camera profiles of the cameras, nullptr if not of interest
				 * @param frames Optional resulting camera images, nullptr if not of interest
				 * @param world_T_device Optional resulting transformation from device to world, nullptr if not of interest
				 * @param device_T_cameras Optional resulting transformation from each camera to the device, nullptr if not of interest
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(SharedAnyCamerasD* cameras, FrameRefs* frames, HomogenousMatrixD4* world_T_device, HomogenousMatricesD4* device_T_cameras, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the last camera data.
				 * @param cameras Optional resulting camera profiles of the cameras, nullptr if not of interest
				 * @param frames Optional resulting camera images, nullptr if not of interest
				 * @param world_T_device Optional resulting transformation from device to world, nullptr if not of interest
				 * @param device_T_cameras Optional resulting transformation from each camera to the device, nullptr if not of interest
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(SharedAnyCamerasF* cameras, FrameRefs* frames, HomogenousMatrixF4* world_T_device, HomogenousMatricesF4* device_T_cameras, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the last camera data.
				 * @param frames Optional resulting camera images, nullptr if not of interest
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(FrameRefs* frames, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the timestamp of the latest frames.
				 * @return The frame's timestamp, an invalid timestamp if currently no latest frames exist
				 */
				inline Timestamp latestTimestamp() const;

			protected:

				/// The camera profiles of the cameras.
				SharedAnyCamerasD cameras_;

				/// The images of each camera
				FrameRefs frames_;

				/// The transformation between device and world.
				HomogenousMatrixD4 world_T_device_;

				/// The transformations between the device and cameras.
				HomogenousMatricesD4 device_T_cameras_;

				/// The lock object.
				mutable Lock lock_;
		};

		/**
		 * This class implements a container for the camera image data.
		 */
		class StereoCameraData
		{
			public:

				/**
				 * Default constructor.
				 */
				StereoCameraData();

				/**
				 * Updates the camera data, image data will be copied.
				 * @param cameraA The camera profile of the first camera
				 * @param cameraB The camera profile of the second camera
				 * @param frameA The first camera image
				 * @param frameB The second camera image
				 * @param world_T_device The transformation from device to world
				 * @param device_T_cameraA The transformation from the first camera to the device
				 * @param device_T_cameraB The transformation from the second camera to the device
				 */
				void update(const SharedAnyCameraD& cameraA, const SharedAnyCameraD& cameraB, Frame&& frameA, Frame&& frameB, const HomogenousMatrixD4& world_T_device, const HomogenousMatrixD4& device_T_cameraA, const HomogenousMatrixD4& device_T_cameraB);

				/**
				 * Returns the last camera data.
				 * @param cameraA Optional resulting camera profile of the first camera
				 * @param cameraB Optional resulting camera profile of the second camera
				 * @param frameA Optional resulting first camera image
				 * @param frameB Optional resulting second camera image
				 * @param world_T_device Optional resulting transformation from device to world
				 * @param device_T_cameraA Optional resulting transformation from the first camera to the device
				 * @param device_T_cameraB Optional resulting transformation from the second camera to the device
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(SharedAnyCameraD* cameraA, SharedAnyCameraD* cameraB, FrameRef* frameA, FrameRef* frameB, HomogenousMatrixD4* world_T_device, HomogenousMatrixD4* device_T_cameraA, HomogenousMatrixD4* device_T_cameraB, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the last camera data.
				 * @param cameraA Optional resulting camera profile of the first camera
				 * @param cameraB Optional resulting camera profile of the second camera
				 * @param frameA Optional resulting first camera image
				 * @param frameB Optional resulting second camera image
				 * @param world_T_device Optional resulting transformation from device to world
				 * @param device_T_cameraA Optional resulting transformation from the first camera to the device
				 * @param device_T_cameraB Optional resulting transformation from the second camera to the device
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(SharedAnyCameraF* cameraA, SharedAnyCameraF* cameraB, FrameRef* frameA, FrameRef* frameB, HomogenousMatrixF4* world_T_device, HomogenousMatrixF4* device_T_cameraA, HomogenousMatrixF4* device_T_cameraB, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the last camera data.
				 * @param frameA Optional resulting first camera image
				 * @param frameB Optional resulting second camera image
				 * @param lastTimestamp Optional timestamp of the last frame data which has been accessed before, the function will return `False` if the current frame data is not younger, an invalid timestamp otherwise
				 * @return True, if the container stored valid data and if the image data was younger than the provided timestamp in `latestTimestamp`
				 */
				bool latest(FrameRef* frameA, FrameRef* frameB, const Timestamp& lastTimestamp = Timestamp(false)) const;

				/**
				 * Returns the timestamp of the latest frames.
				 * @return The frame's timestamp, an invalid timestamp if currently no latest frames exist
				 */
				inline Timestamp latestTimestamp() const;

			protected:

				/// The camera profile of the first camera.
				SharedAnyCameraD cameraA_;

				/// The camera profile of the second camera.
				SharedAnyCameraD cameraB_;

				/// The image of the first camera.
				FrameRef frameA_;

				/// The image of the second image.
				FrameRef frameB_;

				/// The transformation between device and world.
				HomogenousMatrixD4 world_T_device_;

				/// The transformation between first camera and device.
				HomogenousMatrixD4 device_T_cameraA_;

				/// The transformation between second camera and device.
				HomogenousMatrixD4 device_T_cameraB_;

				/// The lock object.
				mutable Lock lock_;
		};

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit CameraVRNativeApplicationT(struct android_app* androidApp);

		/**
		 * Destructs this object.
		 */
		~CameraVRNativeApplicationT() override;

		/**
		 * Starts receiving camera frames of a specific camera frame type.
		 * In case the provider is already reciving camera frames with a different type, the provider switches to the camera frame with the new type.
		 * @param cameraFrameType The type of the camera frame to be received
		 * @param cameraType The type of the cameras to be received
		 * @return True, if succeeded
		 * @see stopReceivingCameraFrames().
		 */
		inline bool startReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType cameraType = Sensors::FrameProvider::CT_ALL_CAMERAS);

		/**
		 * Stops receiving camera frames.
		 * @return True, if succeeded
		 * @see startReceivingCameraFrames().
		 */
		inline bool stopReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param nativeApplication Application object which would have been copied
		 */
		CameraVRNativeApplicationT(const CameraVRNativeApplicationT& nativeApplication) = delete;

		/**
		 * Returns the set of permissions that are required by the app and that have to be requested from the operating system
		 * @see NativeApplication::androidPermissionsToRequest()
		 */
		typename TBaseApplication::AndroidPermissionsSet androidPermissionsToRequest() override;

		/**
		 * Event function call when an Android (or Oculus) permission is granted.
		 * @see NativeApplication::onPermissionGranted().
		 */
		void onPermissionGranted(const std::string& permission) override;

		/**
		 * Renders a new frame.
		 * @see VRNativeApplication.
		 */
		void render(const double predictedDisplayTime) override;

		/**
		 * Checks if new camera images are available and handles them if so.
		 */
		virtual void checkAndHandleCameraImages();

		/**
		 * Returns the desired method to handle the cameras.
		 * Note: Override this function in a derived class to customize the behavior.
		 * @return The desired method to handle the cameras
		 */
		virtual CameraHandling desiredCameraHandling();

		/**
		 * Events function called before the scene is rendered.
		 * @param renderTimestamp The timestamp which will be used for rendering, must be valid
		 * @see VRNativeApplication::onPreRender(const Timestamp&)
		 */
		void onPreRender(const Timestamp& renderTimestamp) override;

		/**
		 * Event function call when the Android Activity is destroyed.
		 * @see NativeApplication().
		 */
		void onActivityDestroy() override;

		/**
		 * Event function called whenever new stereo camera images have arrived with Ocean-specific data types.
		 * This event function will be called if `cameraAccessMode_` is set to `CAM_BOTTOM_STEREO_CAMERAS` or `CAM_TOP_STEREO_CAMERAS`.<br>
		 * Beware: The camera to device transformations (device_T_cameraA, and device_T_cameraB) as provided in Ocean's standard coordinate system and not in the inverted flipped coordinate system (as provided by VrApi).
		 * @param anyCameraA The camera profile of the first camera
		 * @param anyCameraB The camera profile of the second camera
		 * @param frameA The first camera image
		 * @param frameB The second camera image
		 * @param world_T_device The transformation from device to world
		 * @param device_T_cameraA The transformation from the first camera to the device
		 * @param device_T_cameraB The transformation from the second camera to the device
		 * @param cameraFrameType The camera frame type of the stereo images
		 * @param cameraType The camera type of the stereo images
		 * @param captureTimestampHostClockInSeconds The capture timestamp of the images
		 * @param frameMetadataA The metadata of the first camera
		 * @param frameMetadataB The metadata of the second camera
		 */
		virtual void onStereoCameraImages(const SharedAnyCameraD& anyCameraA, const SharedAnyCameraD& anyCameraB, Frame&& frameA, Frame&& frameB, const HomogenousMatrixD4& world_T_device, const HomogenousMatrixD4& device_T_cameraA, const HomogenousMatrixD4& device_T_cameraB, const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType& cameraType, const Timestamp& captureTimestampHostClockInSeconds, const FrameMetadata& frameMetadataA, const FrameMetadata& frameMetadataB);

		/**
		 * Event function called whenever new camera images have arrived with Ocean-specific data types.
		 * This event function will be called if cameraAccessMode_` is set to `CAM_ALL_CAMERAS`.<br>
		 * Beware: The camera to device transformations (device_T_cameraA, and device_T_cameraB) as provided in Ocean's standard coordinate system and not in the inverted flipped coordinate system (as provided by VrApi).
		 * @param anyCameras The camera profiles of all images, one for each image
		 * @param frames The camera images
		 * @param world_T_device The transformation from device to world
		 * @param device_T_cameras The transformations from the individual cameras to the device
		 * @param cameraFrameType The camera frame type of the images
		 * @param cameraType The camera type of the images
		 * @param captureTimestampHostClockInSeconds The capture timestamp of the images
		 * @param frameMetadatas The metadata of the individual cameras, one for each camera image
		 */
		virtual void onCameraImages(SharedAnyCamerasD&& anyCameras, Frames&& frames, const HomogenousMatrixD4& world_T_device, HomogenousMatricesD4&& device_T_cameras, const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType& cameraType, const Timestamp& captureTimestampHostClockInSeconds, const FrameMetadatas& frameMetadatas);

		/**
		 * Disabled copy operator.
		 * @param nativeApplication Application object which would have been copied
		 * @return Reference to this object
		 */
		CameraVRNativeApplicationT& operator=(const CameraVRNativeApplicationT& nativeApplication) = delete;

	protected:

		/// The camera handling.
		CameraHandling cameraHandling_;

		/// The frame provider object allowing access to the cameras of the device.
		Sensors::FrameProvider frameProvider_;

		// Optional camera thread.
		CameraThread cameraThread_;

		/// The lock for the camera frames.
		mutable Lock cameraInitializationLock_;

		/// Indicates whether the user notification about denied camera permissions should be displayed.
		bool visualizeCameraPermissionDeniedMessage_;

		/// The ID of the string that is displayed if the camera permissions has been denied by the user.
		Index32 cameraPermissionDeniedMessageId_;
};

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::CameraThread::startThread(CameraVRNativeApplicationT* owner)
{
	ocean_assert(owner_ == nullptr);
	ocean_assert(owner != nullptr);

	owner_ = owner;
	Thread::startThread();
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::CameraThread::threadRun()
{
	ocean_assert(owner_ != nullptr);
	ocean_assert(owner_->androidApp_ != nullptr);

	while (owner_->androidApp_->destroyRequested == 0 && !shouldThreadStop())
	{
		const Timestamp timestamp(true);

		owner_->checkAndHandleCameraImages();

		if (double(Timestamp(true) - timestamp) <= 0.001)
		{
			// we sleep for one 1ms in case the processing of checkAndHandleCameraImages() was very fast
			Thread::sleep(1u);
		}
	}
}

template <typename TBaseApplication>
CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::MultiCameraData() :
	world_T_device_(false)
{
	// nothing to do here
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::update(SharedAnyCamerasD&& cameras, Frames&& frames, const HomogenousMatrixD4& world_T_device, HomogenousMatricesD4&& device_T_cameras)
{
	const ScopedLock scopedLock(lock_);

	cameras_ = std::move(cameras);
	frames_.clear();
	frames_.reserve(frames.size());

	for (size_t n = 0; n < frames.size(); ++n)
	{
		frames_.emplace_back(new Frame(std::move(frames[n])));
	}

	world_T_device_ = world_T_device;

	device_T_cameras_ = std::move(device_T_cameras);
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::latest(SharedAnyCamerasD* cameras, FrameRefs* frames, HomogenousMatrixD4* world_T_device, HomogenousMatricesD4* device_T_cameras, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (cameras_.empty())
	{
		return false;
	}

	ocean_assert(frames_.size() == cameras_.size());
	ocean_assert(frames_.size() == device_T_cameras_.size());
	ocean_assert(world_T_device_.isValid());

	for (size_t n = 0; n < frames_.size(); ++n)
	{
		ocean_assert(frames_[n]->isValid());
		ocean_assert(cameras_[n] && cameras_[n]->isValid());
		ocean_assert(device_T_cameras_[n].isValid());
	}

	if (lastTimestamp.isValid())
	{
		for (const FrameRef& frame : frames_)
		{
			ocean_assert(frame && frame->isValid());

			if (frame->timestamp() <= lastTimestamp)
			{
				return false;
			}
		}
	}

	if (cameras)
	{
		*cameras = cameras_;
	}

	if (frames)
	{
		*frames = frames_;
	}

	if (world_T_device)
	{
		*world_T_device = world_T_device_;
	}

	if (device_T_cameras)
	{
		*device_T_cameras = device_T_cameras_;
	}

	return true;
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::latest(SharedAnyCamerasF* cameras, FrameRefs* frames, HomogenousMatrixF4* world_T_device, HomogenousMatricesF4* device_T_cameras, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (cameras_.empty())
	{
		return false;
	}

	ocean_assert(frames_.size() == cameras_.size());
	ocean_assert(frames_.size() == device_T_cameras_.size());
	ocean_assert(world_T_device_.isValid());

	for (size_t n = 0; n < frames_.size(); ++n)
	{
		ocean_assert(frames_[n]->isValid());
		ocean_assert(cameras_[n]->isValid());
		ocean_assert(device_T_cameras_[n].isValid());
	}

	if (lastTimestamp.isValid())
	{
		for (const FrameRef& frame : frames_)
		{
			ocean_assert(frame && frame->isValid());

			if (frame->timestamp() <= lastTimestamp)
			{
				return false;
			}
		}
	}

	if (cameras)
	{
		cameras->clear();
		cameras->reserve(cameras_.size());

		for (const SharedAnyCameraD& camera : cameras_)
		{
			cameras->emplace_back(camera->cloneToFloat());
		}
	}

	if (frames)
	{
		*frames = frames_;
	}

	if (world_T_device)
	{
		*world_T_device = HomogenousMatrixF4(world_T_device_);
	}

	if (device_T_cameras)
	{
		device_T_cameras->clear();
		device_T_cameras->reserve(device_T_cameras_.size());

		for (const HomogenousMatrixD4& device_T_camera : device_T_cameras_)
		{
			device_T_cameras->emplace_back(device_T_camera);
		}
	}

	return true;
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::latest(FrameRefs* frames, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(frames_.size() == cameras_.size());
	ocean_assert(frames_.size() == device_T_cameras_.size());
	ocean_assert(world_T_device_.isValid());

	for (size_t n = 0; n < frames_.size(); ++n)
	{
		ocean_assert(frames_[n]->isValid());
		ocean_assert(cameras_[n]->isValid());
		ocean_assert(device_T_cameras_[n].isValid());
	}

	if (lastTimestamp.isValid())
	{
		for (const FrameRef& frame : frames_)
		{
			ocean_assert(frame && frame->isValid());

			if (frame->timestamp() <= lastTimestamp)
			{
				return false;
			}
		}
	}

	if (frames)
	{
		*frames = frames_;
	}

	return true;
}

template <typename TBaseApplication>
inline Timestamp CameraVRNativeApplicationT<TBaseApplication>::MultiCameraData::latestTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	if (frames_.empty())
	{
		return Timestamp(false);
	}

	return frames_.front()->timestamp();
}

template <typename TBaseApplication>
CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::StereoCameraData() :
	world_T_device_(false),
	device_T_cameraA_(false),
	device_T_cameraB_(false)
{
	// nothing to do here
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::update(const SharedAnyCameraD& cameraA, const SharedAnyCameraD& cameraB, Frame&& frameA, Frame&& frameB, const HomogenousMatrixD4& world_T_device, const HomogenousMatrixD4& device_T_cameraA, const HomogenousMatrixD4& device_T_cameraB)
{
	const ScopedLock scopedLock(lock_);

	cameraA_ = cameraA;
	cameraB_ = cameraB;

	frameA_ = FrameRef(new Frame(std::move(frameA)));
	frameB_ = FrameRef(new Frame(std::move(frameB)));

	world_T_device_ = world_T_device;

	device_T_cameraA_ = device_T_cameraA;
	device_T_cameraB_ = device_T_cameraB;
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::latest(SharedAnyCameraD* cameraA, SharedAnyCameraD* cameraB, FrameRef* frameA, FrameRef* frameB, HomogenousMatrixD4* world_T_device, HomogenousMatrixD4* device_T_cameraA, HomogenousMatrixD4* device_T_cameraB, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (!cameraA_ || !cameraA_->isValid() || !cameraB_ || !cameraB_->isValid())
	{
		return false;
	}

	if (lastTimestamp.isValid())
	{
		if ((frameA_ && frameA_->timestamp() <= lastTimestamp) || (frameB_ && frameB_->timestamp() <= lastTimestamp))
		{
			return false;
		}
	}

	ocean_assert(frameA_ && frameB_);
	ocean_assert(world_T_device_.isValid());
	ocean_assert(device_T_cameraA_.isValid() && device_T_cameraB_.isValid());

	if (cameraA)
	{
		*cameraA = cameraA_;
	}

	if (cameraB)
	{
		*cameraB = cameraB_;
	}

	if (frameA)
	{
		*frameA = frameA_;
	}

	if (frameB)
	{
		*frameB = frameB_;
	}

	if (world_T_device)
	{
		*world_T_device = world_T_device_;
	}

	if (device_T_cameraA)
	{
		*device_T_cameraA = device_T_cameraA_;
	}

	if (device_T_cameraB)
	{
		*device_T_cameraB = device_T_cameraB_;
	}

	return true;
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::latest(SharedAnyCameraF* cameraA, SharedAnyCameraF* cameraB, FrameRef* frameA, FrameRef* frameB, HomogenousMatrixF4* world_T_device, HomogenousMatrixF4* device_T_cameraA, HomogenousMatrixF4* device_T_cameraB, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (!cameraA_ || !cameraA_->isValid() || !cameraB_ || !cameraB_->isValid())
	{
		return false;
	}

	if (lastTimestamp.isValid())
	{
		if ((frameA_ && frameA_->timestamp() <= lastTimestamp) || (frameB_ && frameB_->timestamp() <= lastTimestamp))
		{
			return false;
		}
	}

	ocean_assert(frameA_ && frameB_);
	ocean_assert(world_T_device_.isValid());
	ocean_assert(device_T_cameraA_.isValid() && device_T_cameraB_.isValid());

	if (cameraA)
	{
		*cameraA = cameraA_->cloneToFloat();
	}

	if (cameraB)
	{
		*cameraB = cameraB_->cloneToFloat();
	}

	if (frameA)
	{
		*frameA = frameA_;
	}

	if (frameB)
	{
		*frameB = frameB_;
	}

	if (world_T_device)
	{
		*world_T_device = HomogenousMatrixF4(world_T_device_);
	}

	if (device_T_cameraA)
	{
		*device_T_cameraA = HomogenousMatrixF4(device_T_cameraA_);
	}

	if (device_T_cameraB)
	{
		*device_T_cameraB = HomogenousMatrixF4(device_T_cameraB_);
	}

	return true;
}

template <typename TBaseApplication>
bool CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::latest(FrameRef* frameA, FrameRef* frameB, const Timestamp& lastTimestamp) const
{
	const ScopedLock scopedLock(lock_);

	if (lastTimestamp.isValid())
	{
		if ((frameA_ && frameA_->timestamp() <= lastTimestamp) || (frameB_ && frameB_->timestamp() <= lastTimestamp))
		{
			return false;
		}
	}

	ocean_assert(frameA_ && frameB_);
	ocean_assert(world_T_device_.isValid());
	ocean_assert(device_T_cameraA_.isValid() && device_T_cameraB_.isValid());

	if (frameA)
	{
		*frameA = frameA_;
	}

	if (frameB)
	{
		*frameB = frameB_;
	}

	return true;
}

template <typename TBaseApplication>
inline Timestamp CameraVRNativeApplicationT<TBaseApplication>::StereoCameraData::latestTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	if (frameA_)
	{
		return frameA_->timestamp();
	}

	return Timestamp(false);
}

template <typename TBaseApplication>
CameraVRNativeApplicationT<TBaseApplication>::CameraVRNativeApplicationT(struct android_app* androidApp) :
	TBaseApplication(androidApp),
	cameraHandling_(CH_UNKNOWN),
	visualizeCameraPermissionDeniedMessage_(true),
	cameraPermissionDeniedMessageId_(Index32(-1))
{
	// nothing to do here
}

template <typename TBaseApplication>
CameraVRNativeApplicationT<TBaseApplication>::~CameraVRNativeApplicationT()
{
	// nothing to do here
}

template <typename TBaseApplication>
inline bool CameraVRNativeApplicationT<TBaseApplication>::startReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType, const CameraType cameraType)
{
	return frameProvider_.startReceivingCameraFrames(cameraFrameType, cameraType);
}

template <typename TBaseApplication>
inline bool CameraVRNativeApplicationT<TBaseApplication>::stopReceivingCameraFrames(const OSSDK::Sensors::v3::FrameType& cameraFrameType)
{
	return frameProvider_.stopReceivingCameraFrames(cameraFrameType);
}

template <typename TBaseApplication>
typename TBaseApplication::AndroidPermissionsSet CameraVRNativeApplicationT<TBaseApplication>::androidPermissionsToRequest()
{
	typename TBaseApplication::AndroidPermissionsSet permissions = TBaseApplication::androidPermissionsToRequest();

	permissions.emplace("com.oculus.permission.ACCESS_MR_SENSOR_DATA");

	return permissions;
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::onPermissionGranted(const std::string& permission)
{
	TBaseApplication::onPermissionGranted(permission);

	if (permission == "com.oculus.permission.ACCESS_MR_SENSOR_DATA")
	{
		frameProvider_.initialize(Sensors::FrameProvider::FCM_MAKE_COPY);

		// by default, we access the lower two stereo cameras of the heaset cameras frames

		constexpr OSSDK::Sensors::v3::FrameType cameraFrameType = OSSDK::Sensors::v3::FrameType::Headset;
		constexpr CameraType cameraType = Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS;

		if (frameProvider_.isCameraFrameTypeAvailable(cameraFrameType))
		{
			frameProvider_.startReceivingCameraFrames(cameraFrameType, cameraType);
		}
	}
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::render(const double predictedDisplayTime)
{
	if (cameraHandling_ == CH_UNKNOWN)
	{
		cameraHandling_ = desiredCameraHandling();

		if (cameraHandling_ == CH_ASYNCRONOUS)
		{
			cameraThread_.startThread(this);

			// sleeping 5ms to allow a proper initialization of the camera streams
			Thread::sleep(5u);
		}
	}

	if (cameraHandling_ == CH_SYNCRONOUS)
	{
		checkAndHandleCameraImages();
	}

	TBaseApplication::render(predictedDisplayTime);
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::checkAndHandleCameraImages()
{
	if (!frameProvider_.isValid())
	{
		return;
	}

	Frames frames;
	SharedAnyCamerasD anyCameras;

	HomogenousMatrixD4 world_T_device;
	HomogenousMatricesD4 device_T_cameras;

	OSSDK::Sensors::v3::FrameType cameraFrameType;
	CameraType cameraType;

	FrameMetadatas frameMetadatas;

	if (!frameProvider_.latestFrames(frames, &anyCameras, &world_T_device, &device_T_cameras, &cameraFrameType, &cameraType, &frameMetadatas))
	{
		// no new frames available
		return;
	}

	ocean_assert(!frames.empty());
	ocean_assert(frames.size() == frameMetadatas.size());

	const Timestamp timestamp(frames[0].timestamp());

	if (frames.size() == 2 && (cameraType == Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS || cameraType == Sensors::FrameProvider::CT_UPPER_STEREO_CAMERAS))
	{
		ocean_assert(anyCameras.size() == 2 && anyCameras.size() == 2);

		onStereoCameraImages(anyCameras[0], anyCameras[1], std::move(frames[0]), std::move(frames[1]), world_T_device, device_T_cameras[0], device_T_cameras[1], cameraFrameType, cameraType, timestamp, frameMetadatas[0], frameMetadatas[1]);
	}
	else
	{
		onCameraImages(std::move(anyCameras), std::move(frames), world_T_device, std::move(device_T_cameras), cameraFrameType, cameraType, timestamp, frameMetadatas);
	}
}

template <typename TBaseApplication>
typename CameraVRNativeApplicationT<TBaseApplication>::CameraHandling CameraVRNativeApplicationT<TBaseApplication>::desiredCameraHandling()
{
	// Note: Override this function in a derived class to customize the behavior
	return CH_SYNCRONOUS;
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::onPreRender(const Timestamp& renderTimestamp)
{
	TBaseApplication::onPreRender(renderTimestamp);

	if (visualizeCameraPermissionDeniedMessage_)
	{
		if (frameProvider_.isValid())
		{
			visualizeCameraPermissionDeniedMessage_ = false;
		}
		else
		{
			// In case the cameras have been initialized correctly, use invalid parameters in order to remove the text object from the text visualizer
			const HomogenousMatrix4 world_T_text(Vector3(0, 0, Scalar(-0.5)));
			const std::string cameraPermissionDeniedMessage = "Please grant permission to access camera sensors";

			ocean_assert(cameraPermissionDeniedMessageId_ == Index32(-1));
			this->vrTextVisualizer_.visualizeTextInWorld(cameraPermissionDeniedMessageId_, world_T_text, cameraPermissionDeniedMessage, Quest::Application::VRVisualizer::ObjectSize(0, 0.025), renderTimestamp);
		}
	}
	else
	{
		if (cameraPermissionDeniedMessageId_ != 0u)
		{
			this->vrTextVisualizer_.visualizeTextInWorld(cameraPermissionDeniedMessageId_, HomogenousMatrix4(false), "", Quest::Application::VRVisualizer::ObjectSize(), renderTimestamp);
			cameraPermissionDeniedMessageId_ = 0u;
		}
	}
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::onActivityDestroy()
{
	frameProvider_.release();

	TBaseApplication::onActivityDestroy();
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::onStereoCameraImages(const SharedAnyCameraD& /*anyCameraA*/, const SharedAnyCameraD& /*sharedCameraB*/, Frame&& /*frameA*/, Frame&& /*fameB*/, const HomogenousMatrixD4& /*world_T_device*/, const HomogenousMatrixD4& /*device_T_cameraA*/, const HomogenousMatrixD4& /*device_T_cameraB*/, const OSSDK::Sensors::v3::FrameType& /*cameraFrameType*/, const CameraType& cameraType, const Timestamp& /*captureTimestampHostClockInSeconds*/, const FrameMetadata& /*frameMetadataA*/, const FrameMetadata& /*frameMetadataB*/)
{
	ocean_assert(cameraType == Sensors::FrameProvider::CT_LOWER_STEREO_CAMERAS || cameraType == Sensors::FrameProvider::CT_UPPER_STEREO_CAMERAS);

	// can be implemented in a derived class
}

template <typename TBaseApplication>
void CameraVRNativeApplicationT<TBaseApplication>::onCameraImages(SharedAnyCamerasD&& /*anyCameras*/, Frames&& /*frames*/, const HomogenousMatrixD4& /*world_T_device*/, HomogenousMatricesD4&& /*device_T_cameras*/, const OSSDK::Sensors::v3::FrameType& /*cameraFrameType*/, const CameraType& cameraType, const Timestamp& /*captureTimestampHostClockInSeconds*/, const FrameMetadatas& /*frameMetadatas*/)
{
	ocean_assert(cameraType == Sensors::FrameProvider::CT_ALL_CAMERAS);

	// can be implemented in a derived class
}

}

}

}

}

}

}
