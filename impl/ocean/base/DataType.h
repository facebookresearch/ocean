/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_DATA_TYPE_H
#define META_OCEAN_BASE_DATA_TYPE_H

#include "ocean/base/Base.h"

#include <limits>

namespace Ocean
{

/**
 * Template class allowing to define an array of data types.
 * The size of the resulting object is guaranteed to be sizeof(T) * tElements.
 * @tparam T The data type of each element
 * @tparam tElements Number of elements in the data type, with range [1, infinity)
 * @ingroup base
 */
template <typename T, unsigned int tElements>
class DataType
{
	public:

		/// Default definition of a type with tBytes bytes.
		struct Type
		{
			/**
			 * Returns whether two objects store the same values.
			 * @param type The second type to compare
			 * @return True, if so
			 */
			inline bool operator==(const Type& type) const;

			/**
			 * Returns whether two objects do not store the same values.
			 * @param type The second type to compare
			 * @return True, if so
			 */
			inline bool operator!=(const Type& type) const;

			/// Data values of the type.
			T values_[tElements];
		};

		static_assert(sizeof(Type) == sizeof(T) * tElements, "Invalid data type!");
};

/**
 * This class is a helper class allowing to define the square difference data type of a given type.
 * Beware: There is no guarantee that the resulting data type can store any squared value without a value overflow.<br>
 * By default the resulting data type is identical with the given data type.<br>
 * The following table shows the resulting data types which are different from the given data types:<br>
 * <pre>
 * Data type:        Squared data type:
 * int               unsigned int
 * char              unsigned int
 * signed char       unsigned int
 * unsigned char     unsigned int
 * short             unsigned int
 * long              unsigned long
 * long long         unsigned long long
 * </pre>
 * The following code example explains the usage of the helper class:
 * @code
 * void function()
 * {
 *     typedef SquareValueTyper<char>::Type SquareType;
 *
 *     char value = 7;
 *
 *     SquareType squareValue = SquareType(value * value);
 * }
 * @endcode
 * @tparam T Data type for that the square value type has to be found
 * @see DifferenceValueTyper
 * @ingroup base
 */
template <typename T>
class SquareValueTyper
{
	public:

		/**
		 * Definition of the data type for the square value.
		 * The default type is the type itself.
		 */
		typedef T Type;
};

/**
 * This class is a helper class allowing to define the signed difference data type of a given type.
 * Beware: There is no guarantee that the resulting data type can store any difference value without a value overflow.<br>
 * By default the resulting data type is identical with the given data type.<br>
 * The following table shows the resulting data types which are different from the given data types:<br>
 * <pre>
 * Data type:            Signed difference data type:
 * char                  int
 * signed char           int
 * unsigned char         int
 * unsigned int          int
 * short                 int
 * unsigned short        int
 * unsigned long         long
 * unsigned long long    long long
 * </pre>
 * The following code example explains the usage of the helper class:
 * @code
 * void function()
 * {
 *     typedef DifferenceValueTyper<char>::Type DifferenceType;
 *
 *     char valueA = 7;
 *     char valueB = 170;
 *
 *     DifferenceType differenceValue = DifferenceType(valueA - valueB);
 * }
 * @endcode
 * @tparam T Data type for that the signed difference value type has to be found
 * @see AbsoluteDifferenceValueTyper, SquareValueTyper
 * @ingroup base
 */
template <typename T>
class DifferenceValueTyper
{
	public:

