// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/Helpers.h"
#include "ocean/io/vrs/Reader.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

bool Helpers::readHandTrackingFromVRS(const std::string& vrsFile, std::map<double, HomogenousMatricesD4>& map_world_T_leftHandJoints, std::map<double, HomogenousMatricesD4>& map_world_T_rightHandJoints)
{
	IO::VRS::Reader reader;

	IO::VRS::PlayableHomogenousMatrices4 playableHandPosesLeft;
	IO::VRS::PlayableHomogenousMatrices4 playableHandPosesRight;

	reader.addPlayable(&playableHandPosesLeft, vrs::RecordableTypeId::PoseRecordableClass, "hand_pose_left");
	reader.addPlayable(&playableHandPosesRight, vrs::RecordableTypeId::PoseRecordableClass, "hand_pose_right");

	if (reader.read(vrsFile) != 2)
	{
		return false;
	}

	map_world_T_leftHandJoints = std::move(playableHandPosesLeft.homogenousMatrices4Map());
	map_world_T_rightHandJoints = std::move(playableHandPosesRight.homogenousMatrices4Map());

	return !map_world_T_leftHandJoints.empty() && !map_world_T_rightHandJoints.empty();
}

}

}

}
