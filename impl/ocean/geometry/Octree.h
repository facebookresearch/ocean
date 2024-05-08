/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_OCTREE_H
#define META_OCEAN_GEOMETRY_OCTREE_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/BoundingBox.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/// Forward declaration.
template <typename T>
class OctreeT;

/**
 * Definition of an Octree using Scalar as data type.
 * @see OctreeT
 * @ingroup geometry
 */
typedef OctreeT<Scalar> Octree;

/**
 * Definition of an Octree using double as data type.
 * @see OctreeT
 * @ingroup geometry
 */
typedef OctreeT<double> OctreeD;

/**
 * Definition of an Octree using float as data type.
 * @see OctreeT
 * @ingroup geometry
 */
typedef OctreeT<float> OctreeF;

/**
 * This class implements an Octree allowing to manage 3D points.
 * Each node in the Octree has exactly eight child nodes (unless it is a leaf node).
 * @ingroup geometry
 */
template <typename T>
class OctreeT
{
	public:

		/// The data type of this octree.
		typedef T Type;

		/**
		 * This class stores construction parameters for an octree.
		 */
		class Parameters
		{
			public:

				/**
				 * Default constructor.
				 */
				Parameters() = default;

				/**
				 * Creates a new parameter object.
				 * @param maximalPointsPerLeaf The maximal number of points each leaf node can have, with range [1, infinity)
				 * @param useTightBoundingBoxes True, to use tight bounding boxes for each individual node (only covering the actual points); False, to use bisected bounding boxes of the bounding box of the previous node
				 */
				inline Parameters(const unsigned int maximalPointsPerLeaf, const bool useTightBoundingBoxes);

				/**
				 * Returns whether this object holds valid parameters.
				 * @return True, if so
				 */
				inline bool isValid() const;

			public:

				/// The maximal number of points each leaf node can have.
				unsigned int maximalPointsPerLeaf_ = 40u;

				/// True, to use tight bounding boxes for each individual node (only covering the actual points); False, to use bisected bounding boxes of the bounding box of the previous node
				bool useTightBoundingBoxes_ = true;
		};

		/**
		 * Definition of a class which holds reusable data for internal use.
		 * This object can avoid reallocating memory when calling a matching function several times in a row.<br>
		 * Simply define this object outside of the loop and provide the object as parameter, e.g.,
		 * @code
		 * std::vector<const Indices32*> leafs;
		 * ReusableData reusableData;
		 * for (const Vector3& point : points)
		 * {
		 *     leafs.clear();
		 *     octree.closestLeafs(point, maximalDistance, leafs, reusableData);
		 *     ...
		 * }
		 * @endcode
		 */
		class ReusableData
		{
			friend class OctreeT<T>;

			public:

				/**
				 * Creates a new object.
				 */
				ReusableData() = default;

			protected:

				/// The internal reusable data.
				mutable std::vector<const OctreeT<T>*> internalData_;
		};

	public:

		/**
		 * Default constructor creating an empty tree.
		 */
		OctreeT() = default;

		/**
		 * Disabled copy constructor.
		 * @param octree The octree which would be copied
		 */
		OctreeT(const OctreeT& octree) = delete;

		/**
		 * Move constructor.
		 * @param octree The octree to be moved
		 */
		OctreeT(OctreeT&& octree);

		/**
		 * Creates a new Octree for a given set of 3D points.
		 * The given points must not change afterwards, the points must exist as long as the tree exists.
		 * @param treePoints The points for which the tree will be created, can be 'nullptr' if 'numberTreePoints == 0'
		 * @param numberTreePoints The number given tree points, with range [0, infinity)
		 * @param parameters The parameters to used to construct the tree, must be valid
		 */
		OctreeT(const VectorT3<T>* treePoints, const size_t numberTreePoints, const Parameters& parameters = Parameters());

		/**
		 * Destructs this tree node.
		 */
		~OctreeT();

