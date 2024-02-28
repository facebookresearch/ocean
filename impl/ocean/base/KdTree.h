// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_BASE_KD_TREE_H
#define META_OCEAN_BASE_KD_TREE_H

#include "ocean/base/Base.h"
#include "ocean/base/DataType.h"
#include "ocean/base/Median.h"
#include "ocean/base/Utilities.h"

#include <limits>

namespace Ocean
{

/**
 * This class implements a k-d tree.
 * In general, k-d trees should be applied for problems with small dimensions only as the performance benefit decreases with increasing dimension significantly.<br>
 * That means for the number of nodes (n) and the dimension (k) the following should hold: n >> 2^k.
 * @tparam T The data type of one element for all dimensions.
 * @ingroup base
 */
template <typename T>
class KdTree
{
	protected:

		/**
		 * This class defines a node of the k-d tree.
		 */
		class Node
		{
			public:

				/**
				 * Creates an empty node.
				 */
				inline Node();

				/**
				 * Destructs a node.
				 */
				inline ~Node();

				/**
				 * Creates a new node with given value.
				 * @param value Node value
				 */
				explicit inline Node(const T* value);

				/**
				 * Returns the left child of this node.
				 * @return Left child
				 */
				inline Node* left();

				/**
				 * Returns the right child of this node.
				 * @return Right child
				 */
				inline Node* right();

				/**
				 * Returns value of this node.
				 * @return Node value
				 */
				inline const T* value();

				/**
				 * Sets the left child of this node.
				 * @param left Left child
				 */
				inline void setLeft(Node* left);

				/**
				 * Sets the right child of this node.
				 * @param right Right child
				 */
				inline void setRight(Node* right);

				/**
				 * Returns whether this node holds a valid value.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// Node value.
				const T* nodeValue;

				/// Left node.
				Node* nodeLeft;

				/// Right node.
				Node* nodeRight;
		};

		/**
		 * Definition of a vector holding single elements.
		 */
		typedef std::vector<T> Elements;

		/**
		 * Definition of a vector holding single pointers.
		 */
		typedef std::vector<const T*> Pointers;

	public:

		/**
		 * Creates a new k-d tree.
		 * @param dimension Number of dimensions the tree will have, with range [1, infinity)
		 */
		explicit inline KdTree(const unsigned int dimension);

		/**
		 * Destructs a k-d tree.
		 */
		inline ~KdTree();

		/**
		 * Inserts a set of values to this empty tree.
		 * Beware: Adding elements to an already existing tree with nodes is not supported.
		 * @param values Values to be added
		 * @param number Number of elements to be added, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool insert(const T** values, const size_t number);

		/**
		 * Applies a nearest neighbor search for a given value.
		 * @param value Value to be searched
		 * @param distance Resulting minimal distance
		 * @return Resulting nearest neighbor
		 */
		const T* nearestNeighbor(const T* value, typename SquareValueTyper<T>::Type& distance) const;

		/**
		 * Applies a radius search for neighbors of a given value.
		 * Beware: Function offers performance boost over brute force search only if radius is so small that relatively few values are returned.
		 * @param value Value to be searched
		 * @param radius Neighboorhood radius
		 * @param values Found values within radius distance from a given value
		 * @param maxValues Limit number of returned values
		 * @return Number of returned values
		 */
		size_t radiusSearch(const T* value, const typename SquareValueTyper<T>::Type radius, const T** values, const size_t maxValues) const;

		/**
		 * Returns the dimension of the tree's values.
		 * @return The tree's dimension
		 */
		inline unsigned int dimension() const;

		/**
		 * Returns the number of tree nodes.
		 * @return Tree size
		 */
		inline size_t size() const;

	protected:

		/**
		 * Inserts a set of values to a given parent as left children.
		 * @param parent Parent node
		 * @param values Values to be added
		 * @param number Number of elements to be added
		 * @param depth Current depth of the new nodes
		 */
		void insertLeft(Node& parent, const T** values, const size_t number, const unsigned int depth);

