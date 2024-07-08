/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_MATCHING_H
#define META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_MATCHING_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/DescriptorHandling.h"
#include "ocean/tracking/mapbuilding/PoseEstimationT.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptorMap.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptors.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/geometry/Octree.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/Database.h"
#include "ocean/tracking/VocabularyTree.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * The base class for all unified matching objects.
 * @ingroup trackingmapbuilding
 */
class UnifiedMatching
{
	public:

		/**
		 * Definition of a descriptor distance value.
		 */
		class DistanceValue
		{
			public:

				/**
				 * Default constructor with an invalid descriptor distance.
				 */
				DistanceValue() = default;

				/**
				 * Creates a new distance value for a binary descriptor.
				 * @param binaryDistance The binary distance, with range [0, infinity)
				 */
				explicit inline DistanceValue(const unsigned int binaryDistance);

				/**
				 * Creates a new distance for a floating point descriptor.
				 * @param floatDistance The floating point distance, with range [0, 1]
				 */
				explicit inline DistanceValue(const float floatDistance);

				/**
				 * Creates a new distance value which can be used for a binary descriptor and a floating point descriptor.
				 * @param binaryDistance The binary distance, with range [0, infinity)
				 * @param floatDistance The floating point distance, with range [0, 1]
				 */
				explicit inline DistanceValue(const unsigned int binaryDistance, const float floatDistance);

				/**
				 * Returns the binary distance.
				 * @return Binary distance, with range [0, infinity)
				 */
				inline unsigned int binaryDistance() const;

				/**
				 * Returns the floating point distance.
				 * @return Floating point distance, with range [0, 1]
				 */
				inline float floatDistance() const;

				/**
				 * Returns either the binary or the floating point distance.
				 * @return The binary or floating point distance depending on whether TDistance is an integer or a floating point
				 */
				template <typename TDistance>
				inline TDistance distance() const;

				/**
				 * Returns whether the object holds a valid distance.
				 * @return True, if so
				 */
				inline bool isValid() const;

			protected:

				/// The binary distance, with range [0, infinity), -1 if unknown.
				unsigned int binaryDistance_ = (unsigned int)(-1);

				/// The floating point distance, with range [0, 1], -1 if unknown
				float floatDistance_ = -1.0f;
		};

	public:

		/**
		 * Disposes this object.
		 */
		virtual ~UnifiedMatching() = default;

		/**
		 * Returns the number of image points.
		 * @return The number of image points
		 */
		inline size_t numberImagePoints() const;

	protected:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 */
		inline UnifiedMatching(const Vector3* objectPoints, const size_t numberObjectPoints);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 */
		inline UnifiedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints);

	protected:

		/// The 2D image points.
		const Vector2* imagePoints_ = nullptr;

		/// The number of 2D image points.
		size_t numberImagePoints_ = 0;

		/// The 3D object points.
		const Vector3* objectPoints_ = nullptr;

		/// The number of 3D object points.
		const size_t numberObjectPoints_ = 0;
};

/**
 * This class implements the base class for all guided matching objects.
 * @ingroup trackingmapbuilding
 */
class UnifiedGuidedMatching : public UnifiedMatching
{
	public:

		/**
		 * Determines the guided matching between 2D and 3D feature points.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param world_T_camera The camera pose, transforming camera to world, with default viewing direction into negative z-space and u-axis up, must be valid
		 * @param matchedImagePoints The resulting matched 2D image points
		 * @param matchedObjectPoints The resulting matched 3D object points, one for each matched image point
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptor count as match, must be valid
		 * @param matchedImagePointIndices Optional resulting indices of the matched 2D image points, nullptr if not of interest
		 * @param matchedObjectPointIds Optional resulting ids of the matched 3D image points, nullptr if not of interest
		 * @param worker Optional worker object to distribute the computation
		 */
		virtual void determineGuidedMatchings(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const DistanceValue& maximalDescriptorDistance, Indices32* matchedImagePointIndices = nullptr, Indices32* matchedObjectPointIds = nullptr, Worker* worker = nullptr) const = 0;

