/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_VIDEO_VS_DESCRIPTOR_H
#define META_OCEAN_SYSTEM_USB_VIDEO_VS_DESCRIPTOR_H

#include "ocean/system/usb/video/Video.h"

#include "ocean/base/Frame.h"

#include "ocean/system/usb/Descriptor.h"

namespace Ocean
{

namespace System
{

namespace USB
{

namespace Video
{

/**
 * This class implements the base class for all video streaming descriptors.
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusbvideo
 */
class VSDescriptor : public Descriptor
{
	public:

		/**
		 * Definition of individual video streaming interfaces types.
		 * Video Class-Specific VS Interface Descriptor Subtypes.
		 */
		enum VideoStreamingInterfaceTypes : uint8_t
		{
			/// Undefined descriptor.
			VS_UNDEFINED  = 0x00u,
			/// The Input Header descriptor is used for VS interfaces that contain an IN endpoint for streaming video data.
			VS_INPUT_HEADER = 0x01u,
			/// The Output Header descriptor for VS interfaces that contain an OUT endpoint for streaming video data.
			VS_OUTPUT_HEADER = 0x02u,
			/// Descriptor for still images.
			VS_STILL_FRAME = 0x03u,
			/// Format descriptor for uncompressed streams.
			VS_FORMAT_UNCOMPRESSED = 0x04u,
			/// Image descriptor for uncompressed stream.
			VS_FRAME_UNCOMPRESSED = 0x05u,
			/// Format descriptor for MJPEG streams.
			VS_FORMAT_MJPEG = 0x06u,
			/// Image descriptor for MJPEG stream.
			VS_FRAME_MJPEG = 0x07u,
			/// Color format descriptor for any stream.
			VS_COLORFORMAT = 0x0Du,
			/// Format descriptor for frame-based streams.
			VS_FORMAT_FRAME_BASED = 0x10u,
			/// Image descriptor for frame-based stream.
			VS_FRAME_FRAME_BASED = 0x11u
		};

	protected:

		/**
		 * Returns the frame intervals of a given descriptor.
		 * @param descriptor The descriptor from which the intervals will be extracted
		 * @param frameRates Optional resulting vector holding the individual frame rates, one for each interval
		 * @return The descriptor's frame intervals, in 100ns
		 * @tparam TDescriptor The type of the descriptor
		 */
		template <typename TDescriptor>
		static Indices32 frameIntervals(const TDescriptor& descriptor, std::vector<double>* frameRates = nullptr);
};

/**
 * Input Header Descriptor (VS_INPUT_HEADER).
 *
 * The Input Header descriptor is used for VS interfaces that contain an IN endpoint for streaming video data.
 * It provides information on the number of different format descriptors that will follow it, as well as the total size of all class-specific descriptors in alternate setting zero of this interface.
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSInputHeaderDescriptor : public VSDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSInputHeaderDescriptor() = default;

		/**
		 * Creates a new input header descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [13, infinity)
		 */
		VSInputHeaderDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the endpoint address of this descriptor.
		 * @return The descriptor's endpoint address
		 */
		inline uint8_t endpointAddress() const;

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this descriptor, in bytes.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VS_INPUT_HEADER descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Number of video payload format descriptors following for this interface (excluding video frame descriptors): p.
		uint8_t bNumFormats_ = 0u;

		/// Total number of bytes returned for the class-specific VideoStreaming interface descriptors including this header descriptor.
		uint16_t wTotalLength_ = 0u;

		/**
		 * The address of the isochronous or bulk endpoint used for video data.
		 * The address is encoded as follows:
		 * D7: Direction
		 * 1 = IN endpoint
		 * D6..4: Reserved, set to zero.
		 * D3..0: The endpoint number, determined by the designer.
		 */
		uint8_t bEndpointAddress_ = 0u;

		/**
		 * Indicates the capabilities of this VideoStreaming interface:
		 * D0: Dynamic Format Change supported
		 * D7..1: Reserved, set to zero.
		 */
		uint8_t bmInfo_ = 0u;

		/// The terminal ID of the Output Terminal to which the video endpoint of this interface is connected.
		uint8_t bTerminalLink_ = 0u;

		/**
		 * Method of still image capture supported as described in section 2.4.2.4, "Still Image Capture":
		 * 0: None (Host software will not support any form of still image capture)
		 * 1: Method 1
		 * 2: Method 2
		 * 3: Method 3
		 */
		uint8_t bStillCaptureMethod_ = 0u;

		/**
		 * Specifies if hardware triggering is supported through this interface
		 * 0: Not supported
		 * 1: Supported
		 */
		uint8_t bTriggerSupport_= 0u;

