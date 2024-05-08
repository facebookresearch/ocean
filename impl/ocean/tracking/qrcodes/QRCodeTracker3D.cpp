/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/qrcodes/QRCodeTracker3D.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/geometry/RANSAC.h"

#include "ocean/tracking/oculustags/Utilities.h"

#include "ocean/tracking/qrcodes/QRCodesDebugElements.h"

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	#include "ocean/cv/FrameConverter.h"
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

using namespace Ocean::CV::Detector::QRCodes;

namespace Ocean
{

namespace Tracking
{

namespace QRCodes
{

void QRCodeTracker3D::ObservationHistory::addObservation(const SharedAnyCamera& sharedAnyCamera, const HomogenousMatrix4& world_T_camera, Geometry::ObjectPoints&& objectPoints, Geometry::ImagePoints&& imagePoints)
{
	ocean_assert(sharedAnyCamera != nullptr && sharedAnyCamera->isValid());
	ocean_assert(world_T_camera.isValid());
	// ocean_assert(objectPoints.size() >= 3 && objectPoints.size() == imagePoints.size());

	sharedAnyCameras_.emplace_back(sharedAnyCamera);
	world_T_cameras_.emplace_back(world_T_camera);
	objectPointsGroups_.emplace_back(std::move(objectPoints));
	imagePointsGroups_.emplace_back(std::move(imagePoints));
}

size_t QRCodeTracker3D::ObservationHistory::removeObservations(const SharedAnyCamera& sharedAnyCamera, const HomogenousMatrix4& world_T_code, const Scalar maxProjectionError, const Scalar maxOutliersPercent)
{
	ocean_assert(sharedAnyCamera != nullptr && sharedAnyCamera->isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(maxProjectionError > Scalar(0));
	ocean_assert(maxOutliersPercent > Scalar(0));

	// Starting with the newest observation iterate in reverse order until the first observation is found that does not match the given pose.

	constexpr size_t invalidIndex = size_t(-1);
	size_t firstViolatingObservation = invalidIndex;

	const Scalar maxSqrProjectionError = Numeric::sqr(maxProjectionError);

	for (size_t iObservation = size() - 1; iObservation < size(); --iObservation)
	{
		ocean_assert(iObservation < world_T_cameras_.size());
		ocean_assert(iObservation < objectPointsGroups_.size());
		ocean_assert(iObservation < imagePointsGroups_.size());
		ocean_assert(world_T_cameras_[iObservation].isValid());

		const HomogenousMatrix4 camera_T_world = world_T_cameras_[iObservation].inverted();
		const HomogenousMatrix4 camera_T_code = camera_T_world * world_T_code;

		const Geometry::ObjectPoints& objectPoints = objectPointsGroups_[iObservation];
		const Geometry::ImagePoints& imagePoints = imagePointsGroups_[iObservation];
		ocean_assert(!objectPoints.empty() && objectPoints.size() == imagePoints.size());

		const size_t numberMaxOutliers = size_t(Scalar(objectPoints.size()) * maxOutliersPercent + Scalar(0.5));
		size_t numberOutliers = 0;

		for (size_t iPoint = 0; iPoint < objectPoints.size(); ++iPoint)
		{
			const Vector2 projectedPoint = sharedAnyCamera->projectToImage(camera_T_code * objectPoints[iPoint]);

			const Scalar sqrProjectionError = imagePoints[iPoint].sqrDistance(projectedPoint);

			if (sqrProjectionError > maxSqrProjectionError)
			{
				numberOutliers++;
			}

			if (numberOutliers > numberMaxOutliers)
			{
				firstViolatingObservation = iObservation;
				break;
			}
		}
	}

	if (firstViolatingObservation == invalidIndex)
	{
		return 0;
	}

	// Delete the first violating observation and everything else that comes before it.

	const size_t numberElementsToRemove = firstViolatingObservation + 1;

	sharedAnyCameras_.erase(sharedAnyCameras_.begin(), sharedAnyCameras_.begin() + numberElementsToRemove);

	world_T_cameras_.erase(world_T_cameras_.begin(), world_T_cameras_.begin() + numberElementsToRemove);

	objectPointsGroups_.erase(objectPointsGroups_.begin(), objectPointsGroups_.begin() + numberElementsToRemove);
	imagePointsGroups_.erase(imagePointsGroups_.begin(), imagePointsGroups_.begin() + numberElementsToRemove);

	return numberElementsToRemove;
}

const Geometry::ObjectPoints& QRCodeTracker3D::ObservationHistory::latestObjectPoints() const
{
	ocean_assert(!objectPointsGroups_.empty() && objectPointsGroups_.size() == imagePointsGroups_.size());

	return objectPointsGroups_.back();
}

const Geometry::ImagePoints& QRCodeTracker3D::ObservationHistory::latestImagePoints() const
{
	ocean_assert(!imagePointsGroups_.empty() && objectPointsGroups_.size() == imagePointsGroups_.size());

	return imagePointsGroups_.back();
}

size_t QRCodeTracker3D::ObservationHistory::size()
{
	ocean_assert(sharedAnyCameras_.size() == world_T_cameras_.size());
	ocean_assert(sharedAnyCameras_.size() == objectPointsGroups_.size());
	ocean_assert(sharedAnyCameras_.size() == imagePointsGroups_.size());

	return sharedAnyCameras_.size();
}

void QRCodeTracker3D::ObservationHistory::clear()
{
	sharedAnyCameras_.clear();
	world_T_cameras_.clear();
	objectPointsGroups_.clear();
	imagePointsGroups_.clear();
}

QRCodeTracker3D::TrackedQRCode::TrackedQRCode(CV::Detector::QRCodes::QRCode&& code, HomogenousMatrix4&& world_T_code, const Scalar codeSize, Geometry::ObjectPoints&& trackingObjectPoints, const TrackingState trackingState, const Timestamp trackingTimestamp) :
	code_(std::move(code)),
	world_T_code_(std::move(world_T_code)),
	codeSize_(codeSize),
	trackingState_(trackingState),
	trackingTimestamp_(trackingTimestamp),
	trackingObjectPoints_(std::move(trackingObjectPoints))
{
	// Nothing else to do.
}

QRCodeTracker3D::TrackedQRCode::TrackedQRCode(const CV::Detector::QRCodes::QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, const Geometry::ObjectPoints& trackingObjectPoints, const TrackingState trackingState, const Timestamp trackingTimestamp) :
	code_(code),
	world_T_code_(world_T_code),
	codeSize_(codeSize),
	trackingState_(trackingState),
	trackingTimestamp_(trackingTimestamp),
	trackingObjectPoints_(trackingObjectPoints)
{
	// Nothing else to do.
}

QRCodeTracker3D::QRCodeTracker3D(const Parameters& parameters, CallbackQRCodeDetection3D callbackQRCodeDetection3D, CallbackNewQRCode callbackNewQRCode, const bool forceDetectionOnlyAndAllow2DCodes) :
	parameters_(parameters),
	forceDetectionOnlyAndAllow2DCodes_(forceDetectionOnlyAndAllow2DCodes)
{
	if (callbackQRCodeDetection3D != nullptr)
	{
		callbackQRCodeDetection3D_ = callbackQRCodeDetection3D;
	}

	if (callbackNewQRCode != nullptr)
	{
		callbackNewQRCode_ = callbackNewQRCode;
	}

	ocean_assert(callbackQRCodeDetection3D_ != nullptr);
}

const QRCodeTracker3D::TrackedQRCodesMap& QRCodeTracker3D::trackQRCodes(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, Worker* worker)
{
	ocean_assert(sharedAnyCameras.size() == yFrames.size());
	ocean_assert(device_T_cameras.size() == yFrames.size());
	ocean_assert(world_T_device.isValid());

	ocean_assert(previousSharedAnyCameras_.empty() || previousSharedAnyCameras_.size() == sharedAnyCameras.size());

#if defined(OCEAN_DEBUG)
	for (const SharedAnyCamera& sharedAnyCamera : sharedAnyCameras)
	{
		ocean_assert(sharedAnyCamera && sharedAnyCamera->isValid());
	}

	for (const Frame& yFrame : yFrames)
	{
		ocean_assert(yFrame.isValid());
		ocean_assert(yFrame.width() >= 29u && yFrame.height() >= 29u);
		ocean_assert(FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8));
		ocean_assert(yFrame.timestamp() != Timestamp(false));
	}

	for (const HomogenousMatrix4& device_T_camera : device_T_cameras)
	{
		ocean_assert(device_T_camera.isValid());
	}
#endif

	frameCounter_++;

	if (yFrames.size() < 2)
	{
		Log::error() << "Two or more cameras must be provided!";
		ocean_assert(false && "Two or more cameras must be provided!");

		return trackedQRCodesMap_;
	}

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	{
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_CAMERA_FRAMES))
		{
			for (size_t iFrame = 0; iFrame < yFrames.size(); ++iFrame)
			{
				const Frame& yFrame = yFrames[iFrame];
				ocean_assert(yFrame.isValid());

				Frame rgbFrame;
				CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, worker);
				ocean_assert(rgbFrame.isValid());

				QRCodesDebugElements::get().updateElement(QRCodesDebugElements::EI_CAMERA_FRAMES, std::move(rgbFrame), /* explicitHierarchy */ { String::toAString(iFrame) });
			}
		}
	}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	const Timestamp trackingTimestamp = yFrames.front().timestamp();