		/**
		 * Inserts a set of values to a given parent as right children.
		 * @param parent Parent node
		 * @param values Values to be added
		 * @param number Number of elements to be added
		 * @param depth Current depth of the new nodes
		 */
		void insertRight(Node& parent, const T** values, const size_t number, const unsigned int depth);

		/**
		 * Applies a nearest neighbor search for a given node and value.
		 * @param node Reference node
		 * @param value Value to be searched
		 * @param nearest Resulting nearest value
		 * @param distance Resulting minimal distance
		 * @param index The index of the dimension for the recent search, with range [0, dimension())
		 */
		void nearestNeighbor(Node& node, const T* value, const T*& nearest, typename SquareValueTyper<T>::Type& distance, const unsigned int index) const;

		/**
		 * Applies a radius search for neighbors of a given value.
		 * @param node Reference node
		 * @param value Value to be searched
		 * @param radius Neighboorhood radius
		 * @param values Found values within radius distance from a given value
		 * @param maxValues Limit number of returned values
		 * @param index The index of the dimension for the recent search, with range [0, dimension())
		 * @return Number of returned values
		 */
		size_t radiusSearch(Node& node, const T* value, const typename SquareValueTyper<T>::Type radius, const T** values, const size_t maxValues, const unsigned int index) const;

		/**
		 * Returns the median for a given set of values.
		 * @param values Values to return the median for
		 * @param number Number of values
		 * @param index Dimension index
		 * @return Resulting median
		 */
		static T median(const T** values, const size_t number, const unsigned int index);

		/**
		 * Distributes a given set of values into two subset according to the median.
		 * @param values Values to distribute
		 * @param number Number of values
		 * @param index Dimension index to be used for distribution
		 * @param medianValue Resulting median value
		 * @param leftValues Resulting left values
		 * @param rightValues Resulting right values
		 */
		static void distribute(const T** values, const size_t number, const unsigned int index, const T*& medianValue, Pointers& leftValues, Pointers& rightValues);

		/**
		 * Determines the square distance between two values.
		 * @param first First value
		 * @param second Second valu
		 * @return Square distance
		 */
		inline typename SquareValueTyper<T>::Type determineSquareDistance(const T* first, const T* second) const;

	protected:

		/// Root node of this tree.
		Node* treeRoot;

		/// Number of nodes.
		size_t treeSize;

