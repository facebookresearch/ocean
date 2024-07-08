/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSConfigMedium.h"
#include "ocean/media/directshow/DSObject.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSConfigMedium::DSConfigMedium(const std::string& url) :
	Medium(url),
	DSMedium(url),
	ConfigMedium(url)
{
	// nothing to do here
}

bool DSConfigMedium::showPropertyDialog(IUnknown* object, HWND owner, bool checkOnly)
{
	if (object == nullptr)
	{
		return false;
	}

	bool noError = false;

	ScopeDirectShowObject<ISpecifyPropertyPages> propertyPage;
	if (S_OK == object->QueryInterface(IID_ISpecifyPropertyPages, (void **)(&propertyPage.resetObject())))
	{
		CAUUID cauuid;
		if (S_OK == propertyPage->GetPages(&cauuid))
		{
			if (checkOnly == false)
			{
				OleCreatePropertyFrame(owner, 30, 30, nullptr, 1, (IUnknown**)&object, cauuid.cElems, (GUID*)cauuid.pElems, 0, 0, nullptr);
			}

			noError = true;
			CoTaskMemFree(cauuid.pElems);
		}
	}

	return noError;
}

}

}

}