	protected:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 */
		inline UnifiedGuidedMatching(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 */
		inline UnifiedGuidedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds);

	protected:

		/// The octree holding all 3D object points.
		const Geometry::Octree& objectPointOctree_;

		/// The ids of all 3D object points.
		const Index32* objectPointIds_ = nullptr;
};

/**
 * Definition of a shared pointer holding an UnifiedGuidedMatching object.
 * @see UnifiedGuidedMatching.
 * @ingroup trackingmapbuilding
 */
using SharedUnifiedGuidedMatching = std::shared_ptr<UnifiedGuidedMatching>;

/**
 * This class implements the base class for all unguided matching objects.
 * @ingroup trackingmapbuilding
 */
class UnifiedUnguidedMatching : public UnifiedMatching
{
	public:

		/**
		 * Determines the unguided matching between 2D and 3D feature points.
		 * @param minimalNumberCorrespondences The minimal number of feature correspondences, with range [1, infinity)
		 * @param maximalDescriptorDistance The maximal descriptor distance so that two descriptor count as match, must be valid
		 * @param matchedImagePoints Optional resulting matched 2D image points, nullptr if not of interest
		 * @param matchedObjectPoints Optional resulting matched 3D image points, nullptr if not of interest
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		virtual bool determineUnguidedMatchings(const unsigned int minimalNumberCorrespondences, const DistanceValue& maximalDescriptorDistance, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, Worker* worker = nullptr) const = 0;

	protected:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 */
		inline UnifiedUnguidedMatching(const Vector3* objectPoints, const size_t numberObjectPoints, const Index32* objectPointIndices);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 */
		inline UnifiedUnguidedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Index32* objectPointIndices);

	protected:

		/// The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices.
		const Index32* objectPointIndices_ = nullptr;
};

/**
 * Definition of a shared pointer holding an UnifiedUnguidedMatching object.
 * @see UnifiedUnguidedMatching.
 * @ingroup trackingmapbuilding
 */
using SharedUnifiedUnguidedMatching = std::shared_ptr<UnifiedUnguidedMatching>;

/**
 * This class implements the guided matching object for specific features.
 * @tparam TImagePointDescriptor The data type of the image point descriptors, e.g., a single-level or a multi-level descriptor binary/float descriptor
 * @tparam TObjectPointDescriptor The data type of the object point descriptors, e.g., a single-level or multi-level single/multi-view descriptor
 * @tparam TDistance The data type of the distance between an image point and object point descriptor e.g., unsigned int or float
 * @ingroup trackingmapbuilding
 */
template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance = typename UnifiedDescriptor::DistanceTyper<TImagePointDescriptor>::Type>
class UnifiedGuidedMatchingT : public UnifiedGuidedMatching
{
	public:

		/// Definition of the distance data type.
		typedef TDistance DescriptorDistance;

		/// Definition of the descriptor for 2D image points.
		typedef TImagePointDescriptor ImagePointDescriptor;

		/// Definition of the descriptor for 3D object points.
		typedef TObjectPointDescriptor ObjectPointDescriptor;

		/**
		 * Definition of an unordered map mapping object point ids to descriptors.
		 */
		template <typename TDescriptor>
		using UnorderedDescriptorMap = std::unordered_map<Index32, TDescriptor>;

	public:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 * @param objectPointDescriptorMap The map mapping object point ids to their corresponding descriptors
		 */
		inline UnifiedGuidedMatchingT(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptors The descriptors for the image points, one for each image point, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 * @param objectPointDescriptorMap The map mapping object point ids to their corresponding descriptors
		 */
		inline UnifiedGuidedMatchingT(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap);

		/**
		 * Updates the 2D image points e.g., to allow matching for a new camera frame.
		 * The input data will not be copied.
		 * @param imagePoints The new 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptors The descriptors for the image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints The number of image points, with range [0, infinity)
		 */
		inline void updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints);

		/**
		 * Removes the image points from this object.
		 */
		inline void clearImagePoints();

