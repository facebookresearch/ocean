/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/FASTFeatureDetector.h"

#include "ocean/cv/FrameConverter.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

#define HIGHER00 (PIXEL00 > centerHigh)
#define HIGHER01 (PIXEL01 > centerHigh)
#define HIGHER02 (PIXEL02 > centerHigh)
#define HIGHER03 (PIXEL03 > centerHigh)
#define HIGHER04 (PIXEL04 > centerHigh)
#define HIGHER05 (PIXEL05 > centerHigh)
#define HIGHER06 (PIXEL06 > centerHigh)
#define HIGHER07 (PIXEL07 > centerHigh)
#define HIGHER08 (PIXEL08 > centerHigh)
#define HIGHER09 (PIXEL09 > centerHigh)
#define HIGHER10 (PIXEL10 > centerHigh)
#define HIGHER11 (PIXEL11 > centerHigh)
#define HIGHER12 (PIXEL12 > centerHigh)
#define HIGHER13 (PIXEL13 > centerHigh)
#define HIGHER14 (PIXEL14 > centerHigh)
#define HIGHER15 (PIXEL15 > centerHigh)

#define LOWER00 (PIXEL00 < centerLow)
#define LOWER01 (PIXEL01 < centerLow)
#define LOWER02 (PIXEL02 < centerLow)
#define LOWER03 (PIXEL03 < centerLow)
#define LOWER04 (PIXEL04 < centerLow)
#define LOWER05 (PIXEL05 < centerLow)
#define LOWER06 (PIXEL06 < centerLow)
#define LOWER07 (PIXEL07 < centerLow)
#define LOWER08 (PIXEL08 < centerLow)
#define LOWER09 (PIXEL09 < centerLow)
#define LOWER10 (PIXEL10 < centerLow)
#define LOWER11 (PIXEL11 < centerLow)
#define LOWER12 (PIXEL12 < centerLow)
#define LOWER13 (PIXEL13 < centerLow)
#define LOWER14 (PIXEL14 < centerLow)
#define LOWER15 (PIXEL15 < centerLow)

void FASTFeatureDetector::detectFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker)
{
	// minimal width/height is 7 pixel for the response + 2 for non-maximum suppression == 9

	if (yFrame == nullptr || subRegionWidth < 9u || subRegionHeight < 9u || subRegionLeft + subRegionWidth > width || subRegionTop + subRegionHeight > height)
	{
		ocean_assert(false && "Invalid input!");
		return;
	}

	const FASTFeature::DistortionState distortionState = frameIsUndistorted ? FASTFeature::DS_UNDISTORTED : FASTFeature::DS_DISTORTED;

	NonMaximumSuppressionVote nonMaximumSuppression(width, height);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FASTFeatureDetector::detectFeatureCandidatesSubset, yFrame, width, height, threshold, &nonMaximumSuppression, subRegionLeft, subRegionWidth, framePaddingElements, 0u, 0u), subRegionTop, subRegionHeight, 8u, 9u, 20u);
	}
	else
	{
		detectFeatureCandidatesSubset(yFrame, width, height, threshold, &nonMaximumSuppression, subRegionLeft, subRegionWidth, framePaddingElements, subRegionTop, subRegionHeight);
	}

	const NonMaximumSuppressionVote::StrengthPositions<uint32_t, int32_t> strengthPositions(nonMaximumSuppression.suppressNonMaximum<uint32_t, int32_t>(subRegionLeft + 4u, subRegionWidth - 8u, subRegionTop + 4u, subRegionHeight - 8u, worker));

	features.reserve(strengthPositions.size());
	features.clear();

	for (NonMaximumSuppressionVote::StrengthPositions<uint32_t, int32_t>::const_iterator i = strengthPositions.begin(); i != strengthPositions.end(); ++i)
	{
		features.emplace_back(Vector2(Scalar(i->x()), Scalar(i->y())), distortionState, Scalar(i->strength()));
	}

	if (preciseScoring)
	{
		scoreFeaturesPrecise(yFrame, width, height, threshold, features, framePaddingElements, worker);
	}
}

bool FASTFeatureDetector::Comfort::detectFeatures(const Frame& frame, const unsigned int subRegionLeft, const unsigned int subRegionTop, const unsigned int subRegionWidth, const unsigned int subRegionHeight, const unsigned int threshold, const bool frameIsUndistorted, const bool preciseScoring, FASTFeatures& features, Worker* worker)
{
	ocean_assert(frame.isValid());

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		return false;
	}

	FASTFeatureDetector::detectFeatures(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), subRegionLeft, subRegionTop, subRegionWidth, subRegionHeight, threshold, frameIsUndistorted, preciseScoring, features, yFrame.paddingElements(), worker);

	return true;
}