		/**
		 * Definition of the data type for the signed difference value.
		 * The default type is the type itself.
		 */
		typedef T Type;
};

/**
 * This class is a helper class allowing to define the resulting data type for an absolute difference operation.
 * The following table shows the resulting data types:
 * <pre>
 * Data type:            Absolute difference data type:
 *
 * char                  unsigned int
 * signed char           unsigned int
 * unsigned char         unsigned int
 *
 * short                 unsigned long long
 * unsigned short        unsigned long long
 * int                   unsigned long long
 * unsigned int          unsigned long long
 *
 * long long             unsigned long long
 * unsigned long long    unsigned long long
 *
 * float                 double
 * double                double
 * </pre>
 * The following code example shows the usage of the helper class:
 * @code
 * void function()
 * {
 *     typedef AbsoluteDifferenceValueTyper<char>::Type AbsoluteDifferenceType;
 *
 *     char valueA = 7;
 *     char valueB = 170;
 *
 *     AbsoluteDifferenceType absoluteDifferenceValue = AbsoluteDifferenceType(valueA - valueB);
 * }
 * @endcode
 * @tparam T Data type for that the signed difference value type has to be found
 * @see DifferenceValueTyper
 * @ingroup base
 */
template <typename T>
class AbsoluteDifferenceValueTyper
{
	public:

		/**
		 * Definition of the data type for the absolute difference value.
		 * The default type is 'unsigned long long'.
		 */
		typedef unsigned long long Type;
};

/**
 * This class is a helper class allowing to determine the next larger data type for a given data type.
 * By default the resulting data type is identical with the given data type.<br>
 * The following table shows the resulting data types which are different from the given data types:<br>
 * <pre>
 * Data type:            Next larger data type:      Next larger data type regarding performance:
 * char                  UNDEFINED                   UNDEFINED, (as the signed of 'char' is not standardized)
 * signed char           short                       int
 * unsigned char         unsigned short              unsigned int
 * short                 int                         int
 * unsigned short        unsigned int                unsigned int
 * int                   long long                   long long
 * unsigned int          unsigned long long          unsigned long long
 * float                 double                      float
 * </pre>
 * @tparam T Data type for which the next larger data type has to be found
 * @ingroup base
 */
template <typename T>
class NextLargerTyper
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 * The default type is the type itself.
		 */
		typedef T Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 * The default type is the type itself.
		 */
		typedef T TypePerformance;
};

/**
 * This class is a helper class allowing to determine the signed data type for a given data type if existing, otherwise the identity data type.
 * By default the resulting data type is identical with the given data type.<br>
 * The following table shows the resulting data types which are different from the given data types:
 * <pre>
 * Data type:            Resulting (signed if possible) Data type
 * bool                  bool (but it is not signed)
 * char                  UNDEFINED, (as the signed of 'char' is not standardized)
 * unsigned char         signed char
 * short                 short
 * unsigned short        short
 * int                   int
 * unsigned int          int
 * long long             long long
 * unsigned long long    long long
 * float                 float
 * double                double
 * </pre>
 * Beware: The resulting data type of SignedTyper<T>::Type may be different from std::make_signed<T>::type (as e.g., std::make_signed<double>::type is not double).
 * @tparam T Data type for which the signed data type has to be found
 * @ingroup base
 */
template <typename T>
class SignedTyper
{
	public:

		/**
		 * Definition of the signed data type, if existing.
		 */
		typedef T Type;

		/**
		 * True, if the data type T is an signed data type; False, otherwise.
		 */
		static const bool isSigned = true;

		/**
		 * True, if the data has a corresponding signed data type (or itself is signed).
		 */
		static const bool hasSigned = true;
};

/**
 * This class is a helper class allowing to determine the unsigned data type for a given data type if existing, otherwise the identity data type.
 * By default the resulting data type is identical with the given data type.<br>
 * The following table shows the resulting data types which are different from the given data types:
 * <pre>
 * Data type:            Resulting (unsigned if possible) Data type
 * bool                  bool (seems to be an unsigned data type)
 * char                  UNDEFINED, (as the signed of 'char' is not standardized)
 * signed char           unsigned char
 * unsigned char         unsigned char
 * short                 unsigned short
 * unsigned short        unsigned short
 * int                   unsigned int
 * unsigned int          unsigned int
 * long                  unsigned long
 * long long             unsigned long long
 * unsigned long long    unsigned long long
 * float                 float (no corresponding unsigned data type exists)
 * double                double (no corresponding unsigned data type exists)
 * </pre>
 * Beware: The resulting data type of UnsignedTyper<T>::Type may be different from std::make_signed<T>::type (as e.g., std::make_signed<double>::type is not double).
 * @tparam T Data type for which the unsigned data type has to be found
 * @ingroup base
 */
