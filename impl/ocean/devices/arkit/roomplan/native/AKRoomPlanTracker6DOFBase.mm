// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFBase.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

std::string AKRoomPlanTracker6DOFBase::translateInstruction(const InstructionValue instructionValue)
{
	switch (instructionValue)
	{
		case IV_UNKNOWN:
			return "unknown";

		case IV_MOVE_CLOSE_TO_WALL:
			return "moveCloseToWall";

		case IV_MOVE_AWAY_FROM_WALL:
			return "moveAwayFromWall";

		case IV_SLOW_DOWN:
			return "slowDown";

		case IV_TURN_ON_LIGHT:
			return "turnOnLight";

		case IV_NORMAL:
			return "normal";

		case IV_LOW_TEXTURE:
			return "lowTexture";
	}

	ocean_assert(false && "Invalid instruction!");
	return "";
}

}

}

}
