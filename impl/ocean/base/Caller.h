/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_CALLER_H
#define META_OCEAN_BASE_CALLER_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"

#ifdef OCEAN_SUPPORT_RTTI
	#include <typeinfo>
#endif

namespace Ocean
{

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * As this class is a helper class, there is no need to use it directly.<br>
 * The template parameters define the function's ten possible function parameters (PT0 - PT19).<br>
 * Therefore, functions with at most ten function parameters are supported.<br>
 * @tparam RT Return type
 * @tparam PT0 Data type of the 1st parameter
 * @tparam PT1 Data type of the 2nd parameter
 * @tparam PT2 Data type of the 3rd parameter
 * @tparam PT3 Data type of the 4th parameter
 * @tparam PT4 Data type of the 5th parameter
 * @tparam PT5 Data type of the 6th parameter
 * @tparam PT6 Data type of the 7th parameter
 * @tparam PT7 Data type of the 8th parameter
 * @tparam PT8 Data type of the 9th parameter
 * @tparam PT9 Data type of the 10th parameter
 * @tparam PT10 Data type of the 11th parameter
 * @tparam PT11 Data type of the 12th parameter
 * @tparam PT12 Data type of the 13th parameter
 * @tparam PT13 Data type of the 14th parameter
 * @tparam PT14 Data type of the 15th parameter
 * @tparam PT15 Data type of the 16th parameter
 * @tparam PT16 Data type of the 17th parameter
 * @tparam PT17 Data type of the 18th parameter
 * @tparam PT18 Data type of the 19th parameter
 * @tparam PT19 Data type of the 20th parameter
 * @ingroup base
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
class ParameterCounter
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};


/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
class ParameterCounter<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1, typename PT2>
class ParameterCounter<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0, typename PT1>
class ParameterCounter<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT, typename PT0>
class ParameterCounter<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

/**
 * This class implements a helper class providing the number of parameters of a parameterized function.<br>
 * @see ParameterCounter.
 */
template <typename RT>
class ParameterCounter<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Returns the number of parameters for a given function type.
		 * @return Parameter number
		 */
		static inline unsigned int parameters();
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::parameters()
{
	return 20u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::parameters()
{
	return 19u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::parameters()
{
	return 18u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::parameters()
{
	return 17u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 16u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 15u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 14u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 13u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 12u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 11u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 10u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 9u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 8u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 7u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 6u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 5u;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 4u;
}

template <typename RT, typename PT0, typename PT1, typename PT2>
inline unsigned int ParameterCounter<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 3u;
}

template <typename RT, typename PT0, typename PT1>
inline unsigned int ParameterCounter<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 2u;
}

template <typename RT, typename PT0>
inline unsigned int ParameterCounter<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 1u;
}

template <typename RT>
inline unsigned int ParameterCounter<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::parameters()
{
	return 0u;
}

/**
 * This class implements a simple helper class allowing to access the value of an arbitrary data type.
 * The assignment will be applied for any non-reference parameter only.
 * @tparam tIsReference True, if the target data type is a reference (so that nothing happens); False, if the target data type will receive the value of the source parameter
 * @ingroup base
 */
template <bool tIsReference>
class FunctionParameterAccessor
{
	public:

		/**
		 * Assigns the value of a source parameter to a target parameter.
		 * @param target The target parameter receiving the value of the source parameter
		 * @param source The address of the source parameter
		 * @tparam T The data type of the source and target parameter
		 */
		template <typename T>
		static inline void assign(typename std::add_lvalue_reference<T>::type target, const void* source);

		/**
		 * Receives the address of a specified parameter.
		 * The address will be nullptr if the provided parameter is a reference.
		 * @param parameter The parameter for which the address is requested
		 * @return The resulting type-less address of the specified parameter, nullptr if tIsReference is True
		 * @tparam T The data type of the source and target parameter
		 */
		template <typename T>
		static inline void* address(typename std::add_lvalue_reference<T>::type parameter);
};

template <bool tTargetIsReference>
template <typename T>
inline void FunctionParameterAccessor<tTargetIsReference>::assign(typename std::add_lvalue_reference<T>::type target, const void* source)
{
	typedef typename std::add_const<T>::type SourceType;

	target = *reinterpret_cast<SourceType*>(source);
}

