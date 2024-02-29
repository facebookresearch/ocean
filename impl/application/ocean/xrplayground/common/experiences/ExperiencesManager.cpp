// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "application/ocean/xrplayground/common/experiences/crossplatformhangout/CrossPlatformHangoutExperience.h"

#include "application/ocean/xrplayground/common/experiences/interactivehome/InteractiveHomeExperience.h"

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignmentPhoneExperience.h"
#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignmentQuestExperience.h"

#include "application/ocean/xrplayground/common/experiences/metaportation/MetaportationExperience.h"
#include "application/ocean/xrplayground/common/experiences/metaportation/MetaportationScannerExperience.h"

#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlanPhoneExperience.h"
#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlanQuestExperience.h"

#include "application/ocean/xrplayground/common/experiences/mrstereocamera/MRStereoCameraExperience.h"

#include "application/ocean/xrplayground/common/experiences/navigation/BasemapQuestExperience.h"
#include "application/ocean/xrplayground/common/experiences/navigation/BasemapViewExperience.h"
#include "application/ocean/xrplayground/common/experiences/navigation/BasemapWorldExperience.h"
#include "application/ocean/xrplayground/common/experiences/navigation/DisplayMapsRenderingExperience.h"
#include "application/ocean/xrplayground/common/experiences/navigation/NavigationValhallaExperience.h"
#include "application/ocean/xrplayground/common/experiences/navigation/WorldLayerCircleExperience.h"

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/AudioExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/CameraStreamingReceiverExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/CameraStreamingSenderExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/HandGesturesExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/LevelOfDetailExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/MicrophoneSpeakerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/PhoneCameraExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/PlatformSDKNetworkExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/QuestCameraAnalyzerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/QuestCameraExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/UserInterfaceExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/UserProfileExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/VertsNetworkExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/AvatarMirrorExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsPhoneExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsQuestExperience.h"

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/CreditCardDetectorExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/DepthTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/EnvironmentRendererExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/EnvironmentScannerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/FloorTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/GravityAndHeadingNativeExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/ObjectCaptureExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/OculusTagTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/PatternTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/PlaneTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QuestQRCodeTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QRCodeGeneratorExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QRCodeWifiExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/RoomPlanTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/SceneTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/TexturedSceneTrackerExperience.h"
#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/VogonMeshCreatorExperience.h"

#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceMapCreatorExperience.h"
#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceRelocalizerExperience.h"

#include "application/ocean/xrplayground/common/experiences/sharedboardgame/SharedBoardGameExperience.h"

#include "application/ocean/xrplayground/common/experiences/turingdemo/OculusCameraTuringFeed.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
  #include "application/ocean/xrplayground/common/experiences/hypernovarightclick/HyperNovaRightClickExperience.h"
  #include "application/ocean/xrplayground/common/experiences/onerecognizer/OneRecognizerExperience.h"
#endif

#include "ocean/devices/Manager.h"

#include "ocean/interaction/experiences/Experiences.h"
#include "ocean/interaction/experiences/ExperiencesLibrary.h"

