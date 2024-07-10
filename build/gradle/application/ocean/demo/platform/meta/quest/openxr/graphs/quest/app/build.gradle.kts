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
            "$oceanDevelopmentPath/impl/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/AndroidManifest.xml"))
    into(sourcePath)
  }

  println("Copying Java files ...")

  val javaSourcePath = "$sourcePath/java/"
  file(javaSourcePath).mkdirs()

  val javaSourceFiles =
      listOf(
          "$oceanDevelopmentPath/impl/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplicationActivity.java")

  copy {
    from(javaSourceFiles)
    into("$javaSourcePath")
  }

  println("Copying resource files ...")

  val resourcePath = "$sourcePath/res/"
  file(resourcePath).mkdirs()

  copy {
    from(
        "$oceanDevelopmentPath/res/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/res/")
    into("$resourcePath")
  }

  println("Copying asset files ...")

  val assetsPath = "$sourcePath/assets/"
  file(assetsPath).mkdirs()

  // Copy assets required by "ocean_platform_meta_quest_application" library.
  copy {
    from("$oceanDevelopmentPath/res/ocean/platform/meta/quest/application/assets")
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
  namespace = "com.meta.ocean.app.demo.platform.meta.quest.openxr.graphs.android"
  compileSdk = 33
  ndkVersion = getEnv("ANDROID_NDK_VERSION")

  defaultConfig {
    applicationId = "com.meta.ocean.app.demo.platform.meta.quest.openxr.graphs.android"
    minSdk = 29
    targetSdk = 33
    versionCode = 1
    versionName = "1.0"

    ndk { abiFilters.add("arm64-v8a") }

    externalNativeBuild {
      cmake {
        arguments += "-DBUILD_SHARED_LIBS=OFF"
        arguments += "-DOCEAN_ENABLE_QUEST=TRUE"
        arguments +=
            "-DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=${project.properties["oceanThirdPartyPath"]}"
        arguments += "-B${project.properties["oceanGradleBuildPath"]}"
        targets += "application_ocean_demo_platform_meta_quest_openxr_graphs_quest_native"
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
