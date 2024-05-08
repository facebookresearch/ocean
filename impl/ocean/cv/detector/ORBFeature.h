/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_ORB_FEATURE_H
#define META_OCEAN_CV_DETECTOR_ORB_FEATURE_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/FASTFeature.h"
#include "ocean/cv/detector/OrientedPointFeature.h"
#include "ocean/cv/detector/ORBDescriptor.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration.
class ORBFeature;

/**
 * Definition of a vector holding ORB features.
 * @ingroup cvdetector
 */
typedef std::vector<ORBFeature> ORBFeatures;

/**
 * This class implements a ORB feature.
 * A ORB feature can hold multiple descriptors.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT ORBFeature : public OrientedPointFeature
{
	public:

		/**
		 * Definition of the ORB descriptor type for this feature.
		 */
		enum FeatureDescriptorType
		{
			/// Undescribed descriptor.
			FDT_UNDESCRIBED,
			/// Oriented descriptor.
			FDT_ORIENTED
		};

	public:

		/**
		 * Creates a new empty ORB feature object.
		 */
		inline ORBFeature();

		/**
		 * Creates a new ORB feature object by a given 2D observation position in e.g. an image.
		 * @param observation 2D feature observation
		 * @param distortionState Distortion state of the 2D feature position
		 * @param strength Strength of the feature
		 * @param orientation Orientation angle of the feature in radian, range [0, 2*PI)
		 */
		inline ORBFeature(const Vector2& observation, const DistortionState distortionState = DS_UNKNOWN, const Scalar strength = Scalar(0), const Scalar orientation = Scalar(0));

		/**
		 * Returns the number of descriptors holding by this feature point.
		 * @return Number of descriptors
		 */
		inline size_t numberDescriptors() const;

		/**
		 * Returns the first descriptor of this feature.
		 * Beware: Ensure that this object holds at least one descriptor before accessing it.
		 * @return First feature descriptor
		 */
		inline const ORBDescriptor& firstDescriptor() const;

		/**
		 * Returns the first descriptor of this feature.
		 * Beware: Ensure that this object holds at least one descriptor before accessing it.
		 * @return First feature descriptor
		 */
		inline ORBDescriptor& firstDescriptor();

		/**
		 * Returns a vector of all descriptors of this feature.
		 * Beware: Ensure to check the size of the vector before accessing elements; the vector can be empty
		 * @return All Feature descriptors
		 */
		inline const ORBDescriptors& descriptors() const;

		/**
		 * Returns a vector of all descriptors of this feature.
		 * Beware: Ensure to check the size of the vector before accessing elements; the vector can be empty
		 * @return All Feature descriptors
		 */
		inline ORBDescriptors& descriptors();

		/**
		 * Adds a given descriptor to this feature.
		 * @param descriptor Descriptor to add
		 */
		inline void addDescriptor(const ORBDescriptor& descriptor);

		/**
		 * Returns the type of all descriptors of this feature.
		 * @return Descriptor type
		 */
		inline FeatureDescriptorType descriptorType() const;

		/**
		 * Sets or changes the type of all descriptors of this feature.
		 * @param type Descriptor type to set
		 */
		inline void setDescriptorType(const FeatureDescriptorType type);

		/**
		 * Converts a vector of point features to ORB feature points.
		 * @param features Point features to convert
		 * @return Converted ORB feature points
		 */
		template <typename T>
		static ORBFeatures features2ORBFeatures(const std::vector<T>& features);

		/**
		 * Converts a vector of point features to ORB feature points.
		 * Further, feature points too close to the image border will be skipped.
		 * @param features The point features to convert
		 * @param width The width of the frame in which the feature points have been detected, in pixel, with range [border, infinity)
		 * @param height The height of the frame in which the feature points have been detected, in pixel, with range [border, infinity)
		 * @param border The minimal distance between the image border and a feature points, with range [0, infinity)
		 * @param validIndices Optional resulting indices of features which were not skipped
		 * @return The resulting ORB feature points
		 */
		template <typename T>
		static ORBFeatures features2ORBFeatures(const std::vector<T>& features, const unsigned int width, const unsigned int height, const unsigned int border = 31u, Indices32* validIndices = nullptr);

	protected:

		/// Feature descriptor type.
		FeatureDescriptorType featureDescriptorType;

		/// Feature descriptor.
		ORBDescriptors featureDescriptors;
};

inline ORBFeature::ORBFeature() :
	OrientedPointFeature(),
	featureDescriptorType(FDT_UNDESCRIBED)
{
	// nothing to do here
}

inline ORBFeature::ORBFeature(const Vector2& observation, const DistortionState distortionState, const Scalar strength, const Scalar orientation) :
	OrientedPointFeature(observation, distortionState, strength, orientation),
	featureDescriptorType(FDT_UNDESCRIBED)
{
	// nothing to do here
}

inline size_t ORBFeature::numberDescriptors() const
{
	return featureDescriptors.size();
}

inline const ORBDescriptor& ORBFeature::firstDescriptor() const
{
	ocean_assert(!featureDescriptors.empty());
	return featureDescriptors.front();
}

inline ORBDescriptor& ORBFeature::firstDescriptor()
{
	ocean_assert(!featureDescriptors.empty());
	return featureDescriptors.front();
}

inline const ORBDescriptors& ORBFeature::descriptors() const
{
	return featureDescriptors;
}

inline ORBDescriptors& ORBFeature::descriptors()
{
	return featureDescriptors;
}

inline void ORBFeature::addDescriptor(const ORBDescriptor& descriptor)
{
	featureDescriptors.pushBack(descriptor);
}

inline ORBFeature::FeatureDescriptorType ORBFeature::descriptorType() const
{
	return featureDescriptorType;
}

inline void ORBFeature::setDescriptorType(const FeatureDescriptorType type)
{
	featureDescriptorType = type;
}

template <typename T>
ORBFeatures ORBFeature::features2ORBFeatures(const std::vector<T>& features)
{
	ORBFeatures result;
	result.resize(features.size());

	for (size_t i = 0; i < features.size(); i++)
	{
		const PointFeature& feature = features[i];
		result.push_back(ORBFeature(feature.observation(), feature.distortionState(), feature.strength()));
	}

	return result;
}

template <typename T>
ORBFeatures ORBFeature::features2ORBFeatures(const std::vector<T>& features, const unsigned int width, const unsigned int height, const unsigned int border, Indices32* validIndicesPtr)
{
	ORBFeatures result;
	result.reserve(features.size());

	Indices32 validIndices;
	validIndices.reserve(features.size());

	const Scalar borderLeftTop = Scalar(border);
	const Scalar borderRight = Scalar(width) - Scalar(border);
	const Scalar borderBottom = Scalar(height) - Scalar(border);

	for (size_t i = 0; i < features.size(); i++)
	{
		const PointFeature& feature = features[i];

		const Scalar x = feature.observation().x();
		const Scalar y = feature.observation().y();

		if (x >= borderLeftTop && y >= borderLeftTop && x < borderRight && y < borderBottom)
		{
			result.push_back(ORBFeature(feature.observation(), feature.distortionState(), feature.strength()));
			validIndices.push_back((Index32)i);
		}
	}

	ocean_assert(validIndices.size() == result.size());

	if (validIndicesPtr) {
		*validIndicesPtr = std::move(validIndices);
	}

	return result;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORB_FEATURE_H