	if (trackingTimestamp.isInvalid())
	{
		ocean_assert(false && "This should never happen!");
		return invalidTrackedQRCodesMap();
	}

	if (forceDetectionOnlyAndAllow2DCodes_)
	{
		// Tracking has been disabled, so all tracking data should be removed at the beginning of each new iteration.
		trackedQRCodesMap_.clear();
	}

	// Tracking

	std::vector<CV::FramePyramid> framePyramids;
	unsigned int numberTrackedCodes = 0u;

	if (!forceDetectionOnlyAndAllow2DCodes_)
	{
		framePyramids.reserve(yFrames.size());

		for (const Frame& yFrame : yFrames)
		{
			ocean_assert(parameters_.trackingNumberFramePyramidLayers_ != 0u);
			CV::FramePyramid framePyramid(yFrame, parameters_.trackingNumberFramePyramidLayers_, true /*copyFirstLayer*/, worker);
			ocean_assert(framePyramid.isValid());

			framePyramids.emplace_back(std::move(framePyramid));
		}

		ocean_assert(previousFramePyramids_.empty() || previousFramePyramids_.size() == framePyramids.size());

		if (!trackedQRCodesMap_.empty())
		{
			TrackedQRCodesMap::iterator trackedQRCodeIter = trackedQRCodesMap_.begin();

			while (trackedQRCodeIter != trackedQRCodesMap_.end())
			{
				TrackedQRCode& trackedCode = trackedQRCodeIter->second;

				if (trackedCode.isValid() && trackQRCode(previousSharedAnyCameras_[0], previousSharedAnyCameras_[1], previousWorld_T_device_, previousDevice_T_cameras_[0], previousDevice_T_cameras_[1], sharedAnyCameras[0], sharedAnyCameras[1], world_T_device, device_T_cameras[0], device_T_cameras[1], previousFramePyramids_[0], previousFramePyramids_[1], framePyramids[0], framePyramids[1], trackingTimestamp, trackedCode))
				{
					ocean_assert(trackedCode.trackingState() == QRCodeTracker3D::TS_TRACKING);

					numberTrackedCodes++;
				}
				else
				{
					trackedCode.setTrackingLost();
				}

				if (!trackedCode.isValid() || (trackedCode.trackingState() == TS_LOST && trackedCode.trackingTimestamp() + parameters_.trackingLostGraceTimeout_ < trackingTimestamp))
				{
					trackedQRCodeIter = trackedQRCodesMap_.erase(trackedQRCodeIter);
					continue;
				}

				trackedQRCodeIter++;
			}
		}
	}

