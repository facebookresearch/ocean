// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/sharedboardgame/SharedBoardGameExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/io/FileResolver.h"

#include "ocean/io/image/Image.h"

#include "ocean/math/Random.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/avatars/Manager.h"

	#include "ocean/platform/meta/quest/vrapi/HandPoses.h"
#endif

namespace Ocean
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	using namespace Platform::Meta::Quest;
	using namespace Platform::Meta::Quest::VrApi;
	using namespace Platform::Meta::Quest::VrApi::Application;
#endif

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

SharedBoardGameExperience::SharedBoardGameExperience() :
	XRPlaygroundSharedSpaceExperience(false /*avatarsAtFloorLevel*/)
{
	// nothing to do here
}

SharedBoardGameExperience::~SharedBoardGameExperience()
{
	// nothing to do here
}

bool SharedBoardGameExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	if (!XRPlaygroundSharedSpaceExperience::load(userInterface, engine, timestamp, properties))
	{
		return false;
	}

	renderingTransformBoardGame_ = engine->factory().createTransform();
	renderingTransformBoardGame_->setVisible(false);

	renderingTransformBoardGameReal_ = engine->factory().createTransform();
	renderingTransformBoardGame_->addChild(renderingTransformBoardGameReal_);
	renderingTransformBoardGameVirtual_ = engine->factory().createTransform();
	renderingTransformBoardGame_->addChild(renderingTransformBoardGameVirtual_);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	if (useDemoMode_)
	{
		if (loadGame("GAME_RIDE", engine))
		{
			gameSelectionTimestamp_ = timestamp;
			experienceState_ = ES_TRY_DETECT_REAL_BOARD;
		}
		else
		{
			showMessage("Failed to load game");
		}
	}
	else if (!properties.empty())
	{
		if (loadGame(properties, engine))
		{
			gameSelectionTimestamp_ = timestamp;
			experienceState_ = ES_TRY_DETECT_REAL_BOARD;
		}
		else
		{
			showMessage("Failed to load game '" + properties + "'");
		}
	}
	else
	{
		VRTableMenu::Entries menuEntries =
		{
			VRTableMenu::Entry("Debug: SIFT", "GAME_SIFT"),
			VRTableMenu::Entry("The Game of Life", "GAME_LIFE"),
			VRTableMenu::Entry("Ticket to Ride", "GAME_RIDE")
		};

		const VRTableMenu::Group menuGroup("Which game to you want to play?", std::move(menuEntries));
		vrTableMenu_.setMenuEntries(menuGroup, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), Scalar(0.02), Scalar(0), Scalar(0));

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

		vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.65))), world_T_device);

		PlatformSpecific::get().vrControllerVisualizer().show();
		PlatformSpecific::get().vrControllerVisualizer().setControllerRayLength(Scalar(0.2));
	}

	// by default, we activate passthough and we hide the board game texture on Quest

	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();
	renderingTransformBoardGameReal_->setVisible(false);

	PlatformSpecific::get().vrHandVisualizer().setTransparency(Scalar(0.2));
	PlatformSpecific::get().vrHandVisualizer().hide();

#else

	if (loadGame("GAME_RIDE", engine))
	{
		gameSelectionTimestamp_ = timestamp;
		experienceState_ = ES_USE_REAL_BOARD;

		const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
		absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
		absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
		experienceScene()->addChild(absoluteTransform);

		const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Watch the 'Ticket to Ride' game ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0.005, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "");

		absoluteTransform->addChild(textTransform);
	}
	else
	{
		showMessage("Failed to load game");
	}

	renderingTransformBoardGameReal_->setVisible(false);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	soundScopedSubscriptions_.emplace_back(SoundManager::get().subscribe("game_piece_drop.mp3", "GAME_PIECE_DROP"));

	return true;
}

bool SharedBoardGameExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	soundScopedSubscriptions_.clear();

	gamePieceMap_.clear();
	newEntityScopedSubscription_.release();

	renderingView_.release();

	renderingTransformBoardGameReal_.release();
	renderingTransformBoardGameVirtual_.release();
	renderingTransformBoardGame_.release();
	patternTracker_.release();
	frameMedium_.release();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	vrTableMenu_.release();
