/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H
#define META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H

#include "ocean/io/IO.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/base/Singleton.h"

#include <map>

namespace Ocean
{

namespace IO
{

/**
 * This class implements a camera calibration manager as singleton.<br>
 * The manager organizes camera calibrations with different dimensions.<br>
 * @ingroup io
 */
class OCEAN_IO_EXPORT CameraCalibrationManager : public Singleton<CameraCalibrationManager>
{
	friend class Singleton<CameraCalibrationManager>;

	public:

		/**
		 * Definition of different priority values for the camera calibrations.
		 */
		enum Priority
		{
			/// Low priority
			PRIORITY_LOW,
			/// Medium priority
			PRIORITY_MEDIUM,
			/// High priority
			PRIORITY_HIGH,
			/// Explicit use priority
			PRIORITY_EXPLICIT
		};

		/**
		 * Definition of different calibration qualities.
		 */
		enum Quality
		{
			/// Exact calibration
			QUALITY_EXACT,
			/// Interpolated calibration
			QUALITY_INTERPOLATED,
			/// Default calibration
			QUALITY_DEFAULT
		};

	protected:

		/**
		 * Device calibration object.
		 */
		class Device
		{
			friend class CameraCalibrationManager;

			protected:

				/**
				 * Definition of a image resolution.
				 */
				typedef std::pair<unsigned int, unsigned int> Resolution;

				/**
				 * Definition of a pair of camera and priority values.
				 */
				typedef std::pair<PinholeCamera, Priority> CameraPair;

				/**
				 * Definition of a map mapping image resolutions to camera objects.
				 */
				typedef std::map<Resolution, CameraPair> ResolutionMap;

			public:

				/**
				 * Returns a calibration for a specific frame resolution.
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param quality Optional resulting quality of the calibration
				 * @param defaultFovX The default horizontal field of view the resulting camera profile will provide if no camera calibration exists for the specified resolution, with range (0, PI)
				 * @return The camera for the given resolution
				 */
				PinholeCamera camera(const unsigned int width, const unsigned int height, Quality* quality, const Scalar defaultFovX = Numeric::deg2rad(45)) const;

			protected:

				/**
				 * Adds a new calibration for a specific frame resolution.
				 * @param camera The camera object
				 * @param priority Priority of the calibration
				 * @return True, if succeeded
				 */
				bool addResolution(const PinholeCamera& camera, const Priority priority);

			protected:

				/// Resolution map.
				ResolutionMap resolutionMap_;
		};

		/**
		 * Definition of a map mapping device names to device objects.
		 */
		typedef std::map<std::string, Device> DeviceMap;

		/**
		 * Definition of a map mapping alias device names to device names.
		 */
		typedef std::map<std::string, std::string> AliasMap;

	public:

		/**
		 * Registers a new camera calibration.
		 * @param camera Name or url of the camera to register
		 * @param width The width of the camera resolution in pixel, with range [1, infinity)
		 * @param height The height of the camera resolution in pixel, with range [1, infinity)
		 * @param fovX Horizontal field of view in radian, with range (0, PI)
		 * @return True, if succeeded
		 */
		bool registerCalibration(const std::string& camera, const unsigned int width, const unsigned int height, const Scalar fovX);

		/**
		 * Registers a new camera calibration file.
		 * The file must have a specific file format.
		 * @param url Url of the calibration file
		 * @return True, if succeeded
		 */
		bool registerCalibrationFile(const std::string& url);

		/**
		 * Registers a new camera calibration.
		 * This explicit calibration will receive the highest priority.
		 * @param camera Name of the device or camera
		 * @param calibration The camera calibration profile
		 * @return True, if succeeded
		 */
		bool registerCalibration(const std::string& camera, const PinholeCamera& calibration);

		/**
		 * Registers an alias camera name.
		 * @param camera Original camera name
		 * @param alias Alias camera name
		 * @return True, if succeeded
		 */
		bool registerAlias(const std::string& camera, const std::string& alias);

		/**
		 * Returns a calibration for a specific device with a specific frame resolution.
		 * @param device Name of the device
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param quality Optional resulting quality of the calibration
		 * @param defaultFovX The default horizontal field of view the resulting camera profile will provide if no camera calibration exists for the specified device, with range (0, PI)
		 * @return The camera profile for the given resolution
		 */
		PinholeCamera camera(const std::string& device, const unsigned int width, const unsigned int height, Quality* quality = nullptr, const Scalar defaultFovX = Numeric::deg2rad(45)) const;

		/**
		 * Returns whether this manager holds a valid camera profile for a specified device.
		 * @param device The name of the device to check
		 * @return True, if so
		 */
		bool hasCalibration(const std::string& device) const;

	protected:

		/**
		 * Creates a new manager.
		 */
		CameraCalibrationManager();

		/**
		 * Destructs the manager.
		 */
		~CameraCalibrationManager();

	protected:

		/// Map holding all registered devices.
		DeviceMap deviceMap_;

		/// Map holding alias names.
		AliasMap aliasMap_;

		/// The lock of this manager.
		mutable Lock lock_;
};

}

}

#endif // META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H
