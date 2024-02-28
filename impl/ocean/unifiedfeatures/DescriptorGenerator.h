// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/Descriptor.h"
#include "ocean/unifiedfeatures/DescriptorContainer.h"
#include "ocean/unifiedfeatures/FeatureContainer.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/// Forward declaration
class FeatureDetector;

/**
 * Definition of a generic interface for descriptor generators.
 * @ingroup unifiedfeatures
 */
class DescriptorGenerator : public UnifiedObject
{
	public:

		/**
		 * Definition of return codes of the descriptor generation
		 */
		enum class Result : uint32_t
		{
			/// The generation was successful
			SUCCESS = 0u,
			/// An (unspecified) error has occured
			GENERAL_ERROR = 1u << 0u,
			/// The input has a wrong pixel format
			ERROR_PIXEL_FORMAT_INVALID = GENERAL_ERROR | 1u << 1u,
			/// The provided features cannot be used for the descriptor generation
			ERROR_INVALID_FEATURES = GENERAL_ERROR | 1u << 2u,
		};

	public:

		/**
		 * Deleted default constructor
		 */
		DescriptorGenerator() = delete;

		/**
		 * Default deconstructor
		 */
		virtual ~DescriptorGenerator() = default;

		/**
		 * Returns the required distance to the image border
		 * Some implementation require that features have a minimum distance to the image borders in order to guarantee that the no memory outside the image will have to be accessed.
		 * @return The required border value, range: [0, infinity)
		 */
		inline unsigned int requiredBorder() const;

		/**
		 * Returns the category of the descriptors that are computed by this generator
		 * @return The category of the descriptors
		 */
		inline DescriptorCategory descriptorCategory() const;

		/**
		 * Checks if the specified feature detector can be used with this descriptor generator
		 * This function should return true if the feature of the specified feature detector can be used to compute
		 * detectors, otherwise it should return false
		 * @note The default implementation of this function will always return false; derived classes will have to implement a check that is tailored to their specific situation
		 * @param featureDetector The feature detector that will be checked for compatiblility
		 * @return True if the detector is supported, otherwise false
		 */
		virtual bool isSupportedFeatureDetector(const FeatureDetector& featureDetector) const;

		/**
		 * Returns the pixel formats that this descriptor generator can process
		 * @return The supported pixel formats
		 */
		inline const PixelFormatSet& supportedPixelFormats() const;

		/**
		 * Checks if a given pixel format is supported by this descriptor generator
		 * @param pixelFormat The pixel format that will be checked
		 * @return True if this pixel format is supported, otherwise false
		 */
		inline virtual bool isSupportedPixelFormat(const FrameType::PixelFormat pixelFormat) const;

		/**
		 * Generates descriptors
		 * @param frame The input frame that will be used to generate the descriptors, must be valid and pixel format must be supported
		 * @param featureContainer The container that holds the feature for which descriptors will be generated
		 * @param descriptorContainer The resulting container holding the descriptors
		 * @param worker Optional worker instance for parallelization
		 * @return Will return `DGR_SUCCESS` on success, otherwise a corresponding status code
		 */
		virtual Result generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptorContainer, Worker* worker = nullptr) const = 0;

		/**
		 * Translates a generation status into a human-readable string
		 * @param result The result status that will converted into a string
		 * @return The string
		 */
		inline static std::string translateResult(const Result result);

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the library that this descriptor generator belongs to
		 * @param descriptorGeneratorName The name of the descriptor, must be valid
		 * @param requiredBorder The required minimum distance of features to the image border that this descriptor generator requires, range: [0, infinity)
		 * @param descriptorCategory The category of the descriptors that will be computed by this generator, must not be `DescriptorCategory::UNKNOWN`
		 * @param supportedPixelFormats A list of pixel formats that this descriptor generator can support
		 * @param descriptorGeneratorVersion Optional version of the underlying implementation of the descriptor generator, range: [0, infinity)
		 */
		inline DescriptorGenerator(const std::string& libraryName, const std::string& descriptorGeneratorName, const unsigned int requiredBorder, const DescriptorCategory descriptorCategory, PixelFormatSet&& supportedPixelFormats, const unsigned int descriptorGeneratorVersion = 0u);

	protected:

		/// The required distance to the image border; some implementation require that features have a minimum distance to the image borders in order to guarantee that the no memory outside the image will have to be accessed.
		const unsigned int requiredBorder_;

		/// The type of the descriptor that are computed by this generator.
		const DescriptorCategory descriptorCategory_;

		/// A list of supported pixel formats
		const PixelFormatSet supportedPixelFormats_;
};

inline unsigned int DescriptorGenerator::requiredBorder() const
{
	return requiredBorder_;
}

inline DescriptorCategory DescriptorGenerator::descriptorCategory() const
{
	return descriptorCategory_;
}

inline const PixelFormatSet& DescriptorGenerator::supportedPixelFormats() const
{
	return supportedPixelFormats_;
}

inline bool DescriptorGenerator::isSupportedPixelFormat(const FrameType::PixelFormat pixelFormat) const
{
	for (const FrameType::PixelFormat& supportedPixelFormat : supportedPixelFormats())
	{
		if (FrameType::arePixelFormatsCompatible(supportedPixelFormat, pixelFormat))
		{
			return true;
		}
	}

	return false;
}

inline std::string DescriptorGenerator::translateResult(const Result result)
{
	switch (result)
	{
		case Result::SUCCESS:
			return "SUCCESS";

		case Result::GENERAL_ERROR:
			return "GENERAL_ERROR";

		case Result::ERROR_PIXEL_FORMAT_INVALID:
			return "ERROR_PIXEL_FORMAT_INVALID";

		case Result::ERROR_INVALID_FEATURES:
			return "ERROR_INVALID_FEATURES";

		// no default case
	}

	ocean_assert(false && "Invalid result status");
	return "INVALID";
}

DescriptorGenerator::DescriptorGenerator(const std::string& libraryName, const std::string& descriptorGeneratorName, const unsigned int requiredBorder, const DescriptorCategory descriptorCategory, PixelFormatSet&& supportedPixelFormats, const unsigned int descriptorGeneratorVersion) :
	UnifiedObject(libraryName, descriptorGeneratorName, descriptorGeneratorVersion),
	requiredBorder_(requiredBorder),
	descriptorCategory_(descriptorCategory),
	supportedPixelFormats_(std::move(supportedPixelFormats))
{
	ocean_assert(descriptorCategory_ != DescriptorCategory::UNKNOWN);
}

} // namespace UnifiedFeatures

} // namespace Ocean
