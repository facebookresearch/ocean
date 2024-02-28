// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_H
#define META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_H

#include "ocean/cv/detector/blob/Blob.h"
#include "ocean/cv/detector/blob/BlobDescriptor.h"

#include "ocean/base/ObjectRef.h"

#include "ocean/cv/detector/OrientedPointFeature.h"

#include "ocean/geometry/Geometry.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

// Forward declaration.
class BlobFeature;

/**
 * Definition of a vector holding Blob features.
 * @ingroup cvdetectorblob
 */
typedef std::vector<BlobFeature> BlobFeatures;

/**
 * Definition of a object reference for Blob features.
 * @ingroup cvdetectorblob
 */
typedef ObjectRef<BlobFeatures> BlobFeaturesRef;

/**
 * This class implements a Blob feature.
 * @ingroup cvdetectorblob
 */
class OCEAN_CV_DETECTOR_BLOB_EXPORT BlobFeature : public OrientedPointFeature
{
	public:

		/**
		 * Definition of different Blob descriptor types.
		 */
		enum DescriptorType
		{
			/// Undescribed descriptor.
			DESCRIPTOR_UNDESCRIBED,
			/// Not oriented descriptor.
			DESCRIPTOR_NOT_ORIENTED,
			/// Oriented descriptor.
			DESCRIPTOR_ORIENTED
		};

		/**
		 * Definition of different orientation determination methods.
		 */
		enum OrientationType
		{
			/// No orientation is determined.
			ORIENTATION_NOT_ORIENTED,
			/// The orientation is determined by a sliding window.
			ORIENTATION_SLIDING_WINDOW,
			/// The orientation is determined by the sum of all orientations.
			ORIENTATION_SUMMED_ORIENTATION
		};

	public:

		/**
		 * Creates a new empty blob feature object.
		 */
		inline BlobFeature();

		/**
		 * Creates a new feature object by a given 2D observation position in e.g. an image.
		 * @param observation 2D feature observation
		 * @param distortionState Distortion state of the 2D feature position
		 * @param scale Feature scale
		 * @param strength Feature strength
		 * @param laplace Laplacian sign of the hessian matrix
		 */
		inline BlobFeature(const Vector2& observation, const DistortionState distortionState, const Scalar scale, const Scalar strength, const bool laplace);

		/**
		 * Creates a new feature object by a given 2D observation position in e.g. an image.
		 * @param observation 2D feature observation
		 * @param distortionState Distortion state of the 2D feature position
		 * @param scale Feature scale
		 * @param strength Feature strength
		 * @param laplace Laplacian sign of the hessian matrix
		 * @param orientation Orientation of the feature.
		 */
		inline BlobFeature(const Vector2& observation, const DistortionState distortionState, const Scalar scale, const Scalar strength, const bool laplace, const Scalar orientation);

		/**
		 * Returns the scale of this feature
		 * @return Feature scale
		 */
		inline Scalar scale() const;

		/**
		 * Returns the laplace sign of this feature.
		 * @return Laplace sign
		 */
		inline bool laplace() const;

		/**
		 * Returns the descriptor of this feature.
		 * @return Feature descriptor
		 */
		inline const BlobDescriptor& descriptor() const;

		/**
		 * Returns the descriptor of this feature.
		 * @return Feature descriptor
		 */
		inline BlobDescriptor& descriptor();

		/**
		 * Returns the descriptor type of this feature.
		 * @return Descriptor type
		 */
		inline DescriptorType descriptorType() const;

		/**
		 * Returns the orientation type of this feature.
		 * @return Orientation type
		 */
		inline OrientationType orientationType() const;

		/**
		 * Sets the scale of this feature.
		 * @param scale Feature scale to set
		 */
		inline void setScale(const Scalar scale);

		/**
		 * Sets the laplace sign of this feature.
		 * @param laplace Laplace sign to set
		 */
		inline void setLaplace(const bool laplace);

		/**
		 * Sets or changes the descriptor type of this feature.
		 * @param type Descriptor type to set
		 */
		inline void setDescriptorType(const DescriptorType type);

		/**
		 * Sets or changes the orientation type of this feature.
		 * @param type Orientation type to set
		 */
		inline void setOrientationType(const OrientationType type);

		/**
		 * Sets or changes the descriptor and orientation type of this feature.
		 * @param descriptorType Descriptor type to set
		 * @param orientationType Orientation type to set
		 */
		inline void setType(const DescriptorType descriptorType, const OrientationType orientationType);

		/**
		 * Returns the summed square distance (SSD) between two descriptors.
		 * @param feature Second feature
		 * @return SSD between both feature descriptors
		 */
		inline BlobDescriptor::DescriptorSSD ssd(const BlobFeature& feature) const;