		/**
		 * Determines the guided matching between 2D and 3D feature points.
		 * @see UnifiedGuidedMatching::determineGuidedMatchings().
		 */
		void determineGuidedMatchings(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const DistanceValue& maximalDescriptorDistance, Indices32* matchedImagePointIndices = nullptr, Indices32* matchedObjectPointIds = nullptr, Worker* worker = nullptr) const override;

	protected:

		/// The descriptors for the image points, one for each image point.
		const ImagePointDescriptor* imagePointDescriptors_ = nullptr;

		/// The map mapping object point ids to their corresponding descriptors.
		const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap_;
};

/**
 * Definition of a UnifiedGuidedMatchingT object for FREAK descriptors with 256 bits.
 * @ingroup trackingmapbuilding
 */
using UnifiedGuidedMatchingFreakMultiLevelDescriptor256 = UnifiedGuidedMatchingT<CV::Detector::FREAKDescriptor32, CV::Detector::FREAKDescriptors32>;

/**
 * Definition of a UnifiedGuidedMatchingT object for float descriptors.
 * @tparam tElements The number of float elements, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tElements>
using UnifiedGuidedMatchingFloatSingleLevelDescriptor = UnifiedGuidedMatchingT<UnifiedDescriptor::FloatDescriptor<tElements>, UnifiedDescriptor::FloatDescriptors<tElements>>;

/**
 * This class implements the unguided matching object for FREAK Multi features with 32 bytes or 256 bits.
 * @tparam TImagePointDescriptor The data type of the image point descriptors, e.g., a single-level or a multi-level descriptor binary/float descriptor
 * @tparam TObjectPointVocabularyDescriptor The data type of the object point descriptors, e.g., a single-level or multi-level single/multi-view descriptor
 * @tparam TDistance The data type of the distance between an image point and object point descriptor e.g., unsigned int or float
 * @ingroup trackingmapbuilding
 */
template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance = typename UnifiedDescriptor::DistanceTyper<TImagePointDescriptor>::Type>
class UnifiedUnguidedMatchingT : public UnifiedUnguidedMatching
{
	public:

		/// Definition of the distance for the binary descriptor.
		typedef TDistance DescriptorDistance;

		/// Definition of the descriptor for 2D image points.
		typedef TImagePointDescriptor ImagePointDescriptor;

		/// Definition of the descriptor for 3D object points.
		typedef TObjectPointVocabularyDescriptor ObjectPointVocabularyDescriptor;

		/// Definition of a vocabulary forest for object point descriptors.
		typedef Tracking::VocabularyForest<ObjectPointVocabularyDescriptor, DescriptorDistance, UnifiedDescriptorT<ObjectPointVocabularyDescriptor>::determineDistance> VocabularyForest;

		/// Definition of a vocabulary tree for object point descriptors.
		typedef typename VocabularyForest::TVocabularyTree VocabularyTree;

	public:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param objectPointVocabularyDescriptors The descriptors for the object points, one for each index in 'objectPointIndices'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices
		 * @param forestObjectPointDescriptors The vocabulary forest for the object point features
		 */
		inline UnifiedUnguidedMatchingT(const Vector3* objectPoints, const ObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const VocabularyForest& forestObjectPointDescriptors);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptors The descriptors for the image points, one for each image point, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param objectPointVocabularyDescriptors The descriptors for the object points, one for each index in 'objectPointIndices'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param forestObjectPointDescriptors The vocabulary forest for the object point features
		 */
		inline UnifiedUnguidedMatchingT(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const ObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const VocabularyForest& forestObjectPointDescriptors);

		/**
		 * Updates the 2D image points e.g., to allow matching for a new camera frame.
		 * The input data will not be copied.
		 * @param imagePoints The new 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptors The descriptors for the image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints The number of image points, with range [0, infinity)
		 */
		inline void updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints);

		/**
		 * Removes the image points from this object.
		 */
		inline void clearImagePoints();

		/**
		 * Determines the unguided matching between 2D and 3D feature points.
		 * @see UnifiedUnguidedMatching::determineUnguidedMatchings().
		 */
		bool determineUnguidedMatchings(const unsigned int minimalNumberCorrespondences, const DistanceValue& maximalDescriptorDistance, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, Worker* worker = nullptr) const override;

	protected:

		/// The descriptors for the image points, one for each image point.
		const ImagePointDescriptor* imagePointDescriptors_ = nullptr;

		/// The descriptors for the object points, one for each index in 'objectPointIndices'.
		const ObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors_;

		/// The vocabulary forest for the object point features.
		const VocabularyForest& forestObjectPointDescriptors_;
};

