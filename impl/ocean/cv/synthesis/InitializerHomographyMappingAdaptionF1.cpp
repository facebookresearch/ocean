/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerHomographyMappingAdaptionF1.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

void InitializerHomographyMappingAdaptionF1::initializeSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	if (sourceMappingF1_)
	{
		initializeSubsetF1(firstColumn, numberColumns, firstRow, numberRows);
	}
	else
	{
		initializeSubsetI1(firstColumn, numberColumns, firstRow, numberRows);
	}
}

void InitializerHomographyMappingAdaptionF1::initializeSubsetF1(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(sourceMappingF1_);

	const uint8_t* const maskData = layerF_.mask().constdata<uint8_t>();
	const SquareMatrix3 invertedHomography(homography_.inverted());

	const unsigned int layerWidth = layerF_.width();
	const unsigned int layerHeight = layerF_.height();

	ocean_assert(firstColumn + numberColumns <= layerWidth);
	ocean_assert(firstRow + numberRows <= layerHeight);

	RandomGenerator generator(randomGenerator_);

	const unsigned int maskStrideElements = layerF_.mask().strideElements();

	const Vector2* sourceMapping = (*sourceMappingF1_)();
	Vector2* mapping = layerF_.mapping()();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskData[y * maskStrideElements + x] != 0xFF)
			{
				const Vector2 currentPosition = Vector2(Scalar(x), Scalar(y));
				const Vector2 previousPosition(homography_ * currentPosition);

				const int left = int(previousPosition.x());
				const int top = int(previousPosition.y());

				const int right = left + 1;
				const int bottom = top + 1;

				if (left >= 0 && top >= 0 && right < int(layerWidth) && bottom < int(layerHeight))
				{
					// **TODO** use previous inpainting mask to determine whether a mapping is valid
					const Vector2& positionTopLeft = sourceMapping[top * layerWidth + left];
					const Vector2& positionTopRight = sourceMapping[top * layerWidth + right];
					const Vector2& positionBottomLeft = sourceMapping[bottom * layerWidth + left];
					const Vector2& positionBottomRight = sourceMapping[bottom * layerWidth + right];

					if (positionTopLeft.x() > 0 && positionTopRight.x() > 0 && positionBottomLeft.x() > 0 && positionBottomRight.x() > 0)
					{
						const int roundedX = Numeric::round32(previousPosition.x());
						const int roundedY = Numeric::round32(previousPosition.y());

						const Scalar tx = previousPosition.x() - Scalar(left);
						const Scalar ty = previousPosition.y() - Scalar(top);
						const Scalar tx_ = Scalar(1) - tx;
						const Scalar ty_ = Scalar(1) - ty;

						ocean_assert(tx >= 0 && tx <= 1);
						ocean_assert(ty >= 0 && ty <= 1);

						ocean_assert(tx_ >= 0 && tx_ <= 1);
						ocean_assert(ty_ >= 0 && ty_ <= 1);

						const Vector2 newPositionTopLeft(invertedHomography * Vector2(Scalar(positionTopLeft.x()), Scalar(positionTopLeft.y())));
						const Vector2 newPositionTopRight(invertedHomography * Vector2(Scalar(positionTopRight.x()), Scalar(positionTopRight.y())));
						const Vector2 newPositionBottomLeft(invertedHomography * Vector2(Scalar(positionBottomLeft.x()), Scalar(positionBottomLeft.y())));
						const Vector2 newPositionBottomRight(invertedHomography * Vector2(Scalar(positionBottomRight.x()), Scalar(positionBottomRight.y())));

						Vector2 newPosition;

						// 0 1
						// 2 3

						if (roundedX == left && roundedY == top)
						{
							// if top left (0) is the dominant position
							// 0 1
							// 2 3

							const bool distance01 = newPositionTopLeft.sqrDistance(newPositionTopRight) < 2 * 2;
							const bool distance02 = newPositionTopLeft.sqrDistance(newPositionBottomLeft) < 2 * 2;
							const bool distance03 = newPositionTopLeft.sqrDistance(newPositionBottomRight) < 2 * 2;

							if (distance01 && distance02 && distance03)
								newPosition = (newPositionTopLeft * tx_ + newPositionTopRight * tx) * ty_ + (newPositionBottomLeft * tx_ + newPositionBottomRight * tx) * ty;
							/*else if (distance01 && distance02)
								newPosition = (newPositionTopLeft + newPositionTopRight + newPositionBottomLeft) / 3;
							else if (distance01 && distance03)
								newPosition = (newPositionTopLeft + newPositionTopRight + newPositionBottomRight) / 3;
							else if (distance02 && distance03)
								newPosition = (newPositionTopLeft + newPositionBottomLeft + newPositionBottomRight) / 3;
							else if (distance01)
								newPosition = (newPositionTopLeft * tx_ + newPositionTopRight * tx);
							else if (distance02)
								newPosition = (newPositionTopLeft * ty_ + newPositionBottomLeft * ty);
							else if (distance03)
								newPosition = (newPositionTopLeft + newPositionBottomRight) / 2;*/
							else
								newPosition = newPositionTopLeft;
						}
						else if (roundedX == left && roundedY == bottom)
						{
							// if bottom left (2) is the dominant position
							// 0 1
							// 2 3

							const bool distance20 = newPositionBottomLeft.sqrDistance(newPositionTopLeft) < 2 * 2;
							const bool distance21 = newPositionBottomLeft.sqrDistance(newPositionTopRight) < 2 * 2;
							const bool distance23 = newPositionBottomLeft.sqrDistance(newPositionBottomRight) < 2 * 2;

							if (distance20 && distance21 && distance23)
								newPosition = (newPositionTopLeft * tx_ + newPositionTopRight * tx) * ty_ + (newPositionBottomLeft * tx_ + newPositionBottomRight * tx) * ty;
							/*else if (distance20 && distance21)
								newPosition = (newPositionBottomLeft + newPositionTopLeft + newPositionTopRight) / 3;
							else if (distance20 && distance23)
								newPosition = (newPositionBottomLeft + newPositionTopLeft + newPositionBottomRight) / 3;
							else if (distance21 && distance23)
								newPosition = (newPositionBottomLeft + newPositionTopRight + newPositionBottomRight) / 3;
							else if (distance20)
								newPosition = (newPositionBottomLeft * ty + newPositionTopLeft * ty_);
							else if (distance21)
								newPosition = (newPositionBottomLeft + newPositionTopRight) / 2;
							else if (distance23)
								newPosition = (newPositionBottomLeft * tx_ + newPositionBottomRight * tx);*/
							else
								newPosition = newPositionBottomLeft;
						}
						else if (roundedX == right && roundedY == top)
						{
							// if top right (1) is the dominant position
							// 0 1
							// 2 3

							const bool distance10 = newPositionTopRight.sqrDistance(newPositionTopLeft) < 2 * 2;
							const bool distance12 = newPositionTopRight.sqrDistance(newPositionBottomLeft) < 2 * 2;
							const bool distance13 = newPositionTopRight.sqrDistance(newPositionBottomRight) < 2 * 2;

							if (distance10 && distance12 && distance13)
								newPosition = (newPositionTopLeft * tx_ + newPositionTopRight * tx) * ty_ + (newPositionBottomLeft * tx_ + newPositionBottomRight * tx) * ty;
							/*else if (distance10 && distance12)
								newPosition = (newPositionTopRight + newPositionTopLeft + newPositionBottomLeft) / 3;
							else if (distance10 && distance13)
								newPosition = (newPositionTopRight + newPositionTopLeft + newPositionBottomRight) / 3;
							else if (distance12 && distance13)
								newPosition = (newPositionTopRight + newPositionBottomLeft + newPositionBottomRight) / 3;
							else if (distance10)
								newPosition = newPositionTopLeft * tx_ + newPositionTopRight * tx;
							else if (distance12)
								newPosition = (newPositionTopRight + newPositionBottomLeft) / 2;
							else if (distance13)
								newPosition = newPositionTopRight * ty_ + newPositionBottomRight * ty;*/
							else
								newPosition = newPositionTopRight;
						}
						else
						{
							// if top right (3) is the dominant position
							// 0 1
							// 2 3

							ocean_assert(roundedX == right && roundedY == bottom);

							const bool distance30 = newPositionBottomRight.sqrDistance(newPositionTopLeft) < 2 * 2;
							const bool distance31 = newPositionBottomRight.sqrDistance(newPositionTopRight) < 2 * 2;
							const bool distance32 = newPositionBottomRight.sqrDistance(newPositionBottomLeft) < 2 * 2;

							if (distance30 && distance31 && distance32)
								newPosition = (newPositionTopLeft * tx_ + newPositionTopRight * tx) * ty_ + (newPositionBottomLeft * tx_ + newPositionBottomRight * tx) * ty;
							/*else if (distance30 && distance31)
								newPosition = (newPositionBottomRight + newPositionTopLeft + newPositionTopRight) / 3;
							else if (distance30 && distance32)
								newPosition = (newPositionBottomRight + newPositionTopLeft + newPositionBottomLeft) / 3;
							else if (distance31 && distance32)
								newPosition = (newPositionBottomRight + newPositionTopRight + newPositionBottomLeft) / 3;
							else if (distance30)
								newPosition = (newPositionBottomRight + newPositionTopLeft) / 2;
							else if (distance31)
								newPosition = newPositionBottomRight * ty + newPositionTopRight * ty_;
							else if (distance32)
								newPosition = newPositionBottomRight * tx + newPositionBottomLeft * tx_;*/
							else
								newPosition = newPositionBottomRight;
						}

						if (newPosition.x() >= 2 && newPosition.x() < Scalar(layerWidth - 3) && newPosition.y() >= 2 && newPosition.y() < Scalar(layerHeight - 3))
						{
							if (maskData[Numeric::round32(newPosition.y()) * maskStrideElements + Numeric::round32(newPosition.x())] == 0xFF)
							{
								mapping[y * layerWidth + x] = newPosition;
								continue;
							}
						}

						mapping[y * layerWidth + x] = Vector2(4, 4);
						continue;
					}
				}

				while (true)
				{
					Scalar px = Random::scalar(generator, Scalar(2u), Scalar(layerWidth - 4u));
					Scalar py = Random::scalar(generator, Scalar(2u), Scalar(layerHeight - 4u));

					const int roundedX = Numeric::round32(px);
					const int roundedY = Numeric::round32(py);

					if (maskData[roundedY * maskStrideElements + roundedX] == 0xFF)
					{
						mapping[y * layerWidth + x] = Vector2(px, py);
						break;
					}
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = 0u; x < layerWidth; ++x)
		{
			if (layerF_.mask().constpixel<uint8_t>(x, y)[0] != 0xFFu)
			{
				const Vector2& position = layerF_.mapping().position(x, y);

				ocean_assert(position.x() >= 0 && position.x() < Scalar(layerWidth));
				ocean_assert(position.y() >= 0 && position.y() < Scalar(layerHeight));
			}
		}
	}
