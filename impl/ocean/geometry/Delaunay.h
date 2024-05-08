/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_DELAUNAY_H
#define META_OCEAN_GEOMETRY_DELAUNAY_H

#include "ocean/geometry/Geometry.h"

#include "ocean/math/Triangle2.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Line2.h"

#include <list>

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements Delaunay triangulation functions.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT Delaunay
{
	public:

		/**
		 * This class holds three indices of points representing a triangle.
		 */
		class IndexTriangle
		{
			public:

				/**
				 * Creates an invalid index triangle object.
				 */
				inline IndexTriangle();

				/**
				 * Creates a new index triangle object.
				 * @param index0 First index
				 * @param index1 Second index
				 * @param index2 Third index
				 */
				inline IndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2);

				/**
				 * Returns the first index.
				 * First triangle index
				 */
				inline unsigned int index0() const;

				/**
				 * Returns the second index.
				 * Second triangle index
				 */
				inline unsigned int index1() const;

				/**
				 * Returns the third index.
				 * Third triangle index
				 */
				inline unsigned int index2() const;

				/**
				 * Returns whether three individual indices are stored.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns one index of this triangle.
				 * @param n The (corner) index of the triangle for which the index is returned, with range [0, 2]
				 * @return The requested index
				 */
				inline unsigned int operator[](const unsigned int n) const;

				/**
				 * Returns whether two triangles are equal (i.e. whether both triangles are composed with the same indices, independent of their order).
				 * @param second The second triangle to test
				 * @return True, if both triangles are equal
				 */
				inline bool operator==(const IndexTriangle &second) const;

				/**
				 * Creates a 2D triangle with positions as corners from this index triangle.
				 * Beware: The number of points in the provided set, must exceed the maximal index, no check is done!
				 * @param points The set of points from which the corners are taken
				 */
				inline Triangle2 triangle2(const Vector2* points) const;

				/**
				 * Creates a 3D triangle with positions as corners from this index triangle.
				 * Beware: The number of points in the provided set, must exceed the maximal index, not check is done!
				 * @param points The set of points from which the corners are taken
				 */
				inline Triangle3 triangle3(const Vector3* points) const;

			protected:

				/// Triangle indices;
				unsigned int indices_[3];
		};

		/**
		 * Definition of a vector holding index triangles.
		 */
		typedef std::vector<IndexTriangle> IndexTriangles;

	protected:

		/**
		 * This class extends the IndexTriangle class by an additional circumcircle as the Delaunay triangulation is based on this information.
		 */
		class CircumCricleIndexTriangle : public IndexTriangle
		{
			public:

				/**
				 * Creates a new index triangle object and calculates circumcenter and radius.
				 * Beware: The number of points in the provided set, must exceed the maximal index, no range check is done!
				 * @param index0 First index
				 * @param index1 Second index
				 * @param index2 Third index
				 * @param points The set of points from which the corners are taken, must be valid
				 */
				inline CircumCricleIndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2, const Vector2* points);

				/**
				 * Creates a new index triangle object and calculates circumcenter and radius.
				 * This constructor takes either the corners from the set of provided points or from a second set of points defining the super triangle.<br>
				 * The indices of the three corners of the super triangle are considered to be [size, size + 1, size + 2], while 'size' will be subtracted from those indices before accessing the corresponding point from 'pointsSuperTriangle'.
				 * @param index0 First index, with range [0, size + 3)
				 * @param index1 Second index, with range [0, size + 3)
				 * @param index2 Third index, with range [0, size + 3)
				 * @param points The set of points from which the corners are taken, must be valid
				 * @param size The number of points not including points from the super triangle
				 * @param pointsSuperTriangle The three points from the super triangle
				 */
				inline CircumCricleIndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2, const Vector2* points, const size_t size, const Vector2* pointsSuperTriangle);

				/**
				 * Swaps the order of the indices from a counter clockwise order to a clockwise order or vice versa.
				 */
				inline void swap();

				/**
				 * Returns whether a point is within the circumcircle of the triangle.
				 * @param point The point to evaluate if inside circumcircle
				 * @param epsilon The epsilon value used for a slightly more generous comparison, with range [0, infinity)
				 * @return True, if so
				 */
				inline bool isInsideCircumCircle(const Vector2 &point, const Scalar epsilon = Numeric::eps()) const;

				/**
				 * Returns whether a point is outside the circumcircle of the triangle.
				 * @param point The point to evaluate if outside circumcircle
				 * @param epsilon The epsilon value used for a slightly more generous comparison, with range [0, infinity)
				 * @return True, if so
				 */
				inline bool isOutsideCircumCircle(const Vector2 &point, const Scalar epsilon = Numeric::eps()) const;

				/**
				 * Returns whether a point is completely right of the circumcircle (i.e. point.x > circle.center + circle radius).
				 * This IndexTriangle must be initialized with a defined point set (i.e. circumcenter and circumradius must be calculated during initialization).
				 * @param point The point to evaluate if completely left of circumcircle
				 * @return True, if so
				 */
				inline bool isRightOfCircumCircle(const Vector2 &point) const;

			private:

				/// 2D (Cartesian) circumcenter.
				Vector2 circumcenter_ = Vector2(0, 0);

				/// Radius of the circumcircle, with range [0, infinity), -1 for and invalid triangle object.
				Scalar circumcircleRadius_ = -1;
		};

		/**
		 * Definition of a vector holding extended index triangles.
		 */
		typedef std::vector<CircumCricleIndexTriangle> CircumCricleIndexTriangles;

		/**
		 * Definition of a list holding extended index triangles.
		 */
		typedef std::list<CircumCricleIndexTriangle> CircumCricleIndexTriangleList;

		/**
		 * This class implements the lesser operator for indices of points.
		 * The sorting will be applied according to x coordinates of points, but instead of modifying the order of points, this class modifies the order of indices of points.
		 */
		class ComparePointsX
		{
			public:

				/**
				 * Creates a new comparison object by a set of given points.
				 * @param points The points to be used for sorting, must be valid
				 */
				explicit inline ComparePointsX(const Vector2* points);

				/**
				 * Compares two point indices and returns whether the x coordinate of the first point is below the x coordinate of the second point.
				 * @param a The index of the first point
				 * @param b The index of the second point
				 * @return True, if so
				 */
				inline bool operator()(const unsigned int a, const unsigned int b) const;

			protected:

				/// The points to be compared.
				const Vector2* dataPoints_ = nullptr;
		};

		/**
		 * This class stores the sorted indices of an edge.
		 */
		class IndexEdge
		{
			public:

				/**
				 * Creates a new edge object and sorts the provided two point indices to ensure that the first index is smaller than the second index.
				 * @param indexFirst The index of the first point
				 * @param indexSecond The index of the second point, must be different from indexFirst
				 */
				inline IndexEdge(const unsigned int indexFirst, const unsigned int indexSecond);

				/**
				 * Creates a new edge object and sorts the provided two point indices to ensure that the point of the first index is 'smaller' than the point of the second index.
				 * @param indexFirst The index of the first point
				 * @param indexSecond The index of the second point, must be different from indexFirst
				 * @param points The points to be used for sorting, must be valid, ensure that enough points are provided
				 */
				inline IndexEdge(const unsigned int indexFirst, const unsigned int indexSecond, const Vector2* points);

				/**
				 * Returns the index of the first point.
				 * @return The point's first index
				 */
				inline unsigned int firstIndex() const;

				/**
				 * Returns the index of the second point.
				 * @return The point's second index
				 */
				inline unsigned int secondIndex() const;

				/**
				 * Lesser operator for two edge objects.
				 * @param right The second edge object to compare
				 * @return True, if this edge object is 'lesser' than the second one
				 */
				inline bool operator<(const IndexEdge& right) const;

			protected:

				/// The index of the first point.
				unsigned int firstIndex_ = (unsigned int)(-1);

				/// The index of the second point.
				unsigned int secondIndex_ = (unsigned int)(-1);
		};

		/**
		 * Definition of a map mapping edge pairs to a counter.
		 */
		typedef std::map<IndexEdge, unsigned int> EdgeMap;

	public:

		/**
		 * Determines the delaunay triangulation for a given 2D point set.
		 * The implementation is based on the Bowyer-Watson algorithm.
		 * @param points 2D point set to be triangulated, at least three
		 * @return Resulting triangulation
		 */
		static IndexTriangles triangulation(const Vectors2& points);

		/**
		 * Checks a Delaunay triangulation for integrity: no points are allowed within the circumcircle of a triangle.
		 * @param triangles Delaunay triangulation
		 * @param points Vector with points coordinates
		 * @param epsilon The epsilon value used for a slightly more generous comparison, with range [0, infinity)
		 * @return True, if the provided triangulation is a valid Delaunay triangulation
		 */
		static bool checkTriangulation(const IndexTriangles& triangles, const Vectors2& points, const Scalar epsilon = Numeric::eps());

	protected:

		/**
		 * Checks a Delaunay triangulation for integrity: no points are allowed within the circumcircle of a triangle.
		 * @param triangles Delaunay triangulation
		 * @param points Vector with points coordinates
		 * @param epsilon The epsilon value used for a slightly more generous comparison, with range [0, infinity)
		 * @return True, if the provided triangulation is a valid Delaunay triangulation
		 */
		static bool checkTriangulation(const CircumCricleIndexTriangles& triangles, const Vectors2& points, const Scalar epsilon = Numeric::eps());
};

