/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H
#define META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H

#include "ocean/io/IO.h"

#include "ocean/base/Singleton.h"

#include "ocean/io/JSONParser.h"

#include "ocean/math/AnyCamera.h"

#include <functional>

namespace Ocean
{

namespace IO
{

/**
 * This class implements a manager for camera calibrations using a modern JSON-based format.
 * The manager supports multiple camera types through a factory pattern and can be extended with custom camera types.
 *
 * By default, the manager supports "Ocean Pinhole" and "Ocean Fisheye" camera models, an example JSON file is:
 * <pre>
 * {
 *   "cameras": [
 *     {
 *       "name": "Camera Name",
 *       "aliases": ["Optional Alias 1", "Optional Alias 2"],
 *       "priority": 10,
 *       "calibrations": [
 *         {
 *           "resolution": {"width": 1920, "height": 1080},
 *           "model": "Ocean Pinhole",
 *           "configuration": "8_PARAMETERS",
 *           "parameters": [fx, fy, mx, my, k1, k2, p1, p2]
 *           "comment": "fully calibrated camera"
 *         },
 *         {
 *           "resolution": {"width": 640, "height": 480},
 *           "model": "Ocean Pinhole",
 *           "fovx": "1.05",
 *           "comment": "manually calibrate camera with approx. 60 degree of horizontal field of view"
 *         }
 *       ]
 *     }
 *   ]
 * }
 * </pre>
 * @ingroup io
 */
class OCEAN_IO_EXPORT CameraCalibrationManager : public Singleton<CameraCalibrationManager>
{
	friend class Singleton<CameraCalibrationManager>;

	public:

		/**
		 * Definition of different calibration qualities indicating how the calibration was obtained.
		 */
		enum CalibrationQuality : uint32_t
		{
			/// Unknown or invalid calibration quality.
			CQ_UNKNOWN = 0u,
			/// The calibration was interpolated from a calibration with a different resolution but the same aspect ratio.
			CQ_INTERPOLATED,
			/// The calibration exactly matches the requested resolution.
			CQ_EXACT
		};

		/**
		 * Definition of a factory function that creates a SharedAnyCamera from a JSON model object.
		 * @param modelObject The JSON object containing the camera model calibration, must be valid
		 * @return The created camera object, nullptr if creation failed
		 */
		using FactoryFunction = std::function<SharedAnyCamera(const JSONParser::JSONValue& modelObject)>;

	protected:

		/**
		 * This class stores multiple camera calibrations with the same priority.
		 * Each Calibrations object can hold camera models for different resolutions, but all share the same priority level.
		 */
		class CalibrationGroup
		{
			public:

				/**
				 * Creates a new Calibrations object with a specific priority.
				 * @param priority The priority of the calibrations, higher values indicate higher priority, with range (-infinity, infinity)
				 */
				explicit inline CalibrationGroup(const int32_t priority = 0);

				/**
				 * Adds a camera model to this calibration group.
				 * @param camera The camera to add, must be valid
				 * @return True if the camera was added successfully, false if a camera with the same resolution already exists
				 */
				bool addCamera(SharedAnyCamera&& camera);

				/**
				 * Returns the best matching camera calibration for a given resolution.
				 * The function first tries to find an exact resolution match.
				 * If no exact match is found, it tries to interpolate a calibration from cameras with the same aspect ratio.
				 * @param width The width of the requested camera image in pixels, with range [1, infinity)
				 * @param height The height of the requested camera image in pixels, with range [1, infinity)
				 * @param calibrationQuality The resulting quality of the calibration
				 * @return The best matching camera model, nullptr if no match was found
				 */
				SharedAnyCamera camera(const unsigned int width, const unsigned int height, CalibrationQuality& calibrationQuality) const;

				/**
				 * Returns the number of camera calibrations in this group.
				 * @return The number of calibrations
				 */
				inline size_t size() const;

			public:

				/// The priority of all calibrations in this group, higher values indicate higher priority.
				int32_t priority_ = 0;

				/// All camera models with individual resolutions.
				SharedAnyCameras cameras_;
		};

		/**
		 * Definition of a vector holding calibration groups.
		 */
		using CalibrationGroups = std::vector<CalibrationGroup>;

		/**
		 * Definition of a map mapping camera model names to factory functions.
		 */
		using FactoryFunctionMap = std::unordered_map<std::string, FactoryFunction>;

		/**
		 * Definition of a map mapping camera names to calibration groups.
		 */
		using CameraMap = std::unordered_map<std::string, CalibrationGroups>;

		/**
		 * Definition of a map mapping camera aliases to their actual camera names.
		 */
		using AliasMap = std::unordered_map<std::string, std::string>;

	public:

