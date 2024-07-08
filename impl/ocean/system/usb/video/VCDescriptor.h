/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_USB_VIDEO_VC_DESCRIPTOR_H
#define META_OCEAN_SYSTEM_USB_VIDEO_VC_DESCRIPTOR_H

#include "ocean/system/usb/video/Video.h"

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
 * This class implements the base class for all video control descriptors.
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusbvideo
 */
class VCDescriptor : public Descriptor
{
	public:

		/**
		 * Definition of individual video control interfaces types.
		 * Video Class-Specific VC Interface Descriptor Subtypes.
		 */
		enum VideoControlInterfaceTypes : uint8_t
		{
			/// Undefined descriptor.
			VC_DESCRIPTOR_UNDEFINED  = 0x00u,
			/// Video interface header descriptor.
			VC_HEADER = 0x01u,
			/// This descriptor describes the input terminal that represents a camera or other video source.
			VC_INPUT_TERMINAL = 0x02u,
			/// This descriptor describes the output terminal that represents the USB pipe to the host.
			VC_OUTPUT_TERMINAL = 0x03u,
			/// This descriptor describes the selector unit that is connected to the processing unit. Either the CCD sensor or media transport mechanism can be selected as the input.
			VC_SELECTOR_UNIT = 0x04u,
			/// This descriptor describes the processing unit that processes the video stream data that is delivered by the selector unit. The only control supported by this implementation is the brightness control.
			VC_PROCESSING_UNIT = 0x05u,
			/// Extension unit descriptor.
			VC_EXTENSION_UNIT = 0x06u
		};
};

/**
 * Class-Specific VC Interface Descriptor (VC_HEADER).
 *
 * The class-specific VC interface descriptor is a concatenation of all the descriptors that are used
 * to fully describe the video function, i.e., all Unit Descriptors (UDs) and Terminal Descriptors
 * (TDs).
 *
 * The total length of the class-specific VC interface descriptor depends on the number of Units and
 * Terminals in the video function. Therefore, the descriptor starts with a header that reflects the
 * total length in bytes of the entire class-specific VC interface descriptor in the wTotalLength
 * field. The bcdUVC field identifies the release of the Video Device Class Specification with
 * which this video function and its descriptors are compliant. The bInCollection field indicates
 * how many VideoStreaming interfaces there are in the Video Interface Collection to which this
 * VideoControl interface belongs. The baInterfaceNr() array contains the interface numbers of all
 * the VideoStreaming interfaces in the Collection. The bInCollection and baInterfaceNr() fields
 * together provide all necessary information to determine which interfaces together constitute the
 * entire USB interface to the video function, i.e., describe the Video Interface Collection.
 *
 * The order in which the Unit and Terminal descriptors are reported is not important, because
 * every descriptor can be identified through its bDescriptorType and bDescriptorSubtype fields.
 *
 * http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VCHeaderDescriptor : public VCDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VCHeaderDescriptor() = default;

		/**
		 * Creates a new header descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [12, infinity)
		 */
		VCHeaderDescriptor(const void* buffer, const size_t size);

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

		/// Size of this descriptor, in bytes: 12+n.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VC_HEADER descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Video Device Class Specification release number in binary-coded decimal. (i.e. 2.10 is 210H).
		uint16_t bcdUVC_ = 0u;

		/// Total number of bytes returned for the class-specific VideoControl interface descriptor. Includes the combined length of this descriptor header and all Unit and Terminal descriptors.
		uint16_t wTotalLength_ = 0u;

		/**
		 * Use of this field has been deprecated.
		 *
		 * The device clock frequency in Hz. This will specify the units used for the time information fields in the Video Payload Headers of the primary data stream and format.
		 *
		 * The dwClockFrequency field of the Video Probe and Commit control replaces this descriptor field.
		 * A value for this field shall be chosen such that the primary or default function of the device will be available to host software that implements Version 1.0 of this specification.
		 */
		uint32_t dwClockFrequency_ = 0u;

		/// The number of VideoStreaming interfaces in the Video Interface Collection to which this VideoControl interface belongs: n
		uint8_t bInCollection_ = 0u;

		/// Interface number of the first VideoStreaming interface in the Collection.
		std::vector<uint8_t> baInterfaceNr_;
};

