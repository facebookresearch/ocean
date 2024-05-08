/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_PERFORMANCE_H
#define META_OCEAN_SYSTEM_PERFORMANCE_H

#include "ocean/system/System.h"

#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace System
{

/**
 * This class implements functionalities concerning the underlying system performance.
 * @ingroup system
 */
class OCEAN_SYSTEM_EXPORT Performance : public Singleton<Performance>
{
	friend class Singleton<Performance>;

	public:

		/**
		 * Definition of different system performance levels.
		 */
		enum PerformanceLevel : uint32_t
		{
			/// Performance level for e.g. mobile phones
			LEVEL_LOW,
			/// Performance level for e.g. mobile computers
			LEVEL_MEDIUM,
			/// Performance level for e.g. desktop computers
			LEVEL_HIGH,
			/// Performance level for e.g. workstations
			LEVEL_VERY_HIGH,
			/// Performance level for e.g. high performance workstations
			LEVEL_ULTRA
		};

	public:

		/**
		 * Returns the current performance level.
		 * @return Current selected performance level
		 */
		PerformanceLevel performanceLevel();

		/**
		 * Sets the current performance level.
		 * @param level New performance level
		 */
		void setPerformanceLevel(const PerformanceLevel level);

	protected:

		/**
		 * Creates a new Performance object.<br>
		 * The default medium performance depends on the number of available processor cores.<br>
		 * A system with one processor core has performance value 'medium' as default.<br>
		 * Systems with two processor cores have performance value 'high' as default.<br>
		 * And systems with four or more processor cores have performance value 'ultra' as default.<br>
		 * However the performance value can be changed at any time.
		 */
		Performance();

	protected:

		/// Current performance level.
		PerformanceLevel performanceLevel_ = LEVEL_MEDIUM;
};

}

}

#endif // META_OCEAN_SYSTEM_PERFORMANCE_H
