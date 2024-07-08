/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/maintenanceviewer/win/MVMainWindow.h"
#include "application/ocean/demo/misc/maintenanceviewer/win/MVMaintenance.h"

#include "ocean/base/Maintenance.h"
#include "ocean/base/String.h"

#include "ocean/io/Utilities.h"

#include "ocean/media/wic/Image.h"

MVMainWindow::MVMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	ApplicationWindow(instance, name),
	renderWindow_(instance, L"Renderer"),
	textWindow_(instance, L"Text", true),
	bitmapWindow_(instance, L"Frames")
{
	// nothing to do here
}

MVMainWindow::~MVMainWindow()
{
	// nothing to do here
}

void MVMainWindow::onInitialized()
{
	renderWindow_.setParent(handle());
	renderWindow_.initialize(nullptr, "renderer");
	renderWindow_.show();

	textWindow_.setParent(handle());
	textWindow_.initialize(nullptr, "");
	textWindow_.setText("");
	textWindow_.show();

	bitmapWindow_.setParent(handle());
	bitmapWindow_.initialize();
	bitmapWindow_.show();

	resize(600, 800);
}

void MVMainWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	textWindow_.resize(clientWidth, clientHeight);
}

void MVMainWindow::onIdle()
{
	std::string name, tag;
	unsigned long long id;
	Maintenance::Buffer buffer;
	Timestamp timestamp;

	if (Maintenance::get().receive(name, id, tag, buffer, timestamp))
	{
		if (tag == "OCEAN_MESSENGER")
		{
			const std::string line((char*)buffer.data(), buffer.size());

			ApplicationIdMap::const_iterator i = applicationIdMap_.find(id);
			if (i == applicationIdMap_.cend())
			{
				i = applicationIdMap_.insert(std::make_pair(id, (unsigned int)(applicationIdMap_.size()) + 1u)).first;
			}

			std::string output = String::toAString(i->second, 3) + std::string("> ");

			if (!name.empty())
			{
				output += name + std::string(": ");
			}

			output += line;

			textWindow_.appendText(output + std::string("\n"));
		}
		else if (tag == "FRAME")
		{
			Frame frame = Media::WIC::Image::decodeImage(buffer.data(), buffer.size());

			if (frame)
			{
				bitmapWindow_.setFrame(frame);
				bitmapWindow_.repaint();
			}
		}
		else if (tag == "OBJECT_POINTS")
		{
			Vectors3 objectPoints;
			if (IO::Utilities::decodeVectors3(buffer, objectPoints))
			{
				renderWindow_.clear();
				renderWindow_.addPoints(objectPoints, true);
			}
		}
		else if (tag == "ENVIRONMENT")
		{
			Frame frame;
			Vectors2 frameImagePoints;
			Vectors3 frameObjectPoints;
			HomogenousMatrix4 framePose;
			Vectors3 objectPoints;

			if (MVMaintenance::decodeEnvironment(buffer, frame, frameImagePoints, frameObjectPoints, framePose, objectPoints))
			{
				// ensuring that we do not receive an image point twice, O(n^2) but good enough for now

				for (size_t nOuter = 0; nOuter + 1 < frameImagePoints.size(); ++nOuter)
				{
					for (size_t nInner = nOuter + 1; nInner < frameImagePoints.size(); /* noop */)
					{
						if (frameImagePoints[nOuter].sqrDistance(frameImagePoints[nInner]) < Scalar(0.0001))
						{
							frameImagePoints[nInner] = frameImagePoints.back();
							frameObjectPoints[nInner] = frameObjectPoints.back();

							frameImagePoints.pop_back();
							frameObjectPoints.pop_back();
						}
						else
						{
							++nInner;
						}
					}
				}

				renderWindow_.clear();
				renderWindow_.addMesh(frameObjectPoints, frameImagePoints, frame);
				renderWindow_.addPoints(objectPoints);

				if (framePose.isValid())
				{
					renderWindow_.updateCoordinateSystem(framePose);
				}
			}
		}
		else if (tag == "EXTRINSIC_CAMERA_MATRIX_16")
		{
			HomogenousMatrix4 pose;
			if (IO::Utilities::decodeHomogenousMatrix4(buffer, pose) && pose.isValid())
			{
				renderWindow_.updateCoordinateSystem(HomogenousMatrix4(pose));
			}
		}
		else if (tag == "PLANE_TRANSFORMATION_16")
		{
			if (buffer.size() == sizeof(double) * 16)
			{
				const HomogenousMatrixD4 planeTransformation((double*)buffer.data());

				renderWindow_.updatePlane(HomogenousMatrix4(planeTransformation));
			}
		}
		else if (tag == "HIGHLIGHT_OBJECT_POINTS")
		{
			Vectors3 objectPoints;

			if (IO::Utilities::decodeVectors3(buffer, objectPoints))
			{
				renderWindow_.updateHighlightObjectPoints(objectPoints);
			}
		}
		else if (tag == "ADD_CAMERA")
		{
			HomogenousMatrix4 cameraTransformation;
			if (IO::Utilities::decodeHomogenousMatrix4(buffer, cameraTransformation) && cameraTransformation.isValid())
			{
				renderWindow_.addCamera(cameraTransformation);
				renderWindow_.update();
			}
		}
	}

	renderWindow_.onIdle();
}
