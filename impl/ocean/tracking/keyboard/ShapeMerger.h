// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_KEYBOARD_SHAPE_MERGER_H
#define META_OCEAN_TRACKING_KEYBOARD_SHAPE_MERGER_H

#include "ocean/tracking/keyboard/Keyboard.h"
#include "ocean/tracking/keyboard/ShapeMatcher.h"

#include "ocean/base/Accessor.h"

#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Tracking
{

namespace Keyboard
{

/**
 * This class implements a merger for shapes.
 * The merger can be used to merge corresponding shapes which have been observed within individual images.<br>
 * The individual frames must align with each other e.g., a series of rectified images of a specific area (e.g., several rectified images of a keyboard).
 * @ingroup trackingkeyboard
 */
class OCEAN_TRACKING_KEYBOARD_EXPORT ShapeMerger
{
	public:

		using LShape = CV::Detector::ShapeDetector::LShape;
		using LShapes = CV::Detector::ShapeDetector::LShapes;

		using TShape = CV::Detector::ShapeDetector::TShape;
		using TShapes = CV::Detector::ShapeDetector::TShapes;

		using XShape = CV::Detector::ShapeDetector::XShape;
		using XShapes = CV::Detector::ShapeDetector::XShapes;

		/**
		 * Definition of a pair combining a pose index (a frame index) with a 2D location.
		 */
		typedef std::pair<Index32, Vector2> ObservationPair;

		/**
		 * Definition of a vector holding several observation pairs.
		 */
		typedef std::vector<ObservationPair> ObservationPairs;

		/**
		 * Definition of a vector holding a group of observation pairs.
		 */
		typedef std::vector<ObservationPairs> ObservationPairGroups;

	public:

		/**
		 * Creates an invalid ShapeMerger object.
		 */
		ShapeMerger();

		/**
		 * Creates a new ShapeMerger object.
		 * @param width The width of the all images in which shapes will be merged, in pixels, with range [1, infinity)
		 * @param height The height of the all images in which shapes will be merged, in pixels, with range [1, infinity)
		 * @param maximalDistance The maximal distance between two shape to be considered to be similar (and thus can be merged), in pixel, with range [0, infinity).
		 */
		ShapeMerger(const unsigned int width, const unsigned int height, const Scalar maximalDistance);

		/**
		 * Returns the width of this merger.
		 * @return The merger's width, in pixels, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this merger.
		 * @return The merger's width, in pixels, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the maximal distance between two shapes to be considered to be similar.
		 * @return The shape's maximal distance, in pixel, with range [0, infinity)
		 */
		inline Scalar maximalDistance() const;

		/**
		 * Returns the number of observations (number of frames) the merger has access to.
		 * @return The merger's observations, with range [0, infinity)
		 */
		inline size_t observations() const;

		/**
		 * Returns all L-shapes which have been observed in any frames.
		 * @return The merger's L-shapes without any specific order
		 */
		inline const LShapes& lShapes() const;

		/**
		 * Returns all T-shapes which have been observed in any frames.
		 * @return The merger's T-shapes without any specific order
		 */
		inline const TShapes& tShapes() const;

		/**
		 * Returns all X-shapes which have been observed in any frames.
		 * @return The merger's X-shapes without any specific order
		 */
		inline const XShapes& xShapes() const;

		/**
		 * Adds the observation of new shapes all observed in the same frame.
		 * @param poseIndex The index of the frame in which the shapes have been observed, with range [0, infinity)
		 * @param lShapes The L-shapes which have been observed
		 * @param tShapes The T-shapes which have been observed
		 * @param xShapes the X-shapes which have been observed
		 */
		void addObservations(const unsigned int poseIndex, const ConstArrayAccessor<LShape>& lShapes, const ConstArrayAccessor<TShape>& tShapes, const ConstArrayAccessor<XShape>& xShapes);

		void mergeObservations(const unsigned int minimalObservations, LShapes& mergedLShapes, TShapes& mergedTShapes, XShapes& mergedXShapes, ObservationPairGroups* mergedLShapesObservationPairGroups = nullptr, ObservationPairGroups* mergedTShapesObservationPairGroups = nullptr, ObservationPairGroups* mergedXShapesObservationPairGroups = nullptr) const;

		/**
		 * Removes all observations so that new observations can be added.
		 */
		void clear();

		/**
		 * Returns whether this merger is valid and can be used to merge shapes.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		template <typename T>
		static std::vector<T> mergeShapes(const std::vector<T>& shapes, Indices32& usedShapeIndices, const Geometry::SpatialDistribution::DistributionArray& distributionArray, const ShapeMatcher& shapeMatcher, const unsigned int minimalObservations, IndexGroups32* mergedShapeIndexGroups = nullptr);

		template <typename T>
		static void mergeShapes(const std::vector<T>& shapes, const unsigned int shapeIndex, const Geometry::SpatialDistribution::DistributionArray& distributionArray, const ShapeMatcher& shapeMatcher, const Indices32& usedIndices, Indices32& mergedIndices);

		template <typename T0, typename T1>
		static void mergeShapes(const T0& shape0, const std::vector<T1>& shapes1, const Geometry::SpatialDistribution::DistributionArray& distributionArray1, const ShapeMatcher& shapeMatcher, const Indices32& usedIndices1, Indices32& mergedIndices1);

	protected:

		/// The maximal distance between two shape objects to be considered to be similar (and thus can be merged), in pixel, with range [0, infinity).
		Scalar maximalDistance_ = Scalar(-1);

		/// The number of observations (number of frames) the merger has access to, with range [0, infinity).
		size_t observations_ = 0;

		/// The L-shapes which have been observed.
		LShapes lShapes_;

		/// The T-shapes which have been observed.
		TShapes tShapes_;

		/// The X-shapes which have been observed.
		XShapes xShapes_;

		/// The pose indices of each individual L-shape (the indices of the poses/frames in which the shapes have been observed), one for each L-shape.
		Indices32 lShapes2poseIndex_;

		/// The pose indices of each individual T-shape (the indices of the poses/frames in which the shapes have been observed), one for each T-shape.
		Indices32 tShapes2poseIndex_;

		/// The pose indices of each individual X-shape (the indices of the poses/frames in which the shapes have been observed), one for each X-shape.
		Indices32 xShapes2poseIndex_;

		/// The distribution array of all observed L-shapes.
		Geometry::SpatialDistribution::DistributionArray distributionArrayLShapes_;

		/// The distribution array of all observed T-shapes.
		Geometry::SpatialDistribution::DistributionArray distributionArrayTShapes_;

		/// The distribution array of all observed X-shapes.
		Geometry::SpatialDistribution::DistributionArray distributionArrayXShapes_;
};

inline ShapeMerger::operator bool() const
{
	return maximalDistance_ >= Scalar(0);
}

inline unsigned int ShapeMerger::width() const
{
	ocean_assert(distributionArrayLShapes_.width() == distributionArrayTShapes_.width());
	ocean_assert(distributionArrayLShapes_.width() == distributionArrayXShapes_.width());

	return (unsigned int)(distributionArrayLShapes_.width());
}

inline unsigned int ShapeMerger::height() const
{
	ocean_assert(distributionArrayLShapes_.height() == distributionArrayTShapes_.height());
	ocean_assert(distributionArrayLShapes_.height() == distributionArrayXShapes_.height());

	return (unsigned int)(distributionArrayLShapes_.height());
}

inline Scalar ShapeMerger::maximalDistance() const
{
	return maximalDistance_;
}

inline size_t ShapeMerger::observations() const
{
	return observations_;
}

inline const ShapeMerger::LShapes& ShapeMerger::lShapes() const
{
	return lShapes_;
}

inline const ShapeMerger::TShapes& ShapeMerger::tShapes() const
{
	return tShapes_;
}

inline const ShapeMerger::XShapes& ShapeMerger::xShapes() const
{
	return xShapes_;
}

} // namespace Keyboard

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_KEYBOARD_SHAPE_MERGER_H