template <typename T>
class UnsignedTyper
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef T Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = true;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * This struct is a helper struct providing a floating point data type best matching for a given data type.
 * The resulting data type is either a 32 bit floating point data type or a 64 bit floating point data type.<br>
 * Overall, the Type field of this struct follows this simple pattern:
 * <pre>
 * T:                 MatchingFloat<T>::Type:
 * double             double
 * float              float
 * char               float
 * int                float
 * 'any other type'   float
 * </pre>
 * @tparam T The data type for which the matching floating point data type is needed
 * @ingroup base
 */
template <typename T>
struct FloatTyper
{
	/**
	 * The 32 bit floating point data type for any data type T but 'double'
	 */
	typedef float Type;
};

/**
 * This struct implements a helper struct allowing to use a data type instead of void.
 * Overall, the Type field of this struct follows this simple pattern:
 * <pre>
 * T:                 NotVoidTyper<T>::Type:             NotVoidTyper<T>::defaultValue:
 * void               bool                               true
 * 'any other type'   'any other type'                   T()
 * </pre>
 * @tparam T The data type
 * @ingroup base
 */
template <typename T>
class NotVoidTyper
{
	public:

		/**
		 * Definition of the data type which is 'bool' in case of 'T' is 'void'
		 */
		typedef T Type;

		/**
		 * Returns a default value for 'T'.
		 * @return The default value
		 */
		static constexpr T defaultValue()
		{
			return T();
		}
};

/**
 * Specialization of NotVoidTyper<T>.
 */
template <>
class NotVoidTyper<void>
{
	public:

		/**
		 * Definition of the data type which is 'bool' in case of 'T' is 'void'
		 */
		typedef bool Type;

		/**
		 * Returns a default value for 'T'.
		 * @return The default value
		 */
		static constexpr bool defaultValue()
		{
			return true;
		}
};

/**
 * This class implements a helper class allowing to determine unique names for most of native C++ data types.
 * @ingroup base
 */
class TypeNamer
{
	public:

		/**
		 * Returns a user-friendly name for most of native C++ data types.
		 * @return The user friendly name
		 * @tparam T The data type for which the name is returned
		 */
		template <typename T>
		static inline const char* name();
};

/**
 * This class provides a unique data type for any data type specified by the number of bytes.
 * The class can be used to reduce binary size of template functions with code mainly defined by the size of a data type instead of the data type itself.<br>
 * This class provides the following mapping:
 * <pre>
 * Size of data type:         Resulting mapped data type
 * 1 (e.g., char)             uint8_t
 * 2 (e.g., unigned short)    uint16_t
 * 4 (e.g., float)            uint32_t
 * 8 (e.g., double)           uint64_t
 * </pre>
 * @tparam tBytes The number of bytes the data type must have, possible values are {1, 2, 4, 8}
 * @see TypeMapper.
 * @ingroup base
 */
template <size_t tBytes>
class TypeMapperBySize
{
	static_assert(tBytes == 1 || tBytes == 2 || tBytes == 4 || tBytes == 8, "Invalid data type!");

	public:

		/**
		 * Definition of an invalid mapped data type.
		 */
		typedef void Type;
};

/**
 * This class provides a unique data type for any data type.
 * The class can be used to reduce binary size of template functions with code mainly defined by the size of a data type instead of the data type itself.<br>
 * This class provides the following mapping:
 * <pre>
 * Data type:            Resulting mapped data type
 * bool                  unsigned char
 * char                  unsigned char
 * signed char           unsigned char
 * unsigned char         unsigned char
 * short                 unsigned short
 * unsigned short        unsigned short
 * int                   unsigned int
 * unsigned int          unsigned int
 * float                 unsigned int
 * long long             unsigned long long
 * unsigned long long    unsigned long long
 * double                unsigned long long
 * </pre>
 * @tparam T The data type for which a corresponding data type will be determined, with possible sizeof {1, 2, 4, 8}
 * @see TypeMapperBySize.
 * @ingroup base
 */