inline Delaunay::IndexTriangle::IndexTriangle()
{
	indices_[0] = (unsigned int)(-1);
	indices_[1] = (unsigned int)(-1);
	indices_[2] = (unsigned int)(-1);

	ocean_assert(!isValid());
}

inline Delaunay::IndexTriangle::IndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2)
{
	indices_[0] = index0;
	indices_[1] = index1;
	indices_[2] = index2;

	ocean_assert(isValid());
}

inline unsigned int Delaunay::IndexTriangle::index0() const
{
	return indices_[0];
}

inline unsigned int Delaunay::IndexTriangle::index1() const
{
	return indices_[1];
}

inline unsigned int Delaunay::IndexTriangle::index2() const
{
	return indices_[2];
}

inline bool Delaunay::IndexTriangle::isValid() const
{
	return indices_[0] != indices_[1] && indices_[0] != indices_[2] && indices_[1] != indices_[2];
}

inline unsigned int Delaunay::IndexTriangle::operator[](const unsigned int n) const
{
	ocean_assert(n < 3u);
	return indices_[n];
}

inline bool Delaunay::IndexTriangle::operator==(const IndexTriangle &second) const
{
	ocean_assert(isValid() && second.isValid());

	return (index0() == second.index0() || index0() == second.index1() || index0() == second.index2())
				&& (index1() == second.index0() || index1() == second.index1() || index1() == second.index2())
				&& (index2() == second.index0() || index2() == second.index1() || index2() == second.index2());
}

