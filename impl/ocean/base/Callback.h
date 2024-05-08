/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_CALLBACK_H
#define META_OCEAN_BASE_CALLBACK_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

#include <vector>

namespace Ocean
{

/**
 * Definition of a class holding no objects.
 * This class can do nothing, but can be used as default class types.
 * @ingroup base
 */
typedef class {} NullClass;

// Forward declaration.
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class Callback;

// Forward declaration.
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class MemberFunction;

// Forward declaration.
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class StaticFunction;

// Forward declaration.
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class MemberFunctionCaller;

// Forward declaration.
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class StaticFunctionCaller;

// Forward declaration.
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class ParameterizedMemberFunction;

// Forward declaration.
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class ParameterizedMemberFunctionConst;

// Forward declaration.
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19> class ParameterizedStaticFunction;

// Forward declaration.
template <typename RT> class Caller;

/**
 * This class implements a helper class providing a definition of a member function pointer only.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's owner class type (CT), the return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * @tparam CT Specifies the type of the member object
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2nd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @ingroup base
 */
template <typename CT, typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class MemberFunctionPointerMaker
{
	public:

		/**
		 * Definition of a function pointer with twenty parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19);

		/**
		 * Definition of a const function pointer with twenty parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18>
{
	public:

		/**
		 * Definition of a function pointer with nineteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18);

		/**
		 * Definition of a const function pointer with nineteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17>
{
	public:

		/**
		 * Definition of a function pointer with eighteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17);

		/**
		 * Definition of a const function pointer with eighteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16>
{
	public:

		/**
		 * Definition of a function pointer with seventeen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16);

		/**
		 * Definition of a const function pointer with seventeen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15>
{
	public:

		/**
		 * Definition of a function pointer with sixteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15);

		/**
		 * Definition of a const function pointer with sixteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14>
{
	public:

		/**
		 * Definition of a function pointer with fifteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14);

		/**
		 * Definition of a const function pointer with fifteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13>
{
	public:

		/**
		 * Definition of a function pointer with fourteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13);

		/**
		 * Definition of a const function pointer with fourteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12>
{
	public:

		/**
		 * Definition of a function pointer with thirteen parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12);

		/**
		 * Definition of a const function pointer with thirteen parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11>
{
	public:

		/**
		 * Definition of a function pointer with twelve parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11);

		/**
		 * Definition of a const function pointer with twelve parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10>
{
	public:

		/**
		 * Definition of a function pointer with eleven parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10);

		/**
		 * Definition of a const function pointer with eleven parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9>
{
	public:

		/**
		 * Definition of a function pointer with ten parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9);

		/**
		 * Definition of a const function pointer with ten parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with nine parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8);

		/**
		 * Definition of a const function pointer with nine parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with eight parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7);

		/**
		 * Definition of a const function pointer with eight parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with seven parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6);

		/**
		 * Definition of a const function pointer with seven parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5, PT6) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with six parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4, PT5);

		/**
		 * Definition of a const function pointer with six parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4, PT5) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with five parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3, PT4);

		/**
		 * Definition of a const function pointer with five parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3, PT4) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with four parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2, PT3);

		/**
		 * Definition of a const function pointer with four parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2, PT3) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with three parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1, PT2);

		/**
		 * Definition of a const function pointer with three parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1, PT2) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0, typename PT1>
class MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with two parameters.
		 */
		typedef RT (CT::*Type)(PT0, PT1);

		/**
		 * Definition of a const function pointer with two parameters.
		 */
		typedef RT (CT::*TypeConst)(PT0, PT1) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT, typename PT0>
class MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with one parameter.
		 */
		typedef RT (CT::*Type)(PT0);

		/**
		 * Definition of a const function pointer with one parameter.
		 */
		typedef RT (CT::*TypeConst)(PT0) const;
};

/**
 * This class is a specialization of the original helper class providing a definition of a member function pointer only.
 * @see MemberFunctionPointerMaker
 */
template <typename CT, typename RT>
class MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with no parameter.
		 */
		typedef RT (CT::*Type)();

		/**
		 * Definition of a const function pointer with no parameter.
		 */
		typedef RT (CT::*TypeConst)() const;
};

/**
 * This class is a helper class providing a definition of a static function pointer only.
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class StaticFunctionPointerMaker
{
	public:

		/**
		 * Definition of a function pointer with twenty parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with nineteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with eighteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with seventeen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with sixteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with fifteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with fourteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with thirteen parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with twelve parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with elven parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with ten parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with nine parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with eight parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with seven parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5, PT6);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with six parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4, PT5);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with five parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3, PT4);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with four parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2, PT3);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1, typename PT2>
class StaticFunctionPointerMaker<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with three parameters.
		 */
		typedef RT (*Type)(PT0, PT1, PT2);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0, typename PT1>
class StaticFunctionPointerMaker<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with two parameters.
		 */
		typedef RT (*Type)(PT0, PT1);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT, typename PT0>
class StaticFunctionPointerMaker<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with one parameter.
		 */
		typedef RT (*Type)(PT0);
};

/**
 * This class is a specialization of the original helper class providing a definition of a static function pointer only.
 * @see StaticFunctionPointerMaker
 */
template <typename RT>
class StaticFunctionPointerMaker<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	public:

		/**
		 * Definition of a function pointer with no parameter.
		 */
		typedef RT (*Type)();
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @tparam CT Specifies the type of the member object
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @ingroup base
 */
template <typename CT, typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class MemberFunctionCaller
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
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
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
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
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @param param18 19th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @param param17 18th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @param param16 17th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @param param15 16th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @param param10 11th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 10th function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1, typename PT2>
class MemberFunctionCaller<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0, typename PT1>
class MemberFunctionCaller<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT, typename PT0>
class MemberFunctionCaller<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see MemberFunctionCaller.
 */
