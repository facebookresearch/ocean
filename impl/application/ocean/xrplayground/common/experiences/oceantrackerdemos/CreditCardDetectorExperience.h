// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CREDIT_CARD_DETECTOR_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CREDIT_CARD_DETECTOR_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Text.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"
#endif

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include <CCScanner/CCScanner.hpp>
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience demonstrates a credit card detector for MR devices.
 * @ingroup xrplayground
 */
class CreditCardDetectorExperience :
	public XRPlaygroundExperience,
	protected Thread
{
	protected:

		/**
		 * Definition of a class to hold credit card information
		 */
		class CreditCardInfo
		{
			public:

				/**
				 * The default constructor for an invalid credit card.
				 */
				CreditCardInfo() = default;

				/**
				 * Constructor for a credit card.
				 * @param number The number of the credit card, must be valid
				 * @param name The nname of the credit card holder, must be valid
				 * @param expirationDate The expiration date of the credit card, must be valid
				 */
				inline CreditCardInfo(const std::string& number, const std::string& name, const std::string expirationDate);

				/**
				 * Returns the number of the credit card.
				 * @return The number
				 */
				inline const std::string& number() const;

				/**
				 * Returns the name of the credit card holder.
				 * @return The name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the expiration date of the credit card.
				 * @return The expiration date
				 */
				inline const std::string& expirationDate() const;

				/**
				 * Returns whether the credit card is valid.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether this object is equal to a given object.
				 * @param creditCardInfo The object to compare to
				 * @return True, if so, otherwise false
				 */
				inline bool operator==(const CreditCardInfo& creditCardInfo) const;

				/**
				 * Returns whether this object is not equal to a given object.
				 * @param creditCardInfo The object to compare to
				 * @return True, if so, otherwise false
				 */
				inline bool operator!=(const CreditCardInfo& creditCardInfo) const;

			protected:

				/// The number of credit card.
				std::string number_ = "";

				/// The name of the credit card holder.
				std::string name_ = "";

				/// The expiration date of the credit card.
				std::string expirationDate_ = "";
		};

		/// Typedef for a vector of credit card information.
		typedef std::vector<CreditCardInfo> CreditCardInfos;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~CreditCardDetectorExperience() override;

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
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		/**
		 * Copies a frame into a corresponding SmartCapture frame.
		 * @param frame The frame which will be copied, must be valid and the pixel format must be compatible with FORMAT_RGB24
		 * @param ccFrame The resulting SmartCapture frame
		 * @return True if the frame was copied successfully, otherwise false
		 */
		static bool toSmartCaptureFrame(const Frame& frame, facebook::identity_integrity::smart_capture::Frame& ccFrame);
#endif

		/**
		 * Returns whether a finger tip is near a given point.
		 * @param worldPoint The world point to check the distance to, must be valid
		 * @param maxDistance The maximal distance between any finger tip and the given point, range: [0, infinity) and maxDistance >= minDistance
		 * @param minDistance The minimal distance between any finger tip and the given point, range: [0, infinity) and maxDistance >= minDistance
		 */
		static bool isAHandNearPoint(const Vector3& worldPoint, const Scalar maxDistance, const Scalar minDistance = Scalar(0));

		/**
		 * Computes the image bounding box of the corners of a 3D box
		 * @param camera The camera model that will be used to project the 3D box into the 2D image, must be valid
		 * @param world_T_camera The transformation from the camera to world coordinate system, must be valid
		 * @param world_T_detectionBox The transformation from the 3D box to the world coordinate system, must be valid
		 * @param detectionBoxWidth The width of the detection box, range: (0, infinity)
		 * @param detectionBoxHeight The height of the detection box, range: (0, infinity)
		 * @param detectionBoxInImage The resulting bounding box of the projected corners
		 * @return True, if succeeded
		 */
		static bool computeDetectionBoxInImage(const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_detectionBox, const Scalar detectionBoxWidth, const Scalar detectionBoxHeight, Box2& detectionBoxInImage);

	protected:

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		// SmartCapture wrapper for OCR2Go
		std::unique_ptr<facebook::identity_integrity::smart_capture::cc_scanner::CCScanner> ccScanner_;
#endif

		/// The standard width of a credit card in meters.
		static constexpr Scalar creditCardWidth_ = Scalar(0.08560);
		/// The standard height of a credit card in meters.
		static constexpr Scalar creditCardHeight_ = Scalar(0.05398);

		/// The rendering text object to display usage help for the user.
		Rendering::TransformRef helpTextTransform_;
		/// The rendered help text.
		Rendering::TextRef helpText_;

		/// The rendering text object to display a disclaimer for the user.
		Rendering::TransformRef disclaimerTextTransform_;

		/// The FrameMedium objects of all cameras that will be displayed.
		Media::FrameMediumRefs frameMediums_;

		/// The transformation for the detection box.
		Rendering::TransformRef detectionBoxTransform_;
		/// The detection box that the card has to be placed in.
		Rendering::BoxRef detectionBox_;
		/// The material of the detection box.
		Rendering::MaterialRef detectionBoxMaterial_;

		/// The transformation of the credit card information text.
		Rendering::TransformRef creditCardInfoTextTransform_;
		/// The text object holding the credit card information.
		Rendering::TextRef creditCardInfoText_;

		/// The dimensions of the detection box for this device (in meters).
		Vector3 detectionBoxSize_;
		/// The location of the detection box for this device.
		Vector3 detectionBoxTranslation_;

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
		Platform::Meta::Quest::Application::VRImageVisualizer vrImageVisualizer_;
#endif
		/// Indicates whether a hand is near the detection box.
		std::atomic_bool isDetectionBoxOccupied_ = false;

		/// Indicates whether to display the detection frames. Mostly for debugging.
		std::atomic_bool displayDetectionFrames_ = false;

		/// Indicates that for stereo images, a credit card will have to be detected in both frames to count as a valid detection. Otherwise the first detection in either frame will be used.
		std::atomic_bool enforceIdenticalDetectionsInStereo = false;

		/// This timestamp is used to indicate for how long the information of a detected credit card should be displayed (to avoid flickering).
		Timestamp displayCreditCardInfoTimestamp_;

		/// The lock for the detection results.
		Lock resultLock_;

		/// The input frames for the detector
		Frames detectionFrames_;

		/// The most recent result from detecting a credit card.
		CreditCardInfo creditCardInfo_;

		/// The average performance of the detection process.
		double averageDetectionTimeMs_ = -1.0;
};