		/// Number of dimensions.
		const unsigned int treeDimension;
};

template <typename T>
inline KdTree<T>::Node::Node() :
	nodeValue(NULL),
	nodeLeft(NULL),
	nodeRight(NULL)
{
	// nothing to do here
}

template <typename T>
inline KdTree<T>::Node::Node(const T* value) :
	nodeValue(value),
	nodeLeft(NULL),
	nodeRight(NULL)
{
	// nothing to do here
}

template <typename T>
inline KdTree<T>::Node::~Node()
{
	delete nodeLeft;
	delete nodeRight;
}

template <typename T>
inline typename KdTree<T>::Node* KdTree<T>::Node::left()
{
	return nodeLeft;
}

template <typename T>
inline typename KdTree<T>::Node* KdTree<T>::Node::right()
{
	return nodeRight;
}

template <typename T>
inline const T* KdTree<T>::Node::value()
{
	return nodeValue;
}

template <typename T>
inline void KdTree<T>::Node::setLeft(Node* left)
{
	ocean_assert(!nodeLeft);
	nodeLeft = left;
}

template <typename T>
inline void KdTree<T>::Node::setRight(Node* right)
{
	ocean_assert(!nodeRight);
	nodeRight = right;
}

template <typename T>
inline KdTree<T>::Node::operator bool() const
{
	return nodeValue != NULL;
}

template <typename T>
inline KdTree<T>::KdTree(const unsigned int dimension) :
	treeRoot(NULL),
	treeSize(0),
	treeDimension(dimension)
{
	ocean_assert(dimension >= 1u);
}

template <typename T>
inline KdTree<T>::~KdTree()
{
	delete treeRoot;
}

template <typename T>
bool KdTree<T>::insert(const T** values, const size_t number)
{
	if (number == 0)
		return true;

	if (treeRoot)
		return false;

	ocean_assert(values);

	const T* median = NULL;
	Pointers left, right;

	distribute(values, number, 0u, median, left, right);

	treeRoot = new Node(median);

	if (!left.empty())
		insertLeft(*treeRoot, left.data(), left.size(), 1u);

	if (!right.empty())
		insertRight(*treeRoot, right.data(), right.size(), 1u);

	treeSize = number;

	return true;
}

template <typename T>
const T* KdTree<T>::nearestNeighbor(const T* value, typename SquareValueTyper<T>::Type& distance) const
{
	ocean_assert(value);

	distance = std::numeric_limits<T>::max();

	if (!treeRoot)
	{
		return nullptr;
	}

	const T* nearest = nullptr;

	nearestNeighbor(*treeRoot, value, nearest, distance, 0u);
	return nearest;
}

template <typename T>
size_t KdTree<T>::radiusSearch(const T* value, const typename SquareValueTyper<T>::Type radius, const T** values, const size_t maxValues) const
{
	ocean_assert(value);
	ocean_assert(values);

	if (!treeRoot || maxValues == 0)
	{
		return 0;
	}

	size_t found = radiusSearch(*treeRoot, value, radius, values, maxValues, 0u);

	ocean_assert(found <= maxValues);

	return found;
}

template <typename T>
inline unsigned int KdTree<T>::dimension() const
{
	return treeDimension;
}

template <typename T>
inline size_t KdTree<T>::size() const
{
	return treeSize;
}

template <typename T>
void KdTree<T>::insertLeft(Node& parent, const T** values, const size_t number, const unsigned int depth)
{
#ifdef OCEAN_DEBUG
	ocean_assert(parent);
	ocean_assert(values && number > 0);

	for (size_t n = 0; n < number; ++n)
		ocean_assert(values[n][(depth - 1) % treeDimension] <= parent.value()[(depth - 1) % treeDimension]);
#endif

	ocean_assert(!parent.left());

	const unsigned int index = depth % treeDimension;

	const T* median = NULL;
	Pointers left, right;

	distribute(values, number, index, median, left, right);

	parent.setLeft(new Node(median));

	if (!left.empty())
		insertLeft(*parent.left(), left.data(), left.size(), depth + 1);

	if (!right.empty())
		insertRight(*parent.left(), right.data(), right.size(), depth + 1);
}

template <typename T>
void KdTree<T>::insertRight(Node& parent, const T** values, const size_t number, const unsigned int depth)
{
#ifdef OCEAN_DEBUG
	ocean_assert(parent);
	ocean_assert(values && number > 0);

	for (size_t n = 0; n < number; ++n)
		ocean_assert(parent.value()[(depth - 1) % treeDimension] < values[n][(depth - 1) % treeDimension]);
#endif

	ocean_assert(!parent.right());

	const unsigned int index = depth % treeDimension;

	const T* median = NULL;
	Pointers left, right;

	distribute(values, number, index, median, left, right);

	parent.setRight(new Node(median));

	if (!left.empty())
		insertLeft(*parent.right(), left.data(), left.size(), depth + 1);

	if (!right.empty())
		insertRight(*parent.right(), right.data(), right.size(), depth + 1);
}

template <typename T>
void KdTree<T>::nearestNeighbor(Node& node, const T* value, const T*& nearest, typename SquareValueTyper<T>::Type& distance, const unsigned int index) const
{
	ocean_assert(node && value);
	ocean_assert(index < treeDimension);

	const typename SquareValueTyper<T>::Type localDistance = determineSquareDistance(value, node.value());
	if (localDistance < distance)
	{
		distance = localDistance;
		nearest = node.value();
	}

	const unsigned int nextIndex = (index + 1u) % treeDimension;

	// depth-first-search
	if (value[index] <= node.value()[index])
	{
		if (node.left())
			nearestNeighbor(*node.left(), value, nearest, distance, nextIndex);

		// check the neighboring branch not covered by the depth-first-search
		if (node.right() && sqr(value[index] - node.value()[index]) < distance)
			nearestNeighbor(*node.right(), value, nearest, distance, nextIndex);
	}
	else
	{
		ocean_assert(value[index] > node.value()[index]);

		if (node.right())
			nearestNeighbor(*node.right(), value, nearest, distance, nextIndex);

		// check the neighboring branch not covered by the depth-first-search
		if (node.left() && sqr(value[index] - node.value()[index]) < distance)
			nearestNeighbor(*node.left(), value, nearest, distance, nextIndex);
	}
}

template <typename T>
size_t KdTree<T>::radiusSearch(Node& node, const T* value, const typename SquareValueTyper<T>::Type radius, const T** values, const size_t maxValues, const unsigned int index) const
{
	ocean_assert(node && value && values);
	ocean_assert(index < treeDimension);

	if (maxValues == 0)
		return 0;

	T const ** current = values;
	T const ** const end = values + maxValues;

	const typename SquareValueTyper<T>::Type localDistance = determineSquareDistance(value, node.value());
	if (localDistance <= radius)
	{
		*current++ = node.value();
		if (current == end)
			return maxValues;
	}

	const unsigned int nextIndex = (index + 1u) % treeDimension;

	// depth-first-search
	if (value[index] <= node.value()[index])
	{
		if (node.left())
		{
			current += radiusSearch(*node.left(), value, radius, current, end - current, nextIndex);
			if (current == end)
				return maxValues;
		}

		// check the neighboring branch not covered by the depth-first-search
		if (node.right() && sqr(value[index] - node.value()[index]) <= radius)
			current += radiusSearch(*node.right(), value, radius, current, end - current, nextIndex);
	}
	else
	{
		ocean_assert(value[index] > node.value()[index]);

		if (node.right())
		{
			current += radiusSearch(*node.right(), value, radius, current, end - current, nextIndex);
			if (current == end)
				return maxValues;
		}

		// check the neighboring branch not covered by the depth-first-search
		if (node.left() && sqr(value[index] - node.value()[index]) <= radius)
			current += radiusSearch(*node.left(), value, radius, current, end - current, nextIndex);
	}

	ocean_assert(current <= end);

	return current - values;
}

template <typename T>
T KdTree<T>::median(const T** values, const size_t number, const unsigned int index)
{
	ocean_assert(values && number > 0);

	Elements elements(number);
	for (size_t n = 0; n < number; ++n)
		elements[n] = values[n][index];

	return Median::median<T>((T*)elements.data(), elements.size());
}

template <typename T>
void KdTree<T>::distribute(const T** values, const size_t number, const unsigned int index, const T*& medianValue, Pointers& leftValues, Pointers& rightValues)
{
	ocean_assert(values && number > 0);

	const T middle = median(values, number, index);

	leftValues.reserve(number / 2);
	rightValues.reserve(number / 2);

	ocean_assert(leftValues.empty() && rightValues.empty());

	bool medianFound = false;

	for (size_t n = 0; n < number; ++n)
	{
		if (values[n][index] < middle || (values[n][index] == middle && medianFound))
			leftValues.push_back(values[n]);
		else if (middle < values[n][index])
			rightValues.push_back(values[n]);
		else
		{
			ocean_assert(values[n][index] == middle);
			ocean_assert(!medianFound);

			medianValue = values[n];
			medianFound = true;
		}
	}

	ocean_assert(medianFound);
}

template <typename T>
inline typename SquareValueTyper<T>::Type KdTree<T>::determineSquareDistance(const T* first, const T* second) const
{
	ocean_assert(first && second);

	typename SquareValueTyper<T>::Type ssd = 0;

	for (size_t n = 0; n < treeDimension; ++n)
		ssd += sqr(first[n] - second[n]);

	return ssd;
}

}

#endif // META_OCEAN_BASE_KD_TREE_H