template <typename CT, typename RT>
class MemberFunctionCaller<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class MemberFunction<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunction<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedMemberFunctionConst<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @return Return value
		 */
		static inline RT call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);

		/**
		 * Calls a const function defined by the function pointer the class object and the function parameters.
		 * @param functionClass Class object holding the member function to call
		 * @param functionPointer Pointer of the member function to call
		 * @return Return value
		 */
		static inline RT call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7, param8);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6, param7);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5, param6);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4, param5);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3, param4);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2, param3);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1, param2);
}

template <typename CT, typename RT, typename PT0, typename PT1>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1);
}

template <typename CT, typename RT, typename PT0, typename PT1>
inline RT MemberFunctionCaller<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0, param1);
}

template <typename CT, typename RT, typename PT0>
inline RT MemberFunctionCaller<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0);
}

template <typename CT, typename RT, typename PT0>
inline RT MemberFunctionCaller<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)(param0);
}

template <typename CT, typename RT>
inline RT MemberFunctionCaller<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)();
}

template <typename CT, typename RT>
inline RT MemberFunctionCaller<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(const CT* functionClass, typename MemberFunctionPointerMaker<CT, RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::TypeConst functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionClass && functionPointer);
	return (functionClass->*functionPointer)();
}

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class StaticFunctionCaller
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
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
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19);
};


/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 19 possible function parameters (PT0 - PT18).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
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
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 18 possible function parameters (PT0 - PT17).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
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
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 17 possible function parameters (PT0 - PT16).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
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
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 16 possible function parameters (PT0 - PT15).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
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
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 15 possible function parameters (PT0 - PT14).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @param param14 15th function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 14 possible function parameters (PT0 - PT13).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @param param13 14th function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 13 possible function parameters (PT0 - PT12).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @param param12 13th function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 12 possible function parameters (PT0 - PT11).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @param param11 12th function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 11 possible function parameters (PT0 - PT10).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @param param10 11th function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a static function defined by a function pointer.
 * Because of being a helper class there is no need to use it directly.<br>
 * The template parameters define the function's return type (RT) and 10 possible function parameters (PT0 - PT9).<br>
 * Therefore, functions with at most eight function parameters are supported.<br>
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Six function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @param param9 Tenth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @param param8 Ninth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};


/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eighth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};


/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
class StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1, typename PT2>
class StaticFunctionCaller<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0, typename PT1>
class StaticFunctionCaller<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT, typename PT0>
class StaticFunctionCaller<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @param param0 First function parameter
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

/**
 * This class implements a helper class providing a functionality to call a member function defined by the owner class and a local function pointer.
 * @see StaticFunctionCaller.
 */
template <typename RT>
class StaticFunctionCaller<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>
{
	friend class StaticFunction<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;
	friend class ParameterizedStaticFunction<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>;

	protected:

		/**
		 * Calls a function defined by the function pointer the class object and the function parameters.
		 * @param functionPointer Pointer of the member function to call
		 * @return Return value
		 */
		static inline RT call(typename StaticFunctionPointerMaker<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass);
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7, param8);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6, param7);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5, param6);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4, param5);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3, param4);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, PT3 param3, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2, param3);
}

template <typename RT, typename PT0, typename PT1, typename PT2>
inline RT StaticFunctionCaller<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, PT2 param2, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1, param2);
}

template <typename RT, typename PT0, typename PT1>
inline RT StaticFunctionCaller<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, PT1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, PT1 param1, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0, param1);
}

template <typename RT, typename PT0>
inline RT StaticFunctionCaller<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, PT0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, PT0 param0, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer(param0);
}

template <typename RT>
inline RT StaticFunctionCaller<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::call(typename StaticFunctionPointerMaker<RT, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass>::Type functionPointer, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass, NullClass)
{
	ocean_assert(functionPointer);
	return functionPointer();
}

/**
 * This class is the base class for an unparameterized function container.
 * This base function container holds no function parameters and therefore can call the encapsulated function with explicit parameters only.<br>
 * The template parameters define the function's return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class FunctionBase
{
	public:

		/**
		 * Destructs the object.
		 */
		virtual ~FunctionBase();

		/**
		 * Calls the internal encapsulated function by given parameters.
		 * @param param0 First parameter
		 * @param param1 Second parameter
		 * @param param2 Third parameter
		 * @param param3 Fourth parameter
		 * @param param4 Fifth parameter
		 * @param param5 Sixth parameter
		 * @param param6 Seventh parameter
		 * @param param7 Eighth parameter
		 * @param param8 Ninth parameter
		 * @param param9 Tenth parameter
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
		 * @return Return value
		 */
		virtual RT call(PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19()) const = 0;

		/**
		 * Creates a copy of the entire derived object.
		 * @return Instance copy
		 */
		virtual FunctionBase* copy() const = 0;

		/**
		 * Returns whether two functions are equal.
		 * @param functionBase Right callback to check
		 * @return True, if so
		 */
		virtual bool operator==(const FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& functionBase) const = 0;
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::~FunctionBase()
{
	// nothing to do here
}

/**
 * This class implements an unparameterized member function container.
 * This function container holds no function parameters and therefore can call the encapsulated function with explicit parameters only.
 * The template parameters define the function's owner class type (CT), the return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * This class is a helper class only and used to allow callback functions to be independent from the member function's class type.
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @see StaticFunction, Callback.
 * @ingroup base
 */