template <bool tTargetIsReference>
template <typename T>
void* FunctionParameterAccessor<tTargetIsReference>::address(typename std::add_lvalue_reference<T>::type parameter)
{
	return reinterpret_cast<void *>(&parameter);
}

template <>
template <typename T>
inline void FunctionParameterAccessor<true>::assign(typename std::add_lvalue_reference<T>::type /*target*/, const void* /*source*/)
{
	ocean_assert(false && "Must not be called!");
}

template <>
template <>
inline void FunctionParameterAccessor<true>::assign<NullClass>(NullClass& /*target*/, const void* /*source*/)
{
	ocean_assert(false && "Must not be called!");
}

template <>
template <>
inline void FunctionParameterAccessor<false>::assign<NullClass>(NullClass& /*target*/, const void* /*source*/)
{
	ocean_assert(false && "Must not be called!");
}

template <>
template <typename T>
inline void*  FunctionParameterAccessor<true>::address(typename std::add_lvalue_reference<T>::type /*parameter*/)
{
	ocean_assert(false && "Must not be called!");
	return nullptr;
}

template <>
template <>
inline void* FunctionParameterAccessor<true>::address<NullClass>(NullClass& /*parameter*/)
{
	ocean_assert(false && "Must not be called!");
	return nullptr;
}

template <>
template <>
inline void* FunctionParameterAccessor<false>::address<NullClass>(NullClass& /*parameter*/)
{
	ocean_assert(false && "Must not be called!");
	return nullptr;
}

/**
 * This class is the base class for a parameterized function container.
 * This base function cantainer allows the specification of parameters and therefore can call the encapsulated function without explicit parameters.
 * @tparam RT Defines the return data type
 * @ingroup base
 */
template <typename RT>
class ParameterizedFunctionBase
{
	public:

		/**
		 * Destructs a parameterized function object.
		 */
		virtual ~ParameterizedFunctionBase();

		/**
		 * Calls the internal encapsulated function and uses the specified parameters.
		 * @return Return value
		 */
		virtual RT call() const = 0;

		/**
		 * Creates a copy of the entire derivated object.
		 * @return Instance copy
		 */
		virtual ParameterizedFunctionBase* copy() const = 0;

		/**
		 * Returns the number of function parameters.
		 * @return Parameter number
		 */
		virtual unsigned int parameters() const = 0;

		/**
		 * Returns a specified parameter as a void pointer to the parameter's pointer.
		 * @param index Index of the parameter to change
		 * @return Requested parameter pointer
		 */
		virtual void* parameter(const unsigned int index) = 0;

		/**
		 * Sets a specified parameter.
		 * @param index Index of the parameter to change
		 * @param value New parameter value
		 */
		virtual void setParameter(const unsigned int index, const void* value) = 0;
	
#ifdef OCEAN_SUPPORT_RTTI

		/**
		 * Returns the parameter type of a specific parameter.
		 * @param index Index of the parameter
		 * @return Type information
		 */
		virtual const std::type_info& type(const unsigned int index) const = 0;
	
#endif
};

template <typename RT>
ParameterizedFunctionBase<RT>::~ParameterizedFunctionBase()
{
	// nothing to do here
}