void FASTFeatureDetector::detectFeatureCandidatesSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, NonMaximumSuppressionVote* nonMaximumSuppression, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int framePaddingElements, const unsigned int firstRow, const unsigned int numberRows)
{
	/**
	 * Pixel Positions for FAST features
	 *
	 *      00 01 02 03 04 05 06
	 *     ----------------------
	 * 00 |       15 00 01
	 * 01 |    14          02
	 * 02 | 13                03
	 * 03 | 12       XX       04
	 * 04 | 11                05
	 * 05 |    10          06
	 * 06 |       09 08 07
	 */

	const int frameStrideElements = int(width + framePaddingElements);

	#define PIXEL00 (*(topMiddle))
	#define PIXEL01 (*(topMiddle + 1))
	#define PIXEL02 (*(topMiddle + frameStrideElements + 2))
	#define PIXEL03 (*(middleLeft - frameStrideElements + 6))
	#define PIXEL04 (*(middleLeft + 6))
	#define PIXEL05 (*(middleLeft + frameStrideElements + 6))
	#define PIXEL06 (*(bottomMiddle - frameStrideElements + 2))
	#define PIXEL07 (*(bottomMiddle + 1))
	#define PIXEL08 (*(bottomMiddle))
	#define PIXEL09 (*(bottomMiddle - 1))
	#define PIXEL10 (*(bottomMiddle - frameStrideElements - 2))
	#define PIXEL11 (*(middleLeft + frameStrideElements))
	#define PIXEL12 (*(middleLeft))
	#define PIXEL13 (*(middleLeft - frameStrideElements))
	#define PIXEL14 (*(topMiddle + frameStrideElements - 2))
	#define PIXEL15 (*(topMiddle - 1))
	#define CENTER  (*(middleLeft + 3))

	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 7u && height >= 7u);
	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(firstRow + numberRows <= height);

	const unsigned int beginResponseColumn = max(0, int(firstColumn) - 3) + 3;
	const unsigned int endResponseColumn = min(firstColumn + numberColumns + 3, width) - 3; // exclusive end position
	ocean_assert(endResponseColumn > beginResponseColumn);

	const unsigned int beginResponseRow = max(0, int(firstRow) - 3) + 3;
	const unsigned int endResponseRow = min(firstRow + numberRows + 3, height) - 3; // exclusive end position
	ocean_assert(endResponseRow > beginResponseRow);

	const uint8_t* topMiddle = yFrame + (unsigned int)(frameStrideElements) * (beginResponseRow - 3u) + beginResponseColumn - 1u;
	const uint8_t* middleLeft = yFrame + (unsigned int)(frameStrideElements) * beginResponseRow + beginResponseColumn - 4u;
	const uint8_t* bottomMiddle = yFrame + (unsigned int)(frameStrideElements) * (beginResponseRow + 3u) + beginResponseColumn - 1u;

	ocean_assert(topMiddle >= yFrame && topMiddle <= yFrame + (unsigned int)(frameStrideElements) * height);
	ocean_assert(middleLeft >= yFrame && middleLeft <= yFrame + (unsigned int)(frameStrideElements) * height);
	ocean_assert(bottomMiddle >= yFrame && bottomMiddle <= yFrame + (unsigned int)(frameStrideElements) * height);

	// line end offset: number of pixels the mask must be shifted to the first sampling position in the next sampling line
	const unsigned int lineEndOffset = width - endResponseColumn + beginResponseColumn + framePaddingElements;

	const uint8_t* const topMiddleEnd = topMiddle + (unsigned int)(frameStrideElements) * (endResponseRow - beginResponseRow);

	unsigned int y = beginResponseRow - 1u;

	const unsigned int correctionValue = 16u * 255u * threshold;

	while (topMiddle != topMiddleEnd)
	{
		ocean_assert(topMiddle <= topMiddleEnd);
		ocean_assert(((topMiddle - yFrame) % (unsigned int)(frameStrideElements)) + 1u == beginResponseColumn);

		++y;
		unsigned int x = beginResponseColumn - 1u;

		const uint8_t* const topMiddleRowEnd = topMiddle + endResponseColumn - beginResponseColumn;

		while (topMiddle != topMiddleRowEnd)
		{
			ocean_assert(topMiddle <= topMiddleRowEnd);

			++topMiddle;
			++middleLeft;
			++bottomMiddle;

			++x;

			const int centerHigh = int(CENTER) + int(threshold);
			const int centerLow = int(CENTER) - int(threshold);

			if (HIGHER00)
			{
				// valid   : top
				// invalid : -

				if (HIGHER04)
				{
					// valid   : top, right
					// invalid : -

					if (HIGHER01 && HIGHER02 && HIGHER03)
					{
						// valid   : quarter0 (00 - 04)
						// invalid : -

						if (HIGHER08)
						{
							// valid   : quarter0, bottom
							// invalid : -

							if (HIGHER05 && HIGHER06 && HIGHER07)
							{
								// valid   : quarter0, quarter1 (00 - 08)
								// invalid : -

								if (HIGHER09)
								{
									// valid   : quarter0, quarter1, 09 (00 - 09)
									// invalid : -

									if (HIGHER10)
									{
										// valid   : quarter0, quarter1, 09, 10 (00 - 10)
										// invalid : -

										if (HIGHER11)
										{
											// valid   : quarter0, quarter1, 09, 10, 11 (00 - 11)
											// invalid : -

											// VALID FEATURE: 00 - 11

											nonMaximumSuppression->addCandidate(x, y, PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11
															+ PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09, 10 (00 - 10)
											// invalid : 11

											if (HIGHER15)
											{
												// VALID FEATURE: 15 - 10

												nonMaximumSuppression->addCandidate(x, y, PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10
															+ PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter1, 09, 10 (00 - 10)
												// invalid : 11, 15

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter1, 09 (00 - 09)
										// invalid : 10

										if (HIGHER15 && HIGHER14)
										{
											// valid   : quarter0, quarter1, 09, 14, 15 (14 - 09)
											// invalid : 10

											// VALID FEATURE: 14 - 09

											nonMaximumSuppression->addCandidate(x, y, PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09
															+ PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09 (00 - 09)
											// invalid : 10 & (14 | 15)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter1 (00 - 08)
									// invalid : 09

									if (HIGHER13 && HIGHER14 && HIGHER15)
									{
										// valid   : quarter0, quarter1, quarter3 (13 - 08)
										// invalid : 09

										//  VALID FEATURE: 13 - 08

										nonMaximumSuppression->addCandidate(x, y, PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08
														+ PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 - 16 * CENTER + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter0, quarter1 (00 - 08)
										// invalid : 09 & (13 | 14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, bottom (00 - 04) & 08
								// invalid : (05 | 06 | 07)

								if (HIGHER12)
								{
									// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
									// invalid : (05 | 06 | 07)

									if (HIGHER13 && HIGHER14 && HIGHER15)
									{
										// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
										// invalid : (05 | 06 | 07)

										if (HIGHER05)
										{
											// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
											// invalid : (06 | 07)

											if (HIGHER06)
											{
												// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
												// invalid : 07

												if (HIGHER11)
												{
													// valid   : quarter0, quarter3, bottom, (11 - 06) & 08
													// invalid : 07

													// VALID FEATURE: 11 - 06

													nonMaximumSuppression->addCandidate(x, y, PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06
														+ PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 - 16 * CENTER + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
													// invalid : 07 & 11

													// nothing to do here
												}
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
												// invalid : 06

												if (HIGHER11 && HIGHER10)
												{
													// valid   : quarter0, quarter3, bottom, (10 - 05) & 08
													// invalid : 06

													// VALID FEATURE: 10 - 05

													nonMaximumSuppression->addCandidate(x, y, PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05
														 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 - 16 * CENTER + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
													// invalid : 06 & (10 | 11)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
											// invalid : 05

											if (HIGHER09 && HIGHER10 && HIGHER11)
											{
												// valid   : quarter0, quarter2, quarter3, (08 - 04)
												// invalid : 05

												// VALID FEATURE 08 - 04

												nonMaximumSuppression->addCandidate(x, y, PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04
														 + PIXEL05 + PIXEL06 + PIXEL07 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
												// invalid : 05 & (09 | 10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
										// invalid : (05 | 06 | 07) & (13 | 14 | 15)

										// nothing to do here
									}
								}
								else
								{
									// valid   : quarter0, bottom (00 - 04) & 08
									// invalid : (05 | 06 | 07) & 12

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter0 (00 - 04)
							// invalid : 08

							if (HIGHER12)
							{
								// valid   : quarter0, left (00 - 04) & 12
								// invalid : 08

								if (HIGHER13 && HIGHER14 && HIGHER15)
								{
									// valid   : quarter0, quarter3, (12 - 04)
									// invalid : 08

									if (HIGHER05)
									{
										// valid   : quarter0, quarter3, (12 - 04) & 05
										// invalid : 08

										if (HIGHER06)
										{
											// valid   : quarter0, quarter3, (12 - 06)
											// invalid : 08

											if (HIGHER07)
											{
												// valid   : quarter0, quarter3, (12 - 07)
												// invalid : 08

												// VALID FEATURE: 12 - 07

												nonMaximumSuppression->addCandidate(x, y, PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07
														 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 06)
												// invalid : 08 & 07

												if (HIGHER11)
												{
													// valid   : quarter0, quarter3, (11 - 06)
													// invalid : 08 & 07

													// VALID FEATURE: 11 - 06

													nonMaximumSuppression->addCandidate(x, y, PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06
														 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 - 16 * CENTER + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, (12 - 06)
													// invalid : 08 & 07 && 11

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 05)
											// invalid : 08 & 06

											if (HIGHER10 && HIGHER11)
											{
												// valid   : quarter0, quarter3, (10 - 05)
												// invalid : 08 & 06

												// VALID FEATURE: 10 - 05

												nonMaximumSuppression->addCandidate(x, y, PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05
														 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 05)
												// invalid : 08 & 06 & (10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 04)
										// invalid : 08 & 05

										if (HIGHER09 && HIGHER10 && HIGHER11)
										{
											// valid   : quarter0, quarter3, (09 - 04)
											// invalid : 08 & 05

											// VALID FEATURE: 09 - 04

											nonMaximumSuppression->addCandidate(x, y, PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04
														 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 04)
											// invalid : 08 & 05 & (09 | 10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, left (00 - 04) & 12
									// invalid : 08 & (13 | 14 | 15)

									// nothing to do here
								}
							}
							else
							{
								// valid   : quarter0 (00 - 04)
								// invalid : 08 & 12

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : top, right
						// invalid : (01 | 02 | 03)

						if (HIGHER12)
						{
							// valid   : top, right, left
							// invalid : (01 | 02 | 03)

							if (HIGHER08)
							{
								// valid   : top, right, left, bottom
								// invalid : (01 | 02 | 03)

								if (HIGHER09 && HIGHER10 && HIGHER11)
								{
									// valid   : top, right, quarter2
									// invalid : (01 | 02 | 03)

									if (HIGHER13 && HIGHER14 && HIGHER15)
									{
										// valid   : quarter2, quarter3, right
										// invalid : (01 | 02 | 03)

										if (HIGHER01)
										{
											// valid   : quarter2, quarter3, right & 01
											// invalid : (02 | 03)

											if (HIGHER02)
											{
												// valid   : quarter2, quarter3, right & 01 & 02
												// invalid : 03

												if (HIGHER07)
												{
													// valid   : quarter2, quarter3, right & 01 & 02 & 07
													// invalid : 03

													// VALID FEATURE: 07 - 02

													nonMaximumSuppression->addCandidate(x, y, PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02
																	+ PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 - 16 * CENTER + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter2, quarter3, right & 01 & 02
													// invalid : 03 & 07

													// nothing to do here
												}
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01
												// invalid : 02

												if (HIGHER06 && HIGHER07)
												{
													// valid   : quarter2, quarter3, right & 01 & 06 & 07
													// invalid : 02

													// VALID FEATURE: 06 - 01

													nonMaximumSuppression->addCandidate(x, y, PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01
																	+ PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 - 16 * CENTER + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter2, quarter3, right & 01
													// invalid : 02 & (06 | 07)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter2, quarter3, right (08 - 00)
											// invalid : 01

											if (HIGHER05 && HIGHER06 && HIGHER07)
											{
												// valid   : quarter2, quarter3, right (05 - 00)
												// invalid : 01

												// VALID FEATURE: 05 - 00

												nonMaximumSuppression->addCandidate(x, y, PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00
																+ PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right
												// invalid : 01 & (05 | 06 | 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : top, right, quarter2 (08 - 12)
										// invalid : (01 | 02 | 03) & (13 | 14 | 15)

										if (HIGHER05 && HIGHER06 && HIGHER07)
										{
											// valid   : top, right, quarter1, quarter2 (04 - 12)
											// invalid : (01 | 02 | 03) & (13 | 14 | 15)

											if (HIGHER03)
											{
												// valid   : top, right, quarter1, quarter2 (03 - 12)
												// invalid : (01 | 02) & (13 | 14 | 15)

												if (HIGHER02)
												{
													// valid   : top, right, quarter1, quarter2 (02 - 12)
													// invalid : 01 & (13 | 14 | 15)

													if (HIGHER13)
													{
														// valid   : top, right, quarter1, quarter2 (02 - 13)
														// invalid : 01 & (14 | 15)

														// VALID FEATURE: 02 - 13

														nonMaximumSuppression->addCandidate(x, y, PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13
																+ PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 - 16 * CENTER + correctionValue);
														continue;
													}
													else
													{
														// valid   : top, right, quarter1, quarter2 (02 - 12)
														// invalid : 01 & 13

														// nothing to do here
													}
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (03 - 12)
													// invalid : 02 & (13 | 14 | 15)

													if (HIGHER13 && HIGHER14)
													{
														// valid   : top, right, quarter1, quarter2 (03 - 14)
														// invalid : 02 & 15

														// VALID FEATURE: 03 - 14

														nonMaximumSuppression->addCandidate(x, y, PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14
																+ PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 - 16 * CENTER + correctionValue);
														continue;
													}
													else
													{
														// valid   : top, right, quarter1, quarter2 (03 - 12)
														// invalid : 02 & (13 | 14)

														// nothing to do here
													}
												}
											}
											else
											{
												// valid   : top, right, quarter1, quarter2 (04 - 12)
												// invalid : 03 & (13 | 14 | 15)

												// nothing to do here
											}
										}
										else
										{
											// valid   : top, right, quarter2 (08 - 12)
											// invalid : (01 | 02 | 03) & (05 | 06 | 07) & (13 | 14 | 15)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : top, right, left, bottom
									// invalid : (01 | 02 | 03) & (09 | 10 | 11)

									// nothing to do here
								}
							}
							else
							{
								// valid   : top, right, left
								// invalid : 08 & (01 | 02 | 03)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, right
							// invalid : 12 & (01 | 02 | 03)

							// nothing to do here
						}
					}
				}
				else
				{
					// valid   : top (00)
					// invalid : right (04)

					if (HIGHER12)
					{
						// valid   : top, left (00 & 12)
						// invalid : right (04)

						if (HIGHER08)
						{
							// valid   : top, left, bottom (00 & 08 & 12)
							// invalid : right (04)

							if (HIGHER09 && HIGHER10 && HIGHER11 && HIGHER13 && HIGHER14 && HIGHER15)
							{
								// valid   : quarter2, quarter3, (08 - 00)
								// invalid : right (04)

								if (HIGHER01)
								{
									// valid   : quarter2, quarter3, (08 - 01)
									// invalid : right (04)

									if (HIGHER02)
									{
										// valid   : quarter2, quarter3, (08 - 02)
										// invalid : right (04)

										if (HIGHER03)
										{
											// valid   : quarter2, quarter3, (08 - 03)
											// invalid : right (04)

											// VALID FEATURE: 08 - 03

											nonMaximumSuppression->addCandidate(x, y, PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03
																+ PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 02)
											// invalid : right (03 & 04)

											if (HIGHER07)
											{
												// valid   : quarter2, quarter3, (07 - 02)
												// invalid : right (03 & 04)

												// VALID FEATURE: 07 - 02

												nonMaximumSuppression->addCandidate(x, y, PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02
																+ PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 - 16 * CENTER + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, (08 - 02)
												// invalid : right (03 & 04 & 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 01)
										// invalid : right (02 & 04)

										if (HIGHER06 && HIGHER07)
										{
											// valid   : quarter2, quarter3, (06 - 01)
											// invalid : right (02 & 04)

											// VALID FEATURE: 06 - 01

											nonMaximumSuppression->addCandidate(x, y, PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01
															+ PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 01)
											// invalid : right 02 & 04 & (06 | 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 00)
									// invalid : right (01 & 04)

									if (HIGHER05 && HIGHER06 && HIGHER07)
									{
										// valid   : quarter2, quarter3, (05 - 00)
										// invalid : right (01 & 04)

										// VALID FEATURE: (05 - 00)

										nonMaximumSuppression->addCandidate(x, y, PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00
														+ PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 - 16 * CENTER + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 00)
										// invalid : right 01 & 04 & (05 | 06 | 07)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : top, left, bottom (00 & 08 & 12)
								// invalid : right 04 & (09 | 10 | 11 | 13 | 14 | 15)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, left (00 & 12)
							// invalid : right, bottom (04 & 08)

						}	// nothing to do here
					}
					else
					{
						// valid   : top (00)
						// invalid : right, left (04 & 12)

						// nothing to do here
					}
				}
			}
			else
			{
				// valid   : -
				// invalid : 00

				if (HIGHER08)
				{
					// valid   : 08
					// invalid : 00

					if (HIGHER04 && HIGHER12)
					{
						// valid   : 04 & 08 & 12
						// invalid : 00

						if (HIGHER05 && HIGHER06 && HIGHER07 && HIGHER09 && HIGHER10 && HIGHER11)
						{
							// valid   : quarter1, quarter2, (04 - 12)
							// invalid : 00

							if (HIGHER03)
							{
								// valid   : quarter1, quarter2, (03 - 12)
								// invalid : 00

								if (HIGHER02)
								{
									// valid   : quarter1, quarter2, (02 - 12)
									// invalid : 00

									if (HIGHER01)
									{
										// valid   : quarter1, quarter2, (01 - 12)
										// invalid : 00

										// VALID FEATURE: 01 - 12

										nonMaximumSuppression->addCandidate(x, y, PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12
														+ PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 - 16 * CENTER + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (02 - 12)
										// invalid : 00 & 01

										if (HIGHER13)
										{
											// valid   : quarter1, quarter2, (02 - 13)
											// invalid : 00 & 01

											// VALID FEATURE: 02 - 13

											nonMaximumSuppression->addCandidate(x, y, PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13
														+ PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 - 16 * CENTER + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter1, quarter2, (02 - 12)
											// invalid : 00 & 01 & 13

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter1, quarter2, (03 - 12)
									// invalid : 00 & 02

									if (HIGHER13 && HIGHER14)
									{
										// valid   : quarter1, quarter2, (03 - 14)
										// invalid : 00 & 02

										// VALID FEATURE: 03 - 14

										nonMaximumSuppression->addCandidate(x, y, PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14
														+ PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 - 16 * CENTER + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (03 - 12)
										// invalid : 00 & 02 & (13 | 14)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter1, quarter2, (04 - 12)
								// invalid : 00 & 03

								if (HIGHER13 && HIGHER14 && HIGHER15)
								{
									// valid   : quarter1, quarter2, (04 - 15)
									// invalid : 00 & 03

									// VALID FEATURE: 04 - 15

									nonMaximumSuppression->addCandidate(x, y, PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15
														+ PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 - 16 * CENTER + correctionValue);
									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (04 - 12)
									// invalid : 00 & 03 & (13 | 14 | 15)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : 04 & 08 & 12
							// invalid : 00 & (05 | 06 | 07 | 09 | 10 | 11)

							// nothing to do here
						}
					}
					else
					{
						// valid   : 08
						// invalid : 00 & (04 | 12)

						// nothing to do here
					}
				}
				else
				{
					// valid   : -
					// invalid : 00 & 08

					// nothing to do here
				}
			}





			if (LOWER00)
			{
				// valid   : top
				// invalid : -

				if (LOWER04)
				{
					// valid   : top, right
					// invalid : -

					if (LOWER01 && LOWER02 && LOWER03)
					{
						// valid   : quarter0 (00 - 04)
						// invalid : -

						if (LOWER08)
						{
							// valid   : quarter0, bottom
							// invalid : -

							if (LOWER05 && LOWER06 && LOWER07)
							{
								// valid   : quarter0, quarter1 (00 - 08)
								// invalid : -

								if (LOWER09)
								{
									// valid   : quarter0, quarter1, 09 (00 - 09)
									// invalid : -

									if (LOWER10)
									{
										// valid   : quarter0, quarter1, 09, 10 (00 - 10)
										// invalid : -

										if (LOWER11)
										{
											// valid   : quarter0, quarter1, 09, 10, 11 (00 - 11)
											// invalid : -

											// VALID FEATURE: 00 - 11

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11
														- PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09, 10 (00 - 10)
											// invalid : 11

											if (LOWER15)
											{
												// VALID FEATURE: 15 - 10

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10
																	- PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter1, 09, 10 (00 - 10)
												// invalid : 11, 15

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter1, 09 (00 - 09)
										// invalid : 10

										if (LOWER15 && LOWER14)
										{
											// valid   : quarter0, quarter1, 09, 14, 15 (14 - 09)
											// invalid : 10

											// VALID FEATURE: 14 - 09

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09
																- PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09 (00 - 09)
											// invalid : 10 & (14 | 15)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter1 (00 - 08)
									// invalid : 09

									if (LOWER13 && LOWER14 && LOWER15)
									{
										// valid   : quarter0, quarter1, quarter3 (13 - 08)
										// invalid : 09

										//  VALID FEATURE: 13 - 08

										nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08
															- PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter0, quarter1 (00 - 08)
										// invalid : 09 & (13 | 14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, bottom (00 - 04) & 08
								// invalid : (05 | 06 | 07)

								if (LOWER12)
								{
									// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
									// invalid : (05 | 06 | 07)

									if (LOWER13 && LOWER14 && LOWER15)
									{
										// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
										// invalid : (05 | 06 | 07)

										if (LOWER05)
										{
											// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
											// invalid : (06 | 07)

											if (LOWER06)
											{
												// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
												// invalid : 07

												if (LOWER11)
												{
													// valid   : quarter0, quarter3, bottom, (11 - 06) & 08
													// invalid : 07

													// VALID FEATURE: 11 - 06

													nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06
																	- PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
													// invalid : 07 & 11

													// nothing to do here
												}
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
												// invalid : 06

												if (LOWER11 && LOWER10)
												{
													// valid   : quarter0, quarter3, bottom, (10 - 05) & 08
													// invalid : 06

													// VALID FEATURE: 10 - 05

													nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05
																	- PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
													// invalid : 06 & (10 | 11)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
											// invalid : 05

											if (LOWER09 && LOWER10 && LOWER11)
											{
												// valid   : quarter0, quarter2, quarter3, (08 - 04)
												// invalid : 05

												// VALID FEATURE 08 - 04

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04
																- PIXEL05 - PIXEL06 - PIXEL07 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
												// invalid : 05 & (09 | 10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
										// invalid : (05 | 06 | 07) & (13 | 14 | 15)

										// nothing to do here
									}
								}
								else
								{
									// valid   : quarter0, bottom (00 - 04) & 08
									// invalid : (05 | 06 | 07) & 12

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter0 (00 - 04)
							// invalid : 08

							if (LOWER12)
							{
								// valid   : quarter0, left (00 - 04) & 12
								// invalid : 08

								if (LOWER13 && LOWER14 && LOWER15)
								{
									// valid   : quarter0, quarter3, (12 - 04)
									// invalid : 08

									if (LOWER05)
									{
										// valid   : quarter0, quarter3, (12 - 04) & 05
										// invalid : 08

										if (LOWER06)
										{
											// valid   : quarter0, quarter3, (12 - 06)
											// invalid : 08

											if (LOWER07)
											{
												// valid   : quarter0, quarter3, (12 - 07)
												// invalid : 08

												// VALID FEATURE: 12 - 07

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07
																- PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 06)
												// invalid : 08 & 07

												if (LOWER11)
												{
													// valid   : quarter0, quarter3, (11 - 06)
													// invalid : 08 & 07

													// VALID FEATURE: 11 - 06

													nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06
																	- PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter0, quarter3, (12 - 06)
													// invalid : 08 & 07 && 11

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 05)
											// invalid : 08 & 06

											if (LOWER10 && LOWER11)
											{
												// valid   : quarter0, quarter3, (10 - 05)
												// invalid : 08 & 06

												// VALID FEATURE: 10 - 05

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05
																- PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 05)
												// invalid : 08 & 06 & (10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 04)
										// invalid : 08 & 05

										if (LOWER09 && LOWER10 && LOWER11)
										{
											// valid   : quarter0, quarter3, (09 - 04)
											// invalid : 08 & 05

											// VALID FEATURE: 09 - 04

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04
															- PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 04)
											// invalid : 08 & 05 & (09 | 10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, left (00 - 04) & 12
									// invalid : 08 & (13 | 14 | 15)

									// nothing to do here
								}
							}
							else
							{
								// valid   : quarter0 (00 - 04)
								// invalid : 08 & 12

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : top, right
						// invalid : (01 | 02 | 03)

						if (LOWER12)
						{
							// valid   : top, right, left
							// invalid : (01 | 02 | 03)

							if (LOWER08)
							{
								// valid   : top, right, left, bottom
								// invalid : (01 | 02 | 03)

								if (LOWER09 && LOWER10 && LOWER11)
								{
									// valid   : top, right, quarter2
									// invalid : (01 | 02 | 03)

									if (LOWER13 && LOWER14 && LOWER15)
									{
										// valid   : quarter2, quarter3, right
										// invalid : (01 | 02 | 03)

										if (LOWER01)
										{
											// valid   : quarter2, quarter3, right & 01
											// invalid : (02 | 03)

											if (LOWER02)
											{
												// valid   : quarter2, quarter3, right & 01 & 02
												// invalid : 03

												if (LOWER07)
												{
													// valid   : quarter2, quarter3, right & 01 & 02 & 07
													// invalid : 03

													// VALID FEATURE: 07 - 02

													nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02
																	- PIXEL03  - PIXEL04 - PIXEL05 - PIXEL06 + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter2, quarter3, right & 01 & 02
													// invalid : 03 & 07

													// nothing to do here
												}
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01
												// invalid : 02

												if (LOWER06 && LOWER07)
												{
													// valid   : quarter2, quarter3, right & 01 & 06 & 07
													// invalid : 02

													// VALID FEATURE: 06 - 01

													nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01
																	- PIXEL02  - PIXEL03 - PIXEL04 - PIXEL05 + correctionValue);
													continue;
												}
												else
												{
													// valid   : quarter2, quarter3, right & 01
													// invalid : 02 & (06 | 07)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : quarter2, quarter3, right (08 - 00)
											// invalid : 01

											if (LOWER05 && LOWER06 && LOWER07)
											{
												// valid   : quarter2, quarter3, right (05 - 00)
												// invalid : 01

												// VALID FEATURE: 05 - 00

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00
																- PIXEL01  - PIXEL02 - PIXEL03 - PIXEL04 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right
												// invalid : 01 & (05 | 06 | 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : top, right, quarter2 (08 - 12)
										// invalid : (01 | 02 | 03) & (13 | 14 | 15)

										if (LOWER05 && LOWER06 && LOWER07)
										{
											// valid   : top, right, quarter1, quarter2 (04 - 12)
											// invalid : (01 | 02 | 03) & (13 | 14 | 15)

											if (LOWER03)
											{
												// valid   : top, right, quarter1, quarter2 (03 - 12)
												// invalid : (01 | 02) & (13 | 14 | 15)

												if (LOWER02)
												{
													// valid   : top, right, quarter1, quarter2 (02 - 12)
													// invalid : 01 & (13 | 14 | 15)

													if (LOWER13)
													{
														// valid   : top, right, quarter1, quarter2 (02 - 13)
														// invalid : 01 & (14 | 15)

														// VALID FEATURE: 02 - 13

														nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13
																		- PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 + correctionValue);
														continue;
													}
													else
													{
														// valid   : top, right, quarter1, quarter2 (02 - 12)
														// invalid : 01 & 13

														// nothing to do here
													}
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (03 - 12)
													// invalid : 02 & (13 | 14 | 15)

													if (LOWER13 && LOWER14)
													{
														// valid   : top, right, quarter1, quarter2 (03 - 14)
														// invalid : 02 & 15

														// VALID FEATURE: 03 - 14

														nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14
																		- PIXEL15  - PIXEL00 - PIXEL01 - PIXEL02 + correctionValue);
														continue;
													}
													else
													{
														// valid   : top, right, quarter1, quarter2 (03 - 12)
														// invalid : 02 & (13 | 14)

														// nothing to do here
													}
												}
											}
											else
											{
												// valid   : top, right, quarter1, quarter2 (04 - 12)
												// invalid : 03 & (13 | 14 | 15)

												// nothing to do here
											}
										}
										else
										{
											// valid   : top, right, quarter2 (08 - 12)
											// invalid : (01 | 02 | 03) & (05 | 06 | 07) & (13 | 14 | 15)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : top, right, left, bottom
									// invalid : (01 | 02 | 03) & (09 | 10 | 11)

									// nothing to do here
								}
							}
							else
							{
								// valid   : top, right, left
								// invalid : 08 & (01 | 02 | 03)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, right
							// invalid : 12 & (01 | 02 | 03)

							// nothing to do here
						}
					}
				}
				else
				{
					// valid   : top (00)
					// invalid : right (04)

					if (LOWER12)
					{
						// valid   : top, left (00 & 12)
						// invalid : right (04)

						if (LOWER08)
						{
							// valid   : top, left, bottom (00 & 08 & 12)
							// invalid : right (04)

							if (LOWER09 && LOWER10 && LOWER11 && LOWER13 && LOWER14 && LOWER15)
							{
								// valid   : quarter2, quarter3, (08 - 00)
								// invalid : right (04)

								if (LOWER01)
								{
									// valid   : quarter2, quarter3, (08 - 01)
									// invalid : right (04)

									if (LOWER02)
									{
										// valid   : quarter2, quarter3, (08 - 02)
										// invalid : right (04)

										if (LOWER03)
										{
											// valid   : quarter2, quarter3, (08 - 03)
											// invalid : right (04)

											// VALID FEATURE: 08 - 03

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03
															- PIXEL04  - PIXEL05 - PIXEL06 - PIXEL07 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 02)
											// invalid : right (03 & 04)

											if (LOWER07)
											{
												// valid   : quarter2, quarter3, (07 - 02)
												// invalid : right (03 & 04)

												// VALID FEATURE: 07 - 02

												nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02
																- PIXEL03  - PIXEL04 - PIXEL05 - PIXEL06 + correctionValue);
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, (08 - 02)
												// invalid : right (03 & 04 & 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 01)
										// invalid : right (02 & 04)

										if (LOWER06 && LOWER07)
										{
											// valid   : quarter2, quarter3, (06 - 01)
											// invalid : right (02 & 04)

											// VALID FEATURE: 06 - 01

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01
															- PIXEL02  - PIXEL03 - PIXEL04 - PIXEL05 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 01)
											// invalid : right 02 & 04 & (06 | 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 00)
									// invalid : right (01 & 04)

									if (LOWER05 && LOWER06 && LOWER07)
									{
										// valid   : quarter2, quarter3, (05 - 00)
										// invalid : right (01 & 04)

										// VALID FEATURE: (05 - 00)

										nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00
														- PIXEL01  - PIXEL02 - PIXEL03 - PIXEL04 + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 00)
										// invalid : right 01 & 04 & (05 | 06 | 07)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : top, left, bottom (00 & 08 & 12)
								// invalid : right 04 & (09 | 10 | 11 | 13 | 14 | 15)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, left (00 & 12)
							// invalid : right, bottom (04 & 08)

						}	// nothing to do here
					}
					else
					{
						// valid   : top (00)
						// invalid : right, left (04 & 12)

						// nothing to do here
					}
				}
			}
			else
			{
				// valid   : -
				// invalid : 00

				if (LOWER08)
				{
					// valid   : 08
					// invalid : 00

					if (LOWER04 && LOWER12)
					{
						// valid   : 04 & 08 & 12
						// invalid : 00

						if (LOWER05 && LOWER06 && LOWER07 && LOWER09 && LOWER10 && LOWER11)
						{
							// valid   : quarter1, quarter2, (04 - 12)
							// invalid : 00

							if (LOWER03)
							{
								// valid   : quarter1, quarter2, (03 - 12)
								// invalid : 00

								if (LOWER02)
								{
									// valid   : quarter1, quarter2, (02 - 12)
									// invalid : 00

									if (LOWER01)
									{
										// valid   : quarter1, quarter2, (01 - 12)
										// invalid : 00

										// VALID FEATURE: 01 - 12

										nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12
														- PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (02 - 12)
										// invalid : 00 & 01

										if (LOWER13)
										{
											// valid   : quarter1, quarter2, (02 - 13)
											// invalid : 00 & 01

											// VALID FEATURE: 02 - 13

											nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13
															- PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 + correctionValue);
											continue;
										}
										else
										{
											// valid   : quarter1, quarter2, (02 - 12)
											// invalid : 00 & 01 & 13

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter1, quarter2, (03 - 12)
									// invalid : 00 & 02

									if (LOWER13 && LOWER14)
									{
										// valid   : quarter1, quarter2, (03 - 14)
										// invalid : 00 & 02

										// VALID FEATURE: 03 - 14

										nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14
														- PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 + correctionValue);
										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (03 - 12)
										// invalid : 00 & 02 & (13 | 14)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter1, quarter2, (04 - 12)
								// invalid : 00 & 03

								if (LOWER13 && LOWER14 && LOWER15)
								{
									// valid   : quarter1, quarter2, (04 - 15)
									// invalid : 00 & 03

									// VALID FEATURE: 04 - 15

									nonMaximumSuppression->addCandidate(x, y, 16 * CENTER - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15
													- PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 + correctionValue);
									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (04 - 12)
									// invalid : 00 & 03 & (13 | 14 | 15)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : 04 & 08 & 12
							// invalid : 00 & (05 | 06 | 07 | 09 | 10 | 11)

							// nothing to do here
						}
					}
					else
					{
						// valid   : 08
						// invalid : 00 & (04 | 12)

						// nothing to do here
					}
				}
				else
				{
					// valid   : -
					// invalid : 00 & 08

					// nothing to do here
				}
			}
		}

		topMiddle += lineEndOffset;
		middleLeft += lineEndOffset;
		bottomMiddle += lineEndOffset;
	}

	#undef PIXEL00
	#undef PIXEL01
	#undef PIXEL02
	#undef PIXEL03
	#undef PIXEL04
	#undef PIXEL05
	#undef PIXEL06
	#undef PIXEL07
	#undef PIXEL08
	#undef PIXEL09
	#undef PIXEL10
	#undef PIXEL11
	#undef PIXEL12
	#undef PIXEL13
	#undef PIXEL14
	#undef PIXEL15
	#undef CENTER
}

void FASTFeatureDetector::scoreFeaturesPrecise(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeatures& features, const unsigned int framePaddingElements, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 7 && height >= 7);

	if (worker && features.size() > 100)
	{
		worker->executeFunction(Worker::Function::createStatic(&FASTFeatureDetector::scoreFeaturesPreciseSubset, yFrame, width, height, threshold, &features, framePaddingElements, 0u, 0u), 0u, (unsigned int)features.size(), 6u, 7u, 100u);
	}
	else
	{
		scoreFeaturesPreciseSubset(yFrame, width, height, threshold, &features, framePaddingElements, 0u, (unsigned int)features.size());
	}
}

void FASTFeatureDetector::scoreFeaturesPreciseSubset(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeatures* features, const unsigned int framePaddingElements, const unsigned int firstFeature, const unsigned int numberFeatures)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 7 && height >= 7);

	ocean_assert(features != nullptr);
	ocean_assert(firstFeature + numberFeatures <= features->size());

	for (unsigned int n = firstFeature; n < firstFeature + numberFeatures; ++n)
	{
		scoreFeaturePrecise(yFrame, width, height, threshold, (*features)[n], framePaddingElements);
	}
}

void FASTFeatureDetector::scoreFeaturePrecise(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int threshold, FASTFeature& feature, const unsigned int framePaddingElements)
{
	/**
	 * Pixel Positions for FAST features
	 *
	 *      00 01 02 03 04 05 06
	 *     ----------------------
	 * 00 |       15 00 01
	 * 01 |    14          02
	 * 02 | 13                03
	 * 03 | 12       XX       04
	 * 04 | 11                05
	 * 05 |    10          06
	 * 06 |       09 08 07
	 */

	const int frameStrideElements = int(width + framePaddingElements);

	#define PIXEL00 (*(topMiddle))
	#define PIXEL01 (*(topMiddle + 1))
	#define PIXEL02 (*(topMiddle + frameStrideElements + 2))
	#define PIXEL03 (*(middleLeft - frameStrideElements + 6))
	#define PIXEL04 (*(middleLeft + 6))
	#define PIXEL05 (*(middleLeft + frameStrideElements + 6))
	#define PIXEL06 (*(bottomMiddle - frameStrideElements + 2))
	#define PIXEL07 (*(bottomMiddle + 1))
	#define PIXEL08 (*(bottomMiddle))
	#define PIXEL09 (*(bottomMiddle - 1))
	#define PIXEL10 (*(bottomMiddle - frameStrideElements - 2))
	#define PIXEL11 (*(middleLeft + frameStrideElements))
	#define PIXEL12 (*(middleLeft))
	#define PIXEL13 (*(middleLeft - frameStrideElements))
	#define PIXEL14 (*(topMiddle + frameStrideElements - 2))
	#define PIXEL15 (*(topMiddle - 1))
	#define CENTER  (*(middleLeft + 3))

	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 7 && height >= 7);

	const unsigned int featureX = (unsigned int)(feature.observation().x());
	const unsigned int featureY = (unsigned int)(feature.observation().y());

	ocean_assert(featureX >= 3 && featureY >= 3);
	ocean_assert_and_suppress_unused(featureX < width - 3 && featureY < height, height);

	const uint8_t* topMiddle = yFrame + (featureY - 3u) * (unsigned int)(frameStrideElements) + featureX;
	const uint8_t* middleLeft = yFrame + (featureY + 0u) * (unsigned int)(frameStrideElements) + featureX - 3u;
	const uint8_t* bottomMiddle = yFrame + (featureY + 3u) * (unsigned int)(frameStrideElements) + featureX;

	unsigned int lowerTest = threshold;
	unsigned int upperTest = 255u;
	unsigned int oldTest = 0xFFFFFFFFu;

	int strength = 0;

	while (true)
	{
		unsigned int test = lowerTest + (upperTest - lowerTest + 1u) / 2u;

		if (oldTest == test)
		{
			break;
		}

		oldTest = test;

		ocean_assert(test >= lowerTest && test <= upperTest);

		const int centerHigh = int(CENTER) + int(test);
		const int centerLow = int(CENTER) - int(test);

		if (HIGHER00)
		{
			// valid   : top
			// invalid : -

			if (HIGHER04)
			{
				// valid   : top, right
				// invalid : -

				if (HIGHER01 && HIGHER02 && HIGHER03)
				{
					// valid   : quarter0 (00 - 04)
					// invalid : -

					if (HIGHER08)
					{
						// valid   : quarter0, bottom
						// invalid : -

						if (HIGHER05 && HIGHER06 && HIGHER07)
						{
							// valid   : quarter0, quarter1 (00 - 08)
							// invalid : -

							if (HIGHER09)
							{
								// valid   : quarter0, quarter1, 09 (00 - 09)
								// invalid : -

								if (HIGHER10)
								{
									// valid   : quarter0, quarter1, 09, 10 (00 - 10)
									// invalid : -

									if (HIGHER11)
									{
										// valid   : quarter0, quarter1, 09, 10, 11 (00 - 11)
										// invalid : -

										// VALID FEATURE: 00 - 11

										strength = PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11
														+ PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter0, quarter1, 09, 10 (00 - 10)
										// invalid : 11

										if (HIGHER15)
										{
											// VALID FEATURE: 15 - 10

											strength = PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10
														+ PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09, 10 (00 - 10)
											// invalid : 11, 15

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter1, 09 (00 - 09)
									// invalid : 10

									if (HIGHER15 && HIGHER14)
									{
										// valid   : quarter0, quarter1, 09, 14, 15 (14 - 09)
										// invalid : 10

										// VALID FEATURE: 14 - 09

										strength = PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09
														+ PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter0, quarter1, 09 (00 - 09)
										// invalid : 10 & (14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, quarter1 (00 - 08)
								// invalid : 09

								if (HIGHER13 && HIGHER14 && HIGHER15)
								{
									// valid   : quarter0, quarter1, quarter3 (13 - 08)
									// invalid : 09

									//  VALID FEATURE: 13 - 08

									strength = PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08
													+ PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 - 16 * CENTER;
									lowerTest = test;

									continue;
								}
								else
								{
									// valid   : quarter0, quarter1 (00 - 08)
									// invalid : 09 & (13 | 14 | 15)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter0, bottom (00 - 04) & 08
							// invalid : (05 | 06 | 07)

							if (HIGHER12)
							{
								// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
								// invalid : (05 | 06 | 07)

								if (HIGHER13 && HIGHER14 && HIGHER15)
								{
									// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
									// invalid : (05 | 06 | 07)

									if (HIGHER05)
									{
										// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
										// invalid : (06 | 07)

										if (HIGHER06)
										{
											// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
											// invalid : 07

											if (HIGHER11)
											{
												// valid   : quarter0, quarter3, bottom, (11 - 06) & 08
												// invalid : 07

												// VALID FEATURE: 11 - 06

												strength = PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06
													+ PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 - 16 * CENTER;
												lowerTest = test;

												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
												// invalid : 07 & 11

												// nothing to do here
											}
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
											// invalid : 06

											if (HIGHER11 && HIGHER10)
											{
												// valid   : quarter0, quarter3, bottom, (10 - 05) & 08
												// invalid : 06

												// VALID FEATURE: 10 - 05

												strength = PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05
													 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 - 16 * CENTER;
												lowerTest = test;

												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
												// invalid : 06 & (10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
										// invalid : 05

										if (HIGHER09 && HIGHER10 && HIGHER11)
										{
											// valid   : quarter0, quarter2, quarter3, (08 - 04)
											// invalid : 05

											// VALID FEATURE 08 - 04

											strength = PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04
													 + PIXEL05 + PIXEL06 + PIXEL07 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
											// invalid : 05 & (09 | 10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
									// invalid : (05 | 06 | 07) & (13 | 14 | 15)

									// nothing to do here
								}
							}
							else
							{
								// valid   : quarter0, bottom (00 - 04) & 08
								// invalid : (05 | 06 | 07) & 12

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : quarter0 (00 - 04)
						// invalid : 08

						if (HIGHER12)
						{
							// valid   : quarter0, left (00 - 04) & 12
							// invalid : 08

							if (HIGHER13 && HIGHER14 && HIGHER15)
							{
								// valid   : quarter0, quarter3, (12 - 04)
								// invalid : 08

								if (HIGHER05)
								{
									// valid   : quarter0, quarter3, (12 - 04) & 05
									// invalid : 08

									if (HIGHER06)
									{
										// valid   : quarter0, quarter3, (12 - 06)
										// invalid : 08

										if (HIGHER07)
										{
											// valid   : quarter0, quarter3, (12 - 07)
											// invalid : 08

											// VALID FEATURE: 12 - 07

											strength = PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07
													 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 06)
											// invalid : 08 & 07

											if (HIGHER11)
											{
												// valid   : quarter0, quarter3, (11 - 06)
												// invalid : 08 & 07

												// VALID FEATURE: 11 - 06

												strength = PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06
													 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 - 16 * CENTER;
												lowerTest = test;

												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 06)
												// invalid : 08 & 07 && 11

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 05)
										// invalid : 08 & 06

										if (HIGHER10 && HIGHER11)
										{
											// valid   : quarter0, quarter3, (10 - 05)
											// invalid : 08 & 06

											// VALID FEATURE: 10 - 05

											strength = PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05
													 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 05)
											// invalid : 08 & 06 & (10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter3, (12 - 04)
									// invalid : 08 & 05

									if (HIGHER09 && HIGHER10 && HIGHER11)
									{
										// valid   : quarter0, quarter3, (09 - 04)
										// invalid : 08 & 05

										// VALID FEATURE: 09 - 04

										strength = PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04
													 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 04)
										// invalid : 08 & 05 & (09 | 10 | 11)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, left (00 - 04) & 12
								// invalid : 08 & (13 | 14 | 15)

								// nothing to do here
							}
						}
						else
						{
							// valid   : quarter0 (00 - 04)
							// invalid : 08 & 12

							// nothing to do here
						}
					}
				}
				else
				{
					// valid   : top, right
					// invalid : (01 | 02 | 03)

					if (HIGHER12)
					{
						// valid   : top, right, left
						// invalid : (01 | 02 | 03)

						if (HIGHER08)
						{
							// valid   : top, right, left, bottom
							// invalid : (01 | 02 | 03)

							if (HIGHER09 && HIGHER10 && HIGHER11)
							{
								// valid   : top, right, quarter2
								// invalid : (01 | 02 | 03)

								if (HIGHER13 && HIGHER14 && HIGHER15)
								{
									// valid   : quarter2, quarter3, right
									// invalid : (01 | 02 | 03)

									if (HIGHER01)
									{
										// valid   : quarter2, quarter3, right & 01
										// invalid : (02 | 03)

										if (HIGHER02)
										{
											// valid   : quarter2, quarter3, right & 01 & 02
											// invalid : 03

											if (HIGHER07)
											{
												// valid   : quarter2, quarter3, right & 01 & 02 & 07
												// invalid : 03

												// VALID FEATURE: 07 - 02

												strength = PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02
																+ PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 - 16 * CENTER;
												lowerTest = test;

												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01 & 02
												// invalid : 03 & 07

												// nothing to do here
											}
										}
										else
										{
											// valid   : quarter2, quarter3, right & 01
											// invalid : 02

											if (HIGHER06 && HIGHER07)
											{
												// valid   : quarter2, quarter3, right & 01 & 06 & 07
												// invalid : 02

												// VALID FEATURE: 06 - 01

												strength = PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01
																+ PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 - 16 * CENTER;
												lowerTest = test;

												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01
												// invalid : 02 & (06 | 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter2, quarter3, right (08 - 00)
										// invalid : 01

										if (HIGHER05 && HIGHER06 && HIGHER07)
										{
											// valid   : quarter2, quarter3, right (05 - 00)
											// invalid : 01

											// VALID FEATURE: 05 - 00

											strength = PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00
															+ PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, right
											// invalid : 01 & (05 | 06 | 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : top, right, quarter2 (08 - 12)
									// invalid : (01 | 02 | 03) & (13 | 14 | 15)

									if (HIGHER05 && HIGHER06 && HIGHER07)
									{
										// valid   : top, right, quarter1, quarter2 (04 - 12)
										// invalid : (01 | 02 | 03) & (13 | 14 | 15)

										if (HIGHER03)
										{
											// valid   : top, right, quarter1, quarter2 (03 - 12)
											// invalid : (01 | 02) & (13 | 14 | 15)

											if (HIGHER02)
											{
												// valid   : top, right, quarter1, quarter2 (02 - 12)
												// invalid : 01 & (13 | 14 | 15)

												if (HIGHER13)
												{
													// valid   : top, right, quarter1, quarter2 (02 - 13)
													// invalid : 01 & (14 | 15)

													// VALID FEATURE: 02 - 13

													strength = PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13
															+ PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 - 16 * CENTER;
													lowerTest = test;

													continue;
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (02 - 12)
													// invalid : 01 & 13

													// nothing to do here
												}
											}
											else
											{
												// valid   : top, right, quarter1, quarter2 (03 - 12)
												// invalid : 02 & (13 | 14 | 15)

												if (HIGHER13 && HIGHER14)
												{
													// valid   : top, right, quarter1, quarter2 (03 - 14)
													// invalid : 02 & 15

													// VALID FEATURE: 03 - 14

													strength = PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14
															+ PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 - 16 * CENTER;
													lowerTest = test;

													continue;
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (03 - 12)
													// invalid : 02 & (13 | 14)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : top, right, quarter1, quarter2 (04 - 12)
											// invalid : 03 & (13 | 14 | 15)

											// nothing to do here
										}
									}
									else
									{
										// valid   : top, right, quarter2 (08 - 12)
										// invalid : (01 | 02 | 03) & (05 | 06 | 07) & (13 | 14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : top, right, left, bottom
								// invalid : (01 | 02 | 03) & (09 | 10 | 11)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, right, left
							// invalid : 08 & (01 | 02 | 03)

							// nothing to do here
						}
					}
					else
					{
						// valid   : top, right
						// invalid : 12 & (01 | 02 | 03)

						// nothing to do here
					}
				}
			}
			else
			{
				// valid   : top (00)
				// invalid : right (04)

				if (HIGHER12)
				{
					// valid   : top, left (00 & 12)
					// invalid : right (04)

					if (HIGHER08)
					{
						// valid   : top, left, bottom (00 & 08 & 12)
						// invalid : right (04)

						if (HIGHER09 && HIGHER10 && HIGHER11 && HIGHER13 && HIGHER14 && HIGHER15)
						{
							// valid   : quarter2, quarter3, (08 - 00)
							// invalid : right (04)

							if (HIGHER01)
							{
								// valid   : quarter2, quarter3, (08 - 01)
								// invalid : right (04)

								if (HIGHER02)
								{
									// valid   : quarter2, quarter3, (08 - 02)
									// invalid : right (04)

									if (HIGHER03)
									{
										// valid   : quarter2, quarter3, (08 - 03)
										// invalid : right (04)

										// VALID FEATURE: 08 - 03

										strength = PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03
															+ PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 02)
										// invalid : right (03 & 04)

										if (HIGHER07)
										{
											// valid   : quarter2, quarter3, (07 - 02)
											// invalid : right (03 & 04)

											// VALID FEATURE: 07 - 02

											strength = PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02
															+ PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 - 16 * CENTER;
											lowerTest = test;

											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 02)
											// invalid : right (03 & 04 & 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 01)
									// invalid : right (02 & 04)

									if (HIGHER06 && HIGHER07)
									{
										// valid   : quarter2, quarter3, (06 - 01)
										// invalid : right (02 & 04)

										// VALID FEATURE: 06 - 01

										strength = PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01
														+ PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 01)
										// invalid : right 02 & 04 & (06 | 07)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter2, quarter3, (08 - 00)
								// invalid : right (01 & 04)

								if (HIGHER05 && HIGHER06 && HIGHER07)
								{
									// valid   : quarter2, quarter3, (05 - 00)
									// invalid : right (01 & 04)

									// VALID FEATURE: (05 - 00)

									strength = PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00
													+ PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 - 16 * CENTER;
									lowerTest = test;

									continue;
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 00)
									// invalid : right 01 & 04 & (05 | 06 | 07)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : top, left, bottom (00 & 08 & 12)
							// invalid : right 04 & (09 | 10 | 11 | 13 | 14 | 15)

							// nothing to do here
						}
					}
					else
					{
						// valid   : top, left (00 & 12)
						// invalid : right, bottom (04 & 08)

					}	// nothing to do here
				}
				else
				{
					// valid   : top (00)
					// invalid : right, left (04 & 12)

					// nothing to do here
				}
			}
		}
		else
		{
			// valid   : -
			// invalid : 00

			if (HIGHER08)
			{
				// valid   : 08
				// invalid : 00

				if (HIGHER04 && HIGHER12)
				{
					// valid   : 04 & 08 & 12
					// invalid : 00

					if (HIGHER05 && HIGHER06 && HIGHER07 && HIGHER09 && HIGHER10 && HIGHER11)
					{
						// valid   : quarter1, quarter2, (04 - 12)
						// invalid : 00

						if (HIGHER03)
						{
							// valid   : quarter1, quarter2, (03 - 12)
							// invalid : 00

							if (HIGHER02)
							{
								// valid   : quarter1, quarter2, (02 - 12)
								// invalid : 00

								if (HIGHER01)
								{
									// valid   : quarter1, quarter2, (01 - 12)
									// invalid : 00

									// VALID FEATURE: 01 - 12

									strength = PIXEL01 + PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12
													+ PIXEL13 + PIXEL14 + PIXEL15 + PIXEL00 - 16 * CENTER;
									lowerTest = test;

									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (02 - 12)
									// invalid : 00 & 01

									if (HIGHER13)
									{
										// valid   : quarter1, quarter2, (02 - 13)
										// invalid : 00 & 01

										// VALID FEATURE: 02 - 13

										strength = PIXEL02 + PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13
													+ PIXEL14 + PIXEL15 + PIXEL00 + PIXEL01 - 16 * CENTER;
										lowerTest = test;

										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (02 - 12)
										// invalid : 00 & 01 & 13

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter1, quarter2, (03 - 12)
								// invalid : 00 & 02

								if (HIGHER13 && HIGHER14)
								{
									// valid   : quarter1, quarter2, (03 - 14)
									// invalid : 00 & 02

									// VALID FEATURE: 03 - 14

									strength = PIXEL03 + PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14
													+ PIXEL15 + PIXEL00 + PIXEL01 + PIXEL02 - 16 * CENTER;
									lowerTest = test;

									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (03 - 12)
									// invalid : 00 & 02 & (13 | 14)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter1, quarter2, (04 - 12)
							// invalid : 00 & 03

							if (HIGHER13 && HIGHER14 && HIGHER15)
							{
								// valid   : quarter1, quarter2, (04 - 15)
								// invalid : 00 & 03

								// VALID FEATURE: 04 - 15

								strength = PIXEL04 + PIXEL05 + PIXEL06 + PIXEL07 + PIXEL08 + PIXEL09 + PIXEL10 + PIXEL11 + PIXEL12 + PIXEL13 + PIXEL14 + PIXEL15
													+ PIXEL00 + PIXEL01 + PIXEL02 + PIXEL03 - 16 * CENTER;
								lowerTest = test;

								continue;
							}
							else
							{
								// valid   : quarter1, quarter2, (04 - 12)
								// invalid : 00 & 03 & (13 | 14 | 15)

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : 04 & 08 & 12
						// invalid : 00 & (05 | 06 | 07 | 09 | 10 | 11)

						// nothing to do here
					}
				}
				else
				{
					// valid   : 08
					// invalid : 00 & (04 | 12)

					// nothing to do here
				}
			}
			else
			{
				// valid   : -
				// invalid : 00 & 08

				// nothing to do here
			}
		}





		if (LOWER00)
		{
			// valid   : top
			// invalid : -

			if (LOWER04)
			{
				// valid   : top, right
				// invalid : -

				if (LOWER01 && LOWER02 && LOWER03)
				{
					// valid   : quarter0 (00 - 04)
					// invalid : -

					if (LOWER08)
					{
						// valid   : quarter0, bottom
						// invalid : -

						if (LOWER05 && LOWER06 && LOWER07)
						{
							// valid   : quarter0, quarter1 (00 - 08)
							// invalid : -

							if (LOWER09)
							{
								// valid   : quarter0, quarter1, 09 (00 - 09)
								// invalid : -

								if (LOWER10)
								{
									// valid   : quarter0, quarter1, 09, 10 (00 - 10)
									// invalid : -

									if (LOWER11)
									{
										// valid   : quarter0, quarter1, 09, 10, 11 (00 - 11)
										// invalid : -

										// VALID FEATURE: 00 - 11

										strength = 16 * CENTER - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11
													- PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter0, quarter1, 09, 10 (00 - 10)
										// invalid : 11

										if (LOWER15)
										{
											// VALID FEATURE: 15 - 10

											strength = 16 * CENTER - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10
																- PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter0, quarter1, 09, 10 (00 - 10)
											// invalid : 11, 15

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter1, 09 (00 - 09)
									// invalid : 10

									if (LOWER15 && LOWER14)
									{
										// valid   : quarter0, quarter1, 09, 14, 15 (14 - 09)
										// invalid : 10

										// VALID FEATURE: 14 - 09

										strength = 16 * CENTER - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09
															- PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter0, quarter1, 09 (00 - 09)
										// invalid : 10 & (14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, quarter1 (00 - 08)
								// invalid : 09

								if (LOWER13 && LOWER14 && LOWER15)
								{
									// valid   : quarter0, quarter1, quarter3 (13 - 08)
									// invalid : 09

									//  VALID FEATURE: 13 - 08

									strength = 16 * CENTER - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08
														- PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12;
									lowerTest = test;
									continue;
								}
								else
								{
									// valid   : quarter0, quarter1 (00 - 08)
									// invalid : 09 & (13 | 14 | 15)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter0, bottom (00 - 04) & 08
							// invalid : (05 | 06 | 07)

							if (LOWER12)
							{
								// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
								// invalid : (05 | 06 | 07)

								if (LOWER13 && LOWER14 && LOWER15)
								{
									// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
									// invalid : (05 | 06 | 07)

									if (LOWER05)
									{
										// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
										// invalid : (06 | 07)

										if (LOWER06)
										{
											// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
											// invalid : 07

											if (LOWER11)
											{
												// valid   : quarter0, quarter3, bottom, (11 - 06) & 08
												// invalid : 07

												// VALID FEATURE: 11 - 06

												strength = 16 * CENTER - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06
																- PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10;
												lowerTest = test;
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 06) & 08
												// invalid : 07 & 11

												// nothing to do here
											}
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
											// invalid : 06

											if (LOWER11 && LOWER10)
											{
												// valid   : quarter0, quarter3, bottom, (10 - 05) & 08
												// invalid : 06

												// VALID FEATURE: 10 - 05

												strength = 16 * CENTER - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05
																- PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09;
												lowerTest = test;
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, bottom, (12 - 05) & 08
												// invalid : 06 & (10 | 11)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
										// invalid : 05

										if (LOWER09 && LOWER10 && LOWER11)
										{
											// valid   : quarter0, quarter2, quarter3, (08 - 04)
											// invalid : 05

											// VALID FEATURE 08 - 04

											strength = 16 * CENTER - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04
															- PIXEL05 - PIXEL06 - PIXEL07;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, bottom, (12 - 04) & 08
											// invalid : 05 & (09 | 10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, bottom, left (00 - 04) & 08 & 12
									// invalid : (05 | 06 | 07) & (13 | 14 | 15)

									// nothing to do here
								}
							}
							else
							{
								// valid   : quarter0, bottom (00 - 04) & 08
								// invalid : (05 | 06 | 07) & 12

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : quarter0 (00 - 04)
						// invalid : 08

						if (LOWER12)
						{
							// valid   : quarter0, left (00 - 04) & 12
							// invalid : 08

							if (LOWER13 && LOWER14 && LOWER15)
							{
								// valid   : quarter0, quarter3, (12 - 04)
								// invalid : 08

								if (LOWER05)
								{
									// valid   : quarter0, quarter3, (12 - 04) & 05
									// invalid : 08

									if (LOWER06)
									{
										// valid   : quarter0, quarter3, (12 - 06)
										// invalid : 08

										if (LOWER07)
										{
											// valid   : quarter0, quarter3, (12 - 07)
											// invalid : 08

											// VALID FEATURE: 12 - 07

											strength = 16 * CENTER - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07
															- PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 06)
											// invalid : 08 & 07

											if (LOWER11)
											{
												// valid   : quarter0, quarter3, (11 - 06)
												// invalid : 08 & 07

												// VALID FEATURE: 11 - 06

												strength = 16 * CENTER - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06
																- PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10;
												lowerTest = test;
												continue;
											}
											else
											{
												// valid   : quarter0, quarter3, (12 - 06)
												// invalid : 08 & 07 && 11

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 05)
										// invalid : 08 & 06

										if (LOWER10 && LOWER11)
										{
											// valid   : quarter0, quarter3, (10 - 05)
											// invalid : 08 & 06

											// VALID FEATURE: 10 - 05

											strength = 16 * CENTER - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05
															- PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter0, quarter3, (12 - 05)
											// invalid : 08 & 06 & (10 | 11)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter0, quarter3, (12 - 04)
									// invalid : 08 & 05

									if (LOWER09 && LOWER10 && LOWER11)
									{
										// valid   : quarter0, quarter3, (09 - 04)
										// invalid : 08 & 05

										// VALID FEATURE: 09 - 04

										strength = 16 * CENTER - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04
														- PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter0, quarter3, (12 - 04)
										// invalid : 08 & 05 & (09 | 10 | 11)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter0, left (00 - 04) & 12
								// invalid : 08 & (13 | 14 | 15)

								// nothing to do here
							}
						}
						else
						{
							// valid   : quarter0 (00 - 04)
							// invalid : 08 & 12

							// nothing to do here
						}
					}
				}
				else
				{
					// valid   : top, right
					// invalid : (01 | 02 | 03)

					if (LOWER12)
					{
						// valid   : top, right, left
						// invalid : (01 | 02 | 03)

						if (LOWER08)
						{
							// valid   : top, right, left, bottom
							// invalid : (01 | 02 | 03)

							if (LOWER09 && LOWER10 && LOWER11)
							{
								// valid   : top, right, quarter2
								// invalid : (01 | 02 | 03)

								if (LOWER13 && LOWER14 && LOWER15)
								{
									// valid   : quarter2, quarter3, right
									// invalid : (01 | 02 | 03)

									if (LOWER01)
									{
										// valid   : quarter2, quarter3, right & 01
										// invalid : (02 | 03)

										if (LOWER02)
										{
											// valid   : quarter2, quarter3, right & 01 & 02
											// invalid : 03

											if (LOWER07)
											{
												// valid   : quarter2, quarter3, right & 01 & 02 & 07
												// invalid : 03

												// VALID FEATURE: 07 - 02

												strength = 16 * CENTER - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02
																- PIXEL03  - PIXEL04 - PIXEL05 - PIXEL06;
												lowerTest = test;
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01 & 02
												// invalid : 03 & 07

												// nothing to do here
											}
										}
										else
										{
											// valid   : quarter2, quarter3, right & 01
											// invalid : 02

											if (LOWER06 && LOWER07)
											{
												// valid   : quarter2, quarter3, right & 01 & 06 & 07
												// invalid : 02

												// VALID FEATURE: 06 - 01

												strength = 16 * CENTER - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01
																- PIXEL02  - PIXEL03 - PIXEL04 - PIXEL05;
												lowerTest = test;
												continue;
											}
											else
											{
												// valid   : quarter2, quarter3, right & 01
												// invalid : 02 & (06 | 07)

												// nothing to do here
											}
										}
									}
									else
									{
										// valid   : quarter2, quarter3, right (08 - 00)
										// invalid : 01

										if (LOWER05 && LOWER06 && LOWER07)
										{
											// valid   : quarter2, quarter3, right (05 - 00)
											// invalid : 01

											// VALID FEATURE: 05 - 00

											strength = 16 * CENTER - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00
															- PIXEL01  - PIXEL02 - PIXEL03 - PIXEL04;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, right
											// invalid : 01 & (05 | 06 | 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : top, right, quarter2 (08 - 12)
									// invalid : (01 | 02 | 03) & (13 | 14 | 15)

									if (LOWER05 && LOWER06 && LOWER07)
									{
										// valid   : top, right, quarter1, quarter2 (04 - 12)
										// invalid : (01 | 02 | 03) & (13 | 14 | 15)

										if (LOWER03)
										{
											// valid   : top, right, quarter1, quarter2 (03 - 12)
											// invalid : (01 | 02) & (13 | 14 | 15)

											if (LOWER02)
											{
												// valid   : top, right, quarter1, quarter2 (02 - 12)
												// invalid : 01 & (13 | 14 | 15)

												if (LOWER13)
												{
													// valid   : top, right, quarter1, quarter2 (02 - 13)
													// invalid : 01 & (14 | 15)

													// VALID FEATURE: 02 - 13

													strength = 16 * CENTER - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13
																	- PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01;
													lowerTest = test;
													continue;
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (02 - 12)
													// invalid : 01 & 13

													// nothing to do here
												}
											}
											else
											{
												// valid   : top, right, quarter1, quarter2 (03 - 12)
												// invalid : 02 & (13 | 14 | 15)

												if (LOWER13 && LOWER14)
												{
													// valid   : top, right, quarter1, quarter2 (03 - 14)
													// invalid : 02 & 15

													// VALID FEATURE: 03 - 14

													strength = 16 * CENTER - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14
																	- PIXEL15  - PIXEL00 - PIXEL01 - PIXEL02;
													lowerTest = test;
													continue;
												}
												else
												{
													// valid   : top, right, quarter1, quarter2 (03 - 12)
													// invalid : 02 & (13 | 14)

													// nothing to do here
												}
											}
										}
										else
										{
											// valid   : top, right, quarter1, quarter2 (04 - 12)
											// invalid : 03 & (13 | 14 | 15)

											// nothing to do here
										}
									}
									else
									{
										// valid   : top, right, quarter2 (08 - 12)
										// invalid : (01 | 02 | 03) & (05 | 06 | 07) & (13 | 14 | 15)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : top, right, left, bottom
								// invalid : (01 | 02 | 03) & (09 | 10 | 11)

								// nothing to do here
							}
						}
						else
						{
							// valid   : top, right, left
							// invalid : 08 & (01 | 02 | 03)

							// nothing to do here
						}
					}
					else
					{
						// valid   : top, right
						// invalid : 12 & (01 | 02 | 03)

						// nothing to do here
					}
				}
			}
			else
			{
				// valid   : top (00)
				// invalid : right (04)

				if (LOWER12)
				{
					// valid   : top, left (00 & 12)
					// invalid : right (04)

					if (LOWER08)
					{
						// valid   : top, left, bottom (00 & 08 & 12)
						// invalid : right (04)

						if (LOWER09 && LOWER10 && LOWER11 && LOWER13 && LOWER14 && LOWER15)
						{
							// valid   : quarter2, quarter3, (08 - 00)
							// invalid : right (04)

							if (LOWER01)
							{
								// valid   : quarter2, quarter3, (08 - 01)
								// invalid : right (04)

								if (LOWER02)
								{
									// valid   : quarter2, quarter3, (08 - 02)
									// invalid : right (04)

									if (LOWER03)
									{
										// valid   : quarter2, quarter3, (08 - 03)
										// invalid : right (04)

										// VALID FEATURE: 08 - 03

										strength = 16 * CENTER - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03
														- PIXEL04  - PIXEL05 - PIXEL06 - PIXEL07;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 02)
										// invalid : right (03 & 04)

										if (LOWER07)
										{
											// valid   : quarter2, quarter3, (07 - 02)
											// invalid : right (03 & 04)

											// VALID FEATURE: 07 - 02

											strength = 16 * CENTER - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02
															- PIXEL03  - PIXEL04 - PIXEL05 - PIXEL06;
											lowerTest = test;
											continue;
										}
										else
										{
											// valid   : quarter2, quarter3, (08 - 02)
											// invalid : right (03 & 04 & 07)

											// nothing to do here
										}
									}
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 01)
									// invalid : right (02 & 04)

									if (LOWER06 && LOWER07)
									{
										// valid   : quarter2, quarter3, (06 - 01)
										// invalid : right (02 & 04)

										// VALID FEATURE: 06 - 01

										strength = 16 * CENTER - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01
														- PIXEL02  - PIXEL03 - PIXEL04 - PIXEL05;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter2, quarter3, (08 - 01)
										// invalid : right 02 & 04 & (06 | 07)

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter2, quarter3, (08 - 00)
								// invalid : right (01 & 04)

								if (LOWER05 && LOWER06 && LOWER07)
								{
									// valid   : quarter2, quarter3, (05 - 00)
									// invalid : right (01 & 04)

									// VALID FEATURE: (05 - 00)

									strength = 16 * CENTER - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00
													- PIXEL01  - PIXEL02 - PIXEL03 - PIXEL04;
									lowerTest = test;
									continue;
								}
								else
								{
									// valid   : quarter2, quarter3, (08 - 00)
									// invalid : right 01 & 04 & (05 | 06 | 07)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : top, left, bottom (00 & 08 & 12)
							// invalid : right 04 & (09 | 10 | 11 | 13 | 14 | 15)

							// nothing to do here
						}
					}
					else
					{
						// valid   : top, left (00 & 12)
						// invalid : right, bottom (04 & 08)

					}	// nothing to do here
				}
				else
				{
					// valid   : top (00)
					// invalid : right, left (04 & 12)

					// nothing to do here
				}
			}
		}
		else
		{
			// valid   : -
			// invalid : 00

			if (LOWER08)
			{
				// valid   : 08
				// invalid : 00

				if (LOWER04 && LOWER12)
				{
					// valid   : 04 & 08 & 12
					// invalid : 00

					if (LOWER05 && LOWER06 && LOWER07 && LOWER09 && LOWER10 && LOWER11)
					{
						// valid   : quarter1, quarter2, (04 - 12)
						// invalid : 00

						if (LOWER03)
						{
							// valid   : quarter1, quarter2, (03 - 12)
							// invalid : 00

							if (LOWER02)
							{
								// valid   : quarter1, quarter2, (02 - 12)
								// invalid : 00

								if (LOWER01)
								{
									// valid   : quarter1, quarter2, (01 - 12)
									// invalid : 00

									// VALID FEATURE: 01 - 12

									strength = 16 * CENTER - PIXEL01 - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12
													- PIXEL13 - PIXEL14 - PIXEL15 - PIXEL00;
									lowerTest = test;
									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (02 - 12)
									// invalid : 00 & 01

									if (LOWER13)
									{
										// valid   : quarter1, quarter2, (02 - 13)
										// invalid : 00 & 01

										// VALID FEATURE: 02 - 13

										strength = 16 * CENTER - PIXEL02 - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13
														- PIXEL14 - PIXEL15 - PIXEL00 - PIXEL01;
										lowerTest = test;
										continue;
									}
									else
									{
										// valid   : quarter1, quarter2, (02 - 12)
										// invalid : 00 & 01 & 13

										// nothing to do here
									}
								}
							}
							else
							{
								// valid   : quarter1, quarter2, (03 - 12)
								// invalid : 00 & 02

								if (LOWER13 && LOWER14)
								{
									// valid   : quarter1, quarter2, (03 - 14)
									// invalid : 00 & 02

									// VALID FEATURE: 03 - 14

									strength = 16 * CENTER - PIXEL03 - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14
													- PIXEL15 - PIXEL00 - PIXEL01 - PIXEL02;
									lowerTest = test;
									continue;
								}
								else
								{
									// valid   : quarter1, quarter2, (03 - 12)
									// invalid : 00 & 02 & (13 | 14)

									// nothing to do here
								}
							}
						}
						else
						{
							// valid   : quarter1, quarter2, (04 - 12)
							// invalid : 00 & 03

							if (LOWER13 && LOWER14 && LOWER15)
							{
								// valid   : quarter1, quarter2, (04 - 15)
								// invalid : 00 & 03

								// VALID FEATURE: 04 - 15

								strength = 16 * CENTER - PIXEL04 - PIXEL05 - PIXEL06 - PIXEL07 - PIXEL08 - PIXEL09 - PIXEL10 - PIXEL11 - PIXEL12 - PIXEL13 - PIXEL14 - PIXEL15
												- PIXEL00 - PIXEL01 - PIXEL02 - PIXEL03;
								lowerTest = test;
								continue;
							}
							else
							{
								// valid   : quarter1, quarter2, (04 - 12)
								// invalid : 00 & 03 & (13 | 14 | 15)

								// nothing to do here
							}
						}
					}
					else
					{
						// valid   : 04 & 08 & 12
						// invalid : 00 & (05 | 06 | 07 | 09 | 10 | 11)

						// nothing to do here
					}
				}
				else
				{
					// valid   : 08
					// invalid : 00 & (04 | 12)

					// nothing to do here
				}
			}
			else
			{
				// valid   : -
				// invalid : 00 & 08

				// nothing to do here
			}
		}

		upperTest = test;

		if (lowerTest + 1u == upperTest)
		{
			--upperTest;
		}
	}

	ocean_assert(strength != 0);
	feature.setStrength(Scalar(16 * 255 * oldTest + strength));

	#undef PIXEL00
	#undef PIXEL01
	#undef PIXEL02
	#undef PIXEL03
	#undef PIXEL04
	#undef PIXEL05
	#undef PIXEL06
	#undef PIXEL07
	#undef PIXEL08
	#undef PIXEL09
	#undef PIXEL10
	#undef PIXEL11
	#undef PIXEL12
	#undef PIXEL13
	#undef PIXEL14
	#undef PIXEL15
	#undef CENTER
}

}

}

}