		/**
		 * Returns the bounding box containing all points of this node (of all points in all child leaf nodes)
		 * @return The tree node's bounding box
		 */
		inline const BoundingBox& boundingBox() const;

		/**
		 * Returns the indices of the tree points which belong to this leaf node.
		 * @return The leaf node's point indices, empty if this node is not a leaf node
		 */
		inline const Indices32& pointIndices() const;

		/**
		 * Returns the eight child nodes of this tree node
		 * @return The node's eight child node, nullptr if this node is a leaf node
		 */
		inline const OctreeT<T>* const * childNodes() const;

		/**
		 * Returns the closest leaf nodes for a given query point.
		 * @param queryPoint The query point for which the closest leaf nodes will be returned
		 * @param maximalDistance The maximal distance between the query point and any potential point in a leaf node, with range [0, infinity)
		 * @param leafs The resulting leaf nodes, mainly the point indices of the tree points which stored in the closest leaf nodes
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 */
		void closestLeafs(const VectorT3<T>& queryPoint, const T maximalDistance, std::vector<const Indices32*>& leafs, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Returns the intersecting leaf nodes for a given query ray.
		 * @param queryRay The query ray for which the intersecting leaf nodes will be returned, the search treats the ray as an infinite ray in space, must be valid
		 * @param leafs The resulting leaf nodes, mainly the point indices of the tree points which stored in the intersecting leaf nodes
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 */
		void intersectingLeafs(const LineT3<T>& queryRay, std::vector<const Indices32*>& leafs, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Returns the intersecting leaf nodes for a given approximated query cone expressed as a ray with a cone apex angle.
		 * This function applies an approximation to determine the distance between the cone's apex and leaf nodes.
		 * @param queryRay The query ray defining the apex and axis of the cone for which the intersecting leaf nodes will be returned, the search treats the ray as an infinite ray in space, must be valid
		 * @param tanHalfAngle The tangent of the cone's half apex angle, e.g., Numeric::tan(Numeric::deg2rad(1)), with range [0, 1)
		 * @param leafs The resulting leaf nodes, mainly the point indices of the tree points which stored in the intersecting leaf nodes
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 */
		void intersectingLeafs(const LineT3<T>& queryRay, const Scalar tanHalfAngle, std::vector<const Indices32*>& leafs, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Returns the closest tree points for a given query point.
		 * @param treePoints The tree points from which the closest points will be determined (must be the same points for which the tree has been created), must be valid
		 * @param queryPoint The query point for which the closest points will be returned
		 * @param maximalDistance The maximal distance between the query point and any potential tree point in a leaf node, with range [0, infinity)
		 * @param pointIndices The resulting indices of the tree points which have a maximal distance of 'maximalDistance' to the query point
		 * @param points Optional resulting tree points, one for each resulting index, will be {treePoints[pointIndices[0]], treePoints[pointIndices[1]], ..., treePoints[pointIndices[pointIndices.size() - 1]]}; nullptr if not of interest
		 * @param reusableData An reusable object to speedup the search, should be located outside of the function call if several function calls are done after each other
		 */
		void closestPoints(const VectorT3<T>* treePoints, const VectorT3<T>& queryPoint, const T maximalDistance, Indices32& pointIndices, VectorsT3<T>* points = nullptr, const ReusableData& reusableData = ReusableData()) const;

		/**
		 * Returns whether this node is valid (if this node has a valid bounding box)
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Move operator.
		 * @param octree The octree to be moved
		 * @return Reference to this octree
		 */
		OctreeT& operator=(OctreeT&& octree);

		/**
		 * Disabled copy constructor.
		 * @param octree The octree which would be copied
		 * @return Reference to this octree
		 */
		OctreeT& operator=(const OctreeT& octree) = delete;

	protected:

		/**
		 * Creates a new octree node.
		 * @param parameters The parameters to be used, must be valid
		 * @param treePoints The points for which the tree will be created, must be valid
		 * @param reusablePointIndicesInput The indices of the points for which the new node will be created, must be valid
		 * @param reusablePointIndicesOutput Memory block of indices with same size as 'reusableIndicesInput' which can be re-used internally, must be valid
		 * @param numberPointIndices The number of given indices in 'reusablePointIndicesInput', with range [0, infinity)
		 * @param boundingBox The bounding box of the new child node; will be ignored if `parameters.useTightBoundingBoxes_ == true`
		 */
		OctreeT(const Parameters& parameters, const VectorT3<T>* treePoints, Index32* reusablePointIndicesInput, Index32* reusablePointIndicesOutput, const size_t numberPointIndices, const BoundingBox& boundingBox);

	protected:

		/// The bounding box of this tree node.
		BoundingBox boundingBox_;

		/// The indicies of the tree points which belong to this leaf node, empty if this node is not a leaf node.
		Indices32 pointIndices_;

		/// The eight child nodes.
		OctreeT<T>* childNodes_[8] = {nullptr};
};

template <typename T>
inline OctreeT<T>::Parameters::Parameters(const unsigned int maximalPointsPerLeaf, const bool useTightBoundingBoxes) :
	maximalPointsPerLeaf_(maximalPointsPerLeaf),
	useTightBoundingBoxes_(useTightBoundingBoxes)
{
	ocean_assert(maximalPointsPerLeaf_ >= 1u);
}

template <typename T>
inline bool OctreeT<T>::Parameters::isValid() const
{
	return maximalPointsPerLeaf_ >= 1u;
}

template <typename T>
OctreeT<T>::OctreeT(OctreeT&& octree)
{
	*this = std::move(octree);
}

template <typename T>
OctreeT<T>::OctreeT(const VectorT3<T>* treePoints, const size_t numberTreePoints, const Parameters& parameters)
{
	ocean_assert(parameters.isValid());

	if (numberTreePoints == 0)
	{
		return;
	}

	ocean_assert(treePoints != nullptr);

	Indices32 reusablePointIndicesInput(createIndices<Index32>(numberTreePoints, 0u));
	Indices32 reusablePointIndicesOutput(reusablePointIndicesInput.size());

	BoundingBox boundingBox;

	if (parameters.useTightBoundingBoxes_ == false)
	{
		for (size_t n = 0; n < numberTreePoints; ++n)
		{
			boundingBox += Vector3(treePoints[n]);
		}
	}

	*this = Octree(parameters, treePoints, reusablePointIndicesInput.data(), reusablePointIndicesOutput.data(), numberTreePoints, boundingBox);
}

template <typename T>
OctreeT<T>::OctreeT(const Parameters& parameters, const VectorT3<T>* treePoints, Index32* reusablePointIndicesInput, Index32* reusablePointIndicesOutput, const size_t numberPointIndices, const BoundingBox& boundingBox)
{
	ocean_assert(parameters.isValid());
	ocean_assert(treePoints != nullptr);

	if (numberPointIndices == 0)
	{
		return;
	}

	ocean_assert(reusablePointIndicesInput != nullptr && reusablePointIndicesOutput != nullptr);

	if (parameters.useTightBoundingBoxes_ == false)
	{
		ocean_assert(boundingBox.isValid());
		boundingBox_ = boundingBox;

#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < numberPointIndices; ++n)
		{
			const Index32& index = reusablePointIndicesInput[n];
			ocean_assert(boundingBox_.isInside(Vector3(treePoints[index])));
		}
#endif

		if (numberPointIndices <= parameters.maximalPointsPerLeaf_)
		{
			// we have a leaf node

			pointIndices_.reserve(numberPointIndices);

			for (size_t n = 0; n < numberPointIndices; ++n)
			{
				const Index32& index = reusablePointIndicesInput[n];
				pointIndices_.emplace_back(index);
			}

			return;
		}
	}
	else
	{
		if (numberPointIndices <= parameters.maximalPointsPerLeaf_)
		{
			// we have a leaf node

			pointIndices_.reserve(numberPointIndices);

			for (size_t n = 0; n < numberPointIndices; ++n)
			{
				const Index32& index = reusablePointIndicesInput[n];

				boundingBox_ += Vector3(treePoints[index]);
				pointIndices_.emplace_back(index);
			}

			ocean_assert(boundingBox_.isValid());

			return;
		}

		// we need to separate the points

		for (size_t n = 0; n < numberPointIndices; ++n)
		{
			const Index32& index = reusablePointIndicesInput[n];
			boundingBox_ += Vector3(treePoints[index]);
		}
	}

	ocean_assert(boundingBox_.isValid());

	const VectorT3<T> center(boundingBox_.center());

	// first, we count how may points fall in which node

	size_t lowLowLow = 0;
	size_t lowLowHigh = 0;
	size_t lowHighLow = 0;
	size_t lowHighHigh = 0;

	size_t highLowLow = 0;
	size_t highLowHigh = 0;
	size_t highHighLow = 0;
	size_t highHighHigh = 0;

	for (size_t n = 0; n < numberPointIndices; ++n)
	{
		const Index32& index = reusablePointIndicesInput[n];
		const VectorT3<T>& point = treePoints[index];

		if (point.x() < center.x())
		{
			if (point.y() < center.y())
			{
				if (point.z() < center.z())
				{
					++lowLowLow;
				}
				else
				{
					++lowLowHigh;
				}
			}
			else
			{
				if (point.z() < center.z())
				{
					++lowHighLow;
				}
				else
				{
					++lowHighHigh;
				}
			}
		}
		else
		{
			if (point.y() < center.y())
			{
				if (point.z() < center.z())
				{
					++highLowLow;
				}
				else
				{
					++highLowHigh;
				}
			}
			else
			{
				if (point.z() < center.z())
				{
					++highHighLow;
				}
				else
				{
					++highHighHigh;
				}
			}
		}
	}

	// now, we shuffle the indices of the individual points

	Index32* lowLowLowPointer = reusablePointIndicesOutput;
	Index32* lowLowHighPointer = lowLowLowPointer + lowLowLow;
	Index32* lowHighLowPointer = lowLowHighPointer + lowLowHigh;
	Index32* lowHighHighPointer = lowHighLowPointer + lowHighLow;

	Index32* highLowLowPointer = lowHighHighPointer + lowHighHigh;
	Index32* highLowHighPointer = highLowLowPointer + highLowLow;
	Index32* highHighLowPointer = highLowHighPointer + highLowHigh;
	Index32* highHighHighPointer = highHighLowPointer + highHighLow;

	ocean_assert(highHighHighPointer + highHighHigh == reusablePointIndicesOutput + numberPointIndices);

	for (size_t n = 0; n < numberPointIndices; ++n)
	{
		const Index32& index = reusablePointIndicesInput[n];
		const VectorT3<T>& point = treePoints[index];

		if (point.x() < center.x())
		{
			if (point.y() < center.y())
			{
				if (point.z() < center.z())
				{
					*lowLowLowPointer++ = index;
				}
				else
				{
					*lowLowHighPointer++ = index;
				}
			}
			else
			{
				if (point.z() < center.z())
				{
					*lowHighLowPointer++ = index;
				}
				else
				{
					*lowHighHighPointer++ = index;
				}
			}
		}
		else
		{
			if (point.y() < center.y())
			{
				if (point.z() < center.z())
				{
					*highLowLowPointer++ = index;
				}
				else
				{
					*highLowHighPointer++ = index;
				}
			}
			else
			{
				if (point.z() < center.z())
				{
					*highHighLowPointer++ = index;
				}
				else
				{
					*highHighHighPointer++ = index;
				}
			}
		}
	}

	ocean_assert(lowLowLowPointer == reusablePointIndicesOutput + lowLowLow);
	ocean_assert(lowLowHighPointer == lowLowLowPointer + lowLowHigh);
	ocean_assert(lowHighLowPointer == lowLowHighPointer + lowHighLow);
	ocean_assert(lowHighHighPointer == lowHighLowPointer + lowHighHigh);

	ocean_assert(highLowLowPointer == lowHighHighPointer + highLowLow);
	ocean_assert(highLowHighPointer == highLowLowPointer + highLowHigh);
	ocean_assert(highHighLowPointer == highLowHighPointer + highHighLow);
	ocean_assert(highHighHighPointer == highHighLowPointer + highHighHigh);

	if (parameters.useTightBoundingBoxes_)
	{
		childNodes_[0] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput, reusablePointIndicesInput, lowLowLow, BoundingBox()); // with swapped reusableIndicesOutput and reusableIndicesInput
		childNodes_[1] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowLowLow, reusablePointIndicesInput += lowLowLow, lowLowHigh, BoundingBox());
		childNodes_[2] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowLowHigh, reusablePointIndicesInput += lowLowHigh, lowHighLow, BoundingBox());
		childNodes_[3] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowHighLow, reusablePointIndicesInput += lowHighLow, lowHighHigh, BoundingBox());