/**
 * Definition of an UnifiedUnguidedMatchingT object for FREAK descriptors with 256 bits.
 * @ingroup trackingmapbuilding
 */
using UnifiedUnguidedMatchingFreakMultiLevelDescriptor256 = UnifiedUnguidedMatchingT<CV::Detector::FREAKDescriptor32, UnifiedDescriptor::BinaryDescriptor<256u>>;

/**
 * Definition of an UnifiedUnguidedMatchingT object for float descriptors.
 * @tparam tElements The number of float elements the descriptors have, with range [1, infinity)
 * @ingroup trackingmapbuilding
 */
template <uint16_t tElements>
using UnifiedUnguidedMatchingFloatDescriptor = UnifiedUnguidedMatchingT<UnifiedDescriptor::FloatDescriptor<tElements>, UnifiedDescriptor::FloatDescriptor<tElements>>;

/**
 * This class implements the guided matching object for groups of FREAK Multi features with 32 bytes or 256 bits.
 * @ingroup trackingmapbuilding
 */
class UnifiedGuidedMatchingFreakMultiDescriptor256Group : public UnifiedGuidedMatching
{
	public:
		/// Definition of the distance for the binary descriptor.
		typedef unsigned int DescriptorDistance;

		/// Definition of the descriptor for 2D image points.
		typedef CV::Detector::FREAKDescriptor32 ImagePointDescriptor;

		/// Definition of the groups of descriptors for 2D image points.
		typedef const CV::Detector::FREAKDescriptors32* ImagePointDescriptorGroup;

		/// Definition of the descriptor for 3D object points (several descriptors per point possible).
		typedef CV::Detector::FREAKDescriptors32 ObjectPointDescriptor;

		/**
		 * Definition of an unordered map mapping object point ids to descriptors.
		 */
		template <typename TDescriptor>
		using UnorderedDescriptorMap = std::unordered_map<Index32, TDescriptor>;

	public:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 * @param objectPointDescriptorMap The map mapping object point ids to their corresponding descriptors
		 */
		inline UnifiedGuidedMatchingFreakMultiDescriptor256Group(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptorGroups The groups of descriptors for the image points, one for each image point, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointOctree The octree holding all 3D object points
		 * @param objectPointIds The ids of all 3D object points, must be valid
		 * @param objectPointDescriptorMap The map mapping object point ids to their corresponding descriptors
		 */
		inline UnifiedGuidedMatchingFreakMultiDescriptor256Group(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap);

		/**
		 * Determines the guided matching between 2D and 3D feature points.
		 * @see UnifiedGuidedMatching::determineGuidedMatchings().
		 */
		void determineGuidedMatchings(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const DistanceValue& maximalDescriptorDistance, Indices32* matchedImagePointIndices = nullptr, Indices32* matchedObjectPointIds = nullptr, Worker* worker = nullptr) const override;

		/**
		 * Updates the 2D image points e.g., to allow matching for a new camera frame.
		 * The input data will not be copied.
		 * @param imagePoints The new 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptorGroups The descriptors for the image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints The number of image points, with range [0, infinity)
		 */
		inline void updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints);

		/**
		 * Removes the image points from this object.
		 */
		inline void clearImagePoints();

	protected:

		/// The groups of descriptors for the image points, one for each image point.
		const ImagePointDescriptorGroup* imagePointDescriptorGroups_ = nullptr;

		/// The map mapping object point ids to their corresponding descriptors.
		const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap_;
};

/**
 * This class implements the unguided matching object for groups of FREAK Multi features with 32 bytes or 256 bits.
 * @ingroup trackingmapbuilding
 */