	// Detection

	if (trackedQRCodesMap_.empty() || numberTrackedCodes == 0u || (frameCounter_ % parameters_.detectionCadence_ == 0u) || forceDetectionOnlyAndAllow2DCodes_)
	{
		CV::Detector::QRCodes::QRCodes newCodes;
		HomogenousMatrices4 world_T_newCodes;
		Scalars newCodeSizes;

		const bool allow2DCodes = forceDetectionOnlyAndAllow2DCodes_;

		if (callbackQRCodeDetection3D_(sharedAnyCameras, yFrames, world_T_device, device_T_cameras, newCodes, world_T_newCodes, newCodeSizes, worker, allow2DCodes))
		{
			ocean_assert(newCodes.size() == world_T_newCodes.size());
			ocean_assert(newCodes.size() == newCodeSizes.size());

			for (size_t i = 0; i < newCodes.size(); ++i)
			{
				CV::Detector::QRCodes::QRCode& newCode = newCodes[i];
				HomogenousMatrix4& world_T_newCode = world_T_newCodes[i];
				const Scalar newCodeSize = newCodeSizes[i];

				const bool is2DCode = newCodeSize <= Scalar(0) || !world_T_newCode.isValid();

				ocean_assert(forceDetectionOnlyAndAllow2DCodes_ || !is2DCode);

				ObjectId objectId = invalidObjectId();
				TrackedQRCodesMap::iterator iter;

				if (!is2DCode && isAlreadyTracked(trackedQRCodesMap_, newCode, world_T_newCode, newCodeSize, objectId))
				{
					// Update the code that has been tracked already
					iter = trackedQRCodesMap_.find(objectId);
					ocean_assert(iter != trackedQRCodesMap_.end());

					if (iter->second.trackingState() != TS_TRACKING)
					{
						iter->second.updateTrackingPose(std::move(world_T_newCode), newCodeSize, trackingTimestamp);
					}
				}
				else
				{
					// Add the code to the map of tracked codes.
					const ObjectId newCodeObjectId = objectIdCounter_;

					++objectIdCounter_;

					if (callbackNewQRCode_ != nullptr)
					{
						callbackNewQRCode_(newCode, world_T_newCode, newCodeSize, newCodeObjectId);
					}

					// Define object points that can be used for tracking, if applicable
					Geometry::ObjectPoints trackingObjectPoints;

					if (!is2DCode)
					{
						ocean_assert(newCodeSize > Scalar(0));

						trackingObjectPoints = createTrackingObjectPoints(newCode, newCodeSize);
					}

					iter = trackedQRCodesMap_.end();
					bool isAdded = false;
					std::tie(iter, isAdded) = trackedQRCodesMap_.emplace(std::piecewise_construct, std::forward_as_tuple(newCodeObjectId), std::forward_as_tuple(std::move(newCode), std::move(world_T_newCode), newCodeSize, std::move(trackingObjectPoints), /* trackingState */ TS_TRACKING, trackingTimestamp));

					ocean_assert(isAdded && iter != trackedQRCodesMap_.end());
				}
			}
		}
	}

	previousSharedAnyCameras_ = sharedAnyCameras;
	previousFramePyramids_ = std::move(framePyramids);

	previousWorld_T_device_ = world_T_device;
	previousDevice_T_cameras_ = std::move(device_T_cameras);