template <typename T>
class TypeMapper
{
	static_assert(sizeof(T) == 1 || sizeof(T)  == 2 || sizeof(T)  == 4 || sizeof(T)  == 8, "Invalid data type!");

	public:

		/**
		 * Definition of an invalid mapped data type.
		 */
		typedef typename TypeMapperBySize<sizeof(T)>::Type Type;
};

/**
 * Helper class allowing to define an ordered or unordered map based on the template parameter 'tOrderedKeys'.
 * @tparam tOrderedKeys True, to allow accessing the keys in order; False, if the order of the keys is not of interest
 * @ingroup base
 */
template <bool tOrderedKeys>
class MapTyper
{
	public:

		/**
		 * Definition of an ordered map.
		 */
		template <typename TKey, typename TElement>
		using TMap = std::map<TKey, TElement>;
};

/**
 * Helper class allowing to define an ordered or unordered map based on the template parameter 'tOrderedKeys'.
 */
template <>
class MapTyper<false>
{
	public:

		/**
		 * Definition of an unordered map.
		 */
		template <typename TKey, typename TElement>
		using TMap = std::unordered_map<TKey, TElement>;
};

/**
 * Definition of a 16-bit float type.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Float16
{
	public:

		/**
		 * Default constructor.
		 */
		Float16() = default;

		/**
		 * Creates a new 16-bit float.
		* @param sign The 1-bit sign to be used, with range [0, 1]
		 * @param fraction The 10-bit fraction to be used, with range [0, 1023]
		 * @param exponent The 5-bit exponent to be used, with range [0, 31]
		 */
		inline Float16(const uint16_t sign, const uint16_t fraction, const uint16_t exponent);

		/**
		 * Creates a new 16-bit float based on a 16-bit binary value.
		 * @param binary The binary value to be used
		 */
		explicit inline Float16(const uint16_t binary);

		/**
		 * Returns the binary representation of this 16-bit float value.
		 * @return The value's binary representation
		 */
		inline uint16_t binary() const;

		/**
		 * Cast operator to 32-bit float.
		 * @return The 32-bit float representation
		 */
		explicit operator float() const;

		/**
		 * Negates the 16-bit float value.
		 * @return The negated value
		 */
		inline Float16 operator-() const;

		/**
		 * Returns whether two 16-bit float values are identical.
		 * @param second The second value to compare
		 * @return True, if so
		 */
		inline bool operator==(const Float16& second) const;

		/**
		 * Returns whether two 16-bit float values are not identical.
		 * @param second The second value to compare
		 * @return True, if so
		 */
		inline bool operator!=(const Float16& second) const;

		/**
		 * Returns +infinity.
		 * @return The 16-bit float representing positive infinity
		 */
		static inline Float16 infinity();

	protected:

		/**
		 * Union allowing to access the binary representation or the IEEE representation.
		 */
		union
		{
			/// The binary representation.
			uint16_t binary_ = 0u;

			/**
			 * The struct with the IEEE representation.
			 */
			struct
			{
				/// The mantissa composed of 10 bits.
				uint16_t fraction_ : 10;

				/// The exponent composed of 5 bits.
				uint16_t exponent_ : 5;

				/// The sign composed of 1 bit.
				uint16_t sign_ : 1;
			} ieee_;
		} data_;
};

template <typename T, unsigned int tElements>
inline bool DataType<T, tElements>::Type::operator==(const Type& type) const
{
	for (unsigned int n = 0u; n < tElements; ++n)
	{
		if (values_[n] != type.values_[n])
		{
			return false;
		}
	}

	return true;
}

