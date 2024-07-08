/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterCanny.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/FrameFilterScharr.h"
#include "ocean/cv/FrameFilterSobel.h"

namespace Ocean
{

namespace CV
{

void FrameFilterCanny::filterCannySobelNormalized(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int8_t lowThreshold, const int8_t highThreshold, Worker* worker)
{
	filterCanny<int8_t, EF_SOBEL>(source, target, width, height, sourcePaddingElements, targetPaddingElements, lowThreshold, highThreshold, worker);
}

void FrameFilterCanny::filterCannySobel(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int16_t lowThreshold, const int16_t highThreshold, Worker* worker)
{
	filterCanny<int16_t, EF_SOBEL>(source, target, width, height, sourcePaddingElements, targetPaddingElements, lowThreshold, highThreshold, worker);
}

void FrameFilterCanny::filterCannyScharrNormalized(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int8_t lowThreshold, const int8_t highThreshold, Worker* worker)
{
	filterCanny<int8_t, EF_SCHARR>(source, target, width, height, sourcePaddingElements, targetPaddingElements, lowThreshold, highThreshold, worker);
}

void FrameFilterCanny::filterCannyScharr(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const int16_t lowThreshold, const int16_t highThreshold, Worker* worker)
{
	filterCanny<int16_t, EF_SCHARR>(source, target, width, height, sourcePaddingElements, targetPaddingElements, lowThreshold, highThreshold, worker);
}

template<typename TFilterOutputElementType, FrameFilterCanny::EdgeFilter tEdgeFilter>
void FrameFilterCanny::filterCanny(const uint8_t* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, Worker* worker)
{
	static_assert(std::is_same<TFilterOutputElementType, int8_t>::value || std::is_same<TFilterOutputElementType, int16_t>::value, "TFilterType must be an 8 or 16 bit signed type");

	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(lowThreshold < highThreshold);

	ocean_assert((std::is_same<TFilterOutputElementType, int16_t>::value && tEdgeFilter == EF_SOBEL) == false || (highThreshold <= 1020 && "Threshold range for unnormalized Sobel filter is [0, 1020]"));
	ocean_assert((std::is_same<TFilterOutputElementType, int16_t>::value && tEdgeFilter == EF_SCHARR) == false || (highThreshold <= 4080 && "Threshold range for unnormalized Sobel filter is [0, 4080]"));

	static_assert(tEdgeFilter == EF_SOBEL || tEdgeFilter == EF_SCHARR, "The edge filter must be either Sobel or Scharr");

	Memory filterResponses(sizeof(TFilterOutputElementType) * width * height * /* number of filter directions */ 4u);

	if constexpr (tEdgeFilter == EF_SOBEL)
	{
		constexpr unsigned int filterResponsesPaddingElements = 0u;

		FrameFilterSobel::filter8BitPerChannel<TFilterOutputElementType, 1u>(source, (TFilterOutputElementType*)filterResponses.data(), width, height, sourcePaddingElements, filterResponsesPaddingElements, worker);
	}
	else
	{
		constexpr unsigned int filterResponsesPaddingElements = 0u;

		FrameFilterScharr::filter8BitPerChannel<TFilterOutputElementType, 1u>(source, filterResponses.data<TFilterOutputElementType>(), width, height, sourcePaddingElements, filterResponsesPaddingElements, worker);
	}

	Memory gradientDirections(sizeof(uint8_t) * width * height);
	Memory gradientMagnitudes(sizeof(TFilterOutputElementType) * height * width);

	extractGradientDirectionsAndMagnitudes((TFilterOutputElementType*)filterResponses.data(), (uint8_t*)gradientDirections.data(), (TFilterOutputElementType*)gradientMagnitudes.data(), TFilterOutputElementType(lowThreshold), width, height, worker);

	extractEdgePixels((uint8_t*)gradientDirections.data(), (TFilterOutputElementType*)gradientMagnitudes.data(), target, width, height, targetPaddingElements, lowThreshold, highThreshold, worker);
}

template <typename T>
void FrameFilterCanny::extractGradientDirectionsAndMagnitudesSubset(const T* egdeFilterResults, uint8_t* direction, T* magnitude, const T lowThreshold, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(egdeFilterResults && direction && magnitude);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int sourceWidth = width * 4u;

	const T* source0 = egdeFilterResults + firstRow * sourceWidth - 1; // minus 1, because of ++
	const T* const source0End = source0 + numberRows * sourceWidth;

	direction += (firstRow * width);
	magnitude += (firstRow * width);

#ifdef OCEAN_DEBUG
	const T* const debugMagnitudeEnd = magnitude + numberRows * width;
	const uint8_t* const debugDirectionEnd = direction + numberRows * width;
#endif

	while (source0 != source0End)
	{
		ocean_assert(source0 < source0End);
		ocean_assert(magnitude < debugMagnitudeEnd);
		ocean_assert(direction < debugDirectionEnd);

		// Finding the intensity gradient of the image and absolute magnitudes
		const T edge0 = T(abs(*++source0));
		const T edge90 = T(abs(*++source0));
		const T edge45 = T(abs(*++source0));
		const T edge135 = T(abs(*++source0));

		if (edge0 > edge45 && edge0 > edge90 && edge0 > edge135 && edge0 > lowThreshold)
		{
			*direction = ED_VERTICAL;
			*magnitude = edge0;
		}
		else if (edge45 > edge0 && edge45 > edge90 && edge45 > edge135 && edge45 > lowThreshold)
		{
			*direction = ED_DIAGONAL_45;
			*magnitude = edge45;
		}
		else if (edge90 > edge0 && edge90 > edge45 && edge90 > edge135 && edge90 > lowThreshold)
		{
			*direction = ED_HORIZONTAL;
			*magnitude = edge90;
		}
		else if (edge135 > edge0 && edge135 > edge90 && edge135 > edge45 && edge135 > lowThreshold)
		{
			*direction = ED_DIAGONAL_135;
			*magnitude = edge135;
		}
		else
		{
			// no edge because magnitude < lowThreshold
			*direction = ED_NO_EDGE;
			*magnitude = 0;
		}

		ocean_assert(*direction != ED_NO_EDGE || *magnitude <= lowThreshold);

		magnitude++;
		direction++;
	}
}

template<typename TFilterOutputElementType>
inline void FrameFilterCanny::extractEdgePixels(const uint8_t* const gradientDirections, const TFilterOutputElementType* const gradientMagnitudes, uint8_t* const target, const unsigned int width, const unsigned int height, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, Worker* worker)
{
	ocean_assert(gradientDirections != nullptr);
	ocean_assert(gradientMagnitudes != nullptr);
	ocean_assert(target != nullptr);
	ocean_assert(width >= 3u && height >= 3u);

	const unsigned int targetStrideElements = width + targetPaddingElements;

	// Edges will not be detected in border pixels by design, so, set the first and the last rows to zero (left- and right-most columns are set inside the subset function below)
	memset(target, 0, width);
	memset(target + (height - 1u) * targetStrideElements, 0, width);

	// Map to store edge type of pixels (no edge, weak edge, strong edge), stack to store indices of strong edge pixels which are used to find connected weak edge pixels
	Memory edgeCandidates(sizeof(uint8_t) * width * height);
	CV::PixelPositions strongEdgeLocations;
	Lock strongEdgeLock;

	memset(edgeCandidates.data(), 0, width);
	memset((uint8_t*)edgeCandidates.data() + (height - 1u) * width, 0, width);

	// Extract edge but skip the first and last row
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&extractEdgePixelsSubset, gradientDirections, gradientMagnitudes, target, (uint8_t*)edgeCandidates.data(), &strongEdgeLocations, &strongEdgeLock, width, height, targetPaddingElements, lowThreshold, highThreshold, 0u, 0u), 1u, height - 2u, 11u, 12u, 20u);
	}
	else
	{
		extractEdgePixelsSubset(gradientDirections, gradientMagnitudes, target, (uint8_t*)edgeCandidates.data(), &strongEdgeLocations, nullptr, width, height, targetPaddingElements, lowThreshold, highThreshold, 1u, height - 2u);
	}

	// In the map of edge candidates find all weak edges (128) that are connected to strong edge (255)
	while (!strongEdgeLocations.empty())
	{
		const CV::PixelPosition strongEdgeLocation = strongEdgeLocations.back();
		ocean_assert(strongEdgeLocation.x() != 0u && strongEdgeLocation.x() < (width - 1u) && strongEdgeLocation.y() != 0u && strongEdgeLocation.y() < (height - 1u));

		strongEdgeLocations.pop_back();

		// Weak edge locations (128u) in the 8-neighborhood of the current strong edge (255u) are upgraded to a strong edge as well.
		const CV::PixelPosition neighbors[8] =
		{
			strongEdgeLocation.northWest(),
			strongEdgeLocation.north(),
			strongEdgeLocation.northEast(),

			strongEdgeLocation.west(),
			strongEdgeLocation.east(),

			strongEdgeLocation.southWest(),
			strongEdgeLocation.south(),
			strongEdgeLocation.southEast(),
		};

		uint8_t* edgeCandidatesData = edgeCandidates.data<uint8_t>();

		for (unsigned int i = 0u; i < 8u; ++i)
		{
			const unsigned int targetPixelIndex = neighbors[i].y() * targetStrideElements + neighbors[i].x();
			const unsigned int edgePixelIndex = neighbors[i].y() * width + neighbors[i].x();

			if (edgeCandidatesData[edgePixelIndex] == 128u)
			{
				ocean_assert(neighbors[i].x() != 0u && neighbors[i].x() < (width - 1u) && neighbors[i].y() != 0u && neighbors[i].y() < (height - 1u));

				target[targetPixelIndex] = 255u;
				edgeCandidatesData[edgePixelIndex] = 255u;

				strongEdgeLocations.push_back(neighbors[i]);
			}
		}
	}
}