/**
 * Input Terminal Descriptor (VC_INPUT_TERMINAL)
 *
 * The Input Terminal descriptor (ITD) provides information to the Host that is related to the
 * functional aspects of the Input Terminal.
 *
 * The Input Terminal is uniquely identified by the value in the bTerminalID field. No other Unit
 * or Terminal within the same video function may have the same ID. This value must be passed in
 * the bTerminalID field of each request that is directed to the Terminal.
 *
 * The wTerminalType field provides pertinent information about the physical entity that the Input
 * Terminal represents. This could be a USB OUT endpoint, an external Composite Video In
 * connection, a camera sensor, etc. A complete list of Terminal Type codes is provided in section
 * B.2, "Input Terminal Types".
 *
 * The bAssocTerminal field is used to associate an Output Terminal to this Input Terminal,
 * effectively implementing a bi-directional Terminal pair. An example of this would be a tape unit
 * on a camcorder, which would have Input and Output Terminals to sink and source video
 * respectively. If the bAssocTerminal field is used, both associated Terminals must belong to the
 * bi-directional Terminal Type group. If no association exists, the bAssocTerminal field must be
 * set to zero.
 *
 * The Host software can treat the associated Terminals as being physically related. In many cases,
 * one Terminal can not exist without the other. An index to a string descriptor is provided to
 * further describe the Input Terminal
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VCInputTerminalDescriptor : public VCDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Definition of individual terminal types.
		 */
		enum InputTerminalType : uint16_t
		{
			/// Undefined input terminal type.
			ITT_UNDEFINED,
			/// Vendor-Specific Input Terminal.
			ITT_VENDOR_SPECIFIC = 0x200u,
			/// Camera sensor. To be used only in Camera Terminal descriptors.
			ITT_CAMERA = 0x0201u,
			/// Sequential media. To be used only in Media Transport Terminal Descriptors.
			ITT_MEDIA_TRANSPORT_INPUT = 0x0202u
		};

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VCInputTerminalDescriptor() = default;

		/**
		 * Creates a new input terminal descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [8, infinity)
		 */
		VCInputTerminalDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @param usbDeviceHandle Optional handle of the device associated with this descriptor to provide the terminal description
		 * @return The string representation of this descriptor
		 */
		std::string toString(libusb_device_handle* usbDeviceHandle = nullptr) const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this descriptor, in bytes: 8+ n (e.g., 15+n for ITT_CAMERA).
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VC_INPUT_TERMINAL descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// Constant A non-zero constant that uniquely identifies the Terminal within the video function. This value is used in all requests to address this Terminal.
		uint8_t bTerminalID_ = 0u;

		/// Constant that characterizes the type of Terminal. See Appendix B, "Terminal Types".
		uint16_t wTerminalType_ = 0u;

		/// ID of the Output Terminal to which this Input Terminal is associated, or zero (0) if no such association exists.
		uint8_t bAssocTerminal_ = 0u;

		/// Index of a string descriptor, describing the Input Terminal.
		uint8_t iTerminal_ = 0u;

		/// The value of Lmin (see section 2.4.2.5.1 "Optical Zoom"). If Optical Zoom is not supported, this field shall be set to 0.
		uint16_t wObjectiveFocalLengthMin_ = 0u;

		/// The value of Lmax (see section 2.4.2.5.1 "Optical Zoom"). If Optical Zoom is not supported, this field shall be set to 0.
		uint16_t wObjectiveFocalLengthMax_ = 0u;

		/// The value of Locular (see section 2.4.2.5.1 "Optical Zoom"). If Optical Zoom is not supported, this field shall be set to 0.
		uint16_t wOcularFocalLength_ = 0u;

		/// Size in bytes of the bmControls field: n
		uint8_t bControlSize_ = 0u;

		/**
		 * A bit set to 1 indicates that the mentioned Control is supported for the video stream.
		 * D0: Scanning Mode
		 * D1: Auto-Exposure Mode
		 * D2: Auto-Exposure Priority
		 * D3: Exposure Time (Absolute)
		 * D4: Exposure Time (Relative)
		 * D5: Focus (Absolute)
		 * D6 : Focus (Relative)
		 * D7: Iris (Absolute)
		 * D8 : Iris (Relative)
		 * D9: Zoom (Absolute)
		 * D10: Zoom (Relative)
		 * D11: PanTilt (Absolute)
		 * D12: PanTilt (Relative)
		 * D13: Roll (Absolute)
		 * D14: Roll (Relative)
		 * D15: Reserved
		 * D16: Reserved
		 * D17: Focus, Auto
		 * D18: Privacy
		 * D19..(n*8-1): Reserved, set to zero
		 */
		std::vector<uint8_t> bmControls_;
};

