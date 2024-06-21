/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/panoramaviewer/quest/PanoramaViewer.h"

#include "ocean/io/Directory.h"

#include "ocean/io/image/Image.h"

#include "ocean/platform/android/ResourceManager.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Utilities.h"

PanoramaViewer::PanoramaViewer(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// nothing to do here
}

XrSpace PanoramaViewer::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void PanoramaViewer::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	// we create a new scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();

	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// Copy the assets to disk
	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const IO::Directory temporaryTargetDirectory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("images");

	if (!Platform::Android::ResourceManager::get().copyAssets(temporaryTargetDirectory(), /* createDirectory */ true, "images"))
	{
		Log::error() << "Failed to copy the application assets to disk";
		return;
	}

	if (!temporaryTargetDirectory.exists())
	{
		Log::error() << "The directory containing the application assets does not exist";
		return;
	}

	const IO::File file = temporaryTargetDirectory + IO::File("vlt-mw-potw.jpg");

	if (!file.exists())
	{
		Log::error() << "The file '" << file.base() << "' does not exist, full path '" << file() << "'";
		return;
	}

	Rendering::TransformRef transform = createTexturedSphere(engine_, file);

	if (transform)
	{
		scene->addChild(std::move(transform));

		// let's honor the photographer
		renderingTextImageCredits_ = Rendering::Utilities::createText(*engine_, " Image Credit: \n ESO/H.H. Heyer ", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.1) /*fixedLineHeight*/, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
		renderingTextImageCredits_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2))); // 2meter in front of user

		scene->addChild(renderingTextImageCredits_);
	}
}

void PanoramaViewer::onFramebufferReleasing()
{
	renderingTextImageCredits_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void PanoramaViewer::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	if (renderingTextImageCredits_)
	{
		if (imageCreditsVisibleTimestamp_.isValid())
		{
			if (predictedDisplayTime >= imageCreditsVisibleTimestamp_)
			{
				ocean_assert(renderingTextImageCredits_);
				renderingTextImageCredits_->setVisible(false);
			}
		}
		else
		{
			imageCreditsVisibleTimestamp_ = predictedDisplayTime + 5.0;
		}
	}
}

Rendering::TransformRef PanoramaViewer::createTexturedSphere(const Rendering::EngineRef& engine, const IO::File& imageFile)
{
	ocean_assert(engine);
	ocean_assert(imageFile.exists());

	Frame frame = IO::Image::readImage(imageFile());

	if (!frame.isValid())
	{
		Log::error() << "Failed to load the image texture";
		return Rendering::TransformRef();
	}

	// we create a sphere with radius 50 meter
	constexpr Scalar radius = 50;

	Rendering::AttributeSetRef attributeSet;
	Rendering::TransformRef transform = Rendering::Utilities::createSphere(engine, radius, std::move(frame), nullptr /*texture*/, true /*createMipmaps*/, &attributeSet);

	if (attributeSet)
	{
		// we need to render the sphere from inside (which is culled by default),
		// thus disabling culling for the sphere

		Rendering::PrimitiveAttributeRef primitiveAttribute = engine->factory().createPrimitiveAttribute();
		primitiveAttribute->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
		attributeSet->addAttribute(primitiveAttribute);
	}

	return transform;
}
