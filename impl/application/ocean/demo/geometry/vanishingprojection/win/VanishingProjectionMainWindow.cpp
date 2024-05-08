/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/geometry/vanishingprojection/win/VanishingProjectionMainWindow.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/advanced/FrameRectification.h"

#include "ocean/geometry/Utilities.h"
#include "ocean/geometry/VanishingProjection.h"

#include "ocean/math/Plane3.h"
#include "ocean/math/Rotation.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

VanishingProjectionMainWindow::VanishingProjectionMainWindow(HINSTANCE instance, const std::wstring& name, const std::wstring& mediaFile) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	rectificationWindowRectangle_(instance, L"Rectangle result"),
	rectificationWindowParallelogram_(instance, L"Parallelogram result")
{
	if (!mediaFile.empty())
	{
		frame_ = Media::Utilities::loadImage(String::toAString(mediaFile));
	}

	if (!frame_.isValid())
	{
		frame_ = Frame(FrameType(800u, 800u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame_.setValue(0xFF);
	}

	if (frame_)
	{
		CV::FrameConverter::Comfort::change(frame_, FrameType::ORIGIN_UPPER_LEFT);
	}
}

VanishingProjectionMainWindow::~VanishingProjectionMainWindow()
{
	// nothing to do here
}

void VanishingProjectionMainWindow::onInitialized()
{
	setFrame(frame_);

	adjustToBitmapSize();

	rectificationWindowRectangle_.setParent(handle());
	rectificationWindowRectangle_.initialize();
	rectificationWindowRectangle_.show();

	rectificationWindowParallelogram_.setParent(handle());
	rectificationWindowParallelogram_.initialize();
	rectificationWindowParallelogram_.show();
}

void VanishingProjectionMainWindow::onMouseUp(const MouseButton button, const int x, const int y)
{
	if (button == BUTTON_RIGHT)
	{
		pixelPositions_.clear();
	}
	else
	{
		if (pixelPositions_.size() >= 4)
		{
			pixelPositions_.clear();
		}

		int bitmapX, bitmapY;
		if (window2bitmap(x, y, bitmapX, bitmapY) && bitmapX >= 0 && bitmapX < int(bitmap().width()) && bitmapY >= 0 && bitmapY < int(bitmap().height()))
		{
			pixelPositions_.push_back(CV::PixelPosition(bitmapX, bitmapY));
		}

		repaint();
	}

	onPaint();
}

void VanishingProjectionMainWindow::onPaint()
{
	bitmap_ = Platform::Win::Bitmap(frame_);

	if (pixelPositions_.size() == 4)
	{
		const PinholeCamera pinholeCamera(frame_.width(), frame_.height(), Numeric::deg2rad(65));

		Vector2 points[4];
		for (unsigned int n = 0u; n < 4u; ++n)
		{
			points[n] = Vector2(Scalar(pixelPositions_[n].x()), Scalar(pixelPositions_[n].y()));
		}

		Vector3 normal;
		const bool result = Geometry::VanishingProjection::planeNormal(pinholeCamera, points, pinholeCamera.hasDistortionParameters(), normal);
		ocean_assert_and_suppress_unused(result, result);

		const Plane3 plane(Vector3(0, 0, -1), normal);

		const Vectors3 objectPoints(Geometry::Utilities::backProjectImagePoints(pinholeCamera, HomogenousMatrix4(true), plane, points, 4u, false));

		const Vector3 objectEdge01(objectPoints[1] - objectPoints[0]);
		const Vector3 objectEdge12(objectPoints[2] - objectPoints[1]);
		const Vector3 objectEdge23(objectPoints[3] - objectPoints[2]);
		const Vector3 objectEdge30(objectPoints[0] - objectPoints[3]);

		const Scalar angle02 = objectEdge01.angle(objectEdge12);
		const Scalar angle13 = objectEdge12.angle(objectEdge23);
		const Scalar angle20 = objectEdge23.angle(objectEdge30);
		const Scalar angle31 = objectEdge30.angle(objectEdge01);

		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, String::toAString(Numeric::rad2deg(angle31)) + std::string("deg"));
		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 25, String::toAString(Numeric::rad2deg(angle02)) + std::string("deg"));
		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 45, String::toAString(Numeric::rad2deg(angle13)) + std::string("deg"));
		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 65, String::toAString(Numeric::rad2deg(angle20)) + std::string("deg"));

		const Vector3 imageEdge01(points[1] - points[0]);
		const Vector3 imageEdge12(points[2] - points[1]);
		const Vector3 imageEdge23(points[3] - points[2]);
		const Vector3 imageEdge30(points[0] - points[3]);

		const Scalar imageVerticalLength = max(imageEdge01.length(), imageEdge23.length());
		const Scalar imageHorizontalLength = max(imageEdge12.length(), imageEdge30.length());

		Scalar objectVerticalLength = max(objectEdge01.length(), objectEdge23.length());
		Scalar objectHorizontalLength = max(objectEdge12.length(), objectEdge30.length());

		unsigned int horizontalLength = 0u;
		unsigned int verticalLength = 0u;

		if (objectHorizontalLength >= Numeric::weakEps() && objectVerticalLength >= Numeric::weakEps())
		{
			if (imageVerticalLength >= imageHorizontalLength)
			{
				verticalLength = (unsigned int)(imageVerticalLength + Scalar(0.5));
				horizontalLength = (unsigned int)(Scalar(verticalLength) * objectHorizontalLength / objectVerticalLength + Scalar(0.5));
			}
			else
			{
				horizontalLength = (unsigned int)(imageHorizontalLength + Scalar(0.5));
				verticalLength = (unsigned int)(Scalar(horizontalLength) * objectVerticalLength / objectHorizontalLength + Scalar(0.5));
			}
		}

		if (horizontalLength != 0u && verticalLength != 0u)
		{
			Frame rectificationFrame(FrameType(frame_, horizontalLength, verticalLength));

			Vector2 imagePoints[4] =
			{
				Vector2(0, 0),
				Vector2(0, Scalar(rectificationFrame.height())),
				Vector2(Scalar(rectificationFrame.width()), Scalar(rectificationFrame.height())),
				Vector2(Scalar(rectificationFrame.width()), 0)
			};

			Triangle2 triangles2[2] =
			{
				Triangle2(imagePoints[0], imagePoints[1], imagePoints[2]),
				Triangle2(imagePoints[0], imagePoints[2], imagePoints[3])
			};

			Vector3 correctedObjectPoints[4] =
			{
				Vector3(0, 0, 0),
				Vector3(0, 0, 0),
				Vector3(0, 0, 0),
				Vector3(0, 0, 0)
			};

			if (angle31 <= Numeric::pi_2())
			{
				// top left and bottom right has the small angle

				correctedObjectPoints[0] = objectPoints[0];

				const Vector3 correctedDirection01((Rotation(normal, -(Numeric::pi_2() - angle31) * Scalar(0.5)) * (objectPoints[1] - objectPoints[0])).normalized());
				const Vector3 correctedDirection21((Rotation(normal, (Numeric::pi_2() - angle31) * Scalar(0.5)) * (objectPoints[1] - objectPoints[2])).normalized());

				Line3(objectPoints[0], correctedDirection01).nearestPoint(Line3(objectPoints[2], correctedDirection21), correctedObjectPoints[1]);
				ocean_assert(plane.isInPlane(correctedObjectPoints[1]));

				correctedObjectPoints[2] = objectPoints[2];

				const Vector3 correctedDirection23((Rotation(normal, -(Numeric::pi_2() - angle31) * Scalar(0.5)) * (objectPoints[3] - objectPoints[2])).normalized());
				const Vector3 correctedDirection03((Rotation(normal, (Numeric::pi_2() - angle31) * Scalar(0.5)) * (objectPoints[3] - objectPoints[0])).normalized());

				Line3(objectPoints[2], correctedDirection23).nearestPoint(Line3(objectPoints[0], correctedDirection03), correctedObjectPoints[3]);
				ocean_assert(plane.isInPlane(correctedObjectPoints[3]));
			}
			else
			{
				ocean_assert(angle02 <= Numeric::pi_2());

				// bottom left and top right has the small angle

				correctedObjectPoints[1] = objectPoints[1];

				const Vector3 correctedDirection12((Rotation(normal, -(Numeric::pi_2() - angle02) * Scalar(0.5)) * (objectPoints[2] - objectPoints[1])).normalized());
				const Vector3 correctedDirection32((Rotation(normal, (Numeric::pi_2() - angle02) * Scalar(0.5)) * (objectPoints[2] - objectPoints[3])).normalized());

				Line3(objectPoints[1], correctedDirection12).nearestPoint(Line3(objectPoints[3], correctedDirection32), correctedObjectPoints[2]);
				ocean_assert(plane.isInPlane(correctedObjectPoints[2]));

				correctedObjectPoints[3] = objectPoints[3];

				const Vector3 correctedDirection30((Rotation(normal, -(Numeric::pi_2() - angle02) * Scalar(0.5)) * (objectPoints[0] - objectPoints[3])).normalized());
				const Vector3 correctedDirection10((Rotation(normal, (Numeric::pi_2() - angle02) * Scalar(0.5)) * (objectPoints[0] - objectPoints[1])).normalized());

				Line3(objectPoints[3], correctedDirection30).nearestPoint(Line3(objectPoints[1], correctedDirection10), correctedObjectPoints[0]);
				ocean_assert(plane.isInPlane(correctedObjectPoints[0]));
			}

			Triangle3 triangles3[2] =
			{
				Triangle3(objectPoints[0], objectPoints[1], objectPoints[2]),
				Triangle3(objectPoints[0], objectPoints[2], objectPoints[3])
			};

			CV::Advanced::FrameRectification::Comfort::triangleObject(frame_, AnyCameraPinhole(pinholeCamera), HomogenousMatrix4(true), triangles2[0], triangles3[0], rectificationFrame);
			CV::Advanced::FrameRectification::Comfort::triangleObject(frame_, AnyCameraPinhole(pinholeCamera), HomogenousMatrix4(true), triangles2[1], triangles3[1], rectificationFrame);

			rectificationWindowRectangle_.setFrame(rectificationFrame);
			rectificationWindowRectangle_.update();
			rectificationWindowRectangle_.adjustToBitmapSize();

			Triangle3 parallelogramTriangles3[2] =
			{
				Triangle3(correctedObjectPoints[0], correctedObjectPoints[1], correctedObjectPoints[2]),
				Triangle3(correctedObjectPoints[0], correctedObjectPoints[2], correctedObjectPoints[3])
			};

			CV::Advanced::FrameRectification::Comfort::triangleObject(frame_, AnyCameraPinhole(pinholeCamera), HomogenousMatrix4(true), triangles2[0], parallelogramTriangles3[0], rectificationFrame);
			CV::Advanced::FrameRectification::Comfort::triangleObject(frame_, AnyCameraPinhole(pinholeCamera), HomogenousMatrix4(true), triangles2[1], parallelogramTriangles3[1], rectificationFrame);

			rectificationWindowParallelogram_.setFrame(rectificationFrame);
			rectificationWindowParallelogram_.update();
			rectificationWindowParallelogram_.adjustToBitmapSize();
		}
	}

	if (!pixelPositions_.empty())
	{
		HPEN pen = CreatePen(0, 3, 0);
		HPEN old = (HPEN)SelectObject(bitmap().dc(), pen);

		MoveToEx(bitmap().dc(), int(pixelPositions_.back().x()), int(pixelPositions_.back().y()), nullptr);
		for (unsigned int n = 0; n < pixelPositions_.size(); ++n)
		{
			LineTo(bitmap().dc(), int(pixelPositions_[n].x()), int(pixelPositions_[n].y()));
		}

		SelectObject(bitmap().dc(), old);
		DeleteObject(pen);
	}

	BitmapWindow::onPaint();
}
