/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/oculustags/OculusTags.h"

#include "ocean/tracking/oculustags/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

OculusTag::OculusTag() :
	tagID_(uint32_t(-1)),
	reflectanceType_(OculusTag::RT_REFLECTANCE_UNDEFINED),
	tagSize_(-1)
{
	ocean_assert(isValid() == false);
}

OculusTag::OculusTag(const uint32_t& tagID, const ReflectanceType& reflectanceType, const uint8_t& intensityThreshold, const HomogenousMatrix4& world_T_tag, const Scalar tagSize) :
	tagID_(tagID),
	reflectanceType_(reflectanceType),
	intensityThreshold_(intensityThreshold),
	world_T_tag_(world_T_tag),
	tagSize_(tagSize)
{
	DataMatrix dataMatrix = 0u;

	if (!Utilities::encode(tagID_, dataMatrix))
	{
		ocean_assert(false && "This should never happen!");
	}
}

bool OculusTag::isValid() const
{
	return tagID() < 1024u
		&& reflectanceType() != RT_REFLECTANCE_UNDEFINED
		&& world_T_tag().isValid()
		&& tagSize() > 0;
}

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean
