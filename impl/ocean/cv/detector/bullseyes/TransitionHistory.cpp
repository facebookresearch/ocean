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

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