template <typename CT, typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class MemberFunction : public FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>
{
	friend class Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;

	protected:

		/**
		 * Definition of a function pointer for the encapsulated function.
		 */
		typedef typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type FunctionType;

	protected:

		/**
		 * Creates a new member function object.
		 * @param object Class object holding the member function
		 * @param function Local member function pointer
		 */
		inline MemberFunction(CT& object, FunctionType function);

		/**
		 * Calls the encapsulated function.
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
		 * @return Return value
		 */
		virtual RT call(PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19()) const;

		/**
		 * Creates a copy of this element.
		 * @return Instance copy
		 */
		virtual FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* copy() const;

		/**
		 * Returns whether two functions are equal.
		 * @param functionBase Right callback to check
		 * @return True, if so
		 */
		virtual bool operator==(const FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& functionBase) const;

	protected:

		/// Class object holding the member function.
		CT* functionClass;

		/// Function pointer of the member function.
		FunctionType functionPointer;
};

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::MemberFunction(CT &object, FunctionType function) :
	functionClass(&object),
	functionPointer(function)
{
	// nothing to do here
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
RT MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != 0);
	return MemberFunctionCaller<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(functionClass, functionPointer, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::copy() const
{
	ocean_assert(functionClass != nullptr);
	ocean_assert(functionPointer != 0);
	return new MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(*functionClass, functionPointer);
}

template <typename CT, typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
bool MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator==(const FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& functionBase) const
{
	const MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* function = dynamic_cast< const MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* >(&functionBase);
	return function != nullptr && (functionClass == function->functionClass && functionPointer == function->functionPointer);
}

/**
 * This class implements an unparameterized static function container.
 * This function container holds no function parameters and therefore can call the encapsulated function with explicit parameters only.
 * The template parameters define the function's return type (RT) and twenty possible function parameters (PT0 - PT19).<br>
 * This class is a helper class only and used to allow callback functions to be independent from the member function's class type.
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @see MemberFunction, Callback.
 * @ingroup base
 */
template < typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class StaticFunction : public FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>
{
	friend class Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>;

	protected:

		/**
		 * Definition of a function pointer for the encapsulated function.
		 */
		typedef typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type FunctionType;

	protected:

		/**
		 * Creates a new member function object.
		 * @param function Local member function pointer
		 */
		inline StaticFunction(FunctionType function);

		/**
		 * Calls the encapsulated function.
		 * @param param0 First function parameter
		 * @param param1 Second function parameter
		 * @param param2 Third function parameter
		 * @param param3 Fourth function parameter
		 * @param param4 Fifth function parameter
		 * @param param5 Sixth function parameter
		 * @param param6 Seventh function parameter
		 * @param param7 Eight function parameter
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
		 * @return Return value
		 */
		virtual RT call(PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19()) const;

		/**
		 * Creates a copy of this element.
		 * @return Instance copy
		 */
		virtual FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* copy() const;

		/**
		 * Returns whether two functions are equal.
		 * @param functionBase Right callback to check
		 * @return True, if so
		 */
		virtual bool operator==(const FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& functionBase) const;

	protected:

		/// Function pointer of the member function.
		FunctionType functionPointer;
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::StaticFunction(FunctionType function) :
	functionPointer(function)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
RT StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) const
{
	ocean_assert(functionPointer != nullptr);
	return StaticFunctionCaller<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::call(functionPointer, param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::copy() const
{
	ocean_assert(functionPointer != nullptr);
	return new StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(functionPointer);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
bool StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator==(const FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& functionBase) const
{
	const StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* function = dynamic_cast< const StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* >(&functionBase);
	return function != nullptr && functionPointer == function->functionPointer;
}

/**
 * This class implements a container for callback functions.
 * Use this callback container to encapsulate callback functions to static functions or member functions (of classes).<br>
 * For member functions, this class is independent from the function's owner (class) and therefore can be stored without the definition of the class type.<br>
 * This template class holds several template parameters, all specifying return and function parameter types.<br>
 * @tparam RT Specifies the return type of the callback function
 * @tparam PT0 Specifies the 1st possible function parameter type
 * @tparam PT1 Specifies the 2rd possible function parameter type
 * @tparam PT2 Specifies the 3rd possible function parameter type
 * @tparam PT3 Specifies the 4th possible function parameter type
 * @tparam PT4 Specifies the 5th possible function parameter type
 * @tparam PT5 Specifies the 6th possible function parameter type
 * @tparam PT6 Specifies the 7th possible function parameter type
 * @tparam PT7 Specifies the 8th possible function parameter type
 * @tparam PT8 Specifies the 9th possible function parameter type
 * @tparam PT9 Specifies the 10th possible function parameter type
 * @tparam PT10 Specifies the 11th possible function parameter type
 * @tparam PT11 Specifies the 12th possible function parameter type
 * @tparam PT12 Specifies the 13th possible function parameter type
 * @tparam PT13 Specifies the 14th possible function parameter type
 * @tparam PT14 Specifies the 15th possible function parameter type
 * @tparam PT15 Specifies the 16th possible function parameter type
 * @tparam PT16 Specifies the 17th possible function parameter type
 * @tparam PT17 Specifies the 18th possible function parameter type
 * @tparam PT18 Specifies the 19th possible function parameter type
 * @tparam PT19 Specifies the 20th possible function parameter type
 * @see Callbacks, ConcurrentCallbacks.
 *
 * See the following code tutorial:<br>
 * @code
 * // The best way to use the callback function container is to combine it with a typedef.
 *
 * #include "ocean/base/Callback.h"
 *
 * // Some class with a member function
 * class TestClass
 * {
 *     public:
 *
 *         // First member function later handled by the callback
 *         void firstMemberFunction();
 *
 *         // Second member function later handled by the callback
 *         bool secondMemberFunction(int param0, const unsigned char* param1);
 *
 *         // Static function later handled by the callback
 *         static std::string staticFunction(short param);
 * }
 *
 * static void globalFunction()
 * {
 *     // do anything
 * }
 *
 * // Definition of a callback function with not return value and no function parameter
 * typedef Callback<void> FirstCallback;
 *
 * // Definition of a callback function with 'bool' as return type and 'int' and 'unsigned char*' as function parameter
 * typedef Callback<bool, int, const unsigned char*> SecondCallback;
 *
 * // Definition of a callback function with 'std::string' as return type and 'short' as function parameter
 * typedef Callback<std::string, short> ThirdCallback;
 *
 * // Any function using the callback
 * void main()
 * {
 *     // Instance of the test class
 *     TestClass testClassObject;
 *
 *     // Instance of the first callback function to the member function 'firstMemberFunction' of the above created instance of TestClass
 *     FirstCallback firstCallback(testClassObject, &TestClass::firstMemberFunction);
 *
 *     // Instance of the second callback function to the member function 'secondMemberFunction' of the above created instance of TestClass
 *     SecondCallback secondCallback(testClassObject, &TestClass::secondMemberFunction);
 *
 *     // Instance of the third callback function to the static function 'staticFunction' of the above created instance of TestClass
 *     ThirdCallback thirdCallback(&TestClass::staticFunction);
 *
 *     // Instance of the fourth callback function to the global function
 *     FirstCallback fourthCallback(&globalFunction);
 *
 *     // Usage of the first callback
 *     firstCallback();
 *
 *     // Usage of the second callback
 *     bool resultBool = secondCallback(5, "test");
 *
 *     // Usage of the third callback
 *     std::string resultString = thirdCallback(3);
 *
 *     // Usage of the fourth callback
 *     fourthCallback();
 * }
 * @endcode
 *
 * @ingroup base
 */
template <typename RT, typename PT0 = NullClass, typename PT1 = NullClass, typename PT2 = NullClass, typename PT3 = NullClass, typename PT4 = NullClass, typename PT5 = NullClass, typename PT6 = NullClass, typename PT7 = NullClass, typename PT8 = NullClass, typename PT9 = NullClass, typename PT10 = NullClass, typename PT11 = NullClass, typename PT12 = NullClass, typename PT13 = NullClass, typename PT14 = NullClass, typename PT15 = NullClass, typename PT16 = NullClass, typename PT17 = NullClass, typename PT18 = NullClass, typename PT19 = NullClass>
class Callback
{
	public:

		/**
		 * Definition of the return type of this callback object.
		 */
		typedef RT ReturnType;

		/**
		 * Definition of the 1st parameter type of this callback object.
		 */
		typedef PT0 ParameterType0;

		/**
		 * Definition of the 2rd parameter type of this callback object.
		 */
		typedef PT1 ParameterType1;

		/**
		 * Definition of the 3rd parameter type of this callback object.
		 */
		typedef PT2 ParameterType2;

		/**
		 * Definition of the 4th parameter type of this callback object.
		 */
		typedef PT3 ParameterType3;

		/**
		 * Definition of the 5th parameter type of this callback object.
		 */
		typedef PT4 ParameterType4;

		/**
		 * Definition of the 6th parameter type of this callback object.
		 */
		typedef PT5 ParameterType5;

		/**
		 * Definition of the 7th parameter type of this callback object.
		 */
		typedef PT6 ParameterType6;

		/**
		 * Definition of the 8th parameter type of this callback object.
		 */
		typedef PT7 ParameterType7;

		/**
		 * Definition of the 9th parameter type of this callback object.
		 */
		typedef PT8 ParameterType8;

		/**
		 * Definition of the 10th parameter type of this callback object.
		 */
		typedef PT9 ParameterType9;

		/**
		 * Definition of the 11th parameter type of this callback object.
		 */
		typedef PT10 ParameterType10;

		/**
		 * Definition of the 12th parameter type of this callback object.
		 */
		typedef PT11 ParameterType11;

		/**
		 * Definition of the 13th parameter type of this callback object.
		 */
		typedef PT12 ParameterType12;

		/**
		 * Definition of the 14th parameter type of this callback object.
		 */
		typedef PT13 ParameterType13;

		/**
		 * Definition of the 15th parameter type of this callback object.
		 */
		typedef PT14 ParameterType14;

		/**
		 * Definition of the 16th parameter type of this callback object.
		 */
		typedef PT15 ParameterType15;

		/**
		 * Definition of the 17th parameter type of this callback object.
		 */
		typedef PT16 ParameterType16;

		/**
		 * Definition of the 18th parameter type of this callback object.
		 */
		typedef PT17 ParameterType17;

		/**
		 * Definition of the 19th parameter type of this callback object.
		 */
		typedef PT18 ParameterType18;

		/**
		 * Definition of the 20th parameter type of this callback object.
		 */
		typedef PT19 ParameterType19;

	public:

		/**
		 * Creates an empty callback container.
		 */
		inline Callback();

		/**
		 * Copies a callback container.
		 * @param callback The callback container to copy
		 */
		inline Callback(const Callback& callback);

		/**
		 * Creates a new callback container for a member function.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 */
		template<typename CT> inline Callback(CT &object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function);

		/**
		 * Creates a new callback container for a static function.
		 * @param function Local function pointer of the static function
		 */
		explicit inline Callback(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function);

		/**
		 * Destructs a callback container object.
		 */
		virtual ~Callback();

		/**
		 * Creates a new callback container for a member function.
		 * @param object Class object holding the member function
		 * @param function Local function pointer of the member function
		 * @return New callback container
		 */
		template<typename CT>
		static inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19> create(CT &object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function);

		/**
		 * Creates a new callback container for a static function.
		 * @param function Local function pointer of the static function
		 * @return New callback container
		 */
		static inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19> createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function);


		/**
		 * Returns whether this container holds no function.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Calls the encapsulated callback function.
		 * @param param0 Possible first function parameter
		 * @param param1 Possible second function parameter
		 * @param param2 Possible third function parameter
		 * @param param3 Possible fourth function parameter
		 * @param param4 Possible fifth function parameter
		 * @param param5 Possible sixth function parameter
		 * @param param6 Possible seventh function parameter
		 * @param param7 Possible eighth function parameter
		 * @param param8 Possible ninth function parameter
		 * @param param9 Possible tenth function parameter
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
		 * @return Return value
		 */
		inline RT operator()(PT0 param0 = PT0(), PT1 param1 = PT1(), PT2 param2 = PT2(), PT3 param3 = PT3(), PT4 param4 = PT4(), PT5 param5 = PT5(), PT6 param6 = PT6(), PT7 param7 = PT7(), PT8 param8 = PT8(), PT9 param9 = PT9(), PT10 param10 = PT10(), PT11 param11 = PT11(), PT12 param12 = PT12(), PT13 param13 = PT13(), PT14 param14 = PT14(), PT15 param15 = PT15(), PT16 param16 = PT16(), PT17 param17 = PT17(), PT18 param18 = PT18(), PT19 param19 = PT19()) const;

		/**
		 * Assigns a callback container.
		 * @param callback The callback container to assign
		 * @return Reference to this object
		 */
		inline Callback& operator=(const Callback& callback);

		/**
		 * Returns whether two callback container addressing the same class object and the same local member function.
		 * @param callback Right callback to check
		 * @return True, if so
		 */
		inline bool operator==(const Callback& callback) const;

		/**
		 * Returns whether two callback container addressing not the same class object or not the same local member function.
		 * @param callback Right callback to check
		 * @return True, if so
		 */
		inline bool operator!=(const Callback& callback) const;

		/**
		 * Returns whether this container holds an encapsulated function.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/**
		 * Creates a new function container by a given base function.
		 * @param base The base function to create a new container form
		 */
		explicit inline Callback(FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* base);

	private:

		/// Base function container.
		FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* callbackFunction;
};

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Callback() :
	callbackFunction(nullptr)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Callback(const Callback& callback) :
	callbackFunction(callback.callbackFunction ? callback.callbackFunction->copy() : nullptr)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
template<typename CT>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Callback(CT &object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function) :
	callbackFunction(new MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(object, function))
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Callback(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function) :
	callbackFunction(new StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(function))
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Callback(FunctionBase<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>* base) :
	callbackFunction(base)
{
	// nothing to do here
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::~Callback()
{
	delete callbackFunction;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
template<typename CT>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19> Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::create(CT &object, typename MemberFunctionPointerMaker<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function)
{
	return Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(new MemberFunction<CT, RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(object, function));
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19> Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::createStatic(typename StaticFunctionPointerMaker<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::Type function)
{
	return Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(new StaticFunction<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>(function));
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline bool Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::isNull() const
{
	return callbackFunction == nullptr;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline RT Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator()(PT0 param0, PT1 param1, PT2 param2, PT3 param3, PT4 param4, PT5 param5, PT6 param6, PT7 param7, PT8 param8, PT9 param9, PT10 param10, PT11 param11, PT12 param12, PT13 param13, PT14 param14, PT15 param15, PT16 param16, PT17 param17, PT18 param18, PT19 param19) const
{
	ocean_assert(callbackFunction != nullptr);

	return callbackFunction->call(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>& Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator=(const Callback& callback)
{
	delete callbackFunction;
	callbackFunction = callback.callbackFunction ? callback.callbackFunction->copy() : nullptr;

	return *this;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline bool Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator==(const Callback& callback) const
{
	if (callbackFunction == nullptr && callback.callbackFunction == nullptr)
		return true;

	if (callbackFunction == nullptr || callback.callbackFunction == nullptr)
	{
		ocean_assert((callbackFunction != nullptr && callback.callbackFunction == nullptr) || (callbackFunction == nullptr && callback.callbackFunction != nullptr));

		return false;
	}

	return *callbackFunction == *callback.callbackFunction;
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline bool Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator!=(const Callback& callback) const
{
	return !(*this == callback);
}

template <typename RT, typename PT0, typename PT1, typename PT2, typename PT3, typename PT4, typename PT5, typename PT6, typename PT7, typename PT8, typename PT9, typename PT10, typename PT11, typename PT12, typename PT13, typename PT14, typename PT15, typename PT16, typename PT17, typename PT18, typename PT19>
inline Callback<RT, PT0, PT1, PT2, PT3, PT4, PT5, PT6, PT7, PT8, PT9, PT10, PT11, PT12, PT13, PT14, PT15, PT16, PT17, PT18, PT19>::operator bool() const
{
	return callbackFunction != nullptr;
}

/**
 * This class implements a vector holding several callbacks with the same type.
 * The class is thread-safe but does not allow to remove or add new callbacks while a callback is currently invoked (by the same thread).<br>
 * @see ConcurrentCallbacks, Callback.
 * @tparam T Type of the callbacks to be stored
 * @ingroup base
 */
template <typename T>
class Callbacks
{
	public:

		/**
		 * Definition of the return type of the callback object.
		 */
		typedef typename T::ReturnType ReturnType;

		/**
		 * Definition of the 1st function parameter of the callback object.
		 */
		typedef typename T::ParameterType0 ParameterType0;

		/**
		 * Definition of the 2nd function parameter of the callback object.
		 */
		typedef typename T::ParameterType1 ParameterType1;

		/**
		 * Definition of the 3rd function parameter of the callback object.
		 */
		typedef typename T::ParameterType2 ParameterType2;

		/**
		 * Definition of the 4th function parameter of the callback object.
		 */
		typedef typename T::ParameterType3 ParameterType3;

		/**
		 * Definition of the 5th function parameter of the callback object.
		 */
		typedef typename T::ParameterType4 ParameterType4;

		/**
		 * Definition of the 6th function parameter of the callback object.
		 */
		typedef typename T::ParameterType5 ParameterType5;

		/**
		 * Definition of the 7th function parameter of the callback object.
		 */
		typedef typename T::ParameterType6 ParameterType6;

		/**
		 * Definition of the 8th function parameter of the callback object.
		 */
		typedef typename T::ParameterType7 ParameterType7;

		/**
		 * Definition of the 9th function parameter of the callback object.
		 */
		typedef typename T::ParameterType8 ParameterType8;

		/**
		 * Definition of the 10th function parameter of the callback object.
		 */
		typedef typename T::ParameterType9 ParameterType9;

		/**
		 * Definition of the 11th function parameter of the callback object.
		 */
		typedef typename T::ParameterType10 ParameterType10;

		/**
		 * Definition of the 12th function parameter of the callback object.
		 */
		typedef typename T::ParameterType11 ParameterType11;

		/**
		 * Definition of the 13th function parameter of the callback object.
		 */
		typedef typename T::ParameterType12 ParameterType12;

		/**
		 * Definition of the 14th function parameter of the callback object.
		 */
		typedef typename T::ParameterType13 ParameterType13;

		/**
		 * Definition of the 15th function parameter of the callback object.
		 */
		typedef typename T::ParameterType14 ParameterType14;

		/**
		 * Definition of the 16th function parameter of the callback object.
		 */
		typedef typename T::ParameterType15 ParameterType15;

		/**
		 * Definition of the 17th function parameter of the callback object.
		 */
		typedef typename T::ParameterType16 ParameterType16;

		/**
		 * Definition of the 18th function parameter of the callback object.
		 */
		typedef typename T::ParameterType17 ParameterType17;

		/**
		 * Definition of the 19th function parameter of the callback object.
		 */
		typedef typename T::ParameterType18 ParameterType18;

		/**
		 * Definition of the 20th function parameter of the callback object.
		 */
		typedef typename T::ParameterType19 ParameterType19;

	protected:

		/**
		 * Definition of a vector holding callback objects.
		 */
		typedef std::vector<T> CallbackObjects;

	public:

		/**
		 * Creates a new object without any initial callback function.
		 */
		inline Callbacks();

		/**
		 * Copy constructor.
		 * @param callbacks The callbacks object to copy
		 */
		inline Callbacks(const Callbacks<T>& callbacks);

		/**
		 * Move constructor.
		 * @param callbacks The callbacks object to move
		 */
		inline Callbacks(Callbacks<T>&& callbacks) noexcept;

		/**
		 * Destructs this object.
		 */
		inline ~Callbacks();

		/**
		 * Adds a new callback object.
		 * @param callback The callback to be added
		 */
		void addCallback(const T& callback);

		/**
		 * Removes a callback object.
		 * @param callback The callback to be removed
		 */
		void removeCallback(const T& callback);

		/**
		 * Calls all encapsulated callback functions.
		 * @param param0 Possible first function parameter
		 * @param param1 Possible second function parameter
		 * @param param2 Possible third function parameter
		 * @param param3 Possible fourth function parameter
		 * @param param4 Possible fifth function parameter
		 * @param param5 Possible sixth function parameter
		 * @param param6 Possible seventh function parameter
		 * @param param7 Possible eighth function parameter
		 * @param param8 Possible ninth function parameter
		 * @param param9 Possible tenth function parameter
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
		 * @return Return value of the last callback object
		 */
		ReturnType operator()(ParameterType0 param0 = ParameterType0(), ParameterType1 param1 = ParameterType1(), ParameterType2 param2 = ParameterType2(), ParameterType3 param3 = ParameterType3(), ParameterType4 param4 = ParameterType4(), ParameterType5 param5 = ParameterType5(), ParameterType6 param6 = ParameterType6(), ParameterType7 param7 = ParameterType7(), ParameterType8 param8 = ParameterType8(), ParameterType9 param9 = ParameterType9(), ParameterType10 param10 = ParameterType10(), ParameterType11 param11 = ParameterType11(), ParameterType12 param12 = ParameterType12(), ParameterType13 param13 = ParameterType13(), ParameterType14 param14 = ParameterType14(), ParameterType15 param15 = ParameterType15(), ParameterType16 param16 = ParameterType16(), ParameterType17 param17 = ParameterType17(), ParameterType18 param18 = ParameterType18(), ParameterType19 param19 = ParameterType19()) const;

		/**
		 * Returns whether no callback is stored.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether at least one callback is stored.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Assign operator.
		 * @param callbacks The callbacks object to assign
		 */
		inline Callbacks<T>& operator=(const Callbacks<T>& callbacks);

		/**
		 * Move operator.
		 * @param callbacks The callbacks object to move
		 */
		inline Callbacks<T>& operator=(Callbacks<T>&& callbacks) noexcept;

	protected:

		/// The callback objects.
		CallbackObjects callbackObjects;

		/// Callback lock.
		mutable Lock callbackLock;
};

template <typename T>
inline Callbacks<T>::Callbacks()
{
	// nothing to do here
}

template <typename T>
inline Callbacks<T>::Callbacks(const Callbacks<T>& callbacks) :
	callbackObjects(callbacks.callbackObjects)
{
	// nothing to do here
}

template <typename T>
inline Callbacks<T>::Callbacks(Callbacks<T>&& callbacks) noexcept :
	callbackObjects(std::move(callbacks.callbackObjects))
{
	// nothing to do here
}

template <typename T>
inline Callbacks<T>::~Callbacks()
{
	ocean_assert(callbackObjects.empty());
}

template <typename T>
void Callbacks<T>::addCallback(const T& callback)
{
	const ScopedLock scopedLock(callbackLock);

#ifdef OCEAN_DEBUG
	for (typename CallbackObjects::const_iterator i = callbackObjects.begin(); i != callbackObjects.end(); ++i)
		ocean_assert(*i != callback && "The callback has been added already!");
#endif // OCEAN_DEBUG

	callbackObjects.push_back(callback);
}

template <typename T>
void Callbacks<T>::removeCallback(const T& callback)
{
	const ScopedLock scopedLock(callbackLock);

	for (typename CallbackObjects::iterator i = callbackObjects.begin(); i != callbackObjects.end(); ++i)
		if (*i == callback)
		{
			callbackObjects.erase(i);
			return;
		}

	ocean_assert(false && "Unknown callback!");
}

template <typename T>
typename Callbacks<T>::ReturnType Callbacks<T>::operator()(ParameterType0 param0, ParameterType1 param1, ParameterType2 param2, ParameterType3 param3, ParameterType4 param4, ParameterType5 param5, ParameterType6 param6, ParameterType7 param7, ParameterType8 param8, ParameterType9 param9, ParameterType10 param10, ParameterType11 param11, ParameterType12 param12, ParameterType13 param13, ParameterType14 param14, ParameterType15 param15, ParameterType16 param16, ParameterType17 param17, ParameterType18 param18, ParameterType19 param19) const
{
	const ScopedLock scopedLock(callbackLock);

	if (callbackObjects.empty())
	{
		return ReturnType();
	}

	for (int n = 0; n < int(callbackObjects.size()) - 1; ++n)
	{
		callbackObjects[n](param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
	}

	return callbackObjects.back()(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);
}

template <typename T>
inline bool Callbacks<T>::isEmpty() const
{
	const ScopedLock scopedLock(callbackLock);
	return callbackObjects.empty();
}

template <typename T>
inline Callbacks<T>& Callbacks<T>::operator=(const Callbacks<T>& callbacks)
{
	callbackObjects = callbacks.callbackObjects;

	return *this;
}

template <typename T>
inline Callbacks<T>& Callbacks<T>::operator=(Callbacks<T>&& callbacks) noexcept
{
	if (this != &callbacks)
	{
		callbackObjects = std::move(callbacks.callbackObjects);
	}

	return *this;
}

template <typename T>
inline Callbacks<T>::operator bool() const
{
	const ScopedLock scopedLock(callbackLock);
	return !callbackObjects.empty();
}

/**
 * This class implements an extended callbacks object and is also thread-safe.
 * This implementation allows to remove or add callback objects while the callbacks are invoked (by the same thread).<br>
 * @see Callbacks, Callback.
 * @tparam T Type of the callbacks to be stored
 * @ingroup base
 */
template <typename T>
class ConcurrentCallbacks : public Callbacks<T>
{
	public:

		/**
		 * Definition of the 1st function parameter of the callback object.
		 */
		typedef typename T::ParameterType0 ParameterType0;

		/**
		 * Definition of the 2nd function parameter of the callback object.
		 */
		typedef typename T::ParameterType1 ParameterType1;

		/**
		 * Definition of the 3rd function parameter of the callback object.
		 */
		typedef typename T::ParameterType2 ParameterType2;

		/**
		 * Definition of the 4th function parameter of the callback object.
		 */
		typedef typename T::ParameterType3 ParameterType3;

		/**
		 * Definition of the 5th function parameter of the callback object.
		 */
		typedef typename T::ParameterType4 ParameterType4;

		/**
		 * Definition of the 6th function parameter of the callback object.
		 */
		typedef typename T::ParameterType5 ParameterType5;

		/**
		 * Definition of the 7th function parameter of the callback object.
		 */
		typedef typename T::ParameterType6 ParameterType6;

		/**
		 * Definition of the 8th function parameter of the callback object.
		 */
		typedef typename T::ParameterType7 ParameterType7;

		/**
		 * Definition of the 9th function parameter of the callback object.
		 */
		typedef typename T::ParameterType8 ParameterType8;

		/**
		 * Definition of the 10th function parameter of the callback object.
		 */
		typedef typename T::ParameterType9 ParameterType9;

		/**
		 * Definition of the 11th function parameter of the callback object.
		 */
		typedef typename T::ParameterType10 ParameterType10;

		/**
		 * Definition of the 12th function parameter of the callback object.
		 */
		typedef typename T::ParameterType11 ParameterType11;

		/**
		 * Definition of the 13th function parameter of the callback object.
		 */
		typedef typename T::ParameterType12 ParameterType12;

		/**
		 * Definition of the 14th function parameter of the callback object.
		 */
		typedef typename T::ParameterType13 ParameterType13;

		/**
		 * Definition of the 15th function parameter of the callback object.
		 */
		typedef typename T::ParameterType14 ParameterType14;

		/**
		 * Definition of the 16th function parameter of the callback object.
		 */
		typedef typename T::ParameterType15 ParameterType15;

		/**
		 * Definition of the 17th function parameter of the callback object.
		 */
		typedef typename T::ParameterType16 ParameterType16;

		/**
		 * Definition of the 18th function parameter of the callback object.
		 */
		typedef typename T::ParameterType17 ParameterType17;

		/**
		 * Definition of the 19th function parameter of the callback object.
		 */
		typedef typename T::ParameterType18 ParameterType18;

		/**
		 * Definition of the 20th function parameter of the callback object.
		 */
		typedef typename T::ParameterType19 ParameterType19;

	public:

		/**
		 * Creates a new callbacks object.
		 */
		inline ConcurrentCallbacks();

		/**
		 * Destructs a callbacks object.
		 */
		inline ~ConcurrentCallbacks();

		/**
		 * Adds a new callback object.
		 * @param callback The callback to be added
		 */
		void addCallback(const T& callback);

		/**
		 * Removes a callback object.
		 * @param callback The callback to be removed
		 */
		void removeCallback(const T& callback);

		/**
		 * Calls all encapsulated callback functions.
		 * @param param0 Possible first function parameter
		 * @param param1 Possible second function parameter
		 * @param param2 Possible third function parameter
		 * @param param3 Possible fourth function parameter
		 * @param param4 Possible fifth function parameter
		 * @param param5 Possible sixth function parameter
		 * @param param6 Possible seventh function parameter
		 * @param param7 Possible eighth function parameter
		 * @param param8 Possible ninth function parameter
		 * @param param9 Possible tenth function parameter
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
		void operator()(ParameterType0 param0 = ParameterType0(), ParameterType1 param1 = ParameterType1(), ParameterType2 param2 = ParameterType2(), ParameterType3 param3 = ParameterType3(), ParameterType4 param4 = ParameterType4(), ParameterType5 param5 = ParameterType5(), ParameterType6 param6 = ParameterType6(), ParameterType7 param7 = ParameterType7(), ParameterType8 param8 = ParameterType8(), ParameterType9 param9 = ParameterType9(), ParameterType10 param10 = ParameterType10(), ParameterType11 param11 = ParameterType11(), ParameterType12 param12 = ParameterType12(), ParameterType13 param13 = ParameterType13(), ParameterType14 param14 = ParameterType14(), ParameterType15 param15 = ParameterType15(), ParameterType16 param16 = ParameterType16(), ParameterType17 param17 = ParameterType17(), ParameterType18 param18 = ParameterType18(), ParameterType19 param19 = ParameterType19());

	private:

		/// Intermediate callbacks to be removed.
		typename Callbacks<T>::CallbackObjects intermediateRemoveCallbacks;

		/// Intermediate callbacks to be added.
		typename Callbacks<T>::CallbackObjects intermediateAddCallbacks;

		/// State that the callbacks are currently invoked.
		mutable bool callbacksInvoked;
};

template <typename T>
inline ConcurrentCallbacks<T>::ConcurrentCallbacks() :
	callbacksInvoked(false)
{
	// nothing to do here
}

template <typename T>
inline ConcurrentCallbacks<T>::~ConcurrentCallbacks()
{
	ocean_assert(intermediateRemoveCallbacks.empty());
	ocean_assert(intermediateAddCallbacks.empty());
	ocean_assert(!callbacksInvoked);
}

template <typename T>
void ConcurrentCallbacks<T>::addCallback(const T& callback)
{
	ocean_assert(callback);

	const ScopedLock scopedLock(Callbacks<T>::callbackLock);

	if (callbacksInvoked)
	{

#ifdef OCEAN_DEBUG
	for (typename Callbacks<T>::CallbackObjects::const_iterator i = Callbacks<T>::callbackObjects.begin(); i != Callbacks<T>::callbackObjects.end(); ++i)
		ocean_assert(*i != callback && "The callback has been added already!");
	for (typename Callbacks<T>::CallbackObjects::const_iterator i = intermediateAddCallbacks.begin(); i != intermediateAddCallbacks.end(); ++i)
		ocean_assert(*i != callback && "The callback has been added already!");
#endif // OCEAN_DEBUG

		intermediateAddCallbacks.push_back(callback);
	}
	else
	{
		Callbacks<T>::addCallback(callback);
	}
}

template <typename T>
void ConcurrentCallbacks<T>::removeCallback(const T& callback)
{
	const ScopedLock scopedLock(Callbacks<T>::callbackLock);

	if (callbacksInvoked)
	{

#ifdef OCEAN_DEBUG

		bool exists = false;
		for (typename Callbacks<T>::CallbackObjects::const_iterator i = Callbacks<T>::callbackObjects.begin(); i != Callbacks<T>::callbackObjects.end(); ++i)
		{
			if (*i == callback)
			{
				exists = true;
				break;
			}
		}

		ocean_assert(exists && "The callback does not exist!");

		for (typename Callbacks<T>::CallbackObjects::const_iterator i = intermediateRemoveCallbacks.begin(); i != intermediateRemoveCallbacks.end(); ++i)
		{
			ocean_assert(*i != callback && "The callback has been removed already!");
		}

#endif // OCEAN_DEBUG

		intermediateRemoveCallbacks.push_back(callback);
	}
	else
	{
		Callbacks<T>::removeCallback(callback);
	}
}

template <typename T>
void ConcurrentCallbacks<T>::operator()(ParameterType0 param0, ParameterType1 param1, ParameterType2 param2, ParameterType3 param3, ParameterType4 param4, ParameterType5 param5, ParameterType6 param6, ParameterType7 param7, ParameterType8 param8, ParameterType9 param9, ParameterType10 param10, ParameterType11 param11, ParameterType12 param12, ParameterType13 param13, ParameterType14 param14, ParameterType15 param15, ParameterType16 param16, ParameterType17 param17, ParameterType18 param18, ParameterType19 param19)
{
	const ScopedLock scopedLock(Callbacks<T>::callbackLock);

	callbacksInvoked = true;

	Callbacks<T>::operator()(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11, param12, param13, param14, param15, param16, param17, param18, param19);

	callbacksInvoked = false;

	if (!intermediateRemoveCallbacks.empty())
	{
		for (typename Callbacks<T>::CallbackObjects::const_iterator i = intermediateRemoveCallbacks.begin(); i != intermediateRemoveCallbacks.end(); ++i)
		{
			Callbacks<T>::removeCallback(*i);
		}

		intermediateRemoveCallbacks.clear();
	}

	if (!intermediateAddCallbacks.empty())
	{
		for (typename Callbacks<T>::CallbackObjects::const_iterator i = intermediateAddCallbacks.begin(); i != intermediateAddCallbacks.end(); ++i)
		{
			Callbacks<T>::addCallback(*i);
		}

		intermediateAddCallbacks.clear();
	}
}

}

#endif // META_OCEAN_BASE_CALLBACK_H