		childNodes_[4] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowHighHigh, reusablePointIndicesInput += lowHighHigh, highLowLow, BoundingBox());
		childNodes_[5] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highLowLow, reusablePointIndicesInput += highLowLow, highLowHigh, BoundingBox());
		childNodes_[6] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highLowHigh, reusablePointIndicesInput += highLowHigh, highHighLow, BoundingBox());
		childNodes_[7] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highHighLow, reusablePointIndicesInput += highHighLow, highHighHigh, BoundingBox());
	}
	else
	{
		const BoundingBox boxLowLowLow(Vector3(boundingBox_.lower().x(), boundingBox_.lower().y(), boundingBox_.lower().z()), Vector3(center.x(), center.y(), center.z()));
		const BoundingBox boxLowLowHigh(Vector3(boundingBox_.lower().x(), boundingBox_.lower().y(), center.z()), Vector3(center.x(), center.y(), boundingBox_.higher().z()));
		const BoundingBox boxLowHighLow(Vector3(boundingBox_.lower().x(), center.y(), boundingBox_.lower().z()), Vector3(center.x(), boundingBox_.higher().y(), center.z()));
		const BoundingBox boxLowHighHigh(Vector3(boundingBox_.lower().x(), center.y(), center.z()), Vector3(center.x(), boundingBox_.higher().y(), boundingBox_.higher().z()));

		const BoundingBox boxHighLowLow(Vector3(center.x(), boundingBox_.lower().y(), boundingBox_.lower().z()), Vector3(boundingBox_.higher().x(), center.y(), center.z()));
		const BoundingBox boxHighLowHigh(Vector3(center.x(), boundingBox_.lower().y(), center.z()), Vector3(boundingBox_.higher().x(), center.y(), boundingBox_.higher().z()));
		const BoundingBox boxHighHighLow(Vector3(center.x(), center.y(), boundingBox_.lower().z()), Vector3(boundingBox_.higher().x(), boundingBox_.higher().y(), center.z()));
		const BoundingBox boxHighHighHigh(Vector3(center.x(), center.y(), center.z()), Vector3(boundingBox_.higher().x(), boundingBox_.higher().y(), boundingBox_.higher().z()));

		childNodes_[0] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput, reusablePointIndicesInput, lowLowLow, boxLowLowLow); // with swapped reusableIndicesOutput and reusableIndicesInput
		childNodes_[1] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowLowLow, reusablePointIndicesInput += lowLowLow, lowLowHigh, boxLowLowHigh);
		childNodes_[2] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowLowHigh, reusablePointIndicesInput += lowLowHigh, lowHighLow, boxLowHighLow);
		childNodes_[3] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowHighLow, reusablePointIndicesInput += lowHighLow, lowHighHigh, boxLowHighHigh);

		childNodes_[4] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += lowHighHigh, reusablePointIndicesInput += lowHighHigh, highLowLow, boxHighLowLow);
		childNodes_[5] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highLowLow, reusablePointIndicesInput += highLowLow, highLowHigh, boxHighLowHigh);
		childNodes_[6] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highLowHigh, reusablePointIndicesInput += highLowHigh, highHighLow, boxHighHighLow);
		childNodes_[7] = new OctreeT<T>(parameters, treePoints, reusablePointIndicesOutput += highHighLow, reusablePointIndicesInput += highHighLow, highHighHigh, boxHighHighHigh);
	}
}