inline CreditCardDetectorExperience::CreditCardInfo::CreditCardInfo(const std::string& number, const std::string& name, const std::string expirationDate) :
	number_(number),
	name_(name),
	expirationDate_(expirationDate)
{
	// Nothing else to do.
}

inline const std::string& CreditCardDetectorExperience::CreditCardInfo::number() const
{
	return number_;
}

inline const std::string& CreditCardDetectorExperience::CreditCardInfo::name() const
{
	return name_;
}

inline const std::string& CreditCardDetectorExperience::CreditCardInfo::expirationDate() const
{
	return expirationDate_;
}

inline bool CreditCardDetectorExperience::CreditCardInfo::isValid() const
{
	return !number_.empty() && !name_.empty() && !expirationDate_.empty();
}

inline bool CreditCardDetectorExperience::CreditCardInfo::operator==(const CreditCardInfo& creditCardInfo) const
{
	return number_ == creditCardInfo.number() && name_ == creditCardInfo.name() && expirationDate_ == creditCardInfo.expirationDate();
}

inline bool CreditCardDetectorExperience::CreditCardInfo::operator!=(const CreditCardInfo& creditCardInfo) const
{
	return !(*this == creditCardInfo);
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_CREDIT_CARD_DETECTOR_EXPERIENCE_H
