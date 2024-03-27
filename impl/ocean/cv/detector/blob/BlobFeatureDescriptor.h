// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_DESCRIPTOR_H
#define META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_DESCRIPTOR_H

#include "ocean/cv/detector/blob/Blob.h"
#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

/**
 * This class implements a blob feature descriptor.
 * @ingroup cvdetectorblob
 */
class OCEAN_CV_DETECTOR_BLOB_EXPORT BlobFeatureDescriptor
{
	public:

		/**
		 * Redefinition of Blob feature orientation types.
		 */
		typedef BlobFeature::OrientationType OrientationType;

	private:

		/**
		 * Definition of a lookup table for Gaussian weights
		 */
		class LookupTable
		{
			public:

				/**
				 * Creates a new lookup table.
				 */
				LookupTable();

			private:

				/**
				 * Initializes the lookup tables.
				 */
				void initializeGaussianDistribution();

			public:

				/// 2D Gaussian distribution of the first quadrant with a sampling step of 1 and a standard deviation of 2 with range [0, 3].
				Scalar gaussianWeights_03_2[4][4];

				/// 2D Gaussian distribution of the first quadrant with a sampling step of 1 and a standard deviation of 2 with range [0, 5].
				Scalar gaussianWeights_05_2[6][6];

#ifdef OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

				/// 2D Gaussian distribution of the first quadrant with a sampling step of 8/6 and a standard deviation of 3 with range [0, 6].
				Scalar gaussianWeights_06_3[7][7];

				/// 2D Gaussian distribution of the entire area standard deviation of 3.
				Scalar gaussianWeights_225_3[225];

#else

				/// 2D Gaussian distribution of the first quadrant with a sampling step of 1 and a standard deviation of 3 with range [0, 8].
				Scalar gaussianWeights_08_3[9][9];

				/// 2D Gaussian distribution of the entire area standard deviation of 3.
				Scalar gaussianWeights_400_3[400];

#endif // OCEAN_CV_DETECTOR_BLOB_USE_DESCRIPTOR_36

				/// Lookup table for sinus values.
				Scalar sinValues[361];

				/// Lookup table for cosine values.
				Scalar cosValues[361];
		};

	public:

		/**
		 * Calculates the dominant orientation of an already detected Blob feature.
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param feature The feature to calculate the dominant orientation for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @see BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateDescriptor(), BlobFeatureDetector::calculateOrientations().
		 */
		static void calculateOrientation(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeature& feature, const bool forceCalculation);

		/**
		 * Calculates the dominant orientation for all given features.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features The features to calculate the dominant orientation for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker is defined
		 * @see BlobFeatureDescriptor::calculateOrientation(), BlobFeatureDetector::detectFeatures().
		 */
		static inline void calculateOrientations(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the Blob descriptor for an already detected Blob feature with already assigned orientation.
		 * To be orientation invariant the dominant orientation must be determined first.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param feature Already detected and oriented Blob feature to calculate the descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @see BlobFeatureDetector::calculateDescriptors(), BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateOrientation(), IntegralImage::createLinedImage().
		 */
		static void calculateDescriptor(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeature& feature, const bool forceCalculation);

		/**
		 * Calculates the Blob descriptors for all specified detected Blob features with already assigned orientation.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * To be orientation invariant the dominant orientations must be determined first.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features Already detected and oriented Blob features to calculate the descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see BlobFeatureDetector::calculateDescriptor(), BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateOrientation(), IntegralImage::createLinedImage().
		 */
		static inline void calculateDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the orientation and (oriented) descriptor for all given features.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features The features to calculate the orientation and descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see calculateOrientation(), calculateDescriptor().
		 */
		static inline void calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the orientation and (oriented) descriptor for all given features specified by their indices.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features The features to calculate the orientation and descriptor for
		 * @param featureIndices Vector holding the indices of all descriptors to be calculated, the indices are defined w.r.t. the specified features
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see calculateOrientation(), calculateDescriptor().
		 */
		static inline void calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const FeatureIndices& featureIndices, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the orientation and (oriented) descriptor for the first n given features.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features The features to calculate the orientation and descriptor for
		 * @param number The number of features to be oriented and described (defined from the beginning)
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see calculateOrientation(), calculateDescriptor().
		 */
		static inline void calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const unsigned int number, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the not oriented Blob descriptor for an already detected Blob feature.
		 * This descriptor is not orientation invariant, however calculation can be done much faster than the oriented one.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param feature Already detected and oriented Blob feature to calculate the descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @see BlobFeatureDetector::detectFeatures(), IntegralImage::createLinedImage().
		 */
		static void calculateNotOrientedDescriptor(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeature& feature, const bool forceCalculation);

		/**
		 * Calculates the not oriented Blob descriptor for all already detected Blob features.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.<br>
		 * This descriptor is not orientation invariant, however calculation can be done much faster than the oriented one.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features The features to calculate the not oriented descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateNotOrientedDescriptor(), IntegralImage::createLinedImage().
		 */
		static inline void calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the not oriented Blob descriptor for all already detected Blob features specified by their indices.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.<br>
		 * This descriptor is not orientation invariant, however calculation can be done much faster than the oriented one.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features The features to calculate the not oriented descriptor for
		 * @param featureIndices Vector holding the indices of all descriptors to be calculated, the indices are defined w.r.t. the specified features
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateNotOrientedDescriptor(), IntegralImage::createLinedImage().
		 */
		static inline void calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const FeatureIndices& featureIndices, const bool forceCalculation, Worker* worker = nullptr);