template <typename T>
OctreeT<T>::~OctreeT()
{
	for (unsigned int n = 0u; n < 8u; ++n)
	{
		delete childNodes_[n];
	}
}

template <typename T>
inline const BoundingBox& OctreeT<T>::boundingBox() const
{
	return boundingBox_;
}

template <typename T>
inline const Indices32& OctreeT<T>::pointIndices() const
{
	return pointIndices_;
}

template <typename T>
inline const OctreeT<T>* const * OctreeT<T>::childNodes() const
{
	if (childNodes_[0] == nullptr)
	{
		return nullptr;
	}

	return childNodes_;
}

template <typename T>
void OctreeT<T>::closestLeafs(const VectorT3<T>& queryPoint, const T maximalDistance, std::vector<const Indices32*>& leafs, const ReusableData& reusableData) const
{
	if (!isValid())
	{
		return;
	}

	const Vector3 scalarPoint(queryPoint);

	const T maximalSqrDistance = Scalar(maximalDistance * maximalDistance);

	if (!boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)))
	{
		return;
	}

	std::vector<const OctreeT<T>*>& nodes = reusableData.internalData_;
	nodes.emplace_back(this);

	while (!nodes.empty())
	{
		const OctreeT<T>* node = nodes.back();
		nodes.pop_back();

		ocean_assert(node != nullptr);
		ocean_assert(node->boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)));

		if (node->childNodes_[0] != nullptr)
		{
			for (unsigned int n = 0u; n < 8u; ++n)
			{
				ocean_assert(node->childNodes_[n] != nullptr);
				const OctreeT<T>& childNode = *node->childNodes_[n];

				if (childNode.isValid() && childNode.boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)))
				{
					nodes.emplace_back(&childNode);
				}
			}
		}
		else if (!node->pointIndices_.empty())
		{
			leafs.emplace_back(&node->pointIndices_);
		}
	}
}

