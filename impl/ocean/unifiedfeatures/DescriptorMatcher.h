// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"
#include "ocean/unifiedfeatures/DescriptorContainer.h"
#include "ocean/unifiedfeatures/UnifiedObject.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/**
 * Defintion of a generic interface for descriptor matchers
 * @ingroup unifiedfeatures
 */
struct DescriptorMatcher : public UnifiedObject
{
	public:

		/**
		 * Return codes for matching
		 */
		enum class Result : uint32_t
		{
			/// Successfully matched (does not indicate the existence of matches)
			SUCCESS = 0u,
			/// An (unspecified) error has occured
			GENERAL_ERROR = 1u << 0u,
			/// Descriptor cannot be matched, e.g., because they have the incompatible types
			ERROR_INCOMPATIBLE_DESCRIPTORS = GENERAL_ERROR | 1u << 1u,
		};

	public:

		/**
		 * Default desctructor
		 */
		virtual ~DescriptorMatcher() = default;

		/**
		 * Matches two descriptor containers
		 * @param descriptorContainer0 The first container that will be matched, must have the same type as `descriptorContainer1` and both must be compatible with this matcher
		 * @param descriptorContainer1 The first container that will be matched, must have the same type as `descriptorContainer0` and both must be compatible with this matcher
		 * @param correspondences The resulting correespondence that have been found
		 * @param worker Optional worker for parallelization.
		 * @return Will return `Result::SUCCESS` on success, otherwise a corresponding value
		 */
		virtual Result matchDescriptors(const DescriptorContainer& descriptorContainer0, const DescriptorContainer& descriptorContainer1, IndexPairs32& correspondences, Worker* worker = nullptr) const = 0;

		/**
		 * Translates a matching status into a human-readable string
		 * @param result The matching status that will translated into a string
		 * @return The string
		 */
		inline static std::string translateResult(const Result result);

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the library that this descriptor matcher belongs to
		 * @param descriptorMatcherName The name of the descriptor matcher, must be valid
		 * @param descriptorMatcherVersion Optional version number of the underlying implementation of the descriptor matcher, range: [0, infinity)
		 */
		inline DescriptorMatcher(const std::string& libraryName, const std::string& descriptorMatcherName, const unsigned int descriptorMatcherVersion = 0u);
};

inline std::string DescriptorMatcher::translateResult(const Result result)
{
	switch (result)
	{
		case Result::SUCCESS:
			return "SUCCESS";

		case Result::ERROR_INCOMPATIBLE_DESCRIPTORS:
			return "ERROR_INCOMPATIBLE_DESCRIPTOR_CONTAINERS";

		case Result::GENERAL_ERROR:
			return "GENERAL_ERROR";

		// no default case
	}

	ocean_assert(false && "Invalid matchting status");
	return "INVALID";
}

inline DescriptorMatcher::DescriptorMatcher(const std::string& libraryName, const std::string& descriptorMatcherName, const unsigned int descriptorMatcherVersion) :
	UnifiedObject(libraryName, descriptorMatcherName, descriptorMatcherVersion)
{
	// Nothing else to do.
}

} // namespace UnifiedFeatures

} // namespace Ocean
