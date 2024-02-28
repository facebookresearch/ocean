// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_UTILITIES_H
#define META_OCEAN_MEDIA_VRS_UTILITIES_H

#include "ocean/media/vrs/VRS.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

#include <perception/camera/Fisheye62CameraModelLut1D.h>

#include <vrs/RecordFormatStreamPlayer.h>

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class implements several utility functions.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT Utilities
{
	public:

		/**
		 * Definition of a map mapping timestamps to poses.
		 */
		typedef SampleMap<HomogenousMatrixD4> PosesMap;

		/**
		 * Definition of an unordered set holding recordable type ids.
		 */
		typedef std::unordered_set<vrs::RecordableTypeId> RecordableTypeIdSet;

	protected:

		/**
		 * This class implements a specialization of a `RecordFormatStreamPlayer` object for IMU data.
		 */
		class IMUDataPlayable : public vrs::RecordFormatStreamPlayer
		{
			public:

				/**
				 * Creates a new playable object.
				 */
				IMUDataPlayable();

				/**
				 * Data layout read event function of the playable.
				 * @see RecordFormatStreamPlayer::onDataLayoutRead().
				 */
				bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

				/**
				 * Returns the calibration transformation device from imu.
				 * @return The transformation, may be invalid
				 */
				inline const HomogenousMatrixD4& device_T_imu() const;

				/**
				 * Returns the calibration rotation imu from imuraw.
				 * @return The rotation, may be invalid
				 */
				inline const QuaternionD& imu_R_imuraw() const;

			protected:

				/// The calibration transformtion providing device from imu.
				HomogenousMatrixD4 device_T_imu_;

				/// The calibration rotation providing imu from imuraw.
				QuaternionD imu_R_imuraw_;
		};

		/**
		 * This class implements a specialization of a `RecordFormatStreamPlayer` object for images.
		 */
		class ImagePlayable : public vrs::RecordFormatStreamPlayer
		{
			public:

				/**
				 * Creates a new playable object.
				 */
				ImagePlayable();

				/**
				 * Data layout read event function of the playable.
				 * @see RecordFormatStreamPlayer::onDataLayoutRead().
				 */
				bool onDataLayoutRead(const vrs::CurrentRecord& header, size_t blockIndex, vrs::DataLayout& dataLayout) override;

				/**
				 * Returns the camera model of the playable.
				 * @return The camera model
				 */
				inline std::shared_ptr<perception::CameraModelInterface<double>> cameraModel() const;

				/**
				 * Returns the transformation from camera to device.
				 * @return The camera transformation
				 */
				inline const HomogenousMatrixD4& device_T_camera() const;

			protected:

				/// The camera model of the playable.
				std::shared_ptr<perception::CameraModelInterface<double>> cameraModel_;

				/// The transformation from camera to device.
				HomogenousMatrixD4 device_T_camera_;
		};

	public:

		/**
		 * Returns the IMU data calibration data from a vrs file.
		 * @param vrsFile The vrs file from which the calibration data will be read
		 * @param device_T_imu The resulting calibration transformation device from imu, will be valid
		 * @param imu_R_imuraw The resulting calibration rotation imu from imuraw, will be valid
		 * @return True, if succeeded
		 */
		static bool readCalibrationData(const std::string& vrsFile, HomogenousMatrixD4& device_T_imu, QuaternionD& imu_R_imuraw);

		/**
		 * Returns the camera calibration datas from a vrs file.
		 * @param vrsFile The vrs file from which the calibration data will be read
		 * @param cameraRecordables Resulting names of the invidiual recordables for which the camera calibration is determined
		 * @param cameraModels Optional resulting camera models, one for each recordable
		 * @param device_T_cameras Optional resulting transformations from cameras to device, one for each recordable
		 * @return True, if succeeded
		 */
		static bool readCameraCalibrationData(const std::string& vrsFile, std::vector<std::string>& cameraRecordables, std::vector<std::shared_ptr<perception::CameraModelInterface<double>>>* cameraModels = nullptr, HomogenousMatricesD4* device_T_cameras = nullptr);

		/**
		 * Writes 6-DOF poses to a data file (e.g., precalculated camera poses or device poses).
		 * The data file will store 7 parameters per row, one row provides one 6-DOF pose:
		 * <pre>
		 * timestamp exponentialRotationX exponentialRotationY exponentialRotationZ translationX translationY translationZ
		 * </pre>
		 * @param poses The map with 6-DOF poses to be written
		 * @param filename The file to which the poses will be written
		 * @return True, if succeeded
		 * @see loadPosesMap().
		 */
		static bool writePosesMap(const PosesMap& poses, const std::string& filename);

		/**
		 * Loads 6-DOF poses from a data file (e.g., precalculated camera poses or device poses).
		 * The data file must store 7 parameters per row, one row provides one 6-DOF pose:
		 * <pre>
		 * timestamp exponentialRotationX exponentialRotationY exponentialRotationZ translationX translationY translationZ
		 * </pre>
		 * @param filename The file from which the poses will be read
		 * @return The resulting map with 6-DOF poses
		 * @see writePosesMap().
		 */
		static PosesMap loadPosesMap(const std::string& filename);

		/**
		 * Extracts 6-DOF device poses from a JSON file created with the replay tool.
		 * The pose is extracted from one of both branches:
		 * <pre>
		 * map_data/frames/vision_estimate/se3/rotation,translation
		 * map_data/frames/vision_prediction/se3/rotation,translation
		 * with timestamp:
		 * map_data/frames/tracking_arrival_timestamp
		 * </pre>
		 * The replay tool can be executed via:
		 * @code
		 * cd ~/ovrsource
		 * buck run @arvr/mode/mac/opt //arvr/projects/viper/applications/replay:replay -- -s input.vrs
		 * @endcode
		 * @param jsonFilename The filename of the JSON file from which the poses will be extracted
		 * @param poses The resulting extracted poses
		 * @param extractEstimatedPoses True, to extract estimated poses; False, to extract predicted poses
		 * @return True, if succeeded
		 */
		static bool extractReplayPosesFromJSON(const std::string& jsonFilename, PosesMap& poses, const bool extractEstimatedPoses = true);
};

inline const HomogenousMatrixD4& Utilities::IMUDataPlayable::device_T_imu() const
{
	return device_T_imu_;
}

inline const QuaternionD& Utilities::IMUDataPlayable::imu_R_imuraw() const
{
	return imu_R_imuraw_;
}

inline std::shared_ptr<perception::CameraModelInterface<double>> Utilities::ImagePlayable::cameraModel() const
{
	return cameraModel_;
}

inline const HomogenousMatrixD4& Utilities::ImagePlayable::device_T_camera() const
{
	return device_T_camera_;
}

}

}

}

#endif // META_OCEAN_MEDIA_VRS_UTILITIES_H
