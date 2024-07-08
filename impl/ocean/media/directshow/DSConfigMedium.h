/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_DS_CONFIG_MEDIUM_H
#define META_OCEAN_MEDIA_DS_CONFIG_MEDIUM_H

#include "ocean/media/directshow/DirectShow.h"
#include "ocean/media/directshow/DSMedium.h"

#include "ocean/media/ConfigMedium.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

/**
 * This class is the base class for all DirectShow mediums with advanced configuration possibilities.
 * @ingroup mediads
 */
class OCEAN_MEDIA_DS_EXPORT DSConfigMedium :
	public virtual DSMedium,
	public virtual ConfigMedium
{
	protected:

		/**
		 * Creates a new DSConfigMedium object by a given url.
		 * @param url Url of medium
		 */
		explicit DSConfigMedium(const std::string& url);

		/**
		 * Shows a property dialog or checks whether the dialog exists.
		 * @param object DirectShow filter or interface holding the property dialog
		 * @param owner Windows handle of the dialog owner
		 * @param checkOnly Determines whether the availability check is enough.
		 * @return True, if succeeded
		 */
		static bool showPropertyDialog(IUnknown* object, HWND owner, bool checkOnly);
};

}

}

}

#endif // META_OCEAN_MEDIA_DS_CONFIG_MEDIUM_H
