/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_DEVICE_H
#define META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_DEVICE_H

#include "ocean/system/usb/video/Video.h"
#include "ocean/system/usb/video/VCDescriptor.h"
#include "ocean/system/usb/video/VSDescriptor.h"

#include "ocean/system/usb/Device.h"

#include "ocean/base/Memory.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

// Forward declaration.
class VideoDevice;

/**
 * Definition of a shared pointer holding a VideoDevice.
 * @see VideoDevice
 * @ingroup systemusbvideo
 */
using SharedVideoDevice = std::shared_ptr<VideoDevice>;

/**
 * This class implements a USB video device.
 * The device allows to select individual stream configurations, to start a stream, and to receive samples from the stream.
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VideoDevice : public Device
{
	protected:

		// Forward declaration.
		class PayloadHeader;

	public:

		/**
		 * Definition of individual USB UVC device stream types.
		 */
		enum DeviceStreamType : uint32_t
		{
			/// An invalid stream type.
			DST_INVALID = 0u,
			/// A stream composed of uncompressed video frames.
			DST_UNCOMPRESSED,
			/// A stream based on MJpeg.
			DST_MJPEG,
			/// A stream based on advanced compression like H.264 or H.265
			DST_FRAME_BASED
		};

		/**
		 * This class holds the data of a media sample.
		 */
		class OCEAN_SYSTEM_USB_VIDEO_EXPORT Sample
		{
			friend class VideoDevice;

			public:

				/**
				 * Default constructor creating an invalid sample object.
				 */
				Sample() = default;

				/**
				 * Creates a new sample associated with a specific stream configuration.
				 * @param capacity The maximal capacity of the sample, in bytes, with range [0, infinity)
				 * @param descriptorFormatIndex The video format index specifying the format descriptor of the stream delivering this sample, must be valid
		 		 * @param descriptorFrameIndex The video frame index specifying the frame descriptor of the stream delivering this sample, must be valid
				 * @param dwClockFrequency The clock frequency of the stream, in Hz, with range [1, infinity)
				 */
				explicit Sample(const size_t capacity, const uint8_t descriptorFormatIndex, const uint8_t descriptorFrameIndex, const uint32_t dwClockFrequency);

				/**
				 * Returns the video format index specifying the format descriptor of the stream delivering this sample.
				 * @return The sample's format index
				 */
				inline uint8_t descriptorFormatIndex() const;

				/**
				 * Returns the video frame index specifying the frame descriptor of the stream delivering this sample, must be valid.
				 * @return The sample's frame index
				 */
				inline uint8_t descriptorFrameIndex() const;

				/**
				 * Determines the timestamp the sample was captured.
				 * @return The sample's capture timestamp
				 */
				Timestamp determineCaptureTimestamp() const;

				/**
				 * Returns the buffer of this sample.
				 * @return The sample's buffer
				 */
				inline const void* data() const;

				/**
				 * Returns the size of the sample.
				 * @return The sample's size, in bytes
				 */
				inline size_t size() const;

				/**
				 * Returns whether this sample may contain errors (e.g., because there was a transmission error).
				 * @return True, if so
				 */
				inline bool mayContainError() const;

				/**
				 * Returns a string with the relevant information of this sample.
				 * @return The string with the relevant information
				 */
				std::string toString() const;

			protected:

				/**
				 * Appends a payload packet to the sample.
				 * @param payloadHeader The payload header of the packet
				 * @param data The actual payload data to append, must be valid
				 * @param size The size of the payload data to append, in bytes, with range [1, infinity)
				 * @return True, if succeeded
				 */
				bool append(const PayloadHeader& payloadHeader, const void* data, const size_t size);

				/**
				 * Resets the sample so that it can be reused.
				 */
				void reset();

			protected:

				/**
				 * Unwraps two timestamps while both are wrapped around 2^32.
				 * @param timestampA The first timestamp, with input range [0, 2^32-1] and output range [0, 2^33-1]
				 * @param timestampB The second timestamp, with input range [0, 2^32-1] and output range [0, 2^33-1]
				 * @return Which timestamp was unwrapped, 0 for timestampA, 1 for timestampB and -1 if no timestamp needed to be unwrapped
				 */
				static int unwrapTimestamps(uint64_t& timestampA, uint64_t& timestampB);

				/**
				 * Returns the earlier timestamp of the two given timestamps while both timestamps are wrapped around 2^32.
				 * @param timestampA The first timestamp, with range [0, 2^31-1]
				 * @param timestampB The second timestamp, with range [0, 2^31-1]
				 * @return The earlier of both timestamps
				 */
				static uint64_t earlierTimestamp(const uint64_t timestampA, const uint64_t timestampB);

			public:

				/// The payload buffer which can be appended with payload packets.
				std::vector<uint8_t> buffer_;

				/// The current position inside the payload buffer, with range [0, data_.size() - 1].
				size_t position_ = 0;

				/// True, if the sample may contain any error.
				bool mayContainError_ = false;

				/// The device's timestamp when capturing began (taken from dwPresentationTime).
				uint64_t captureDeviceTime_ = uint64_t(-1);

				/// Up to two device timestamps when the first and last payload packets were delivered/received (taken from scrSourceClock).
				uint64_t payloadDeviceTimes_[2] = {uint64_t(-1), uint64_t(-1)};

				/// Up to two host/local unix timestamps when the first and last payload packets were received.
				Timestamp payloadHostTimestamps_[2] = {Timestamp(false), Timestamp(false)};

				//// The index of the next device timestamps, with range [0, 1]
				size_t nextDeviceTimeIndex_ = 0;

				/// True, to allow the buffer to be resized if incoming data does not fit into the buffer.
				bool allowToResize_ = false;

				/// The video format index specifying the format descriptor of the stream delivering this sample.
				uint8_t descriptorFormatIndex_ = 0u;

				/// The video frame index specifying the frame descriptor of the stream delivering this sample.
				uint8_t descriptorFrameIndex_ = 0u;

				/// The dwClockFrequency field of the VideoProbe Control response in Hz (the frequency of captureDeviceTime_ and payloadDeviceTimes_), with range [1, infinity)
				uint32_t dwClockFrequency_ = 0u;
		};

		/**
		 * Definition of a shared pointer holding a Sample.
		 */
		using SharedSample = std::shared_ptr<Sample>;

		/**
		 * Definition of a vector holding samples.
		 */
		using Samples = std::vector<SharedSample>;

		/**
		 * Definition of a queue holding samples.
		 */
		using SampleQueue = std::queue<SharedSample>;

		/// The maximal size of the sample queue before queued samples will be dropped.
		static constexpr size_t maximalSampleQueueSize_ = 10;

		/**
		 * This class combines the video control interface functionality.
		 */
		class OCEAN_SYSTEM_USB_VIDEO_EXPORT VideoControlInterface
		{
			friend class VideoDevice;

			public:

				/**
				 * Returns whether this interface object is valid
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Default constructor creating an invalid interface.
				 */
				VideoControlInterface() = default;

				/**
				 * Creates a new video control interface based on a given interface descriptor.
				 * @param interfaceDescriptor The descriptor of the interface based on which this object will be created
				 * @param usbDeviceHandle The handle to the USB device associated with given interface descriptor
				 */
				VideoControlInterface(const libusb_interface_descriptor& interfaceDescriptor, libusb_device_handle* usbDeviceHandle);

			protected:

				/// The descriptor of the header interface.
				VCHeaderDescriptor vcHeaderDescriptor_;

				/// The input terminal descriptors.
				std::vector<VCInputTerminalDescriptor> vcInputTerminalDescriptors_;

				/// The selector unit descriptors.
				std::vector<VCSelectorUnitDescriptor> vcSelectorUnitDescriptors_;

				/// The processing unit descriptors.
				std::vector<VCProcessingUnitDescriptor> vcProcessingUnitDescriptors_;

				/// The extension unit descriptors.
				std::vector<VCExtensionUnitDescriptor> vcExtensionUnitDescriptors_;

				/// The index of the control interface.
				uint8_t bInterfaceIndex_ = 0u;

				/// The endpoint of the control interface.
				uint8_t bEndpointAddress_ = 0u;

				/// True, if this control interface object is valid.
				bool isValid_ = false;
		};

		/**
		 * This class combines the video streaming interface functionality.
		 */
		class OCEAN_SYSTEM_USB_VIDEO_EXPORT VideoStreamingInterface
		{
			friend class VideoDevice;

			public:

				/**
				 * Definition of a priority triple combining format index, frame index, and frame interval of a stream.
				 */
				using PriorityTriple = Triple<uint8_t, uint8_t, uint32_t>;

				/**
				 * Definition of an ordered map mapping priorities to triples combining formats, frames, and intervals.
				 */
				using PriorityMap = std::map<float, PriorityTriple>;

			public:

				/**
				 * This class implements the base class for all video streams.
				 */
				class VideoStream
				{
					public:

						/// The color matching descriptor of the stream.
						VSColorMatchingDescriptor vsColorMatchingDescriptor_;
				};

				/**
				 * This class implements an uncompressed video stream.
				 */
				class UncompressedVideoStream : public VideoStream
				{
					public:

						/// The format descriptor of the uncompressed stream.
						VSUncompressedVideoFormatDescriptor vsUncompressedVideoFormatDescriptor_;

						/// The frame descriptors of the uncompressed stream.
						std::vector<VSUncompressedVideoFrameDescriptor> vsUncompressedVideoFrameDescriptors_;
				};

				/**
				 * This class implements a frame-based video stream.
				 */
				class FrameBasedVideoStream : public VideoStream
				{
					public:

						/// The format descriptor of the frame-based stream.
						VSFrameBasedVideoFormatDescriptor vsFrameBasedVideoFormatDescriptor_;

						/// The frame descriptors of the frame-based stream.
						std::vector<VSFrameBasedFrameDescriptor> vsFrameBasedFrameDescriptors_;
				};

				/**
				 * This class implements a Mjpeg video stream.
				 */
				class MJPEGVideoStream : public VideoStream
				{
					public:

						/// The format descriptor of the Mjpeg stream.
						VSMJPEGVideoFormatDescriptor vsMJPEGVideoFormatDescriptor_;

						/// The frame descriptors of the Mjpeg stream.
						std::vector<VSMJPEGVideoFrameDescriptor> vsMJPEGVideoFrameDescriptors_;
				};

				/**
				 * Definition of a vector holding UncompressedVideoStream objects.
				 */
				using UncompressedVideoStreams = std::vector<UncompressedVideoStream>;

				/**
				 * Definition of a vector holding FrameBasedVideoStream objects.
				 */
				using FrameBasedVideoStreams = std::vector<FrameBasedVideoStream>;

				/**
				 * Definition of a vector holding MJPEGVideoStream objects.
				 */
				using MJPEGVideoStreams = std::vector<MJPEGVideoStream>;

			public:

				/**
				 * Returns the available uncompressed video streams.
				 * @return The interface's uncompressed video streams
				 */
				inline const UncompressedVideoStreams& uncompressedVideoStreams() const;

				/**
				 * Returns the available frame-based video streams.
				 * @return The interface's frame-based video streams
				 */
				inline const FrameBasedVideoStreams& frameBasedVideoStreams() const;

				/**
				 * Returns the available Mjpeg video streams.
				 * @return The interface's Mjpeg video streams
				 */
				inline const MJPEGVideoStreams& mjpegVideoStreams() const;

				/**
				 * Returns a priority map with best matching stream configurations.
				 * @param preferredWidth The preferred width, in pixel, 0 to use a default width, with range [0, infinity)
				 * @param preferredHeight The preferred height, in pixel, 0 to use a default height, with range [0, infinity)
				 * @param preferredFrameRate The preferred frame rate, in Hz, 0 to use a default frame rate, with range [0, infinity)
				 * @param deviceStreamType The stream type to be used, ST_INVALID to allow any stream type
				 * @param pixelFormat The pixel format which needs to be used in case of an uncompressed stream, FORMAT_UNDEFINED to use any pixel format
				 * @param encodingFormat The encoding format which needs to be used in case of an frame-based stream, EF_INVALID to allow any encoding format
				 * @return The map with best matching stream configurations, the higher the key the better the match
				 */
				PriorityMap findBestMatchingStream(const unsigned int preferredWidth, const unsigned int preferredHeight, const double preferredFrameRate, const DeviceStreamType deviceStreamType = DST_INVALID, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, VSFrameBasedVideoFormatDescriptor::EncodingFormat encodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID);

				/**
				 * Returns whether this interface object is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Creates an invalid interface object.
				 */
				VideoStreamingInterface() = default;

				/**
				 * Parses an additional interface and adds it to this streaming interface object.
				 * @param interfaceDescriptor The descriptor of the interface to be added
				 * @param usbDeviceHandle The handle to the USB device associated with given interface descriptor
				 */
				bool parseAdditionalInterface(const libusb_interface_descriptor& interfaceDescriptor, libusb_device_handle* usbDeviceHandle);

				/**
				 * Returns a factor describing how well the resolution of a descriptor matches a preferred image resolution.
				 * @param frameDescriptor The frame descriptor for which the factor will be determined
				 * @param preferredWidth The preferred width, in pixel, with range [0, infinity)
				 * @param preferredHeight The preferred height, in pixel, with range [0, infinity)
				 * @param defaultPixels The default number of pixels which is used as a reference resolution in case preferred width and height are zero, with range [1, infinity)
				 * @return The matching factor, the higher the better the match, with range [0, 1]
				 * @tparam T The data type of the descriptor
				 */
				template <typename T>
				static float determineResolutionFactor(const T& frameDescriptor, const unsigned int preferredWidth, const unsigned int preferredHeight, const unsigned int defaultPixels = 1280u * 720u);

				/**
				 * Returns a factor describing how well one of the frame rates of a descriptor matches a preferred frame rate.
				 * @param frameDescriptor The frame descriptor for which the factor will be determined
				 * @param preferredFrameRate The preferred frame rate, in Hz, with range [0, infinity)
				 * @param frameInterval The resulting best matching frame interval, in 100ns
				 * @param defaultFrameRate The default frame rate which is used as a reference frame rate in case the preferred frame rate is zero, with range (0, infinity)
				 * @return The matching factor, the higher the better the match, with range [0, 1]
				 * @tparam T The data type of the descriptor
				 */
				template <typename T>
				static float determineFrameRateFactor(const T& frameDescriptor, const double preferredFrameRate, uint32_t& frameInterval, const double defaultFrameRate);

			protected:

				/// The descriptor of the input header interface.
				VSInputHeaderDescriptor vsInputHeaderDescriptor_;

				/// The uncompressed video streams.
				UncompressedVideoStreams uncompressedVideoStreams_;

				/// The frame-based video streams.
				FrameBasedVideoStreams frameBasedVideoStreams_;

				/// The Mjpeg-based video streams.
				MJPEGVideoStreams mjpegVideoStreams_;

				/// The index of the control interface.
				uint8_t bInterfaceIndex_ = 0u;

				/// The endpoint of the control interface.
				uint8_t bEndpointAddress_ = 0u;
		};

	protected:

		/// The maximal size the memory of a sample can have, in bytes.
		static constexpr size_t sampleMaxPayloadSize_ = 1024 * 1024 * 10;

		/**
		 * Definition of USB UVC Video Interface Subclass Codes.
		 */
		enum VideoInterfaceSubclass : uint8_t
		{
			/// Undefined
			SC_UNDEFINED = 0x00u,
			/// This code is used for interfaces that manage control operations for video streaming devices.
			SC_VIDEOCONTROL = 0x01u,
			/// This code is used for interfaces that handle video streaming data.
			SC_VIDEOSTREAMING = 0x02u,
			/// This code is used for a collection of video interfaces that are grouped together to form a single functional entity.
			SC_VIDEO_INTERFACE_COLLECTION = 0x03u
		};

		enum ControlSelectors : uint8_t
		{
			VS_CONTROL_UNDEFINED = 0x00u,
			/// Video Probe control.
			VS_PROBE_CONTROL = 0x01u,
			/// Video commit control.
			VS_COMMIT_CONTROL = 0x02u,
			VS_STILL_PROBE_CONTROL = 0x03u,
			VS_STILL_COMMIT_CONTROL = 0x04u,
			VS_STILL_IMAGE_TRIGGER_CONTROL = 0x05u,
			VS_STREAM_ERROR_CODE_CONTROL = 0x06u,
			VS_GENERATE_KEY_FRAME_CONTROL = 0x07u,
			VS_UPDATE_FRAME_SEGMENT_CONTROL = 0x08u,
			VS_SYNCH_DELAY_CONTROL = 0x09u
		};