/**
 * Processing Unit Descriptor (VC_PROCESSING_UNIT).
 *
 * The Processing Unit is uniquely identified by the value in the bUnitID field of the Processing
 * Unit descriptor (PUD). No other Unit or Terminal within the same video function may have the
 * same ID. This value must be passed with each request that is directed to the Processing Unit.
 *
 * The bSourceID field is used to describe the connectivity for this Processing Unit. It contains the
 * ID of the Unit or Terminal to which this Processing Unit is connected via its Input Pin. The
 * bmControls field is a bit-map, indicating the availability of certain processing Controls for the
 * video stream. For future expandability, the number of bytes occupied by the bmControls field is
 * indicated in the bControlSize field. The bControlSize field is permitted to specify a value less
 * than the value needed to cover all the control bits (including zero), in which case the unspecified
 * bmControls bytes will not be present and the corresponding control bits are assumed to be zero.
 *
 * An index to a string descriptor is provided to further describe the Processing Unit.
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VCProcessingUnitDescriptor : public VCDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VCProcessingUnitDescriptor() = default;

		/**
		 * Creates a new processing unit descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [10, infinity)
		 */
		VCProcessingUnitDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @param usbDeviceHandle Optional handle of the device associated with this descriptor to provide the terminal description
		 * @return The string representation of this descriptor
		 */
		std::string toString(libusb_device_handle* usbDeviceHandle = nullptr) const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this descriptor, in bytes: 10+n.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VC_PROCESSING_UNIT descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// A non-zero constant that uniquely identifies the Unit within the video function. This value is used in all requests to address this Unit.
		uint8_t bUnitID_ = 0u;

		/// ID of the Unit or Terminal to which this Unit is connected.
		uint8_t bSourceID_ = 0u;

		/**
		 * If the Digital Multiplier control is supported, this field indicates the maximum digital magnification, multiplied by 100.
		 * For example, for a device that supports 1-4.5X digital zoom (a multiplier of 4.5), this field would be set to 450. If the Digital Multiplier control is not supported, this field shall be set to 0.
		 */
		uint16_t wMaxMultiplier_ = 0u;

		/// Size of the bmControls field, in bytes: n.
		uint8_t bControlSize_ = 0u;

		/**
		 * A bit set to 1 indicates that the mentioned Control is supported for the video stream.
		 * D0: Brightness
		 * D1: Contrast
		 * D2: Hue
		 * D3: Saturation
		 * D4: Sharpness
		 * D5: Gamma
		 * D6: White Balance Temperature
		 * D7: White Balance Component
		 * D8: Backlight Compensation
		 * D9: Gain
		 * D10: Power Line Frequency
		 * D11: Hue, Auto
		 * D12: White Balance Temperature, Auto
		 * D13: White Balance Component, Auto
		 * D14: Digital Multiplier
		 * D15: Digital Multiplier Limit
		 * D16: Analog Video Standard
		 * D17: Analog Video Lock Status
		 * D18..(n*8-1): Reserved. Set to zero.
		 */
		std::vector<uint8_t> bmControls_;

		// Index of a string descriptor that describes this processing unit.
		uint8_t iProcessing_ = 0u;

		/**
		 * A bitmap of all analog video standards supported by the Processing Unit.
		 * A value of zero indicates that this bitmap should be ignored.
		 * D0: None
		 * D1: NTSC – 525/60
		 * D2: PAL – 625/50
		 * D3: SECAM – 625/50
		 * D4: NTSC – 625/50
		 * D5: PAL – 525/60
		 */
		uint8_t bmVideoStandards_ = 0u;
};

/**
 * Selector Unit Descriptor (VC_SELECTOR_UNIT).
 *
 * The Selector Unit is uniquely identified by the value in the bUnitID field of the Selector Unit
 * descriptor (SUD). No other Unit or Terminal within the same video function may have the same
 * ID. This value must be passed with each request that is directed to the Selector Unit.
 *
 * The bNrInPins field contains the number of Input Pins (p) of the Selector Unit. The connectivity
 * of the Input Pins is described via the baSourceID() array that contains p elements. The index i
 * into the array is one-based and directly related to the Input Pin numbers. baSourceID(i) contains
 * the ID of the Unit or Terminal to which Input Pin i is connected.
 *
 * An index to a string descriptor is provided to further describe the Selector Unit.
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VCSelectorUnitDescriptor : public VCDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VCSelectorUnitDescriptor() = default;

		/**
		 * Creates a new selector unit descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [6, infinity)
		 */
		VCSelectorUnitDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @param usbDeviceHandle Optional handle of the device associated with this descriptor to provide the terminal description
		 * @return The string representation of this descriptor
		 */
		std::string toString(libusb_device_handle* usbDeviceHandle = nullptr) const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this descriptor, in bytes: 6+p.
		uint8_t bLength_ = 0u;

		/// Constant CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VC_SELECTOR_UNIT descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// A non-zero constant that uniquely identifies the Unit within the video function. This value is used in all requests to address this Unit.
		uint8_t bUnitID_ = 0u;

		/// Number of Input Pins of this Unit: p.
		uint8_t bNrInPins_ = 0u;

		/// ID of the Unit or Terminal to which the Input Pins of this Selector Unit is connected.
		std::vector<uint8_t> baSourceID_;

		/// Index of a string descriptor, describing the Selector Unit.
		uint8_t iSelector_ = 0u;
};

