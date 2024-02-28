// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/MRPassthroughVisualizer.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

MRPassthroughVisualizer::ScopedState::ScopedState(MRPassthroughVisualizer& mrPassthroughVisualizer)
{
	if (mrPassthroughVisualizer.isValid())
	{
		mrPassthroughVisualizer_ = &mrPassthroughVisualizer;

		wasRunning_ = mrPassthroughVisualizer.isPassthroughRunning();
	}
}

MRPassthroughVisualizer::ScopedState::ScopedState(ScopedState&& scopedState)
{
	*this = std::move(scopedState);
}

MRPassthroughVisualizer::ScopedState::~ScopedState()
{
	release();
}

void MRPassthroughVisualizer::ScopedState::release()
{
	if (mrPassthroughVisualizer_ != nullptr)
	{
		if (wasRunning_)
		{
			mrPassthroughVisualizer_->resumePassthrough();
		}
		else
		{
			mrPassthroughVisualizer_->pausePassthrough();
		}

		mrPassthroughVisualizer_ = nullptr;
	}

	wasRunning_ = false;
}

MRPassthroughVisualizer::ScopedState& MRPassthroughVisualizer::ScopedState::operator=(ScopedState&& scopedState)
{
	if (this != &scopedState)
	{
		release();

		mrPassthroughVisualizer_ = scopedState.mrPassthroughVisualizer_;
		scopedState.mrPassthroughVisualizer_ = nullptr;

		wasRunning_ = scopedState.wasRunning_;
		scopedState.wasRunning_ = false;
	}

	return *this;
}

bool MRPassthroughVisualizer::pausePassthrough()
{
	const ScopedLock scopedLock(lock_);

	if (!isPassthroughRunning())
	{
		return true;
	}

	MrResult mrResult = MrPassthroughPause(mrPassthroughFeature_);

	if (MR_FAILED(mrResult))
	{
		Log::error() << "Failed to pause passthrough, error: " << toString(mrResult);
		return false;
	}

	return true;
}

bool MRPassthroughVisualizer::resumePassthrough()
{
	const ScopedLock scopedLock(lock_);

	if (isPassthroughRunning())
	{
		return true;
	}

	MrResult mrResult = MrPassthroughStart(mrPassthroughFeature_);

	if (MR_FAILED(mrResult))
	{
		Log::error() << "Failed to resume passthrough, error: " << toString(mrResult);
		return false;
	}

	return true;
}

bool MRPassthroughVisualizer::enterVrMode()
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(ovrMobile_ != nullptr);
	ocean_assert(ovrJava_ != nullptr);

	// If a session had been created already, enter the VR mode directly
	if (mrSession_ != XR_NULL_HANDLE)
	{
		MrSessionConfiguration mrConfiguration;
		MrSessionConfigurationService mrConfigurationService;
		MrSessionConfigurationSetService(&mrConfiguration, &mrConfigurationService, ovrJava_->Vm, ovrJava_->Env, ovrJava_->ActivityObject, ovrMobile_);

		const MrResult mrResult = MrSessionEnterVrMode(mrSession_, &mrConfiguration);

		if (MR_FAILED(mrResult))
		{
			Log::error() << "MR session failed to enter VR mode, error: " << toString(mrResult);
			return false;
		}

		applyUpdatedPassthroughStyle();

		return true;
	}

	// Create the MR session objects
	ocean_assert(mrSession_ == XR_NULL_HANDLE);
	ocean_assert(mrPassthroughFeature_ == XR_NULL_HANDLE);

	if (!createMrSession(mrSession_, mrPassthroughFeature_, mrCompositionLayer_, ovrJava_, ovrMobile_))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// Initialize custom shaders
	for (CustomShader customShader : customShaders_)
	{
		Log::info() << "Initializing custom shader: " << customShader.name;
		const MrResult mrResult = MrPassthroughRequestShaderProgram(mrPassthroughFeature_, customShader.name.c_str(), &customShader.program);

		if (MR_FAILED(mrResult))
		{
			Log::error() << "Failed load custom shader: " << customShader.name;

			if (customShader.required)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}
		}
	}

	applyUpdatedPassthroughStyle();

	return true;
}

bool MRPassthroughVisualizer::leaveVrMode()
{
	const ScopedLock scopedLock(lock_);

	if (mrSession_ != XR_NULL_HANDLE)
	{
		const MrResult mrResult = MrSessionLeaveVrMode(mrSession_);

		if (MR_FAILED(mrResult))
		{
			Log::error() << "MR session failed to leave VR mode, error: " << toString(mrResult);
			return false;
		}
	}

	return true;
}