		/**
		 * Returns whether the gaussian sign of two Blob featuers are identical
		 * and whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * @param feature Second feature
		 * @param threshold Upper SSD boundary, with range (0, infinity)
		 * @param ssd Resulting SSD between two descriptors, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isDescriptorEqual(const BlobFeature& feature, const BlobDescriptor::DescriptorSSD threshold, BlobDescriptor::DescriptorSSD& ssd) const;

		/**
		 * Returns whether the gaussian sign of two Blob featuers are identical
		 * and whether the summed square distance (SSD) between two descriptors is below a specified threshold.
		 * This test checks the inner kernel first and stopps if more than 80% of the threshold value is reached.
		 * @param feature Second feature
		 * @param threshold Upper SSD boundary, with range (0, infinity)
		 * @param ssd Resulting SSD between two descriptors if the calculation is not stopped early, with range [0, infinity)
		 * @return True, if so
		 */
		inline bool isDescriptorEqualEarlyReject(const BlobFeature& feature, const BlobDescriptor::DescriptorSSD threshold, BlobDescriptor::DescriptorSSD& ssd) const;

		/**
		 * Extracts the observation of one Blob feature.
		 * @param feature Blob feature to convert
		 * @return Resulting image point
		 */
		inline static Geometry::ImagePoint feature2imagePoint(const BlobFeature& feature);

		/**
		 * Converts Blob features to simple 2D image positions.
		 * @param features Blob features to convert
		 * @return Resulting image points
		 */
		inline static Geometry::ImagePoints features2imagePoints(const BlobFeatures& features);

		/**
		 * Converts Blob features to simple 2D image positions.
		 * @param features Blob feature to convert
		 * @param number Number of features to be converted
		 * @return Resulting image points
		 */
		inline static Geometry::ImagePoints features2imagePoints(const BlobFeatures& features, const unsigned int number);

		/**
		 * Converts Blob features to simple 3D objects positions.
		 * @param features Blob features to convert
		 * @return Resulting object points
		 */
		inline static Geometry::ObjectPoints features2objectPoints(const BlobFeatures& features);

		/**
		 * Converts Blob features to simple 3D object positions.
		 * @param features Blob feature to convert
		 * @param number Number of features to be converted
		 * @return Resulting object points
		 */
		inline static Geometry::ObjectPoints features2objectPoints(const BlobFeatures& features, const unsigned int number);

		/**
		 * Converts Blob features to point features.
		 * @param features Blob features to convert
		 * @return Resulting point features
		 */
		inline static PointFeatures features2pointFeatures(const BlobFeatures& features);

		/**
		 * Converts Blob features to point features.
		 * @param features Blob features to convert
		 * @param number Number of features to be converted
		 * @return Resulting point features
		 */
		inline static PointFeatures features2pointFeatures(const BlobFeatures& features, const unsigned int number);

	protected:

		/// Feature scale.
		Scalar featureScale;

		/// Feature laplace sign.
		bool featureLaplace;

		/// Feature descriptor type.
		DescriptorType featureDescriptorType;

		/// Feature orientation type.
		OrientationType featureOrientationType;

		/// Feature descriptor.
		BlobDescriptor featureDescriptor;
};

inline BlobFeature::BlobFeature() :
	OrientedPointFeature(),
	featureScale(0),
	featureLaplace(false),
	featureDescriptorType(DESCRIPTOR_UNDESCRIBED),
	featureOrientationType(ORIENTATION_NOT_ORIENTED)
{
	// nothing to do here
}

inline BlobFeature::BlobFeature(const Vector2& observation, const DistortionState distortionState, const Scalar scale, const Scalar strength, const bool laplace) :
	OrientedPointFeature(observation, distortionState, strength, 0),
	featureScale(scale),
	featureLaplace(laplace),
	featureDescriptorType(DESCRIPTOR_UNDESCRIBED),
	featureOrientationType(ORIENTATION_NOT_ORIENTED)
{
	// nothing to do here
}

inline BlobFeature::BlobFeature(const Vector2& observation, const DistortionState distortionState, const Scalar scale, const Scalar strength, const bool laplace, const Scalar orientation) :
	OrientedPointFeature(observation, distortionState, strength, orientation),
	featureScale(scale),
	featureLaplace(laplace),
	featureDescriptorType(DESCRIPTOR_UNDESCRIBED),
	featureOrientationType(ORIENTATION_NOT_ORIENTED)
{
	// nothing to do here
}

inline Scalar BlobFeature::scale() const
{
	return featureScale;
}

inline bool BlobFeature::laplace() const
{
	return featureLaplace;
}

inline const BlobDescriptor& BlobFeature::descriptor() const
{
	return featureDescriptor;
}

inline BlobDescriptor& BlobFeature::descriptor()
{
	return featureDescriptor;
}