		/**
		 * Calculates the not oriented Blob descriptor for the first n given features.
		 * This function may use an optional worker object to distribute the computation on as many CPU cores as defined in the worker object.<br>
		 * This descriptor is not orientation invariant, however calculation can be done much faster than the oriented one.
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features The features to calculate the not oriented descriptor for
		 * @param number The number of features to be described (defined from the beginning)
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param worker Optional worker object used for computation distribution, nullptr if no worker object is available
		 * @see BlobFeatureDetector::detectFeatures(), BlobFeatureDetector::calculateNotOrientedDescriptor(), IntegralImage::createLinedImage().
		 */
		static inline void calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const unsigned int number, const bool forceCalculation, Worker* worker = nullptr);

	private:

		/**
		 * Calculates the dominant orientation for a specified subset of given features.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Lined integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features Subset of features to calculate the orientation for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateOrientationsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Calculates the Blob descriptors for a specified subset of given Blob features with already assigned orientations.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features Subset of features to calculate the descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Calculates the orientation and (oriented) descriptor for a specified subset of given features.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features Subset of features to calculate the orientation and descriptor for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateOrientationsAndDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Calculates the orientation and (oriented) descriptor for a subset of features specified by the feature indices.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param type The type of the orientation determination
		 * @param features Subset of features to calculate the orientation and description for
		 * @param featureIndices Indices of all features to be oriented and described
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateOrientationsAndDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures* features, const FeatureIndices* featureIndices, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Calculates the not oriented descriptor for a specified subset of given features.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features Subset of features to calculate the orientation and description for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateNotOrientedDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Calculates the not oriented descriptor for a subset of given features specified by their feature indices.
		 * Beware: No range check will be done!
		 * @param linedIntegralImage Integral image of the original (grayscale) frame added by one additional column and row with zero entries
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param pixelOrigin Origin of the original frame, to date UPPER_LEFT is supported only
		 * @param features Subset of features to calculate the orientation and description for
		 * @param forceCalculation Determines whether the calculation will be enforced even if the feature has been described already
		 * @param featureIndices Indices of all features to be oriented and described
		 * @param firstFeature Specifies the index of the first feature to be handled
		 * @param numberFeatures Specifies the number of features to be handled, with start point as specified
		 */
		static void calculateNotOrientedDescriptorsSubset(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures* features, const FeatureIndices* featureIndices, const bool forceCalculation, const unsigned int firstFeature, const unsigned int numberFeatures);

		/**
		 * Initializes the pre-calculated Gaussian distribution weight arrays, if they have not yet been initialized.
		 */
		static const LookupTable& lookupTable();
};

inline void BlobFeatureDescriptor::calculateOrientations(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateOrientationsSubset, linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0u, 0u), 0u, (unsigned int)(features.size()), 7u, 8u);
	else
		calculateOrientationsSubset(linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0, (unsigned int)(features.size()));
}

inline void BlobFeatureDescriptor::calculateDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0u, 0u), 0u, (unsigned int)(features.size()), 6u, 7u);
	else
		calculateDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0, (unsigned int)(features.size()));
}

inline void BlobFeatureDescriptor::calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateOrientationsAndDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0u, 0u), 0u, (unsigned int)(features.size()), 7u, 8u);
	else
		calculateOrientationsAndDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0, (unsigned int)(features.size()));
}

inline void BlobFeatureDescriptor::calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const FeatureIndices& featureIndices, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateOrientationsAndDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, type, &features, &featureIndices, forceCalculation, 0u, 0u), 0u, (unsigned int)(featureIndices.size()), 8u, 9u);
	else
		calculateOrientationsAndDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, type, &features, &featureIndices, forceCalculation, 0, (unsigned int)(featureIndices.size()));
}

inline void BlobFeatureDescriptor::calculateOrientationsAndDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, const OrientationType type, BlobFeatures& features, const unsigned int number, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateOrientationsAndDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0u, 0u), 0u, min(number, (unsigned int)features.size()), 7u, 8u);
	else
		calculateOrientationsAndDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, type, &features, forceCalculation, 0, min(number, (unsigned int)(features.size())));
}

inline void BlobFeatureDescriptor::calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateNotOrientedDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0u, 0u), 0u, (unsigned int)(features.size()), 6u, 7u);
	else
		calculateNotOrientedDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0, (unsigned int)(features.size()));
}

inline void BlobFeatureDescriptor::calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const FeatureIndices& featureIndices, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateNotOrientedDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, &features, &featureIndices, forceCalculation, 0u, 0u), 0u, (unsigned int)(featureIndices.size()), 7u, 8u);
	else
		calculateNotOrientedDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, &features, &featureIndices, forceCalculation, 0, (unsigned int)(featureIndices.size()));
}

inline void BlobFeatureDescriptor::calculateNotOrientedDescriptors(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const FrameType::PixelOrigin pixelOrigin, BlobFeatures& features, const unsigned int number, const bool forceCalculation, Worker* worker)
{
	if (worker)
		worker->executeFunction(Worker::Function::createStatic(&calculateNotOrientedDescriptorsSubset, linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0u, 0u), 0u, min(number, (unsigned int)features.size()), 6u, 7u);
	else
		calculateNotOrientedDescriptorsSubset(linedIntegralImage, width, height, pixelOrigin, &features, forceCalculation, 0, min(number, (unsigned int)(features.size())));
}

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_DESCRIPTOR_H
