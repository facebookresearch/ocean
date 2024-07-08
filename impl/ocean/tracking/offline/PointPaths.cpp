/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/offline/PointPaths.h"

#include "ocean/base/Median.h"
#include "ocean/base/Subset.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/FeatureDetector.h"

namespace Ocean
{

namespace Tracking
{

namespace Offline
{

bool PointPaths::TrackingConfiguration::weakenConfiguration(const Scalar binSizeFactor, const Scalar strengthFactor, const unsigned int minimalBinSize, const unsigned int minimalStrength)
{
	ocean_assert(binSizeFactor > Numeric::eps() && strengthFactor > Numeric::eps());

	const unsigned int newHorizontalBinSize = max(minimalBinSize, (unsigned int)(Scalar(horizontalBinSize_) * binSizeFactor + Scalar(0.5)));
	const unsigned int newVerticalBinSize = max(minimalBinSize, (unsigned int)(Scalar(verticalBinSize_) * binSizeFactor + Scalar(0.5)));
	const unsigned int newStrength = max(minimalStrength, (unsigned int)(Scalar(strength_) * strengthFactor + Scalar(0.5)));

	if (newHorizontalBinSize == horizontalBinSize_ && newVerticalBinSize == verticalBinSize_ && newStrength == strength_)
	{
		return false;
	}

	horizontalBinSize_ = newHorizontalBinSize;
	verticalBinSize_ = newVerticalBinSize;
	strength_ = newStrength;

	return true;
}

bool PointPaths::determinePointPaths(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const TrackingConfiguration& trackingConfiguration, const unsigned int lowerFrameIndex, const unsigned int startFrameIndex, const unsigned int upperFrameIndex, const unsigned int invalidBorderSize, const bool onlyNewObjectPoints, Database& database, Worker* worker, bool* abort, Scalar* progress)
{
	ocean_assert(lowerFrameIndex <= upperFrameIndex);
	ocean_assert(lowerFrameIndex <= startFrameIndex && startFrameIndex <= upperFrameIndex);

	while (!frameProviderInterface.isInitialized())
	{
		if (abort && *abort)
		{
			return false;
		}

		Thread::sleep(1);
	}

	FrameRef initialFrameRef = frameProviderInterface.synchronFrameRequest(startFrameIndex, 10.0, abort);
	if (initialFrameRef.isNull())
	{
		return false;
	}

	Frame initialFrame;
	if (!CV::FrameConverter::Comfort::convert(*initialFrameRef, pixelFormat, pixelOrigin, initialFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	Indices32 debugAlreadyExistingObjectPointIdsLowerFrame, debugAlreadyExistingObjectPointIdsUpperFrame;
	Vectors2 debugAlreadyExistingFeaturePointsLowerFrame, debugAlreadyExistingFeaturePointsUpperFrame;

	if (database.hasPose<false>(lowerFrameIndex))
		debugAlreadyExistingFeaturePointsLowerFrame = database.imagePointsWithObjectPoints<false>(lowerFrameIndex, debugAlreadyExistingObjectPointIdsLowerFrame);

	if (database.hasPose<false>(upperFrameIndex))
		debugAlreadyExistingFeaturePointsUpperFrame = database.imagePointsWithObjectPoints<false>(upperFrameIndex, debugAlreadyExistingObjectPointIdsUpperFrame);
#endif

	ocean_assert(invalidBorderSize * 2u < initialFrame.width() && invalidBorderSize * 2u < initialFrame.height());
	const Box2 validFrameArea((Scalar)(invalidBorderSize), (Scalar)(invalidBorderSize), Scalar(initialFrame.width() - invalidBorderSize), Scalar(initialFrame.height() - invalidBorderSize));

	Log::info() << "Pyramid parameters: " << trackingConfiguration.pyramidLayers() << " layers and " << trackingConfiguration.coarsestLayerRadius() << " search radius";

	const CV::FramePyramid startFramePyramid(initialFrame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), false /*copyFirstLayer*/, worker);

	const unsigned int horizontalBins = trackingConfiguration.horizontalBins(initialFrame.width(), 4u);
	const unsigned int verticalBins = trackingConfiguration.verticalBins(initialFrame.height(), 4u);

	// we detect strong feature points in the start frame
	Strengths startFrameFeatureStrengths;
	Vectors2 startFrameFeaturePoints = CV::Detector::FeatureDetector::determineHarrisPoints(initialFrame, CV::SubRegion(), horizontalBins, verticalBins, trackingConfiguration.strength(), worker, &startFrameFeatureStrengths);

	if (horizontalBins != 0u && verticalBins != 0u)
	{
		// we apply a further filtering to determine sparse feature points

		Indices32 validIndices;
		validIndices.reserve(startFrameFeaturePoints.size());

		Geometry::SpatialDistribution::OccupancyArray occupancyArray(Scalar(0), Scalar(0), Scalar(startFramePyramid.finestLayer().width()), Scalar(startFramePyramid.finestLayer().height()), min(horizontalBins * 3u, startFramePyramid.finestLayer().width()), min(verticalBins * 3u, startFramePyramid.finestLayer().height()), true);
		for (unsigned int n = 0u; n < startFrameFeaturePoints.size(); ++n)
		{
			if (occupancyArray.addPoint(startFrameFeaturePoints[n]))
			{
				validIndices.push_back(n);
			}
		}

		if (validIndices.size() != startFrameFeaturePoints.size())
		{
			startFrameFeaturePoints = Subset::subset(startFrameFeaturePoints, validIndices);
			startFrameFeatureStrengths = Subset::subset(startFrameFeatureStrengths, validIndices);
		}
	}

	// check whether we have to filter some of the detected feature points as they may already be in the database
	if (onlyNewObjectPoints && database.hasPose<false>(startFrameIndex))
	{
		Indices32 validIndices;
		validIndices.reserve(startFrameFeaturePoints.size());

		const Vectors2 existingImagePoints(database.imagePoints<false>(startFrameIndex));

		for (unsigned int n = 0; n < startFrameFeaturePoints.size(); ++n)
		{
			const Vector2& startFrameFeaturePoint = startFrameFeaturePoints[n];

			// we need to filter all tracked points which are outside our valid area
			if (validFrameArea.isInside(startFrameFeaturePoint))
			{
				bool existingPoint = false;

				for (Vectors2::const_iterator i = existingImagePoints.begin(); existingPoint == false && i != existingImagePoints.end(); ++i)
				{
					existingPoint = existingPoint || i->sqrDistance(startFrameFeaturePoint) <= Scalar(5 * 5); // **TODO** better threshold?
				}

				if (!existingPoint)
				{
					validIndices.push_back(n);
				}
			}
		}

		if (validIndices.size() != startFrameFeatureStrengths.size())
		{
			startFrameFeatureStrengths = Subset::subset(startFrameFeatureStrengths, validIndices);
			startFrameFeaturePoints = Subset::subset(startFrameFeaturePoints, validIndices);
		}
	}
	else
	{
		// we need to filter all tracked points which are outside our valid area
		Indices32 validIndices;
		validIndices.reserve(startFrameFeaturePoints.size());

		for (unsigned int n = 0; n < startFrameFeaturePoints.size(); ++n)
		{
			if (validFrameArea.isInside(startFrameFeaturePoints[n]))
			{
				validIndices.push_back(n);
			}
		}

		if (validIndices.size() != startFrameFeaturePoints.size())
		{
			startFrameFeatureStrengths = Subset::subset(startFrameFeatureStrengths, validIndices);
			startFrameFeaturePoints = Subset::subset(startFrameFeaturePoints, validIndices);
		}
	}

	ocean_assert(startFrameFeaturePoints.size() == startFrameFeatureStrengths.size());

	// we add a new pose (if not existing) for the current frame so that all image points can be added to this pose
	if (!database.hasPose<false>(startFrameIndex))
	{
		database.addPose<false>(startFrameIndex);
	}

	// we store the id of the first object point which will be added to ensure that we only track our 'own' object points from this process
	// beware: this works only as long as ids of new object point is higher the all previous ids (which is the case in the database)
	Index32 ourFirstObjectPointId = Database::invalidId;

	Indices32 startFrameObjectPointIds;
	startFrameObjectPointIds.reserve(startFrameFeaturePoints.size());

	for (size_t i = 0; i < startFrameFeaturePoints.size(); ++i)
	{
		const Index32 imagePointId = database.addImagePoint<false>(startFrameFeaturePoints[i]);
		const Index32 objectPointId = database.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(startFrameFeatureStrengths[i]));

		if (ourFirstObjectPointId == Database::invalidId)
		{
			ourFirstObjectPointId = objectPointId;
		}

		database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		database.attachImagePointToPose<false>(imagePointId, startFrameIndex);

		startFrameObjectPointIds.push_back(objectPointId);
	}

	CV::FramePyramid currentFramePyramid;
	CV::FramePyramid previousFramePyramid(startFramePyramid, true /*copyData*/);
	Vectors2 previousFeaturePoints(startFrameFeaturePoints);
	Strengths previousFeatureStrengths(startFrameFeatureStrengths);
	Indices32 previousObjectPointIds(startFrameObjectPointIds);

	// we have a (major) forward iteration and a (major) backward iteration, therefore we need to known how many frames belong to which iteration
	const Scalar forwardOverallProgress = Scalar(upperFrameIndex - startFrameIndex) / Scalar(upperFrameIndex - lowerFrameIndex + 1u);

	// we start with the major forward iteration (which has a minor backward iteration afterwards)

	for (unsigned int frameIndex = startFrameIndex + 1u; frameIndex <= upperFrameIndex && (!abort || !*abort); ++frameIndex)
	{
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex < upperFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex + 1u, 9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		const Index32 poseId = frameIndex;

		// we add a new pose (if not existing) for the current frame so that all image points can be added to this pose
		if (!database.hasPose<false>(poseId))
		{
			database.addPose<false>(poseId);
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid.isValid());

		// we detect strong feature points in the current frame
		Strengths newCurrentFeatureStrengths;
		Vectors2 newCurrentFeaturePoints = CV::Detector::FeatureDetector::determineHarrisPoints(frame, CV::SubRegion(), horizontalBins, verticalBins, trackingConfiguration.strength(), worker, &newCurrentFeatureStrengths);

		Vectors2 currentFeaturePoints;
		Strengths currentFeatureStrengths = previousFeatureStrengths;

		// we track the points from the previous frame to the current frame
		Indices32 validIndices;
		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengths, trackingConfiguration.trackingMethod(), previousFeaturePoints, currentFeaturePoints, validIndices, worker))
		{
			return false;
		}