class UnifiedUnguidedMatchingFreakMultiFeatures256Group : public UnifiedUnguidedMatching
{
	public:

		/// Definition of the distance for the binary descriptor.
		typedef unsigned int DescriptorDistance;

		/// Definition of the descriptor for 2D image points.
		typedef CV::Detector::FREAKDescriptor32 ImagePointDescriptor;

		/// Definition of the groups of descriptors for 2D image points.
		typedef const CV::Detector::FREAKDescriptors32* ImagePointDescriptorGroup;

		/// Definition of the descriptor for 3D object points (several descriptors per point possible).
		typedef UnifiedDescriptor::BinaryDescriptor<256u> ObjectPointDescriptor;

		/// Definition of a vocabulary forest for object point descriptors.
		typedef Tracking::VocabularyForest<ObjectPointDescriptor, DescriptorDistance, UnifiedDescriptorT<ObjectPointDescriptor>::determineDistance> BinaryVocabularyForest;

		/// Definition of a vocabulary tree for object point descriptors.
		typedef BinaryVocabularyForest::TVocabularyTree BinaryVocabularyTree;

	public:

		/**
		 * Creates a new matching object with 3D object points only.
		 * Does not create a copy of the given input.
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param objectPointDescriptors The descriptors for the object points, one for each index in 'objectPointIndices'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices
		 * @param forestObjectPointDescriptors The vocabulary forest for the object point features
		 */
		inline UnifiedUnguidedMatchingFreakMultiFeatures256Group(const Vector3* objectPoints, const ObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const BinaryVocabularyForest& forestObjectPointDescriptors);

		/**
		 * Creates a new matching object with 2D image points and 3D object points only.
		 * Does not create a copy of the given input.
		 * @param imagePoints The 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptorGroups The groups of descriptors for the image points, one for each image point, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints the number of 2D image points, with range [0, infinity)
		 * @param objectPoints The 3D object points, can be nullptr if 'numberObjectPoints == 0'
		 * @param objectPointDescriptors The descriptors for the object points, one for each index in 'objectPointIndices'
		 * @param numberObjectPoints The number of 3D object points, with range [0, infinity)
		 * @param objectPointIndices The indices of the corresponding 3D object points, one for each object point descriptor, mainly a map mapping descriptor indices to point indices, must be valid
		 * @param forestObjectPointDescriptors The vocabulary forest for the object point features
		 */
		inline UnifiedUnguidedMatchingFreakMultiFeatures256Group(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints, const Vector3* objectPoints, const ObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const BinaryVocabularyForest& forestObjectPointDescriptors);

		/**
		 * Updates the 2D image points e.g., to allow matching for a new camera frame.
		 * The input data will not be copied.
		 * @param imagePoints The new 2D image points, can be nullptr if 'numberImagePoints == 0'
		 * @param imagePointDescriptorGroups The descriptors for the image points, can be nullptr if 'numberImagePoints == 0'
		 * @param numberImagePoints The number of image points, with range [0, infinity)
		 */
		inline void updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints);

		/**
		 * Removes the image points from this object.
		 */
		inline void clearImagePoints();

		/**
		 * Determines the guided matching between 2D and 3D feature points.
		 * @see UnifiedUnguidedMatching::determineUnguidedMatchings().
		 */
		bool determineUnguidedMatchings(const unsigned int minimalNumberCorrespondences, const DistanceValue& maximalDescriptorDistance, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, Worker* worker = nullptr) const override;

	protected:

		/// The groups of descriptors for the image points, one for each image point.
		const ImagePointDescriptorGroup* imagePointDescriptorGroups_ = nullptr;

		/// The descriptors for the object points, one for each index in 'objectPointIndices'.
		const ObjectPointDescriptor* objectPointDescriptors_;

		/// The vocabulary forest for the object point features.
		const BinaryVocabularyForest& forestObjectPointDescriptors_;
};

inline UnifiedMatching::DistanceValue::DistanceValue(const unsigned int binaryDistance) :
	binaryDistance_(binaryDistance)
{
	ocean_assert(binaryDistance_ != (unsigned int)(-1));
}