		/**
		 * Specifies how the host software shall respond to a hardware trigger interrupt event from this interface. This is ignored if the bTriggerSupport field is zero.
		 * 0: Initiate still image capture
		 * 1: General purpose button event. Host driver will notify client application of button press and button release events
		 */
		uint8_t bTriggerUsage_ = 0u;

		/**
		 * Size of each bmaControls(x) field, in bytes: n
		 */
		uint8_t bControlSize_ = 0u;

		/**
		 * 'bNumFormats' entries of control bitmaps
		 *
		 * For bits D3..0, a bit set to 1 indicates that the named field is supported by the Video Probe and Commit Control when bFormatIndex is 1:
		 * D0: wKeyFrameRate
		 * D1: wPFrameRate
		 * D2: wCompQuality
		 * D3: wCompWindowSize
		 * For bits D5..4, a bit set to 1 indicates that the named control is supported by the device when bFormatIndex is 1:
		 * D4: Generate Key Frame
		 * D5: Update Frame Segment
		 * D6..(n*8-1): Reserved, set to zero
		 */
		std::vector<std::vector<uint8_t>> bmaControls_;
};

/**
 * UncompressedVideoFormatDescriptor (VS_FORMAT_UNCOMPRESSED)
 *
 * The Uncompressed Video Format descriptor defines the characteristics of a specific video stream.
 * It is used for formats that carry uncompressed video information, including all YUV variants.
 *
 * A Terminal corresponding to a USB IN or OUT endpoint, and the interface it belongs to, supports one or more format definitions.
 * To select a particular format, host software sends control requests to the corresponding interface.
 *
 * The bFormatIndex field contains the one-based index of this format descriptor, and is used by requests from the host to set and get the current video format.
 *
 * The guidFormat field uniquely identifies the video data format that shall be used when communicating with this interface at the corresponding format index.
 * For a video source function, the host software will deploy the corresponding video format decoder (if necessary) based on the format specified in this field.
 *
 * The bAspectRatioX and bAspectRatioY fields specify the X and Y dimensions of the picture aspect ratio respectively for video field (interlaced) data.
 * For example, bAspectRatioX will be 16 and bAspectRatioY will be 9 for a 16:9 display.
 *
 * An Uncompressed Video Format Descriptor is followed by one or more Uncompressed Video Frame Descriptor(s);
 * each Video Frame Descriptor conveys information specific to a frame size supported for the format.
 *
 * http://www.cajunbot.com/wiki/images/8/88/USB_Video_Payload_Uncompressed_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSUncompressedVideoFormatDescriptor : public VSDescriptor
{
	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSUncompressedVideoFormatDescriptor() = default;

		/**
		 * Creates a new format descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, must be 27
		 */
		VSUncompressedVideoFormatDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the pixel format of this descriptor.
		 * @return The descriptor's pixel format, FORMAT_UNDEFINED if unknown
		 */
		inline FrameType::PixelFormat pixelFormat() const;

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Translates a GUID to the corresponding pixel format.
		 * @param guidFormat The GUID to translate, must be valid
		 * @return The corresponding pixel format, FORMAT_UNDEFINED if unknown
		 */
		static FrameType::PixelFormat translatePixelFormat(const uint8_t guidFormat[16]);

	protected:

		/// Size of this descriptor in byte: 27
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type
		uint8_t bDescriptorType_ = 0u;

		/// VS_FORMAT_UNCOMPRESSED descriptor subtype
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this format descriptor (stating with 1)
		uint8_t bFormatIndex_ = 0u;

		/// Number of frame descriptors following
		uint8_t bNumFrameDescriptors_ = 0u;

		/// GUID Globally Unique Identifier used to identify stream-encoding format
		uint8_t guidFormat_[16] = {};

		/// Number of bits per pixel used to specify color in the decoded video frame
		uint8_t bBitsPerPixel_ = 0u;

		/// Optimum Frame Index (used to select resolution) for this stream
		uint8_t bDefaultFrameIndex_ = 0u;

		/// The X dimension of the picture aspect ratio
		uint8_t bAspectRatioX_ = 0u;

		/// The Y dimension of the picture aspect ratio
		uint8_t bAspectRatioY_ = 0u;

		/**
		 * Specifies interlace information.
		 * If the scanning mode control in the Camera Terminal is supported for this stream, this field shall reflect the field format used in interlaced mode.
		 * (Top field in PAL is field 1, top field in NTSC is field 2.):
		 * D0: Interlaced stream or variable. 1 = Yes
		 * D1: Fields per frame. 0= 2 fields, 1 = 1 field
		 * D2: Field 1 first. 1 = Yes
		 * D3: Reserved
		 * D5..4: Field pattern
		 * 00 = Field 1 only
		 * 01 = Field 2 only
		 * 10 = Regular pattern of fields 1 and 2
		 * 11 = Random pattern of fields 1 and 2
		 * D7..6: Reserved. Do not use.
		 */
		uint8_t bmInterlaceFlags_ = 0u;