#endif

	return XRPlaygroundSharedSpaceExperience::unload(userInterface, engine, timestamp);
}

Timestamp SharedBoardGameExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const Timestamp updateTimestamp = XRPlaygroundSharedSpaceExperience::preUpdate(userInterface, engine, view, timestamp);

	renderingView_ = view;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (PlatformSpecific::get().mrPassthroughVisualizer().isPassthroughRunning())
	{
		setLocalAvatarVisible(false);

		if (hideHands_)
		{
			PlatformSpecific::get().vrHandVisualizer().hide();
		}
		else
		{
			PlatformSpecific::get().vrHandVisualizer().show();
		}
	}
	else
	{
		setLocalAvatarVisible(true);

		PlatformSpecific::get().vrHandVisualizer().hide();
	}

	if (experienceState_ == ES_SELECT_GAME)
	{
		if (vrTableMenu_.isShown())
		{
			std::string entryUrl;
			if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
			{
				vrTableMenu_.hide();

				PlatformSpecific::get().vrControllerVisualizer().hide();

				if (loadGame(entryUrl, engine))
				{
					gameSelectionTimestamp_ = timestamp;
					experienceState_ = ES_TRY_DETECT_REAL_BOARD;
				}
				else
				{
					showMessage("Failed to load game");
				}
			}
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (experienceState_ == ES_SELECT_GAME)
	{
		return updateTimestamp;
	}

	ocean_assert(experienceState_ > ES_SELECT_GAME);

	updateBoardLocation(view->transformation());

	handleGamePieces(engine, timestamp);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (experienceState_ == ES_TRY_DETECT_REAL_BOARD)
	{
		if (gameSelectionTimestamp_.isValid() && gameSelectionTimestamp_ + 6.0 < timestamp)
		{
			if (!vrTableMenu_.isShown())
			{
				VRTableMenu::Entries menuEntries =
				{
					VRTableMenu::Entry("Yes, let's use a virtual board", "YES"),
					VRTableMenu::Entry("No, I have a board in front of me", "NO")
				};

				const VRTableMenu::Group menuGroup("No physical board? Want to use a virtual board?", std::move(menuEntries));

				vrTableMenu_.setMenuEntries(menuGroup, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), Scalar(0.02), Scalar(0), Scalar(0));
				vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.65))), view->transformation());
			}
		}

		if (vrTableMenu_.isShown())
		{
			std::string entryUrl;
			if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
			{
				vrTableMenu_.hide();

				if (entryUrl == "YES")
				{
					experienceState_ = ES_USE_VIRTUAL_BOARD;

					renderingTransformBoardGameReal_->setVisible(true);
					PlatformSpecific::get().vrControllerVisualizer().show();
					PlatformSpecific::get().vrControllerVisualizer().setControllerRayLength(0);

					patternTracker_.release();
				}
				else if (entryUrl == "NO")
				{
					experienceState_ = ES_USE_REAL_BOARD;

					PlatformSpecific::get().vrControllerVisualizer().hide();
				}
			}
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return updateTimestamp;
}

void SharedBoardGameExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (world_T_board_.isValid())
	{
		// the user can toggle the visibility of the board only when the board is already placed

		if (key == "A")
		{
			ocean_assert(renderingTransformBoardGameReal_);
			renderingTransformBoardGameReal_->setVisible(!renderingTransformBoardGameReal_->visible());
		}
	}

	if (key == "X")
	{
		hideHands_ = !hideHands_;
	}
}

void SharedBoardGameExperience::onMousePress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& screenPosition, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	screenPositionPressed_ = screenPosition;
}

