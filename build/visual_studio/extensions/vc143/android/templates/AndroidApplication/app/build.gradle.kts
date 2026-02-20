/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

plugins {
  id("com.android.application")
  id("org.jetbrains.kotlin.android")
}

android {
  // Package name is passed from MSBuild via -PandroidPackageName property
  val androidPackageName =
      project.findProperty("androidPackageName") as String?
          ?: error(
              "androidPackageName property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )

  // SDK versions are passed from MSBuild via -P properties
  val androidMinSdk: Int =
      (project.findProperty("androidMinSdk") as String?)?.toIntOrNull()
          ?: error(
              "androidMinSdk property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )
  val androidTargetSdk: Int =
      (project.findProperty("androidTargetSdk") as String?)?.toIntOrNull()
          ?: error(
              "androidTargetSdk property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )

  // Java version for source/target compatibility
  val javaVersionStr: String =
      project.findProperty("javaVersion") as String?
          ?: error(
              "javaVersion property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )
  val javaVersion: JavaVersion =
      when (javaVersionStr) {
        "VERSION_11" -> JavaVersion.VERSION_11
        "VERSION_17" -> JavaVersion.VERSION_17
        "VERSION_21" -> JavaVersion.VERSION_21
        else -> JavaVersion.VERSION_17
      }

  namespace = androidPackageName
  compileSdk = androidTargetSdk

  defaultConfig {
    applicationId = androidPackageName
    minSdk = androidMinSdk
    targetSdk = androidTargetSdk
    versionCode = 1
    versionName = "1.0"

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

  buildFeatures { viewBinding = true }

  // Simplified folder structure - sources directly under app/
  // Java sources are staged by MSBuild before Gradle runs
  // The staging path is passed via -PjavaStagingDir property from MSBuild
  val javaStagingDir =
      project.findProperty("javaStagingDir") as String?
          ?: error(
              "javaStagingDir property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )

  // Redirect Gradle build output to intermediate directory (keeps source tree clean)
  val gradleBuildDir = project.findProperty("gradleBuildDir") as String?
  if (gradleBuildDir != null) {
    layout.buildDirectory.set(file(gradleBuildDir))
  }

  // JNI libs are staged to intermediate folder by MSBuild
  // The path is passed via -PjniLibsDir property from MSBuild
  val jniLibsDir =
      project.findProperty("jniLibsDir") as String?
          ?: error(
              "jniLibsDir property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )

  // Android manifest file path - passed via -PandroidManifest property from MSBuild
  val androidManifest =
      project.findProperty("androidManifest") as String?
          ?: error(
              "androidManifest property not set. Build must be invoked from Visual Studio/MSBuild, not standalone Gradle."
          )

  sourceSets {
    getByName("main") {
      java.setSrcDirs(listOf(file(javaStagingDir)))
      res.setSrcDirs(listOf(file("res")))
      assets.setSrcDirs(listOf(file("assets")))
      manifest.srcFile(file(androidManifest))
      jniLibs.setSrcDirs(listOf(file(jniLibsDir)))
    }
  }
}

dependencies {
  implementation("androidx.core:core-ktx:1.12.0")
  implementation("androidx.appcompat:appcompat:1.6.1")
  implementation("com.google.android.material:material:1.11.0")
  implementation("androidx.constraintlayout:constraintlayout:2.1.4")

  testImplementation("junit:junit:4.13.2")
  androidTestImplementation("androidx.test.ext:junit:1.1.5")
  androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")

  // Additional dependencies from Visual Studio property pages (semicolon or newline separated)
  val gradleDependencies: String = project.findProperty("gradleDependencies") as String? ?: ""
  gradleDependencies
      .split(";", "\n", "\r\n")
      .map { it.trim() }
      .filter { it.isNotEmpty() && it.contains(":") }
      .forEach { dependency -> implementation(dependency) }

  // Local AAR dependencies from Ocean Java library projects (semicolon or newline separated file
  // names)
  val gradleLocalAARDependencies: String =
      project.findProperty("gradleLocalAARDependencies") as String? ?: ""
  val gradleLocalAARDir: String = project.findProperty("gradleLocalAARDir") as String? ?: ""

  gradleLocalAARDependencies
      .split(";", "\n", "\r\n")
      .map { it.trim() }
      .filter { it.isNotEmpty() }
      .forEach { aarName ->
        val aarFile =
            if (gradleLocalAARDir.isNotEmpty()) file("$gradleLocalAARDir/$aarName")
            else file(aarName)
        if (aarFile.exists()) {
          implementation(files(aarFile))
        } else {
          logger.warn(
              "WARNING: Local AAR dependency not found: $aarFile — build the dependency project first."
          )
        }
      }
}