#endif // OCEAN_DEBUG
}

// **TODO** verify
void InitializerHomographyMappingAdaptionF1::initializeSubsetI1(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(sourceMappingI1_);

	const uint8_t* const maskData = layerF_.mask().constdata<uint8_t>();
	const SquareMatrix3 invertedHomography(homography_.inverted());

	const unsigned int layerWidth = layerF_.width();
	const unsigned int layerHeight = layerF_.height();

	RandomGenerator generator(randomGenerator_);

	ocean_assert_and_suppress_unused(firstColumn + numberColumns <= layerWidth, numberColumns);
	ocean_assert(firstRow + numberRows <= layerHeight);

	const unsigned int maskStrideElements = layerF_.mask().strideElements();

	const PixelPosition* sourceMapping = (*sourceMappingI1_)();
	Vector2* mapping = layerF_.mapping()();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		for (unsigned int x = firstColumn; x < firstColumn + numberRows; ++x)
		{
			if (maskData[y * maskStrideElements + x] != 0xFF)
			{
				const Vector2 previousMaskPosition(homography_ * Vector2(Scalar(x), Scalar(y)));

				const int mainX = Numeric::round32(previousMaskPosition.x());
				const int mainY = Numeric::round32(previousMaskPosition.y());

				const PixelPosition& positionMain = sourceMapping[mainY * layerWidth + mainX];

				if (positionMain)
				{
					const int left = int(previousMaskPosition.x());
					const int top = int(previousMaskPosition.y());

					const int right = left + 1;
					const int bottom = top + 1;

					// **TODO** use previous inpainting mask to determine whether a mapping is valid

					// 0 1
					// 2 3
					const PixelPosition& inPosition0 = sourceMapping[top * layerWidth + left];
					const PixelPosition& inPosition1 = sourceMapping[top * layerWidth + right];
					const PixelPosition& inPosition2 = sourceMapping[bottom * layerWidth + left];
					const PixelPosition& inPosition3 = sourceMapping[bottom * layerWidth + right];

					const Scalar tx = previousMaskPosition.x() - Scalar(left);
					const Scalar ty = previousMaskPosition.y() - Scalar(top);
					const Scalar tx_ = Scalar(1) - tx;
					const Scalar ty_ = Scalar(1) - ty;

					ocean_assert(tx >= 0 && tx <= 1);
					ocean_assert(ty >= 0 && ty <= 1);

					ocean_assert(tx_ >= 0 && tx_ <= 1);
					ocean_assert(ty_ >= 0 && ty_ <= 1);

					const Vector2 outPosition0(invertedHomography * Vector2(Scalar(inPosition0.x()), Scalar(inPosition0.y())));
					const Vector2 outPosition1(invertedHomography * Vector2(Scalar(inPosition1.x()), Scalar(inPosition1.y())));
					const Vector2 outPosition2(invertedHomography * Vector2(Scalar(inPosition2.x()), Scalar(inPosition2.y())));
					const Vector2 outPosition3(invertedHomography * Vector2(Scalar(inPosition3.x()), Scalar(inPosition3.y())));

					Vector2 newPosition(-1, -1);

					if (mainX == left && mainY == top)
					{
						// position 0 is main position

						// 0 1
						// 2 3
						if (inPosition0.east() == inPosition1 && inPosition0.south() == inPosition2 && inPosition0.southEast() == inPosition3)
						{
							// 0 1
							// 2 3

							newPosition = (outPosition0 * tx_ + outPosition1 * tx) * ty_ + (outPosition2 * tx_ + outPosition3 * tx) * ty;
						}
						else if (inPosition0.east() == inPosition1 && inPosition0.south() == inPosition2)
						{
							// 0 1
							// 2 X

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition0, outPosition1, outPosition2);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition0.east() == inPosition1 && inPosition0.southEast() == inPosition3)
						{
							// 0 1
							// X 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition0, outPosition1, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition0.south() == inPosition2 && inPosition0.southEast() == inPosition3)
						{
							// 0 X
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition0, outPosition2, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition0.east() == inPosition1)
						{
							// 0 1
							// X X

							const Vector2 outDirection(outPosition1 - outPosition0);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, 0).cross(Vector2(0, 1)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(top)) + Vector2(1, 0) * tx + Vector2(0, 1) * ty == previousMaskPosition);
							newPosition = outPosition0 + outDirection * tx + outPerpendicular * ty;
						}
						else if (inPosition0.south() == inPosition2)
						{
							// 0 X
							// 2 X

							const Vector2 outDirection(outPosition2 - outPosition0);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(0, 1).cross(Vector2(1, 0)) < 0);

							ocean_assert(Vector2(Scalar(left), Scalar(top)) + Vector2(0, 1) * ty - Vector2(-1, 0) * tx == previousMaskPosition);
							newPosition = outPosition0 + outDirection * ty - outPerpendicular * tx;
						}
						else if (inPosition0.southEast() == inPosition3)
						{
							// 0 X
							// X 3

							const Vector2 inDirection(Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(-Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(left), Scalar(top)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(outPosition3 - outPosition0);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, 1).cross(Vector2(-1, 1)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(top)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition0 + outDirection * td + outPerpendicular * tp;
						}
						else
						{
							// 0 X
							// X X

							const Vector2 inDirection(Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(-Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(left), Scalar(top)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							ocean_assert(Vector2(Scalar(left), Scalar(top)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);

							const Vector2 outDirection(invertedHomography * Vector2(Scalar(inPosition0.x() + 1u), Scalar(inPosition0.y() + 1u)) - outPosition0);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, 1).cross(Vector2(-1, 1)) > 0);

							newPosition = outPosition0 + outDirection * td + outPerpendicular * tp;
						}
					}
					else if (mainX == right && mainY == top)
					{
						// position 1 is main position

						// 0 1
						// 2 3

						if (inPosition1.west() == inPosition0 && inPosition1.southWest() == inPosition2 && inPosition1.south() == inPosition3)
						{
							// 0 1
							// 2 3

							newPosition = (outPosition0 * tx_ + outPosition1 * tx) * ty_ + (outPosition2 * tx_ + outPosition3 * tx) * ty;
						}
						else if (inPosition1.west() == inPosition0 && inPosition1.southWest() == inPosition2)
						{
							// 0 1
							// 2 X

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition1, outPosition0, outPosition2);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition1.west() == inPosition0 && inPosition1.south() == inPosition3)
						{
							// 0 1
							// X 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition1, outPosition0, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition1.southWest() == inPosition2 && inPosition1.south() == inPosition3)
						{
							// X 1
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition1, outPosition2, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition1.west() == inPosition0)
						{
							// 0 1
							// X X

							const Vector2 outDirection(outPosition1 - outPosition0);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, 0).cross(Vector2(0, 1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(top)) - Vector2(1, 0) * tx_ + Vector2(0, 1) * ty == previousMaskPosition);
							newPosition = outPosition1 - outDirection * tx_ + outPerpendicular * ty;
						}
						else if (inPosition1.south() == inPosition3)
						{
							// X 1
							// X 3

							const Vector2 outDirection(outPosition3 - outPosition1);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(0, 1).cross(Vector2(1, 0)) < 0);

							ocean_assert(Vector2(Scalar(right), Scalar(top)) + Vector2(0, 1) * ty + Vector2(-1, 0) * tx_ == previousMaskPosition);
							newPosition = outPosition1 + outDirection * ty + outPerpendicular * tx_;
						}
						else if (inPosition1.southWest() == inPosition2)
						{
							// X 1
							// 2 X

							const Vector2 inDirection(-Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(-Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(right), Scalar(top)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(outPosition2 - outPosition1);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(-1, 1).cross(Vector2(-1, -1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(top)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition1 + outDirection * td + outPerpendicular * tp;
						}
						else
						{
							// X 1
							// X X

							const Vector2 inDirection(-Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(-Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(right), Scalar(top)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(invertedHomography * Vector2(Scalar(inPosition1.x() - 1u), Scalar(inPosition1.y() + 1u)) - outPosition1);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(-1, 1).cross(Vector2(-1, -1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(top)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition1 + outDirection * td + outPerpendicular * tp;
						}
					}
					else if (mainX == left && mainY == bottom)
					{
						// position 2 is main position

						// 0 1
						// 2 3

						if (inPosition2.north() == inPosition0 && inPosition2.northEast() == inPosition1 && inPosition2.east() == inPosition3)
						{
							// 0 1
							// 2 3

							newPosition = (outPosition0 * tx_ + outPosition1 * tx) * ty_ + (outPosition2 * tx_ + outPosition3 * tx) * ty;
						}
						else if (inPosition2.north() == inPosition0 && inPosition2.northEast() == inPosition1)
						{
							// 0 1
							// 2 X

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(bottom)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(top)));
							const Triangle2 outTriangle(outPosition2, outPosition0, outPosition1);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition2.north() == inPosition0 && inPosition2.east() == inPosition3)
						{
							// 0 X
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(bottom)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition2, outPosition0, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition2.northEast() == inPosition1 && inPosition2.east() == inPosition3)
						{
							// X 1
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(left), Scalar(bottom)), Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(right), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition2, outPosition1, outPosition3);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition2.north() == inPosition0)
						{
							// 0 X
							// 2 X

							const Vector2 outDirection(outPosition0 - outPosition2);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(0, -1).cross(Vector2(1, 0)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(bottom)) + Vector2(0, -1) * ty_ + Vector2(1, 0) * tx == previousMaskPosition);
							newPosition = outPosition2 + outDirection * ty_ + outPerpendicular * tx;
						}
						else if (inPosition2.east() == inPosition3)
						{
							// X X
							// 2 3

							const Vector2 outDirection(outPosition3 - outPosition2);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, 0).cross(Vector2(0, 1)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(bottom)) + Vector2(1, 0) * tx - Vector2(0, 1) * ty_ == previousMaskPosition);
							newPosition = outPosition2 + outDirection * tx - outPerpendicular * ty_;
						}
						else if (inPosition2.northEast() == inPosition1)
						{
							// X 1
							// 2 X

							const Vector2 inDirection(Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(left), Scalar(bottom)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(outPosition1 - outPosition2);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, -1).cross(Vector2(1, 1)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(bottom)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition2 + outDirection * td + outPerpendicular * tp;
						}
						else
						{
							// X X
							// 2 X

							const Vector2 inDirection(Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(Numeric::sqrt(2) * Scalar(0.5), Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(left), Scalar(bottom)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(invertedHomography * Vector2(Scalar(inPosition2.x() + 1u), Scalar(inPosition2.y() - 1u)) - outPosition2);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(1, -1).cross(Vector2(1, 1)) > 0);

							ocean_assert(Vector2(Scalar(left), Scalar(bottom)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition2 + outDirection * td + outPerpendicular * tp;
						}
					}
					else
					{
						ocean_assert(mainX == right && mainY == bottom);

						// position 3 is main position

						// 0 1
						// 2 3

						if (inPosition3.northWest() == inPosition0 && inPosition3.north() == inPosition1 && inPosition3.west() == inPosition2)
						{
							newPosition = (outPosition0 * tx_ + outPosition1 * tx) * ty_ + (outPosition2 * tx_ + outPosition3 * tx) * ty;
						}
						else if (inPosition3.northWest() == inPosition0 && inPosition3.north() == inPosition1)
						{
							// 0 1
							// X 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(bottom)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(right), Scalar(top)));
							const Triangle2 outTriangle(outPosition3, outPosition0, outPosition1);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition3.northWest() == inPosition0 && inPosition3.west() == inPosition2)
						{
							// 0 X
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(bottom)), Vector2(Scalar(left), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition3, outPosition0, outPosition2);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition3.north() == inPosition1 && inPosition3.west() == inPosition2)
						{
							// X 1
							// 2 3

							const Triangle2 inTriangle = Triangle2(Vector2(Scalar(right), Scalar(bottom)), Vector2(Scalar(right), Scalar(top)), Vector2(Scalar(left), Scalar(bottom)));
							const Triangle2 outTriangle(outPosition3, outPosition1, outPosition2);
							newPosition = outTriangle.barycentric2cartesian(inTriangle.cartesian2barycentric(previousMaskPosition));
						}
						else if (inPosition3.north() == inPosition1)
						{
							// X 1
							// X 3

							const Vector2 outDirection(outPosition1 - outPosition3);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(0, -1).cross(Vector2(1, 0)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(bottom)) + Vector2(0, -1) * ty_ - Vector2(1, 0) * tx_ == previousMaskPosition);
							newPosition = outPosition3 + outDirection * ty_ - outPerpendicular * tx_;
						}
						else if (inPosition3.west() == inPosition2)
						{
							// X X
							// 2 3

							const Vector2 outDirection(outPosition2 - outPosition3);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(-1, 0).cross(Vector2(0, -1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(bottom)) + Vector2(-1, 0) * tx_ + Vector2(0, -1) * ty_ == previousMaskPosition);
							newPosition = outPosition3 + outDirection * tx_ + outPerpendicular * ty_;
						}
						else if (inPosition3.northWest() == inPosition0)
						{
							// 0 X
							// X 3

							const Vector2 inDirection(-Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(right), Scalar(bottom)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(outPosition0 - outPosition3);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(-1, -1).cross(Vector2(1, -1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(bottom)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition3 + outDirection * td + outPerpendicular * tp;
						}
						else
						{
							// X X
							// X 3

							const Vector2 inDirection(-Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							const Vector2 inPerpendicular(Numeric::sqrt(2) * Scalar(0.5), -Numeric::sqrt(2) * Scalar(0.5));
							ocean_assert(inPerpendicular == inDirection.perpendicular());
							ocean_assert(inDirection.cross(inPerpendicular) > 0);

							const Vector2 inPosition(previousMaskPosition - Vector2(Scalar(right), Scalar(bottom)));
							const Scalar td = inDirection * inPosition;
							const Scalar tp = inPerpendicular * inPosition;

							const Vector2 outDirection(invertedHomography * Vector2(Scalar(inPosition3.x() - 1u), Scalar(inPosition3.y() - 1u)) - outPosition3);
							const Vector2 outPerpendicular(outDirection.perpendicular());
							ocean_assert(Vector2(-1, -1).cross(Vector2(1, -1)) > 0);

							ocean_assert(Vector2(Scalar(right), Scalar(bottom)) + inDirection * td + inPerpendicular * tp == previousMaskPosition);
							newPosition = outPosition3 + outDirection * td + outPerpendicular * tp;
						}
					}

					if (newPosition.x() >= 2 && newPosition.x() < Scalar(layerWidth - 3) && newPosition.y() >= 2 && newPosition.y() < Scalar(layerHeight - 3))
					{
						if (maskData[Numeric::round32(newPosition.y()) * maskStrideElements + Numeric::round32(newPosition.x())] == 0xFF)
						{
							mapping[y * layerWidth + x] = newPosition;
							continue;
						}
					}

					mapping[y * layerWidth + x] = Vector2(4, 4);
					continue;
				}

				while (true)
				{
					Scalar px = Random::scalar(generator, Scalar(2u), Scalar(layerWidth - 4u));
					Scalar py = Random::scalar(generator, Scalar(2u), Scalar(layerHeight - 4u));

					const int roundedX = Numeric::round32(px);
					const int roundedY = Numeric::round32(py);

					if (maskData[roundedY * maskStrideElements + roundedX] == 0xFF)
					{
						mapping[y * layerWidth + x] = Vector2(px, py);
						break;
					}
				}
			}
		}
	}

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0u; y < layerHeight; ++y)
	{
		for (unsigned int x = 0u; x < layerWidth; ++x)
		{
			if (layerF_.mask().constpixel<uint8_t>(x, y)[0] != 0xFFu)
			{
				const Vector2& position = layerF_.mapping().position(x, y);

				ocean_assert(position.x() >= 0 && position.x() < Scalar(layerWidth));
				ocean_assert(position.y() >= 0 && position.y() < Scalar(layerHeight));
			}
		}
	}
#endif // OCEAN_DEBUG
}

}

}

}
