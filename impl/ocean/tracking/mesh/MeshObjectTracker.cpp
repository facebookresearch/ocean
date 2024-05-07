// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/mesh/MeshObjectTracker.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

MeshObjectTracker::~MeshObjectTracker()
{
	// nothing to do here
}

unsigned int MeshObjectTracker::add(const Frame& textureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker)
{
	if (!textureFrame.isValid() || !meshUVTextureMappingRef || !meshUVTextureMappingRef->isValid())
	{
		return MeshObjectTrackerCore::kInvalidRegisteredObjectId;
	}

	Frame yTextureFrame;
	if (!CV::FrameConverter::Comfort::convert(textureFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yTextureFrame, false, worker))
	{
		return MeshObjectTrackerCore::kInvalidRegisteredObjectId;
	}

	return MeshObjectTrackerCore::add(yTextureFrame, meshUVTextureMappingRef, worker);
}

bool MeshObjectTracker::remove(const unsigned int patternId)
{
	return MeshObjectTrackerCore::remove(patternId);
}

bool MeshObjectTracker::clear()
{
	return MeshObjectTrackerCore::clear();
}

bool MeshObjectTracker::determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& previousCamera_R_camera, Worker* worker)
{
	ocean_assert(frames.size() == 1 && frames.front());
	ocean_assert(anyCameras.size() == 1 && anyCameras.front() && anyCameras.front()->isValid());

	if (anyCameras.front()->name() != AnyCameraPinhole::WrappedCamera::name())
	{
		Log::error() << "Incompatible camera!";
		return false;
	}

	const Frame& frame = frames.front();
	ocean_assert(frame.isValid());

	const std::shared_ptr<AnyCameraPinhole> anyCameraPinhole = std::dynamic_pointer_cast<AnyCameraPinhole>(anyCameras.front());
	ocean_assert(anyCameraPinhole && anyCameraPinhole->isValid());

	const PinholeCamera& pinholeCamera = anyCameraPinhole->actualCamera();
	ocean_assert(pinholeCamera.isValid());

	ocean_assert(frame.width() == pinholeCamera.width() && frame.height() == pinholeCamera.height());

	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yTrackerFrame_, false, worker))
	{
		return false;
	}

	ocean_assert(yTrackerFrame_.width() == pinholeCamera.width() && yTrackerFrame_.height() == pinholeCamera.height());
	yTrackerFrame_.setTimestamp(frame.timestamp());

	return MeshObjectTrackerCore::determinePoses(yTrackerFrame_, pinholeCamera, transformations, previousCamera_R_camera, worker);
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