bool MRPassthroughVisualizer::update()
{
	const ScopedLock scopedLock(lock_);

	if (!isValid())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const MrResult mrResult = MrSessionProcess(mrSession_);

	return MR_SUCCEEDED(mrResult);
}

VRNativeApplication::CustomLayer MRPassthroughVisualizer::render(const Timestamp renderTimestamp)
{
	const ScopedLock scopedLock(lock_);

	if (!isValid())
	{
		ocean_assert(false && "This should never happen!");
		return VRNativeApplication::CustomLayer();
	}

	std::shared_ptr<ovrLayer_Union2> passthroughLayer = std::make_shared<ovrLayer_Union2>();

	const ovrTracking2 tracking = vrapi_GetPredictedTracking2(ovrMobile_, double(renderTimestamp));
	const MrResult mrResult = MrCompositionLayerConfigureVr(mrCompositionLayer_, &tracking, double(renderTimestamp), passthroughLayer.get());

	if (MR_FAILED(mrResult))
	{
		Log::error() << "Updating the MR session passthrough failed: " + toString(mrResult);
		return VRNativeApplication::CustomLayer();
	}

	return VRNativeApplication::CustomLayer(std::move(passthroughLayer));
}

MRPassthroughVisualizer& MRPassthroughVisualizer::operator=(MRPassthroughVisualizer&& mrPassthroughVisualizer)
{
	if (this != &mrPassthroughVisualizer)
	{
		if (isValid())
		{
			leaveVrMode();
			destroyMrSession(mrSession_, mrPassthroughFeature_);
		}

		VRVisualizer::operator=(std::move(mrPassthroughVisualizer));

		ovrMobile_ = mrPassthroughVisualizer.ovrMobile_;
		mrPassthroughVisualizer.ovrMobile_ = nullptr;

		ovrJava_ = mrPassthroughVisualizer.ovrJava_;
		mrPassthroughVisualizer.ovrJava_ = nullptr;

		mrSession_ = mrPassthroughVisualizer.mrSession_;
		mrPassthroughVisualizer.mrSession_ = XR_NULL_HANDLE;

		mrPassthroughFeature_ = mrPassthroughVisualizer.mrPassthroughFeature_;
		mrPassthroughVisualizer.mrPassthroughFeature_ = XR_NULL_HANDLE;

		mrCompositionLayer_ = mrPassthroughVisualizer.mrCompositionLayer_;
		mrPassthroughVisualizer.mrCompositionLayer_ = XR_NULL_HANDLE;

		passthroughStyles_ = std::move(mrPassthroughVisualizer.passthroughStyles_);
		customShaders_ = std::move(mrPassthroughVisualizer.customShaders_);
	}

	return *this;
}

void MRPassthroughVisualizer::initializePassthroughStyles()
{
	// Initialize default styles

	PassthroughStyle defaultStyle;
	defaultStyle.name = "default";
	MrPassthroughStyleSetDefaults(&defaultStyle.style);

	passthroughStyles_.emplace_back(std::move(defaultStyle));

	// Initialize custom shaders

	customShaderStyleStartIndex_ = passthroughStyles_.size();

	customShaders_.clear(); // currently none added
}

void MRPassthroughVisualizer::applyUpdatedPassthroughStyle()
{
	Log::info() << "Applying passthrough style: " << passthroughStyles_[passthroughStyleIndex_].name;

	MrResult result = MR_SUCCESS;
	if (lastAppliedCustomShaderIndex_ >= 0)
	{
		result = MrCompositionLayerRemoveShader(mrCompositionLayer_, customShaders_[lastAppliedCustomShaderIndex_].program);
		lastAppliedCustomShaderIndex_ = -1;
	}

	if (MR_SUCCEEDED(result))
	{
		size_t customShaderStyleIndex = passthroughStyleIndex_ - customShaderStyleStartIndex_;
		if (customShaderStyleIndex < customShaders_.size())
		{
			Log::info() << "Applying custom shader: " << customShaders_[size_t(customShaderStyleIndex)].name;
			if (customShaders_[customShaderStyleIndex].program != XR_NULL_HANDLE)
			{
				result = MrCompositionLayerAssignShader(mrCompositionLayer_, customShaders_[customShaderStyleIndex].program);
				if (MR_SUCCEEDED(result))
				{
					lastAppliedCustomShaderIndex_ = customShaderStyleIndex;
				}
			}
			else
			{
				Log::error() << "Custom shader not properly initialized.";
			}
		}
		else
		{
			result = MrPassthroughSetStyle(mrCompositionLayer_, &passthroughStyles_[passthroughStyleIndex_].style);
		}
	}

	if (MR_FAILED(result))
	{
		Log::error() << "MrPassthroughSetStyle failed, error: " << toString(result);
	}
}