		/// Boolean Specifies whether duplication of the video stream is restricted: 0: No restrictions 1: Restrict duplication
		uint8_t bCopyProtect_ = 0u;
};

/**
 * Uncompressed Frame Descriptor (VS_FRAME_UNCOMPRESSED).
 *
 * Uncompressed Video Frame descriptors (or Frame descriptors for short) are used to describe the decoded video and still-image frame dimensions and other frame-specific characteristics supported by a particular stream.
 * One or more Frame descriptors follow the Uncompressed Video Format descriptor they correspond to.
 * The Frame descriptor is also used to determine the range of frame intervals supported for the frame size specified.
 *
 * The Uncompressed Video Frame descriptor is used only for video formats for which the Uncompressed Video Format descriptor applies (see section 3.1.1, "Uncompressed Video Format Descriptor").
 *
 * The bFrameIndex field contains the one-based index of this frame descriptor, and is used by requests from the host to set and get the current frame index for the format in use.
 * This index is one-based for each corresponding format descriptor supported by the device.
 *
 * The range of frame intervals supported can be either a continuous range or a discrete set of values.
 * For a continuous range, dwMinFrameInterval, dwMaxFrameInterval and dwFrameIntervalStep indicate the limits and granularity of the range.
 * For discrete values, the dwFrameInterval(x) fields indicate the range of frame intervals (and therefore frame rates) supported at this frame size.
 * The frame interval is the average display time of a single decoded video frame in 100ns units.
 *
 * http://www.cajunbot.com/wiki/images/8/88/USB_Video_Payload_Uncompressed_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSUncompressedVideoFrameDescriptor : public VSDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSUncompressedVideoFrameDescriptor() = default;

		/**
		 * Creates a new frame descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [27, infinity)
		 */
		VSUncompressedVideoFrameDescriptor(const uint8_t* buffer, const size_t size);

		/**
		 * Returns the frame intervals of this descriptor.
		 * @param frameRates Optional resulting vector holding the individual frame rates, one for each interval
		 * @return The descriptor's frame intervals, in 100ns
		 */
		Indices32 frameIntervals(std::vector<double>* frameRates = nullptr) const;

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	private:

		/**
		 * Size of this descriptor in bytes when bFrameIntervalType is 0: 38
		 * Size of this descriptor in bytes when bFrameIntervalType > 0: 26+(4*n)
		 */
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type
		uint8_t bDescriptorType_ = 0u;

		/// VS_FRAME_UNCOMPRESSED descriptor subtype
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this frame descriptor.
		uint8_t bFrameIndex_ = 0u;

		/**
		 * D0: Still image supported Specifies whether still images are supported at this frame setting.
		 * This is only applicable for VS interfaces with an IN video endpoint using Still Image Capture Method 1, and should be set to 0 in all other cases.
		 *
		 * D1: Fixed frame-rate Specifies whether the device provides a fixed frame rate on a stream associated with this frame descriptor.
		 */
		uint8_t bmCapabilities_ = 0u;

		/// Number Height of decoded bitmap frame in pixels
		uint16_t wWidth_ = 0u;

		/// Number Height of decoded bitmap frame in pixels
		uint16_t wHeight_ = 0u;

		/// Specifies the minimum bit rate at the longest frame interval in units of bps at which the data can be transmitted.
		uint32_t dwMinBitRate_ = 0u;

		/// Specifies the maximum bit rate at the shortest frame interval in units of bps at which the data can be transmitted.
		uint32_t dwMaxBitRate_ = 0u;

		/**
		 * Use of this field has been deprecated.
		 *
		 * Specifies the maximum number of bytes that the compressor will produce for a video frame or still image.
		 * The dwMaxVideoFrameSize field of the Video Probe and Commit control replaces this descriptor field.
		 * A value for this field shall be chosen for compatibility with host software that implements an earlier version of this specification.
		 */
		uint32_t dwMaxVideoFrameBufferSize_ = 0u;

		/// Number Specifies the frame interval the device would like to indicate for use as a default. This must be a valid frame interval described in the fields below.
		uint32_t dwDefaultFrameInterval_ = 0u;