		// we need to filter all tracked points which are outside our valid area
		Indices32 frameAreaValidIndices;
		frameAreaValidIndices.reserve(validIndices.size());

		for (size_t n = 0; n < validIndices.size(); ++n)
		{
			if (validFrameArea.isInside(currentFeaturePoints[validIndices[n]]))
			{
				frameAreaValidIndices.push_back(validIndices[n]);
			}
		}

		validIndices = std::move(frameAreaValidIndices);

		// now we remove all bad/invalid point correspondences and their corresponding object points etc.

		previousFeaturePoints = Subset::subset(previousFeaturePoints, validIndices);
		currentFeaturePoints = Subset::subset(currentFeaturePoints, validIndices);
		currentFeatureStrengths = Subset::subset(currentFeatureStrengths, validIndices);
		previousObjectPointIds = Subset::subset(previousObjectPointIds, validIndices);

		Geometry::SpatialDistribution::OccupancyArray occupancyArray;

		if (horizontalBins != 0u && verticalBins != 0u)
		{
			occupancyArray = Geometry::SpatialDistribution::OccupancyArray(Scalar(0), Scalar(0), Scalar(currentFramePyramid.finestWidth()), Scalar(currentFramePyramid.finestHeight()), min(horizontalBins * 3u, currentFramePyramid.finestWidth()), min(verticalBins * 3u, currentFramePyramid.finestHeight()), true);
			for (Vectors2::const_iterator i = currentFeaturePoints.begin(); i != currentFeaturePoints.end(); ++i)
			{
				occupancyArray += *i;
			}
		}

		ocean_assert(currentFeaturePoints.size() == previousObjectPointIds.size());

		// check whether we have empty space which can be filled with new feature points
		for (unsigned int n = 0u; n < newCurrentFeaturePoints.size(); ++n)
		{
			if (validFrameArea.isInside(newCurrentFeaturePoints[n]) && (!occupancyArray.isValid() || !occupancyArray.isOccupiedNeighborhood9(newCurrentFeaturePoints[n])))
			{
				currentFeaturePoints.push_back(newCurrentFeaturePoints[n]);
				currentFeatureStrengths.push_back(newCurrentFeatureStrengths[n]);

				const Index32 objectPointId = database.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(newCurrentFeatureStrengths[n]));

				if (ourFirstObjectPointId == Database::invalidId)
				{
					ourFirstObjectPointId = objectPointId;
				}

				previousObjectPointIds.push_back(objectPointId);
			}
		}

		// the re-tracked image points need to be added to the database each point must be registered to the correct 3D object point
		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 imagePointId = database.addImagePoint<false>(currentFeaturePoints[n]);
			const Index32 objectPointId = previousObjectPointIds[n];

			// create the relationship between pose (camera frame), object point and image point
			database.attachImagePointToPose<false>(imagePointId, poseId);
			database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		}

		previousFeaturePoints = std::move(currentFeaturePoints);
		previousFeatureStrengths = std::move(currentFeatureStrengths);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(upperFrameIndex != startFrameIndex);
			*progress = forwardOverallProgress * Scalar(0.65) * Scalar(frameIndex - startFrameIndex) / Scalar(upperFrameIndex - startFrameIndex);
		}
	}

#ifdef OCEAN_DEBUG
	if (!abort || !*abort)
	{
		// ensure that every image point has been assigned to one object point
		const Indices32 imagePointIds = database.imagePointIds<false>();
		for (Indices32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(database.objectPointFromImagePoint<false>(*i) != Database::invalidId);
		}
	}

	if (!abort || !*abort)
	{
		Indices32 debugObjectPointIds;
		const Vectors2 debugFeaturePoints = database.imagePointsWithObjectPoints<false>(upperFrameIndex, debugObjectPointIds);

		IndexSet32 debugPreviousObjectPointIdSet(previousObjectPointIds.begin(), previousObjectPointIds.end());

		for (Indices32::const_iterator i = debugAlreadyExistingObjectPointIdsUpperFrame.begin(); i != debugAlreadyExistingObjectPointIdsUpperFrame.end(); ++i)
		{
			ocean_assert(debugPreviousObjectPointIdSet.find(*i) == debugPreviousObjectPointIdSet.end());
		}

		debugPreviousObjectPointIdSet.insert(debugAlreadyExistingObjectPointIdsUpperFrame.begin(), debugAlreadyExistingObjectPointIdsUpperFrame.end());

		std::set<Vector2> debugPreviousImagePointSet(previousFeaturePoints.begin(), previousFeaturePoints.end());
		debugPreviousImagePointSet.insert(debugAlreadyExistingFeaturePointsUpperFrame.begin(), debugAlreadyExistingFeaturePointsUpperFrame.end());

		ocean_assert(IndexSet32(debugObjectPointIds.begin(), debugObjectPointIds.end()) == debugPreviousObjectPointIdSet);
		ocean_assert(std::set<Vector2>(debugFeaturePoints.begin(), debugFeaturePoints.end()) == debugPreviousImagePointSet);
	}
