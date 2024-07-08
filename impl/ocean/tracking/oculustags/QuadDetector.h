/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_QUADDETECTOR_H
#define META_OCEAN_TRACKING_OCULUSTAGS_QUADDETECTOR_H

#include "ocean/tracking/oculustags/OculusTags.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/math/Vector2.h"

#include <array>
#include <vector>

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * This class implements a detector for quadrilaterals
 * @ingroup trackingoculustags
 */
class OCEAN_TRACKING_OCULUSTAGS_EXPORT QuadDetector
{
	public:

		/// Definition of a quadrilateral
		typedef std::array<Vector2, 4> Quad;


		/// A vector of quadrilaterals
		typedef std::vector<Quad> Quads;

	public:

		/**
		 * Detects boundary patterns (possible candidates) and filters them
		 * @param yFrame The image in which boundary patterns will be searched, must be valid
		 * @param frameBorder Defines a perimeter inside the image along the image border in which nothing will be processed (in pixels), range: [0, min(yFrame.width(), yFrame.height())/2)
		 * @return A vector of detected boundary patterns
		 */
		static Quads detectQuads(const Frame& yFrame, const uint32_t frameBorder = 0u);

	protected:

		/**
		 * Determines quads from a set of L-shapes
		 * This function tries to find 4-tuples of L-shapes that form a quadrilateral (without self-intersection)
		 * @param yFrame The image in which boundary patterns will be searched, must be valid
		 * @param lShapes A list of L-shapes, must be valid
		 * @param finiteLines The list of finite lines from the list of L-shapes was constructed, must be valid
		 * @param angleThreshold The maximum deviation from a right angle when comparing adjacent and connected L-shapes, range: [0, deg2rad(90)) (in radian)
		 * @param frameBorder Defines a perimeter inside the image along the image border in which nothing will be processed (in pixels), range: [0, min(yFrame.width(), yFrame.height())/2)
		 * @return A vector of detected boundary patterns
		 */
		static Quads extractQuads(const Frame& yFrame, const CV::Detector::ShapeDetector::LShapes& lShapes, const FiniteLines2& finiteLines, const Scalar angleThreshold, const uint32_t frameBorder);

		/**
		 * Applies subpixel refinement to the points of a boundary pattern
		 * @param yFrame The image in which the boundary pattern was detected, must be valid
		 * @param quad The boundary pattern of which the points will be refined, must be valid
		 * @return True if the refinement was successful, otherwise false
		 */
		static bool refineQuad(const Frame& yFrame, Quad& quad);
};

}  // namespace OculusTags

}  // namespace Tracking

}  // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_QUADDETECTOR_H