/**
 * This class implements an parameterized function container.
 * This function cantainer holds function parameters and therefore can call the encapsulated function without explicit parameters.<br>
 * The template parameters define the return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * This class is a helper class only and used to allow caller functions to be independent from the member function's class type.
 * @tparam RT Return type
 * @tparam PT0 Data type of the 1st parameter
 * @tparam PT1 Data type of the 2nd parameter
 * @tparam PT2 Data type of the 3rd parameter
 * @tparam PT3 Data type of the 4th parameter
 * @tparam PT4 Data type of the 5th parameter
 * @tparam PT5 Data type of the 6th parameter
 * @tparam PT6 Data type of the 7th parameter
 * @tparam PT7 Data type of the 8th parameter
 * @tparam PT8 Data type of the 9th parameter
 * @tparam PT9 Data type of the 10th parameter
 * @tparam PT10 Data type of the 11th parameter
 * @tparam PT11 Data type of the 12th parameter
 * @tparam PT12 Data type of the 13th parameter
 * @tparam PT13 Data type of the 14th parameter
 * @tparam PT14 Data type of the 15th parameter
 * @tparam PT15 Data type of the 16th parameter
 * @tparam PT16 Data type of the 17th parameter
 * @tparam PT17 Data type of the 18th parameter
 * @tparam PT18 Data type of the 19th parameter
 * @tparam PT19 Data type of the 20th parameter
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class ParameterizedFunction : public ParameterizedFunctionBase<RT>
{
	protected:

		/**
		 * Creates a new function object.
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @param param19 20th function parameter
		 */
		inline ParameterizedFunction(PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19());

		/**
		 * Destructs a parameter function.
		 */
		virtual ~ParameterizedFunction();

		/**
		 * Returns the number of function parameters.
		 * @return Parameter number
		 */
		virtual unsigned int parameters() const;

		/**
		 * Returns a specified parameter as a void pointer to the parameter's pointer.
		 * @param index Index of the parameter to change
		 * @return Requested parameter pointer
		 */
		virtual void* parameter(const unsigned int index);

		/**
		 * Sets a specified parameter as long as the parameter is not a reference.
		 * @param index Index of the (non-reference) parameter to change
		 * @param value New parameter value
		 */
		virtual void setParameter(const unsigned int index, const void* value);
	
#ifdef OCEAN_SUPPORT_RTTI

		/**
		 * Returns the parameter type of a specific parameter.
		 * @param index Index of the parameter
		 * @return Type information
		 */
		virtual const std::type_info& type(const unsigned int index) const;
	