/**
 * Extension Unit Descriptor (VC_EXTENSION_UNIT)
 *
 * The Extension Unit is uniquely identified by the value in the bUnitID field of the Extension Unit
 * descriptor (XUD). No other Unit or Terminal within the same video function may have the same
 * ID. This value must be passed with each request that is directed to the Extension Unit.
 *
 * The Extension Unit Descriptor allows the hardware designer to define any arbitrary set of
 * controls such that a class driver can act as an intermediary between vendor-supplied host
 * software and functionality of the device.
 *
 * The guidExtensionCode field contains a vendor-specific code that further identifies the
 * Extension Unit.
 *
 * The bNrInPins field contains the number of Input Pins (p) of the Extension Unit. The
 * connectivity of the Input Pins is described via the baSourceID() array that contains p elements.
 * The index i into the array is one-based and directly related to the Input Pin numbers.
 * baSourceID(i) contains the ID of the Unit or Terminal to which Input Pin i is connected.
 *
 * The bmControls field is a bitmap, indicating the availability of certain video Controls in the
 * Extension Unit. For future expandability, the number of bytes occupied by the bmControls field
 * is indicated in the bControlSize field. All Controls are optional.
 *
 * An index to a string descriptor is provided to further describe the Extension Unit.
 *
 * @see http://www.cajunbot.com/wiki/images/8/85/USB_Video_Class_1.1.pdf
 * @ingroup systemusb
 */
class OCEAN_SYSTEM_USB_VIDEO_EXPORT VCExtensionUnitDescriptor : public VCDescriptor
{
	friend class Descriptor;

	public:

		/**
		 * Default constructor creating an invalid descriptor.
		 */
		VCExtensionUnitDescriptor() = default;

		/**
		 * Creates a new extension unit descriptor from memory.
		 * @param buffer The memory buffer in which the descriptor is located, must be valid
		 * @param size The size of the buffer, in bytes, with range [24, infinity)
		 */
		VCExtensionUnitDescriptor(const void* buffer, const size_t size);

		/**
		 * Returns the content of this descriptor as string.
		 * @param usbDeviceHandle Optional handle of the device associated with this descriptor to provide the terminal description
		 * @return The string representation of this descriptor
		 */
		std::string toString(libusb_device_handle* usbDeviceHandle = nullptr) const;

		/**
		 * Returns whether this object holds valid descriptor information.
		 * @return True, if so
		 */
		bool isValid() const;

	protected:

		/// Size of this descriptor, in bytes: 24+p+n.
		uint8_t bLength_ = 0u;

		/// CS_INTERFACE descriptor type.
		uint8_t bDescriptorType_ = 0u;

		/// VC_EXTENSION_UNIT descriptor subtype.
		uint8_t bDescriptorSubtype_ = 0u;

	public:

		/// A non-zero constant that uniquely identifies the Unit within the video function. This value is used in all requests to address this Unit.
		uint8_t bUnitID_ = 0u;

		/// Vendor-specific code identifying the Extension Unit (GUID).
		uint8_t guidExtensionCode_[16];

		/// Number of controls in this extension unit.
		uint8_t bNumControls_ = 0u;

		/// Number of Input Pins of this Unit: p.
		uint8_t bNrInPins_ = 0u;

		/// IDs of the Unit or Terminal to which the Input Pins of this Extension Unit is connected.
		std::vector<uint8_t> baSourceID_;

		/// Size of the bmControls field, in bytes: n.
		uint8_t bControlSize_ = 0u;

		/// A bit set to 1 indicates that the mentioned Control is supported: D(n*8-1)..0: Vendor-specific
		std::vector<uint8_t> bmControls_;

		/// Index of a string descriptor that describes this extension unit.
		uint8_t iExtension_ = 0u;
};

}

}

}

}

#endif // META_OCEAN_SYSTEM_USB_VIDEO_VC_DESCRIPTOR_H