namespace Ocean
{

namespace XRPlayground
{

bool ExperiencesManager::SelectableExperience::isAvailableAtLocation(const Devices::GPSTracker::Location& queryLocation) const
{
	ocean_assert(queryLocation.isValid());

	if (!location_.isValid())
	{
		// this experience is not restricted to a GPS location
		return true;
	}

	constexpr double defaultMaximalDistance = 500.0;

	const double maximalDistance = location_.accuracy() >= 0.0f ? double(location_.accuracy()) : defaultMaximalDistance;

	return Devices::GPSTracker::approximatedDistanceBetweenLocations(location_, queryLocation) <= maximalDistance;
}

void ExperiencesManager::registerAllCodeBasedExperiences()
{
	// we are registering all available experiences, we may need to find a better initialization method in the future

	static bool registeredAlready = false;
	if (registeredAlready)
	{
		Log::warning() << "The code based experiences have been registered already";
		return;
	}

	registeredAlready = true;

	Interaction::Experiences::registerExperiencesLibrary();

	registerNewExperience("NavigationValhalla", std::bind(&NavigationValhallaExperience::createExperience, NavigationValhallaExperience::EM_RANDOM_TARGET));
	registerNewExperience("WorldLayerCircle", std::bind(&WorldLayerCircleExperience::createExperience));
	registerNewExperience("BasemapQuest", std::bind(&BasemapQuestExperience::createExperience));
	registerNewExperience("BasemapView", std::bind(&BasemapViewExperience::createExperience));
	registerNewExperience("BasemapWorld", std::bind(&BasemapWorldExperience::createExperience));
	registerNewExperience("GravityAndHeadingNative", std::bind(&GravityAndHeadingNativeExperience::createExperience));
	registerNewExperience("UserInterface", std::bind(&UserInterfaceExperience::createExperience));
	registerNewExperience("UserProfile", std::bind(&UserProfileExperience::createExperience));
	registerNewExperience("SceneTracker", std::bind(&SceneTrackerExperience::createExperience));
	registerNewExperience("TexturedSceneTracker", std::bind(&TexturedSceneTrackerExperience::createExperience));
	registerNewExperience("PlaneTracker", std::bind(&PlaneTrackerExperience::createExperience));
	registerNewExperience("CameraStreamingSender", std::bind(&CameraStreamingSenderExperience::createExperience));
	registerNewExperience("CameraStreamingReceiver", std::bind(&CameraStreamingReceiverExperience::createExperience));
	registerNewExperience("QuestCamera", std::bind(&QuestCameraExperience::createExperience));
	registerNewExperience("QuestCameraAnalyzer", std::bind(&QuestCameraAnalyzerExperience::createExperience));
	registerNewExperience("OculusCameraTuringFeed", std::bind(&OculusCameraTuringFeed::createExperience));
	registerNewExperience("PhoneCamera", std::bind(&PhoneCameraExperience::createExperience));
	registerNewExperience("QuestQRCodeTracker", std::bind(&QuestQRCodeTrackerExperience::createExperience));
	registerNewExperience("OnDeviceRelocalizer", std::bind(&OnDeviceRelocalizerExperience::createExperience));
	registerNewExperience("OnDeviceMapCreator", std::bind(&OnDeviceMapCreatorExperience::createExperience));
	registerNewExperience("EnvironmentRenderer", std::bind(&EnvironmentRendererExperience::createExperience));
	registerNewExperience("EnvironmentScanner", std::bind(&EnvironmentScannerExperience::createExperience));
	registerNewExperience("PlatformSDKNetwork", std::bind(&PlatformSDKNetworkExperience::createExperience));
	registerNewExperience("MetaAvatarsPhone", std::bind(&MetaAvatarsPhoneExperience::createExperience));
	registerNewExperience("MetaAvatarsQuest", std::bind(&MetaAvatarsQuestExperience::createExperience));
	registerNewExperience("AvatarMirror", std::bind(&AvatarMirrorExperience::createExperience));
	registerNewExperience("Metaportation", std::bind(&MetaportationExperience::createExperience));
	registerNewExperience("MetaportationScanner", std::bind(&MetaportationScannerExperience::createExperience));
	registerNewExperience("DepthTracker", std::bind(&DepthTrackerExperience::createExperience));
	registerNewExperience("VogonMeshCreator", std::bind(&VogonMeshCreatorExperience::createExperience));
	registerNewExperience("ObjectCapture", std::bind(&ObjectCaptureExperience::createExperience));
	registerNewExperience("QRCodeGenerator", std::bind(&QRCodeGeneratorExperience::createExperience));
	registerNewExperience("MapAlignmentPhone", std::bind(&MapAlignmentPhoneExperience::createExperience));
	registerNewExperience("MapAlignmentQuest", std::bind(&MapAlignmentQuestExperience::createExperience));
	registerNewExperience("LevelOfDetail", std::bind(&LevelOfDetailExperience::createExperience));
	registerNewExperience("MicrophoneSpeaker", std::bind(&MicrophoneSpeakerExperience::createExperience));
	registerNewExperience("VertsNetwork", std::bind(&VertsNetworkExperience::createExperience));
	registerNewExperience("OculusTagTracker", std::bind(&OculusTagTrackerExperience::createExperience));
	registerNewExperience("PatternTracker", std::bind(&PatternTrackerExperience::createExperience));
	registerNewExperience("RoomPlanTracker", std::bind(&RoomPlanTrackerExperience::createExperience));
	registerNewExperience("MRRoomPlanPhone", std::bind(&MRRoomPlanPhoneExperience::createExperience));
	registerNewExperience("MRRoomPlanQuest", std::bind(&MRRoomPlanQuestExperience::createExperience));
	registerNewExperience("MRStereoCamera", std::bind(&MRStereoCameraExperience::createExperience));
	registerNewExperience("CrossPlatformHangout", std::bind(&CrossPlatformHangoutExperience::createExperience));
	registerNewExperience("FloorTracker", std::bind(&FloorTrackerExperience::createExperience));
	registerNewExperience("InteractiveHome", std::bind(&InteractiveHomeExperience::createExperience));
	registerNewExperience("SharedBoardGame", std::bind(&SharedBoardGameExperience::createExperience));
	registerNewExperience("Audio", std::bind(&AudioExperience::createExperience));
	registerNewExperience("HandGestures", std::bind(&HandGesturesExperience::createExperience));
	registerNewExperience("CreditCardDetectorExperience", std::bind(&CreditCardDetectorExperience::createExperience));
	registerNewExperience("QRCodeWifiExperience", std::bind(&QRCodeWifiExperience::createExperience));

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	registerNewExperience("DisplayMapsRenderingExperience", std::bind(&DisplayMapsRenderingExperience::createExperience));
#endif

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	registerNewExperience("HyperNovaRightClick", std::bind(&HyperNovaRightClickExperience::createExperience));
	registerNewExperience("OneRecognizer", std::bind(&OneRecognizerExperience::createExperience));
#endif
}

ExperiencesManager::SelectableExperienceGroups ExperiencesManager::defineExperienceGroups()
{
	SelectableExperienceGroups selectableExperienceGroups;

	{
		// Main Experiences

		SelectableExperiences experiences =
		{
			SelectableExperience("Metaportation", "Metaportation.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Metaportation Scanner", "MetaportationScanner.experience", PT_PHONE_IOS),
			SelectableExperience("Shared Board Game", "SharedBoardGame.experience", PT_PORTABLE),
			SelectableExperience("Display Maps Renderer", "DisplayMapsRenderingExperience.experience", PT_PHONE_IOS),
			SelectableExperience("Environment Scanner", "EnvironmentScanner.experience", PT_PHONE_IOS),
			SelectableExperience("Environment Renderer", "EnvironmentRenderer.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("MR Room Plan", "MRRoomPlanPhone.experience", PT_PHONE_IOS),
			SelectableExperience("MR Room Plan", "MRRoomPlanQuest.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Cross-Platform Hangout", "CrossPlatformHangout.experience", PT_PORTABLE),

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
			SelectableExperience("HyperNova NTI RightClick", "HyperNovaRightClick.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("OneRecognizer", "OneRecognizer.experience", PT_HMD_QUEST_INDOOR),
#endif
		};

		selectableExperienceGroups.emplace_back("Main Experiences", std::move(experiences));
	}


	{
		// Technology experiences

		SelectableExperiences experiences =
		{
			SelectableExperience("Navigation Valhalla", "NavigationValhalla.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("World Layer Circle", "WorldLayerCircle.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Navigation Valhalla & World Layer Circle", "navigation_and_worldlayer.xrp", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Basemap in Quest", "BasemapQuest.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Basemap in View", "BasemapView.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Basemap in World", "BasemapWorld.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Solar System", "assetId:1135146120638291", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Daylight", "assetId:1323743761383847", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Virus", "assetId:523386069149647", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Daniel's Museum", "assetId:5168043326580248", PT_ANY),
			SelectableExperience("Map Alignment", "MapAlignmentPhone.experience", PT_PHONE),
			SelectableExperience("Map Alignment", "MapAlignmentQuest.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("QR Code Generator", "QRCodeGenerator.experience", PT_PHONE_IOS),
			SelectableExperience("Interactive Home", "InteractiveHome.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("MR Stereo Camera", "MRStereoCamera.experience", PT_HMD_QUEST_INDOOR),
		};

		selectableExperienceGroups.emplace_back("Technology Experiences", std::move(experiences));
	}

	{
		// Map Spots

		SelectableExperiences landmarkExperiencesProduction =
		{
			SelectableExperience("On-Device Map Creator", "OnDeviceMapCreator.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("On-Device Relocalizer", "OnDeviceRelocalizer.experience", PT_PHONE_OR_DESKTOP)
		};

		selectableExperienceGroups.emplace_back("Map Spots", std::move(landmarkExperiencesProduction));
	}

	{
		// Ocean's tracker demos and examples

		SelectableExperiences oceanTrackerExperiences =
		{
			SelectableExperience("Gravity & Heading Tracker (X3D)", "trackers_gravity_and_heading_tracker.ox3dv", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Gravity & Heading Tracker (C++)", "GravityAndHeadingNative.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("SLAM Tracker (ARKit/ARCore)", "trackers_slam_tracker.ox3dv", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Plane Tracker (ARKit/ARCore)", "PlaneTracker.experience", PT_PHONE),
			SelectableExperience("Scene Tracker (ARKit/ARCore)", "SceneTracker.experience", PT_PHONE),
			SelectableExperience("Textured Scene Tracker (ARKit)", "TexturedSceneTracker.experience", PT_PHONE),
			SelectableExperience("Static Pattern Tracker", "trackers_static_pattern_tracker.zip", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Depth Tracker", "DepthTracker.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("VOGON Mesh Creator", "VogonMeshCreator.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("Object Capture", "ObjectCapture.experience", PT_PHONE_OR_DESKTOP),
			SelectableExperience("GeoAnchor Tracker", "trackers_geoanchors.ox3dv", PT_PHONE_OR_DESKTOP),
			SelectableExperience("ARKit GeoAnchor Tracker", "trackers_arkit_geoanchors.ox3dv", PT_PHONE_IOS),
			SelectableExperience("Google Earth Seattle", "assetId:668012951160539", PT_PHONE_IOS, Devices::GPSTracker::Location(47.60990961796367, -122.3239863557857, NumericF::minValue() /*altitude*/, -1.0f /*direction*/, -1.0f /*speed*/, 1000.0f /*accuracy*/)),
			SelectableExperience("Quest QR Code Tracker", "QuestQRCodeTracker.experience", PT_HMD_QUEST_INDOOR),
			// SelectableExperience("QR Code Wifi", "QRCodeWifiExperience.experience", PT_HMD_QUEST_INDOOR), // Disabled for now as the underlying Android functionality is only available in Android SDK 28 and below.
			SelectableExperience("Oculus Tag Tracker", "OculusTagTracker.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Pattern Tracker", "PatternTracker.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Room Plan Tracker", "RoomPlanTracker.experience", PT_PHONE_IOS),
			SelectableExperience("Floor Tracker", "FloorTracker.experience", PT_PORTABLE),
			SelectableExperience("Credit Card Detector", "CreditCardDetectorExperience.experience", PT_HMD_QUEST_INDOOR),
		};

		selectableExperienceGroups.emplace_back("Ocean Tracker Demos", std::move(oceanTrackerExperiences));
	}

	{
		// Ocean's functionality demos and examples

		SelectableExperiences oceanFunctionalityExperiences =
		{
			SelectableExperience("User Interface", "UserInterface.experience", PT_PHONE),
			SelectableExperience("User Profile", "UserProfile.experience", PT_PORTABLE),
			SelectableExperience("Camera Streaming Sender", "CameraStreamingSender.experience", PT_ANY),
			SelectableExperience("Camera Streaming Receiver", "CameraStreamingReceiver.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Quest Camera (C++)", "QuestCamera.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Oculus Camera (Turing Feed)", "OculusCameraTuringFeed.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Quest Camera (X3D)", "quest_camera.ox3dv", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Dual Cameras (X3D)", "DualCameras.ox3dv", PT_ANY),
			SelectableExperience("Quest Camera Analyzer", "QuestCameraAnalyzer.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Phone Camera", "PhoneCamera.experience", PT_PHONE_IOS),
			SelectableExperience("Meta Avatars", "MetaAvatarsPhone.experience", PT_PHONE),
			SelectableExperience("Meta Avatars", "MetaAvatarsQuest.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Avatar Mirror", "AvatarMirror.experience", PT_PHONE),
			SelectableExperience("VERTS Network", "VertsNetwork.experience", PT_ANY),
			SelectableExperience("Platform SDK Network", "PlatformSDKNetwork.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Video (X3D)", "assetId:500326375209619", PT_ANY),
			SelectableExperience("Level of detail", "LevelOfDetail.experience", PT_PORTABLE),
			SelectableExperience("Microphone & Speaker", "MicrophoneSpeaker.experience", PT_ANY),
			SelectableExperience("Audio", "Audio.experience", PT_HMD_QUEST_INDOOR),
			SelectableExperience("Hand Gestures", "HandGestures.experience", PT_HMD_QUEST_INDOOR),
		};

		selectableExperienceGroups.emplace_back("Ocean Functionality Demos", std::move(oceanFunctionalityExperiences));
	}

	{
		// Basic

		SelectableExperiences basicExperiences =
		{
			SelectableExperience("Basic VRS Recording", "", PT_PHONE),
		};

		selectableExperienceGroups.emplace_back("Basic", std::move(basicExperiences));
	}

	return selectableExperienceGroups;
}

ExperiencesManager::SelectableExperienceGroups ExperiencesManager::selectableExperienceGroups(const PlatformType platformType)
{
	ocean_assert(platformType != PT_UNKNOWN);

	SelectableExperienceGroups selectableExperienceGroups = defineExperienceGroups();

	// first, we remove all experiences which cannot be executed on the specified platform

	for (SelectableExperienceGroups::iterator iGroup = selectableExperienceGroups.begin(); iGroup != selectableExperienceGroups.end(); ++iGroup)
	{
		for (SelectableExperiences::iterator iExperience = iGroup->second.begin(); iExperience != iGroup->second.end(); /*noop*/)
		{
			if (iExperience->platformType() & platformType)
			{
				++iExperience;
			}
			else
			{
				iExperience = iGroup->second.erase(iExperience);
			}
		}
	}

	if (platformType != PT_HMD_QUEST_INDOOR)
	{
		// now, we remove all experiences which are not supported on the specified platform

		bool gpsBasedFiltered = false;

		const Devices::GPSTrackerRef localGPSTracker = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

		if (localGPSTracker)
		{
			localGPSTracker->start();

			const Devices::GPSTracker::GPSTrackerSampleRef gpsSample = localGPSTracker ? localGPSTracker->sample() : Devices::GPSTracker::GPSTrackerSampleRef();

			if (gpsSample && !gpsSample->locations().empty())
			{
				gpsBasedFiltered = true;

				// removing all experience which are not supported in the current location

				const Devices::GPSTracker::Location& currentLocation = gpsSample->locations().front();

				for (SelectableExperienceGroups::iterator iGroup = selectableExperienceGroups.begin(); iGroup != selectableExperienceGroups.end(); ++iGroup)
				{
					SelectableExperiences& groupExperiences = iGroup->second;

					for (SelectableExperiences::iterator iExperience = groupExperiences.begin(); iExperience != groupExperiences.end(); /*noop*/)
					{
						if (iExperience->isAvailableAtLocation(currentLocation))
						{
							++iExperience;
						}
						else
						{
							iExperience = groupExperiences.erase(iExperience);
						}
					}
				}
			}
		}

		if (!gpsBasedFiltered)
		{
			Log::warning() << "Failed to access GPS tracker to filter experiences";
		}
	}
	else
	{
		// remove all GPS-based experiences

		for (SelectableExperienceGroups::iterator iGroup = selectableExperienceGroups.begin(); iGroup != selectableExperienceGroups.end(); ++iGroup)
		{
			for (SelectableExperiences::iterator iExperience = iGroup->second.begin(); iExperience != iGroup->second.end(); /*noop*/)
			{
				if (iExperience->location().isValid())
				{
					iExperience = iGroup->second.erase(iExperience);
				}
				else
				{
					++iExperience;
				}
			}
		}
	}

	// removing all empty groups

	for (SelectableExperienceGroups::iterator iGroup = selectableExperienceGroups.begin(); iGroup != selectableExperienceGroups.end(); /*noop*/)
	{
		if (iGroup->second.empty())
		{
			iGroup = selectableExperienceGroups.erase(iGroup);
		}
		else
		{
			++iGroup;
		}
	}

	return selectableExperienceGroups;
}

bool ExperiencesManager::registerNewExperience(std::string experienceName, CreateXRPlaygroundExperienceFunction createXRPlaygroundExperienceFunction)
{
	ocean_assert(!experienceName.empty());
	ocean_assert(createXRPlaygroundExperienceFunction);

	Interaction::Experiences::ExperiencesLibrary::CreateExperienceFunction createExperienceFunction = std::move(createXRPlaygroundExperienceFunction);
	ocean_assert(createExperienceFunction);

	return Interaction::Experiences::ExperiencesLibrary::registerNewExperience(std::move(experienceName), std::move(createExperienceFunction));
}

} // namespace XRPlayground

} // namespace Ocean
