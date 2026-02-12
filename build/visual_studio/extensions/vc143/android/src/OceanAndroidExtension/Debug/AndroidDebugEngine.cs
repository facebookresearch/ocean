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
 * This class implements the Android Debug Engine for Java/Kotlin debugging via JDWP.
 * @ingroup oceanandroidextension
 */
[Guid(EngineGuidString)]
public class AndroidDebugEngine
{
	/// The GUID string identifying this debug engine.
	public const string EngineGuidString = "D5E6F7A8-B9C0-1234-5678-9ABCDEF01234";

	/// The GUID identifying this debug engine.
	public static readonly Guid EngineGuid = new Guid(EngineGuidString);

	/// The display name of this debug engine.
	public const string EngineName = "Android Debug Engine";
}

}