	return trackedQRCodesMap_;
}

constexpr QRCodeTracker3D::ObjectId QRCodeTracker3D::invalidObjectId()
{
	return ObjectId(-1);
}

bool QRCodeTracker3D::trackQRCode(const SharedAnyCamera& previousSharedAnyCameraA, const SharedAnyCamera& previousSharedAnyCameraB, const HomogenousMatrix4& previousWorld_T_device, const HomogenousMatrix4& previousDevice_T_cameraA, const HomogenousMatrix4& previousDevice_T_cameraB, const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const CV::FramePyramid& previousFramePyramidA, const CV::FramePyramid& previousFramePyramidB, const CV::FramePyramid& framePyramidA, const CV::FramePyramid& framePyramidB, const Timestamp& trackingTimestamp, TrackedQRCode& trackedCode)
{
	ocean_assert(previousSharedAnyCameraA != nullptr && previousSharedAnyCameraA->isValid());
	ocean_assert(previousSharedAnyCameraB != nullptr && previousSharedAnyCameraB->isValid());
	ocean_assert(previousWorld_T_device.isValid());
	ocean_assert(previousDevice_T_cameraA.isValid());
	ocean_assert(previousDevice_T_cameraB.isValid());
	ocean_assert(sharedAnyCameraA != nullptr && sharedAnyCameraA->isValid());
	ocean_assert(sharedAnyCameraB != nullptr && sharedAnyCameraB->isValid());
	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameraA.isValid());
	ocean_assert(device_T_cameraB.isValid());
	ocean_assert(previousFramePyramidA.isValid());
	ocean_assert(previousFramePyramidB.isValid());
	ocean_assert(framePyramidA.isValid());
	ocean_assert(framePyramidB.isValid());

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	Frame debugFrameStereo;
	Frame debugFrameA;
	Frame debugFrameB;

	{
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
		{
			const Frame* yFrames[2] =
			{
				&framePyramidA.finestLayer(),
				&framePyramidB.finestLayer(),
			};

			debugFrameStereo = Frame(FrameType(yFrames[0]->width() * 2u, yFrames[0]->height(), FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

			debugFrameA = debugFrameStereo.subFrame(0u, 0u, yFrames[0]->width(), yFrames[0]->height());
			debugFrameB = debugFrameStereo.subFrame(yFrames[0]->width(), 0u, yFrames[0]->width(), yFrames[0]->height());

			for (size_t iFrame = 0; iFrame < 2; ++iFrame)
			{
				const Frame& yFrame = *yFrames[iFrame];
				ocean_assert(yFrame.isValid());

				Frame rgbFrame;
				CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, worker);
				ocean_assert(rgbFrame.isValid());

				Frame& debugFrame = iFrame == 0 ? debugFrameA : debugFrameB;

				for (unsigned int iRow = 0u; iRow < rgbFrame.height(); ++iRow)
				{
					const uint8_t* rgbFrameRow = rgbFrame.constrow<uint8_t>(iRow);
					uint8_t* debugFrameRow = debugFrame.row<uint8_t>(iRow);

					memcpy(debugFrameRow, rgbFrameRow, rgbFrame.planeWidthBytes(/* planeIndex */ 0u));
				}
			}
		}
	}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	if (trackedCode.trackingState() != TS_TRACKING)
	{
#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
		{
			QRCodesDebugElements::get().updateElement(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS, std::move(debugFrameStereo));
		}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

		return false;
	}

	FinderPatterns finderPatternsA;
	FinderPatterns finderPatternsB;

	for (bool useCameraA : { true, false })
	{
		const SharedAnyCamera& previousSharedAnyCamera = useCameraA ? previousSharedAnyCameraA : previousSharedAnyCameraB;
		const SharedAnyCamera& sharedAnyCamera = useCameraA ? sharedAnyCameraA : sharedAnyCameraB;

		const HomogenousMatrix4& previousDevice_T_camera = useCameraA ? previousDevice_T_cameraA : previousDevice_T_cameraB;
		const HomogenousMatrix4& device_T_camera = useCameraA ? device_T_cameraA : device_T_cameraB;

		const HomogenousMatrix4 world_T_prevCamera = previousWorld_T_device * previousDevice_T_camera;
		const HomogenousMatrix4 world_T_camera = world_T_device * device_T_camera;

		const HomogenousMatrix4 flippedPrevCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_prevCamera);
		const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);

		const CV::FramePyramid& previousFramePyramid = useCameraA ? previousFramePyramidA : previousFramePyramidB;
		const CV::FramePyramid& framePyramid = useCameraA ? framePyramidA : framePyramidB;

		FinderPatterns& finderPatterns = useCameraA ? finderPatternsA : finderPatternsB;

		const Scalar codeSize_2 = trackedCode.codeSize() * Scalar(0.5);
		const CV::Detector::QRCodes::Utilities::CoordinateSystem coordinateSystem(trackedCode.code().version(), codeSize_2);

		const unsigned int modulesPerSide = QRCode::modulesPerSide(trackedCode.code().version());
		const Scalar moduleSize = trackedCode.codeSize() / Scalar(modulesPerSide);

		// Use the centers of the finder patterns for pattern tracking

		const Scalar leftCenterX = -codeSize_2 + moduleSize * Scalar(3.5);
		const Scalar rightCenterX = codeSize_2 - moduleSize * Scalar(3.5);

		const Scalar topCenterY = codeSize_2 - moduleSize * Scalar(3.5);
		const Scalar bottomCenterY = -codeSize_2 + moduleSize * Scalar(3.5);

		Geometry::ObjectPoints objectPoints =
		{
			Vector3(leftCenterX, topCenterY, Scalar(0)), // TL
			Vector3(leftCenterX, bottomCenterY, Scalar(0)), // BL
			Vector3(rightCenterX, topCenterY, Scalar(0)), // TR
		};

		Vectors2 previousImagePoints;
		Vectors2 predictedImagePoints;

		for (size_t iPoint = 0; iPoint < objectPoints.size(); ++iPoint)
		{
			const Vector3 previousWorldPoint = trackedCode.world_T_code() * objectPoints[iPoint];

			const Vector2 previousImagePoint = previousSharedAnyCamera->projectToImageIF(flippedPrevCamera_T_world * previousWorldPoint);
			const Vector2 predictedImagePoint = sharedAnyCamera->projectToImageIF(flippedCamera_T_world * previousWorldPoint);

			if (previousSharedAnyCamera->isInside(previousImagePoint, /* border */ Scalar(10)) && sharedAnyCamera->isInside(predictedImagePoint, /* border */ Scalar(10)))
			{
				previousImagePoints.emplace_back(previousImagePoint);
				predictedImagePoints.emplace_back(predictedImagePoint);
			}
		}

		ocean_assert(previousImagePoints.size() == predictedImagePoints.size());

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
		{
			Frame& debugFrame = useCameraA ? debugFrameA : debugFrameB;

			for (size_t iPoint = 0; iPoint < previousImagePoints.size(); ++iPoint)
			{
				CV::Canvas::line<3u>(debugFrame, previousImagePoints[iPoint], predictedImagePoints[iPoint], CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::line<1u>(debugFrame, previousImagePoints[iPoint], predictedImagePoints[iPoint], CV::Canvas::white(debugFrame.pixelFormat()));
			}

			for (const Vector2& previousImagePoint : previousImagePoints)
			{
				CV::Canvas::point<3u>(debugFrame, previousImagePoint, CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::point<1u>(debugFrame, previousImagePoint, CV::Canvas::red(debugFrame.pixelFormat()));
			}

			for (const Vector2& predictedImagePoint : predictedImagePoints)
			{
				CV::Canvas::point<3u>(debugFrame, predictedImagePoint, CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::point<1u>(debugFrame, predictedImagePoint, CV::Canvas::yellow(debugFrame.pixelFormat()));
			}
		}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

		if (previousImagePoints.size() < 3)
		{
			continue;
		}

		Vectors2 trackedImagePoints;

		if (framePyramid.finestLayer().width() <= 640u)
		{
			CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<1u, 7u>(previousFramePyramid, framePyramid, previousImagePoints, predictedImagePoints, trackedImagePoints, /* coarsestLayerRadius */ 2u);
		}
		else
		{
			CV::Advanced::AdvancedMotionSSD::trackPointsSubPixelMirroredBorder<1u, 15u>(previousFramePyramid, framePyramid, previousImagePoints, predictedImagePoints, trackedImagePoints, /* coarsestLayerRadius */ 2u);
		}

		ocean_assert(trackedImagePoints.size() == predictedImagePoints.size());

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
		{
			Frame& debugFrame = useCameraA ? debugFrameA : debugFrameB;

			for (size_t iPoint = 0; iPoint < previousImagePoints.size(); ++iPoint)
			{
				CV::Canvas::line<3u>(debugFrame, predictedImagePoints[iPoint], trackedImagePoints[iPoint], CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::line<1u>(debugFrame, predictedImagePoints[iPoint], trackedImagePoints[iPoint], CV::Canvas::yellow(debugFrame.pixelFormat()));
			}

			for (const Vector2& predictedImagePoint : predictedImagePoints)
			{
				CV::Canvas::point<3u>(debugFrame, predictedImagePoint, CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::point<1u>(debugFrame, predictedImagePoint, CV::Canvas::yellow(debugFrame.pixelFormat()));
			}

			for (const Vector2& trackedImagePoint : trackedImagePoints)
			{
				CV::Canvas::point<3u>(debugFrame, trackedImagePoint, CV::Canvas::black(debugFrame.pixelFormat()));
				CV::Canvas::point<1u>(debugFrame, trackedImagePoint, CV::Canvas::green(debugFrame.pixelFormat()));
			}
		}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

		// In the vicinity of the tracked image points, try to find the finder patterns again

		ocean_assert(trackedImagePoints.size() == 3);
		const Scalar averageDistanceInPixels = Scalar(0.5) * (trackedImagePoints[0].distance(trackedImagePoints[1]) + trackedImagePoints[0].distance(trackedImagePoints[2]));

		ocean_assert(modulesPerSide >= 21u);
		const Scalar imageModuleSizeInPixels = averageDistanceInPixels / Scalar(modulesPerSide - 7u);

		const unsigned int cropSize = std::max(29u, (unsigned int)(imageModuleSizeInPixels * Scalar(27) + Scalar(0.5)));
		const Scalar cropSize_2 = Scalar(cropSize) * Scalar(0.5);

		const Frame& frame = framePyramid.finestLayer();

		for (size_t iPoint = 0; iPoint < trackedImagePoints.size(); ++iPoint)
		{
			unsigned int leftX = 0u;
			unsigned int topY = 0u;

			if (trackedImagePoints[iPoint].x() > cropSize_2)
			{
				leftX = (unsigned int)(trackedImagePoints[iPoint].x() - cropSize_2);
			}

			if (trackedImagePoints[iPoint].y() > cropSize_2)
			{
				topY = (unsigned int)(trackedImagePoints[iPoint].y() - cropSize_2);
			}

			unsigned int rightX = frame.width() - 1u;
			unsigned int bottomY = frame.height() - 1u;

			if (leftX + cropSize - 1u < frame.width() - 1u)
			{
				rightX = leftX + cropSize - 1u;
			}

			if (topY + cropSize - 1u < frame.height() - 1u)
			{
				bottomY = topY + cropSize - 1u;
			}

			ocean_assert(leftX < rightX && topY < bottomY && rightX < frame.width() && bottomY < frame.height());

			if (rightX - leftX + 1u < 21u || bottomY - topY + 1u < 21u)
			{
				// Crop is too small to find anything.
				continue;
			}

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
			{
				if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
				{
					Frame& debugFrame = useCameraA ? debugFrameA : debugFrameB;

					const Vector2 cropCorners[4] =
					{
						Vector2(leftX, topY),
						Vector2(leftX, bottomY),
						Vector2(rightX, bottomY),
						Vector2(rightX, topY),
					};

					CV::Canvas::line<1u>(debugFrame, cropCorners[3], cropCorners[0], CV::Canvas::red(debugFrame.pixelFormat()));

					for (size_t iCorner = 1; iCorner < 4; ++iCorner)
					{
						CV::Canvas::line<1u>(debugFrame, cropCorners[iCorner], cropCorners[iCorner - 1], CV::Canvas::red(debugFrame.pixelFormat()));
					}

					const FinderPatterns& finderPatterns = useCameraA ? finderPatternsA : finderPatternsB;

					for (const FinderPattern& finderPattern : finderPatterns)
					{
						CV::Detector::QRCodes::Utilities::drawFinderPattern(debugFrame, finderPattern, CV::Canvas::yellow(debugFrame.pixelFormat()));
					}
				}
			}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

			const unsigned int actualCropWidth = rightX - leftX + 1u;
			const unsigned int actualCropHeight = bottomY - topY + 1;
			ocean_assert(leftX + actualCropWidth <= frame.width() && topY + actualCropHeight <= frame.height());

			const Frame cropFrame = frame.subFrame(leftX, topY, actualCropWidth, actualCropHeight);
			ocean_assert(cropFrame.isValid());

			const Vector2 cropTopLeftCorner((Scalar)leftX, (Scalar)topY);

			constexpr unsigned minDistance = 5u;
			const FinderPatterns cropFinderPatterns = FinderPatternDetector::detectFinderPatterns(cropFrame.constdata<uint8_t>(), cropFrame.width(), cropFrame.height(), minDistance, cropFrame.paddingElements(), /* worker */ nullptr);

			for (const FinderPattern& cropFinderPattern : cropFinderPatterns)
			{
				// Translate all positions from the crop image to the full image.
				const Vector2 finderPatternPosition = cropFinderPattern.position() + cropTopLeftCorner;

				// Filter out potential duplicates
				constexpr Scalar minSqrDistance = Scalar(minDistance) * Scalar(minDistance);
				bool addFinderPattern = true;

				for (const FinderPattern& finderPattern : finderPatterns)
				{
					if (finderPattern.position().sqrDistance(finderPatternPosition) < minSqrDistance)
					{
						addFinderPattern = false;
						break;
					}
				}

				if (addFinderPattern)
				{
					Vector2 corners[4];
					const Vector2* otherCorners = cropFinderPattern.corners();

					if (otherCorners != nullptr)
					{
						for (size_t iCorner = 0; iCorner < 4; ++iCorner)
						{
							corners[iCorner] = cropTopLeftCorner + otherCorners[iCorner];
						}
					}

					finderPatterns.emplace_back(finderPatternPosition, cropFinderPattern.length(), cropFinderPattern.centerIntensity(), cropFinderPattern.grayThreshold(), cropFinderPattern.symmetryScore(), corners, cropFinderPattern.orientation(), cropFinderPattern.moduleSize());
				}
			}
		}
	}

#if defined(OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	{
		if (QRCodesDebugElements::get().isElementActive(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS))
		{
			for (bool useCameraA : { true, false })
			{
				Frame& debugFrame = useCameraA ? debugFrameA : debugFrameB;

				const FinderPatterns& finderPatterns = useCameraA ? finderPatternsA : finderPatternsB;

				for (const FinderPattern& finderPattern : finderPatterns)
				{
					CV::Detector::QRCodes::Utilities::drawFinderPattern(debugFrame, finderPattern, CV::Canvas::yellow(debugFrame.pixelFormat()));
				}
			}

			QRCodesDebugElements::get().updateElement(QRCodesDebugElements::EI_TRACKING_FINDER_PATTERNS, std::move(debugFrameStereo));
		}
	}
#endif // OCEAN_TRACKING_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

	if (finderPatternsA.size() < 3 || finderPatternsB.size() < 3)
	{
		// Not enough finder patterns to compute a pose.
		return false;
	}

	constexpr size_t maximumNumberOfDetectableCodes = 5;
	constexpr size_t maximumNumberOfFinderPatterns = 3 * maximumNumberOfDetectableCodes;

	if (finderPatternsA.size() > maximumNumberOfFinderPatterns || finderPatternsB.size() > maximumNumberOfFinderPatterns)
	{
		// Too many finder patterns; abort here to avoid potential spike in the runtime performance
		return false;
	}

	const IndexTriplets indexTripletsA = FinderPatternDetector::extractIndexTriplets(finderPatternsA); // TODO Tune parameters, if necessary?
	const IndexTriplets indexTripletsB = FinderPatternDetector::extractIndexTriplets(finderPatternsB);

	if (indexTripletsA.empty() || indexTripletsB.empty())
	{
		// Not enough triplets to compute a pose.
		return false;
	}

	for (size_t iA = 0; iA < indexTripletsA.size(); ++iA)
	{
		const IndexTriplet& indexTripletA = indexTripletsA[iA];

		const FinderPatternTriplet finderPatternTripletA =
		{
			finderPatternsA[indexTripletA[0]],
			finderPatternsA[indexTripletA[1]],
			finderPatternsA[indexTripletA[2]],
		};

		for (size_t iB = 0; iB < indexTripletsB.size(); ++iB)
		{
			const IndexTriplet& indexTripletB = indexTripletsB[iB];

			const FinderPatternTriplet finderPatternTripletB =
			{
				finderPatternsB[indexTripletB[0]],
				finderPatternsB[indexTripletB[1]],
				finderPatternsB[indexTripletB[2]],
			};

			Vectors3 worldPoints;
			Scalar averageFinderPatternCenterDistance;

			if (triangulateFinderPatternsStereo(sharedAnyCameraA, sharedAnyCameraB, world_T_device, device_T_cameraA, device_T_cameraB, finderPatternTripletA, finderPatternTripletB, worldPoints, averageFinderPatternCenterDistance))
			{
				const Frame& yFrameA = framePyramidA.finestLayer();
				const Frame& yFrameB = framePyramidB.finestLayer();

				CV::Detector::QRCodes::QRCode code;
				HomogenousMatrix4 world_T_code;
				Scalar codeSize;

				const unsigned int version = trackedCode.code().version();

				if (computePoseAndExtractQRCodeStereo(yFrameA, yFrameB, sharedAnyCameraA, sharedAnyCameraB, world_T_device, device_T_cameraA, device_T_cameraB, finderPatternTripletA, finderPatternTripletB, worldPoints, averageFinderPatternCenterDistance, code, world_T_code, codeSize, version))
				{
					if (code.isSame(trackedCode.code(), /* ignoreModules */ true))
					{
						trackedCode.updateTrackingPose(std::move(world_T_code), codeSize, trackingTimestamp);

						return true;
					}
				}
			}
		}
	}

	return false;
}

bool QRCodeTracker3D::isAlreadyTracked(const TrackedQRCodesMap& trackedQRCodesMap, const CV::Detector::QRCodes::QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, ObjectId& objectId)
{
	ocean_assert(code.isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(codeSize > Scalar(0));

	const Scalar minCodeSize = Scalar(0.95) * codeSize;
	const Scalar maxCodeSize = Scalar(1.05) * codeSize;

	const Vector3 codeTranslation = world_T_code.translation();

	ObjectId bestMatchObjectId = invalidObjectId();
	Scalar minSqrDistance = Numeric::maxValue();

	for (TrackedQRCodesMap::const_iterator iter = trackedQRCodesMap.cbegin(); iter != trackedQRCodesMap.cend(); ++iter)
	{
		const Scalar trackedCodeSize = iter->second.codeSize();

		if (trackedCodeSize < minCodeSize || trackedCodeSize > maxCodeSize)
		{
			continue;
		}

		const QRCode& trackedCode = iter->second.code();

		if (!code.isSame(trackedCode, /* ignoreModules */ true))
		{
			continue;
		}

		const Vector3 trackedCodeTranslation = iter->second.world_T_code().translation();

		const Scalar sqrDistance = codeTranslation.sqrDistance(trackedCodeTranslation);

		if (sqrDistance < minSqrDistance)
		{
			bestMatchObjectId = iter->first;
			minSqrDistance = sqrDistance;
		}
	}

	// TODO Check if the match is geometrically plausible

	if (bestMatchObjectId == invalidObjectId())
	{
		return false;
	}

	objectId = bestMatchObjectId;

	return true;
}

Geometry::ObjectPoints QRCodeTracker3D::createTrackingObjectPoints(const CV::Detector::QRCodes::QRCode& code, const Scalar codeSize)
{
	ocean_assert(code.isValid());
	ocean_assert(codeSize > Scalar(0));

	const Scalar codeSize_2 = codeSize * Scalar(0.5);
	const CV::Detector::QRCodes::Utilities::CoordinateSystem coordinateSystem(code.version(), codeSize_2);

	const Scalar moduleSize = codeSize / Scalar(QRCode::modulesPerSide(code.version()));
	const Scalar finderPatternWidth = moduleSize * Scalar(7);

	Geometry::ObjectPoints trackingObjectPoints =
	{
		// Top-left finder pattern
		Vector3(-codeSize_2, codeSize_2, Scalar(0)), // TL
		Vector3(-codeSize_2, codeSize_2 - finderPatternWidth, Scalar(0)), // BL
		// Ignoring BR - Vector3(-codeSize_2 + finderPatternWidth, codeSize_2 - finderPatternWidth, Scalar(0)),
		Vector3(-codeSize_2 + finderPatternWidth, codeSize_2, Scalar(0)), // TR

		// Bottom-left finder pattern
		Vector3(-codeSize_2, -codeSize_2, Scalar(0)), // BL
		Vector3(-codeSize_2 + finderPatternWidth, -codeSize_2, Scalar(0)), // BR
		// ignoring TR - Vector3(-codeSize_2 + finderPatternWidth, -codeSize_2 + finderPatternWidth, Scalar(0)),
		Vector3(-codeSize_2, -codeSize_2 + finderPatternWidth, Scalar(0)), // TL

		// nothing in the bottom-right corner of the code

		// Top-right finder pattern
		Vector3(codeSize_2, codeSize_2, Scalar(0)), // TR
		Vector3(codeSize_2 - finderPatternWidth, codeSize_2, Scalar(0)), // TL
		// ignoring BL - Vector3(codeSize_2 - finderPatternWidth, codeSize_2 - finderPatternWidth, Scalar(0)),
		Vector3(codeSize_2, codeSize_2 - finderPatternWidth, Scalar(0)), // BR
	};

	// TODO Add additional points by looking corners in the modules.

	ocean_assert(trackingObjectPoints.size() >= 3);

	return trackingObjectPoints;
}

bool QRCodeTracker3D::createTrackingImagePoints(const SharedAnyCamera& sharedAnyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const bool refineCorners, const Geometry::ObjectPoints& potentialObjectPoints, Geometry::ObjectPoints& objectPoints, Geometry::ImagePoints& imagePoints)
{
	ocean_assert(sharedAnyCamera != nullptr && sharedAnyCamera->isValid());
	ocean_assert(yFrame.isValid());
	ocean_assert(world_T_camera.isValid());
	ocean_assert(world_T_code.isValid());
	ocean_assert(!potentialObjectPoints.empty());

	const HomogenousMatrix4 flippedCamera_T_world = PinholeCamera::standard2InvertedFlipped(world_T_camera);
	ocean_assert(flippedCamera_T_world.isValid());

	const HomogenousMatrix4 flippedCamera_T_code = flippedCamera_T_world * world_T_code;
	ocean_assert(flippedCamera_T_code.isValid());

	Geometry::ObjectPoints internalObjectPoints;
	Geometry::ImagePoints internalImagePoints;

	internalObjectPoints.reserve(potentialObjectPoints.size());
	internalImagePoints.reserve(potentialObjectPoints.size());

	if (refineCorners)
	{
		for (const Vector3& objectPoint : potentialObjectPoints)
		{
			const Vector2 imagePoint = sharedAnyCamera->projectToImageIF(flippedCamera_T_code * objectPoint);

			for (const unsigned int numberNeighbors : { 1u, 2u })
			{
				Vector2 refinedImagePoint = imagePoint;

				if (OculusTags::Utilities::refineCorner(yFrame, refinedImagePoint, numberNeighbors))
				{
					internalObjectPoints.emplace_back(objectPoint);
					internalImagePoints.emplace_back(refinedImagePoint);

					break;
				}
			}
		}
	}
	else
	{
		for (const Vector3& objectPoint : potentialObjectPoints)
		{
			const Vector2 imagePoint = sharedAnyCamera->projectToImageIF(flippedCamera_T_code * objectPoint);

			internalObjectPoints.emplace_back(objectPoint);
			internalImagePoints.emplace_back(imagePoint);
		}
	}

	ocean_assert(internalObjectPoints.size() == internalImagePoints.size());

	if (internalObjectPoints.size() < 3)
	{
		return false;
	}

	objectPoints = std::move(internalObjectPoints);
	imagePoints = std::move(internalImagePoints);

	return true;
}

const QRCodeTracker3D::TrackedQRCodesMap& QRCodeTracker3D::invalidTrackedQRCodesMap()
{
	static const TrackedQRCodesMap invalidTrackedQRCodesMap;
	return invalidTrackedQRCodesMap;
}

} // namespace QRCodes

} // namespace Tracking

} // namespace Ocean
