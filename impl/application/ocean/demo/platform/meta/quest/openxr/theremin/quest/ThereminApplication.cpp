/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/theremin/quest/ThereminApplication.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Utilities.h"

using namespace Platform::Meta::Quest;

ThereminApplication::ThereminApplication(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	liveAudio_ = Media::Manager::get().newMedium("Speaker", Media::Medium::LIVE_AUDIO, true /*useExclusive*/);

	if (liveAudio_)
	{
		liveAudio_->start();
	}
	else
	{
		Log::error() << "Failed to create live audio medium";
	}
}

XrSpace ThereminApplication::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void ThereminApplication::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	vrHandVisualizer_.setTransparency(0);

	ocean_assert(engine_ && framebuffer_);

	// we create a new scenegraph scene object
	const Rendering::SceneRef scene = engine_->factory().createScene();
	// we add the scene to the framebuffer, so that it will be rendered automatically
	framebuffer_->addScene(scene);

	// we create a new transform object which will hold all the rendering objects
	Rendering::TransformRef renderingTransform = engine_->factory().createTransform();
	// adding the transform node to the scene
	scene->addChild(renderingTransform);

	constexpr Scalar bodyWidth = Scalar(0.3);
	constexpr Scalar bodyHeight = Scalar(0.05);
	constexpr Scalar bodyDepth = Scalar(0.15);

	constexpr Scalar pitchAntennaHeight = Scalar(0.3);
	constexpr Scalar pitchAntennaRadius = Scalar(0.005);
	constexpr Scalar pitchAntennaBorder = Scalar(0.02);

	constexpr Scalar volumeAntennaLength = Scalar(0.2);
	constexpr Scalar volumeAntennaRadius = Scalar(0.005);

	const RGBAColor bodyColor(0.82f, 0.41f, 0.12f);
	const RGBAColor antennaColor(1.0f, 1.0f, 1.0f);

	Rendering::TransformRef box = Rendering::Utilities::createBox(engine_, Vector3(bodyWidth, bodyHeight, bodyDepth), bodyColor);
	renderingTransform->addChild(std::move(box));

	renderingTransformPitchAntenna_ = Rendering::Utilities::createCylinder(engine_, pitchAntennaRadius, pitchAntennaHeight, antennaColor);
	const Vector3 pitchAntennaPosition = Vector3(bodyWidth * Scalar(0.5) - pitchAntennaBorder, bodyHeight * Scalar(0.5) + pitchAntennaHeight * Scalar(0.5), -bodyDepth * Scalar(0.5) + pitchAntennaBorder);
	renderingTransformPitchAntenna_->setTransformation(HomogenousMatrix4(pitchAntennaPosition));
	renderingTransform->addChild(renderingTransformPitchAntenna_);

	renderingTransformVolumeAntenna_ = Rendering::Utilities::createCylinder(engine_, volumeAntennaRadius, volumeAntennaLength, antennaColor);
	const Vector3 volumeAntennaPosition = Vector3(-bodyWidth * Scalar(0.5) - volumeAntennaLength * Scalar(0.5), 0, 0);
	renderingTransformVolumeAntenna_->setTransformation(HomogenousMatrix4(volumeAntennaPosition) * HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Numeric::pi_2())));
	renderingTransform->addChild(renderingTransformVolumeAntenna_);

	Rendering::TransformRef transformText = Rendering::Utilities::createText(*engine_, "Frequency: 20Hz", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02) /*fixedLineHeight*/, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextFrequency_);
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, bodyHeight, -bodyDepth * Scalar(0.3)), Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-45))));
	renderingTransform->addChild(std::move(transformText));

	transformText = Rendering::Utilities::createText(*engine_, "Volume: 0dB", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.02) /*fixedLineHeight*/, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextVolume_);
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, bodyHeight, bodyDepth * Scalar(0.3)), Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-45))));
	renderingTransform->addChild(std::move(transformText));

	renderingTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.4), Scalar(-0.5))));
}

