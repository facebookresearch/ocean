/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/homographyimagealigner/win/HIAMainWindow.h"

#include "ocean/base/Thread.h"

#include "ocean/platform/win/Utilities.h"

HIAMainWindow::HIAMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::wstring>& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	applicationHomographyImageAligner(commandArguments)
{
	initialize();
	start();
}

HIAMainWindow::~HIAMainWindow()
{
	applicationHomographyImageAligner.release();
}

void HIAMainWindow::onIdle()
{
	Frame resultingAlignerFrame;
	double resultingAlignerPerformance;

	SquareMatrix3 currentHomographyPrevious;
	Vectors2 previousPoints, currentPoints;
	Indices32 validPointIndices;

	bool reachedLastFrame = false;

	if (applicationHomographyImageAligner.alignNewFrame(resultingAlignerFrame, resultingAlignerPerformance, &currentHomographyPrevious, &previousPoints, &currentPoints, &validPointIndices, &reachedLastFrame) && resultingAlignerFrame.isValid())
	{
#if 0
		// we verify that the resulting homography matches with the entire set of resulting point correspondences

		ocean_assert(previousPoints.size() == currentPoints.size());

		for (size_t n = 0; n < previousPoints.size(); ++n)
		{
			const Vector2 transformedPreviousPoint(currentHomographyPrevious * previousPoints[n]);
			ocean_assert(currentPoints[n].isEqual(transformedPreviousPoint, 5)); // we are quite generous be using 5 pixels as threshold
		}
#endif

		setFrame(resultingAlignerFrame);

		if (resultingAlignerPerformance >= 0.0)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, String::toAString(resultingAlignerPerformance * 1000.0, 2u) + std::string("ms"));
		}

		repaint(false);
	}
	else
	{
		if (reachedLastFrame)
		{
			Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, "Last frame reached...");

			repaint(false);

			close(); // All done, so exit (in case being called from a script)
		}

		Thread::sleep(1u);
	}
}
