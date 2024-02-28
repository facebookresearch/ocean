// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/DescriptorGenerator.h"
#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/FeatureContainer.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"
#include "ocean/unifiedfeatures/Keypoint.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/// Forward declaration
class DescriptorGenerator;

/**
 * Definition of a generic interface for feature detectors
 * @ingroup unifiedfeatures
 */
class FeatureDetector : public UnifiedObject
{
	public:

		/**
		 * Result statuses for the detection process
		 */
		enum class Result : uint32_t
		{
			/// The detection was successful
			SUCCESS = 0u,
			/// An (unspecified) error has occured
			GENERAL_ERROR = 1u << 0u,
			/// The input has a wrong pixel format
			ERROR_PIXEL_FORMAT_INVALID = GENERAL_ERROR | 1u << 1u,
			/// The input is too small in order to be processed
			ERROR_INPUT_TOO_SMALL = GENERAL_ERROR | 1u << 2u,
		};

	public:

		/**
		 * Deleted default constructor
		 */
		FeatureDetector() = delete;

		/**
		 * Default destructor
		 */
		virtual ~FeatureDetector() = default;

		/**
		 * Return the feature category that this detector can extract
		 * @return The feature category that this detector can extract
		 */
		inline FeatureCategory featureCategory() const;

		/**
		 * Returns a list of supported pixel formats
		 * @return The list of pixel formats
		 */
		inline const PixelFormatSet& supportedPixelFormats() const;

		/**
		 * Checks if the specified pixel format is supported
		 * @param pixelFormat The pixel format that will be checked
		 * @return True if the pixel format is supported, otherwise false
		 */
		inline bool isSupportedPixelFormat(const FrameType::PixelFormat pixelFormat) const;

		/**
		 * Returns the descriptor generator that is most suited for the current detector
		 * @note If no suitable descriptor can be found, this function will return `nullptr`
		 * @return A pointer to a descriptor generator or `nullptr` if none could be found
		 */
		virtual std::shared_ptr<DescriptorGenerator> getBestAvailableDescriptorGenerator();

		/**
		 * Detects features without leaving a border
		 * @param frame The frame in which features will be detected, must be valid and have a supported pixel format
		 * @param featureContainer The resulting features that have been detected
		 * @return Will return `DS_SUCCESS` on success, otherwise a corresponding status value
		 */
		inline Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer) const;

		/**
		 * Detects features
		 * @param frame The frame in which features will be detected, must be valid and have a supported pixel format
		 * @param featureContainer The resulting features that have been detected
		 * @param border The minimum distance to the image borders in pixel that features must have, range: [0, infinity)
		 * @param worker Optional worker for parallelization.
		 * @return Will return `DS_SUCCESS` on success, otherwise a corresponding status value
		 */
		virtual Result detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer, const unsigned int border, Worker* worker = nullptr) const = 0;

		/**
		 * Translates a detection status into a human-readable string
		 * @param result The result status that will converted into a string
		 * @return The string
		 */
		inline static std::string translateResult(const Result result);

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the library that this feature detector belongs to
		 * @param detectorName The name of the underlying implementation of the feature detector, must be valid
		 * @param featureCategory The type of features that this feature detector can extract, must not be `FeatureCategory::UNKNOWN`
		 * @param supportedPixelFormats A list of pixel formats that this feature detector can support
		 * @param detectorVersion Optional version of the underlying implementation of the feature detector, range: [0, infinity)
		 */
		inline FeatureDetector(const std::string& libraryName, const std::string& detectorName, const FeatureCategory featureCategory, PixelFormatSet&& supportedPixelFormats, const unsigned int detectorVersion = 0u);

	protected:

		/// The category of features that this detector can extract
		const FeatureCategory featureCategory_;

		/// A list of pixel formats that this feature detector can support
		const PixelFormatSet supportedPixelFormats_;
};

inline FeatureDetector::FeatureDetector(const std::string& libraryName, const std::string& detectorName, const FeatureCategory featureCategory, PixelFormatSet&& supportedPixelFormats, const unsigned int detectorVersion) :
	UnifiedObject(libraryName, detectorName, detectorVersion),
	featureCategory_(featureCategory),
	supportedPixelFormats_(std::move(supportedPixelFormats))
{
	ocean_assert(featureCategory_ != FeatureCategory::UNKNOWN);
}

inline FeatureCategory FeatureDetector::featureCategory() const
{
	return featureCategory_;
}

inline const PixelFormatSet& FeatureDetector::supportedPixelFormats() const
{
	return supportedPixelFormats_;
}

inline bool FeatureDetector::isSupportedPixelFormat(const FrameType::PixelFormat pixelFormat) const
{
	for (const FrameType::PixelFormat supportedPixelFormat : supportedPixelFormats())
	{
		if (FrameType::arePixelFormatsCompatible(supportedPixelFormat, pixelFormat))
		{
			return true;
		}
	}

	return false;
}

inline std::string FeatureDetector::translateResult(const Result result)
{
	switch (result)
	{
		case Result::SUCCESS:
			return "SUCCESS";

		case Result::ERROR_PIXEL_FORMAT_INVALID:
			return "ERROR_PIXEL_FORMAT_INVALID";

		case Result::ERROR_INPUT_TOO_SMALL:
			return "ERROR_INPUT_TOO_SMALL";

		case Result::GENERAL_ERROR:
			return "GENERAL_ERROR";

		// no default case
	}

	ocean_assert(false && "Invalid detection status");
	return "INVALID";
}

FeatureDetector::Result FeatureDetector::detectFeatures(const Frame& frame, std::shared_ptr<FeatureContainer>& featureContainer) const
{
	return detectFeatures(frame, featureContainer, 0u);
}

} // namespace UnifiedFeatures

} // namespace Ocean
