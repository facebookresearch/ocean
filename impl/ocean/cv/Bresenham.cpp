/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/Bresenham.h"

namespace Ocean
{

namespace CV
{

Bresenham::Bresenham(const int x0, const int y0, const int x1, const int y1)
{
	const int deltaX = x1 - x0;
	const int deltaY = y1 - y0;
	orientation_ = ORIENTATION_INVALID;

	if (deltaX == 0 && deltaY == 0)
	{
		ocean_assert(!isValid());
		return;
	}

	int a = 0;
	int b = 0;

	if (deltaX >= 0)
	{
		if (deltaY >= 0)
		{
			if (deltaX >= deltaY)
			{
				orientation_ = ORIENTATION_0;

				a = deltaY;
				b = -deltaX;
			}
			else
			{
				orientation_ = ORIENTATION_1;

				a = deltaX;
				b = -deltaY;
			}
		}
		else
		{
			if (deltaX > -deltaY)
			{
				orientation_ = ORIENTATION_7;

				a = -deltaY;
				b = -deltaX;
			}
			else
			{
				orientation_ = ORIENTATION_6;

				a = deltaX;
				b = deltaY;
			}
		}
	}
	else // deltaX < 0
	{
		if (deltaY >= 0)
		{
			if (-deltaX >= deltaY)
			{
				orientation_ = ORIENTATION_3;

				a = deltaY;
				b = deltaX;
			}
			else
			{
				orientation_ = ORIENTATION_2;

				a = -deltaX;
				b = -deltaY;
			}
		}
		else
		{
			if (-deltaX > -deltaY)
			{
				orientation_ = ORIENTATION_4;

				a = -deltaY;
				b = deltaX;
			}
			else
			{
				orientation_ = ORIENTATION_5;

				a = -deltaX;
				b = deltaY;
			}
		}
	}

	control_ = 2 * a + b;
	updateX_ = 2 * a;
	updateXY_ = 2 * (a + b);

	ocean_assert(orientation_ != ORIENTATION_INVALID);
}

void Bresenham::findNext(int& x, int& y)
{
	ocean_assert(isValid());

	switch (orientation_)
	{
		case ORIENTATION_0:
		{
			++x;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				++y;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_1:
		{
			++y;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				++x;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_7:
		{
			++x;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				--y;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_6:
		{
			--y;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				++x;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_3:
		{
			--x;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				++y;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_2:
		{
			++y;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				--x;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_4:
		{
			--x;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				--y;
				control_ += updateXY_;
			}

			break;
		}

		case ORIENTATION_5:
		{
			--y;

			if (control_ <= 0)
			{
				control_ += updateX_;
			}
			else
			{
				--x;
				control_ += updateXY_;
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid parameter!");
	}
}

bool Bresenham::borderIntersection(const Line2& line, const int leftBorder, const int topBorder, const int rightBorder, const int bottomBorder, int& x0, int& y0, int& x1, int& y1)
{
	ocean_assert(line.isValid());
	ocean_assert(leftBorder <= rightBorder && topBorder <= bottomBorder);

	const Vector2& point = line.point();
	const Vector2& direction = line.direction();

	// if the line is oriented from top to bottom
	if (Numeric::isEqualEps(direction.x()))
	{
		const int x = Numeric::round32(point.x());

		x0 = x;
		y0 = topBorder;

		x1 = x;
		y1 = bottomBorder;

		return x >= leftBorder && x <= rightBorder;
	}

	// determine intersection with left border
	// leftBorder = px + t * dx
	// t = (leftBorder - px) / dx

	const Scalar invX = Scalar(1) / direction.x();

	Scalar t = (Scalar(leftBorder) - point.x()) * invX;
	const Scalar leftY = point.y() + t * direction.y();

	ocean_assert(line.isOnLine(Vector2(Scalar(leftBorder), leftY)));

	// if line has a visible intersection with the left border
	if (leftY >= Scalar(topBorder) && leftY <= Scalar(bottomBorder))
	{
		x0 = leftBorder;
		y0 = Numeric::round32(leftY);

		// determine end position with right border
		// rightBorder == px + t * dx
		// t = (rightBorder - px) / dx

		t = (Scalar(rightBorder) - point.x()) * invX;
		const Scalar rightY = point.y() + t * direction.y();

		ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

		// if line has visible intersection with the right border
		if (rightY >= Scalar(topBorder) && rightY <= Scalar(bottomBorder))
		{
			x1 = rightBorder;
			y1 = Numeric::round32(rightY);
			return true;
		}

		// determine end position with top border
		// topBorder == py + t * dy
		// t = (topBorder - py) / dy

		const Scalar invY = Scalar(1) / direction.y();

		t = (Scalar(topBorder) - point.y()) * invY;
		const Scalar topX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(topX, Scalar(topBorder))));

		// if line has visible intersection with the top border
		if (topX >= Scalar(leftBorder) && topX <= Scalar(rightBorder))
		{
			x1 = Numeric::round32(topX);
			y1 = topBorder;

			if (x0 != x1 || y0 != y1)
				return true;
		}

		// determine end position with bottom border
		// bottomBorder == py + t * dy
		// t = (bottomBorder - py) / dy

		t = (Scalar(bottomBorder) - point.y()) * invY;
		const Scalar bottomX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

		// if line has visible intersection with the bottom border
		if (bottomX >= Scalar(leftBorder) && bottomX <= Scalar(rightBorder))
		{
			x1 = Numeric::round32(bottomX);
			y1 = bottomBorder;

			return x0 != x1 || y0 != y1;
		}
	}
	else // the line does not have a visible intersection with the left border, check for the top border
	{
		// if the line is oriented from left to right
		if (Numeric::isEqualEps(direction.y()))
		{
			const int y = Numeric::round32(point.y());

			x0 = leftBorder;
			y0 = y;

			x1 = rightBorder;
			y1 = y;

			return y >= topBorder && y <= bottomBorder;
		}

		// determine begin position with top border
		// topBorder == py + t * dy
		// t = (topBorder - py) / dy

		const Scalar invY = Scalar(1) / direction.y();

		t = (Scalar(topBorder) - point.y()) * invY;
		const Scalar topX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(topX, Scalar(topBorder))));

		// if line has visible intersection with the top border
		if (topX >= Scalar(leftBorder) && topX <= Scalar(rightBorder))
		{
			x0 = Numeric::round32(topX);
			y0 = topBorder;

			// determine end position with right border
			// rightBorder == px + t * dx
			// t = (rightBorder - px) / dx

			t = (Scalar(rightBorder) - point.x()) * invX;
			const Scalar rightY = point.y() + t * direction.y();

			ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

			// if line has visible intersection with the right border
			if (rightY >= Scalar(topBorder) && rightY <= Scalar(bottomBorder))
			{
				x1 = rightBorder;
				y1 = Numeric::round32(rightY);

				return x0 != x1 || y0 != y1;
			}

			// determine end position with bottom border
			// bottomBorder == py + t * dy
			// t = (bottomBorder - py) / dy

			t = (Scalar(bottomBorder) - point.y()) * invY;
			const Scalar bottomX = point.x() + t * direction.x();

			ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

			// if line has visible intersection with the bottom border
			if (bottomX >= Scalar(leftBorder) && bottomX <= Scalar(rightBorder))
			{
				x1 = Numeric::round32(bottomX);
				y1 = bottomBorder;

				if (x0 > x1)
				{
					std::swap(x1, x0);
					std::swap(y1, y0);
				}

				return x0 != x1 || y0 != y1;
			}
		}
		else // the line does not have a visible intersection with the left border or the top border, check the bottom border
		{
			// determine begin position with bottom border
			// bottomBorder == py + t * dy
			// t = (bottomBorder - py) / dy

			t = (Scalar(bottomBorder) - point.y()) * invY;
			const Scalar bottomX = point.x() + t * direction.x();

			ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

			// if line has visible intersection with the bottom border
			if (bottomX >= Scalar(leftBorder) && bottomX <= Scalar(rightBorder))
			{
				x0 = Numeric::round32(bottomX);
				y0 = bottomBorder;

				// determine end position with right border
				// rightBorder == px + t * dx
				// t = (rightBorder - px) / dx

				t = (Scalar(rightBorder) - point.x()) * invX;
				const Scalar rightY = point.y() + t * direction.y();

				ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

				// if line has visible intersection with the right border
				if (rightY >= Scalar(topBorder) && rightY <= Scalar(bottomBorder))
				{
					x1 = rightBorder;
					y1 = Numeric::round32(rightY);

					return x0 != x1 || y0 != y1;
				}
			}
		}
	}

	// the line is not visible in the frame
	return false;
}

bool Bresenham::borderIntersection(const Line2& line, const Scalar leftBorder, const Scalar topBorder, const Scalar rightBorder, const Scalar bottomBorder, Scalar& x0, Scalar& y0, Scalar& x1, Scalar& y1)
{
	ocean_assert(line.isValid());
	ocean_assert(leftBorder <= rightBorder && topBorder <= bottomBorder);

	const Vector2& point = line.point();
	const Vector2& direction = line.direction();

	// if the line is oriented from top to bottom
	if (Numeric::isEqualEps(direction.x()))
	{
		x0 = point.x();
		y0 = topBorder;

		x1 = point.x();
		y1 = bottomBorder;

		return point.x() >= leftBorder && point.x() <= rightBorder;
	}


	// determine intersection with left border
	// leftBorder = px + t * dx
	// t = (leftBorder - px) / dx

	const Scalar invX = Scalar(1) / direction.x();

	Scalar t = (Scalar(leftBorder) - point.x()) * invX;
	const Scalar leftY = point.y() + t * direction.y();

	ocean_assert(line.isOnLine(Vector2(Scalar(leftBorder), leftY)));

	// if line has a visible intersection with the left border
	if (leftY >= topBorder && leftY <= bottomBorder)
	{
		x0 = leftBorder;
		y0 = leftY;

		// determine end position with right border
		// rightBorder == px + t * dx
		// t = (rightBorder - px) / dx

		t = (Scalar(rightBorder) - point.x()) * invX;
		const Scalar rightY = point.y() + t * direction.y();

		ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

		// if line has visible intersection with the right border
		if (rightY >= topBorder && rightY <= bottomBorder)
		{
			x1 = rightBorder;
			y1 = rightY;
			return true;
		}

		// determine end position with top border
		// topBorder == py + t * dy
		// t = (topBorder - py) / dy

		const Scalar invY = Scalar(1) / direction.y();

		t = (Scalar(topBorder) - point.y()) * invY;
		const Scalar topX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(topX, Scalar(topBorder))));

		// if line has visible intersection with the top border
		if (topX >= leftBorder && topX <= rightBorder)
		{
			x1 = topX;
			y1 = topBorder;

			if (Numeric::isNotEqual(x0, x1) || Numeric::isNotEqual(y0, y1))
				return true;
		}

		// determine end position with bottom border
		// bottomBorder == py + t * dy
		// t = (bottomBorder - py) / dy

		t = (Scalar(bottomBorder) - point.y()) * invY;
		const Scalar bottomX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

		// if line has visible intersection with the bottom border
		if (bottomX >= leftBorder && bottomX <= rightBorder)
		{
			x1 = bottomX;
			y1 = bottomBorder;

			return Numeric::isNotEqual(x0, x1) || Numeric::isNotEqual(y0, y1);
		}
	}
	else // the line does not have a visible intersection with the left border, check for the top border
	{
		// determine begin position with top border
		// topBorder == py + t * dy
		// t = (topBorder - py) / dy

		// if the line is oriented from left to right
		if (Numeric::isEqualEps(direction.y()))
		{
			x0 = leftBorder;
			y0 = point.y();

			x1 = rightBorder;
			y1 = point.y();

			return point.y() >= topBorder && point.y() <= bottomBorder;
		}

		const Scalar invY = Scalar(1) / direction.y();

		t = (Scalar(topBorder) - point.y()) * invY;
		const Scalar topX = point.x() + t * direction.x();

		ocean_assert(line.isOnLine(Vector2(topX, Scalar(topBorder))));

		// if line has visible intersection with the top border
		if (topX >= leftBorder && topX <= rightBorder)
		{
			x0 = topX;
			y0 = topBorder;

			// determine end position with right border
			// rightBorder == px + t * dx
			// t = (rightBorder - px) / dx

			t = (Scalar(rightBorder) - point.x()) * invX;
			const Scalar rightY = point.y() + t * direction.y();

			ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

			// if line has visible intersection with the right border
			if (rightY >= topBorder && rightY <= bottomBorder)
			{
				x1 = rightBorder;
				y1 = rightY;

				return Numeric::isNotEqual(x0, x1) || Numeric::isNotEqual(y0, y1);
			}

			// determine end position with bottom border
			// bottomBorder == py + t * dy
			// t = (bottomBorder - py) / dy

			t = (Scalar(bottomBorder) - point.y()) * invY;
			const Scalar bottomX = point.x() + t * direction.x();

			ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

			// if line has visible intersection with the bottom border
			if (bottomX >= leftBorder && bottomX <= rightBorder)
			{
				x1 = bottomX;
				y1 = bottomBorder;

				if (x0 > x1)
				{
					std::swap(x1, x0);
					std::swap(y1, y0);
				}

				return Numeric::isNotEqual(x0, x1) || Numeric::isNotEqual(y0, y1);
			}
		}
		else // the line does not have a visible intersection with the left border or the top border, check the bottom border
		{
			// determine begin position with bottom border
			// bottomBorder == py + t * dy
			// t = (bottomBorder - py) / dy

			t = (Scalar(bottomBorder) - point.y()) * invY;
			const Scalar bottomX = point.x() + t * direction.x();

			ocean_assert(line.isOnLine(Vector2(bottomX, Scalar(bottomBorder))));

			// if line has visible intersection with the bottom border
			if (bottomX >= leftBorder && bottomX <= rightBorder)
			{
				x0 = bottomX;
				y0 = bottomBorder;

				// determine end position with right border
				// rightBorder == px + t * dx
				// t = (rightBorder - px) / dx

				t = (Scalar(rightBorder) - point.x()) * invX;
				const Scalar rightY = point.y() + t * direction.y();

				ocean_assert(line.isOnLine(Vector2(Scalar(rightBorder), rightY)));

				// if line has visible intersection with the right border
				if (rightY >= topBorder && rightY <= bottomBorder)
				{
					x1 = rightBorder;
					y1 = rightY;

					return Numeric::isNotEqual(x0, x1) || Numeric::isNotEqual(y0, y1);
				}
			}
		}
	}

	// the line is not visible in the frame
	return false;
}

unsigned int Bresenham::numberLinePixels(const int x0, const int y0, const int x1, const int y1)
{
	int x = x0;
	int y = y0;

	Bresenham bresenham(x, y, x1, y1);

	if (!bresenham.isValid())
	{
		ocean_assert(x0 == x1 && y0 == y1);
		return 1u;
	}

	unsigned int number = 1u;

	while (x != x1 || y != y1)
	{
		bresenham.findNext(x, y);
		++number;
	}

	return number;
}

void Bresenham::intermediatePixels(const PixelPosition& position0, const PixelPosition& position1, PixelPositions& pixels)
{
	ocean_assert(position0 && position1);

	int x = int(position0.x());
	int y = int(position0.y());

	const int xEnd = int(position1.x());
	const int yEnd = int(position1.y());

	pixels.reserve(pixels.size() + NumericT<int>::abs(x - xEnd) + NumericT<int>::abs(y - yEnd));

	Bresenham bresenham(x, y, xEnd, yEnd);

	if (!bresenham.isValid())
	{
		ocean_assert(position0 == position1);
		return;
	}

	bresenham.findNext(x, y);

	while (x != xEnd || y != yEnd)
	{
		pixels.emplace_back(x, y);
		bresenham.findNext(x, y);
	}
}

}

}