#endif

	// now we have the minor backward iteration (for all start positions of point paths determined in the major forward iteration)
	// we apply the minor backward iteration only if we added a new object point in the major forward iteration

	for (unsigned int frameIndex = upperFrameIndex - 1u; ourFirstObjectPointId != Database::invalidId && int(frameIndex) >= int(startFrameIndex) && (!abort || !*abort); --frameIndex)
	{
		Vectors2 previousPointsToTrack;
		Indices32 previousObjectPointsToTrack;
		Strengths previousFeatureStrengthsToTrack;

		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 previousObjectPointId = previousObjectPointIds[n];

			if (previousObjectPointId >= ourFirstObjectPointId && !database.hasObservation<false>(frameIndex, previousObjectPointId))
			{
				previousPointsToTrack.push_back(previousFeaturePoints[n]);
				previousObjectPointsToTrack.push_back(previousObjectPointId);
				previousFeatureStrengthsToTrack.push_back(int(database.objectPointPriority<false>(previousObjectPointId)));
			}
		}

		ocean_assert(previousPointsToTrack.size() == previousObjectPointsToTrack.size());
		ocean_assert(previousPointsToTrack.size() == previousFeatureStrengthsToTrack.size());

		// now we track the lost object points from the previous frame to the current frame
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex > lowerFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex - 1u, -9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid);

		Indices32 validTrackedIndices;
		Vectors2 currentPointsToTrack;

		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengthsToTrack, trackingConfiguration.trackingMethod(), previousPointsToTrack, currentPointsToTrack, validTrackedIndices, worker))
		{
			return false;
		}

		// now we add all successfully tracked points into the database (and connect them with the corresponding object points)
		for (Indices32::const_iterator i = validTrackedIndices.begin(); i != validTrackedIndices.end(); ++i)
		{
			const Vector2& currentImagePoint = currentPointsToTrack[*i];

			if (validFrameArea.isInside(currentImagePoint))
			{
				const Index32 currentObjectPointId = previousObjectPointsToTrack[*i];

#ifdef OCEAN_DEBUG
				{
					Vector2 debugImagePoint;
					ocean_assert(database.hasObservation<false>(frameIndex + 1u, currentObjectPointId, &debugImagePoint) && debugImagePoint == previousPointsToTrack[*i]);

					Indices32 debugImagePointIds;
					const Vectors2 debugImagePoints = database.imagePoints<false>(frameIndex, &debugImagePointIds);

					for (size_t n = 0; n < debugImagePoints.size(); ++n)
					{
						if (debugImagePoints[n] == currentImagePoint)
						{
							const Index32 objectPointId = database.objectPointFromImagePoint<false>(debugImagePointIds[n]);
							ocean_assert(objectPointId != Database::invalidId);
							ocean_assert(objectPointId != currentObjectPointId);
						}
					}
				}
#endif

				const Index32 imagePointId = database.addImagePoint<false>(currentImagePoint);
				database.attachImagePointToPose<false>(imagePointId, frameIndex);
				database.attachImagePointToObjectPoint<false>(imagePointId, currentObjectPointId);
			}
		}

		previousObjectPointIds.clear();
		previousFeaturePoints = database.imagePointsWithObjectPoints<false>(frameIndex, previousObjectPointIds);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(upperFrameIndex != startFrameIndex);
			*progress = forwardOverallProgress * (Scalar(0.65) + Scalar(0.35) * Scalar(upperFrameIndex - frameIndex) / Scalar(upperFrameIndex - startFrameIndex));
		}
	}

	// now we go on with the major backward iteration (which has a minor forward iteration afterwards)

	previousFramePyramid = CV::FramePyramid(startFramePyramid, true /*copyData*/);

	Indices32 candidateObjectPointIds;
	Vectors2 candidateFeaturePoints = database.imagePointsWithObjectPoints<false>(startFrameIndex, candidateObjectPointIds);

	previousFeaturePoints.clear();
	previousFeatureStrengths.clear();
	previousObjectPointIds.clear();

	previousFeaturePoints.reserve(candidateFeaturePoints.size());
	previousFeatureStrengths.reserve(candidateFeaturePoints.size());
	previousObjectPointIds.reserve(candidateFeaturePoints.size());

	for (size_t n = 0; n < candidateObjectPointIds.size(); ++n)
	{
		const Index32 candidateObjectPointId = candidateObjectPointIds[n];

		if (candidateObjectPointId >= ourFirstObjectPointId)
		{
			previousFeaturePoints.push_back(candidateFeaturePoints[n]);
			previousObjectPointIds.push_back(candidateObjectPointId);
			previousFeatureStrengths.push_back(int(database.objectPointPriority<false>(candidateObjectPointId)));
		}
	}

	for (unsigned int frameIndex = startFrameIndex - 1u; int(frameIndex) >= int(lowerFrameIndex) && (!abort || !*abort); --frameIndex)
	{
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex > lowerFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex - 1u, -9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		const Index32 poseId = frameIndex;

		// we add a new pose (if not existing) for the current frame so that all image points can be added to this pose
		if (!database.hasPose<false>(poseId))
		{
			database.addPose<false>(poseId);
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid.isValid());

		// we detect strong feature points in the current frame
		Strengths newCurrentFeatureStrengths;
		Vectors2 newCurrentFeaturePoints = CV::Detector::FeatureDetector::determineHarrisPoints(frame, CV::SubRegion(), horizontalBins, verticalBins, trackingConfiguration.strength(), worker, &newCurrentFeatureStrengths);

		Vectors2 currentFeaturePoints;
		Strengths currentFeatureStrengths = previousFeatureStrengths;

		// we track the points from the previous frame to the current frame
		Indices32 validIndices;
		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengths, trackingConfiguration.trackingMethod(), previousFeaturePoints, currentFeaturePoints, validIndices, worker))
		{
			return false;
		}

		// we need to filter all tracked points which are outside our valid area
		Indices32 frameAreaValidIndices;
		frameAreaValidIndices.reserve(validIndices.size());

		for (size_t n = 0; n < validIndices.size(); ++n)
		{
			if (validFrameArea.isInside(currentFeaturePoints[validIndices[n]]))
			{
				frameAreaValidIndices.push_back(validIndices[n]);
			}
		}

		validIndices = std::move(frameAreaValidIndices);

		// now we remove all bad/invalid point correspondences and their corresponding object points etc.

		previousFeaturePoints = Subset::subset(previousFeaturePoints, validIndices);
		currentFeaturePoints = Subset::subset(currentFeaturePoints, validIndices);
		currentFeatureStrengths = Subset::subset(currentFeatureStrengths, validIndices);
		previousObjectPointIds = Subset::subset(previousObjectPointIds, validIndices);

		Geometry::SpatialDistribution::OccupancyArray occupancyArray;

		if (horizontalBins != 0u && verticalBins != 0u)
		{
			occupancyArray = Geometry::SpatialDistribution::OccupancyArray(Scalar(0), Scalar(0), Scalar(currentFramePyramid.finestWidth()), Scalar(currentFramePyramid.finestHeight()), min(horizontalBins * 3u, currentFramePyramid.finestWidth()), min(verticalBins * 3u, currentFramePyramid.finestHeight()), true);
			for (Vectors2::const_iterator i = currentFeaturePoints.begin(); i != currentFeaturePoints.end(); ++i)
			{
				occupancyArray += *i;
			}
		}

		ocean_assert(currentFeaturePoints.size() == previousObjectPointIds.size());

		// check whether we have empty space which can be filled with new feature points
		for (unsigned int n = 0u; n < newCurrentFeaturePoints.size(); ++n)
		{
			if (validFrameArea.isInside(newCurrentFeaturePoints[n]) && (!occupancyArray.isValid() || !occupancyArray.isOccupiedNeighborhood9(newCurrentFeaturePoints[n])))
			{
				currentFeaturePoints.push_back(newCurrentFeaturePoints[n]);
				currentFeatureStrengths.push_back(newCurrentFeatureStrengths[n]);

				const Index32 objectPointId = database.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(newCurrentFeatureStrengths[n]));

				if (ourFirstObjectPointId == Database::invalidId)
				{
					ourFirstObjectPointId = objectPointId;
				}

				previousObjectPointIds.push_back(objectPointId);
			}
		}

		// the re-tracked image points need to be added to the database each point must be registered to the correct 3D object point
		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 imagePointId = database.addImagePoint<false>(currentFeaturePoints[n]);
			const Index32 objectPointId = previousObjectPointIds[n];

			// create the relationship between pose (camera frame), object point and image point
			database.attachImagePointToPose<false>(imagePointId, poseId);
			database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		}

		previousFeaturePoints = std::move(currentFeaturePoints);
		previousFeatureStrengths = std::move(currentFeatureStrengths);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(lowerFrameIndex != startFrameIndex);
			*progress = forwardOverallProgress + (Scalar(1) - forwardOverallProgress) * Scalar(0.5) * Scalar(startFrameIndex - frameIndex) / Scalar(startFrameIndex - lowerFrameIndex);
		}
	}

#ifdef OCEAN_DEBUG

	if (!abort || !*abort)
	{
		// ensure that every image point has been assigned to one object point
		const Indices32 imagePointIds = database.imagePointIds<false>();
		for (Indices32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(database.objectPointFromImagePoint<false>(*i) != Database::invalidId);
		}
	}

	if (!abort || !*abort)
	{
		Indices32 debugObjectPointIds;
		const Vectors2 debugFeaturePoints = database.imagePointsWithObjectPoints<false>(lowerFrameIndex, debugObjectPointIds);

		IndexSet32 debugPreviousObjectPointIdSet(previousObjectPointIds.begin(), previousObjectPointIds.end());

		for (Indices32::const_iterator i = debugAlreadyExistingObjectPointIdsLowerFrame.begin(); i != debugAlreadyExistingObjectPointIdsLowerFrame.end(); ++i)
		{
			ocean_assert(debugPreviousObjectPointIdSet.find(*i) == debugPreviousObjectPointIdSet.end());
		}

		debugPreviousObjectPointIdSet.insert(debugAlreadyExistingObjectPointIdsLowerFrame.begin(), debugAlreadyExistingObjectPointIdsLowerFrame.end());

		std::set<Vector2> debugPreviousImagePointSet(previousFeaturePoints.begin(), previousFeaturePoints.end());
		debugPreviousImagePointSet.insert(debugAlreadyExistingFeaturePointsLowerFrame.begin(), debugAlreadyExistingFeaturePointsLowerFrame.end());

		ocean_assert(IndexSet32(debugObjectPointIds.begin(), debugObjectPointIds.end()) == debugPreviousObjectPointIdSet);
		ocean_assert(std::set<Vector2>(debugFeaturePoints.begin(), debugFeaturePoints.end()) == debugPreviousImagePointSet);
	}