		/// Indicates how the frame interval can be programmed: 0: Continuous frame interval 1..255: The number of discrete frame intervals supported (n)
		uint8_t bFrameIntervalType_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Shortest frame interval supported (at highest frame rate), in 100 ns units.
		 */
		uint32_t dwMinFrameInterval_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Longest frame interval supported (at lowest frame rate), in 100 ns units
		 */
		uint32_t dwMaxFrameInterval_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Indicates granularity of frame interval range, in 100 ns units.
		 */
		uint32_t dwFrameIntervalStep_ = 0u;

		/**
		 * For discrete Frame Intervals
		 * First entry: Shortest frame interval supported (at highest frame rate), in 100 ns units.
		 * ...
		 * Last entry: Longest frame interval supported (at lowest frame rate), in 100 ns units.
		 */
		Indices32 dwFrameInterval_;
};

/**
 * Frame Based Payload Video Format Descriptor (VS_FORMAT_FRAME_BASED).
 *
 * The Frame Based Payload Video Format descriptor defines the characteristics of a specific video stream.
 * It is used for formats that carry Frame Based Payload video information.
 *
 * A Terminal corresponding to a USB IN or OUT endpoint, and the interface it belongs to, supports one or more format definitions.
 * To select a particular format, host software sends control requests to the corresponding interface.
 *
 * The bFormatIndex field contains the one-based index of this format descriptor, and is used by requests from the host to set and get the current video format.
 *
 * The guidFormat field uniquely identifies the video data format that shall be used when communicating with this interface at the corresponding format index.
 * For a video source function, the host software will deploy the corresponding video format decoder (if necessary) based on the format specified in this field.
 *
 * The bAspectRatioX and bAspectRatioY fields specify the X and Y dimensions of the picture aspect ratio, respectively.
 * For example, bAspectRatioX will be 16 and bAspectRatioY will be 9 for a 16:9 display.
 *
 * A Frame Based Payload Video Format Descriptor is followed by one or more Frame Based Payload Video Frame Descriptor(s);
 * each Video Frame Descriptor conveys information specific to a frame size supported for the format.
 *
 * http://cajunbot.com/wiki/images/3/3a/USB_Video_Payload_Frame_Based_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSFrameBasedVideoFormatDescriptor : public VSDescriptor
{
	public:

		/**
		 * Definition of individual encoding formats.
		 */
		enum EncodingFormat : uint32_t
		{
			/// Invalid encoding format.
			EF_INVALID = 0u,
			/// The encoding is using H.264
			EF_H264,
			/// The encoding is using H.265
			EF_H265
		};

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSFrameBasedVideoFormatDescriptor() = default;

		/**
		 * Creates a new format descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, must be 28
		 */
		VSFrameBasedVideoFormatDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns the encoding format of this descriptor.
		 * @return The encoding format, EF_INVALID if unknown
		 */
		inline EncodingFormat encodingFormat() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Translates a GUID to the corresponding encoding format.
		 * @param guidFormat The GUID to translate, must be valid
		 * @return The encoding format, EF_INVALID if unknown
		 */
		static EncodingFormat translateEncodingFormat(const uint8_t guidFormat[16]);

		/**
		 * Translates an encoding format to a string.
		 * @param encodingFormat The encoding format to translate
		 * @return The string representation of the encoding format, 'Invalid' if unknown
		 */
		static std::string translateEncodingFormat(const EncodingFormat encodingFormat);

	protected:

		/// Size of this descriptor in bytes: 28.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VS_FORMAT_FRAME_BASED descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this format descriptor.
		uint8_t bFormatIndex_ = 0u;

		/// Number of frame descriptors following that correspond to this format.
		uint8_t bNumFrameDescriptors_ = 0u;

		/// Globally Unique Identifier used to identify stream-encoding format.
		uint8_t guidFormat_[16] = {};

		/// Number of bits per pixel used to specify color in the decoded video frame. May be zero if not applicable.
		uint8_t bBitsPerPixel_ = 0u;

		/// Optimum Frame Index (used to select resolution) for this stream.
		uint8_t bDefaultFrameIndex_ = 0u;

		/// The X dimension of the picture aspect ratio.
		uint8_t bAspectRatioX_ = 0u;

		/// The Y dimension of the picture aspect ratio.
		uint8_t bAspectRatioY_ = 0u;

		/**
		 * Specifies interlace information.
		 * If the scanning mode control in the Camera Terminal is supported for this stream, this field shall reflect the field format used in interlaced mode.
		 * (Top field in PAL is field 1, top field in NTSC is field 2.):
		 * D0: Interlaced stream or variable. 1 = Yes
		 * D1: Fields per frame. 0= 2 fields, 1 = 1 field
		 * D2: Field 1 first. 1 = Yes
		 * D3: Reserved
		 * D5..4: Field pattern
		 * 00 = Field 1 only
		 * 01 = Field 2 only
		 * 10 = Regular pattern of fields 1 and 2
		 * 11 = Random pattern of fields 1 and 2
		 * D7..6: Reserved. Do not use.
		 */
		uint8_t bmInterlaceFlags_ = 0u;

