/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_MEDIA_TYPE_H
#define META_OCEAN_MEDIA_DS_MEDIA_TYPE_H

#include "ocean/media/directshow/DirectShow.h"

DISABLE_WARNINGS_BEGIN
	#include <Dshow.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class wraps the DirectShow AM_MEDIA_TYPE struct;
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSMediaType final
{
	public:

		DSMediaType() = default;

		~DSMediaType();

		explicit DSMediaType(const AM_MEDIA_TYPE& type);

		DSMediaType(DSMediaType&& dsMediaType);

		inline const AM_MEDIA_TYPE& type() const;

		inline AM_MEDIA_TYPE& type();

		inline AM_MEDIA_TYPE& reset();

		DSMediaType& operator=(DSMediaType&& dsMediaType);

	protected:

		void release();

		DSMediaType(const DSMediaType&) = delete;
		DSMediaType& operator=(const DSMediaType&) = delete;

	protected:

		AM_MEDIA_TYPE type_ = {};
};

const AM_MEDIA_TYPE& DSMediaType::type() const
{
	return type_;
}

AM_MEDIA_TYPE& DSMediaType::type()
{
	return type_;
}

inline AM_MEDIA_TYPE& DSMediaType::reset()
{
	release();

	return type_;
}

}

}

}

#endif // META_OCEAN_MEDIA_DS_MEDIA_TYPE_H