#endif

	// now we have the minor forward iteration (for all start positions of point paths determined in the major backward iteration)
	// we apply the minor forward iteration only if we added a new object point in the major forward or major backward iteration

	for (unsigned int frameIndex = lowerFrameIndex + 1u; ourFirstObjectPointId != Database::invalidId && frameIndex <= upperFrameIndex && (!abort || !*abort); ++frameIndex)
	{
		Vectors2 previousPointsToTrack;
		Indices32 previousObjectPointsToTrack;
		Strengths previousFeatureStrengthsToTrack;

		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 previousObjectPointId = previousObjectPointIds[n];

			if (previousObjectPointId >= ourFirstObjectPointId && !database.hasObservation<false>(frameIndex, previousObjectPointId))
			{
				previousPointsToTrack.push_back(previousFeaturePoints[n]);
				previousObjectPointsToTrack.push_back(previousObjectPointId);
				previousFeatureStrengthsToTrack.push_back(int(database.objectPointPriority<false>(previousObjectPointId)));
			}
		}

		ocean_assert(previousPointsToTrack.size() == previousObjectPointsToTrack.size());
		ocean_assert(previousPointsToTrack.size() == previousFeatureStrengthsToTrack.size());

		// now we track the lost object points from the previous frame to the current frame
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex < upperFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex + 1u, 9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid);

		Indices32 validTrackedIndices;
		Vectors2 currentPointsToTrack;

		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengthsToTrack, trackingConfiguration.trackingMethod(), previousPointsToTrack, currentPointsToTrack, validTrackedIndices, worker))
		{
			return false;
		}

		// now we add all successfully tracked points into the database (and connect them with the corresponding object points)
		for (Indices32::const_iterator i = validTrackedIndices.begin(); i != validTrackedIndices.end(); ++i)
		{
			const Vector2& currentImagePoint = currentPointsToTrack[*i];

			if (validFrameArea.isInside(currentImagePoint))
			{
				const Index32 currentObjectPointId = previousObjectPointsToTrack[*i];

#ifdef OCEAN_DEBUG
				{
					Vector2 debugImagePoint;
					ocean_assert(database.hasObservation<false>(frameIndex - 1u, currentObjectPointId, &debugImagePoint) && debugImagePoint == previousPointsToTrack[*i]);

					Indices32 debugImagePointIds;
					const Vectors2 debugImagePoints = database.imagePoints<false>(frameIndex, &debugImagePointIds);

					for (size_t n = 0; n < debugImagePoints.size(); ++n)
					{
						if (debugImagePoints[n] == currentImagePoint)
						{
							const Index32 objectPointId = database.objectPointFromImagePoint<false>(debugImagePointIds[n]);
							ocean_assert(objectPointId != Database::invalidId);
							ocean_assert(objectPointId != currentObjectPointId);
						}
					}
				}
#endif

				const Index32 imagePointId = database.addImagePoint<false>(currentImagePoint);
				database.attachImagePointToPose<false>(imagePointId, frameIndex);
				database.attachImagePointToObjectPoint<false>(imagePointId, currentObjectPointId);
			}
		}

		previousObjectPointIds.clear();
		previousFeaturePoints = database.imagePointsWithObjectPoints<false>(frameIndex, previousObjectPointIds);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(lowerFrameIndex != upperFrameIndex);
			*progress = forwardOverallProgress + (Scalar(1) - forwardOverallProgress) * (Scalar(0.5) + Scalar(0.5) * Scalar(frameIndex - lowerFrameIndex) / Scalar(upperFrameIndex - lowerFrameIndex));
		}
	}

	if (progress && (!abort || !*abort))
	{
		*progress = Scalar(1.0);
	}

	return !abort || !*abort;
}

bool PointPaths::determinePointPaths(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin, const TrackingConfiguration& trackingConfiguration, const unsigned int lowerFrameIndex, const CV::SubRegion& subRegion, const unsigned int subRegionFrameIndex, const unsigned int upperFrameIndex, const unsigned int invalidBorderSize, const bool onlyNewObjectPoints, Database& database, Worker* worker, bool* abort, Scalar* progress)
{
	ocean_assert(lowerFrameIndex <= upperFrameIndex);
	ocean_assert(lowerFrameIndex <= subRegionFrameIndex && subRegionFrameIndex <= upperFrameIndex);
	ocean_assert(!subRegion.isEmpty());

	while (!frameProviderInterface.isInitialized())
	{
		if (abort && *abort)
		{
			return false;
		}

		Thread::sleep(1);
	}

	ocean_assert(upperFrameIndex < frameProviderInterface.synchronFrameNumberRequest());

	FrameRef initialFrameRef = frameProviderInterface.synchronFrameRequest(subRegionFrameIndex, 10.0, abort);
	if (initialFrameRef.isNull())
	{
		return false;
	}

	Frame initialFrame;
	if (!CV::FrameConverter::Comfort::convert(*initialFrameRef, pixelFormat, pixelOrigin, initialFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	ocean_assert(invalidBorderSize * 2u < initialFrame.width() && invalidBorderSize * 2u < initialFrame.height());
	const Box2 validFrameArea(Vector2((Scalar)(invalidBorderSize), (Scalar)(invalidBorderSize)), Vector2(Scalar(initialFrame.width() - invalidBorderSize), Scalar(initialFrame.height() - invalidBorderSize)));

	Log::info() << "Pyramid parameters: " << trackingConfiguration.pyramidLayers() << " layers and " << trackingConfiguration.coarsestLayerRadius() << " search radius";

	CV::FramePyramid subRegionFramePyramid(initialFrame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker);

	ocean_assert(subRegion.boundingBox().isValid());
	const unsigned int subRegionWidth = (unsigned int)Numeric::ceil(subRegion.boundingBox().width());
	const unsigned int subRegionHeight = (unsigned int)Numeric::ceil(subRegion.boundingBox().height());

	const unsigned int horizontalBins = trackingConfiguration.horizontalBins(subRegionWidth, 4u);
	const unsigned int verticalBins = trackingConfiguration.verticalBins(subRegionHeight, 4u);

	// we detect strong feature points in the current frame
	Strengths subRegionFeatureStrengths;
	Vectors2 subRegionFeaturePoints = CV::Detector::FeatureDetector::determineHarrisPoints(initialFrame, subRegion, horizontalBins, verticalBins, trackingConfiguration.strength(), worker, &subRegionFeatureStrengths);

	if (horizontalBins != 0u && verticalBins != 0u)
	{
		// we apply a further filtering to determine sparse feature points

		Indices32 validIndices;
		validIndices.reserve(subRegionFeaturePoints.size());

		Geometry::SpatialDistribution::OccupancyArray occupancyArray(subRegion.boundingBox(), min(horizontalBins * 3u, subRegionWidth), min(verticalBins * 3u, subRegionHeight), true);
		for (unsigned int n = 0u; n < subRegionFeaturePoints.size(); ++n)
		{
			if (occupancyArray.addPoint(subRegionFeaturePoints[n]))
			{
				validIndices.push_back(n);
			}
		}

		if (validIndices.size() != subRegionFeaturePoints.size())
		{
			subRegionFeaturePoints = Subset::subset(subRegionFeaturePoints, validIndices);
			subRegionFeatureStrengths = Subset::subset(subRegionFeatureStrengths, validIndices);
		}
	}

	// check whether we have to filter some of the detected feature points as they may already be in the database
	if (onlyNewObjectPoints && database.hasPose<false>(subRegionFrameIndex))
	{
		Indices32 validIndices;
		validIndices.reserve(subRegionFeaturePoints.size());

		const Vectors2 existingImagePoints(database.imagePoints<false>(subRegionFrameIndex));

		for (unsigned int n = 0; n < subRegionFeaturePoints.size(); ++n)
		{
			const Vector2& subRegionFeaturePoint = subRegionFeaturePoints[n];

			// we need to filter all tracked points which are outside our valid area
			if (validFrameArea.isInside(subRegionFeaturePoint))
			{
				bool existingPoint = false;

				for (Vectors2::const_iterator i = existingImagePoints.begin(); existingPoint == false && i != existingImagePoints.end(); ++i)
				{
					existingPoint = existingPoint || i->sqrDistance(subRegionFeaturePoint) <= Scalar(5 * 5); // **TODO** better threshold?
				}

				if (!existingPoint)
				{
					validIndices.push_back(n);
				}
			}
		}

		if (validIndices.size() != subRegionFeatureStrengths.size())
		{
			subRegionFeatureStrengths = Subset::subset(subRegionFeatureStrengths, validIndices);
			subRegionFeaturePoints = Subset::subset(subRegionFeaturePoints, validIndices);
		}
	}
	else
	{
		// we need to filter all tracked points which are outside our valid area
		Indices32 validIndices;
		validIndices.reserve(subRegionFeaturePoints.size());

		for (size_t n = 0; n < subRegionFeaturePoints.size(); ++n)
		{
			if (validFrameArea.isInside(subRegionFeaturePoints[n]))
			{
				validIndices.push_back(Index32(n));
			}
		}

		if (validIndices.size() != subRegionFeaturePoints.size())
		{
			subRegionFeatureStrengths = Subset::subset(subRegionFeatureStrengths, validIndices);
			subRegionFeaturePoints = Subset::subset(subRegionFeaturePoints, validIndices);
		}
	}

	if (subRegionFeaturePoints.empty())
	{
		return true;
	}

	ocean_assert(subRegionFeaturePoints.size() == subRegionFeatureStrengths.size());

	// we add a new pose (if not existing) for the current frame so that all image points can be added to this pose
	if (!database.hasPose<false>(subRegionFrameIndex))
	{
		database.addPose<false>(subRegionFrameIndex);
	}

	Indices32 subRegionFrameObjectPointIds;
	subRegionFrameObjectPointIds.reserve(subRegionFeaturePoints.size());

	for (size_t i = 0; i < subRegionFeaturePoints.size(); ++i)
	{
		const Index32 imagePointId = database.addImagePoint<false>(subRegionFeaturePoints[i]);
		const Index32 objectPointId = database.addObjectPoint<false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), Scalar(subRegionFeatureStrengths[i]));

		database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		database.attachImagePointToPose<false>(imagePointId, subRegionFrameIndex);

		subRegionFrameObjectPointIds.push_back(objectPointId);
	}

	CV::FramePyramid currentFramePyramid;
	CV::FramePyramid previousFramePyramid(subRegionFramePyramid, true /*copyData*/);
	Vectors2 previousFeaturePoints(subRegionFeaturePoints);
	Strengths previousFeatureStrengths(subRegionFeatureStrengths);
	Indices32 previousObjectPointIds(subRegionFrameObjectPointIds);

	// we have a forward iteration and a backward iteration, therefore we need to known how many frames belong to which iteration
	const Scalar forwardOverallProgress = Scalar(upperFrameIndex - subRegionFrameIndex) / Scalar(upperFrameIndex - lowerFrameIndex + 1u);

	// forward iteration
	for (unsigned int frameIndex = subRegionFrameIndex + 1u; !previousFeaturePoints.empty() && frameIndex <= upperFrameIndex && (!abort || !*abort); ++frameIndex)
	{
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex < upperFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex + 1u, 9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		// we add a new pose for the current frame (if not existing) so that all image points can be added to this pose
		const Index32 poseId = frameIndex;
		if (!database.hasPose<false>(poseId))
		{
			database.addPose<false>(poseId);
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid.isValid());

		Vectors2 currentFeaturePoints;
		Strengths currentFeatureStrengths = previousFeatureStrengths;

		// we track the points from the previous frame to the current frame
		Indices32 validIndices;
		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengths, trackingConfiguration.trackingMethod(), previousFeaturePoints, currentFeaturePoints, validIndices, worker))
		{
			return false;
		}

		// we need to filter all tracked points which are outside our valid area
		Indices32 frameAreaValidIndices;
		frameAreaValidIndices.reserve(validIndices.size());

		for (size_t n = 0; n < validIndices.size(); ++n)
		{
			if (validFrameArea.isInside(currentFeaturePoints[validIndices[n]]))
			{
				frameAreaValidIndices.push_back(validIndices[n]);
			}
		}

		validIndices = std::move(frameAreaValidIndices);

		// now we remove all bad/invalid point correspondences and their corresponding object points etc.

		previousFeaturePoints = Subset::subset(previousFeaturePoints, validIndices);
		currentFeaturePoints = Subset::subset(currentFeaturePoints, validIndices);
		currentFeatureStrengths = Subset::subset(currentFeatureStrengths, validIndices);
		previousObjectPointIds = Subset::subset(previousObjectPointIds, validIndices);

		// the re-tracked image points need to be added to the database each point must be registered to the correct 3D object point
		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 imagePointId = database.addImagePoint<false>(currentFeaturePoints[n]);
			const Index32 objectPointId = previousObjectPointIds[n];

			// create the relationship between pose (camera frame), object point and image point
			database.attachImagePointToPose<false>(imagePointId, poseId);
			database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		}

		previousFeaturePoints = std::move(currentFeaturePoints);
		previousFeatureStrengths = std::move(currentFeatureStrengths);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(upperFrameIndex != subRegionFrameIndex);
			*progress = forwardOverallProgress * Scalar(frameIndex - subRegionFrameIndex) / Scalar(upperFrameIndex - subRegionFrameIndex);
		}
	}