		/**
		 * Specifies whether duplication of the video stream is restricted:
		 * FALSE (0): No restrictions
		 * TRUE (1): Restrict duplication
		 */
		uint8_t bCopyProtect_ = 0u;

		/**
		 * Specifies whether the data within the frame is of variable length from frame to frame.
		 * TRUE (1): Variable Size
		 * FALSE (0): Fixed Size
		 * If bVariableSize is TRUE (1), then dwBytesPerLine (below) must be set to zero (0).
		 */
		uint8_t bVariableSize_ = 0u;
};

/**
 * Frame Based Payload Frame Descriptor (VS_FRAME_FRAME_BASED).
 *
 * Frame Based Payload Video Frame descriptors (or Frame descriptors for short) are used to
 * describe the decoded video and still-image frame dimensions and other frame-specific
 * characteristics supported by a particular stream. One or more Frame descriptors follow the
 * Frame Based Payload Video Format descriptor they correspond to. The Frame descriptor is also
 * used to determine the range of frame intervals supported for the frame size specified.
 *
 * The bFrameIndex field contains the one-based index of this frame descriptor, and is used by
 * requests from the host to set and get the current frame index for the format in use. This index is
 * one-based for each corresponding format descriptor supported by the device.
 *
 * The range of frame intervals supported can be either a continuous range or a discrete set of
 * values. For a continuous range, dwMinFrameInterval, dwMaxFrameInterval and
 * dwFrameIntervalStep indicate the limits and granularity of the range. For discrete values, the
 * dwFrameInterval(x) fields indicate the range of frame intervals (and therefore frame rates)
 * supported at this frame size. The frame interval is the average display time of a single decoded
 * video frame in 100ns units.
 *
 * http://cajunbot.com/wiki/images/3/3a/USB_Video_Payload_Frame_Based_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSFrameBasedFrameDescriptor : public VSDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSFrameBasedFrameDescriptor() = default;

		/**
		 * Creates a new frame descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, must be 27
		 */
		VSFrameBasedFrameDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the frame intervals of this descriptor.
		 * @param frameRates Optional resulting vector holding the individual frame rates, one for each interval
		 * @return The descriptor's frame intervals, in 100ns
		 */
		Indices32 frameIntervals(std::vector<double>* frameRates = nullptr) const;

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	private:

		/// Size of this descriptor in bytes when bFrameIntervalType is 0: 38 Size of this descriptor in bytes when bFrameIntervalType > 0: 26+(4*n)
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type
		uint8_t bDescriptorType_ = 0u;

		/// VS_FRAME_FRAME_BASED descriptor subtype
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this frame descriptor
		uint8_t bFrameIndex_ = 0u;

		/// D0: Still image supported Specifies whether still images are supported at this frame setting.
		uint8_t bmCapabilities_ = 0u;

		/// Width of decoded bitmap frame in pixels
		uint16_t wWidth_ = 0u;

		/// Height of decoded bitmap frame in pixels
		uint16_t wHeight_ = 0u;

		/// Specifies the minimum bit rate at the longest frame interval in units of bps at which the data can be transmitted.
		uint32_t dwMinBitRate_ = 0u;

		/// Specifies the maximum bit rate at the shortest frame interval in units of bps at which the data can be transmitted.
		uint32_t dwMaxBitRate_ = 0u;

		/// Specifies the frame interval the device would like to indicate for use as a default. This must be a valid frame interval described in the fields below.
		uint32_t dwDefaultFrameInterval_ = 0u;

		/// Indicates how the frame interval can be programmed: 0: Continuous frame interval 1..255: The number of discrete frame intervals supported (n)
		uint8_t bFrameIntervalType_ = 0u;

		/**
		 * Specifies the number of bytes per line of video for packed fixed frame size formats, allowing the receiver to perform stride alignment of the video.
		 * If the bVariableSize value (above) is TRUE (1), or if the format does not permit such alignment, this value shall be set to zero (0).
		 */
		uint32_t dwBytesPerLine_ = 0u;

		/// Shortest frame interval supported (at highest frame rate), in 100 ns units.
		uint32_t dwMinFrameInterval_ = 0u;

		/// Longest frame interval supported (at lowest frame rate), in 100 ns units.
		uint32_t dwMaxFrameInterval_ = 0u;

		/// Indicates granularity of frame interval range, in 100 ns units.
		uint32_t dwFrameIntervalStep_ = 0u;

		/**
		 * For discrete Frame Intervals
		 * First entry: Shortest frame interval supported (at highest frame rate), in 100 ns units.
		 * ...
		 * Last entry: Longest frame interval supported (at lowest frame rate), in 100 ns units.
		 */
		Indices32 dwFrameInterval_;
};