std::string MRPassthroughVisualizer::toString(const MrResult mrResult)
{
	switch (mrResult)
	{
		case MR_SUCCESS:
			return "MR_SUCCESS";

		case MR_EVENT_UNAVAILABLE:
			return "MR_EVENT_UNAVAILABLE";

		case MR_NO_PENDING_WORK:
			return "MR_NO_PENDING_WORK";

		case MR_NO_PASSTHROUGH_BUNDLE:
			return "MR_NO_PASSTHROUGH_BUNDLE";

		case MR_POSE_INTERPOLATED:
			return "MR_POSE_INTERPOLATED";

		case MR_STREAM_ENDED:
			return "MR_STREAM_ENDED";

		case MR_DATA_NOT_UPDATED:
			return "MR_DATA_NOT_UPDATED";

		case MR_ERROR_UNKNOWN:
			return "MR_ERROR_UNKNOWN";

		case MR_ERROR_INCOMPATIBLE_VERSION:
			return "MR_ERROR_INCOMPATIBLE_VERSION";

		case MR_ERROR_VALIDATION_FAILURE:
			return "MR_ERROR_VALIDATION_FAILURE";

		case MR_ERROR_HANDLE_INVALID:
			return "MR_ERROR_HANDLE_INVALID";

		case MR_ERROR_UNEXPECTED_STATE:
			return "MR_ERROR_UNEXPECTED_STATE";

		case MR_ERROR_SYSTEM_FAILURE:
			return "MR_ERROR_SYSTEM_FAILURE";

		case MR_ERROR_FEATURE_ALREADY_CREATED:
			return "MR_ERROR_FEATURE_ALREADY_CREATED";

		case MR_ERROR_FEATURE_REQUIRED:
			return "MR_ERROR_FEATURE_REQUIRED";

		case MR_ERROR_TOO_MANY_COMPOSITION_LAYERS:
			return "MR_ERROR_TOO_MANY_COMPOSITION_LAYERS";

		case MR_ERROR_NOT_SUPPORTED:
			return "MR_ERROR_NOT_SUPPORTED";

		case MR_ERROR_NOT_PERMITTED:
			return "MR_ERROR_NOT_PERMITTED";

		case MR_ERROR_INSUFFICIENT_RESOURCES:
			return "MR_ERROR_INSUFFICIENT_RESOURCES";

		case MR_ERROR_RETRY:
			return "MR_ERROR_RETRY";

		case MR_ERROR_TIMED_OUT:
			return "MR_ERROR_TIMED_OUT";

		case MR_ERROR_DESTROY_WHILE_PROCESSING:
			return "MR_ERROR_DESTROY_WHILE_PROCESSING";

		case MR_ERROR_TIMESTAMP_OUTSIDE_BOUNDS:
			return "MR_ERROR_TIMESTAMP_OUTSIDE_BOUNDS";

		case MR_ERROR_NOT_RUNNING:
			return "MR_ERROR_NOT_RUNNING";

		case MR_ERROR_NYI:
			return "MR_ERROR_NYI";

		case MR_ERROR_BUNDLE_ALREADY_LOCKED:
			return "MR_ERROR_BUNDLE_ALREADY_LOCKED";

		case MR_ERROR_SENSOR_ACCESS:
			return "MR_ERROR_SENSOR_ACCESS";

		case MR_ERROR_LIMIT_REACHED:
			return "MR_ERROR_LIMIT_REACHED";

		case MR_RESULT_MAX_ENUM:
			ocean_assert(false && "Invalid error value");
			return "MR_RESULT_MAX_ENUM";

		// intentionally no default case
	}

	ocean_assert(false && "Never be here!");

	return "UNKNOWN";
}

