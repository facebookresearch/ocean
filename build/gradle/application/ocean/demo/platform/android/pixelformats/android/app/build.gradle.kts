/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

plugins { id("com.android.application") }

fun getOceanDevelopmentPath(): String {
  val environmentVariableName = "OCEAN_DEVELOPMENT_PATH"

  val oceanDevelopmentPath =
      System.getenv(environmentVariableName)
          ?: throw IllegalStateException(
              "Environment variable \"$environmentVariableName\" is not set.")

  val file = File(oceanDevelopmentPath)
  if (!file.exists()) {
    throw IllegalStateException(
        "Path specified in environment variable $environmentVariableName does not exist: $oceanDevelopmentPath")
  }

  println("Value of OCEAN_DEVELOPMENT_PATH: $oceanDevelopmentPath")

  return oceanDevelopmentPath
}

// Copy temporary files required for the build
tasks.register("copyTemporaryFiles") {
  println("Copying temporary files to app/src ...")
  val oceanDevelopmentPath = getOceanDevelopmentPath()

  val sourcePath = "$rootDir/app/src/main"
  file(sourcePath).mkdirs()

  println("Copying AndroidManifest.xml ...")
  copy {
    from(
        file(
            "$oceanDevelopmentPath/impl/application/ocean/demo/platform/android/pixelformats/android/AndroidManifest.xml"))
    into(sourcePath)
  }

  println("Copying Java files ...")

  val javaSourcePath = "$sourcePath/java/"
  file(javaSourcePath).mkdirs()

  // Ocean's Java sources live next to their JNI counterparts in the C++ source
  // tree (e.g. impl/ocean/base/jni/BaseJni.java), but their `package`
  // declarations follow the Java convention (e.g. `package com.meta.ocean.base;`).
  // The two layouts do not line up, so we cannot just point sourceSets at the
  // Ocean tree -- we must copy each file into a directory that matches its
  // declared package, otherwise javac cannot resolve cross-file imports.
  val javaSourcesByPackage =
      mapOf(
          "com/meta/ocean/app/demo/platform/android/pixelformats/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/application/ocean/demo/platform/android/pixelformats/android/DemoPlatformAndroidPixelFormatsActivity.java"),
          "com/meta/ocean/base" to
              listOf("$oceanDevelopmentPath/impl/ocean/base/jni/BaseJni.java"),
          "com/meta/ocean/devices/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/devices/android/jni/DevicesAndroidJni.java"),
          "com/meta/ocean/media/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/media/android/jni/MediaAndroidJni.java"),
          "com/meta/ocean/platform/android" to
              listOf("$oceanDevelopmentPath/impl/ocean/platform/android/Assets.java"),
          "com/meta/ocean/platform/android/application" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/MessengerView.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/TextActivity.java"))

  javaSourcesByPackage.forEach { (packagePath, files) ->
    copy {
      from(files)
      into("$javaSourcePath/$packagePath")
    }
  }

  println("Copying resource files ...")

  val resourcePath = "$sourcePath/res/"
  file(resourcePath).mkdirs()

  copy {
    from(
        "$oceanDevelopmentPath/res/application/ocean/demo/platform/android/pixelformats/android/res/")
    into("$resourcePath")
  }

  println("Copying asset files ...")

  val assetsPath = "$sourcePath/assets/"
  file(assetsPath).mkdirs()

  copy {
    from(
        "$oceanDevelopmentPath/res/application/ocean/demo/platform/android/pixelformats/android/assets/")
    into("$assetsPath")
  }

  println("Copying temporary files to app/src ... completed")
}

tasks.named("preBuild") { dependsOn("copyTemporaryFiles") }

// Removal of temporary files
tasks.register("removeTemporaryFiles") {
  println("Removing temporary files in app/src ...")
  file("$rootDir/app/src").deleteRecursively()
  println("Removing temporary files in app/src ... completed")
}

tasks.named("clean").configure { finalizedBy("removeTemporaryFiles") }

android {
  namespace = "com.meta.ocean.app.demo.platform.android.pixelformats.android"
  compileSdk = 33

  defaultConfig {
    applicationId = "com.meta.ocean.app.demo.platform.android.pixelformats.android"
    minSdk = 33
    targetSdk = 33
    versionCode = 1
    versionName = "1.0"

    // Enable additional ABIs as required
    // ndk { abiFilters.addAll(listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64")) }
    ndk { abiFilters.addAll(listOf("arm64-v8a")) }

    externalNativeBuild {
      cmake {
        arguments += "-DBUILD_SHARED_LIBS=OFF"
        arguments +=
            "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
        arguments += "-B${project.properties["oceanGradleBuildPath"]}"
        targets += "application_ocean_demo_platform_android_pixelformat_android_native"
      }
    }
  }

  buildTypes {
    release {
      isMinifyEnabled = false
      proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
      signingConfig = signingConfigs.getByName("debug")
    }
  }

  compileOptions {
    sourceCompatibility = JavaVersion.VERSION_1_8
    targetCompatibility = JavaVersion.VERSION_1_8
  }

  buildFeatures { compose = true }
  packaging { resources { excludes += "/META-INF/{AL2.0,LGPL2.1}" } }

  lint {
    // The bundled Ocean media JNI shim (MediaAndroidJni) calls `new AudioRecord(...)`
    // to probe mic-capture support, which AGP lint flags as `MissingPermission`.
    // This demo doesn't actually exercise mic capture, so we don't declare
    // RECORD_AUDIO in its manifest; suppress just this one check.
    disable += "MissingPermission"
  }

  externalNativeBuild {
    cmake {
      path("${getOceanDevelopmentPath()}/CMakeLists.txt")
      version = "3.26.0+"
    }
  }
}