/**
 * Color Matching Descriptor (VS_COLORFORMAT).
 *
 * The Color Matching descriptor is an optional descriptor used to describe the color profile of the video data in an unambiguous way.
 * Only one instance is allowed for a given format and if present, the Color Matching descriptor shall be placed following the Video and Still Image Frame descriptors for that format.
 *
 * For example, this descriptor would be used with Uncompressed Video, MJPEG and MPEG-1 formats.
 * It would not be used in the case MPEG-2, DV or MPEG-4 because the information is already available implicitly (DV) or explicitly (MPEG-2, MPEG-4).
 * If a format requires this descriptor, the corresponding payload specification must enforce this requirement.
 * In the absence of this descriptor, or in the case of “Unspecified” values within the descriptor, color matching defaults will be assumed.
 * The color matching defaults are compliant with sRGB since the BT.709 transfer function and the sRGB transfer function are very similar.
 *
 * The viewing conditions and monitor setup are implicitly based on sRGB and the device should compensate for them (D50 ambient white, dim viewing or 64 lux ambient illuminance, 2.2 gamma reference CRT, etc).
 *
 * http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSColorMatchingDescriptor : public VSDescriptor
{
	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSColorMatchingDescriptor() = default;

		/**
		 * Creates a new color matching descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, must be 6
		 */
		VSColorMatchingDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	private:

		/// Size of this descriptor in bytes, 6.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type
		uint8_t bDescriptorType_ = 0u;

		/// VS_COLORFORMAT descriptor subtype
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/**
		 * This defines the color primaries and the reference white.
		 * 0: Unspecified (Image characteristics unknown)
		 * 1: BT.709, sRGB (default)
		 * 2: BT.470-2 (M)
		 * 3: BT.470-2 (B, G)
		 * 4: SMPTE 170M
		 * 5: SMPTE 240M
		 * 6-255: Reserved
		 */
		uint8_t bColorPrimaries_ = 0u;

		/**
		 * This field defines the optoelectronic transfer characteristic of the source picture also called the gamma function.
		 * 0: Unspecified (Image characteristics unknown)
		 * 1: BT.709 (default)
		 * 2: BT.470-2 M
		 * 3: BT.470-2 B, G
		 * 4: SMPTE 170M
		 * 5: SMPTE 240M
		 * 6: Linear (V = Lc)
		 * 7: sRGB (very similar to BT.709)
		 * 8-255: Reserved
		 */
		uint8_t bTransferCharacteristics_ = 0u;

		/**
		 * Matrix used to compute luma and chroma values from the color primaries.
		 * 0: Unspecified (Image characteristics unknown)
		 * 1: BT. 709
		 * 2: FCC
		 * 3: BT.470-2 B, G
		 * 4: SMPTE 170M (BT.601, default)
		 * 5: SMPTE 240M
		 * 6-255: Reserved
		 */
		uint8_t bMatrixCoefficients_ = 0u;
};