inline Triangle2 Delaunay::IndexTriangle::triangle2(const Vector2* points) const
{
	ocean_assert(isValid() && points);

	return Triangle2(points[indices_[0]], points[indices_[1]], points[indices_[2]]);
}

inline Triangle3 Delaunay::IndexTriangle::triangle3(const Vector3* points) const
{
	ocean_assert(isValid() && points);

	return Triangle3(points[indices_[0]], points[indices_[1]], points[indices_[2]]);
}

inline Delaunay::CircumCricleIndexTriangle::CircumCricleIndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2, const Vector2* points) :
	IndexTriangle(index0, index1, index2),
	circumcenter_(0, 0),
	circumcircleRadius_(-1)
{
	ocean_assert(points);

	// make sure points are not co-linear
	ocean_assert(points[index0] != points[index1] && points[index0] != points[index2] && points[index1] != points[index2]);

	if constexpr (std::is_same<double, Scalar>::value)
	{
		ocean_assert(!Line2(points[index0], (points[index1] - points[index0]).normalizedOrZero()).isOnLine(points[index2]));
	}

	const Triangle2 triangle = triangle2(points);
	ocean_assert(triangle.isValid());

	circumcenter_ = triangle.cartesianCircumcenter();

	// radius is equivalent to the distance between the circumcenter and all corners
	circumcircleRadius_ = circumcenter_.distance(triangle.point0());

#ifdef OCEAN_DEBUG

	if constexpr (std::is_same<double, Scalar>::value)
	{
		// sanity check, all distances should be equal
		ocean_assert(Numeric::isEqual(circumcircleRadius_, circumcenter_.distance(triangle.point1()), Numeric::weakEps()));
		ocean_assert(Numeric::isEqual(circumcircleRadius_, circumcenter_.distance(triangle.point2()), Numeric::weakEps()));
		ocean_assert(Numeric::isEqual(circumcenter_.distance(triangle.point1()), circumcenter_.distance(triangle.point2()), Numeric::weakEps()));
	}

#endif
}