template <typename T, unsigned int tElements>
inline bool DataType<T, tElements>::Type::operator!=(const Type& type) const
{
	return !(*this == type);
}

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<int>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<char>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<signed char>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<unsigned char>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<short>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<unsigned short>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<long>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned long Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class SquareValueTyper<long long>
{
	public:

		/**
		 * Definition of the square distance type.
		 */
		typedef unsigned long long Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<char>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<signed char>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<unsigned char>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<unsigned int>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<short>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<unsigned short>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<unsigned long>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef long Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class DifferenceValueTyper<unsigned long long>
{
	public:

		/**
		 * Definition of the signed distance type.
		 */
		typedef long long Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class AbsoluteDifferenceValueTyper<char>
{
	public:

		/**
		 * Definition of the absolute difference value type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class AbsoluteDifferenceValueTyper<signed char>
{
	public:

		/**
		 * Definition of the absolute difference value type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class AbsoluteDifferenceValueTyper<unsigned char>
{
	public:

		/**
		 * Definition of the absolute difference value type.
		 */
		typedef unsigned int Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class AbsoluteDifferenceValueTyper<float>
{
	public:

		/**
		 * Definition of the absolute difference value type.
		 */
		typedef double Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class AbsoluteDifferenceValueTyper<double>
{
	public:

		/**
		 * Definition of the absolute difference value type.
		 */
		typedef double Type;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<char>
{
	// As the sign of 'char' is not standardized, we do not allow the usage of 'char', use 'int8_t' instead.
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<signed char>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef short Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef int TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<unsigned char>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef unsigned short Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef unsigned int TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<short>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef int Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef int TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<unsigned short>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef unsigned int Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef unsigned int TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<int>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef long long Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef long long TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<unsigned int>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef unsigned long long Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef unsigned long long TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class NextLargerTyper<float>
{
	public:

		/**
		 * Definition of the data type for the next larger data type.
		 */
		typedef double Type;

		/**
		 * Definition of the data type for the next larger data type regarding performance.
		 */
		typedef float TypePerformance;
};

/**
 * Specialization of the helper class.
 */
template <>
class SignedTyper<bool>
{
	public:

		/**
		 * Definition of the signed data type, if existing.
		 */
		typedef bool Type;

		/**
		 * True, if the data type T is an signed data type; False, otherwise.
		 */
		static const bool isSigned = false;

		/**
		 * True, if the data has a corresponding signed data type (or itself is signed).
		 */
		static const bool hasSigned = false;
};

/**
 * Specialization of the helper class.
 */
template <>
class SignedTyper<char>
{
	// As the sign of 'char' is not standardized, we do not allow the usage of 'char'.
};

/**
 * Specialization of the helper class.
 */
template <>
class SignedTyper<unsigned short>
{
	public:

		/**
		 * Definition of the signed data type, if existing.
		 */
		typedef short Type;

		/**
		 * True, if the data type T is an signed data type; False, otherwise.
		 */
		static const bool isSigned = false;

		/**
		 * True, if the data has a corresponding signed data type (or itself is signed).
		 */
		static const bool hasSigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class SignedTyper<unsigned int>
{
	public:

		/**
		 * Definition of the signed data type, if existing.
		 */
		typedef int Type;

		/**
		 * True, if the data type T is an signed data type; False, otherwise.
		 */
		static const bool isSigned = false;

		/**
		 * True, if the data has a corresponding signed data type (or itself is signed).
		 */
		static const bool hasSigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class SignedTyper<unsigned long long>
{
	public:

		/**
		 * Definition of the signed data type, if existing.
		 */
		typedef long long Type;

		/**
		 * True, if the data type T is an signed data type; False, otherwise.
		 */
		static const bool isSigned = false;

		/**
		 * True, if the data has a corresponding signed data type (or itself is signed).
		 */
		static const bool hasSigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<char>
{
	// As the sign of 'char' is not standardized, we do not allow the usage of 'char'.
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<signed char>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef unsigned char Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<short>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef unsigned short Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<int>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef unsigned int Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<long>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef unsigned long Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<long long>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef unsigned long long Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = true;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<float>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef float Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = false;
};

/**
 * Specialization of the helper class.
 */
template <>
class UnsignedTyper<double>
{
	public:

		/**
		 * Definition of the unsigned data type, if existing.
		 */
		typedef double Type;

		/**
		 * True, if the data type T is an unsigned data type; False, otherwise.
		 */
		static const bool isUnsigned = false;

		/**
		 * True, if the data has a corresponding unsigned data type (or itself is unsigned).
		 */
		static const bool hasUnsigned = false;
};

/**
 * Specialization of the helper struct.
 */
template <>
struct FloatTyper<double>
{
	/**
	 * The 64 bit floating point data type if T is 'double'
	 */
	typedef double Type;
};

template <>
inline const char* TypeNamer::name<bool>()
{
	static const char* value = "bool";
	return value;
}

template <>
inline const char* TypeNamer::name<char>()
{
	static const char* value = "char";
	return value;
}

template <>
inline const char* TypeNamer::name<signed char>()
{
	static const char* value = "signed char";
	return value;
}

template <>
inline const char* TypeNamer::name<unsigned char>()
{
	static const char* value = "unsigned char";
	return value;
}

template <>
inline const char* TypeNamer::name<short>()
{
	static const char* value = "short";
	return value;
}

template <>
inline const char* TypeNamer::name<unsigned short>()
{
	static const char* value = "unsigned short";
	return value;
}

template <>
inline const char* TypeNamer::name<int>()
{
	static const char* value = "int";
	return value;
}

template <>
inline const char* TypeNamer::name<unsigned int>()
{
	static const char* value = "unsigned int";
	return value;
}

template <>
inline const char* TypeNamer::name<long>()
{
	static const char* value = "long";
	return value;
}

template <>
inline const char* TypeNamer::name<unsigned long>()
{
	static const char* value = "unsigned long";
	return value;
}

template <>
inline const char* TypeNamer::name<long long>()
{
	static const char* value = "long long";
	return value;
}

template <>
inline const char* TypeNamer::name<unsigned long long>()
{
	static const char* value = "unsigned long long";
	return value;
}

template <>
inline const char* TypeNamer::name<float>()
{
	static const char* value = "float";
	return value;
}

template <>
inline const char* TypeNamer::name<double>()
{
	static const char* value = "double";
	return value;
}

/**
 * Specialized object.
 */
template <>
class TypeMapperBySize<1>
{
	public:

		/**
		 * Definition of the mapped data type.
		 */
		typedef uint8_t Type;
};

/**
 * Specialized object.
 */
template <>
class TypeMapperBySize<2>
{
	public:

		/**
		 * Definition of the mapped data type.
		 */
		typedef uint16_t Type;
};

/**
 * Specialized object.
 */
template <>
class TypeMapperBySize<4>
{
	public:

		/**
		 * Definition of the mapped data type.
		 */
		typedef uint32_t Type;
};

/**
 * Specialized object.
 */
template <>
class TypeMapperBySize<8>
{
	public:

		/**
		 * Definition of the mapped data type.
		 */
		typedef uint64_t Type;
};

inline Float16::Float16(const uint16_t sign, const uint16_t fraction, const uint16_t exponent)
{
	ocean_assert(sign <= 1u);
	ocean_assert(fraction <= 1023u);
	ocean_assert(exponent <= 31u);

	data_.ieee_.fraction_ = fraction;
	data_.ieee_.exponent_ = exponent;
	data_.ieee_.sign_ = sign;
}

inline Float16::Float16(const uint16_t binary)
{
	data_.binary_ = binary;
}

inline uint16_t Float16::binary() const
{
	return data_.binary_;
}

inline Float16 Float16::operator-() const
{
	return Float16((~data_.ieee_.sign_) & 0x1u, data_.ieee_.fraction_, data_.ieee_.exponent_);
}

inline bool Float16::operator==(const Float16& second) const
{
	return data_.binary_ == second.data_.binary_;
}

inline bool Float16::operator!=(const Float16& second) const
{
	return !(*this == second);
}

inline Float16 Float16::infinity()
{
	constexpr uint16_t maxExponent = 31u;

	return Float16(0u, maxExponent, 0u);
}

}

#endif // META_OCEAN_BASE_DATA_TYPE_H