#pragma pack(push)
#pragma pack(1)

		/**
		 * Video Probe and Commit Controls.
		 *
		 * The streaming parameters selection process is based on a shared negotiation model between the host and the video streaming interface, taking into account the following features:
		 * - shared nature of the USB
		 * - interdependency of streaming parameters
		 * - payload independence
		 * - modification of streaming parameters during streaming
		 * This negotiation model is supported by the Video Probe and Commit controls.
		 * The Probe control allows retrieval and negotiation of streaming parameters.
		 * When an acceptable combination of streaming parameters has been obtained, the Commit control is used to configure the hardware with the negotiated parameters from the Probe control
		 *
		 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
		 */
		class OCEAN_SYSTEM_USB_VIDEO_EXPORT VideoControl
		{
			public:

				/**
				 * Definition of the individual Video Class-Specific Request Codes.
				 */
				enum RequestCode : uint8_t
				{
					/// Undefined request code.
					RC_UNDEFINED = 0u,
					/// Set current.
					RC_SET_CUR = 0x01u,
					/// Get current.
					RC_GET_CUR = 0x81u,
					/// Get minimum.
					RC_GET_MIN = 0x82u,
					/// Get maximum.
					RC_GET_MAX = 0x83u,
					/// Get resolution.
					RC_GET_RES = 0x84u,
					/// Get length.
					RC_GET_LEN = 0x85u,
					/// Get information.
					RC_GET_INFO = 0x86u,
					/// Get default.
					RC_GET_DEF = 0x87u
				};

				/**
				 * Returns a string with the content of this object.
				 * @return The string holding the content of this object
				 */
				std::string toString() const;

				/**
				 * Executes a video commit control request.
				 * @param usbDeviceHandle The hand of the USB device to which the request will be set
				 * @param interfaceIndex The index of the interface to which the control request will be sent
				 * @param videoControl The control to be sent, must be valid
				 * @param videoControlSize The size of the control to be sent, in bytes, either 26 or 34
				 * @param bRequest The request parameter
				 * @return True, if succeeded
				 */
				static bool executeVideoControlCommit(libusb_device_handle* usbDeviceHandle, const uint8_t interfaceIndex, const VideoControl& videoControl, const size_t videoControlSize, const uint8_t bRequest = RC_SET_CUR);

				/**
				 * Executes a video probe control request.
				 * @param usbDeviceHandle The hand of the USB device to which the request will be set
				 * @param interfaceIndex The index of the interface to which the control request will be sent
				 * @param videoControl The resulting control
				 * @param videoControlSize The size of the control to be sent, in bytes, either 26 or 34
				 * @param bRequest The request parameter
				 * @return True, if succeeded
				 */
				static bool executeVideoControlProbe(libusb_device_handle* usbDeviceHandle, const uint8_t interfaceIndex, VideoControl& videoControl, const size_t videoControlSize, const uint8_t bRequest = RC_GET_CUR);

				/**
				 * Executes a video control commit or probe request.
				 * @param usbDeviceHandle The hand of the USB device to which the request will be set
				 * @param bmRequestType The request type parameter
				 * @param bRequest The request parameter
				 * @param wValue The value parameter
				 * @param wIndex The index parameter
				 * @param buffer The control buffer, must be valid
				 * @param size The size of the control buffer, in bytes, either 26 or 34
				 * @return True, if succeeded
				 */
				static bool executeVideoControl(libusb_device_handle* usbDeviceHandle, const uint8_t bmRequestType, const uint8_t bRequest, const uint16_t wValue, const uint16_t wIndex, uint8_t* buffer, const size_t size);

			public:

				/**
				 * Bitfield control indicating to the function what fields shall be kept fixed (indicative only):
				 * D0: dwFrameInterval
				 * D1: wKeyFrameRate
				 * D2: wPFrameRate
				 * D3: wCompQuality
				 * D4: wCompWindowSize
				 * D15..5: Reserved (0)
				 *
				 * The hint bitmap indicates to the video streaming interface which fields shall be kept constant during stream parameter negotiation.
				 * For example, if the selection wants to favor frame rate over quality, the dwFrameInterval bit will be set (1).
				 *
				 * This field is set by the host, and is read-only for the video streaming interface.
				 */
				uint16_t bmHint_ = 0u;

				/**
				 * Video format index from a format descriptor.
				 *
				 * Select a specific video stream format by setting this field to the one-based index of the associated format descriptor.
				 * To select the first format defined by a device, a value one (1) is written to this field. This field must be supported even if only one video format is supported by the device.
				 *
				 * This field is set by the host.
				 */
				uint8_t bFormatIndex_ = 0u;

				/**
				 * Video frame index from a frame descriptor.
				 *
				 * This field selects the video frame resolution from the array of resolutions supported by the selected stream.
				 * The index value ranges from 1 to the number of Frame descriptors following a particular Format descriptor.
				 * This field must be supported even if only one video frame index is supported by the device.
				 *
				 * For video payloads with no defined frame descriptor, this field shall be set to zero (0).
				 *
				 * This field is set by the host.
				 */
				uint8_t bFrameIndex_ = 0u;

				/**
				 * Frame interval in 100 ns units.
				 *
				 * This field sets the desired video frame interval for the selected video stream and frame index.
				 * The frame interval value is specified in 100 ns units.
				 * The device shall support the setting of all frame intervals reported in the Frame Descriptor corresponding to the selected Video Frame Index.
				 * This field must be implemented even if only one video frame interval is supported by the device.
				 *
				 * When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase.
				 * The value must be from the range of values supported by the device.
				 *
				 * When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
				 */
				uint32_t dwFrameInterval_ = 0u;

				/**
				 * Key frame rate in key-frame per videoframe units.
				 *
				 * This field is only applicable to sources (and formats) capable of streaming video with adjustable compression parameters.
				 * Use of this control is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
				 *
				 * The Key Frame Rate field is used to specify the compressor’s key frame rate.
				 * For example, if one of every ten encoded frames in a video stream sequence is a key frame, this control would report a value of 10.
				 * A value of 0 indicates that only the first frame is a key frame.
				 *
				 * When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase.
				 * The value must be from the range of values supported by the device.
				 *
				 * When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
				 */
				uint16_t wKeyFrameRate_ = 0u;

				/**
				 * PFrame rate in PFrame/key frame units.
				 *
				 * This field is only applicable to sources (and formats) capable of streaming video with adjustable compression parameters.
				 * Use of this control is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
				 *
				 * The P Frame Rate Control is used to specify the number of P frames per key frame.
				 * As an example of the relationship between the types of encoded frames, suppose a key frame occurs once in every 10 frames, and there are 3 P frames per key frame.
				 * The P frames will be spaced evenly between the key frames.
				 * The other 6 frames, which occur between the key frames and the P frames, will be bi-directional (B) frames.
				 *
				 * When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase.
				 * The value must be from the range of values supported by the device.
				 *
				 * When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
				 */
				uint16_t wPFrameRate_ = 0u;

				/**
				 * Compression quality control in abstract units 0 (lowest) to 10000 (highest).
				 *
				 * This field is only applicable to sources (and formats) capable of streaming video with adjustable compression parameters.
				 * Use of this field is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
				 *
				 * This field is used to specify the quality of the video compression.
				 * Values for this property range from 0 to 10000 (0 indicates the lowest quality, 10000 the highest).
				 * The resolution reported by this control will determine the number of discrete quality settings that it can support.
				 *
				 * When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase.
				 * The value must be from the range of values supported by the device.
				 *
				 * When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
				 */
				uint16_t wCompQuality_ = 0u;

				/**
				 * Window size for average bit rate control.
				 *
				 * This field is only applicable to sources (and formats) capable of streaming video with adjustable compression parameters.
				 * Use of this control is at the discretion of the device, and is indicated in the VS Input or Output Header descriptor.
				 *
				 * The Compression Window Size Control is used to specify the number of encoded video frames over which the average size cannot exceed the specified data rate.
				 * For a window of size n, the average frame size of any consecutive n frames will not exceed the stream's specified data rate.
				 * Individual frames can be larger or smaller.
				 *
				 * For example, if the data rate has been set to 100 kilobytes per second (KBps) on a 10 frames per second (fps) movie with a compression window size of 10,
				 * the individual frames can be any size, as long as the average size of a frame in any 10-frame sequence is less than or equal to 10 kilobytes.
				 *
				 * When used in conjunction with an IN endpoint, the host shall indicate its preference during the Probe phase.
				 * The value must be from the range of values supported by the device.
				 *
				 * When used in conjunction with an OUT endpoint, the host shall accept the value indicated by the device.
				 */
				uint16_t wCompWindowSize_ = 0u;

				/**
				 * Internal video streaming interface latency in ms from video data capture to presentation on the USB.
				 *
				 *  When used in conjunction with an IN endpoint, this field is set by the device and read only from the host.
				 *
				 * When used in conjunction with an OUT endpoint, this field is set by the host and read only from the device.
				 */
				uint16_t wDelay_ = 0u;

				/**
				 * Maximum video frame or codec-specific segment size in bytes.
				 *
				 * For frame-based formats, this field indicates the maximum size of a single video frame.
				 *
				 * For stream-based formats, and when this behavior is enabled via the bmFramingInfo field (below), this field indicates the maximum size of a single codec-specific segment.
				 * The sender is required to indicate a segment boundary via the FID bit in the payload header.
				 * This field is ignored (for stream-based formats) if the bmFramingInfo bits are not enabled.
				 *
				 * When used in conjunction with an IN endpoint, this field is set by the device and read only from the host.
				 *
				 * When used in conjunction with an OUT endpoint, this field is set by the host and read only from the device.
				 */
				uint32_t dwMaxVideoFrameSize_ = 0u;

				/**
				 * Specifies the maximum number of bytes that the device can transmit or receive in a single payload transfer.
				 *
				 * This field is set by the device and read only from the host.
				 * Some host implementations restrict the maximum value permitted for this field.
				 */
				uint32_t dwMaxPayloadTransferSize_ = 0u;

				/**
				 * The device clock frequency in Hz for the specified format.
				 * This will specify the units used for the time information fields in the Video Payload Headers in the data stream.
				 *
				 * This parameter is set by the device and read only from the host.
				 */
				uint32_t dwClockFrequency_ = 0u;

				/**
				 * Bitfield control supporting the following values:
				 * D0:
				 * If set to 1, the Frame ID (FID) field is required in the Payload Header (see description of D0 in section 2.4.3.3, “Video and Still Image Payload Headers”).
				 * The sender is required to toggle the Frame ID at least every 'dwMaxVideoFrameSize' bytes (see above).
				 *
				 * D1:
				 * If set to 1, indicates that the End of Frame (EOF) field may be present in the Payload Header (see description of D1 in section 2.4.3.3, “Video and Still Image Payload Headers”).
				 * It is an error to specify this bit without also specifying D0.
				 *
				 * D7..2: Reserved (0)
				 *
				 * This control indicates to the function whether payload transfers will contain out-of-band framing information in the Video Payload Header (see section 2.4.3.3, “Video and Still Image Payload Headers”).
				 * For known frame-based formats (e.g., MJPEG, Uncompressed, DV), this control is ignored.
				 * For known stream-based formats, this control allows the sender to indicate that it will identify segment boundaries in the stream, enabling low-latency buffer handling by the receiver without the overhead of parsing the stream itself.
				 * When used in conjunction with an IN endpoint, this control is set by the device, and is read-only from the host.
				 * When used in conjunction with an OUT endpoint, this parameter is set by the host, and is read-only from the device.
				 */
				uint8_t bmFramingInfo_ = 0u;

				/**
				 * The preferred payload format version supported by the host or device for the specified bFormatIndex value.
				 *
				 * This parameter allows the host and device to negotiate a mutually agreed version of the payload format associated with the bFormatIndex field.
				 * The host initializes this and the following bMinVersion and bMaxVersion fields to zero on the first Probe Set.
				 * Upon Probe Get, the device shall return its preferred version, plus the minimum and maximum versions supported by the device (see bMinVersion and bMaxVersion below).
				 * The host may issue a subsequent Probe Set/Get sequence to specify its preferred version (within the ranges returned in bMinVersion and bMaxVersion from the initial Probe Set/Get sequence).
				 *
				 * The host is not permitted to alter the bMinVersion and bMaxVersion values.
				 *
				 * This field will support up to 256 (0-255) versions of a single payload format.
				 * The version number is drawn from the minor version of the Payload Format specification. For example, version 1.2 of a Payload Format specification would result in a value of 2 for this parameter.
				 */
				uint8_t bPreferedVersion_ = 0u;

				/**
				 * The minimum payload format version supported by the device for the specified bFormatIndex value.
				 *
				 * This value is initialized to zero by the host and reset to a value in the range of 0 to 255 by the device.
				 * The host is not permitted to modify this value (other than to restart the negotiation by setting bPreferredVersion, bMinVersion and bMaxVersion to zero).
				 */
				uint8_t bMinVersion_ = 0u;

				/**
				 * The maximum payload format version supported by the device for the specified bFormatIndex value.
				 *
				 * This value is initialized to zero by the host and reset to a value in the range of 0 to 255 by the device.
				 * The host is not permitted to modify this value (other than to restart the negotiation by setting bPreferredVersion, bMinVersion and bMaxVersion to zero).
				 */
				uint8_t bMaxVersion_ = 0u;
		};