/**
 * MJPEG Video Format Descriptor (VS_FORMAT_MJPEG).
 *
 * The MJPEG Video Format Descriptor defines the characteristics of a specific video stream.
 * It is used for formats that carry MJPEG video information, including all YUV/RGB variants.
 * A Terminal corresponding to a USB IN or OUT endpoint, and the interface it belongs to, supports one or more format definitions.
 * To select a particular format, host software sends control requests to the corresponding interface.
 *
 * The bFormatIndex field contains the one-based index of this format Descriptor, and is used by requests from the host to set and get the current video format.
 * The bDescriptorSubtype field uniquely identifies the video data format that should be used when communicating with this interface at the corresponding format index.
 * For a video source function, the host software will deploy the corresponding video format decoder (if necessary) based on the format specified in this field.
 *
 * The bAspectRatioX and bAspectRatioY fields specify the X and Y dimensions respectively of the picture aspect ratio for video field (interlaced) data.
 * For example, bAspectRatioX will be 16 and bAspectRatioY will be 9 for a 16:9 display.
 *
 * The bmFlags field holds information about the video data stream characteristics.
 * FixedSizeSamples indicates whether all video samples are the same size.
 * A MJPEG Video Format Descriptor is followed by one or more MJPEG Video Frame Descriptor(s);
 * each Video Frame Descriptor conveys information specific to a frame size supported for the format.
 *
 * @see http://www.cajunbot.com/wiki/images/7/71/USB_Video_Payload_MJPEG_1.1.pdf.
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSMJPEGVideoFormatDescriptor : public VSDescriptor
{
	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSMJPEGVideoFormatDescriptor() = default;

		/**
		 * Creates a new format descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, must be 11
		 */
		VSMJPEGVideoFormatDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this Descriptor, in bytes: 11.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE Descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VS_FORMAT_MJPEG Descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this Format Descriptor.
		uint8_t bFormatIndex_ = 0u;

		/// Number of Frame Descriptors following that correspond to this format.
		uint8_t bNumFrameDescriptors_ = 0u;

		/**
		 * Specifies characteristics of this format
		 * D0: FixedSizeSamples. 1 = Yes
		 * All other bits are reserved for future use and shall be reset to zero.
		 */
		uint8_t bmFlags_ = 0u;

		/// Optimum Frame Index (used to select resolution) for this stream.
		uint8_t bDefaultFrameIndex_ = 0u;

		/// The X dimension of the picture aspect ratio.
		uint8_t bAspectRatioX_ = 0u;

		/// The Y dimension of the picture aspect ratio.
		uint8_t bAspectRatioY_ = 0u;

		/**
		 * Specifies interlace information. If the scanning mode control in the Camera Terminal is supported for this stream, this field should reflect the field format used in interlaced mode.
		 * (Top field in PAL is field 1, top field in NTSC is field 2.):
		 * D0: Interlaced stream or variable. 1 = Yes
		 * D1: Fields per frame. 0= 2 fields, 1 = 1 field
		 * D2: Field 1 first. 1 = Yes
		 * D3: Reserved
		 * D5..4: Field pattern
		 * 00 = Field 1 only
		 * 01 = Field 2 only
		 * 10 = Regular pattern of fields 1 and 2
		 * 11 = Random pattern of fields 1 and 2
		 * D7..6: Reserved. Do not use.
		 */
		uint8_t bmInterlaceFlags_ = 0u;

		/**
		 * Specifies if duplication of the video stream should be restricted:
		 * 0: No restrictions
		 * 1: Restrict duplication
		 */
		uint8_t bCopyProtect_ = 0u;
};

