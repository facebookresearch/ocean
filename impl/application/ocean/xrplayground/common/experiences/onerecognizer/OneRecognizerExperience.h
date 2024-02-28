// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#include <application/ocean/xrplayground/common/experiences/Experiences.h>
#include <application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h>

#if defined XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include <ocean/platform/meta/quest/application/VRTextVisualizer.h>
#include <ocean/platform/meta/quest/vrapi/application/VRTableMenu.h>
#include <ocean/media/FrameMedium.h>
#include <ocean/rendering/Box.h>
#include <ocean/rendering/Text.h>
#include <application/ocean/xrplayground/common/PlatformSpecific.h>
#include <recognition/image_tracker_ipc/ImageTrackerRuntimeIpcClient.h>
#include <vros/sys/tracking/HeadTracker.h>

namespace Ocean::XRPlayground
{

class OneRecognizerExperience: public XRPlaygroundExperience
{
	public:
	OneRecognizerExperience();

	/**
	 * Destructs this experience, all resources will be released.
	 */
	~OneRecognizerExperience() override = default;

	/**
	 * Loads this experience.
	 * @see Experience::load().
	 */
	bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

	/**
	 * Unloads this experience.
	 * @see Experience::unload().
	 */
	bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

	/**
	 * Key press function.
	 * @see Library::onKeyPress().
	 */
	void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

	/**
	 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
	 * @see Experience::preUpdate().
	 */
	Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

	/**
	 * Creates a new UserInterfaceExperience object.
	 * @return The new experience
	 */
	static std::unique_ptr<XRPlaygroundExperience> createExperience();

	/**
	 * Render error message in UI
	 **/
	void renderErrorMessage(const Rendering::EngineRef& engine, const std::string errorMessage);

	protected:
    /**
	 * Event callback function for handled content.
	 * @param content The content which has been handled
	 * @param succeeded True, if the content could be handled; False, if the content could not be handled
	 * @param scenes The rendering scenes that have been created when handling the content
	 */
	void onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes);
	/**
	 * Definition of experience states.
	 */
	enum State
	{
		// idling.
		IDLE,
		// state of showing user need to select one OR experience
		SELECTING_MODE,
		// running Scene understanding experience
		SCENE_UNDERSTANDING,
		// running Target AR experience
		TARGET_AR_RECOGNIZING,
	};

	// Internal experience state init with IDLE
	State state_ = IDLE;

	// An additional rendering scene for the models.
	std::vector<Rendering::SceneRef> additionalScenes_;

	// An additional rendering scene for the bounding box.
	std::vector<Rendering::SceneRef> bboxScenes_;

	// IPC client for MRSS connection
	recognition::image_tracker::ipc::ImageTrackerRuntimeIpcClient ipcClient_;

	/**
	* TargetId result type will depends on experience.
	* Scene understanding: TargetID will be the scene concept. e.g. computer/monitor/keyboard...
	* Target AR: TargetId is fbid for FXEntTargetImage
	*/
	std::vector<const recognition::RecognitionTarget> targets;

	// Cache for Target AR entities which is recognized during experience
	std::unordered_map<std::string, const recognition::RecognitionTarget> targetAREntities;

	// Text visualizer in OCEAN experience
	Platform::Meta::Quest::Application::VRTextVisualizer textVisualizer_;

	// The table menu allowing to show a simple user interface.
	Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

	// Definition of a mapping from a human-readable description to a frame medium URL
	typedef std::map<std::string, std::string> FrameMediumUrlMap;

	// The frame medium object of the camera that will be displayed.
	Media::FrameMediumRef frameMedium_;

	// The lock that is used to allow thread-safe updates to the frame medium object.
	Lock frameMediumLock_;

	// The transformation from the coordinate space of the pattern to the world.
	std::vector<HomogenousMatrix4> world_T_patterns_;

	// Used to find matrix transformation from OSSDK's world coordinate system to VRAPI's world coordinate system
	// MRSS uses OSSDK world coordinate system
	// XRPlayground uses VRAPI world coordinate system (because it is not based on OpenXR)
	std::shared_ptr<OSSDK::Tracking::v8::IHeadTracker> ossdkHeadTracker_;
	HomogenousMatrix4 vrapiWorld_T_ossdkWorld_{false};

	// The physical size of the patterns in the world (in meters).
	std::vector<Vector2> patternsDimension_;

	/**
	* callback function for handle target result from IPC client
	* @param RecognitionResult recognition result from MRSS recognition features
	*/
	void targetsHandler(const recognition::RecognitionResult);

	/**
	* callback function for handle pattern tracker state from IPC client
	* @param PatternTrackerState pattern tracker state from MRSS pattern tracker capability
	*/
	void stateHandler(const pattern_tracker::PatternTrackerState);

	/**
	* Activates a new camera frame type.
	* Any previously activated stream will be disabled.
	* @param frameMediumUrl The frame medium be activated, must be valid and supported by the current platform.
	* @return True if the activation was successful, otherwise false.
	*/
	bool activateFrameMedium(const std::string& frameMediumUrl);

	/**
	* Returns a list of available camera frame type for a device type.
	* @return A map from a human-readable description to a frame medium URL
	*/
	static FrameMediumUrlMap availableFrameMediums();
};
} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