#endif

	protected:

		/// First function parameter.
		PT0 functionParam0;

		/// Second function parameter.
		PT1 functionParam1;

		/// Third function parameter.
		PT2 functionParam2;

		/// Fourth function parameter.
		PT3 functionParam3;

		/// Fifth function parameter.
		PT4 functionParam4;

		/// Sixth function parameter.
		PT5 functionParam5;

		/// Seventh function parameter.
		PT6 functionParam6;

		/// Eighth function parameter.
		PT7 functionParam7;

		/// Ninth function parameter.
		PT8 functionParam8;

		/// Tenth function parameter.
		PT9 functionParam9;

		/// Eleventh function parameter.
		PT10 functionParam10;

		/// Twelfth function parameter.
		PT11 functionParam11;

		/// 13th function parameter.
		PT12 functionParam12;

		/// 14th function parameter.
		PT13 functionParam13;

		/// 15th function parameter.
		PT14 functionParam14;

		/// 16th function parameter.
		PT15 functionParam15;

		/// 17th function parameter.
		PT16 functionParam16;

		/// 18th function parameter.
		PT17 functionParam17;

		/// 19th function parameter.
		PT18 functionParam18;

		/// 20th function parameter.
		PT19 functionParam19;
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::ParameterizedFunction(PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) :
	functionParam0(param0),
	functionParam1(param1),
	functionParam2(param2),
	functionParam3(param3),
	functionParam4(param4),
	functionParam5(param5),
	functionParam6(param6),
	functionParam7(param7),
	functionParam8(param8),
	functionParam9(param9),
	functionParam10(param10),
	functionParam11(param11),
	functionParam12(param12),
	functionParam13(param13),
	functionParam14(param14),
	functionParam15(param15),
	functionParam16(param16),
	functionParam17(param17),
	functionParam18(param18),
	functionParam19(param19)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::~ParameterizedFunction()
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
unsigned int ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::parameters() const
{
	return ParameterCounter<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::parameters();
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
void* ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::parameter(const unsigned int index)
{
	switch (index)
	{
		case 0u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT0>::value>:: template address<PT0>(functionParam0);

		case 1u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT1>::value>:: template address<PT1>(functionParam1);

		case 2u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT2>::value>:: template address<PT2>(functionParam2);

		case 3u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT3>::value>:: template address<PT3>(functionParam3);

		case 4u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT4>::value>:: template address<PT4>(functionParam4);

		case 5u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT5>::value>:: template address<PT5>(functionParam5);

		case 6u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT6>::value>:: template address<PT6>(functionParam6);

		case 7u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT7>::value>:: template address<PT7>(functionParam7);

		case 8u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT8>::value>:: template address<PT8>(functionParam8);

		case 9u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT9>::value>:: template address<PT9>(functionParam9);

		case 10u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT10>::value>:: template address<PT10>(functionParam10);

		case 11u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT11>::value>:: template address<PT11>(functionParam11);

		case 12u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT12>::value>:: template address<PT12>(functionParam12);

		case 13u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT13>::value>:: template address<PT13>(functionParam13);

		case 14u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT14>::value>:: template address<PT14>(functionParam14);

		case 15u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT15>::value>:: template address<PT15>(functionParam15);

		case 16u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT16>::value>:: template address<PT16>(functionParam16);

		case 17u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT17>::value>:: template address<PT17>(functionParam17);

		case 18u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT18>::value>:: template address<PT18>(functionParam18);

		case 19u:
			return FunctionParameterAccessor<std::is_lvalue_reference<PT19>::value>:: template address<PT19>(functionParam19);

		default:
			ocean_assert(false && "Missing implementation!");
			return nullptr;
	}
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
void ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::setParameter(const unsigned int index, const void* value)
{
	switch (index)
	{
		case 0u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT0>::value>:: template assign<PT0>(functionParam0, value);
			break;

		case 1u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT1>::value>:: template assign<PT1>(functionParam1, value);
			break;

		case 2u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT2>::value>:: template assign<PT2>(functionParam2, value);
			break;

		case 3u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT3>::value>:: template assign<PT3>(functionParam3, value);
			break;

		case 4u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT4>::value>:: template assign<PT4>(functionParam4, value);
			break;

		case 5u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT5>::value>:: template assign<PT5>(functionParam5, value);
			break;

		case 6u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT6>::value>:: template assign<PT6>(functionParam6, value);
			break;

		case 7u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT7>::value>:: template assign<PT7>(functionParam7, value);
			break;

		case 8u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT8>::value>:: template assign<PT8>(functionParam8, value);
			break;

		case 9u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT9>::value>:: template assign<PT9>(functionParam9, value);
			break;

		case 10u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT10>::value>:: template assign<PT10>(functionParam10, value);
			break;

		case 11u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT11>::value>:: template assign<PT11>(functionParam11, value);
			break;

		case 12u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT12>::value>:: template assign<PT12>(functionParam12, value);
			break;

		case 13u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT13>::value>:: template assign<PT13>(functionParam13, value);
			break;

		case 14u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT14>::value>:: template assign<PT14>(functionParam14, value);
			break;

		case 15u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT15>::value>:: template assign<PT15>(functionParam15, value);
			break;

		case 16u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT16>::value>:: template assign<PT16>(functionParam16, value);
			break;

		case 17u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT17>::value>:: template assign<PT17>(functionParam17, value);
			break;

		case 18u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT18>::value>:: template assign<PT18>(functionParam18, value);
			break;

		case 19u:
			FunctionParameterAccessor<std::is_lvalue_reference<PT19>::value>:: template assign<PT19>(functionParam19, value);
			break;

		default:
			ocean_assert(false && "Missing implementation!");
	}
}

#ifdef OCEAN_SUPPORT_RTTI

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
const std::type_info& ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::type(const unsigned int index) const
{
	switch (index)
	{
		case 0:
			return typeid(PT0);

		case 1:
			return typeid(PT1);

		case 2:
			return typeid(PT2);

		case 3:
			return typeid(PT3);

		case 4:
			return typeid(PT4);

		case 5:
			return typeid(PT5);

		case 6:
			return typeid(PT6);

		case 7:
			return typeid(PT7);

		case 8:
			return typeid(PT8);

		case 9:
			return typeid(PT9);

		case 10:
			return typeid(PT10);

		case 11:
			return typeid(PT11);

		case 12:
			return typeid(PT12);

		case 13:
			return typeid(PT13);

		case 14:
			return typeid(PT14);

		case 15:
			return typeid(PT15);

		case 16:
			return typeid(PT16);

		case 17:
			return typeid(PT17);

		case 18:
			return typeid(PT18);

		case 19:
			return typeid(PT19);
	}

	ocean_assert(false && "Invalid index!");
	return typeid(PT0);
}

#endif // OCEAN_SUPPORT_RTTI

/**
 * This class implements an parameterized member function container.
 * This function cantainer holds function parameters and therefore can call the encapsulated function without explicit parameters.<br>
 * The template parameters define the function's owner class type (CT), the return type (RT) and ten possible function parameters (PT0 - PT9).<br>
 * This class is a helper class only and used to allow caller functions to be independent from the member function's class type.
 * @tparam CT Type of the member object
 * @tparam RT Return type
 * @tparam PT0 Data type of the 1st parameter
 * @tparam PT1 Data type of the 2nd parameter
 * @tparam PT2 Data type of the 3rd parameter
 * @tparam PT3 Data type of the 4th parameter
 * @tparam PT4 Data type of the 5th parameter
 * @tparam PT5 Data type of the 6th parameter
 * @tparam PT6 Data type of the 7th parameter
 * @tparam PT7 Data type of the 8th parameter
 * @tparam PT8 Data type of the 9th parameter
 * @tparam PT9 Data type of the 10th parameter
 * @tparam PT10 Data type of the 11th parameter
 * @tparam PT11 Data type of the 12th parameter
 * @tparam PT12 Data type of the 13th parameter
 * @tparam PT13 Data type of the 14th parameter
 * @tparam PT14 Data type of the 15th parameter
 * @tparam PT15 Data type of the 16th parameter
 * @tparam PT16 Data type of the 17th parameter
 * @tparam PT17 Data type of the 18th parameter
 * @tparam PT18 Data type of the 19th parameter
 * @tparam PT19 Data type of the 20th parameter
 * @ingroup base
 */
template <typename CT, typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class ParameterizedMemberFunction : public ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>
{
	friend class Caller<RT>;

	protected:

		/**
		 * Definition of a function pointer for the encapsulated function.
		 */
		typedef typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type FunctionType;

	protected:

		/**
		 * Creates a new function object.
		 * @param object Class object holding the member function
		 * @param function Local member function pointer
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @param param19 20th function parameter
		 */
		inline ParameterizedMemberFunction(CT &object, FunctionType function, PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19());

		/**
		 * Calls the internal encapsulated function by the used of the previously defined parameters.
		 * @return Return value
		 */
		virtual RT call() const;

		/**
		 * Creates a copy of this function container.
		 * @return Instance copy
		 */
		virtual ParameterizedFunctionBase<RT>* copy() const;

	protected:

		/// Class object holding the member function.
		CT* functionClass;

		/// Function pointer of the member function.
		FunctionType functionPointer;
};

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::ParameterizedMemberFunction(CT &object, FunctionType function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) :
	ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19),
	functionClass(&object),
	functionPointer(function)
{
	// nothing to do here
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
RT ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call() const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != nullptr);
	return MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(functionClass, functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
ParameterizedFunctionBase<RT>* ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::copy() const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != nullptr);
	return new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(*functionClass, functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

/**
 * This class implements an parameterized const member function container.
 * This function cantainer holds function parameters and therefore can call the encapsulated function without explicit parameters.<br>
 * The template parameters define the function's owner class type (CT), the return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * This class is a helper class only and used to allow caller functions to be independent from the member function's class type.
 * @tparam CT Type of the member object
 * @tparam RT Return type
 * @tparam PT0 Data type of the 1st parameter
 * @tparam PT1 Data type of the 2nd parameter
 * @tparam PT2 Data type of the 3rd parameter
 * @tparam PT3 Data type of the 4th parameter
 * @tparam PT4 Data type of the 5th parameter
 * @tparam PT5 Data type of the 6th parameter
 * @tparam PT6 Data type of the 7th parameter
 * @tparam PT7 Data type of the 8th parameter
 * @tparam PT8 Data type of the 9th parameter
 * @tparam PT9 Data type of the 10th parameter
 * @tparam PT10 Data type of the 11th parameter
 * @tparam PT11 Data type of the 12th parameter
 * @tparam PT12 Data type of the 13th parameter
 * @tparam PT13 Data type of the 14th parameter
 * @tparam PT14 Data type of the 15th parameter
 * @tparam PT15 Data type of the 16th parameter
 * @tparam PT16 Data type of the 17th parameter
 * @tparam PT17 Data type of the 18th parameter
 * @tparam PT18 Data type of the 19th parameter
 * @tparam PT19 Data type of the 20th parameter
 * @ingroup base
 */
template <typename CT, typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class ParameterizedMemberFunctionConst : public ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>
{
	friend class Caller<RT>;

	protected:

		/**
		 * Definition of a const function pointer for the encapsulated function.
		 */
		typedef typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::TypeConst FunctionType;

	protected:

		/**
		 * Creates a new const function object.
		 * @param object Class object holding the member function
		 * @param function Local member function pointer
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @param param19 20th function parameter
		 */
		inline ParameterizedMemberFunctionConst(const CT &object, FunctionType function, PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19());

		/**
		 * Calls the internal encapsulated function by the used of the previously defined parameters.
		 * @return Return value
		 */
		virtual RT call() const;

		/**
		 * Creates a copy of this function container.
		 * @return Instance copy
		 */
		virtual ParameterizedFunctionBase<RT>* copy() const;

	protected:

		/// Class object holding the member function.
		const CT* functionClass;

		/// Function pointer of the member function.
		FunctionType functionPointer;
};

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::ParameterizedMemberFunctionConst(const CT &object, FunctionType function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) :
	ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19),
	functionClass(&object),
	functionPointer(function)
{
	// nothing to do here
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
RT ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call() const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != nullptr);
	return MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(functionClass, functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
ParameterizedFunctionBase<RT>* ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::copy() const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != nullptr);
	return new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(*functionClass, functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

/**
 * This class implements an parameterized static function container.
 * This function cantainer holds function parameters and therefore can call the encapsulated function without explicit parameters.<br>
 * The template parameters define the function's return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * This class is a helper class only and used to allow caller functions to be independent from the member function's class type.
 * @tparam RT Return type
 * @tparam PT0 Data type of the 1st parameter
 * @tparam PT1 Data type of the 2nd parameter
 * @tparam PT2 Data type of the 3rd parameter
 * @tparam PT3 Data type of the 4th parameter
 * @tparam PT4 Data type of the 5th parameter
 * @tparam PT5 Data type of the 6th parameter
 * @tparam PT6 Data type of the 7th parameter
 * @tparam PT7 Data type of the 8th parameter
 * @tparam PT8 Data type of the 9th parameter
 * @tparam PT9 Data type of the 10th parameter
 * @tparam PT10 Data type of the 11th parameter
 * @tparam PT11 Data type of the 12th parameter
 * @tparam PT12 Data type of the 13th parameter
 * @tparam PT13 Data type of the 14th parameter
 * @tparam PT14 Data type of the 15th parameter
 * @tparam PT15 Data type of the 16th parameter
 * @tparam PT16 Data type of the 17th parameter
 * @tparam PT17 Data type of the 18th parameter
 * @tparam PT18 Data type of the 19th parameter
 * @tparam PT19 Data type of the 20th parameter
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class ParameterizedStaticFunction : public ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>
{
	friend class Caller<RT>;

	protected:

		/**
		 * Definition of a function pointer for the encapsulated function.
		 */
		typedef typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type FunctionType;

	protected:

		/**
		 * Creates a new function object.
		 * @param function Local member function pointer
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @param param19 20th function parameter
		 */
		inline ParameterizedStaticFunction(FunctionType function, PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19());

		/**
		 * Calls the internal encapsulated function by the used of the previously defined parameters.
		 * @return Return value
		 */
		virtual RT call() const;

		/**
		 * Creates a copy of this function container.
		 * @return Instance copy
		 */
		virtual ParameterizedFunctionBase<RT>* copy() const;

	protected:

		/// Function pointer of the member function.
		FunctionType functionPointer;
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::ParameterizedStaticFunction(FunctionType function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) :
	ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19),
	functionPointer(function)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
RT ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call() const
{
	ocean_assert(functionPointer != nullptr);
	return StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
ParameterizedFunctionBase<RT>* ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::copy() const
{
	ocean_assert(functionPointer != nullptr);
	return new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(functionPointer,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam0,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam1,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam2,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam3,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam4,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam5,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam6,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam7,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam8,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam9,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam10,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam11,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam12,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam13,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam14,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam15,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam16,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam17,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam18,
				ParameterizedFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::functionParam19);
}

/**
 * This class implements a callback function container using defined function parameters.
 * Use this callback container to encapsulate callback functions to member functions of class objects.<br>
 * This class is independent from the class type and therefore can be stored without the knowledge of the class type.<br>
 * @tparam RT Specifies the return type of the encapsulated function
 * @ingroup base
 */
template <typename RT>
class Caller
{
	public:

		/**
		 * Creates an empty caller container.
		 */
		inline Caller();

		/**
		 * Copies a caller container.
		 * @param caller Caller container to copy
		 */
		inline Caller(const Caller& caller);

		/**
		 * Movies a caller container.
		 * @param caller Caller container to move
		 */
		inline Caller(Caller&& caller);

		/**
		 * Creates a new caller container for a static function with no function parameter.
		 * @param function Local function pointer of the member function
		 * @return Resulting static caller function
		 */
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameter.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18);

		/**
		 * Creates a new caller container for a static function and specifies the later used function parameters.
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @param param19 20th function parameter used for each function call
		 * @return Resulting static caller function
		 */
		template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
		static inline Caller<RT> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);

		/**
		 * Creates a new caller container for a member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @return Resulting caller function
		 */
		template <typename CT>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function);

		/**
		 * Creates a new caller container for a const member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @return Resulting caller function
		 */
		template <typename CT>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function);

		/**
		 * Creates a new caller container for a member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0);

		/**
		 * Creates a new caller container for a const member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0);

		/**
		 * Creates a new caller container for a member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1);

		/**
		 * Creates a new caller container for a const member function with no function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameter.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18);

		/**
		 * Creates a new caller container for a member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @param param19 20th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
		static inline Caller<RT> create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);

		/**
		 * Creates a new caller container for a const member function and specifies the later used function parameters.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @param param0 First function parameter used for each function call
		 * @param param1 Second function parameter used for each function call
		 * @param param2 Third function parameter used for each function call
		 * @param param3 Fourth function parameter used for each function call
		 * @param param4 Fifth function parameter used for each function call
		 * @param param5 Sixth function parameter used for each function call
		 * @param param6 Seventh function parameter used for each function call
		 * @param param7 Eighth function parameter used for each function call
		 * @param param8 Ninth function parameter used for each function call
		 * @param param9 Tenth function parameter used for each function call
		 * @param param10 11th function parameter used for each function call
		 * @param param11 12th function parameter used for each function call
		 * @param param12 13th function parameter used for each function call
		 * @param param13 14th function parameter used for each function call
		 * @param param14 15th function parameter used for each function call
		 * @param param15 16th function parameter used for each function call
		 * @param param16 17th function parameter used for each function call
		 * @param param17 18th function parameter used for each function call
		 * @param param18 19th function parameter used for each function call
		 * @param param19 20th function parameter used for each function call
		 * @return Resulting caller function
		 */
		template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
		static inline Caller<RT> create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);

		/**
		 * Destructs a caller container.
		 */
		~Caller();

		/**
		 * Returns the number of function parameters.
		 * @return Parameter number
		 */
		unsigned int parameters() const;

		/**
		 * Template function to return a parameter.
		 * The template type has to match the parameter type.
		 * @param index Index of the parameter to return
		 * @return Parameter value
		 */
		template <typename T> T parameter(const unsigned int index);

		/**
		 * Template function to changes a parameter.
		 * The template type has to match the parameter type.
		 * @param index Index of the parameter to change
		 * @param value Value of the parameter
		 */
		template <typename T> void setParameter(const unsigned int index, const T& value);

		/**
		 * Returns whether this container holds no function.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Calls the encapsulated callback function and uses the previously defined function parameters.
		 * @return Return value
		 */
		inline RT operator()() const;

		/**
		 * Assigns a callback container.
		 * @param caller Callback container to assign
		 * @return Reference to this object
		 */
		Caller& operator=(const Caller& caller);

		/**
		 * Moves a callback container.
		 * @param caller Callback container to move
		 * @return Reference to this object
		 */
		Caller& operator=(Caller&& caller);

		/**
		 * Returns whether two callback container addressing the same class object and the same local member function.
		 * @param caller Right callback to check
		 * @return True, if so
		 */
		inline bool operator==(const Caller& caller) const;

		/**
		 * Returns whether two callback container addressing not the same class object or not the same local member function.
		 * @param caller Right callback to check
		 * @return True, if so
		 */
		inline bool operator!=(const Caller& caller) const;

		/**
		 * Returns whether this container holds an encapsulated function.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Creates a new caller object by a given base parameterized function object.
		 * @param base New base function
		 */
		inline explicit Caller(ParameterizedFunctionBase<RT>* base);

	private:

		/// Base function container.
		ParameterizedFunctionBase<RT>* callerFunction;
};