		/**
		 * Returns a camera for a given camera name and resolution.
		 * The function will find the best matching calibration for the given resolution.<br>
		 * First, it searches for exact resolution matches. If none are found, it attempts to interpolate from calibrations with the same aspect ratio.<br>
		 * When multiple calibrations are available, the one with the highest priority and best quality is selected.
		 * @param cameraName The name of the camera (or alias), must be valid
		 * @param width The width of the camera image in pixels, with range [1, infinity)
		 * @param height The height of the camera image in pixels, with range [1, infinity)
		 * @param calibrationQuality Optional resulting calibration quality, nullptr if not of interest
		 * @return The camera model, nullptr if no matching calibration was found
		 */
		SharedAnyCamera camera(const std::string& cameraName, unsigned int width, unsigned int height, CalibrationQuality* calibrationQuality = nullptr) const;

		/**
		 * Registers calibrations from a JSON file.
		 * @param url The URL or file path of the JSON calibration file, must be valid
		 * @return True if the calibrations were registered successfully, false otherwise
		 */
		bool registerCalibrations(const std::string& url);

		/**
		 * Registers calibrations from a memory buffer containing JSON data.
		 * @param buffer The memory buffer containing the JSON calibration data, must be valid
		 * @param size The size of the buffer in bytes, with range [1, infinity)
		 * @return True if the calibrations were registered successfully, false otherwise
		 */
		bool registerCalibrations(const void* buffer, const size_t size);

		/**
		 * Registers calibrations from a parsed JSON value.
		 * @param jsonValue The parsed JSON value, must be a valid object holding the calibration
		 * @return True if the calibrations were registered successfully, false otherwise
		 */
		bool registerCalibrations(const JSONParser::JSONValue& jsonValue);

		/**
		 * Registers a single camera calibration with a specific priority.
		 * @param cameraName The name of the camera, must be valid
		 * @param camera The camera model to register, must be valid
		 * @param priority The priority of this calibration, higher values indicate higher priority, with range (-infinity, infinity)
		 * @return True if the camera was registered successfully
		 * @see registerCalibrations()
		 */
		bool registerCamera(const std::string& cameraName, SharedAnyCamera&& camera, const int32_t priority);

		/**
		 * Registers a new camera factory for a specific camera type.
		 * Factory functions are used to create camera objects from JSON model configurations.
		 * @param modelName The name of the camera model, e.g., "Custom Camera", must be valid
		 * @param factoryFunction The factory function to create cameras of the specified model, nullptr to unregister a factory
		 * @return True if the factory was registered successfully, false if a factory for the same model name already exists
		 */
		bool registerFactoryFunction(const std::string& modelName, FactoryFunction&& factoryFunction);

		/**
		 * Clears all registered calibrations and aliases.
		 * This function does not remove registered factory functions.
		 */
		void clear();

		/**
		 * Parses the resolution of a camera from a JSON calibration object.
		 * @param calibrationObject The JSON calibration object, must be valid
		 * @param width The resulting width in pixels
		 * @param height The resulting height in pixels
		 * @return True if the resolution was parsed successfully
		 */
		static bool parseResolution(const JSONParser::JSONValue& calibrationObject, unsigned int& width, unsigned int& height);

	protected:

		/**
		 * Protected default constructor.
		 * Automatically registers the built-in factory functions for "Ocean Pinhole" and "Ocean Fisheye" camera models.
		 */
		CameraCalibrationManager();

		/**
		 * Factory function able to create the "Ocean Pinhole" camera model from a JSON configuration.
		 * @param modelObject The JSON object containing the camera model configuration, must be valid
		 * @return The created camera object, nullptr if creation failed
		 */
		static SharedAnyCamera createOceanPinhole(const JSONParser::JSONValue& modelObject);

		/**
		 * Factory function able to create the "Ocean Fisheye" camera model from a JSON configuration.
		 * @param modelObject The JSON object containing the camera model configuration, must be valid
		 * @return The created camera object, nullptr if creation failed
		 */
		static SharedAnyCamera createOceanFisheye(const JSONParser::JSONValue& modelObject);

	protected:

		/// The map mapping camera model names to factory functions.
		FactoryFunctionMap factoryFunctionMap_;

		/// The map mapping camera names to calibration groups.
		CameraMap cameraMap_;

		/// The map mapping camera aliases to their actual camera names.
		AliasMap aliasMap_;

		/// The lock for thread-safe access to all manager data.
		mutable Lock lock_;
};

inline CameraCalibrationManager::CalibrationGroup::CalibrationGroup(const int32_t priority) :
	priority_(priority)
{
	// nothing to do here
}

inline size_t CameraCalibrationManager::CalibrationGroup::size() const
{
	return cameras_.size();
}

}

}

#endif // META_OCEAN_IO_CAMERA_CALIBRATION_MANAGER_H
