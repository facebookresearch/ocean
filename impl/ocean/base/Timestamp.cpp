/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Timestamp.h"
#include "ocean/base/DateTime.h"

namespace Ocean
{

Timestamp::Timestamp(const bool toNow) :
	value_(toNow ? DateTime::timestamp() : invalidTimestampValue())
{
	// nothing to do here
}

Timestamp& Timestamp::toNow()
{
	value_ = DateTime::timestamp();

	return *this;
}

}