template <typename T>
void OctreeT<T>::intersectingLeafs(const LineT3<T>& queryRay, std::vector<const Indices32*>& leafs, const ReusableData& reusableData) const
{
	ocean_assert(queryRay.isValid());
	ocean_assert(leafs.empty());

	if (!isValid())
	{
		return;
	}

	const Line3 scalarRay(queryRay);

	if (!boundingBox_.hasIntersection(scalarRay))
	{
		return;
	}

	std::vector<const OctreeT<T>*>& nodes = reusableData.internalData_;
	nodes.emplace_back(this);

	while (!nodes.empty())
	{
		const OctreeT<T>* node = nodes.back();
		nodes.pop_back();

		ocean_assert(node != nullptr);
		ocean_assert(node->boundingBox_.hasIntersection(scalarRay));

		if (node->childNodes_[0] != nullptr)
		{
			for (unsigned int n = 0u; n < 8u; ++n)
			{
				ocean_assert(node->childNodes_[n] != nullptr);
				const OctreeT<T>& childNode = *node->childNodes_[n];

				if (childNode.isValid() && childNode.boundingBox_.hasIntersection(scalarRay))
				{
					nodes.emplace_back(&childNode);
				}
			}
		}
		else if (!node->pointIndices_.empty())
		{
			leafs.emplace_back(&node->pointIndices_);
		}
	}
}

