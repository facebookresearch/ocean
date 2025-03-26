/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Top-level build file where you can add configuration options common to all sub-projects/modules.
plugins { id("com.android.application") version "8.1.0" apply false }

if (!project.hasProperty("oceanThirdPartyPath")) {
  println(
      "oceanThirdPartyPath not specified, checking for environment variable OCEAN_THIRDPARTY_PATH...")
  var defaultPath = System.getenv("OCEAN_THIRDPARTY_PATH")
  if (defaultPath != null && !defaultPath.isEmpty()) {
    println("Using OCEAN_THIRDPARTY_PATH environment variable: ${defaultPath}")
  } else {
    var osName = System.getProperty("os.name").toLowerCase()
    if (osName.contains("win")) {
      defaultPath = "C:\\tmp\\ocean\\install\\and"
    } else {
      defaultPath = "/tmp/ocean/install/android"
    }
    println("Environment variable not found. Using fallback path: ${defaultPath}")
  }
  project.ext["oceanThirdPartyPath"] = defaultPath
}

if (!file(project.properties["oceanThirdPartyPath"]).exists()) {
  throw GradleException(
      "Path specified for parameter 'oceanThirdPartyPath' does not exist:  '${project.properties["oceanThirdPartyPath"]}'")
}

if (!project.hasProperty("oceanGradleBuildPath")) {
  println(
      "oceanGradleBuildPath not specified, checking for environment variable OCEAN_GRADLE_BUILD_PATH...")
  var defaultPath = System.getenv("OCEAN_GRADLE_BUILD_PATH")
  if (defaultPath != null && !defaultPath.isEmpty()) {
    println("Using OCEAN_GRADLE_BUILD_PATH environment variable: ${defaultPath}")
  } else {
    var osName = System.getProperty("os.name").toLowerCase()
    if (osName.contains("win")) {
      defaultPath = "C:\\tmp\\ocean\\gradle"
    } else {
      defaultPath = "/tmp/ocean/gradle"
    }
    println("Environment variable not found. Using fallback path: ${defaultPath}")
  }
  project.ext["oceanGradleBuildPath"] = defaultPath
}
