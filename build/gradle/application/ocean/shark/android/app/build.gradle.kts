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
    from(file("$oceanDevelopmentPath/impl/application/ocean/shark/android/AndroidManifest.xml"))
    into(sourcePath)
  }

  println("Copying Java files ...")

  val javaSourcePath = "$sourcePath/java/"
  file(javaSourcePath).mkdirs()

  val javaSourceFiles =
      listOf(
          "$oceanDevelopmentPath/impl/application/ocean/shark/android/NativeInterfaceShark.java",
          "$oceanDevelopmentPath/impl/application/ocean/shark/android/SharkActivity.java",
          "$oceanDevelopmentPath/impl/ocean/base/BaseJni.java",
          "$oceanDevelopmentPath/impl/ocean/devices/android/DevicesAndroidJni.java",
          "$oceanDevelopmentPath/impl/ocean/devices/pattern/DevicesPatternJni.java",
          "$oceanDevelopmentPath/impl/ocean/media/android/MediaAndroidJni.java",
          "$oceanDevelopmentPath/impl/ocean/media/openimagelibraries/MediaOpenImageLibrariesJni.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLFrameViewActivity.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/GLView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/MessengerView.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/application/OceanActivity.java",
          "$oceanDevelopmentPath/impl/ocean/platform/android/Assets.java",
          "$oceanDevelopmentPath/impl/ocean/rendering/glescenegraph/RenderingGLESceneGraphJni.java",
          "$oceanDevelopmentPath/impl/ocean/scenedescription/sdl/obj/SceneDescriptionSDLOBJJni.java",
          "$oceanDevelopmentPath/impl/ocean/scenedescription/sdx/x3d/SceneDescriptionSDXX3DJni.java")

  copy {
    from(javaSourceFiles)
    into("$javaSourcePath")
  }

  println("Copying resource files ...")

  val resourcePath = "$sourcePath/res/"
  file(resourcePath).mkdirs()

  copy {
    from("$oceanDevelopmentPath/res/application/ocean/shark/android/res/")
    into("$resourcePath")
  }

  println("Copying asset files ...")

  val assetsPath = "$sourcePath/assets/"
  file(assetsPath).mkdirs()

  copy {
    from("$oceanDevelopmentPath/res/application/ocean/shark/android/assets/")
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
  namespace = "com.meta.ocean.app.shark.android"
  compileSdk = 33

  defaultConfig {
    applicationId = "com.meta.ocean.app.shark.android"
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
        targets += "application_ocean_shark_android_native"
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