#ifdef OCEAN_DEBUG
	if (!abort || !*abort)
	{
		// ensure that every image point has been assigned to one object point
		const Indices32 imagePointIds = database.imagePointIds<false>();
		for (Indices32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(database.objectPointFromImagePoint<false>(*i) != Database::invalidId);
		}
	}
#endif

	ocean_assert(subRegionFramePyramid.isOwner());

	previousFramePyramid = std::move(subRegionFramePyramid);
	previousFeaturePoints = subRegionFeaturePoints;
	previousFeatureStrengths = subRegionFeatureStrengths;
	previousObjectPointIds = subRegionFrameObjectPointIds;

	// backward tracking
	for (unsigned int frameIndex = subRegionFrameIndex - 1u; !previousFeaturePoints.empty() && int(frameIndex) >= int(lowerFrameIndex) && (!abort || !*abort); --frameIndex)
	{
		// now we track the lost object points from the previous frame to the current frame
		const FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		if (frameIndex > lowerFrameIndex)
		{
			frameProviderInterface.frameCacheRequest(frameIndex - 1u, -9);
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, pixelFormat, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		// we add a new pose for the current frame (if not existing) so that all image points can be added to this pose
		const Index32 poseId = frameIndex;
		if (!database.hasPose<false>(poseId))
		{
			database.addPose<false>(poseId);
		}

		if (!currentFramePyramid.replace(frame, CV::FramePyramid::DM_FILTER_14641, trackingConfiguration.pyramidLayers(), true /*copyFirstLayer*/, worker))
		{
			return false;
		}
		ocean_assert(previousFramePyramid);

		Vectors2 currentFeaturePoints;
		Strengths currentFeatureStrengths = previousFeatureStrengths;

		// we track the points from the previous frame to the current frame
		Indices32 validIndices;
		if (!trackPoints(previousFramePyramid, currentFramePyramid, trackingConfiguration.coarsestLayerRadius(), previousFeatureStrengths, trackingConfiguration.trackingMethod(), previousFeaturePoints, currentFeaturePoints, validIndices, worker))
		{
			return false;
		}

		// we need to filter all tracked points which are outside our valid area
		Indices32 frameAreaValidIndices;
		frameAreaValidIndices.reserve(validIndices.size());

		for (size_t n = 0; n < validIndices.size(); ++n)
		{
			if (validFrameArea.isInside(currentFeaturePoints[validIndices[n]]))
			{
				frameAreaValidIndices.push_back(validIndices[n]);
			}
		}

		validIndices = std::move(frameAreaValidIndices);

		// now we remove all bad/invalid point correspondences and their corresponding object points etc.

		previousFeaturePoints = Subset::subset(previousFeaturePoints, validIndices);
		currentFeaturePoints = Subset::subset(currentFeaturePoints, validIndices);
		currentFeatureStrengths = Subset::subset(currentFeatureStrengths, validIndices);
		previousObjectPointIds = Subset::subset(previousObjectPointIds, validIndices);

		// the re-tracked image points need to be added to the database each point must be registered to the correct 3D object point
		for (size_t n = 0; n < previousObjectPointIds.size(); ++n)
		{
			const Index32 imagePointId = database.addImagePoint<false>(currentFeaturePoints[n]);
			const Index32 objectPointId = previousObjectPointIds[n];

			// create the relationship between pose (camera frame), object point and image point
			database.attachImagePointToPose<false>(imagePointId, poseId);
			database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);
		}

		previousFeaturePoints = std::move(currentFeaturePoints);
		previousFeatureStrengths = std::move(currentFeatureStrengths);

		std::swap(previousFramePyramid, currentFramePyramid);

		if (progress)
		{
			ocean_assert(lowerFrameIndex != subRegionFrameIndex);
			*progress = forwardOverallProgress + (Scalar(1) - forwardOverallProgress) * Scalar(subRegionFrameIndex - frameIndex) / Scalar(subRegionFrameIndex - lowerFrameIndex);
		}
	}

	if (progress && (!abort || !*abort))
	{
		*progress = Scalar(1.0);
	}

	// we finally have to ensure that the database has a pose (not a valid but an existing pose) for each frame within the defined frame range

	for (unsigned int frameIndex = lowerFrameIndex; frameIndex <= upperFrameIndex && (!abort || !*abort); ++frameIndex)
	{
		if (!database.hasPose<false>(frameIndex))
		{
			database.addPose<false>(frameIndex);
		}
	}

	return !abort || !*abort;
}