inline UnifiedMatching::DistanceValue::DistanceValue(const float floatDistance) :
	floatDistance_(floatDistance)
{
	ocean_assert(floatDistance_ >= 0.0f && floatDistance_ <= 1.0f);
}

inline UnifiedMatching::DistanceValue::DistanceValue(const unsigned int binaryDistance, const float floatDistance) :
	binaryDistance_(binaryDistance),
	floatDistance_(floatDistance)
{
	ocean_assert(isValid());
}

inline unsigned int UnifiedMatching::DistanceValue::binaryDistance() const
{
	ocean_assert(binaryDistance_ != (unsigned int)(-1));
	return binaryDistance_;
}

inline float UnifiedMatching::DistanceValue::floatDistance() const
{
	ocean_assert(floatDistance_ >= 0.0f);
	return floatDistance_;
}

template <typename TDistance>
inline TDistance UnifiedMatching::DistanceValue::distance() const
{
	if constexpr (std::is_floating_point<TDistance>::value)
	{
		return TDistance(floatDistance_);
	}
	else
	{
		ocean_assert(NumericT<TDistance>::isInsideValueRange(binaryDistance_));

		return TDistance(binaryDistance_);
	}
}

inline bool UnifiedMatching::DistanceValue::isValid() const
{
	return binaryDistance_ != (unsigned int)(-1) || (floatDistance_ >= 0.0f && floatDistance_ <= 1.0f);
}

inline UnifiedMatching::UnifiedMatching(const Vector3* objectPoints, const size_t numberObjectPoints) :
	objectPoints_(objectPoints),
	numberObjectPoints_(numberObjectPoints)
{
	// nothing to do here
}

inline UnifiedMatching::UnifiedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints) :
	imagePoints_(imagePoints),
	numberImagePoints_(numberImagePoints),
	objectPoints_(objectPoints),
	numberObjectPoints_(numberObjectPoints)
{
	// nothing to do here
}

inline size_t UnifiedMatching::numberImagePoints() const
{
	return numberImagePoints_;
}

inline UnifiedGuidedMatching::UnifiedGuidedMatching(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds) :
	UnifiedMatching(objectPoints, numberObjectPoints),
	objectPointOctree_(objectPointOctree),
	objectPointIds_(objectPointIds)
{
	// nothing to do here
}

inline UnifiedGuidedMatching::UnifiedGuidedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds) :
	UnifiedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints),
	objectPointOctree_(objectPointOctree),
	objectPointIds_(objectPointIds)
{
	// nothing to do here
}

inline UnifiedUnguidedMatching::UnifiedUnguidedMatching(const Vector3* objectPoints, const size_t numberObjectPoints, const Index32* objectPointIndices) :
	UnifiedMatching(objectPoints, numberObjectPoints),
	objectPointIndices_(objectPointIndices)
{
	// nothing to do here
}

