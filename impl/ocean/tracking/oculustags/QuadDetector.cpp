/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/oculustags/QuadDetector.h"
#include "ocean/tracking/oculustags/Utilities.h"

#include "ocean/cv/detector/LineDetectorULF.h"

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	#include "ocean/cv/Canvas.h"
	#include "ocean/cv/detector/Utilities.h"
	#include "ocean/tracking/oculustags/OculusTagDebugElements.h"
#endif

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

QuadDetector::Quads QuadDetector::detectQuads(const Frame& yFrame, const uint32_t frameBorder)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(yFrame.width() >= 2u * frameBorder && yFrame.height() >= 2u * frameBorder);

	// Extract line segments

	const unsigned int threshold = 100u;
	const unsigned int minimalLength = OculusTag::numberOfModules;
	const float maximalStraightLineDistance = 3.1f;

	FiniteLines2 finiteLines = CV::Detector::LineDetectorULF::detectLines(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::Detector::LineDetectorULF::SDStepEdgeDetectorI::asEdgeDetectors(2u, 0u), threshold, minimalLength, maximalStraightLineDistance);

	// Remove lines that are too close to the border of the image

	CV::Detector::ShapeDetector::removeLinesTooCloseToBorder(finiteLines, yFrame.width(), yFrame.height(), Scalar(frameBorder));

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LINE_SEGMENTS))
	{
		Frame rgbFrame;
		CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, true);

		for (const FiniteLine2& line : finiteLines)
		{
			CV::Canvas::line<3u>(rgbFrame, line, CV::Canvas::red(rgbFrame.pixelFormat()));
		}

		OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LINE_SEGMENTS, std::move(rgbFrame));
	}
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

	// Determine L-shape from the collection of line segments

	const Scalar lShapeDistanceThreshold = 4u;
	const Scalar lShapeAngleThreshold = Numeric::deg2rad(Scalar(44.9));

	CV::Detector::ShapeDetector::LShapes lShapes = CV::Detector::ShapeDetector::determineLShapes(finiteLines, yFrame.width(), yFrame.height(), lShapeDistanceThreshold, lShapeAngleThreshold);

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_INITIAL))
	{
		Frame rgbFrame;
		CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, true);

		CV::Detector::Utilities::paintLShapes(rgbFrame, finiteLines, lShapes, CV::Canvas::red(rgbFrame.pixelFormat()));

		OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_INITIAL, std::move(rgbFrame));
	}
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

	// Apply non maximum suppression to remove redundant L-shapes

	const Scalar nonMaximumSuppressionDistanceThreshold = Scalar(lShapeDistanceThreshold) * Scalar(0.5);
	const Scalar nonMaximumSuppressionAngleThreshold = Numeric::deg2rad(Scalar(44.9));

	lShapes = CV::Detector::ShapeDetector::nonMaximumSuppressionLShapes(lShapes, yFrame.width(), yFrame.height(), nonMaximumSuppressionDistanceThreshold, nonMaximumSuppressionAngleThreshold);

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_FINAL))
	{
		Frame rgbFrame;
		CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, true);

		CV::Detector::Utilities::paintLShapes(rgbFrame, finiteLines, lShapes, CV::Canvas::green(rgbFrame.pixelFormat()));

		OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_FINAL, std::move(rgbFrame));
	}
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

	// Extract boundary patterns

	const Scalar angleThreshold = Numeric::deg2rad(Scalar(44.9));

	Quads quads = extractQuads(yFrame, lShapes, finiteLines, angleThreshold, frameBorder);

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_DETECTIONS))
	{
		Frame rgbFrame;
		CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame, true);

		Utilities::drawQuads(rgbFrame, quads);
		OculusTagDebugElements::get().updateElement(OculusTagDebugElements::EI_BOUNDARY_PATTERN_DETECTIONS, std::move(rgbFrame));
	}
#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

	return quads;
}

