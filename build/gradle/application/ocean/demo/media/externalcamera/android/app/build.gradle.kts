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

  val javaSourceFiles =
      listOf(
          "$oceanDevelopmentPath/impl/application/ocean/demo/media/externalcamera/android/ExternalCameraActivity.java",
          "$oceanDevelopmentPath/impl/ocean/base/BaseJni.java",
          "$oceanDevelopmentPath/impl/ocean/media/android/MediaAndroidJni.java",
          "$oceanDevelopmentPath/impl/ocean/media/usb/MediaUSBJni.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameViewActivity.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/MessengerView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/OceanActivity.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/Assets.java",
          "$oceanDevelopmentPath/impl/ocean/rendering/glescenegraph/RenderingGLESceneGraphJni.java",
          "$oceanDevelopmentPath/impl/ocean/system/usb/android/OceanUSBManager.java")

  copy {
    from(javaSourceFiles)
    into("$javaSourcePath")
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

  externalNativeBuild {
    cmake {
      path("${getOceanDevelopmentPath()}/CMakeLists.txt")
      version = "3.26.0+"
    }
  }
}