void ThereminApplication::onFramebufferReleasing()
{
	renderingTransformPitchAntenna_.release();
	renderingTransformVolumeAntenna_.release();
	renderingTextFrequency_.release();
	renderingTextVolume_.release();

	liveAudio_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void ThereminApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (passthrough_.isValid() && !passthrough_.isStarted())
	{
		if (!passthrough_.start())
		{
			Log::error() << "Failed to start passthrough";
		}
	}

	if (liveAudio_.isNull())
	{
		return;
	}

	const Vectors3& rightHandJoints = handPoses_.jointPositions(1);

	constexpr float minFrequency = 40.0f;
	constexpr float maxFrequency = 400.0f;

	float frequency = minFrequency;

	if (rightHandJoints.size() == XR_HAND_JOINT_COUNT_EXT)
	{
		ocean_assert(renderingTransformPitchAntenna_);
		const HomogenousMatrix4 world_T_pitchAntenna = renderingTransformPitchAntenna_->worldTransformation();
		const HomogenousMatrix4 pitchAntenna_T_world(world_T_pitchAntenna.inverted());

		const Vector3& worldPalmPosition = rightHandJoints[XR_HAND_JOINT_PALM_EXT];

		const Vector3 antennaPalmPosition = pitchAntenna_T_world * worldPalmPosition;

		const Scalar distance = Vector3(antennaPalmPosition.x(), 0, antennaPalmPosition.z()).length();

		if (distance >= Scalar(0.02))
		{
			// mapping [0.02m, 0.3m] -> [400Hz, 40Hz]

			constexpr float slope = (minFrequency - maxFrequency) / (0.3f - 0.02f);
			constexpr float intersection = minFrequency - slope * 0.3f;

			frequency = minmax(minFrequency, slope * float(distance) + intersection, maxFrequency);
		}
		else
		{
			frequency = maxFrequency;
		}
	}

	const Vectors3& leftHandJoints = handPoses_.jointPositions(0);

	constexpr float minVolume = -300.0f;
	constexpr float maxVolume = 0.0f;

	float volume = minVolume;

	if (leftHandJoints.size() == XR_HAND_JOINT_COUNT_EXT)
	{
		ocean_assert(renderingTransformVolumeAntenna_);
		const HomogenousMatrix4 world_T_volumeAntenna = renderingTransformVolumeAntenna_->worldTransformation();
		const HomogenousMatrix4 volumeAntenna_T_world(world_T_volumeAntenna.inverted());

		const Vector3& worldPalmPosition = leftHandJoints[XR_HAND_JOINT_PALM_EXT];

		const Vector3 antennaPalmPosition = volumeAntenna_T_world * worldPalmPosition;

		const Scalar distance = Vector3(antennaPalmPosition.x(), 0, antennaPalmPosition.z()).length();

		if (distance >= Scalar(0.02))
		{
			// mapping [0.02m, 0.3m] -> [-300dB, 0dB]

			constexpr float slope = (maxVolume - minVolume) / (0.3f - 0.02f);
			constexpr float intersection = maxVolume - slope * 0.3f;

			volume = minmax(minVolume, slope * float(distance) + intersection, maxVolume);
		}
	}

	if (frequency >= 20.0f && frequency <= 20000.0f)
	{
		renderAudioSample(frequency);
	}

	liveAudio_->setSoundVolume(volume);

	ocean_assert(renderingTextFrequency_);
	renderingTextFrequency_->setText("Frequency: " + String::toAString(int(frequency)) + "Hz");

	ocean_assert(renderingTextVolume_);
	renderingTextVolume_->setText("Volume: " + String::toAString(int(volume)) + "dB");
}

void ThereminApplication::renderAudioSample(const float frequency)
{
	ocean_assert(frequency >= 20.0f && frequency <= 20000.0f);
	ocean_assert(liveAudio_);

	if (!liveAudio_->needNewSamples())
	{
		// no need for a new audio sample
		return;
	}

	constexpr size_t elementsPerSecond = 48000; // 48kHz
	constexpr size_t samplesPerSecond = 50; // 20ms
	constexpr size_t elementsPerSample = elementsPerSecond / samplesPerSecond;
	constexpr float durationPerSample = 1.0f / float(samplesPerSecond);

	reusableSample_.resize(elementsPerSample);

	if (previousFrequency_ < 0.0f)
	{
		previousFrequency_ = frequency;
	}

	for (size_t n = 0; n < reusableSample_.size(); ++n)
	{
		const float factor = float(n) / float(reusableSample_.size()); // range [0, 1)
		const float time = factor * durationPerSample; // [0, durationPerSample)

		const float currentFrequency = previousFrequency_ * (1.0f - factor) + frequency * factor; // interpolation between previous frequency and new frequency

		const float value = NumericF::sin(phase_ + time * NumericF::pi2() * currentFrequency);

		const int16_t intValue = int16_t(value * 30000.0f); // [-1, 1] -> [-30000, 30000]

		reusableSample_[n] = intValue;
	}

	phase_ = NumericF::angleAdjustPositive(phase_ + durationPerSample * NumericF::pi2() * frequency); // keep phase in range [0, 2PI) for precision
	previousFrequency_ = frequency;

	liveAudio_->addSamples(Media::LiveAudio::ST_INTEGER_16_MONO_48, reusableSample_.data(), reusableSample_.size() * sizeof(int16_t));
}
