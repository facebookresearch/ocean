// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_UTILITIES_H
#define META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_UTILITIES_H

#include "ocean/tracking/mapbuilding/importers/Importers.h"

#include "arvr/libraries/thrift_if/common/gen-cpp2/numerics_types.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

namespace Importers
{

/**
 * This class implements utility functions
 * @ingroup trackingmapbuildingimporters
 */
class Utilities
{
	public:

		/**
		 * Converts a Thrift CompactVector3 object to an Ocean Vector3 object.
		 * @param thriftVector3 The Thrift object to be converted
		 * @return The resulting Ocean object
		 * @tparam TSource The data type of the source matrix elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline VectorT3<TTarget> toVector3(const arvr::thrift_if::common::sophus_wrapper<Eigen::Matrix<TSource, 3, 1>>& thriftVector3);

		/**
		 * Converts a Thrift CompactSE3 object to an Ocean HomogenousMatrix4 object.
		 * @param thriftCompactSE3 The Thrift object to be converted
		 * @return The resulting Ocean object
		 * @tparam TSource The data type of the source matrix elements, e.g., 'float', or 'double'
		 * @tparam TTarget The data type of the target matrix elements, e.g., 'float', or 'double'
		 */
		template <typename TSource, typename TTarget>
		static inline HomogenousMatrixT4<TTarget> toHomogenousMatrix4(const arvr::thrift_if::common::sophus_wrapper<Sophus::SE3<TSource>>& thriftCompactSE3);
};

template <typename TSource, typename TTarget>
inline VectorT3<TTarget> Utilities::toVector3(const arvr::thrift_if::common::sophus_wrapper<Eigen::Matrix<TSource, 3, 1>>& thriftVector3)
{
	return VectorT3<TTarget>(TTarget(thriftVector3[0]), TTarget(thriftVector3[1]), TTarget(thriftVector3[2]));
}

template <typename TSource, typename TTarget>
inline HomogenousMatrixT4<TTarget> Utilities::toHomogenousMatrix4(const arvr::thrift_if::common::sophus_wrapper<Sophus::SE3<TSource>>& thriftCompactSE3)
{
	ocean_assert(thriftCompactSE3.size() == 7);

	const QuaternionT<TSource> quaternion(thriftCompactSE3[3], thriftCompactSE3[0], thriftCompactSE3[1], thriftCompactSE3[2]);
	ocean_assert(quaternion.isValid());

	const VectorT3<TSource> translation(thriftCompactSE3[4], thriftCompactSE3[5], thriftCompactSE3[6]);

	return HomogenousMatrixT4<TTarget>(VectorT3<TTarget>(translation), QuaternionT<TTarget>(quaternion));
}

}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_IMPORTERS_UTILITIES_H
