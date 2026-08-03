/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_TRIPLE_H
#define META_OCEAN_BASE_TRIPLE_H

#include "ocean/base/Base.h"

namespace Ocean
{

// Forward declaration.
template <typename T1, typename T2, typename T3> class Triple;

/**
 * This class implements a triple object able to hold three individual elements.
 * @tparam T1 Data type of the first element of the triple object
 * @tparam T2 Data type of the second element of the triple object
 * @tparam T3 Data type of the third element of the triple object
 * @ingroup base
 */
template <typename T1, typename T2, typename T3>
class Triple
{
	public:

		/**
		 * Creates a new triple object with default values.
		 */
		inline Triple() = default;

		/**
		 * Creates a new triple object with three elements.
		 * @param first First element
		 * @param second Second element
		 * @param third Third element
		 */
		inline explicit Triple(const T1& first, const T2& second = T2(), const T3& third = T3());

		/**
		 * Returns the first element of this triple object.
		 * @return First element
		 */
		inline const T1& first() const;

		/**
		 * Returns the first element of this triple object.
		 * @return First element
		 */
		inline T1& first();

		/**
		 * Returns the second element of this triple object.
		 * @return First element
		 */
		inline const T2& second() const;

		/**
		 * Returns the second element of this triple object.
		 * @return Second element
		 */
		inline T2& second();

		/**
		 * Returns the third element of this triple object.
		 * @return Third element
		 */
		inline const T3& third() const;

		/**
		 * Returns the third element of this triple object.
		 * @return Third element
		 */
		inline T3& third();

		/**
		 * Returns whether two triples are equal.
		 * @param triple Second triple object
		 * @return True, if so
		 */
		inline bool operator==(const Triple<T1, T2, T3>& triple) const;

		/**
		 * Returns whether two triples are not equal.
		 * @param triple Second triple object
		 * @return True, if so
		 */
		inline bool operator!=(const Triple<T1, T2, T3>& triple) const;

		/**
		 * Returns whether this triple object is 'lesser' than a second triple object.
		 * @param triple The second triple object to be used for comparison
		 * @return True, if so
		 */
		inline bool operator<(const Triple<T1, T2, T3>& triple) const;

	private:

		/// First element of this triple.
		T1 first_ = T1();

		/// Second element of this triple.
		T2 second_ = T2();

		/// Third element of this triple.
		T3 third_ = T3();
};

template <typename T1, typename T2, typename T3>
inline Triple<T1, T2, T3>::Triple(const T1& first, const T2& second, const T3& third) :
	first_(first),
	second_(second),
	third_(third)
{
	// nothing to do here
}

template <typename T1, typename T2, typename T3>
inline const T1& Triple<T1, T2, T3>::first() const
{
	return first_;
}

template <typename T1, typename T2, typename T3>
inline T1& Triple<T1, T2, T3>::first()
{
	return first_;
}

template <typename T1, typename T2, typename T3>
inline const T2& Triple<T1, T2, T3>::second() const
{
	return second_;
}

template <typename T1, typename T2, typename T3>
inline T2& Triple<T1, T2, T3>::second()
{
	return second_;
}

template <typename T1, typename T2, typename T3>
inline const T3& Triple<T1, T2, T3>::third() const
{
	return third_;
}

template <typename T1, typename T2, typename T3>
inline T3& Triple<T1, T2, T3>::third()
{
	return third_;
}

template <typename T1, typename T2, typename T3>
inline bool Triple<T1, T2, T3>::operator==(const Triple<T1, T2, T3>& triple) const
{
	return first_ == triple.first_ && second_ == triple.second_ && third_ == triple.third_;
}

template <typename T1, typename T2, typename T3>
inline bool Triple<T1, T2, T3>::operator!=(const Triple<T1, T2, T3>& triple) const
{
	return !(*this == triple);
}

template <typename T1, typename T2, typename T3>
inline bool Triple<T1, T2, T3>::operator<(const Triple<T1, T2, T3>& triple) const
{
	return (first_ < triple.first_)
				|| (first_ == triple.first_ && second_ < triple.second_)
				|| (first_ == triple.first_ && second_ == triple.second_ && third_ < triple.third_);
}

}

#endif // META_OCEAN_BASE_TRIPLE_H