QuadDetector::Quads QuadDetector::extractQuads(const Frame& yFrame, const CV::Detector::ShapeDetector::LShapes& lShapes, const FiniteLines2& finiteLines, const Scalar angleThreshold, const uint32_t frameBorder)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(angleThreshold < Numeric::deg2rad(90));
	ocean_assert(yFrame.width() >= 2u * frameBorder && yFrame.height() >= 2u * frameBorder);

	// Create a reverse lookup table that maps from any line index to all L-shapes that they are a member of

	std::vector<std::unordered_set<Index32>> reverseLookup(finiteLines.size());

	for (size_t i = 0; i < lShapes.size(); ++i)
	{
		const CV::Detector::ShapeDetector::LShape& lShape = lShapes[i];

		ocean_assert(lShape.finiteLineIndex0() < reverseLookup.size() && lShape.finiteLineIndex1() < reverseLookup.size());

		reverseLookup[lShape.finiteLineIndex0()].insert(Index32(i));
		reverseLookup[lShape.finiteLineIndex1()].insert(Index32(i));
	}

	// Use the reverse lookup above to find 4 connected L-shapes that form a square-like shape
	// (also covers edge cases where less than 4 L-shapes are found).

	const Scalar angleThresholdCos = Numeric::cos(angleThreshold);
	std::set<std::tuple< Index32, Index32, Index32, Index32>> visitedLShapes;
	Quads Quads;

	constexpr uint32_t windingDirection = 0u;  // 0 - left (CCW), 1 - right (CW)

	// 1 L-shape:
	//
	//   CCW:                    CW:
	//
	//   A       e0              A       e3
	//     +-->--------            +------------
	//     |                       |
	//     |                       v
	//  e3 |                    e0 |
	//     |                       |
	//     |                       |
	//
	for (Index32 indexA = 0u; indexA < lShapes.size(); ++indexA)
	{
		const CV::Detector::ShapeDetector::LShape& lShapeA = lShapes[indexA];

		if (lShapeA.position().x() < Scalar(frameBorder) || lShapeA.position().x() >= Scalar(yFrame.width() - frameBorder) || lShapeA.position().y() < Scalar(frameBorder) || lShapeA.position().y() >= Scalar(yFrame.height() - frameBorder))
		{
			continue;
		}

		ocean_assert(lShapeA.finiteLineIndex(windingDirection) < reverseLookup.size());
		const std::unordered_set<Index32>& lShapesB = reverseLookup[lShapeA.finiteLineIndex(windingDirection)];

		// Remember maximum number of sides that have been seen - this helps with to cover edge cases
		bool foundSides[5] = { true, true, false, false, false };

		// 2 L-shapes:
		//
		//   CW:                     CCW:
		//
		//   A       e0              A      e3
		//     +-->--------+           +------------
		//     |           |           |
		//     |           v           v
		//  e3 |           | e1     e0 |
		//     |           |           |
		//     |           |           |
		//                             +-->---------
		//                           B      e1
		for (std::unordered_set<Index32>::const_iterator indexBIter = lShapesB.cbegin(); indexBIter != lShapesB.cend(); ++indexBIter)
		{
			const CV::Detector::ShapeDetector::LShape& lShapeB = lShapes[*indexBIter];

			if (lShapeB.position().x() < Scalar(frameBorder) || lShapeB.position().x() >= Scalar(yFrame.width() - frameBorder) || lShapeB.position().y() < Scalar(frameBorder) || lShapeB.position().y() >= Scalar(yFrame.height() - frameBorder))
			{
				continue;
			}

			if (*indexBIter != indexA
				&& lShapeA.finiteLineIndex(windingDirection) == lShapeB.finiteLineIndex(1u - windingDirection)
				&& std::abs(lShapeA.direction() * lShapeB.direction()) < angleThresholdCos
				&& lShapeA.position().sqrDistance(lShapeB.position()) >= Scalar(OculusTag::numberOfModules * OculusTag::numberOfModules))
			{
				// L-shapes A and B are different, located above the minimum distance, and the winding direction is still correct

				foundSides[2] = true;

				ocean_assert(lShapeB.finiteLineIndex(windingDirection) < reverseLookup.size());
				const std::unordered_set<Index32>& lShapesC = reverseLookup[lShapeB.finiteLineIndex(windingDirection)];

				// 3 L-shapes:
				//
				//   CW:                     CCW:
				//
				//   A      e0       B       A      e3
				//     +-->--------+           +---------
				//     |           |           |
				//     |           v           v           |
				//  e3 |           | e1     e0 |           | e2
				//     |           |           |           ^
				//                 |           |           |
				//        ------<--+           +-->--------+
				//          e2       C       B      e1       C
				for (std::unordered_set<Index32>::const_iterator indexCIter = lShapesC.cbegin(); indexCIter != lShapesC.cend(); ++indexCIter)
				{
					const CV::Detector::ShapeDetector::LShape& lShapeC = lShapes[*indexCIter];

					if (lShapeC.position().x() < Scalar(frameBorder) || lShapeC.position().x() >= Scalar(yFrame.width() - frameBorder) || lShapeC.position().y() < Scalar(frameBorder) || lShapeC.position().y() >= Scalar(yFrame.height() - frameBorder))
					{
						continue;
					}

					if (*indexCIter != indexA && *indexCIter != *indexBIter
						&& lShapeB.finiteLineIndex(windingDirection) == lShapeC.finiteLineIndex(1u - windingDirection)
						&& std::abs(lShapeB.direction() * lShapeC.direction()) < angleThresholdCos
						&& lShapeB.position().sqrDistance(lShapeC.position()) >= Scalar(OculusTag::numberOfModules * OculusTag::numberOfModules))
					{
						// L-shapes A, B, and C are different, located above the minimum distance, and the winding direction is still correct

						foundSides[3] = true;

						ocean_assert(lShapeC.finiteLineIndex(windingDirection) < reverseLookup.size());
						const std::unordered_set<Index32>& lShapesD = reverseLookup[lShapeC.finiteLineIndex(windingDirection)];

						// 4 L-shapes:
						//
						//   CW:                     CCW:
						//
						//   A      e0       B       A      e3       D
						//     +-->--------+           +--------<--+
						//     |           |           |           |
						//     |           v           v           |
						//  e3 |           | e1     e0 |           | e2
						//     ^           |           |           ^
						//     |           |           |           |
						//     +--------<--+           +-->--------+
						//   D      e2       C       B      e1       C
						for (std::unordered_set<Index32>::const_iterator indexDIter = lShapesD.begin(); indexDIter != lShapesD.end(); ++indexDIter)
						{
							Index32 indices[4] = { indexA, *indexBIter, *indexCIter, *indexDIter };
							std::sort(indices, indices + 4);

							if (visitedLShapes.find(std::make_tuple(indices[0], indices[1], indices[2], indices[3])) == visitedLShapes.end())
							{
								const CV::Detector::ShapeDetector::LShape& lShapeD = lShapes[*indexDIter];

								if (lShapeD.position().x() < Scalar(frameBorder) || lShapeD.position().x() >= Scalar(yFrame.width() - frameBorder) || lShapeD.position().y() < Scalar(frameBorder) || lShapeD.position().y() >= Scalar(yFrame.height() - frameBorder))
								{
									continue;
								}

								if (*indexDIter != indexA && *indexDIter != *indexBIter
									&& *indexDIter != *indexCIter && lShapeC.finiteLineIndex(windingDirection) == lShapeD.finiteLineIndex(1u - windingDirection)
									&& std::abs(lShapeC.direction() * lShapeD.direction()) < angleThresholdCos
									&& lShapeC.position().sqrDistance(lShapeD.position()) >= Scalar(OculusTag::numberOfModules * OculusTag::numberOfModules)
									&& std::abs(lShapeD.direction() * lShapeA.direction()) < angleThresholdCos
									&& lShapeD.position().sqrDistance(lShapeA.position()) >= Scalar(OculusTag::numberOfModules * OculusTag::numberOfModules))
								{
									// L-shapes A, B, C and D are different, located above the minimum distance, and the winding direction is still correct

									ocean_assert(lShapeD.finiteLineIndex(windingDirection) < reverseLookup.size());
									const std::unordered_set<Index32>& lShapesFinal = reverseLookup[lShapeD.finiteLineIndex(0)];

									if (lShapesFinal.find(indexA) != lShapesFinal.end())
									{
										if (lShapeA.direction() * lShapeC.direction() >= 0 || lShapeB.direction() * lShapeD.direction() >= 0)
										{
											continue;
										}

										Quad Quad;

										// Make sure the corners are sorted in counter-clockwise order
										if constexpr (windingDirection == 0)
										{
											Quad[3] = lShapeA.position();
											Quad[2] = lShapeB.position();
											Quad[1] = lShapeC.position();
											Quad[0] = lShapeD.position();
										}
										else
										{
											Quad[0] = lShapeA.position();
											Quad[1] = lShapeB.position();
											Quad[2] = lShapeC.position();
											Quad[3] = lShapeD.position();
										}

										ocean_assert((Quad[1] - Quad[0]).cross(Quad[3] - Quad[0]) <= 0);
										ocean_assert((Quad[2] - Quad[1]).cross(Quad[0] - Quad[1]) <= 0);
										ocean_assert((Quad[3] - Quad[2]).cross(Quad[1] - Quad[2]) <= 0);
										ocean_assert((Quad[0] - Quad[3]).cross(Quad[2] - Quad[3]) <= 0);

										if (refineQuad(yFrame, Quad))
										{
											Quads.emplace_back(Quad);
											foundSides[4] = true;
										}
									}
								}
							}

							visitedLShapes.insert(std::make_tuple(indices[0], indices[1], indices[2], indices[3]));
						}

						if (foundSides[0] && foundSides[1] && foundSides[2] && foundSides[3] && foundSides[0] == false)
						{
							// Three matching L-shapes have been found but the fourth one is missing. Is it possible to find the last corner anyway?
							//
							//   CW:                     CCW:
							//
							//   A      e0       B       A      e3
							//     +-->--------+           +---------  ?
							//     |           |           |
							//     |           v           v           |
							//  e3 |           | e1     e0 |           | e2
							//     |           |           |           ^
							//                 |           |           |
							//     ?  ------<--+           +-->--------+
							//          e2       C       B      e1       C

							// TODOX Add implementation

							bool found = false;

							if (found)
							{
								foundSides[4] = true;
							}
						}
					}
				}

				if (foundSides[0] && foundSides[1] && foundSides[2] && foundSides[3] == false && foundSides[0] == false)
				{
					//   CW:                     CCW:
					//
					//   A       e0      B       A      e3
					//     +-->--------+           +------------
					//     |           |           |
					//     |           v           v
					//  e3 |           | e1     e0 |
					//     |           |           |
					//     |           |           |
					//                             +-->---------
					//                           B      e1

					// TODOX Add implementation

					bool found = false;

					if (found)
					{
						foundSides[3] = true;
						foundSides[4] = true;
					}
				}
			}
		}
	}

	return Quads;
}

