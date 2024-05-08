/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/orb/FeatureMap.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/FASTFeatureDetector.h"
#include "ocean/cv/detector/ORBFeatureDescriptor.h"

#include <algorithm>

namespace Ocean
{

using namespace CV;
using namespace CV::Detector;

namespace Tracking
{

namespace ORB
{

FeatureMap::FeatureMap()
{
	// nothing to do here
}

FeatureMap::FeatureMap(const Frame& frame, const Vector2& dimension, const Scalar threshold, const bool /*frameIsUndistorted*/, const unsigned int /*maxFeatures*/, const bool useHarrisFeatures, Worker* worker) :
	usingHarrisFeatures_(useHarrisFeatures)
{
	ocean_assert(dimension.x() > 0);
	if (!frame.isValid() || dimension.x() <= 0)
	{
		return;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		Log::error() << "Failed to create a new ORB feature map: Not supported pixel format.";
		return;
	}

	ORBFeatureDescriptor::detectReferenceFeaturesAndDetermineDescriptors(yFrame, features_, 3u, usingHarrisFeatures_, (unsigned int)(threshold), worker);

	ocean_assert(frame.width() > 0 && frame.height() > 0);

	const Scalar factorX = dimension.x() / Scalar(frame.width());
	const Scalar factorY = dimension.y() > 0 ? dimension.y() / Scalar(frame.height()) : factorX;

	for (ORBFeatures::iterator i = features_.begin(); i != features_.end(); ++i)
	{
		i->setPosition(Vector3(i->observation().x() * factorX, 0, i->observation().y() * factorY));
	}

	boundingBox_ = Box3(Vector3(0, 0, 0), Vector3(frame.width() * factorX, 0, frame.height() * factorY));

	plane_ = Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0));
}

Box2 FeatureMap::projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera)
{
	const HomogenousMatrix4 flipped(PinholeCamera::flippedTransformationRightSide(pose));
	const HomogenousMatrix4 iFlipped(flipped.inverted());

	return projectToImagePlaneIF(iFlipped, pinholeCamera);
}

bool FeatureMap::projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar boundary, int& left, int& top, unsigned int& width, unsigned int& height, unsigned int& leftClamped, unsigned int& topClamped, unsigned int& widthClamped, unsigned int& heightClamped)
{
	const Box2 projectedBox(projectToImagePlane(pose, pinholeCamera));
	if (!projectedBox.isValid())
	{
		return false;
	}

	left = int(projectedBox.lower().x() - boundary);
	top = int(projectedBox.lower().y() - boundary);

	int right = int(projectedBox.higher().x() + boundary);
	int bottom = int(projectedBox.higher().y() + boundary);

	if (left > right || top > bottom)
	{
		ocean_assert(false && "This should never happen");
		return false;
	}

	leftClamped = (unsigned int)max(0, left);
	unsigned int rightClamped = max(leftClamped, (unsigned int)min((int)(pinholeCamera.width() - 1), right));

	topClamped = (unsigned int)max(0, top);
	unsigned int bottomClamped = max(topClamped, (unsigned int)min((int)(pinholeCamera.height() - 1), bottom));

	ocean_assert(left <= right);
	ocean_assert(top <= bottom);

	ocean_assert(rightClamped < (unsigned int)(pinholeCamera.width()));
	ocean_assert(bottomClamped < (unsigned int)(pinholeCamera.height()));

	ocean_assert(leftClamped <= rightClamped);
	ocean_assert(topClamped <= bottomClamped);

	width = right - left;
	height = bottom - top;

	widthClamped = rightClamped - leftClamped;
	heightClamped = bottomClamped - topClamped;
	return true;
}

Box2 FeatureMap::projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera)
{
	if (!boundingBox_.isValid())
	{
		return Box2();
	}

	ocean_assert(iFlippedPose.isValid());

	return pinholeCamera.projectToImageIF<true>(iFlippedPose, boundingBox_, false);
}

bool FeatureMap::projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera, const Scalar boundary, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height)
{
	const Box2 projectedBox(projectToImagePlaneIF(iFlippedPose, pinholeCamera));
	if (!projectedBox.isValid())
	{
		return false;
	}

	const int boxLeft = max(0, int(projectedBox.lower().x() - boundary));
	const int boxRight = min(int(pinholeCamera.width() - 1), int(projectedBox.higher().x() + boundary));

	const int boxTop = max(0, int(projectedBox.lower().y() - boundary));
	const int boxBottom = min(int(pinholeCamera.height() - 1), int(projectedBox.higher().y() + boundary));

	if (boxLeft > boxRight || boxTop > boxBottom)
	{
		return false;
	}

	left = (unsigned int)(boxLeft);
	top = (unsigned int)(boxTop);
	width = (unsigned int)(boxRight - boxLeft + 1);
	height = (unsigned int)(boxBottom - boxTop + 1);

	return true;
}

}

}

}