bool PointPaths::determineTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const CV::SubRegion& regionOfInterest, const OfflineTracker::TrackingQuality trackingQuality, const MotionSpeed motionSpeed, TrackingConfiguration* frameTrackingConfiguration, TrackingConfiguration* regionOfInterestTrackingConfiguration, bool* abort)
{
	ocean_assert(regionOfInterest.isEmpty() || regionOfInterestTrackingConfiguration);

	while (!frameProviderInterface.isInitialized())
	{
		if (abort && *abort)
		{
			return false;
		}

		Thread::sleep(1);
	}

	const FrameType frameType = frameProviderInterface.synchronFrameTypeRequest(10.0, abort);

	ocean_assert(frameType.isValid());
	if (!frameType.isValid())
	{
		return false;
	}

	// we determine the ratio of the region of interest in relation to the entire frame size which is an indicator for the importance of the region
	ocean_assert(frameType.pixels() != 0u);
	const Scalar regionOfInterestRatio = regionOfInterest.isEmpty() ? Scalar(0) : regionOfInterest.size() / Scalar(frameType.pixels());

	// the larger the region of interest the less important the remaining image content
	const unsigned int frameBinNumberFactor = (regionOfInterestRatio < Scalar(0.05)) ? 2u : 1u;
	const unsigned int regionBinNumberFactor = (frameTrackingConfiguration || regionOfInterestRatio >= Scalar(0.20)) ? 1u : 2u;

	switch (trackingQuality)
	{
		case OfflineTracker::TQ_LOW:
		{
			//  maximal coarsest layer radius: 12

			//            1280x720        1920x1080       3840x2160
			//     slow    32: 3, 8        48: 3, 12       96: 4, 12
			// moderate    64: 4, 8        96: 4, 12      192: 5, 12
			//     fast   128: 5, 8       192: 5, 12      384: 6, 12

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 12u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_7, frameType.width(), frameType.height(), 20u, 40u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_7, frameType.width(), frameType.height(), 50u * regionBinNumberFactor, 40u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_MODERATE:
		{
			//  maximal coarsest layer radius: 12

			//            1280x720        1920x1080       3840x2160
			//     slow    32: 3, 8        48: 3, 12       96: 4, 12
			// moderate    64: 4, 8        96: 4, 12      192: 5, 12
			//     fast   128: 5, 8       192: 5, 12      384: 6, 12

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 12u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 40u * frameBinNumberFactor, 25u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 100u * regionBinNumberFactor, 25u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_HIGH:
		{
			//  maximal coarsest layer radius: 26

			//            1280x720        1920x1080       3840x2160
			//     slow    32: 2, 16       48: 2, 24       96: 3, 24
			// moderate    64: 3, 16       96: 3, 24      192: 4, 24
			//     fast   128: 4, 16      192: 4, 24      384: 5, 24

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 26u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 80u * frameBinNumberFactor, 15u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 200u * regionBinNumberFactor, 15u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_ULTRA:
		{
			//  maximal coarsest layer radius: 26

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 26u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 160u * frameBinNumberFactor, 10u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 400u * regionBinNumberFactor, 10u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_INSANE:
		{
			//  maximal coarsest layer radius: 26

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 26u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, 0u, 0u, 5u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_COARSE:
		{
			//  maximal coarsest layer radius: 12

			//            1280x720        1920x1080       3840x2160
			//     slow    32: 3, 8        48: 3, 12       96: 4, 12
			// moderate    64: 4, 8        96: 4, 12      192: 5, 12
			//     fast   128: 5, 8       192: 5, 12      384: 6, 12

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 12u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_7, frameType.width(), frameType.height(), 20u, 5u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_7, frameType.width(), frameType.height(), 50u * regionBinNumberFactor, 5u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		case OfflineTracker::TQ_FINE:
		{
			//  maximal coarsest layer radius: 12

			//            1280x720        1920x1080       3840x2160
			//     slow    32: 3, 8        48: 3, 12       96: 4, 12
			// moderate    64: 4, 8        96: 4, 12      192: 5, 12
			//     fast   128: 5, 8       192: 5, 12      384: 6, 12

			unsigned int coarsestLayerRadius = 0u;
			unsigned int pyramidLayers = 0u;
			idealPyramidParameters(frameType.width(), frameType.height(), motionSpeed, coarsestLayerRadius, pyramidLayers, 12u, (unsigned int)(-1));

			// we determine the best configuration for the entire frame
			if (frameTrackingConfiguration)
			{
				*frameTrackingConfiguration = Tracking::Offline::PointPaths::TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 20u * frameBinNumberFactor, 5u, coarsestLayerRadius, pyramidLayers);
			}

			// we determine the best configuration for the region of interest
			if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
			{
				*regionOfInterestTrackingConfiguration = TrackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, frameType.width(), frameType.height(), 100u * regionBinNumberFactor, 5u, coarsestLayerRadius, pyramidLayers);
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid tracking quality!");
			break;
	}

	return true;
}

bool PointPaths::determineAutomaticTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelOrigin pixelOrigin, const MotionSpeed motionSpeed, const unsigned int frameIndex, const CV::SubRegion& regionOfInterest, TrackingConfiguration* frameTrackingConfiguration, TrackingConfiguration* regionOfInterestTrackingConfiguration, Worker* worker, bool* abort)
{
	ocean_assert(frameTrackingConfiguration || regionOfInterestTrackingConfiguration);
	ocean_assert(regionOfInterest.isEmpty() || regionOfInterestTrackingConfiguration != nullptr);

	while (!frameProviderInterface.isInitialized())
	{
		if (abort && *abort)
		{
			return false;
		}

		Thread::sleep(1);
	}

	FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
	if (frameRef.isNull())
	{
		return false;
	}

	Frame frame;
	if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	if (frameTrackingConfiguration)
	{
		*frameTrackingConfiguration = TrackingConfiguration();
	}

	if (regionOfInterestTrackingConfiguration)
	{
		*regionOfInterestTrackingConfiguration = TrackingConfiguration();
	}

	// we detect feature points in the start frame with the smallest 'realistic' feature point strength
	Strengths featureStrengths;
	Vectors2 featurePoints = CV::Detector::FeatureDetector::determineHarrisPoints(frame, CV::SubRegion(), 0u, 0u, 5u, worker, &featureStrengths);

	unsigned int lowCoarsestLayerRadius = 0u;
	unsigned int lowPyramidLayers = 0u;
	idealPyramidParameters(frame.width(), frame.height(), motionSpeed, lowCoarsestLayerRadius, lowPyramidLayers, 12u, (unsigned int)(-1));

	unsigned int highCoarsestLayerRadius = 0u;
	unsigned int highPyramidLayers = 0u;
	idealPyramidParameters(frame.width(), frame.height(), motionSpeed, highCoarsestLayerRadius, highPyramidLayers, 26u, (unsigned int)(-1));

	// we determine the best configuration for the entire frame
	if (frameTrackingConfiguration)
	{
		// in general we would like to use only very strong feature points (with Harris strengths >= 40)
		// however, there may be very homogeneous image regions which do not provide strong feature points or which may not provide even one 'realistic' feature point
		// thus, we have to weaken our expectations of the minimal feature strengths iteratively

		const TrackingConfigurationPairs candidateConfigurationPairs =
		{
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_7, frame.width(), frame.height(), 20u, 60u, lowCoarsestLayerRadius, lowPyramidLayers), 70u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_7, frame.width(), frame.height(), 20u, 55u, lowCoarsestLayerRadius, lowPyramidLayers), 65u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_7, frame.width(), frame.height(), 20u, 50u, lowCoarsestLayerRadius, lowPyramidLayers), 60u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 40u, 45u, lowCoarsestLayerRadius, lowPyramidLayers), 50u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 40u, 40u, lowCoarsestLayerRadius, lowPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 40u, 35u, lowCoarsestLayerRadius, lowPyramidLayers), 30u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 20u, highCoarsestLayerRadius, highPyramidLayers), 25u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 64u, 20u, highCoarsestLayerRadius, highPyramidLayers), 25u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 64u, 20u, highCoarsestLayerRadius, highPyramidLayers), 20u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 80u, 20u, highCoarsestLayerRadius, highPyramidLayers), 20u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 80u, 20u, highCoarsestLayerRadius, highPyramidLayers), 15u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 15u, highCoarsestLayerRadius, highPyramidLayers), 20u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 64u, 15u, highCoarsestLayerRadius, highPyramidLayers), 20u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 80u, 15u, highCoarsestLayerRadius, highPyramidLayers), 15u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 96u, 15u, highCoarsestLayerRadius, highPyramidLayers), 10u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 120u, 15u, highCoarsestLayerRadius, highPyramidLayers), 10u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 120u, 10u, highCoarsestLayerRadius, highPyramidLayers), 5u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 10u, highCoarsestLayerRadius, highPyramidLayers), 5u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, highCoarsestLayerRadius, highPyramidLayers), 5u)
		};

		for (const TrackingConfigurationPair& trackingConfigurationPair : candidateConfigurationPairs)
		{
			const TrackingConfiguration& candidateConfiguration = trackingConfigurationPair.first;
			const unsigned int minimalValidBinsPercent = trackingConfigurationPair.second;

			// **TODO** **HACK** the threshold must be normalized in the Harris corner detector
			const int harrisThreshold = int((candidateConfiguration.strength() * candidateConfiguration.strength() / 8u) * (candidateConfiguration.strength() * candidateConfiguration.strength() / 8u));

			ocean_assert(candidateConfiguration.horizontalBinSize() == candidateConfiguration.verticalBinSize());
			ocean_assert(minimalValidBinsPercent > 0u);

			const unsigned int horizontalBins = candidateConfiguration.horizontalBins(frame.width(), 4u);
			const unsigned int verticalBins = candidateConfiguration.verticalBins(frame.height(), 4u);

			if (!featurePoints.empty())
			{
				const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(featurePoints.data(), featurePoints.size(), Scalar(0), Scalar(0), Scalar(frame.width()), Scalar(frame.height()), horizontalBins, verticalBins));

				unsigned int validBins = 0u;
				unsigned int maximalBins = 0u;

				for (unsigned int i = 0u; i < distributionArray.bins(); ++i)
				{
					const Indices32& indices = distributionArray[i];

					if (!indices.empty())
					{
						// we explicit count the number of maximal possible bins (so that we do not count bins in which absolutely no feature point is visible, e.g., a blue sky)
						maximalBins++;

						const int featureStrength = featureStrengths[indices.front()];

						if (featureStrength > harrisThreshold)
						{
							validBins++;
						}
					}
				}

				const unsigned int minimalValidBins = max(1u, maximalBins * minimalValidBinsPercent / 100u);

				// check whether the current configuration is weak enough so that we can accept it
				if (validBins >= minimalValidBins)
				{
					*frameTrackingConfiguration = candidateConfiguration;
					break;
				}
			}
		}
	}

	// we determine the best configuration for the region of interest
	if (!regionOfInterest.isEmpty() && regionOfInterestTrackingConfiguration)
	{
		*regionOfInterestTrackingConfiguration = TrackingConfiguration();

		// we have mainly two individual cases: the region of interest is the sole tracking area so that we rely only on the feature points inside the region of interest,
		// or the region of interest is the a region with high priority than the remaining image content so that we can also use feature points in the remaining image
		// thus, the conditions for the region of interest must be stronger (we need more feature points) if the area is the sole tracking area

		const TrackingConfigurationPairs weakCandidateConfigurationPairs =
		{
			// (applied tracking technique, horizontal bin size, vertical bin size, minimal feature strength), minimal percent of bins with strong feature point in relation to bins with any feature points

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 45u, highCoarsestLayerRadius, highPyramidLayers), 55u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 40u, highCoarsestLayerRadius, highPyramidLayers), 50u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 35u, highCoarsestLayerRadius, highPyramidLayers), 45u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 64u, 20u, highCoarsestLayerRadius, highPyramidLayers), 40u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 80u, 15u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 80u, 15u, highCoarsestLayerRadius, highPyramidLayers), 20u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 128u, 15u, highCoarsestLayerRadius, highPyramidLayers), 15u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 128u, 10u, highCoarsestLayerRadius, highPyramidLayers), 10u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200, 10u, highCoarsestLayerRadius, highPyramidLayers), 5u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, highCoarsestLayerRadius, highPyramidLayers), 5u)
		};

		const TrackingConfigurationPairs strongCandidateConfigurationPairs =
		{
			// (applied tracking technique, horizontal bin size, vertical bin size, minimal feature strength), minimal percent of bins with strong feature point in relation to bins with any feature points

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 45u, highCoarsestLayerRadius, highPyramidLayers), 55u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 40u, highCoarsestLayerRadius, highPyramidLayers), 50u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 50u, 35u, highCoarsestLayerRadius, highPyramidLayers), 45u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 25u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 25u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 25u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 25u, highCoarsestLayerRadius, highPyramidLayers), 40u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 20u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 20u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 20u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 20u, highCoarsestLayerRadius, highPyramidLayers), 40u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 15u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 15u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 15u, highCoarsestLayerRadius, highPyramidLayers), 40u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 15u, highCoarsestLayerRadius, highPyramidLayers), 40u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 20u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 20u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 20u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 20u, highCoarsestLayerRadius, highPyramidLayers), 30u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 15u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 15u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 15u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 15u, highCoarsestLayerRadius, highPyramidLayers), 30u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 100u, 10u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 200u, 10u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 300u, 10u, highCoarsestLayerRadius, highPyramidLayers), 30u),
			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, frame.width(), frame.height(), 400u, 10u, highCoarsestLayerRadius, highPyramidLayers), 30u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 10u, highCoarsestLayerRadius, highPyramidLayers), 20u),

			TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, highCoarsestLayerRadius, highPyramidLayers), 5u)
		};

		Log::info() << "We have a region of interest with size: " << regionOfInterest.size();

		// if the selected region of interest is very tiny we must apply an almost insane configuration to ensure that we get as much feature candidates as possible
		if (regionOfInterest.size() < 30 * 30)
		{
			*regionOfInterestTrackingConfiguration = TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 0u, 0u, 5u, highCoarsestLayerRadius, highPyramidLayers);
		}
		else if (regionOfInterest.size() < 100 * 100)
		{
			*regionOfInterestTrackingConfiguration = TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, highCoarsestLayerRadius, highPyramidLayers);
		}

		const TrackingConfigurationPairs& candidateConfigurationPairs = frameTrackingConfiguration ? weakCandidateConfigurationPairs : strongCandidateConfigurationPairs;

		const Box2& boundingBox = regionOfInterest.boundingBox();
		ocean_assert(boundingBox.isValid());

		const unsigned int subRegionWidth = (unsigned int)Numeric::ceil(boundingBox.width());
		const unsigned int subRegionHeight = (unsigned int)Numeric::ceil(boundingBox.height());

		for (const TrackingConfigurationPair& candidateConfigurationPair : candidateConfigurationPairs)
		{
			if (regionOfInterestTrackingConfiguration->isValid())
			{
				break;
			}

			const TrackingConfiguration& candidateConfiguration = candidateConfigurationPair.first;
			const unsigned int minimalValidBinsPercent = candidateConfigurationPair.second;

			// **TODO** **HACK** the threshold must be normalized in the Harris corner detector
			const int harrisThreshold = int((candidateConfiguration.strength() * candidateConfiguration.strength() / 8u) * (candidateConfiguration.strength() * candidateConfiguration.strength() / 8u));

			ocean_assert(candidateConfiguration.horizontalBinSize() == candidateConfiguration.verticalBinSize());
			ocean_assert(minimalValidBinsPercent > 0u);

			const unsigned int horizontalBins = candidateConfiguration.horizontalBins(subRegionWidth, 4u);
			const unsigned int verticalBins = candidateConfiguration.verticalBins(subRegionHeight, 4u);

			if (!featurePoints.empty())
			{
				const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(featurePoints.data(), featurePoints.size(), boundingBox.left(), boundingBox.top(), boundingBox.width(), boundingBox.height(), horizontalBins, verticalBins));

				unsigned int validBins = 0u;
				unsigned int maximalBins = 0u;

				for (unsigned int i = 0u; i < distributionArray.bins(); ++i)
				{
					const Indices32& indices = distributionArray[i];

					if (!indices.empty() && regionOfInterest.isInside(featurePoints[indices.front()]))
					{
						// we explicit count the number of maximal possible bins (so that we do not count bins in which absolutely no feature point is visible, e.g., a blue sky)
						maximalBins++;

						const int featureStrength = featureStrengths[indices.front()];

						if (featureStrength > harrisThreshold)
						{
							validBins++;
						}
					}
				}

				const unsigned int minimalValidBins = max(1u, maximalBins * minimalValidBinsPercent / 100u);

				// check whether the current configuration is weak enough so that we can accept it
				if (validBins >= minimalValidBins)
				{
					*regionOfInterestTrackingConfiguration = candidateConfiguration;
					break;
				}
			}
		}
	}

	return true;
}

