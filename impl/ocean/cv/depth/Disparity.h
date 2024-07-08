/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DEPTH_DISPARITY_H
#define META_OCEAN_CV_DEPTH_DISPARITY_H

#include "ocean/cv/depth/Depth.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

namespace Depth
{

/**
 * This class implements disparity functions.
 * @ingroup cvdepth
 */
class OCEAN_CV_DEPTH_EXPORT Disparity
{
	public:

		/**
		 * Fills holes in a disparity map.
		 * @param disparityMap The disparity map to be filled
		 * @return True, if succeeded
		 */
		static bool fillHolesDisparityMap(Frame& disparityMap);

		/**
		 * Merges a given set of disparity maps into a single disparity map.
		 * The merged disparity map will hold NaN values whenever input is invalid or the specified threshold don't hold.
		 * @param disparityMaps The disparity maps to merge, at least two
		 * @param inlierThreshold The maximal distance to median value to be considered inlier, with range [0, infinity)
		 * @param minInliers The minimal number of inliers required, with range [1, infinity)
		 * @param mergedDisparityMap The resulting disparity map
		 * @return True, if succeeded
		 */
		static bool mergeDisparityMaps(const Frames& disparityMaps, const double inlierThreshold, const size_t minInliers, Frame& mergedDisparityMap);

	protected:

		/**
		 * Fills holes in a disparity map.
		 * @param disparityMap The disparity map to be filled
		 * @return True, if succeeded
		 * @tparam T The data type of the disparity elements, e.g., 'float' or 'double'
		 */
		template <typename T>
		static bool fillHolesDisparityMap(Frame& disparityMap);

		/**
		 * Merges a given set of disparity maps into a single disparity map.
		 * @param disparityMaps The disparity maps to merge, at least two
		 * @param inlierThreshold The maximal distance to median value to be considered inlier, with range [0, infinity)
		 * @param minInliers The minimal number of inliers required, with range [1, infinity)
		 * @param mergedDisparityMap The resulting disparity map
		 * @return True, if succeeded
		 * @tparam T The data type of the disparity elements, e.g., 'float' or 'double'
		 */
		template <typename T>
		static bool mergeDisparityMaps(const Frames& disparityMaps, const double inlierThreshold, const size_t minInliers, Frame& mergedDisparityMap);

		/**
		 * Returns whether a given value is valid number (whether it is not NaN and finite).
		 * @param value The value to check
		 * @return True, if so
		 * @tparam T The data type of the value to check
		 */
		template <typename T>
		static inline bool isValid(const T& value);
};

template <typename T>
inline bool Disparity::isValid(const T& value)
{
	return !NumericT<T>::isNan(value) && !NumericT<T>::isInf(value);
}

}

}

}

#endif // META_OCEAN_CV_DEPTH_DISPARITY_H