inline UnifiedUnguidedMatching::UnifiedUnguidedMatching(const Vector2* imagePoints, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Index32* objectPointIndices) :
	UnifiedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints),
	objectPointIndices_(objectPointIndices)
{
	// nothing to do here
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance>
inline UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor, TDistance>::UnifiedGuidedMatchingT(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap) :
	UnifiedGuidedMatching(objectPoints, numberObjectPoints, objectPointOctree, objectPointIds),
	objectPointDescriptorMap_(objectPointDescriptorMap)
{
	// nothing to do here
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance>
inline UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor, TDistance>::UnifiedGuidedMatchingT(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap) :
	UnifiedGuidedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints, objectPointOctree, objectPointIds),
	imagePointDescriptors_(imagePointDescriptors),
	objectPointDescriptorMap_(objectPointDescriptorMap)
{
	// nothing to do here
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance>
inline void UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor, TDistance>::updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints)
{
	imagePoints_ = imagePoints;
	imagePointDescriptors_ = imagePointDescriptors;
	numberImagePoints_ = numberImagePoints;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance>
inline void UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor, TDistance>::clearImagePoints()
{
	imagePoints_ = nullptr;
	imagePointDescriptors_ = nullptr;
	numberImagePoints_ = 0;
}

template <typename TImagePointDescriptor, typename TObjectPointDescriptor, typename TDistance>
void UnifiedGuidedMatchingT<TImagePointDescriptor, TObjectPointDescriptor, TDistance>::determineGuidedMatchings(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, const DistanceValue& maximalDescriptorDistance, Indices32* matchedImagePointIndices, Indices32* matchedObjectPointIds, Worker* worker) const
{
	PoseEstimationT::determineGuidedMatchings<ImagePointDescriptor, ObjectPointDescriptor, DescriptorDistance, UnifiedDescriptorT<TImagePointDescriptor>::determineDistance>(anyCamera, world_T_camera, imagePoints_, imagePointDescriptors_, numberImagePoints_, objectPoints_, objectPointOctree_, objectPointIds_, objectPointDescriptorMap_, matchedImagePoints, matchedObjectPoints, maximalDescriptorDistance.distance<TDistance>(), matchedImagePointIndices, matchedObjectPointIds, worker);
}

template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance>
inline UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor, TDistance>::UnifiedUnguidedMatchingT(const Vector3* objectPoints, const ObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const VocabularyForest& forestObjectPointDescriptors) :
	UnifiedUnguidedMatching(objectPoints, numberObjectPoints, objectPointIndices),
	objectPointVocabularyDescriptors_(objectPointVocabularyDescriptors),
	forestObjectPointDescriptors_(forestObjectPointDescriptors)
{
	// nothing to do here
}

template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance>
inline UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor, TDistance>::UnifiedUnguidedMatchingT(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints, const Vector3* objectPoints, const ObjectPointVocabularyDescriptor* objectPointVocabularyDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const VocabularyForest& forestObjectPointDescriptors) :
	UnifiedUnguidedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints, objectPointIndices),
	imagePointDescriptors_(imagePointDescriptors),
	objectPointVocabularyDescriptors_(objectPointVocabularyDescriptors),
	forestObjectPointDescriptors_(forestObjectPointDescriptors)
{
	// nothing to do here
}

template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance>
inline void UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor, TDistance>::updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptor* imagePointDescriptors, const size_t numberImagePoints)
{
	imagePoints_ = imagePoints;
	imagePointDescriptors_ = imagePointDescriptors;
	numberImagePoints_ = numberImagePoints;
}

template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance>
inline void UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor, TDistance>::clearImagePoints()
{
	imagePoints_ = nullptr;
	imagePointDescriptors_ = nullptr;
	numberImagePoints_ = 0;
}

template <typename TImagePointDescriptor, typename TObjectPointVocabularyDescriptor, typename TDistance>
bool UnifiedUnguidedMatchingT<TImagePointDescriptor, TObjectPointVocabularyDescriptor, TDistance>::determineUnguidedMatchings(const unsigned int minimalNumberCorrespondences, const DistanceValue& maximalDescriptorDistance, Vectors2& matchedImagePoints, Vectors3& matchedObjectPoints, Worker* worker) const
{
	if (imagePoints_ == nullptr || imagePointDescriptors_ == nullptr || numberImagePoints_ == 0)
	{
		return false;
	}

	typename VocabularyForest::Matches matches;

	if constexpr (std::is_same<TImagePointDescriptor, DescriptorHandling::FreakMultiDescriptor256>::value) // TODO HACK
	{
		forestObjectPointDescriptors_.template matchMultiDescriptors<ImagePointDescriptor, DescriptorHandling::multiDescriptorFunction, VocabularyTree::MM_ALL_GOOD_LEAFS_2>(objectPointVocabularyDescriptors_, imagePointDescriptors_, numberImagePoints_, maximalDescriptorDistance.distance<TDistance>(), matches, worker);
	}
	else
	{
		forestObjectPointDescriptors_.template matchDescriptors<VocabularyTree::MM_ALL_GOOD_LEAFS_2>(objectPointVocabularyDescriptors_, imagePointDescriptors_, numberImagePoints_, maximalDescriptorDistance.distance<TDistance>(), matches, worker);
	}

	if (matches.size() < size_t(minimalNumberCorrespondences))
	{
		return false;
	}

	ocean_assert(matchedImagePoints.empty());
	ocean_assert(matchedObjectPoints.empty());

	matchedImagePoints.clear();
	matchedObjectPoints.clear();

	for (const typename VocabularyTree::Match& match : matches)
	{
		matchedImagePoints.emplace_back(imagePoints_[match.queryDescriptorIndex()]);
		matchedObjectPoints.emplace_back(objectPoints_[objectPointIndices_[match.candidateDescriptorIndex()]]);
	}

	return true;
}

