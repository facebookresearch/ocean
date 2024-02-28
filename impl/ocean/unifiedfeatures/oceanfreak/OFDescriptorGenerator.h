// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/oceanfreak/OceanFreak.h"
#include "ocean/unifiedfeatures/oceanfreak/OFDescriptorContainer.h"

#include "ocean/unifiedfeatures/DescriptorGenerator.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/cv/detector/FREAKDescriptor.h"

namespace Ocean
{

namespace UnifiedFeatures
{

namespace OceanFreak
{

/// Forward declaration
template <size_t tDescriptorSizeBytes>
class OFDescriptorGeneratorT;

/// Typedef for the descriptor generator that creates the 32 byte long FREAK descriptors
typedef OFDescriptorGeneratorT<32> OFDescriptorGenerator32;

/// Typedef for the descriptor generator that creates the 64 byte long FREAK descriptors
typedef OFDescriptorGeneratorT<64> OFDescriptorGenerator64;

/**
 * Definition of a descriptor generator for Ocean ORB descriptors
 * @ingroup unifiedfeaturesoceanfreak
 */
template <size_t tDescriptorSizeBytes>
class OCEAN_UNIFIEDFEATURES_OF_EXPORT OFDescriptorGeneratorT final : public DescriptorGenerator
{
	public:

		/// Short-hand definition of the currently selected type of Freak descriptor
		typedef CV::Detector::FREAKDescriptorT<tDescriptorSizeBytes> FreakDescriptor;

		/// Short-hand definition of a vector of the currently selected type of Freak descriptor
		typedef std::vector<FreakDescriptor> FreakDescriptors;

		/**
		 * Definition of parameters for the detector
		 */
		class OFParameters : public Parameters
		{
			public:

				/**
				 * Constructor
				 * @param cameraDerivativeFunctor A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray.
				 * @param inverseFocalLength The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
				 */
				inline OFParameters(const std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor>& cameraDerivativeFunctor, const float inverseFocalLength);

			public:

				/// The inverse focal length (assumes identical vertical and horizontal focal lengths), range: (0, infinity)
				float inverseFocalLength;

				/// A functor that is called for each input point and which must return its corresponding 2x3 Jacobian of the projection matrix and normalized unprojection ray.
				const std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor> cameraDerivativeFunctor;
		};

	public:

		/**
		 * Deleted default constructor
		 */
		OFDescriptorGeneratorT() = delete;

		/**
		 * Constructor
		 * @param parameters The parameters for this descriptor generator
		 */
		inline OFDescriptorGeneratorT(OFParameters&& parameters);

		/**
		 * @sa DescriptorGenerator::generateDescriptors()
		 */
		Result generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptors, Worker* worker = nullptr) const override;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		static inline std::string nameOFDescriptorGenerator();

		/**
		 * Creates an instance of this descriptor generator
		 * @return The pointer to new instance
		 */
		static std::shared_ptr<UnifiedObject> create(const std::shared_ptr<Parameters>& parameters = nullptr);

	protected:

		/// The parameters of this descriptor generator
		OFParameters parameters_;
};

template <size_t tDescriptorSizeBytes>
inline OFDescriptorGeneratorT<tDescriptorSizeBytes>::OFParameters::OFParameters(const std::shared_ptr<typename FreakDescriptor::CameraDerivativeFunctor>& cameraDerivativeFunctor, const float inverseFocalLength) :
	inverseFocalLength(inverseFocalLength),
	cameraDerivativeFunctor(cameraDerivativeFunctor)
{
	ocean_assert(cameraDerivativeFunctor != nullptr);
	ocean_assert(inverseFocalLength > 0.0f);
}

template <size_t tDescriptorSizeBytes>
inline OFDescriptorGeneratorT<tDescriptorSizeBytes>::OFDescriptorGeneratorT(OFParameters&& parameters) :
	DescriptorGenerator(nameOceanFreakLibrary(), nameOFDescriptorGenerator(), /* border, cf. https://fburl.com/diffusion/b6h2fugt */ 20u, DescriptorCategory::BINARY_DESCRIPTOR, { FrameType::FORMAT_Y8 }),
	parameters_(parameters)
{
	// Nothing else to do.
}

template <size_t tDescriptorSizeBytes>
inline std::string OFDescriptorGeneratorT<tDescriptorSizeBytes>::nameOFDescriptorGenerator()
{
	return "OFDescriptorGenerator" + String::toAString(tDescriptorSizeBytes);
}

template <size_t tDescriptorSizeBytes>
std::shared_ptr<UnifiedObject> OFDescriptorGeneratorT<tDescriptorSizeBytes>::create(const std::shared_ptr<Parameters>& parameters)
{
	if (parameters == nullptr)
	{
		ocean_assert(false && "The initialization requires parameters!");
		return nullptr;
	}

	const std::shared_ptr<OFParameters> ofParametersPtr = std::static_pointer_cast<OFParameters>(parameters);

	if (ofParametersPtr == nullptr)
	{
		ocean_assert(false && "Incompatible or missing parameters");
		return nullptr;
	}

	OFParameters ofParameters = *ofParametersPtr;
	return std::make_shared<OFDescriptorGeneratorT<tDescriptorSizeBytes>>(std::move(ofParameters));
}

template <size_t tDescriptorSizeBytes>
DescriptorGenerator::Result OFDescriptorGeneratorT<tDescriptorSizeBytes>::generateDescriptors(const Frame& frame, const FeatureContainer& featureContainer, std::shared_ptr<DescriptorContainer>& descriptorContainer, Worker* worker) const
{
	ocean_assert(frame.isValid());

	if (!frame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		ocean_assert(false && "Invalid pixel format");
		return Result::ERROR_PIXEL_FORMAT_INVALID;
	}

	if (featureContainer.size() == 0)
	{
		descriptorContainer = std::make_shared<OFDescriptorContainerT<tDescriptorSizeBytes>>();
		return Result::SUCCESS;
	}

	ocean_assert(parameters_.cameraDerivativeFunctor != nullptr);
	ocean_assert(parameters_.inverseFocalLength > 0);

	// Compute FREAK descriptors
	const Keypoints& keypoints = featureContainer.keypoints();
	std::vector<Eigen::Vector2f> points;
	points.reserve(keypoints.size());
	for (const Keypoint& keypoint : keypoints)
	{
		const VectorF2& observation = keypoint.observation();
		points.emplace_back(observation.x(), observation.y());
	}

	const CV::FramePyramid framePyramid(frame, /* layers */ 2u, worker);
	FreakDescriptors freakDescriptors(points.size());
	FreakDescriptor::computeDescriptors(framePyramid, points.data(), points.size(), 0u, freakDescriptors.data(), parameters_.inverseFocalLength, *parameters_.cameraDerivativeFunctor, worker);

	descriptorContainer = std::make_shared<OFDescriptorContainerT<tDescriptorSizeBytes>>(std::move(freakDescriptors));

	return Result::SUCCESS;
}

} // namespace OceanFreak

} // namespace UnifiedFeatures

} // namespace Ocean