template <typename RT>
inline Caller<RT>::Caller() :
	callerFunction(nullptr)
{
	// nothing to do here
}

template <typename RT>
inline Caller<RT>::Caller(const Caller& caller) :
	callerFunction(caller.callerFunction ? caller.callerFunction->copy() : nullptr)
{
	// nothing to do here
}

template <typename RT>
inline Caller<RT>::Caller(Caller&& caller) :
	callerFunction(caller.callerFunction)
{
	caller.callerFunction = nullptr;
}

template <typename RT>
inline Caller<RT>::Caller(ParameterizedFunctionBase<RT>* base) :
	callerFunction(base)
{
	// nothing to do here
}

template <typename RT>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function));
}

template <typename RT>
template <typename PT0>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0));
}

template <typename RT>
template <typename PT0, typename PT1>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18));
}

template <typename RT>
template <typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Caller<RT> Caller<RT>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	return Caller<RT>(new ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19));
}

template <typename RT>
template <typename CT>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function));
}

template <typename RT>
template <typename CT>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function));
}

template <typename RT>
template <typename CT, typename PT0>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0));
}

template <typename RT>
template <typename CT, typename PT0>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Caller<RT> Caller<RT>::create(CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	return Caller<RT>(new ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19));
}

template <typename RT>
template <typename CT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Caller<RT> Caller<RT>::create(const CT& object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::TypeConst function, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	return Caller<RT>(new ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(object, function, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19));
}