template<typename TFilterOutputElementType>
inline void FrameFilterCanny::extractEdgePixelsSubset(const uint8_t* const gradientDirections, const TFilterOutputElementType* const gradientMagnitudes, uint8_t* const target, uint8_t* const edgeCandidateMap, CV::PixelPositions* edgeCandidates, Lock* edgeCandidateLock, const unsigned int width, const unsigned int height, const unsigned int targetPaddingElements, const TFilterOutputElementType lowThreshold, const TFilterOutputElementType highThreshold, const unsigned int firstRow, const unsigned numberRows)
{
	ocean_assert(gradientDirections != nullptr && gradientMagnitudes != nullptr);
	ocean_assert(target != nullptr);
	ocean_assert(edgeCandidateMap != nullptr);
	ocean_assert(edgeCandidates != nullptr);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(lowThreshold < highThreshold);
	ocean_assert_and_suppress_unused(firstRow != 0 && firstRow + numberRows <= (height - 1u) && "The first and the last image row must be skipped!", height);

	CV::PixelPositions localEdgeCandidates;

	const unsigned int targetStrideElements = width + targetPaddingElements;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		ocean_assert(y != 0u && y < (height - 1u));

		const uint8_t* gradientDirectionsRow = gradientDirections + y * width;
		const TFilterOutputElementType* gradientMagnitudesRow = gradientMagnitudes + y * width;

		uint8_t* targetRow = target + y * targetStrideElements;
		uint8_t* edgeCandidateMapRow = edgeCandidateMap + y * width;

		// The left-most and the right-most pixels of the current row, like all border pixels, are zero by definition (first and last row have been set to zero in calling function already)
		targetRow[0] = 0u;
		edgeCandidateMapRow[0] = 0u;

		targetRow[width - 1u] = 0u;
		edgeCandidateMapRow[width - 1u] = 0u;

		for (unsigned int x = 1u; x < (width - 1u); ++x)
		{
			ocean_assert(gradientMagnitudesRow[x] >= 0);

			if (gradientMagnitudesRow[x] > lowThreshold)
			{
				// Apply non-maximum suppression using the neighbors perpendicular to direction of current gradient direction
				//
				// Local 8-neighborhood:
				//
				// 0 1 2
				// 3 4 5   gradientDirectionsRow[x] <=> 4
				// 6 7 8

				TFilterOutputElementType gradientMagnitudeNeighbor1 = 0;
				TFilterOutputElementType gradientMagnitudeNeighbor2 = 0;

				switch (gradientDirectionsRow[x])
				{
					case ED_HORIZONTAL:
						gradientMagnitudeNeighbor1 = *(gradientMagnitudesRow + x - width); // 1
						gradientMagnitudeNeighbor2 = *(gradientMagnitudesRow + x + width); // 7
						break;

					case ED_VERTICAL:
						gradientMagnitudeNeighbor1 = *(gradientMagnitudesRow + x - 1u); // 3
						gradientMagnitudeNeighbor2 = *(gradientMagnitudesRow + x + 1u); // 5
						break;

					case ED_DIAGONAL_45:
						gradientMagnitudeNeighbor1 = *(gradientMagnitudesRow + x - width - 1u); // 0
						gradientMagnitudeNeighbor2 = *(gradientMagnitudesRow + x + width + 1u); // 8
						break;

					case ED_DIAGONAL_135:
						gradientMagnitudeNeighbor1 = *(gradientMagnitudesRow + x - width + 1u); // 2
						gradientMagnitudeNeighbor2 = *(gradientMagnitudesRow + x + width - 1u); // 7
						break;

					default:
						ocean_assert(false && "Never be here");
						break;
				}

				ocean_assert(gradientMagnitudeNeighbor1 >= 0 && gradientMagnitudeNeighbor2 >= 0);

				if (gradientMagnitudesRow[x] > gradientMagnitudeNeighbor1 && gradientMagnitudesRow[x] >= gradientMagnitudeNeighbor2)
				{
					if (gradientMagnitudesRow[x] > highThreshold)
					{
						// Value of current pixel exceeds the high threshold, so mark it as a strong edge
						edgeCandidateMapRow[x] = 255u;
						targetRow[x] = 255u;

						// Store indices of pixels which are strong part of an edge (used as seed points for edge tracing)
						ocean_assert(targetRow + x >= target && targetRow + x < target + y * targetStrideElements + width);
						localEdgeCandidates.emplace_back(x, y);
					}
					else
					{
						// Value of current pixel is in range between the low and high threshold, so mark it as a weak edge. It will be revisited later during edge tracing and target pixel will be changed to 255, if applicable
						edgeCandidateMapRow[x] = 128u;
						targetRow[x] = 0u;
					}

					continue;
				}
			}

			edgeCandidateMapRow[x] = 0u;
			targetRow[x] = 0u;
		}
	}

	if (!localEdgeCandidates.empty())
	{
		const OptionalScopedLock scopedLock(edgeCandidateLock);
		edgeCandidates->insert(edgeCandidates->end(), localEdgeCandidates.begin(), localEdgeCandidates.end());
	}
}

} // namespace CV

} // namespace Ocean
