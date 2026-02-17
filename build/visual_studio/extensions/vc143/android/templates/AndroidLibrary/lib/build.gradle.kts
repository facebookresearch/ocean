/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

plugins {
  id("com.android.library")
  id("org.jetbrains.kotlin.android")
}

// Redirect Gradle build output to intermediate directory (keeps source tree clean)
val gradleBuildDir = project.findProperty("gradleBuildDir") as String?

if (gradleBuildDir != null) {
  layout.buildDirectory.set(file(gradleBuildDir))
}

// Read properties from Gradle command line (-P flags), passed by MSBuild
val androidNamespace: String =
    project.findProperty("androidNamespace") as String?
        ?: error(
            "androidNamespace property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
        )
val androidMinSdk: Int = (project.findProperty("androidMinSdk") as String?)?.toIntOrNull() ?: 24
val androidCompileSdk: Int =
    (project.findProperty("androidCompileSdk") as String?)?.toIntOrNull() ?: 34
val javaVersionStr: String = project.findProperty("javaVersion") as String? ?: "VERSION_17"

// Dependencies from Visual Studio property pages (semicolon or newline separated)
val gradleDependencies: String = project.findProperty("gradleDependencies") as String? ?: ""
val gradleImplementationDependencies: String =
    project.findProperty("gradleImplementationDependencies") as String? ?: ""

// Local AAR dependencies (semicolon or newline separated file paths)
val gradleLocalAARDependencies: String =
    project.findProperty("gradleLocalAARDependencies") as String? ?: ""

// Map Java version string to JavaVersion enum
val javaVersion: JavaVersion =
    when (javaVersionStr) {
      "VERSION_11" -> JavaVersion.VERSION_11
      "VERSION_17" -> JavaVersion.VERSION_17
      "VERSION_21" -> JavaVersion.VERSION_21
      else -> JavaVersion.VERSION_17
    }

android {
  namespace = androidNamespace
  compileSdk = androidCompileSdk

  defaultConfig {
    minSdk = androidMinSdk

    testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
  }

  buildTypes {
    release {
      isMinifyEnabled = false
      proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
    }
  }

  compileOptions {
    sourceCompatibility = javaVersion
    targetCompatibility = javaVersion
  }

  kotlinOptions { jvmTarget = javaVersion.majorVersion }

  // Java sources location
  // If javaStagingDir is provided (from MSBuild), use staged sources
  // Otherwise, use the default src folder (for standalone Gradle builds)
  val javaStagingDir = project.findProperty("javaStagingDir") as String?

  sourceSets {
    getByName("main") {
      if (javaStagingDir != null && file(javaStagingDir).exists()) {
        java.setSrcDirs(listOf(file(javaStagingDir)))
      } else {
        // Default: sources in java folder (flat structure, package in file)
        java.setSrcDirs(listOf(file("java")))
      }
    }
  }
}

dependencies {
  // Parse and add local AAR file dependencies (compileOnly - classes available at compile time)
  // Format: file paths separated by semicolons or newlines
  gradleLocalAARDependencies
      .split(";", "\n", "\r\n")
      .map { it.trim() }
      .filter { it.isNotEmpty() }
      .forEach { aarPath ->
        if (file(aarPath).exists()) {
          compileOnly(files(aarPath))
        } else {
          logger.warn(
              "WARNING: Local AAR dependency not found: $aarPath — build the dependency project first."
          )
        }
      }

  // Parse and add 'api' dependencies (exported to consumers)
  // Format: "group:artifact:version" separated by semicolons or newlines
  gradleDependencies
      .split(";", "\n", "\r\n")
      .map { it.trim() }
      .filter { it.isNotEmpty() && it.contains(":") }
      .forEach { dependency -> api(dependency) }

  // Parse and add 'implementation' dependencies (private, not exported)
  gradleImplementationDependencies
      .split(";", "\n", "\r\n")
      .map { it.trim() }
      .filter { it.isNotEmpty() && it.contains(":") }
      .forEach { dependency -> implementation(dependency) }
}
