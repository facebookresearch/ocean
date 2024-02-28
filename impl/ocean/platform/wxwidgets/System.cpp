// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/wxwidgets/System.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "ocean/base/String.h"

#include <wx/stdpaths.h>

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

IO::Directory System::currentDirectory()
{
	return IO::Directory(Utilities::toAString(wxGetCwd()));
}

IO::Directory System::processDirectory()
{
	const std::wstring path(wxStandardPaths::Get().GetExecutablePath());

	const IO::File applicationFile(String::toAString(path));
	return IO::Directory(applicationFile);
}

std::string System::environmentVariable(const std::string& variable, const bool removeQuotes)
{
	wxString wxValue;

	if (!wxGetEnv(String::toWString(variable), &wxValue))
	{
#ifdef __APPLE__
		// **TODO** **HACK** this is a workaround for macOS 10.10 and up do not support environment variables anymore
		if (variable == "OCEAN_DEVELOPMENT_PATH")
		{
			return std::string("/fayteq/trunk");
		}
#endif

		return std::string();
	}

	std::string value(Utilities::toAString(wxValue));

	if (removeQuotes)
		while (value.length() != 0)
		{
			if (value[0] == '\"')
			{
				value = value.substr(1);
			}
			else if (value[value.length() - 1] == '\"')
			{
				value = value.substr(0, value.length() - 1);
			}
			else
			{
				break;
			}
		}

	return value;
}

}

}

}