bool QuadDetector::refineQuad(const Frame& yFrame, Quad& quad)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));

#if 0

	Scalar minEdgeLength = Numeric::maxValue();

	for (size_t i = 0; i < 4; ++i)
	{
		minEdgeLength = std::min(FiniteLine2(quad[0], quad[1]).sqrLength(), minEdgeLength);
	}

	const uint32_t numberOfNeighborPixels = std::max(2u, std::min(4u, uint32_t(Numeric::round32(Scalar(0.4 * 0.125) * minEdgeLength)))); // Scale module size in order to avoid the corner refinement to jump to an adjacent corner

	for (Vector2& corner : quad)
	{
		Utilities::refineCorner(yFrame, corner, numberOfNeighborPixels);
	}

#else

	const Scalar averageEdgeLength = Scalar(0.25) *
		(FiniteLine2(quad[0], quad[1]).length() +
		FiniteLine2(quad[1], quad[2]).length() +
		FiniteLine2(quad[2], quad[3]).length() +
		FiniteLine2(quad[3], quad[0]).length());

	const Scalar averageModuleSize = averageEdgeLength * Scalar(0.125);
	const uint32_t numberOfNeighborPixels = std::max(2u, std::min(4u, uint32_t(Numeric::round32(Scalar(0.4) * averageModuleSize)))); // Scale module size in order to avoid the corner refinement to jump to an adjacent corner

	for (Vector2& corner : quad)
	{
		Utilities::refineCorner(yFrame, corner, numberOfNeighborPixels);
	}

#endif

	return true;
}

}  // namespace OculusTags

}  // namespace Tracking

}  // namespace Ocean