inline BlobFeature::DescriptorType BlobFeature::descriptorType() const
{
	return featureDescriptorType;
}

inline BlobFeature::OrientationType BlobFeature::orientationType() const
{
	return featureOrientationType;
}

inline void BlobFeature::setScale(const Scalar scale)
{
	ocean_assert(scale > 0);
	featureScale = scale;
}

inline void BlobFeature::setLaplace(const bool laplace)
{
	featureLaplace = laplace;
}

inline void BlobFeature::setDescriptorType(const DescriptorType type)
{
	featureDescriptorType = type;
}

inline void BlobFeature::setOrientationType(const OrientationType type)
{
	featureOrientationType = type;
}

inline void BlobFeature::setType(const DescriptorType descriptorType, const OrientationType orientationType)
{
	featureDescriptorType = descriptorType;
	featureOrientationType = orientationType;
}

inline BlobDescriptor::DescriptorSSD BlobFeature::ssd(const BlobFeature& feature) const
{
	ocean_assert(featureDescriptorType != DESCRIPTOR_UNDESCRIBED);
	ocean_assert(featureDescriptorType == feature.featureDescriptorType);
	ocean_assert(featureOrientationType == feature.featureOrientationType);

	return featureDescriptor.ssd(feature.descriptor());
}

inline bool BlobFeature::isDescriptorEqual(const BlobFeature& feature, const BlobDescriptor::DescriptorSSD threshold, BlobDescriptor::DescriptorSSD& ssd) const
{
	ocean_assert(featureDescriptorType != DESCRIPTOR_UNDESCRIBED && feature.featureDescriptorType != DESCRIPTOR_UNDESCRIBED);
	ocean_assert(featureDescriptorType == feature.featureDescriptorType);
	ocean_assert(featureOrientationType == feature.featureOrientationType);

	if (featureLaplace != feature.featureLaplace)
		return false;

	return featureDescriptor.isDescriptorEqual(feature.descriptor(), threshold, ssd);
}

inline bool BlobFeature::isDescriptorEqualEarlyReject(const BlobFeature& feature, const BlobDescriptor::DescriptorSSD threshold, BlobDescriptor::DescriptorSSD& ssd) const
{
	ocean_assert(featureDescriptorType != DESCRIPTOR_UNDESCRIBED && feature.featureDescriptorType != DESCRIPTOR_UNDESCRIBED);
	ocean_assert(featureDescriptorType == feature.featureDescriptorType);
	ocean_assert(featureOrientationType == feature.featureOrientationType);

	if (featureLaplace != feature.featureLaplace)
		return false;

	return featureDescriptor.isDescriptorEqualEarlyReject(feature.descriptor(), threshold, ssd);
}

inline Geometry::ImagePoint BlobFeature::feature2imagePoint(const BlobFeature& feature)
{
	return feature.observation();
}

inline Geometry::ImagePoints BlobFeature::features2imagePoints(const BlobFeatures& features)
{
	Geometry::ImagePoints result;
	result.reserve(features.size());

	for (BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		result.push_back(i->observation());

	return result;
}

inline Geometry::ImagePoints BlobFeature::features2imagePoints(const BlobFeatures& features, const unsigned int number)
{
	const unsigned int realNumber = min(number, (unsigned int)features.size());

	Geometry::ImagePoints result;
	result.reserve(realNumber);

	for (unsigned int n = 0u; n < realNumber; ++n)
		result.push_back(features[n].observation());

	return result;
}

inline Geometry::ObjectPoints BlobFeature::features2objectPoints(const BlobFeatures& features)
{
	Geometry::ObjectPoints result;
	result.reserve(features.size());

	for (BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		result.push_back(i->position());

	return result;
}

inline Geometry::ObjectPoints BlobFeature::features2objectPoints(const BlobFeatures& features, const unsigned int number)
{
	const unsigned int realNumber = min(number, (unsigned int)features.size());

	Geometry::ObjectPoints result;
	result.reserve(realNumber);

	for (unsigned int n = 0u; n < realNumber; ++n)
		result.push_back(features[n].position());

	return result;
}

inline PointFeatures BlobFeature::features2pointFeatures(const BlobFeatures& features)
{
	PointFeatures result;
	result.reserve(features.size());

	for (BlobFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		result.push_back(*i);

	return result;
}

inline PointFeatures BlobFeature::features2pointFeatures(const BlobFeatures& features, const unsigned int number)
{
	const unsigned int realNumber = min(number, (unsigned int)features.size());

	PointFeatures result;
	result.reserve(realNumber);

	for (unsigned int n = 0u; n < realNumber; ++n)
		result.push_back(features[n]);

	return result;
}

}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_BLOB_BLOB_FEATURE_H