template <typename RT>
inline Caller<RT>::~Caller()
{
	delete callerFunction;
}

template <typename RT>
unsigned int Caller<RT>::parameters() const
{
	ocean_assert(callerFunction != nullptr);
	if (callerFunction == nullptr)
	{
		return 0u;
	}

	return callerFunction->parameters();
}

template <typename RT>
template <typename T>
T Caller<RT>::parameter(const unsigned int index)
{
	ocean_assert(callerFunction != nullptr);
	
#ifdef OCEAN_SUPPORT_RTTI
	ocean_assert(callerFunction->type(index) == typeid(T) && "The parameter has a different type.");
#endif

	return *reinterpret_cast<T*>(callerFunction->parameter(index));
}

template <typename RT>
template <typename T>
void Caller<RT>::setParameter(const unsigned int index, const T& value)
{
	ocean_assert(callerFunction != nullptr);
	
#ifdef OCEAN_SUPPORT_RTTI
	ocean_assert(callerFunction->type(index) == typeid(T) && "The parameter has a different type.");
#endif

	callerFunction->setParameter(index, &value);
}

template <typename RT>
inline bool Caller<RT>::isNull() const
{
	return callerFunction == nullptr;
}

template <typename RT>
inline RT Caller<RT>::operator()() const
{
	ocean_assert(callerFunction != nullptr);

	return callerFunction->call();
}

template <typename RT>
Caller<RT>& Caller<RT>::operator=(const Caller& caller)
{
	delete callerFunction;
	callerFunction = caller.callerFunction ? caller.callerFunction->copy() : nullptr;

	return *this;
}

template <typename RT>
Caller<RT>& Caller<RT>::operator=(Caller<RT>&& caller)
{
	if (this != &caller)
	{
		delete callerFunction;

		callerFunction = caller.callerFunction;
		caller.callerFunction = nullptr;
	}

	return *this;
}

template <typename RT>
inline bool Caller<RT>::operator==(const Caller& caller) const
{
	if (callerFunction == nullptr && caller.callerFunction == nullptr)
	{
		return true;
	}

	if ((callerFunction != nullptr && caller.callerFunction == nullptr)
			|| (callerFunction == nullptr && caller.callerFunction != nullptr))
	{
		return false;
	}

	return *callerFunction == *caller.callerFunction;
}

template <typename RT>
inline bool Caller<RT>::operator!=(const Caller& caller) const
{
	return !(*this == caller);
}

template <typename RT>
inline Caller<RT>::operator bool() const
{
	return callerFunction != nullptr;
}

}

#endif // META_OCEAN_BASE_CALLER_H