inline Delaunay::CircumCricleIndexTriangle::CircumCricleIndexTriangle(const unsigned int index0, const unsigned int index1, const unsigned int index2, const Vector2* points, const size_t size, const Vector2* pointsSuperTriangle) :
	IndexTriangle(index0, index1, index2),
	circumcenter_(0, 0),
	circumcircleRadius_(-1)
{
	ocean_assert(points && pointsSuperTriangle);

	const Vector2& point0 = size_t(index0) < size ? points[index0] : pointsSuperTriangle[size_t(index0) - size];
	const Vector2& point1 = size_t(index1) < size ? points[index1] : pointsSuperTriangle[size_t(index1) - size];
	const Vector2& point2 = size_t(index2) < size ? points[index2] : pointsSuperTriangle[size_t(index2) - size];

	// make sure points are not co-linear
	ocean_assert(point0 != point1 && point0 != point2 && point1 != point2);

	if constexpr (std::is_same<double, Scalar>::value)
	{
		ocean_assert(!Line2(point0, (point1 - point0).normalizedOrZero()).isOnLine(point2));
	}

	const Triangle2 triangle(point0, point1, point2);
	ocean_assert(triangle.isValid());

	circumcenter_ = triangle.cartesianCircumcenter();

	// radius is equivalent to the distance between the circumcenter and all corners
	circumcircleRadius_ = circumcenter_.distance(triangle.point0());

#ifdef OCEAN_DEBUG

	if constexpr (std::is_same<double, Scalar>::value)
	{
		// sanity check, all distances should be equal
		ocean_assert(Numeric::isEqual(circumcircleRadius_, circumcenter_.distance(triangle.point1()), Numeric::weakEps()));
		ocean_assert(Numeric::isEqual(circumcircleRadius_, circumcenter_.distance(triangle.point2()), Numeric::weakEps()));
		ocean_assert(Numeric::isEqual(circumcenter_.distance(triangle.point1()), circumcenter_.distance(triangle.point2()), Numeric::weakEps()));
	}

#endif
}

inline void Delaunay::CircumCricleIndexTriangle::swap()
{
	std::swap(indices_[1], indices_[2]);
}

inline bool Delaunay::CircumCricleIndexTriangle::isInsideCircumCircle(const Vector2 &point, const Scalar epsilon) const
{
	ocean_assert(isValid());
	ocean_assert(circumcircleRadius_ >= 0);
	ocean_assert(epsilon >= 0);

	// make radius slightly higher in order to catch co-circular points
	return circumcenter_.sqrDistance(point) <= Numeric::sqr(circumcircleRadius_ + epsilon);
}

inline bool Delaunay::CircumCricleIndexTriangle::isOutsideCircumCircle(const Vector2 &point, const Scalar epsilon) const
{
	ocean_assert(isValid());
	ocean_assert(circumcircleRadius_ >= 0);
	ocean_assert(epsilon >= 0);

	// make radius slightly smaller in order to catch co-circular points
	return circumcenter_.sqrDistance(point) + Numeric::sqr(epsilon) >= Numeric::sqr(circumcircleRadius_);
}

inline bool Delaunay::CircumCricleIndexTriangle::isRightOfCircumCircle(const Vector2 &point) const
{
	ocean_assert(isValid());
	ocean_assert(circumcircleRadius_ >= 0);

	return (circumcenter_.x() + circumcircleRadius_) < point.x();
}

inline Delaunay::ComparePointsX::ComparePointsX(const Vector2* points) :
	dataPoints_(points)
{
	ocean_assert(dataPoints_);
}

inline bool Delaunay::ComparePointsX::operator()(const unsigned int a, const unsigned int b) const
{
	ocean_assert(dataPoints_);

	return dataPoints_[a].x() < dataPoints_[b].x();
}

inline Delaunay::IndexEdge::IndexEdge(const unsigned int indexFirst, const unsigned int indexSecond) :
	firstIndex_(indexFirst < indexSecond ? indexFirst : indexSecond),
	secondIndex_(indexFirst < indexSecond ? indexSecond : indexFirst)
{
	ocean_assert(firstIndex_ != secondIndex_);
	ocean_assert(firstIndex_ < secondIndex_);
}

inline Delaunay::IndexEdge::IndexEdge(const unsigned int indexFirst, const unsigned int indexSecond, const Vector2* points) :
	firstIndex_(points[indexFirst] < points[indexSecond] ? indexFirst : indexSecond),
	secondIndex_(points[indexFirst] < points[indexSecond] ? indexSecond : indexFirst)
{
	ocean_assert(firstIndex_ != secondIndex_);
	ocean_assert(firstIndex_ < secondIndex_);
}

inline unsigned int Delaunay::IndexEdge::firstIndex() const
{
	return firstIndex_;
}

inline unsigned int Delaunay::IndexEdge::secondIndex() const
{
	return secondIndex_;
}

inline bool Delaunay::IndexEdge::operator<(const IndexEdge& right) const
{
	return firstIndex_ < right.firstIndex_ || (firstIndex_ == right.firstIndex_ && secondIndex_ < right.secondIndex_);
}

}

}

#endif // META_OCEAN_GEOMETRY_DELAUNAY_H