inline UnifiedGuidedMatchingFreakMultiDescriptor256Group::UnifiedGuidedMatchingFreakMultiDescriptor256Group(const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap) :
	UnifiedGuidedMatching(objectPoints, numberObjectPoints, objectPointOctree, objectPointIds),
	objectPointDescriptorMap_(objectPointDescriptorMap)
{
	// nothing to do here
}

inline UnifiedGuidedMatchingFreakMultiDescriptor256Group::UnifiedGuidedMatchingFreakMultiDescriptor256Group(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints, const Vector3* objectPoints, const size_t numberObjectPoints, const Geometry::Octree& objectPointOctree, const Index32* objectPointIds, const UnorderedDescriptorMap<ObjectPointDescriptor>& objectPointDescriptorMap) :
	UnifiedGuidedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints, objectPointOctree, objectPointIds),
	imagePointDescriptorGroups_(imagePointDescriptorGroups),
	objectPointDescriptorMap_(objectPointDescriptorMap)
{
	// nothing to do here
}

inline void UnifiedGuidedMatchingFreakMultiDescriptor256Group::updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints)
{
	imagePoints_ = imagePoints;
	imagePointDescriptorGroups_ = imagePointDescriptorGroups;
	numberImagePoints_ = numberImagePoints;
}

inline void UnifiedGuidedMatchingFreakMultiDescriptor256Group::clearImagePoints()
{
	imagePoints_ = nullptr;
	imagePointDescriptorGroups_ = nullptr;
	numberImagePoints_ = 0;
}

inline UnifiedUnguidedMatchingFreakMultiFeatures256Group::UnifiedUnguidedMatchingFreakMultiFeatures256Group(const Vector3* objectPoints, const ObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const BinaryVocabularyForest& forestObjectPointDescriptors) :
	UnifiedUnguidedMatching(objectPoints, numberObjectPoints, objectPointIndices),
	objectPointDescriptors_(objectPointDescriptors),
	forestObjectPointDescriptors_(forestObjectPointDescriptors)
{
	// nothing to do here
}

inline UnifiedUnguidedMatchingFreakMultiFeatures256Group::UnifiedUnguidedMatchingFreakMultiFeatures256Group(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints, const Vector3* objectPoints, const ObjectPointDescriptor* objectPointDescriptors, const size_t numberObjectPoints, const Index32* objectPointIndices, const BinaryVocabularyForest& forestObjectPointDescriptors) :
	UnifiedUnguidedMatching(imagePoints, numberImagePoints, objectPoints, numberObjectPoints, objectPointIndices),
	imagePointDescriptorGroups_(imagePointDescriptorGroups),
	objectPointDescriptors_(objectPointDescriptors),
	forestObjectPointDescriptors_(forestObjectPointDescriptors)
{
	// nothing to do here
}

inline void UnifiedUnguidedMatchingFreakMultiFeatures256Group::updateImagePoints(const Vector2* imagePoints, const ImagePointDescriptorGroup* imagePointDescriptorGroups, const size_t numberImagePoints)
{
	imagePoints_ = imagePoints;
	imagePointDescriptorGroups_ = imagePointDescriptorGroups;
	numberImagePoints_ = numberImagePoints;
}

inline void UnifiedUnguidedMatchingFreakMultiFeatures256Group::clearImagePoints()
{
	imagePoints_ = nullptr;
	imagePointDescriptorGroups_ = nullptr;
	numberImagePoints_ = 0;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_UNIFIED_MATCHING_H