void SharedBoardGameExperience::onMouseRelease(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& screenPosition, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (screenPosition.sqrDistance(screenPositionPressed_) < Numeric::sqr(10))
	{
		ocean_assert(renderingTransformBoardGameReal_);
		renderingTransformBoardGameReal_->setVisible(!renderingTransformBoardGameReal_->visible());
	}
	else if (Numeric::sqr(screenPositionPressed_.x() - screenPosition.x()) < Numeric::sqr(100) && screenPosition.y() - screenPositionPressed_.y() > 800)
	{
		// the user swiped from top to bottom

		if (renderingView_ && renderingView_->background())
		{
			renderingView_->background()->setVisible(!renderingView_->background()->visible());
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	screenPositionPressed_ = Vector2(Numeric::minValue(), Numeric::minValue());
}

bool SharedBoardGameExperience::loadGame(const std::string& url, const Rendering::EngineRef& engine)
{
	ocean_assert(!url.empty());
	ocean_assert(engine);

	ocean_assert(!patternTracker_);
	if (patternTracker_)
	{
		return false;
	}

	patternTracker_ = Devices::Manager::get().device("Pattern 6DOF Tracker");

	if (!patternTracker_)
	{
		Log::error() << "Failed to create patten tracker";
		return false;
	}

	if (!patternTracker_->setParameter("noFrameToFrameTracking", Value(true)) || !patternTracker_->setParameter("noDownsamplingOnAndroid", Value(true)))
	{
		Log::warning() << "Failed to configure pattern tracker";
	}

	if (Devices::VisualTrackerRef visualTracker = patternTracker_)
	{
#if XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		frameMedium_ = Media::Manager::get().newMedium("ColorCameraId:0");
#else
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
#endif

		if (frameMedium_.isNull())
		{
			Log::warning() << "We could not access the color camera, we try to use the stereo camera as a backup";

			frameMedium_ = Media::Manager::get().newMedium("StereoCamera0Id:0");
		}

		if (frameMedium_.isNull())
		{
			showMessage(MT_CAMERA_ACCESS_FAILED);
			return true;
		}

		frameMedium_->start();

		visualTracker->setInput(std::move(frameMedium_));
	}

	IO::File patternFile;
	Scalar patternSize = 0;

	if (url == "GAME_SIFT")
	{
		patternFile = IO::File("sift800x640.jpg");
		patternSize = Scalar(0.26);
	}
	else if (url == "GAME_LIFE")
	{
		patternFile = IO::File("boardgame_life.jpg");
		patternSize = Scalar(0.76);
	}
	else if (url == "GAME_RIDE")
	{
		patternFile = IO::File("boardgame_ride.jpg");
		patternSize = Scalar(0.79);
	}

	if (!patternFile.isValid() || patternSize == 0)
	{
		return false;
	}

	Frame patternFrame;

	if (Devices::ObjectTrackerRef objectTracker = patternTracker_)
	{
		const IO::Files resolvedFiles = IO::FileResolver::get().resolve(patternFile, true);

		if (!resolvedFiles.empty())
		{
			patternTrackerObjectId_ = objectTracker->registerObject(resolvedFiles.front()(), Vector3(patternSize, 0, 0));
		}

		if (patternTrackerObjectId_ == Devices::Tracker6DOF::invalidObjectId())
		{
			Log::error() << "Failed to register tracking pattern";
			return false;
		}

		patternFrame = IO::Image::readImage(resolvedFiles.front()());

		if (!patternFrame.isValid() || !CV::FrameConverter::Comfort::change(patternFrame, FrameType(patternFrame, patternFrame.hasAlphaChannel() ? FrameType::FORMAT_RGBA32 : FrameType::FORMAT_RGB24)))
		{
			Log::error() << "Failed to load tracking pattern";
			return false;
		}
	}

	if (!patternTracker_->start())
	{
		Log::error() << "Failed to start pattern tracker";
		return false;
	}

	const std::string vertsZoneName = "XRPlayground://SHARED_BOARD_GAME_EXPERIENCE_" + url;

	if (!initializeNetworkZone(vertsZoneName))
	{
		Log::error() << "Failed to initialize public verts zone";
		return false;
	}

	ocean_assert(patternFrame.isValid());
	boardDimension_ = Vector3(patternSize, 0, patternSize * Scalar(patternFrame.height()) / Scalar(patternFrame.width()));

	renderingTransformBoardGameReal_->clear();
	renderingTransformBoardGameVirtual_->clear();

	Rendering::TransformRef box = Rendering::Utilities::createBox(engine, boardDimension_, std::move(patternFrame));
	renderingTransformBoardGameReal_->addChild(std::move(box));

	if (url == "GAME_SIFT")
	{
		renderingTransformBoardGameVirtual_->addChild(Rendering::Utilities::createCoordinateSystem(engine, Scalar(0.05), Scalar(0.005), Scalar(0.0025)));
	}

	loadContent(renderingTransformBoardGame_, false /*floorLevel*/); // the shared content

	Network::Verts::NodeSpecification& gamePieceNodeSpecification = Network::Verts::NodeSpecification::newNodeSpecification(nodeTypeGamePiece_);
	gamePieceNodeSpecification.registerField<VectorF3>("position");
	gamePieceNodeSpecification.registerField<VectorF3>("color");
	gamePieceNodeSpecification.registerField<uint64_t>("dropcounter");
	gamePieceNodeSpecification.registerField<float>("dropheight");

	ocean_assert(vertsDriver_);
	newEntityScopedSubscription_ = vertsDriver_->addNewEntityCallback(std::bind(&SharedBoardGameExperience::onNewEntity, this, std::placeholders::_1, std::placeholders::_2), nodeTypeGamePiece_);

	return bool(newEntityScopedSubscription_);
}

void SharedBoardGameExperience::updateBoardLocation(const HomogenousMatrix4& world_T_device)
{
	ocean_assert(world_T_device.isValid());

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (world_T_board_.isValid())
	{
		if (!localUserGamePieceNode_)
		{
			// let's create a game piece for the local user, and make is visible to all other users

			// random but deterministic color

			RandomGenerator randomGenerator((unsigned int)(Platform::Meta::Avatars::Manager::get().userId() % uint64_t(0xFFFFFFFF)));
			const VectorF3 randomColor(RandomF::vector3(randomGenerator, 0.5f, 1.0f));

			// we set the position in front of the device

			const HomogenousMatrix4 board_T_device(world_T_board_.inverted() * world_T_device);

			Vector3 position(board_T_device.translation());
			position.y() = 0;

			const Scalar length = position.length();

			if (Numeric::isNotEqualEps(length))
			{
				// placing the game piece 20cm in front of the user
				position *= std::max(Scalar(0), length - Scalar(0.2)) / length;
			}

			ocean_assert(vertsDriver_);
			const Network::Verts::SharedEntity entity = vertsDriver_->newEntity({nodeTypeGamePiece_});

			if (entity)
			{
				localUserGamePieceNode_ = entity->node(nodeTypeGamePiece_);
				localUserGamePieceNode_->setField<VectorF3>(0u, VectorF3(position));
				localUserGamePieceNode_->setField<VectorF3>(1u, randomColor);
				localUserGamePieceNode_->setField<uint64_t>(2u, 0ull);

				const ScopedLock scopedLock(gamePieceLock_);

				gamePieceMap_.emplace(localUserGamePieceNode_->nodeId(), GamePiece(renderingTransformBoardGameVirtual_, true /*isLocal*/, localUserGamePieceNode_));
			}
		}
	}

	if (experienceState_ == ES_USE_VIRTUAL_BOARD)
	{
		if (world_T_board_.isValid())
		{
			const HomogenousMatrix4 board_T_world(world_T_board_.inverted());

			Platform::Meta::Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(board_T_world);
		}
		else
		{
			for (const TrackedRemoteDevice::RemoteType remoteType : PlatformSpecific::get().trackedRemoteDevice().activeHandheldDevices())
			{
				HomogenousMatrix4 world_T_remoteDevice(false);
				if (PlatformSpecific::get().trackedRemoteDevice().pose(remoteType, &world_T_remoteDevice) && world_T_remoteDevice.isValid())
				{
					renderingTransformBoardGame_->setVisible(true);

					const Vector3 controllerOffset(0, Scalar(-0.035), 0);

					Vector3 userDirection(world_T_device.translation() - world_T_remoteDevice.translation()); // vector pointing towards the user, will be the z-axis of the board
					userDirection.y() = 0;

					if (userDirection.length() > Scalar(0.10))
					{
						userDirection.normalize(); //

						const Vector3 yAxis = Vector3(0, 1, 0);
						const Vector3 xAxis = -userDirection.cross(yAxis).normalized();

						const SquareMatrix3 rotationMatrix(xAxis, yAxis, userDirection);
						ocean_assert(rotationMatrix.isOrthonormal());

						if (PlatformSpecific::get().trackedRemoteDevice().buttonsPressed(remoteType) & (ovrButton_B | ovrButton_Y))
						{
							virtualBoardRotation_ += Numeric::pi_2();
						}

						const Quaternion boardRotation(Vector3(0, 1, 0), virtualBoardRotation_);

						const HomogenousMatrix4 world_T_board(HomogenousMatrix4(world_T_remoteDevice.translation() + controllerOffset, rotationMatrix) * HomogenousMatrix4(boardRotation));
						renderingTransformBoardGame_->setTransformation(world_T_board);

						if (PlatformSpecific::get().trackedRemoteDevice().buttonsPressed(remoteType) & ovrButton_Trigger)
						{
							world_T_board_ = world_T_board;

							renderingTransformBoardGame_->setTransformation(HomogenousMatrix4(true));
							PlatformSpecific::get().vrControllerVisualizer().hide();
						}
					}

					break;
				}
			}
		}

		return;
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (!patternTracker_)
	{
		return;
	}

	ocean_assert(frameMedium_);
	const Devices::Tracker6DOF::Tracker6DOFSampleRef patternSample(patternTracker_->sample());

	if (patternSample && patternSample->timestamp() > patternTrackerSampleTimestamp_)
	{
		patternTrackerSampleTimestamp_ = patternSample->timestamp();

		if (patternSample->objectIds().size() == 1)
		{
			const HomogenousMatrix4 pattern_T_camera(patternSample->positions().front(), patternSample->orientations().front());
			const HomogenousMatrix4 board_T_camera(HomogenousMatrix4(-boardDimension_ * Scalar(0.5)) * pattern_T_camera); // moving origin into the center of the pattern/board

			HomogenousMatrix4 world_T_camera(false);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
			const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(patternTrackerSampleTimestamp_);
			const HomogenousMatrix4 device_T_camera = HomogenousMatrix4(frameMedium_->device_T_camera());
			world_T_camera = world_T_device * device_T_camera;
#else
			const Devices::Tracker6DOF::Tracker6DOFSampleRef slamSample(slamTracker_->sample(patternTrackerSampleTimestamp_));

			if (slamSample)
			{
				world_T_camera = HomogenousMatrix4(slamSample->positions().front(), slamSample->orientations().front());
			}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

			if (world_T_camera.isValid())
			{
				const HomogenousMatrix4 world_T_roughBoard = world_T_camera * board_T_camera.inverted();

				if (world_T_roughBoard.yAxis() * Vector3(0, 1, 0) >= Numeric::cos(Numeric::deg2rad(5)))
				{
					if (recentScale_ > 0)
					{
						HomogenousMatrix4 roughBoard_T_world(world_T_roughBoard.inverted());
						roughBoard_T_world.applyScale(Vector3(recentScale_, recentScale_, recentScale_));

						ocean_assert(world_T_board_.isValid());
						const Vector3 translationError = (roughBoard_T_world * world_T_board_).translation();

						if (translationError.sqr() >= Numeric::sqr(Scalar(0.05)))
						{
							// the pattern has moved significantly, we reset the transformation history

							world_T_cameras_.clear();
							board_T_cameras_.clear();

							recentScale_ = 0;
						}
					}

					world_T_cameras_.emplace_back(world_T_camera);
					board_T_cameras_.emplace_back(board_T_camera);

					constexpr size_t maxCorrespondences = 40;
					const size_t offset = size_t(std::max(0, int(world_T_cameras_.size()) - int(maxCorrespondences)));

					const size_t correspondences = world_T_cameras_.size() - offset;

					if (correspondences >= 5)
					{
						HomogenousMatrix4 board_T_world(false);
						Scalar scale = 0;
						if (Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(world_T_cameras_.data() + offset, board_T_cameras_.data() + offset, correspondences, board_T_world, Scalar(0.5), Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.5) && scale <= Scalar(1.5))
						{
							board_T_world.applyScale(Vector3(scale, scale, scale));

							world_T_board_ = board_T_world.inverted();
							recentScale_ = scale;

							if (experienceState_ == ES_TRY_DETECT_REAL_BOARD)
							{
								experienceState_ = ES_USE_REAL_BOARD;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
								if (vrTableMenu_.isShown())
								{
									vrTableMenu_.hide(); // we may show the dialog to select a virtual board game
								}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
							}

							Platform::Meta::Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(HomogenousMatrix4(board_T_world.translation(), board_T_world.rotation()));
							renderingTransformBoardGame_->setVisible(true);
						}

						if (world_T_cameras_.size() >= maxCorrespondences * 2)
						{
							world_T_cameras_ = HomogenousMatrices4(world_T_cameras_.cbegin() + world_T_cameras_.size() - maxCorrespondences, world_T_cameras_.cend());
							board_T_cameras_ = HomogenousMatrices4(board_T_cameras_.cbegin() + board_T_cameras_.size() - maxCorrespondences, board_T_cameras_.cend());
						}
					}
				}
			}
		}
	}
}

void SharedBoardGameExperience::handleGamePieces(const Rendering::EngineRef& engine, const Timestamp& timestamp)
{
	TemporaryScopedLock scopedLock(gamePieceLock_);

	for (GamePieceMap::iterator iGamePiece = gamePieceMap_.begin(); iGamePiece != gamePieceMap_.end(); /*noop*/)
	{
		GamePiece& gamePiece = iGamePiece->second;

		const bool useStrongShadow = renderingTransformBoardGameReal_.isNull() || !renderingTransformBoardGameReal_->visible();

		gamePiece.preUpdate(engine, useStrongShadow, timestamp);

		if (gamePiece.hasBeenDeleted())
		{
			iGamePiece = gamePieceMap_.erase(iGamePiece);
			continue;
		}

		++iGamePiece;
	}

	scopedLock.release();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (!world_T_board_.isValid())
	{
		return;
	}

	if (localUserGamePieceNode_)
	{
		GamePieceMap::iterator iGamePiece = gamePieceMap_.find(localUserGamePieceNode_->nodeId());
		if (iGamePiece == gamePieceMap_.cend())
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		iGamePiece->second.handleInteraction(world_T_board_.inverted(), timestamp, interactionState_);
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

void SharedBoardGameExperience::onInitializeLocalUser(HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar)
{
	// no initial user placement before the board is tracked

	remoteHeadsetWorld_T_remoteAvatar.toNull();
}

void SharedBoardGameExperience::onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	const ScopedLock scopedLock(gamePieceLock_);

	Network::Verts::SharedNode node = entity->node(nodeTypeGamePiece_);
	ocean_assert(node);

	const Network::Verts::Node::NodeId nodeId = node->nodeId();

	ocean_assert(gamePieceMap_.find(nodeId) == gamePieceMap_.cend());
	gamePieceMap_.emplace(nodeId, GamePiece(renderingTransformBoardGameVirtual_, false /*isLocal*/, std::move(node)));
}

#else // OCEAN_PLATFORM_BUILD_MOBILE

SharedBoardGameExperience::~SharedBoardGameExperience()
{
	// nothing to do here
}

#endif // OCEAN_PLATFORM_BUILD_MOBILE

std::unique_ptr<XRPlaygroundExperience> SharedBoardGameExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new SharedBoardGameExperience());
}

}

}