#pragma pack(pop)

		static_assert(sizeof(VideoControl) == 34, "Invalid struct size!");

		/**
		 * Video and Still Image Payload Headers.
		 * Every Payload Transfer containing video or still-image sample data must start with a Payload Header.
		 *
		 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
		 */
		class OCEAN_SYSTEM_USB_VIDEO_EXPORT PayloadHeader
		{
			public:

				/**
				 * Creates a new payload header object from memory.
				 * @param buffer The memory buffer in which the header is located, must be valid
				 * @param size The size of the buffer, in bytes, with range [1, infinity)
				 */
				PayloadHeader(const uint8_t* buffer, const size_t size);

				/**
				 * Returns whether the frame id toggling between 0 and 1 every time a new video frame begins.
				 * For frame-based formats, this bit toggles between 0 and 1 every time a new video frame begins.
				 * For stream-based formats, this bit toggles between 0 and 1 at the start of each new codec-specific segment.
				 * @return The frame id, with range [0, 1]
				 */
				inline uint8_t toggledFrameId() const;

				/**
				 * Returns whether the payload marks the end of the current video or still image frame.
				 * @return True, if so
				 */
				inline bool isEndOfFrame() const;

				/**
				 * Returns whether the dwPresentationTime field is being sent as part of the header.
				 * @return True, if so
				 */
				inline bool hasPresentationTime() const;

				/**
				 * Returns whether the dwSourceClock field is being sent as part of the header.
				 * @return True, if so
				 */
				inline bool hasSourceClockReference() const;

				/**
				 * Returns whether the following data is part of a still image frame, and is only used for methods 2 and 3 of still image capture.
				 */
				inline bool isStillImage() const;

				/**
				 * Returns whether there was an error in the video or still image transmission for this payload.
				 * @return True, if so
				 */
				inline bool hasError() const;

				/**
				 * Returns whether the payload is the last header group in the packet.
				 * @return True, if so
				 */
				inline bool isEndOfHeader() const;

				/**
				 * Returns the presentation time stamp (PTS).
				 * @return The presentation time stamp, if hasPresentationTime() == true
				 */
				inline uint32_t sourceTimeClock() const;

				/**
				 * Returns the bus frame counter part of the scrSourceClock field.
				 * @return The bus frame counter, with range [0, 2^11-1], if hasPresentationTime() == true
				 */
				inline uint16_t busFrameCounter() const;

			public:

				/**
				 * Length of the payload header in bytes including this field.
				 */
				uint8_t bHeaderLength_ = 0u;

				/**
				 * Provides information on the sample data following the header, as well as the availability of optional header fields in this header.
				 *
				 * D0:
				 * Frame ID – For frame-based formats, this bit toggles between 0 and 1 every time a new video frame begins.
				 * For stream-based formats, this bit toggles between 0 and 1 at the start of each new codec-specific segment.
				 * This behavior is required for frame-based payload formats (e.g., DV) and is optional for stream-based payload formats (e.g., MPEG-2 TS).
				 * For stream-based formats, support for this bit must be indicated via the bmFramingInfo field of the Video Probe and Commit controls (see section 4.3.1.1, “Video Probe and Commit Controls”).
				 *
				 * D1:
				 * End of Frame – This bit is set if the following payload data marks the end of the current video or still image frame (for frame- based formats), or to indicate the end of a codec-specific segment (for stream-based formats).
				 * This behavior is optional for all payload formats. For stream-based formats, support for this bit must be indicated via the bmFramingInfo field of the Video Probe and Commit Controls (see section 4.3.1.1, “Video Probe and Commit Controls”).
				 *
				 * D2:
				 * Presentation Time – This bit is set if the dwPresentationTime field is being sent as part of the header.
				 *
				 * D3:
				 * Source Clock Reference – This bit is set if the dwSourceClock field is being sent as part of the header.
				 *
				 * D4:
				 * Reserved
				 *
				 * D5:
				 * Still Image – This bit is set if the following data is part of a still image frame, and is only used for methods 2 and 3 of still image capture.
				 *
				 * D6:
				 * Error – This bit is set if there was an error in the video or still image transmission for this payload.
				 * The Stream Error Code control would reflect the cause of the error.
				 *
				 * D7:
				 * End of header – This bit is set if this is the last header group in the packet, where the header group refers to this field and any optional fields identified by the bits in this field (Defined for future extension).
				 */
				uint8_t bmHeaderInfo_ = 0u;

				/**
				 * Presentation Time Stamp (PTS).
				 *
				 * The source clock time in native device clock units when the raw frame capture begins.
				 * This field may be repeated for multiple payload transfers comprising a single video frame, with the restriction that the value shall remain the same throughout that video frame.
				 * The PTS is in the same units as specified in the dwClockFrequency field of the Video Probe Control response.
				 */
				uint32_t dwPresentationTime_ = 0u;

				/**
				 * A two-part Source Clock Reference (SCR) value.
				 *
				 * D31..D0:
				 * Source Time Clock in native device clock units.
				 *
				 * D42..D32:
				 * 1KHz SOF token counter
				 *
				 * D47..D43: Reserved, set to zero.
				 *
				 * The least-significant 32 bits (D31..D0) contain clock values sampled from the System Time Clock (STC) at the source.
				 * The clock resolution shall be according to the dwClockFrequency field of the Probe and Commit response of the device as defined in Table 4-47 of this specification.
				 * This value shall comply with the associated stream payload specification.
				 *
				 * The times at which the STC is sampled must be correlated with the USB Bus Clock.
				 * To that end, the next most significant 11 bits of the SCR (D42..D32) contain a 1 KHz SOF counter, representing the frame number at the time the STC was sampled.
				 * The STC is sampled at arbitrary SOF boundaries.
				 * The SOF counter is the same size and frequency as the frame number associated with USB SOF tokens; however it is not required to match the current frame number.
				 * This allows implementations using a chipset that can trigger on SOF tokens (but not accurately obtain the Frame number) to keep their own frame counters.
				 *
				 * The most-significant 5 bits (D47..D43) are reserved, and must be set to zero.
				 * The maximum interval between Payload Headers containing SCR values is 100ms, or the video frame interval, whichever is greater. Shorter intervals are permitted.
				 */
				uint8_t scrSourceClock_[6];
		};

	public:

		/**
		 * Default constructor creating an invalid video device.
		 */
		VideoDevice() = default;

		/**
		 * Creates a new video device based on a valid device.
		 * @param device The device which will be converted into a video device, must be valid
		 */
		explicit VideoDevice(Device&& device);

		/**
		 * Destructs this video device and releases all associated resources.
		 */
		~VideoDevice() override;

		/**
		 * Extracts the relevant properties for a specific stream this device supports.
		 * @param descriptorFormatIndex The video format index specifying the format descriptor of the stream delivering this sample, must be valid
		 * @param descriptorFrameIndex The video frame index specifying the frame descriptor of the stream delivering this sample, must be valid
		 * @param width The resulting width of the stream, in pixels, with range [1, infinity)
		 * @param height The resulting height of the stream, in pixels, with range [1, infinity)
		 * @param pixelFormat The resulting pixel format of the stream, valid only if stream type is DST_UNCOMPRESSED
		 * @param encodingFormat The resulting encoding format of the stream, valid only if stream type is DST_FRAME_BASED
		 * @return The resulting stream type, DST_INVALID if the properties could not be extracted
		 */
		DeviceStreamType extractStreamProperties(const uint8_t descriptorFormatIndex, const uint8_t descriptorFrameIndex, unsigned int& width, unsigned int& height, FrameType::PixelFormat& pixelFormat, VSFrameBasedVideoFormatDescriptor::EncodingFormat& encodingFormat) const;

		/**
		 * Starts the video device.
		 * In case, the device is already started, nothing will happen.
		 * @param preferredWidth The preferred width of the stream, in pixel, with range [1, infinity), 0 to use a default width
		 * @param preferredHeight The preferred height of the stream, in pixel, with range [1, infinity), 0 to use a default height
		 * @param preferredFrameRate The preferred frame rate, in Hz, with range (0, infinity), 0 to use a default frame rate
		 * @param preferredDeviceStreamType The preferred device stream type, DST_INVALID to use a default stream type
		 * @param preferredPixelFormat The preferred pixel format, will be ignored for streams which do not have a device stream type DST_UNCOMPRESSED
		 * @param preferredEncodingFormat The preferred encoding format, will be ignored for streams which do not have a device stream type DST_FRAME_BASED
		 * @return True, if the stream could be started
		 * @see isStarted(), stopStream(), extractStreamProperties().
		 */
		bool start(const unsigned int preferredWidth = 0u, const unsigned int preferredHeight = 0u, const double preferredFrameRate = 0.0, const DeviceStreamType preferredDeviceStreamType = DST_INVALID, const FrameType::PixelFormat preferredPixelFormat = FrameType::FORMAT_UNDEFINED, const VSFrameBasedVideoFormatDescriptor::EncodingFormat preferredEncodingFormat = VSFrameBasedVideoFormatDescriptor::EF_INVALID);

		/**
		 * Stops the video device.
		 * @return True, if succeeded
		 */
		bool stop();

		/**
		 * Returns the next sample from this device.
		 * Once a sample has been processed, it should be given back to the device so that it can be reused by the device.
		 * @param pendingSamples Optional resulting number of samples which are still in the queue (not including the returned sample), nullptr if not of interest
		 * @return The next sample, nullptr if currently no sample is available
		 * @see giveSampleBack().
		 */
		SharedSample nextSample(size_t* pendingSamples = nullptr);

		/**
		 * Gives a sample back to this device so that it can be reused by the device.
		 * @param sample The sample to give back, must be valid
		 * @see nextSample().
		 */
		void giveSampleBack(SharedSample&& sample);

		/**
		 * Returns the video streaming interface functionality of this device.
		 * @return The device's video streaming interface
		 */
		inline const VideoStreamingInterface& videoStreamingInterface() const;

		/**
		 * Returns whether this video device has an active stream.
		 * @return True, if so
		 * @see startStream().
		 */
		inline bool isStarted() const;

		/**
		 * Returns whether this video device is valid and ready to be used.
		 * @return True, if so; False, if e.g., not all devices of the USB device could be accessed
		 */
		inline bool isValid() const;

		/**
		 * Translates a device stream type to a string.
		 * @param deviceStreamType The stream type to translate
		 * @return The translated type as string, 'Invalid' if the type is invalid or unknown
		 */
		static std::string translateDeviceStreamType(const DeviceStreamType deviceStreamType);

	protected:

		/**
		 * Initializes the control interface.
		 * @return True, if succeeded
		 */
		bool initializeControlInterface();

		/**
		 * Parses all interfaces of this device.
		 * @return True, if succeeded
		 */
		bool parseInterfaces();

		/**
		 * Parses a video interface of this device.
		 * @param interfaceDescriptor The libusb interface descriptor of a video interface to parse, with bInterfaceClass == LIBUSB_CLASS_VIDEO
		 * @return True, if succeeded
		 */
		bool parseVideoInterface(const libusb_interface_descriptor& interfaceDescriptor);

		/**
		 * Processes the payload which has been received from the device via a USB transfer.
		 * @param bufferPointers The buffers holding the payload, at least one
		 */
		void processPayload(const BufferPointers& bufferPointers);

		/**
		 * Libusb status transfer callback function.
		 * @param usbTransfer The transfer object which has been completed, failed, or canceled.
		 * @return True, the transfer should be resubmitted
		 */
		bool libStatusCallback(libusb_transfer& usbTransfer);

		/**
		 * Libusb stream transfer callback function.
		 * @param usbTransfer The transfer object which has been completed, failed, or canceled.
		 * @return True, the transfer should be resubmitted
		 */
		bool libusbStreamCallback(libusb_transfer& usbTransfer);

		/**
		 * Static libusb status transfer callback function.
		 * @param usbTransfer The transfer object which has been completed, failed, or canceled.
		 */
		static void LIBUSB_CALL libStatusCallback(libusb_transfer* usbTransfer);

		/**
		 * Static libusb stream transfer callback function.
		 * @param usbTransfer The transfer object which has been completed, failed, or canceled.
		 */
		static void LIBUSB_CALL libusbStreamCallback(libusb_transfer* usbTransfer);

		/**
		 * Disabled copy constructor.
		 */
		VideoDevice(const VideoDevice&) = delete;

		/**
		 * Disabled move constructor.
		 */
		VideoDevice(VideoDevice&&) = delete;

		/**
		 * Disabled copy operator.
		 * @return Reference to this object
		 */
		VideoDevice& operator=(const VideoDevice&) = delete;

		/**
		 * Disabled move operator.
		 * @return Reference to this object
		 */
		VideoDevice& operator=(VideoDevice&&) = delete;

	public:

		/// The video control interface of this device.
		VideoControlInterface videoControlInterface_;

		/// The video streaming interface of this device.
		VideoStreamingInterface videoStreamingInterface_;

		/// True, if the control interface has been initialized.
		bool initializeControlInterfaceInitialized_ = false;

		/// The libusb device configuration descriptor, nullptr if not yet initialized
		libusb_config_descriptor* usbConfigDescriptor_ = nullptr;

		/// The index of the descriptor format which is currently active/streaming.
		uint8_t activeDescriptorFormatIndex_ = 0u;

		/// The index of the descriptor frame which is currently active/streaming.
		uint8_t activeDescriptorFrameIndex_ = 0u;

		/// The clock frequency of the active stream.
		uint32_t activeClockFrequency_ = 0u;

		/// The maximal size of one sample.
		size_t maximalSampleSize_ = 0;

		/// The transfer object for interrupts.
		ScopedTransfer interruptTransfer_;

		/// The streaming transfer objects.
		ScopedTransfers streamingTransfers_;

		/// The map mapping transfer pointers to indices.
		TransferIndexMap transferIndexMap_;

		/// The memory for the individual streaming transfer objects.
		std::vector<Memory> streamingTransferMemories_;

		/// True, if the video device has an active stream which has been started.
		bool isStarted_ = false;

		/// The currently active sample which is receiving sample data from the device (but not yet filled).
		SharedSample activeSample_;

		/// The queue with all processed samples waiting to be grabbed by an external entity.
		SampleQueue sampleQueue_;

		/// The reusable samples which waiting to be filled.
		Samples reusableSamples_;

		/// The lock for the samples.
		Lock samplesLock_;

		/// Reusable buffer pointers to extract buffers from USB transfers.
		BufferPointers reusableBufferPointers_;

		/// The subscription for the detached kernel driver (if necessary/possible).
		ScopedSubscription detachedKernelDriverSubscription_;

		/// The subscription for the claimed video control interface.
		ScopedSubscription claimedVideoControlInterfaceSubscription_;

		/// The subscription for the claimed video stream interface.
		ScopedSubscription claimedVideoStreamInterfaceSubscription_;
};