template <typename T>
void OctreeT<T>::intersectingLeafs(const LineT3<T>& queryRay, const Scalar tanHalfAngle, std::vector<const Indices32*>& leafs, const ReusableData& reusableData) const
{
	ocean_assert(queryRay.isValid());
	ocean_assert(tanHalfAngle >= 0 && tanHalfAngle < 1);
	ocean_assert(leafs.empty());

	if (!isValid())
	{
		return;
	}

	const Line3 scalarRay(queryRay);

	const Scalar& epsPerDistance = tanHalfAngle;

	if (!boundingBox_.hasIntersection(scalarRay, epsPerDistance))
	{
		return;
	}

	std::vector<const OctreeT<T>*>& nodes = reusableData.internalData_;
	nodes.emplace_back(this);

	while (!nodes.empty())
	{
		const OctreeT<T>* node = nodes.back();
		nodes.pop_back();

		ocean_assert(node != nullptr);
		ocean_assert(node->boundingBox_.hasIntersection(scalarRay, epsPerDistance));

		if (node->childNodes_[0] != nullptr)
		{
			for (unsigned int n = 0u; n < 8u; ++n)
			{
				ocean_assert(node->childNodes_[n] != nullptr);
				const OctreeT<T>& childNode = *node->childNodes_[n];

				if (childNode.isValid() && childNode.boundingBox_.hasIntersection(scalarRay, epsPerDistance))
				{
					nodes.emplace_back(&childNode);
				}
			}
		}
		else if (!node->pointIndices_.empty())
		{
			leafs.emplace_back(&node->pointIndices_);
		}
	}
}

