// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_FRAME_MEDIUM_H
#define META_OCEAN_MEDIA_VRS_FRAME_MEDIUM_H

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSMedium.h"

#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/media/FrameMedium.h"

#include <perception/camera/Fisheye62CameraModelLut1D.h>

#include <vrs/RecordFormatStreamPlayer.h>

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This is the base class for all VRS frame mediums.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT VRSFrameMedium :
	virtual public VRSMedium,
	virtual public FrameMedium
{
	friend class ImagePlayable;

	protected:

		/**
		 * This class implements a specialization of a `RecordFormatStreamPlayer` object for images.
		 */
		class ImagePlayable : public vrs::RecordFormatStreamPlayer
		{
			public:

				/**
				 * Creates a new playable object.
				 * @param owner The owner of this object, which is the parent VRSFrameMedium object, must be valid
				 */
				ImagePlayable(VRSFrameMedium* owner);

				/**
				 * Data layout read event function of the playable.
				 * @see RecordFormatStreamPlayer::onDataLayoutRead().
				 */
				bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

				/**
				 * Image read event function of the playable.
				 * @see RecordFormatStreamPlayer::onImageRead().
				 */
				bool onImageRead(const vrs::CurrentRecord& header, size_t index, const vrs::ContentBlock& block) override;

			protected:

				/// The owner of this playable object.
				VRSFrameMedium* owner_;

				/// The explicit timestamp if available in the data layout, otherwise invalid.
				double explicitTimestamp_;
		};

	public:

		/**
		 * Returns extrinsic camera pose in relation to the coordinate system of the device.
		 * @return The relative extrinsic camera pose
		 */
		inline HomogenousMatrixD4 device_T_camera() const override;

		/**
		 * Returns the camera model if the VRS recording.
		 * @return The camera model, nullptr if unknown
		 */
		inline SharedAnyCamera anyCamera() const;

	protected:

		/**
		 * Creates a new frame medium by a given url.
		 * @param url Url of the frame medium
		 */
		explicit VRSFrameMedium(const std::string& url);

		/**
		 * Destructs a VRSFrameMedium object
		 */
		~VRSFrameMedium() override;

		/**
		 * Event function for a new camera calibration.
		 * @param anyCamera The camera model defining the projection, must be valid
		 * @param device_T_camera The extrinsic camera pose in relation to the coordinate system of the device
		 */
		void onNewCameraCalibration(SharedAnyCamera anyCamera, const HomogenousMatrixD4& device_T_camera);

		/**
		 * Event function for new images.
		 * @param frame The new padding frame which has arrived, must be valid
		 * @param vrsTimestamp The VRS timestamp of the frame, as given by the vrs file, must be valid
		 */
		void onNewFrame(Frame&& frame, const double vrsTimestamp);

		/**
		 * Specifies whether the media playback time will be respected or whether the media content will be provided as fast as possible.
		 * @see VRSMedium::setRespectPlaybackTime().
		 */
		bool setRespectPlaybackTime(const bool respectPlaybackTime) override;

	protected:

		/// The playable object of this medium receiving the images from the vrs player.
		ImagePlayable imagePlayable_;

		/// The (VRS) timestamp of the previous frame.
		Timestamp previousVRSFrameTimestamp_;

		/// The unix timestamp of the previous frame.
		Timestamp previousUnixFrameTimestamp_;

		/// The extrinsic camera pose in relation to the coordinate system of the device (dTc).
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

		/// The camera profile, if known.
		SharedAnyCamera anyCamera_;

		/// Intermediate respect playback time state, True by default.
		bool respectPlaybackTime_ = true;
};

inline HomogenousMatrixD4 VRSFrameMedium::device_T_camera() const
{
	const ScopedLock scopedLock(lock_);

	return device_T_camera_;
}

inline SharedAnyCamera VRSFrameMedium::anyCamera() const
{
	const ScopedLock scopedLock(lock_);

	return anyCamera_;
}

}

}

}

#endif // META_OCEAN_MEDIA_VRS_FRAME_MEDIUM_H