inline uint8_t VideoDevice::PayloadHeader::toggledFrameId() const
{
	constexpr uint8_t checkBit = (1u << 0u);

	return bmHeaderInfo_ & checkBit;
}

inline bool VideoDevice::PayloadHeader::isEndOfFrame() const
{
	constexpr uint8_t checkBit = (1u << 1u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline bool VideoDevice::PayloadHeader::hasPresentationTime() const
{
	constexpr uint8_t checkBit = (1u << 2u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline bool VideoDevice::PayloadHeader::hasSourceClockReference() const
{
	constexpr uint8_t checkBit = (1u << 3u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline bool VideoDevice::PayloadHeader::isStillImage() const
{
	constexpr uint8_t checkBit = (1u << 5u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline bool VideoDevice::PayloadHeader::hasError() const
{
	constexpr uint8_t checkBit = (1u << 6u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline bool VideoDevice::PayloadHeader::isEndOfHeader() const
{
	constexpr uint8_t checkBit = (1u << 7u);

	return (bmHeaderInfo_ & checkBit) != 0u;
}

inline uint32_t VideoDevice::PayloadHeader::sourceTimeClock() const
{
	ocean_assert(hasSourceClockReference());

	uint32_t result;
	memcpy(&result, scrSourceClock_, sizeof(uint32_t));

	return result;
}

inline uint16_t VideoDevice::PayloadHeader::busFrameCounter() const
{
	ocean_assert(hasSourceClockReference());

	uint16_t result;
	memcpy(&result, scrSourceClock_ + 4, sizeof(uint16_t));

	return result & uint16_t(0x07FFu);
}

inline uint8_t VideoDevice::Sample::descriptorFormatIndex() const
{
	return descriptorFormatIndex_;
}

inline uint8_t VideoDevice::Sample::descriptorFrameIndex() const
{
	return descriptorFrameIndex_;
}

inline const void* VideoDevice::Sample::data() const
{
	return buffer_.data();
}

inline size_t VideoDevice::Sample::size() const
{
	return position_;
}

inline bool VideoDevice::Sample::mayContainError() const
{
	return mayContainError_;
}

inline bool VideoDevice::VideoControlInterface::isValid() const
{
	return isValid_;
}

inline const VideoDevice::VideoStreamingInterface::UncompressedVideoStreams& VideoDevice::VideoStreamingInterface::uncompressedVideoStreams() const
{
	ocean_assert(isValid());

	return uncompressedVideoStreams_;
}

inline const VideoDevice::VideoStreamingInterface::FrameBasedVideoStreams& VideoDevice::VideoStreamingInterface::frameBasedVideoStreams() const
{
	ocean_assert(isValid());

	return frameBasedVideoStreams_;
}

inline const VideoDevice::VideoStreamingInterface::MJPEGVideoStreams& VideoDevice::VideoStreamingInterface::mjpegVideoStreams() const
{
	ocean_assert(isValid());

	return mjpegVideoStreams_;
}

inline bool VideoDevice::VideoStreamingInterface::isValid() const
{
	return vsInputHeaderDescriptor_.isValid();
}

inline const VideoDevice::VideoStreamingInterface& VideoDevice::videoStreamingInterface() const
{
	return videoStreamingInterface_;
}

inline bool VideoDevice::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return isStarted_;
}

inline bool VideoDevice::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return Device::isValid() && videoControlInterface_.isValid() && videoStreamingInterface_.isValid() && initializeControlInterfaceInitialized_;
}

}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_VIDEO_VIDEO_DEVICE_H
