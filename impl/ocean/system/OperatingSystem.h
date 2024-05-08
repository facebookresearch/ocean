/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_OPERATING_SYSTEM_H
#define META_OCEAN_SYSTEM_OPERATING_SYSTEM_H

#include "ocean/system/System.h"

namespace Ocean
{

namespace System
{

/**
 * This class implements functions for the operating system.
 * @ingroup system
 */
class OCEAN_SYSTEM_EXPORT OperatingSystem
{
	public:

		/**
		 * Definition of individual operating systems.
		 */
		enum OperatingSystemId
		{
			/// Unknown operating system.
			OSID_UNKNOWN,

			/// Windows operating system without specific version.
			OSID_MICROSOFT_WINDOWS = 0x00001000,
			/// Windows client operating system without specific version.
			OSID_MICROSOFT_WINDOWS_CLIENT = OSID_MICROSOFT_WINDOWS | 0x00000001,
			/// Windows server operating system without specific version.
			OSID_MICROSOFT_WINDOWS_SERVER = OSID_MICROSOFT_WINDOWS | 0x00000002,

			/// Windows 2000 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_2000 = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000020,
			/// Windows XP with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_XP = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000030,
			/// Windows Vista with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_VISTA = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000050,
			/// Windows 7 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_7 = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000070,
			/// Windows 8 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_8 = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000080,
			/// Windows 8.1 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_8_1 = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000090,
			/// Windows 10 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_10 = OSID_MICROSOFT_WINDOWS_CLIENT | 0x00000100,

			/// Windows Server 2003 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_SERVER_2003 = OSID_MICROSOFT_WINDOWS_SERVER | 0x00000030,
			/// Windows Server 2008 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_SERVER_2008 = OSID_MICROSOFT_WINDOWS_SERVER | 0x00000050,
			/// Windows Server 2012 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_SERVER_2012 = OSID_MICROSOFT_WINDOWS_SERVER | 0x00000090,
			/// Windows Server 2016 with any kind of service pack.
			OSID_MICROSOFT_WINDOWS_SERVER_2016 = OSID_MICROSOFT_WINDOWS_SERVER | 0x00000100,

			/// Macintosh operating system without specific version.
			OSID_MACINTOSH_OS = 0x00010000,
			/// Macintosh OS X with any kind of service pack.
			OSID_MACINTOSH_OS_X = OSID_MACINTOSH_OS | 0x00000001,

			/// Android operating system without specific version.
			OSID_ANDROID = 0x00100000,

			// Linux without any specific version.
			OSID_LINUX = 0x01000000,
		};


	public:

		/**
		 * Returns the id of the current operating system.
		 * @param version Optional resulting (detailed) version of the operating system
		 * @return The OS's id
		 */
		static OperatingSystemId id(std::wstring* version = nullptr);

		/**
		 * Returns the name of the current operating system.
		 * @param addVersion True, to add the version of the operating system
		 * @return The OS's name
		 */
		static std::wstring name(const bool addVersion = true);
};

}

}

#endif // META_OCEAN_SYSTEM_OPERATING_SYSTEM_H