bool MRPassthroughVisualizer::createMrSession(MrSession& mrSession, MrPassthroughFeature& mrPassthroughFeature, MrCompositionLayer& backgroundPassthroughLayer, const ovrJava* java, ovrMobile* ovr)
{
	if (mrSession != XR_NULL_HANDLE || mrPassthroughFeature != XR_NULL_HANDLE || backgroundPassthroughLayer != XR_NULL_HANDLE || ovr == nullptr || java == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	MrSessionConfiguration mrConfiguration;
	MrSessionConfigurationService mrConfigurationService;
	MrSessionConfigurationSetService(&mrConfiguration, &mrConfigurationService, java->Vm, java->Env, java->ActivityObject, ovr);

	const MrResult mrSessionCreateResult = MrSessionCreate(&mrConfiguration, &mrSession);

	if (MR_FAILED(mrSessionCreateResult))
	{
		Log::error() << "Failed to create a MR session, error: " << toString(mrSessionCreateResult);
		return false;
	}

	const MrResult mrSessionCreatePassthroughResult = MrSessionCreatePassthrough(mrSession, nullptr, &mrPassthroughFeature);

	if (MR_FAILED(mrSessionCreatePassthroughResult))
	{
		Log::error() << "MR session failed to create passthrough, error: " << toString(mrSessionCreatePassthroughResult);

		MrSessionDestroy(mrSession);
		mrSession = XR_NULL_HANDLE;

		return false;
	}
	else
	{
		if (!recreateCompositionLayer(mrSession, backgroundPassthroughLayer))
		{
			// Something failed, so clean up and abort
			MrSessionDestroyPassthrough(mrSession, mrPassthroughFeature);
			MrSessionDestroy(mrSession);

			mrSession = XR_NULL_HANDLE;
			mrPassthroughFeature = XR_NULL_HANDLE;

			return false;
		}
	}

	ocean_assert(mrPassthroughFeature != XR_NULL_HANDLE);
	ocean_assert(backgroundPassthroughLayer != XR_NULL_HANDLE);

	const MrResult startPassthroughResult = MrPassthroughStart(mrPassthroughFeature);

	if (MR_FAILED(startPassthroughResult))
	{
		Log::error() << "Failed start the MR Passthrough service, error: " << toString(startPassthroughResult);
		return false;
	}


	return true;
}

bool MRPassthroughVisualizer::destroyMrSession(MrSession& mrSession, MrPassthroughFeature& mrPassthroughFeature)
{
	bool successfullyDestroyedMrSession = true;

	if (mrSession != XR_NULL_HANDLE)
	{
		if (mrPassthroughFeature != XR_NULL_HANDLE)
		{
			const MrResult mrResultDestroyPassthrough = MrSessionDestroyPassthrough(mrSession, mrPassthroughFeature);
			if (MR_FAILED(mrResultDestroyPassthrough))
			{
				Log::error() << "Failed to destroy passthrough feature with error: " << toString(mrResultDestroyPassthrough);
				successfullyDestroyedMrSession = false;
			}

			mrPassthroughFeature = XR_NULL_HANDLE;
		}

		const MrResult mrResultSessionDestroy = MrSessionDestroy(mrSession);
		if (MR_FAILED(mrResultSessionDestroy))
		{
			Log::error() << "Failed to destroy MR session with error: " << toString(mrResultSessionDestroy);
			successfullyDestroyedMrSession = false;
		}

		mrSession = XR_NULL_HANDLE;
	}

	return successfullyDestroyedMrSession;
}

bool MRPassthroughVisualizer::recreateCompositionLayer(MrSession& mrSession, MrCompositionLayer& backgroundPassthroughLayer)
{
	if (backgroundPassthroughLayer)
	{
		const MrResult mrResult = MrSessionDestroyCompositionLayer(mrSession, backgroundPassthroughLayer);

		if (MR_FAILED(mrResult))
		{
			Log::error() << "Failed to delete existing composition layer, error: " << toString(mrResult);
			return false;
		}
	}

	MrCompositionLayerConfiguration compositionLayerConfiguration;
	MrCompositionLayerConfigurationSetDefaults(&compositionLayerConfiguration);

	const MrResult createCompositionLayerResult = MrSessionCreateCompositionLayer(mrSession, &compositionLayerConfiguration, &backgroundPassthroughLayer);

	if (MR_FAILED(createCompositionLayerResult))
	{
		Log::error() << "Failed to delete existing composition layer, error: " << toString(createCompositionLayerResult);
		return false;
	}

	return true;
}

} // namespace Application

} // namespace VrApi

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