template <typename T>
void OctreeT<T>::closestPoints(const VectorT3<T>* treePoints, const VectorT3<T>& queryPoint, const T maximalDistance, Indices32& pointIndices, VectorsT3<T>* points, const ReusableData& reusableData) const
{
	ocean_assert(treePoints != nullptr);

	ocean_assert(pointIndices.empty());
	ocean_assert(points == nullptr || points->empty());

	if (!isValid())
	{
		return;
	}

	const Vector3 scalarPoint(queryPoint);

	const T maximalSqrDistance = Scalar(maximalDistance * maximalDistance);

	if (!boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)))
	{
		return;
	}

	std::vector<const OctreeT<T>*>& nodes = reusableData.internalData_;
	nodes.emplace_back(this);

	while (!nodes.empty())
	{
		const OctreeT<T>* node = nodes.back();
		nodes.pop_back();

		ocean_assert(node != nullptr);
		ocean_assert(node->boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)));

		if (node->childNodes_[0] != nullptr)
		{
			for (unsigned int n = 0u; n < 8u; ++n)
			{
				const OctreeT<T>& childNode = *node->childNodes_[n];

				if (childNode.isValid() && childNode.boundingBox_.isInside(scalarPoint, Scalar(maximalDistance)))
				{
					nodes.emplace_back(&childNode);
				}
			}
		}
		else
		{
			for (const Index32& pointIndex : node->pointIndices_)
			{
				const VectorT3<T>& treePoint = treePoints[pointIndex];

				if (points != nullptr)
				{
					if (treePoint.sqrDistance(queryPoint) <= maximalSqrDistance)
					{
						pointIndices.emplace_back(pointIndex);
						points->emplace_back(treePoints[pointIndex]);
					}
				}
				else
				{
					if (treePoint.sqrDistance(queryPoint) <= maximalSqrDistance)
					{
						pointIndices.emplace_back(pointIndex);
					}
				}
			}
		}
	}
}

template <typename T>
inline bool OctreeT<T>::isValid() const
{
	return boundingBox_.isValid();
}

template <typename T>
OctreeT<T>& OctreeT<T>::operator=(OctreeT<T>&& octree)
{
	if (this != &octree)
	{
		boundingBox_ = octree.boundingBox_;
		octree.boundingBox_ = BoundingBox();

		pointIndices_ = std::move(octree.pointIndices_);

		for (unsigned int n = 0u; n < 8u; ++n)
		{
			childNodes_[n] = octree.childNodes_[n];
			octree.childNodes_[n] = nullptr;
		}
	}

	return *this;
}

}

}

#endif // META_OCEAN_GEOMETRY_GRID_H