bool PointPaths::determineAutomaticTrackingConfiguration(CV::FrameProviderInterface& frameProviderInterface, const FrameType::PixelOrigin pixelOrigin, const MotionSpeed motionSpeed, const unsigned int lowerFrameIndex, const unsigned int upperFrameIndex, TrackingConfiguration& frameTrackingConfiguration, const unsigned int intermediateFrames, Worker* worker, bool* abort)
{
	ocean_assert(intermediateFrames != 0u);

	while (!frameProviderInterface.isInitialized())
	{
		if (abort && *abort)
		{
			return false;
		}

		Thread::sleep(1);
	}

	std::vector<Strengths> featureStrengthGroups(intermediateFrames);
	std::vector<Vectors2> featurePointGroups(intermediateFrames);

	frameTrackingConfiguration = TrackingConfiguration();

	unsigned int frameWidth = 0u;
	unsigned int frameHeight = 0u;

	for (unsigned int n = 0u; n < intermediateFrames; ++n)
	{
		const unsigned int frameIndex = lowerFrameIndex + (upperFrameIndex - lowerFrameIndex) * n / (intermediateFrames - 1u);
		ocean_assert(n != 0u || frameIndex == lowerFrameIndex);
		ocean_assert(n + 1 != intermediateFrames || frameIndex == upperFrameIndex);

		FrameRef frameRef = frameProviderInterface.synchronFrameRequest(frameIndex, 10.0, abort);
		if (frameRef.isNull())
		{
			return false;
		}

		Frame frame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, pixelOrigin, frame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
		{
			return false;
		}

		if (frameWidth == 0u)
		{
			frameWidth = frame.width();
			frameHeight = frame.height();
		}

		ocean_assert(frameWidth == frame.width() && frameHeight == frame.height());

		featurePointGroups[n] = CV::Detector::FeatureDetector::determineHarrisPoints(frame, CV::SubRegion(), 0u, 0u, 5u, worker, &featureStrengthGroups[n]);
	}

	ocean_assert(frameWidth != 0u && frameHeight != 0u);
	if (frameWidth == 0u || frameHeight == 0u)
	{
		return false;
	}

	unsigned int lowCoarsestLayerRadius = 0u;
	unsigned int lowPyramidLayers = 0u;
	idealPyramidParameters(frameWidth, frameHeight, motionSpeed, lowCoarsestLayerRadius, lowPyramidLayers, 16u, (unsigned int)(-1));

	unsigned int highCoarsestLayerRadius = 0u;
	unsigned int highPyramidLayers = 0u;
	idealPyramidParameters(frameWidth, frameHeight, motionSpeed, highCoarsestLayerRadius, highPyramidLayers, 26u, (unsigned int)(-1));

	const TrackingConfigurationPairs candidateConfigurationPairs =
	{
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 50u, 50u, 60u, lowCoarsestLayerRadius, lowPyramidLayers), 70u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 50u, 50u, 55u, lowCoarsestLayerRadius, lowPyramidLayers), 65u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 50u, 50u, 50u, lowCoarsestLayerRadius, lowPyramidLayers), 60u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 40u, 40u, 45u, lowCoarsestLayerRadius, lowPyramidLayers), 50u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 40u, 40u, 40u, lowCoarsestLayerRadius, lowPyramidLayers), 40u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 40u, 40u, 35u, lowCoarsestLayerRadius, lowPyramidLayers), 30u),

		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 30u, 30u, 20u, highCoarsestLayerRadius, highPyramidLayers), 25u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 30u, 30u, 20u, highCoarsestLayerRadius, highPyramidLayers), 20u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 25u, 25u, 20u, highCoarsestLayerRadius, highPyramidLayers), 20u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 20u, 20u, 15u, highCoarsestLayerRadius, highPyramidLayers), 10u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 15u, 15u, 15u, highCoarsestLayerRadius, highPyramidLayers), 10u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 15u, 15u, 10u, highCoarsestLayerRadius, highPyramidLayers), 5u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 10u, 10u, 10u, highCoarsestLayerRadius, highPyramidLayers), 5u),
		TrackingConfigurationPair(TrackingConfiguration(TM_FIXED_PATCH_SIZE_15, 5u, 5u, 5u, highCoarsestLayerRadius, highPyramidLayers), 5u)
	};

	for (const TrackingConfigurationPair& candidateConfigurationPair : candidateConfigurationPairs)
	{
		const TrackingConfiguration& candidateConfiguration = candidateConfigurationPair.first;
		const unsigned int minimalValidBinsPercent = candidateConfigurationPair.second;

		// **TODO** **HACK** the threshold must be normalized in the Harris corner detector
		const int harrisThreshold = int((candidateConfiguration.strength() * candidateConfiguration.strength() / 8u) * (candidateConfiguration.strength() * candidateConfiguration.strength() / 8u));


		ocean_assert(candidateConfiguration.horizontalBinSize() == candidateConfiguration.verticalBinSize());
		ocean_assert(minimalValidBinsPercent > 0u);

		const unsigned int horizontalBins = candidateConfiguration.horizontalBins(frameWidth, 4u);
		const unsigned int verticalBins = candidateConfiguration.verticalBins(frameHeight, 4u);

		unsigned int validFrames = 0u;

		for (unsigned int s = 0u; s < intermediateFrames; ++s)
		{
			const Vectors2& featurePoints = featurePointGroups[s];
			const Strengths& featureStrengths = featureStrengthGroups[s];

			if (!featurePoints.empty())
			{
				const Geometry::SpatialDistribution::DistributionArray distributionArray(Geometry::SpatialDistribution::distributeToArray(featurePoints.data(), featurePoints.size(), Scalar(0), Scalar(0), Scalar(frameWidth), Scalar(frameHeight), horizontalBins, verticalBins));

				unsigned int validBins = 0u;
				unsigned int maximalBins = 0u;

				for (unsigned int i = 0u; i < distributionArray.bins(); ++i)
				{
					const Indices32& indices = distributionArray[i];

					if (!indices.empty())
					{
						// we explicit count the number of maximal possible bins (so that we do not count bins in which absolutely no feature point is visible, e.g., a blue sky)
						maximalBins++;

						const int featureStrength = featureStrengths[indices.front()];

						if (featureStrength > harrisThreshold)
						{
							validBins++;
						}
					}
				}

				const unsigned int minimalValidBins = max(1u, maximalBins * minimalValidBinsPercent / 100u);

				// check whether the current configuration is weak enough for the frame
				if (validBins >= minimalValidBins)
				{
					validFrames++;
				}
			}
		}

		// check whether almost all frames support the current configuration
		if ((intermediateFrames <= 1u && validFrames == intermediateFrames) || (intermediateFrames > 1u && validFrames + 1u >= intermediateFrames))
		{
			frameTrackingConfiguration = candidateConfiguration;
			break;
		}
	}

	return true;
}

