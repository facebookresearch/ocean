// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_GENERATOR_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_GENERATOR_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/rendering/FrameTexture2D.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to generate QR codes.
 * @ingroup xrplayground
 */
class QRCodeGeneratorExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~QRCodeGeneratorExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Sets data for a new QR code
		 * @param codePayload The payload that should be encoded into a new QR code, must be valid.
		 * @param codeVersion The returned version of the code that has been created
		 * @return True if creating the code was successful, otherwise false
		 */
		bool generateQRCodeFrame(const std::string& codePayload, unsigned int& codeVersion);

		/**
		 * Creates a new QRCodeGeneratorExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		QRCodeGeneratorExperience() = default;

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

		/**
		 * Shows the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

		/**
		 * Unloads the user interaction on iOS.
		 * This function contains platform specific code for iOS.
		 * @param userInterface The application's user interface
		 */
		void unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface);

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

	protected:

		/// The frame texture for the QR code image.
		Rendering::FrameTexture2DRef renderingFrameTexture_;

		/// The transformatino holding the textured box.
		Rendering::TransformRef renderingBoxTransform_;

		/// The payload that should be encoded in to a QR code.
		Frame codeFrame_;

		/// A lock to guarantee thread-safe moving of the image of the QR code.
		Lock codeFrameLock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_QRCODE_GENERATOR_EXPERIENCE_H
