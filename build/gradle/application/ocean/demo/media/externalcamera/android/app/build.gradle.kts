/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

plugins { id("com.android.application") }

fun getEnv(variableName: String): String {
  return System.getenv(variableName)
      ?: throw IllegalStateException("Environment variable \"$variableName\" is not set.")
}

fun getOceanDevelopmentPath(): String {
  val environmentVariableName = "OCEAN_DEVELOPMENT_PATH"
  val oceanDevelopmentPath = getEnv(environmentVariableName)

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
            "$oceanDevelopmentPath/impl/application/ocean/demo/media/externalcamera/android/AndroidManifest.xml"))
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
          "com/meta/ocean/app/demo/media/externalcamera/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/application/ocean/demo/media/externalcamera/android/ExternalCameraActivity.java"),
          "com/meta/ocean/base" to
              listOf("$oceanDevelopmentPath/impl/ocean/base/jni/BaseJni.java"),
          "com/meta/ocean/media/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/media/android/jni/MediaAndroidJni.java"),
          "com/meta/ocean/media/usb" to
              listOf("$oceanDevelopmentPath/impl/ocean/media/usb/jni/MediaUSBJni.java"),
          "com/meta/ocean/platform/android" to
              listOf("$oceanDevelopmentPath/impl/ocean/platform/android/Assets.java"),
          "com/meta/ocean/platform/android/application" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameView.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameViewActivity.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLRendererView.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLRendererViewActivity.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLView.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLViewActivity.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/MessengerView.java",
                  "$oceanDevelopmentPath/impl/ocean/platform/android/application/OceanActivity.java"),
          "com/meta/ocean/rendering/glescenegraph" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/rendering/glescenegraph/jni/RenderingGLESceneGraphJni.java"),
          "com/meta/ocean/system/usb/android" to
              listOf(
                  "$oceanDevelopmentPath/impl/ocean/system/usb/android/OceanUSBManager.java"))

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
    from("$oceanDevelopmentPath/res/application/ocean/demo/media/externalcamera/android/res")
    into("$resourcePath")
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
  namespace = "com.meta.ocean.app.demo.media.externalcamera.android"
  compileSdk = 33
  ndkVersion = getEnv("ANDROID_NDK_VERSION")

  defaultConfig {
    applicationId = "com.meta.ocean.app.demo.media.externalcamera.android"
    minSdk = 30
    targetSdk = 33
    versionCode = 1
    versionName = "1.0"

    ndk { abiFilters.add("arm64-v8a") }

    externalNativeBuild {
      cmake {
        arguments += "-DBUILD_SHARED_LIBS=OFF"
        arguments +=
            "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
        arguments += "-B${project.properties["oceanGradleBuildPath"]}"
        targets += "application_ocean_demo_media_externalcamera_android_native"
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