/**
 * MJPEG Video Frame Descriptors (VS_FRAME_MJPEG).
 *
 * MJPEG Video Frame Descriptors (or simply Frame Descriptors) are used to describe the decoded video and still image frame dimensions, and other frame-specific characteristics supported by a particular stream.
 * One or more Frame Descriptors follow the MJPEG Video Format Descriptor they correspond to.
 * The Frame Descriptor is also used to determine the range of frame intervals that are supported for the specified frame size.
 *
 * The MJPEG Video Frame Descriptor is used only for video formats for which the MJPEG Video Format Descriptor applies (see section 3.1.1, "MJPEG Video Format Descriptor").
 *
 * The bFrameIndex field contains the one-based index of this Frame Descriptor, and is used by requests from the host to set and get the current frame index for the format in use.
 * This index is one-based for each corresponding Format Descriptor supported by the device.
 *
 * The range of frame intervals supported can be either a continuous range or a discrete set of values.
 * For a continuous range, dwMinFrameInterval, dwMaxFrameInterval and dwFrameIntervalStep indicate the limits and granularity of the range.
 * For discrete values, the dwFrameInterval(x) fields indicate the range of frame intervals (and therefore frame rates) supported at this frame size.
 * The frame interval is the average display time of a single decoded video frame in 100ns Units.
 *
 * @see http://www.cajunbot.com/wiki/images/7/71/USB_Video_Payload_MJPEG_1.1.pdf
 * @ingroup systemusbvideo
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VSMJPEGVideoFrameDescriptor : public VSDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VSMJPEGVideoFrameDescriptor() = default;

		/**
		 * Creates a new frame descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [27, infinity)
		 */
		VSMJPEGVideoFrameDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the frame intervals of this descriptor.
		 * @param frameRates Optional resulting vector holding the individual frame rates, one for each interval
		 * @return The descriptor's frame intervals, in 100ns
		 */
		Indices32 frameIntervals(std::vector<double>* frameRates = nullptr) const;

		/**
		 * Returns the content of this descriptor as string.
		 * @return The string representation of this descriptor
		 */
		std::string toString() const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/**
		 * Size of this descriptor in bytes when bFrameIntervalType is 0: 38.
		 * Size of this descriptor in bytes when bFrameIntervalType > 0: 26+(4*n).
		 */
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE Descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VS_FRAME_MJPEG Descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Index of this Frame Descriptor.
		uint8_t bFrameIndex_ = 0u;

		/**
		 * D0: Still image supported Specifies whether still images are supported at this frame setting.
		 * This is only applicable for VS interfaces with an IN video endpoint using Still Image Capture Method 1, and should be set to 0 in all other cases.
		 *
		 * D1: Fixed frame-rate Specifies whether the device provides a fixed frame rate on a stream associated with this frame descriptor. Set to 1 if fixed rate is enabled; otherwise, set to 0.
		 *
		 * D7..2: Reserved, set to 0.
		 */
		uint8_t bmCapabilities_ = 0u;

		/// Width of decoded bitmap frame in pixels.
		uint16_t wWidth_ = 0u;

		/// Height of decoded bitmap frame in pixels.
		uint16_t wHeight_ = 0u;

		/// Specifies the minimum bit rate at default compression quality and longest frame interval in Units of bps at which the data can be transmitted.
		uint32_t dwMinBitRate_ = 0u;

		/// Specifies the maximum bit rate at default compression quality and shortest frame interval in Units of bps at which the data can be transmitted.
		uint32_t dwMaxBitRate_ = 0u;

		/**
		 * Use of this field has been deprecated.
		 *
		 * Specifies the maximum number of bytes for a video (or still image) frame the compressor will produce.
		 *
		 * The dwMaxVideoFrameSize field of the Video Probe and Commit control replaces this descriptor field.
		 * A value for this field shall be chosen for compatibility with host software that implements an earlier version of this specification.
		 */
		uint32_t dwMaxVideoFrameBufferSize_ = 0u;

		/// Specifies the frame interval the device would like to indicate for use as a default. This must be a valid frame interval described in the fields below.
		uint32_t dwDefaultFrameInterval_ = 0u;

		/**
		 * Indicates how the frame interval can be programmed:
		 * 0: Continuous frame interval
		 * 1..255: The number of discrete frame intervals supported (n)
		 */
		uint8_t bFrameIntervalType_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Shortest frame interval supported (at highest frame rate), in 100 ns units.
		 */
		uint32_t dwMinFrameInterval_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Longest frame interval supported (at lowest frame rate), in 100 ns units
		 */
		uint32_t dwMaxFrameInterval_ = 0u;

		/**
		 * For continuous Frame Intervals:
		 * Indicates granularity of frame interval range, in 100 ns units.
		 */
		uint32_t dwFrameIntervalStep_ = 0u;

		/**
		 * For discrete Frame Intervals
		 * First entry: Shortest frame interval supported (at highest frame rate), in 100 ns units.
		 * ...
		 * Last entry: Longest frame interval supported (at lowest frame rate), in 100 ns units.
		 */
		Indices32 dwFrameInterval_;
};

template <typename TDescriptor>
Indices32 VSDescriptor::frameIntervals(const TDescriptor& descriptor, std::vector<double>* frameRates)
{
	if (frameRates != nullptr)
	{
		frameRates->clear();
	}

	if (!descriptor.isValid())
	{
		return Indices32();
	}

	Indices32 result;

	if (descriptor.bFrameIntervalType_ == 0u)
	{
		// continuous frame intervals mode

		result.reserve(16);

		for (unsigned int frameInterval = descriptor.dwMinFrameInterval_; frameInterval <= descriptor.dwMaxFrameInterval_; frameInterval += descriptor.dwFrameIntervalStep_)
		{
			result.emplace_back(frameInterval);
		}
	}
	else
	{
		// discrete frame intervals mode

		result = descriptor.dwFrameInterval_;
	}

	if (frameRates != nullptr)
	{
		frameRates->reserve(result.size());

		for (const uint32_t frameInterval : result)
		{
			frameRates->emplace_back(interval2frequency(frameInterval));
		}
	}

	return result;
}

inline uint8_t VSInputHeaderDescriptor::endpointAddress() const
{
	ocean_assert(isValid());

	/*
	 * The address is encoded as follows:
	 * D7: Direction
	 * 1 = IN endpoint
	 * D6..4: Reserved, set to zero.
	 * D3..0: The endpoint number, determined by the designer.
	 */
	return bEndpointAddress_ & 0x8Fu;
}

inline FrameType::PixelFormat VSUncompressedVideoFormatDescriptor::pixelFormat() const
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return FrameType::FORMAT_UNDEFINED;
	}

	return translatePixelFormat(guidFormat_);
}

inline VSFrameBasedVideoFormatDescriptor::EncodingFormat VSFrameBasedVideoFormatDescriptor::encodingFormat() const
{
	ocean_assert(isValid());
	if (!isValid())
	{
		return EF_INVALID;
	}

	return translateEncodingFormat(guidFormat_);
}

}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_VIDEO_VS_DESCRIPTOR_H