void PointPaths::idealPyramidParameters(const unsigned int width, const unsigned int height, const MotionSpeed motionSpeed, unsigned int& coarsestLayerRadius, unsigned int& layers, const unsigned int maximalCoarsestLayerRadius, const unsigned int maximalLayers)
{
	ocean_assert(maximalLayers != 0u);
	ocean_assert(maximalCoarsestLayerRadius != 0u);

	// in general the application of three pyramid layers is a good compromise between the detail level and performance

	// motion parameters:
	//     SLOW:  2.5% of frame size (e.g., 1920x1080:  48px)
	// MODERATE:  5.0% of frame size (e.g., 1920x1080:  96px)
	//     FAST: 10.0% of frame size (e.g., 1920x1080: 192px)

	ocean_assert(width != 0u && height != 0u);

	const unsigned int maxSize = max(width, height);
	Scalar sizeFactor = Scalar(0.05);

	switch (motionSpeed)
	{
		case MS_SLOW:
			sizeFactor = Scalar(0.025);
			break;

		case MS_FAST:
			sizeFactor = Scalar(0.1);
			break;

		default:
			break;
	}

	const unsigned int minimalSize = (unsigned int)Numeric::ceil(Scalar(maxSize) * sizeFactor);

	// on each layer we have an upscale factor of 2 (except on the coarsest layer):
	// coarsestLayerRadius * 2 ^ (layers - 1) >= minimalSize

	layers = 1u;
	coarsestLayerRadius = minimalSize;

	while (layers + 1u <= maximalLayers)
	{
		if (coarsestLayerRadius <= maximalCoarsestLayerRadius)
		{
			break;
		}

		layers++;
		coarsestLayerRadius = max(2u, minimalSize / (2u << (layers - 2u)));
	}

	coarsestLayerRadius = min(coarsestLayerRadius, maximalCoarsestLayerRadius);

	ocean_assert(layers <= maximalLayers);
	ocean_assert(coarsestLayerRadius <= maximalCoarsestLayerRadius);
}

bool PointPaths::trackPoints(const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const unsigned int coarsestLayerRadius, const Strengths& /*previousFeatureStrengths*/, const TrackingMethod trackingMethod, Vectors2& previousFeaturePoints, Vectors2& currentFeaturePoints, Indices32& validIndices, Worker* worker)
{
	if (previousFeaturePoints.empty())
	{
		return true;
	}

	switch (trackingMethod)
	{
		case TM_FIXED_PATCH_SIZE_7:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, coarsestLayerRadius, previousFeaturePoints, currentFeaturePoints, Scalar(0.9 * 0.9), worker, &validIndices);

		case TM_FIXED_PATCH_SIZE_15:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<15u>(previousFramePyramid, currentFramePyramid, coarsestLayerRadius, previousFeaturePoints, currentFeaturePoints, Scalar(0.9 * 0.9), worker, &validIndices);

		case TM_FIXED_PATCH_SIZE_31:
			return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<31u>(previousFramePyramid, currentFramePyramid, coarsestLayerRadius, previousFeaturePoints, currentFeaturePoints, Scalar(0.9 * 0.9), worker, &validIndices);

		default:
			break;
	}

	ocean_assert(false && "Invalid tracking method!");
	return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorder<7u>(previousFramePyramid, currentFramePyramid, coarsestLayerRadius, previousFeaturePoints, currentFeaturePoints, Scalar(0.9 * 0.9), worker, &validIndices);
}

}

}

}
