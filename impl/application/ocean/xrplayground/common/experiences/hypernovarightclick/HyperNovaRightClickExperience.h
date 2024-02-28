// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.
#pragma once

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/UserAccount.h"
	#include "ocean/media/FrameMedium.h"
	#include "ocean/media/PixelImage.h"
	#include "ocean/media/Utilities.h"
	#include "ocean/network/PackagedTCPServer.h"
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
	#include "ocean/rendering/AbsoluteTransform.h"
	#include "SmartThings/command/Command.hpp"
	#include <folly/Optional.h>
	#include <folly/concurrency/ConcurrentHashMap.h>
	#include <xplat/arfx/recognizer/pixel_buffer/ServicePixelBuffer.hpp>
	#include <xplat/common/bufferpool/BufferPool.hpp>

namespace arfx::events
{
template <typename>
class Observable;
} // namespace arfx::events

namespace facebook::arfx::recognizer
{
class IData;
class RecognitionCoordinator;
class RecognitionCoordinatorDebugLogger;
class ProxyActivator;
class RecognizedTarget;
class IRecognizedIntent;
} // namespace facebook::arfx::recognizer


namespace Ocean::Platform::Meta::Quest::Application
{
class VRImageVisualizer;
}

namespace facebook::cvmm::smart_things
{
class CurlClient;
class IDevice;
} // namespace facebook::cvmm::smart_things

namespace Ocean::XRPlayground
{
/**
 * This experience demonstrates HyperNova RightClick Experience on Quest platform.
 * [WIP]
 * @ingroup xrplayground
 */
class HyperNovaRightClickExperience : public XRPlaygroundExperience
{
	public:
	struct ShoplistItem
	{
		std::string name;
		std::string time;
		std::string imagePath;

		std::string getMenuText()
		{
			return name;
		}

		std::string getDisplayText()
		{
			return "Name: " + name + "\nTime: " + time;
		}

		folly::Optional<Frame> getImageFrame()
		{
			if (imagePath.empty())
			{
				return folly::none;
			}
			return Media::Utilities::loadImage(imagePath);
		}
	};

	typedef std::map<std::string, ShoplistItem> ItemMap;

	/**
	 * Destructs this experience, all resources will be released.
	 */
	~HyperNovaRightClickExperience() override;

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
	 * Initialize smart home user accounts.
	 */
	void initSmartHomeUserAccount();

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
	enum RenderingMode
	{
		NONE = 0,
		CAPTURE,
		SHOW_RESULTS,
		USER_SELECTING_RESULTS,
		SHOW_DETAILS,
		USER_SELECTING_DETAILS,
		SHOW_SHOPLIST,
	};

	/// The absolute transform that we use as a 'heads up display'.
	Rendering::AbsoluteTransformRef absoluteTransform_;

	/// The rendering Transform object holding the RBG cameras.
	Rendering::TransformRef renderingTransform_;

	/// The FrameMedium object of RGB camera that is used to retrieve frame.
	Media::FrameMediumRef frameMedium_;

	/// The FrameMedium object of RGB frame with overlaid content that will be displayed.
	Media::PixelImageRef frameMediumOverlaid_;

	/// The frame object with overlaid content.
	Frame frameOverlaid_;

	/// The Frame object of the latest RGB camera that is ingested to O.R.
	FrameRef lastFrame_;

	/// A buffer pool to allocate/deallocate memory efficiently.
	std::shared_ptr<facebook::bufferpool::BufferPool> bufferPool_;

	std::map<std::string, ShoplistItem> getShoplist();
	void addToShoplist(const std::string& itemName);

	std::string saveSnapshot();

	void showVRTableMenu(Platform::Meta::Quest::VrApi::Application::VRTableMenu& menu,
		Platform::Meta::Quest::VrApi::Application::VRTableMenu::Entries entries,
		const std::string& Title,
		const Timestamp timestamp);
	void showSnapshot(Frame* frame);
	void clearSnapshot();
	std::string queryDeviceStatus(const std::string componentID = "main");

	std::shared_ptr<
		arfx::events::Observable<std::shared_ptr<const facebook::arfx::recognizer::IData>>>
		pixelBufferProvider_;
	std::shared_ptr<facebook::arfx::recognizer::ProxyActivator> activator_;
	std::shared_ptr<facebook::arfx::recognizer::RecognitionCoordinator> coordinator_;
	std::shared_ptr<facebook::arfx::recognizer::RecognitionCoordinatorDebugLogger> coordinatorLogger_;
	std::shared_ptr<Platform::Meta::Quest::Application::VRImageVisualizer> imageVisualizer_;
	::folly::ConcurrentHashMap<std::string, std::shared_ptr<const facebook::arfx::recognizer::RecognizedTarget>> recognizedTargets_;
	std::string selectedTarget_;
	RenderingMode renderingMode_ = RenderingMode::NONE;
	Platform::Meta::Quest::VrApi::Application::VRTableMenu resultsMenu_;
	Platform::Meta::Quest::VrApi::Application::VRTableMenu detailsMenu_;
	Platform::Meta::Quest::VrApi::Application::VRTableMenu shoplistMenu_;
	Rendering::TextRef instructionsText_;
	Rendering::TextRef deviceStatusText_;
	Rendering::TransformRef deviceStatus_;

	std::unordered_map<std::string, std::shared_ptr<UserAccount>> accounts_;
	std::string user_ = "Chenchen Zhu"; // Hard-coded user name in the demo
	std::shared_ptr<facebook::cvmm::smart_things::CurlClient> curlClient_;
	std::string deviceName_;
	Timestamp deviceStatusUpdateTS_;
	double deviceStatusUpdateInterval_;
	std::shared_ptr<facebook::cvmm::smart_things::IDevice> smartDevice_;
	std::unordered_map<std::string, std::shared_ptr<facebook::cvmm::smart_things::Command>> smartThingsCommands_;

	ItemMap storage_;
};
} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
