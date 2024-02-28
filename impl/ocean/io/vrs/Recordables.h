// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_RENDERABLES_H
#define META_OCEAN_IO_VRS_RENDERABLES_H

#include "ocean/io/vrs/VRS.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/AnyCamera.h"

#include <vrs/Compressor.h>
#include <vrs/DataLayout.h>
#include <vrs/DataLayoutConventions.h>
#include <vrs/DataPieceVector.h>
#include <vrs/DataPieces.h>
#include <vrs/Recordable.h>

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * This class is the base class for all recordabled.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT Recordable : public vrs::Recordable
{
	public:

		/**
		 * Releases the resources of the recordabled.
		 */
		virtual void release();

		/**
		 * Returns whether this recodable is valid and configured.
		 * @return True, if succeeded
		 */
		virtual bool isValid() const;

	protected:

		/**
		 * Creates a new recordable.
		 * @param typeId The type id of the recordable
		 * @param flavor The flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional timestamp of the first configuration and state record, invalid to use the current time instead
		 */
		Recordable(vrs::RecordableTypeId typeId, const std::string& flavor, const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

	protected:

		// The timestamp of the next configuration and state record, invalid to use the current time instead.
		Timestamp vrsRecordTimestamp_;
};

/**
 * This class implements a recordable for camera streams.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT RecordableCamera : public Recordable
{
	protected:

		/// Version of the configuration layout.
		static const uint32_t configurationLayoutVersion_ = 1u;

		/// Version of the data layout.
		static const uint32_t dataLayoutVersion_ = 1u;

		/**
		 * This struct defines the configuration layout of the camera stream.
		 */
		struct ConfigurationLayoutCamera : public vrs::AutoDataLayout
		{
			using ImageSpecType = vrs::DataLayoutConventions::ImageSpecType;

			/// The id of the camera, with range [0, infinity)
			vrs::DataPieceValue<uint32_t> camera_id{"camera_id"};

			/// The width of the camera, in pixel, with range [0, infinity)
			vrs::DataPieceValue<ImageSpecType> width{vrs::DataLayoutConventions::kImageWidth};

			/// The height of the camera, in pixel, with range [0, infinity)
			vrs::DataPieceValue<ImageSpecType> height{vrs::DataLayoutConventions::kImageHeight};

			/// The number of bytes between two image rows of the camera, in bytes, with range [width, infinity)
			vrs::DataPieceValue<ImageSpecType> stride{vrs::DataLayoutConventions::kImageStride};

			/// The pixel format of the camera image.
			vrs::DataPieceEnum<vrs::PixelFormat, ImageSpecType> pixelFormat{vrs::DataLayoutConventions::kImagePixelFormat};

			/// The calibration information of the camera.
			vrs::DataPieceVector<float> cameraCalibrationValues{"camera_calibration_values"};

			/// The transformation between camera and device, if known.
			vrs::DataPieceValue<vrs::Matrix4Dd> device_T_camera{"device_T_camera"};

			/// The transformation between flipped camera and device, if known.
			vrs::DataPieceValue<vrs::Matrix4Dd> device_T_flippedCamera{"device_T_flippedCamera"};

			/// The calibration JSON file of the camera.
			vrs::DataPieceString factory_calibration{"factory_calibration"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * This struct defines the data layout of the camera stream.
		 */
		struct DataLayoutCamera : public vrs::AutoDataLayout
		{
			/// The timestamp of the camera data.
			vrs::DataPieceValue<double> timestamp{"timestamp"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

	public:

		/**
		 * Creates a new recordable.
		 * @param recordableTypeId The type id of the camera recordable
		 * @param flavor The optional flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If not specified, the current time will be used to create the configuration and state record. Range: (0, infinity)
		 */
		explicit RecordableCamera(const vrs::RecordableTypeId recordableTypeId = vrs::RecordableTypeId::ForwardCameraRecordableClass, const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Adds new image data to the stream.
		 * @param cameraId The id of the camera, with range [0, infinity)
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param frame The frame containing the actual image to add, must be valid
		 * @param device_T_camera The transformation between camera and device, if known
		 * @param frameTimestamp Optional explicit frame timestamp to be used instead of the frame's timestamp, invalid to use the frame's timestamp
		 * @param recordTimestamp The optional timestamp of the configuration record, invalid to use the current unix system time
		 * @return True, if succeeded
		 */
		bool addData(const uint32_t cameraId, const SharedAnyCameraD& anyCamera, const Frame& frame, const HomogenousMatrixD4& device_T_camera = HomogenousMatrixD4(false), const Timestamp& frameTimestamp = Timestamp(false), const Timestamp& recordTimestamp = Timestamp(false));

		/**
		 * Deprecated.
		 *
		 * Sets or changes the camera profile of this recordable.
		 * In case the given camera profile is identical to the latest camera profile, nothing happens.
		 * @param cameraId The id of the camera, with range [0, infinity)
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param pixelFormat The pixel format of the camera images, possible values: FORMAT_Y8, FORMAT_Y10, FORMAT_BGR24, FORMAT_RGB24, FORMAT_RGBA32, FORMAT_Y_U_V12, FORMAT_F32
		 * @param strideBytes The number of bytes between two image rows, in bytes, with range [camera.width(), infinity)
		 * @param device_T_camera The transformation between camera and device, if known
		 * @param recordTimestamp The optional timestamp of the configuration record, invalid to use the current time
		 * @return True, if succeeded
		 */
		bool setCamera(const uint32_t cameraId, const SharedAnyCameraD& anyCamera, const FrameType::PixelFormat pixelFormat, const unsigned int strideBytes, const HomogenousMatrix4& device_T_camera = HomogenousMatrix4(false), const Timestamp& recordTimestamp = Timestamp(false));

		/**
		 * Deprecated.
		 *
		 * Sets or changes the camera profile of this recordable.
		 * In case the given camera profile is identical to the latest camera profile, nothing happens.
		 * @param cameraId The id of the camera, with range [0, infinity)
		 * @param width The width of the camera, in pixel, with range [1, infinity)
		 * @param height The height of the camera, in pixel, with range [1, infinity)
		 * @param anyCamera The profile of the camera, must be valid
		 * @param pixelFormat The pixel format of the camera images, possible values: FORMAT_Y8, FORMAT_Y10, FORMAT_BGR24, FORMAT_RGB24, FORMAT_RGBA32, FORMAT_Y_U_V12, FORMAT_F32
		 * @param strideBytes The number of bytes between two image rows, in bytes, with range [camera.width(), infinity)
		 * @param device_T_camera The transformation between camera and device, if known
		 * @param recordTimestamp The optional timestamp of the configuration record, invalid to use the current time
		 * @return True, if succeeded
		 */
		bool setCamera(const uint32_t cameraId, const unsigned int width, const unsigned int height, SharedAnyCameraD anyCamera, const FrameType::PixelFormat pixelFormat, const unsigned int strideBytes, const HomogenousMatrixD4& device_T_camera = HomogenousMatrixD4(false), const Timestamp& recordTimestamp = Timestamp(false));

		/**
		 * Deprecated.
		 *
		 * Adds new image data to the stream, matching to the configured camera profile.
		 * In case the pixel format is FORMAT_Y_UV12, the given memory must contain both planes without any gap between both planes and the planes must not contain padding.
		 * @param frame The frame data to be added, must be valid
		 * @param size The size of the frame in bytes, with range [width * height, infinity)
		 * @param timestamp The timestamp of the image
		 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
		 */
		void addData(const void* frame, const unsigned int size, const double timestamp, const double* recordTimestamp = nullptr);

		/**
		 * Releases the resources of the recordable.
		 */
		void release() override;

		/**
		 * Returns whether this recordable is valid and configured.
		 * @return True, if succeeded
		 */
		bool isValid() const override;

		/**
		 * Returns the default flavor of this recoding.
		 * @return The recording's default flavor
		 */
		static std::string defaultFlavor();

		/**
		 * Returns the default recordable type id of this recording.
		 * @return The recording's default id
		 */
		static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

		/**
		 * Converts the camera profile of an AnyCamera to a perception JSON calibration string.
		 * @param anyCamera The camera profile to convert to a JSON string, must be valid
		 * @param device_T_camera The transformation between camera and device, with camera pointing towards the negative z-space with y-axis up, must be valid
		 * @param perceptionJsonCalibration The resulting JSON calibration string
		 * @return True, if succeeded
		 */
		static bool exportCameraToPerceptionJSON(const AnyCameraD& anyCamera, const HomogenousMatrixD4& device_T_camera, std::string& perceptionJsonCalibration);

		/**
		 * Converts a perception JSON calibration string to an AnyCamera.
		 * @param perceptionJsonCalibration The JSON calibration string to convert
		 * @param anyCamera The resulting camera profile
		 * @param device_T_camera The resulting transformation between camera and device, with camera pointing towards the negative z-space with y-axis up
		 * @return True, if succeeded
		 */
		static bool importCameraFromPerceptionJSON(const std::string& perceptionJsonCalibration, SharedAnyCameraD& anyCamera, HomogenousMatrixD4& device_T_camera);

	protected:

		/**
		 * Configuration records describe how the device recorded is configured/setup.
		 * @see Recordable::createConfigurationRecord().
		 */
		const vrs::Record* createConfigurationRecord() override;

		/**
		 * State records describe the internal state of the device, if it's stateful.
		 * @see Recordable::createStateRecord();
		 */
		const vrs::Record* createStateRecord() override;

	protected:

		/// The id of the camera, with range [0, infinity).
		uint32_t cameraId_ = uint32_t(-1);

		/// The width of the camera in pixel, with range [0, infinity).
		unsigned int cameraWidth_ = 0u;

		/// The height of the camera in pixel, with range [0, infinity).
		unsigned int cameraHeight_ = 0u;

		/// The camera profile.
		SharedAnyCameraD anyCamera_;

		/// The VRS pixel format.
		vrs::PixelFormat vrsPixelFormat_ = vrs::PixelFormat::UNDEFINED;

		/// The number of bytes between two image rows, with range [width, infinity)
		unsigned int strideBytes_ = 0u;

		/// The transformation between camera and device, if known.
		HomogenousMatrixD4 device_T_camera_ = HomogenousMatrixD4(false);

		/// The configuration layout.
		ConfigurationLayoutCamera configurationLayout_;

		/// The data layout.
		DataLayoutCamera dataLayout_;
};

/**
 * This class implements a recordable for 6-DOF pose streams.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT RecordableHomogenousMatrix4 : public Recordable
{
	public:

		/// Version of the configuration layout.
		static const uint32_t configurationLayoutVersion_ = 1u;

		/// Version of the data layout.
		static const uint32_t dataLayoutVersion_ = 1u;

		/**
		 * This struct defines the configuration layout of pose stream.
		 */
		struct ConfigurationLayoutHomogenousMatrix4 : public vrs::AutoDataLayout
		{
			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * This struct defines the data layout of pose stream.
		 */
		struct DataLayoutHomogenousMatrix4 : public vrs::AutoDataLayout
		{
			/// The homogenous transformation matrix.
			vrs::DataPieceValue<vrs::Matrix4Dd> homogenousMatrix4{"homogenous_matrix_4"};

			/// The timestamp of the transformation.
			vrs::DataPieceValue<double> timestamp{"timestamp"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * Creates a new recordable.
		 * @param recordableTypeId The type id of the recordable
		 * @param flavor The optional flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If not specified, the current time will be used to create the configuration and state record. Range: (0, infinity)
		 */
		RecordableHomogenousMatrix4(const vrs::RecordableTypeId recordableTypeId = vrs::RecordableTypeId::PoseRecordableClass, const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Adds a new pose to this stream.
		 * @param homogenousMatrix4 The homogenous matrix to be added, must be valid
		 * @param timestamp The timestamp of the pose
		 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
		 */
		void addData(const HomogenousMatrix4& homogenousMatrix4, const double timestamp, const double* recordTimestamp = nullptr);

		/**
		 * Returns the default flavor of this recoding.
		 * @return The recording's default flavor
		 */
		static std::string defaultFlavor();

		/**
		 * Returns the default recordable type id of this recording.
		 * @return The recording's default id
		 */
		static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

	protected:

		/**
		 * Configuration records describe how the device recorded is configured/setup.
		 * @see Recordable::createConfigurationRecord().
		 */
		const vrs::Record* createConfigurationRecord() override;

		/**
		 * State records describe the internal state of the device, if it's stateful.
		 * @see Recordable::createStateRecord();
		 */
		const vrs::Record* createStateRecord() override;

	protected:

		/// The configuration layout.
		ConfigurationLayoutHomogenousMatrix4 configurationLayout_;

		/// The data layout.
		DataLayoutHomogenousMatrix4 dataLayout_;
};

/**
 * This class implements a recordable stream for multiple simultaneous 6-DOF poses.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT RecordableHomogenousMatrices4 : public Recordable
{
	public:

		/// Version of the configuration layout.
		static const uint32_t configurationLayoutVersion_ = 1u;

		/// Version of the data layout.
		static const uint32_t dataLayoutVersion_ = 1u;

		/**
		 * This struct defines the configuration layout of poses stream.
		 */
		struct ConfigurationLayoutHomogenousMatrices4 : public vrs::AutoDataLayout
		{
			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * This struct defines the data layout of poses stream.
		 */
		struct DataLayoutHomogenousMatrices4 : public vrs::AutoDataLayout
		{
			/// The homogenous transformation matrices.
			vrs::DataPieceVector<vrs::Matrix4Dd> homogenousMatrix4{"homogenous_matrices_4"};

			/// The timestamp of the transformations.
			vrs::DataPieceValue<double> timestamp{"timestamp"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * Creates a new recordable.
		 * @param recordableTypeId The type id of the recordable
		 * @param flavor The optional flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If not specified, the current time will be used to create the configuration and state record. Range: (0, infinity)
		 */
		RecordableHomogenousMatrices4(const vrs::RecordableTypeId recordableTypeId = vrs::RecordableTypeId::PoseRecordableClass, const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Adds new poses to this stream.
		 * @param homogenousMatrices4 The homogenous matrices to be added, must be non-empty and all elements must be valid
		 * @param timestamp The timestamp of the pose
		 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
		 */
		void addData(const HomogenousMatrices4& homogenousMatrices4, const double timestamp, const double* recordTimestamp = nullptr);

		/**
		 * Returns the default flavor of this recoding.
		 * @return The recording's default flavor
		 */
		static std::string defaultFlavor();

		/**
		 * Returns the default recordable type id of this recording.
		 * @return The recording's default id
		 */
		static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

	protected:

		/**
		 * Configuration records describe how the device recorded is configured/setup.
		 * @see Recordable::createConfigurationRecord().
		 */
		const vrs::Record* createConfigurationRecord() override;

		/**
		 * State records describe the internal state of the device, if it's stateful.
		 * @see Recordable::createStateRecord();
		 */
		const vrs::Record* createStateRecord() override;

	protected:

		/// The configuration layout.
		ConfigurationLayoutHomogenousMatrices4 configurationLayout_;

		/// The data layout.
		DataLayoutHomogenousMatrices4 dataLayout_;
};

/**
 * This class implements a recordable for string streams.
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT RecordableString : public Recordable
{
	protected:

		/// Version of the configuration layout.
		static const uint32_t configurationLayoutVersion_ = 1u;

		/// Version of the data layout.
		static const uint32_t dataLayoutVersion_ = 1u;

		/**
		 * This struct defines the configuration layout of camera stream.
		 */
		struct ConfigurationLayoutString : public vrs::AutoDataLayout
		{
			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * This struct defines the data layout of camera stream.
		 */
		struct DataLayoutString : public vrs::AutoDataLayout
		{
			/// The string value.
			vrs::DataPieceString stringValue{"string"};

			/// The timestamp of the transformation.
			vrs::DataPieceValue<double> timestamp{"timestamp"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

	public:

		/**
		 * Creates a new recordable.
		 * @param recordableTypeId The type id of the recordable
		 * @param flavor The optional flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If not specified, the current time will be used to create the configuration and state record. Range: (0, infinity)
		 */
		RecordableString(const vrs::RecordableTypeId recordableTypeId = vrs::RecordableTypeId::AnnotationRecordableClass, const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Adds a new string value to this stream.
		 * @param value The string value to be added, must be valid
		 * @param timestamp The timestamp of the string
		 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
		 */
		void addData(const std::string& value, const double timestamp, const double* recordTimestamp = nullptr);

		/**
		 * Returns the default flavor of this recoding.
		 * @return The recording's default flavor
		 */
		static std::string defaultFlavor();

		/**
		 * Returns the default recordable type id of this recording.
		 * @return The recording's default id
		 */
		static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

	protected:

		/**
		 * Configuration records describe how the device recorded is configured/setup.
		 * @see Recordable::createConfigurationRecord().
		 */
		const vrs::Record* createConfigurationRecord() override;

		/**
		 * State records describe the internal state of the device, if it's stateful.
		 * @see Recordable::createStateRecord();
		 */
		const vrs::Record* createStateRecord() override;

	protected:

		/// The configuration layout.
		ConfigurationLayoutString configurationLayout_;

		/// The data layout.
		DataLayoutString dataLayout_;
};

/**
 * This class implements a recordable for GT object streams (eg plane, objects), where the
 * objects are defined by their world pose and boundary
 * @ingroup iovrs
 */
class OCEAN_IO_VRS_EXPORT RecordableObjectBoundary : public Recordable
{
	public:

		/// Version of the configuration layout.
		static const uint32_t configurationLayoutVersion_ = 1u;

		/// Version of the data layout.
		static const uint32_t dataLayoutVersion_ = 1u;

		/**
		 * This struct defines the configuration layout of camera stream.
		 */
		struct ConfigurationLayoutObjectBoundary : public vrs::AutoDataLayout
		{
			/// Unique identifier of the object
			vrs::DataPieceValue<uint32_t> objectId{"object_id"};

			/// Boundary, defined in the object frame of reference
			/// For planes, y axis is always 0.
			vrs::DataPieceVector<vrs::Point3Dd> boundary{"boundary"};

			/// Semantic label of the object, stored as ID
			vrs::DataPieceValue<int> semanticLabel{"semantic_label_id"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * This struct defines the data layout of camera stream.
		 */
		struct DataLayoutObjectBoundary : public vrs::AutoDataLayout
		{
			/// The per frame world pose of the object.
			vrs::DataPieceValue<vrs::Matrix4Dd> T_World_Object{"T_World_Object"};

			/// The timestamp of the transformation.
			vrs::DataPieceValue<double> timestamp{"timestamp"};

			/// The layout's end object.
			vrs::AutoDataLayoutEnd endLayout;
		};

		/**
		 * Creates a new recordable.
		 * @param recordableTypeId The type id of the recordable
		 * @param flavor The optional flavor of the recordable
		 * @param vrsRecordStartingTimestamp Optional VRS recording timestamp. If not specified, the current time will be used to create the configuration and state record. Range: (0, infinity)
		 */
		RecordableObjectBoundary(const vrs::RecordableTypeId recordableTypeId = vrs::RecordableTypeId::GroundTruthRecordableClass, const std::string& flavor = defaultFlavor(), const Timestamp& vrsRecordStartingTimestamp = Timestamp(false));

		/**
		 * Sets the per object properties for the object
		 * @param objectId The identifier id of the object
		 * @param boundary The boundary of the object in the object's frame of reference. For planes, y axis shouldbe 0
		 * @param semanticLabel The ID of the semantic label of the object
		 */
		void setObjectBoundaryConfig(const uint32_t objectId, const VectorsD3& boundary, const int semanticLabel);

		/**
		 * Adds a new pose value to this stream.
		 * @param T_World_Object The world pose, must be valid
		 * @param timestamp The timestamp of the pose
		 * @param recordTimestamp Optional VRS recording timestamp. If not specified, the current time will be used. Range: (-infinity, infinity)
		 */
		void addData(const HomogenousMatrix4& T_World_Object, const double timestamp, const double* recordTimestamp = nullptr);

		/**
		 * Returns whether this recodable is valid and configured.
		 * @return True, if succeeded
		 */
		bool isValid() const override;

		/**
		 * Returns the default flavor of this recoding.
		 * @return The recording's default flavor
		 */
		static std::string defaultFlavor();

		/**
		 * Returns the default recordable type id of this recording.
		 * @return The recording's default id
		 */
		static constexpr vrs::RecordableTypeId defaultRecordableTypeId();

	protected:

		/**
		 * Configuration records describe how the device recorded is configured/setup.
		 * @see Recordable::createConfigurationRecord().
		 */
		const vrs::Record* createConfigurationRecord() override;

		/**
		 * State records describe the internal state of the device, if it's stateful.
		 * @see Recordable::createStateRecord();
		 */
		const vrs::Record* createStateRecord() override;

	protected:

		/// The unique ID of the object
		uint32_t objectId_;

		/// The boundary of the object, defined in object's frame of reference
		std::vector<vrs::Point3Dd> boundary_;

		/// The semantic label, defined by ID of the label
		int semanticLabel_;

		/// The configuration layout.
		ConfigurationLayoutObjectBoundary configurationLayout_;

		/// The data layout.
		DataLayoutObjectBoundary dataLayout_;
};

constexpr vrs::RecordableTypeId RecordableCamera::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::ForwardCameraRecordableClass;
}

constexpr vrs::RecordableTypeId RecordableHomogenousMatrix4::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::PoseRecordableClass;
}

constexpr vrs::RecordableTypeId RecordableHomogenousMatrices4::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::PoseRecordableClass;
}

constexpr vrs::RecordableTypeId RecordableString::defaultRecordableTypeId()
{
	return vrs::RecordableTypeId::AnnotationRecordableClass;
}

}

}

}

#endif // META_OCEAN_IO_VRS_RENDERABLES_H
