/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_OPERATOR_H
#define META_OCEAN_CV_SYNTHESIS_OPERATOR_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Layer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class is the base class for all image synthesis operators.
 * @ingroup cvsynthesis
 */
class Operator
{
	public:

		/**
		 * Destructs the object.
		 */
		virtual ~Operator() = default;

	protected:

		/**
		 * Creates a new operator object.
		 */
		inline Operator();

		/**
		 * Disabled copy constructor.
		 * @param object The object which would be copied
		 */
		Operator(const Operator& object) = delete;

		/**
		 * Disabled copy operator.
		 * @param object The object which would be copied
		 * @return Reference to this object
		 */
		Operator& operator=(const Operator& object) = delete;
};

inline Operator::Operator()
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_OPERATOR_H
