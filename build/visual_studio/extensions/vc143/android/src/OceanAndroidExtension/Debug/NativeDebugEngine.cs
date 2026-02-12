/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Runtime.InteropServices;

namespace OceanAndroidExtension.Debug
{

/**
 * This class implements the Native Debug Engine for C++ debugging via LLDB.
 * @ingroup oceanandroidextension
 */
[Guid(EngineGuidString)]
public class NativeDebugEngine
{
	/// The GUID string identifying this debug engine.
	public const string EngineGuidString = "E6F7A8B9-C0D1-2345-6789-ABCDEF012345";

	/// The GUID identifying this debug engine.
	public static readonly Guid EngineGuid = new Guid(EngineGuidString);

	/// The display name of this debug engine.
	public const string EngineName = "Android Native Debug Engine (LLDB)";
}

}
