// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/Address4.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Network
{

std::string Address4::readable() const
{
	const unsigned int sub0 = (unsigned int)(address_ & 0xFFu);
	const unsigned int sub1 = (unsigned int)((address_ >> 8u) & 0xFFu);
	const unsigned int sub2 = (unsigned int)((address_ >> 16u) & 0xFFu);
	const unsigned int sub3 = (unsigned int)((address_ >> 24u) & 0xFFu);

	return String::toAString(sub0) + std::string(".") + String::toAString(sub1) + std::string(".")
			+ String::toAString(sub2) + std::string(".") + String::toAString(sub3);
}

}

}
