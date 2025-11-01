/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/TransitionHistory.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

int TransitionHistory::history1()
{
	return deltaMinus1;
}

int TransitionHistory::history2()
{
	return deltaMinus1 + deltaMinus2;
}

int TransitionHistory::history3()
{
	return deltaMinus1 + deltaMinus2 + deltaMinus3;
}

void TransitionHistory::push(const int newDelta)
{
	deltaMinus3 = deltaMinus2;
	deltaMinus2 = deltaMinus1;
	deltaMinus1 = newDelta;
}

void TransitionHistory::reset()
{
	deltaMinus1 = 0;
	deltaMinus2 = 0;
	deltaMinus3 = 0;
}

bool TransitionHistory::isTransitionToBlack(const uint8_t* pixel, TransitionHistory& history, const int deltaThreshold)
{
	ocean_assert(deltaThreshold >= 0);

	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta < -deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() < -(deltaThreshold * 5 / 4))
		|| (currentDelta + history.history2() < -(deltaThreshold * 3 / 2))
		|| (currentDelta + history.history3() < -(deltaThreshold * 3 / 2)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

bool TransitionHistory::isTransitionToWhite(const uint8_t* pixel, TransitionHistory& history, const int deltaThreshold)
{
	ocean_assert(deltaThreshold >= 0);

	const int currentDelta = int(*(pixel + 0) - *(pixel - 1));

	bool result = false;

	if (currentDelta > deltaThreshold)
	{
		result = true;
	}
	else if ((currentDelta + history.history1() > (deltaThreshold * 5 / 4))
		|| (currentDelta + history.history2() > (deltaThreshold * 3 / 2))
		|| (currentDelta + history.history3() > (deltaThreshold * 3 / 2)))
	{
		result = true;
	}

	history.push(currentDelta);

	return result;
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
