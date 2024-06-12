/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.h"

#include "ocean/base/Utilities.h"

#include "ocean/math/FourierTransformation.h"
#include "ocean/math/HSVAColor.h"

#include "ocean/platform/meta/quest/platformsdk/Manager.h"

#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Scene.h"

using namespace Platform::Meta::Quest;

MicrophoneApplication::MicrophoneApplication(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	requestAndroidPermission("android.permission.RECORD_AUDIO");

	reusableVertices_.reserve(1024 * historySize_);
	reusableColorsPerVertex_.reserve(1024 * historySize_);

	lineIndexGroups_.reserve(historySize_);
}

XrSpace MicrophoneApplication::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void MicrophoneApplication::onAndroidPermissionGranted(const std::string& permission)
{
	VRNativeApplication::onAndroidPermissionGranted(permission);

	if (permission == "android.permission.RECORD_AUDIO")
	{
		Log::info() << "Record audio permission granted";

		const std::string appId = "1234567890"; // use a correct app id

		if (PlatformSDK::Manager::get().initialize(androidApp_->activity->clazz, jniEnv_, appId))
		{
			Log::info() << "Platform SDK initialized";
		}
		else
		{
			Log::error() << "Platform SDK not initialized";
		}
	}
}

void MicrophoneApplication::onFramebufferInitialized()
{
	VRNativeApplication::onFramebufferInitialized();

	ocean_assert(engine_ && framebuffer_);

	// we create a new scenegraph scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();
	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// we create a new transform object which will hold all the rendering objects
	renderingTransform_ = engine_->factory().createTransform();
	// adding the transform node to the scene
	scene->addChild(renderingTransform_);
}

void MicrophoneApplication::onFramebufferReleasing()
{
	renderingLineStrips_.release();
	renderingVertexSet_.release();
	renderingTransform_.release();

	microphoneSubscription_.release();

	VRNativeApplication::onFramebufferReleasing();
}

void MicrophoneApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplication::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	PlatformSDK::Manager::get().update(double(predictedDisplayTime));

	if (PlatformSDK::Manager::get().isInitialized())
	{
		if (!microphoneSubscription_)
		{
			microphoneSubscription_ = PlatformSDK::Microphone::get().start(std::bind(&MicrophoneApplication::onMicrophoneSample, this, std::placeholders::_1, std::placeholders::_2));

			if (microphoneSubscription_)
			{
				Log::info() << "Microphone started";
			}
			else
			{
				Log::error() << "Failed to start microphone";
			}
		}
	}
	else
	{
		Log::debug() << "Platform SDK is not yet initialized";
	}

	updateMicrophoneSpectrum();
}

void MicrophoneApplication::updateMicrophoneSpectrum()
{
	TemporaryScopedLock scopedLock(lock_);
		Values newSample(std::move(newSample_));
	scopedLock.release();

	if (newSample.empty())
	{
		// we don't have a new sample, so we don't need to do anything
		return;
	}

	const size_t size = newSample.size();

	if (frequenciesHistory_.empty())
	{
		frequenciesHistory_.resize(historySize_, Values(size, 0.0f));

		static_assert(historySize_ != 0, "Invalid history size!");

		renderingVertexSet_ = engine_->factory().createVertexSet();
		renderingLineStrips_ = engine_->factory().createLineStrips();
		renderingLineStrips_->setVertexSet(renderingVertexSet_);

		Rendering::GeometryRef geometry = engine_->factory().createGeometry();

		geometry->addRenderable(renderingLineStrips_, engine_->factory().createAttributeSet());

		Rendering::TransformRef transform = engine_->factory().createTransform();
		transform->addChild(std::move(geometry));

		renderingTransform_->addChild(std::move(transform));
	}

	ocean_assert(renderingLineStrips_);
	ocean_assert(renderingVertexSet_);

	const size_t histortyIndex = sampleCounter_ % historySize_;

	ocean_assert(histortyIndex < frequenciesHistory_.size());

	if (frequenciesHistory_[0].size() != size)
	{
		Log::error() << "The size of the sample has changed from " << frequenciesHistory_[0].size() << " to " << size << ", this should never happen!";
		return;
	}

	frequenciesHistory_[histortyIndex] = std::move(newSample);

	const size_t validSamples = std::min(sampleCounter_ + 1, historySize_);

	reusableVertices_.clear();

	reusableColorsPerVertex_.clear();

	const HSVAColor hsvaBlue(RGBAColor(0.0f, 0.0f, 1.0f));
	const HSVAColor hsvaRed(RGBAColor(1.0f, 0.0f, 0.0f));

	for (size_t s = 0; s < validSamples; ++s)
	{
		const int sampleIndex = modulo(int(histortyIndex) - int(s), int(historySize_));

		const Values& frequencies = frequenciesHistory_[sampleIndex];

		for (size_t n = 0; n < size; ++n)
		{
			const Scalar x = Scalar(int(n) - int(frequencies.size()) / 2) * Scalar(0.001);
			const Scalar y = Scalar(frequencies[n]) * Scalar(0.5) - Scalar(0.5);
			const Scalar z = Scalar(-0.5) - Scalar(s) * Scalar(0.02);

			reusableVertices_.emplace_back(x, y, z);

			const float factor = std::min(frequencies[n], 1.0f);

			reusableColorsPerVertex_.emplace_back(hsvaBlue.interpolate(hsvaRed, factor));
		}
	}

	renderingVertexSet_->setVertices(reusableVertices_);
	renderingVertexSet_->setColors(reusableColorsPerVertex_);

	if (sampleCounter_ < historySize_)
	{
		// the indices are static, so we don't need to update indices once defined

		lineIndexGroups_.emplace_back(Ocean::createIndices(size, (unsigned int)(sampleCounter_ * size)));

		renderingLineStrips_->setStrips(lineIndexGroups_);
	}

	++sampleCounter_;
}

void MicrophoneApplication::onMicrophoneSample(const int16_t* elements, const size_t size)
{
	// we convert the samples and store the result in 'newSample_'

	Log::debug() << "MicrophoneApplication::onMicrophoneSample(): " << size << " elements";

	Values newSample(size);

	for (size_t n = 0; n < size; ++n)
	{
		constexpr float normalization = 1.0f / 32768.0; // 2^15

		newSample[n] = float(elements[n]) * normalization;
	}

	// let's determine the frequency spectrum of the sample
	// normally, this should be done in a separate thread to don't block the microphone callback, but for the sake of simplicity we do it here

	std::vector<ComplexF> complexFrequencies(size);
	FourierTransformation::spatialToFrequency2(newSample.data(), (unsigned int)(size), 1u, (float*)(complexFrequencies.data()));

	// let's move the low frequencies to the center, and normalized the magnitudes of the frequencies

	ocean_assert(size % 2 == 0);
	const size_t size_2 = size / 2;

	for (size_t n = 0; n < size_2; ++n)
	{
		newSample[size_2 + n] = NumericF::sqrt(std::abs(complexFrequencies[n]));
		newSample[n] = NumericF::sqrt(std::abs(complexFrequencies[size_2 + n]));
	}

	const ScopedLock scopedLocK(lock_);

	newSample_ = std::move(newSample);
}
