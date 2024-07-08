/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/fonts/freetype/Utilities.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

std::string Utilities::translateErrorCode(const FT_Error error)
{
	switch (error)
	{
		case FT_Err_Ok:
			return "No error";

		case FT_Err_Cannot_Open_Resource:
			return "Could not open resource";

		case FT_Err_Unknown_File_Format:
			return "Unnown file format";

		case FT_Err_Invalid_File_Format:
			return "Invalid file format";

		case FT_Err_Invalid_Version:
			return "Invalid version";

		default:
			return "Unknown error";
	}
}

}

}

}
