/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/calibration/CalibrationBoard.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

Vector3 CalibrationBoard::BoardMarker::objectPoint(const Vector3& markerPosition, const Scalar xMarkerSize, const Scalar zMarkerSize, const size_t indexInMarker) const
{
	ocean_assert(indexInMarker < 25);
	ocean_assert(xMarkerSize > Numeric::eps());
	ocean_assert(zMarkerSize > Numeric::eps());

	const int x = int(indexInMarker % 5) - 2;
	const int z = int(indexInMarker / 5) - 2;

	const Scalar xScale = xMarkerSize / Scalar(5);
	const Scalar zScale = zMarkerSize / Scalar(5);

	const Vector3 markerObjectPoint(Scalar(x) * xScale, 0, Scalar(z) * zScale);

	ocean_assert(board_R_marker_.isValid());

	return markerPosition + board_R_marker_ * markerObjectPoint;
}

CV::PixelDirection CalibrationBoard::BoardMarker::neighborDirection(const BoardMarker& neighborBoardMarker) const
{
	ocean_assert(isValid());
	ocean_assert(neighborBoardMarker.isValid());

	ocean_assert(coordinate() != neighborBoardMarker.coordinate());
	ocean_assert(coordinate().isNeighbor4(neighborBoardMarker.coordinate()));

	const CV::PixelDirection direction = MarkerCoordinate::direction(coordinate_, neighborBoardMarker.coordinate());
	ocean_assert(coordinate_.neighbor(direction) == neighborBoardMarker.coordinate());

	return CV::PixelDirection(modulo<int>(int(direction) - int(orientation_), 360));
}

CalibrationBoard::MarkerCoordinate CalibrationBoard::BoardMarker::boardMarkerNeighborCoordinate(const CV::PixelDirection& neighborDirection, const size_t xMarkers, const size_t yMarkers) const
{
	ocean_assert(xMarkers >= 1 && yMarkers >= 1);
	ocean_assert(isValid());

	// the provided direction to the neighbor is relative to the provided board marker which can be oriented in any direction on it's own on the board

	const CV::PixelDirection orientedNeighborDirection = CV::PixelDirection(modulo<int>(orientation() + neighborDirection, 360));

	MarkerCoordinate neighborCoordinate;

	switch (orientedNeighborDirection)
	{
		case CV::PD_NORTH:
			neighborCoordinate = coordinate() - CV::PixelPosition(0u, 1u);
			break;

		case CV::PD_WEST:
			neighborCoordinate = coordinate() - CV::PixelPosition(1u, 0u);
			break;

		case CV::PD_SOUTH:
			neighborCoordinate = coordinate() + CV::PixelPosition(0u, 1u);
			break;

		case CV::PD_EAST:
			neighborCoordinate = coordinate() + CV::PixelPosition(1u, 0u);
			break;

		default:
			ocean_assert(false && "This should never happen!");
			return MarkerCoordinate();
	}

	if (size_t(neighborCoordinate.x()) >= xMarkers || size_t(neighborCoordinate.y()) >= yMarkers)
	{
		// neighbor would be outside
		return MarkerCoordinate();
	}

	return neighborCoordinate;
}

CalibrationBoard::CalibrationBoard(const size_t xMarkers, const size_t yMarkers, BoardMarkers&& boardMarkers) :
	xMarkers_(xMarkers),
	yMarkers_(yMarkers),
	boardMarkers_(std::move(boardMarkers))
{
	ocean_assert(xMarkers_ >= 1 && yMarkers_ >= 1);
	ocean_assert(xMarkers_ * yMarkers_ == boardMarkers_.size());

	if (xMarkers_ == 0 || yMarkers_ == 0 || xMarkers_ * yMarkers_ != boardMarkers_.size())
	{
		xMarkers_ = 0;
		yMarkers_ = 0;

		boardMarkers_.clear();

		return;
	}

	for (size_t yMarker = 0; yMarker < yMarkers_; ++yMarker)
	{
		for (size_t xMarker = 0; xMarker < xMarkers_; ++xMarker)
		{
			const BoardMarker& boardMarker = boardMarkers_[xMarker + yMarker * xMarkers_];

			MarkerCoordinates& markerCoordinates = markerIdMap_[boardMarker.markerType()];
			ocean_assert(markerCoordinates.size() < markerCoordinates.capacity());

			markerCoordinates.pushBack(MarkerCoordinate((unsigned int)(xMarker), (unsigned int)(yMarker)));
		}
	}

	hash_ = hash(*this);
}

bool CalibrationBoard::containsMarkerCandidateWithNeighborhood(const MarkerCandidates& markerCandidates, const size_t markerCandidateIndex, MarkerCoordinate* markerCoordinate, NeighborMarkerCoordinateMap* neighborMarkerCoordinateMap) const
{
	ocean_assert(!markerCandidates.empty());
	ocean_assert(markerCandidateIndex < markerCandidates.size());

	const MarkerCandidate& markerCandidate = markerCandidates[markerCandidateIndex];

	ocean_assert(markerCandidate.isValid());
	ocean_assert(markerCandidate.hasMarkerId());
	ocean_assert(markerCandidate.hasNeighborWithMarkerId(markerCandidates));

	MarkerCoordinates markerCoordinateCandidates;
	if (!containsMarkerTypeIgnoringNeighborhood(markerCandidate, &markerCoordinateCandidates))
	{
		return false;
	}

	NeighborMarkerCoordinateMap neighborMarkerCoordinateCandidates;
	neighborMarkerCoordinateCandidates.reserve(4);

	ocean_assert(!markerCoordinateCandidates.empty());
	for (size_t nCandidateCoordinate = 0; nCandidateCoordinate < markerCoordinateCandidates.size(); ++nCandidateCoordinate)
	{
		const MarkerCoordinate& markerCoordinateCandidate = markerCoordinateCandidates[nCandidateCoordinate];
		const BoardMarker& boardMarkerCandidate = marker(markerCoordinateCandidate);

		neighborMarkerCoordinateCandidates.clear();

		size_t expectedMatchedNeighbors = 0;

		for (const MarkerCandidate::NeighborMap::value_type& neighborPair : markerCandidate.neighbors())
		{
			const CV::PixelDirection neighborDirection = neighborPair.first;
			const size_t neighborIndex = neighborPair.second;

			ocean_assert(neighborIndex < markerCandidates.size());
			ocean_assert(neighborIndex != markerCandidateIndex);
			const MarkerCandidate& neighborMarkerCandidate = markerCandidates[neighborIndex];

			if (!neighborMarkerCandidate.hasMarkerId())
			{
				// the neighbor is not been identified, so the neighbor does not provide any useful information at this point
				continue;
			}

			++expectedMatchedNeighbors;

			const MarkerCoordinate neighborCoordinate = boardMarkerCandidate.boardMarkerNeighborCoordinate(neighborDirection, xMarkers(), yMarkers());

			if (!neighborCoordinate.isValid())
			{
				// no valid neighbor, so the candidate is wrong
				break;
			}

			const BoardMarker& neighborBoardMarker = marker(neighborCoordinate);

			if (neighborBoardMarker.markerId() != neighborMarkerCandidate.markerId() || neighborBoardMarker.sign() != neighborMarkerCandidate.sign())
			{
				// neighbor has different id or sign
				break;
			}

			// now let's see whether the orientation of both markers match (we checked candidate to neighbor direction above already)

			const CV::PixelDirection neighborBoardMarkerEdge = neighborBoardMarker.neighborDirection(boardMarkerCandidate);

			const CV::PixelDirection neighborMarkerCandidateEdge = neighborMarkerCandidate.neighborDirection(markerCandidateIndex);
			ocean_assert(neighborMarkerCandidateEdge != CV::PD_INVALID);

			if (neighborBoardMarkerEdge != neighborMarkerCandidateEdge)
			{
				break;
			}

			ocean_assert(neighborMarkerCoordinateCandidates.find(neighborDirection) == neighborMarkerCoordinateCandidates.cend());
			neighborMarkerCoordinateCandidates.emplace(neighborDirection, neighborCoordinate);
		}

		ocean_assert(expectedMatchedNeighbors >= 1);
		if (neighborMarkerCoordinateCandidates.size() == expectedMatchedNeighbors)
		{
			if (markerCoordinate != nullptr)
			{
				*markerCoordinate = markerCoordinateCandidate;
			}

			if (neighborMarkerCoordinateMap != nullptr)
			{
				*neighborMarkerCoordinateMap = std::move(neighborMarkerCoordinateCandidates);
			}

			return true;
		}
	}

	return false;
}

bool CalibrationBoard::createCalibrationBoard(const unsigned int seed, const size_t xMarkers, const size_t yMarkers, CalibrationBoard& calibrationBoard)
{
	ocean_assert(xMarkers >= 1 && yMarkers >= 1);
	if (xMarkers == 0 || yMarkers == 0)
	{
		return false;
	}

	BoardMarkers boardMarkers;

	if (!createUniqueBoardMarkers(seed, xMarkers, yMarkers, boardMarkers))
	{
		return false;
	}

	ocean_assert(boardMarkers.size() == xMarkers * yMarkers);

	calibrationBoard = CalibrationBoard(xMarkers, yMarkers, std::move(boardMarkers));

	return true;
}

bool CalibrationBoard::determineOptimalMarkerGrid(const double aspectRatio, const size_t minimalNumberMarkers, size_t& xOptimalMarkers, size_t& yOptimalMarkers)
{
	ocean_assert(aspectRatio > NumericD::eps());
	if (aspectRatio <= NumericD::eps())
	{
		return false;
	}

	ocean_assert(minimalNumberMarkers != 0);
	if (minimalNumberMarkers == 0)
	{
		return false;
	}

	double yMarkersD = NumericD::sqrt(double(minimalNumberMarkers) / aspectRatio);
	double xMarkersD = aspectRatio * yMarkersD;

	xMarkersD = std::max(1.0, NumericD::floor(xMarkersD));
	yMarkersD = std::max(1.0, NumericD::floor(yMarkersD));

	size_t xMarkers = 0;
	size_t yMarkers = 0;
	double bestRatio = NumericD::maxValue();

	for (size_t nOption = 0; nOption < 4; ++nOption)
	{
		const size_t xExtra = nOption & 0x01u;
		const size_t yExtra = (nOption >> 1u) & 0x01u;

		const size_t xMarkersCandidate = size_t(xMarkersD) + xExtra;
		const size_t yMarkersCandidate = size_t(yMarkersD) + yExtra;

		if (xMarkersCandidate * yMarkersCandidate < minimalNumberMarkers)
		{
			continue;
		}

		if (xMarkers != 0 && xMarkers <= xMarkersCandidate && yMarkers <= yMarkersCandidate)
		{
			break;
		}

		const double ratio = double(xMarkersCandidate) / double(yMarkersCandidate);

		if (xMarkers == 0 || NumericD::abs(ratio - aspectRatio) < NumericD::abs(bestRatio - aspectRatio))
		{
			bestRatio = ratio;

			xMarkers = xMarkersCandidate;
			yMarkers = yMarkersCandidate;
		}
	}

	if (xMarkers * yMarkers < minimalNumberMarkers)
	{
		return false;
	}

	xOptimalMarkers = xMarkers;
	yOptimalMarkers = yMarkers;

	return true;
}

bool CalibrationBoard::determineOptimalMarkerGrid(const double aspectRatio, const size_t xMarkers, size_t& yMarkers)
{
	ocean_assert(aspectRatio > NumericD::eps());
	if (aspectRatio <= NumericD::eps())
	{
		return false;
	}

	ocean_assert(xMarkers != 0);
	if (xMarkers == 0)
	{
		return false;
	}

	const double yMarkersD = double(xMarkers) / aspectRatio;

	yMarkers = size_t(std::max(1, NumericD::round32(yMarkersD)));

	return true;
}

bool CalibrationBoard::determineUniqueness(const CalibrationBoard& calibrationBoardA, const CalibrationBoard& calibrationBoardB, size_t& oneIdenticalNeighborCounter, size_t& twoIdenticalNeighborCounter, size_t& threeIdenticalNeighborCounter)
{
	NeighborMap neighborMapA;

	for (size_t yMarker = 0; yMarker < calibrationBoardA.yMarkers(); ++yMarker)
	{
		for (size_t xMarker = 0; xMarker < calibrationBoardA.xMarkers(); ++xMarker)
		{
			const BoardMarker& boardMarker = calibrationBoardA.marker(xMarker, yMarker);
			ocean_assert(boardMarker.coordinate().x() == (unsigned int)(xMarker) && boardMarker.coordinate().y() == (unsigned int)(yMarker));

			for (const CV::PixelDirection neighborDirection : {CV::PD_NORTH, CV::PD_WEST, CV::PD_SOUTH, CV::PD_EAST})
			{
				const CV::PixelPosition neighborPosition = boardMarker.coordinate().neighbor(neighborDirection);

				if (neighborPosition.x() < calibrationBoardA.xMarkers() && neighborPosition.y() < calibrationBoardA.yMarkers())
				{
					const BoardMarker& neighborBoardMarker = calibrationBoardA.marker(neighborPosition.x(), neighborPosition.y());

					const uint64_t uniqueValue = uniqueValueOrientedNeighborhood(boardMarker, neighborBoardMarker);

					neighborMapA[uniqueValue]++;
					ocean_assert(neighborMapA[uniqueValue] <= 2);
				}
			}
		}
	}

	for (const NeighborMap::value_type& neighborPair : neighborMapA)
	{
		if (neighborPair.second != 2)
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	oneIdenticalNeighborCounter = 0;
	twoIdenticalNeighborCounter = 0;
	threeIdenticalNeighborCounter = 0;

	for (size_t yMarker = 0; yMarker < calibrationBoardB.yMarkers(); ++yMarker)
	{
		for (size_t xMarker = 0; xMarker < calibrationBoardB.xMarkers(); ++xMarker)
		{
			const BoardMarker& boardMarker = calibrationBoardB.marker(xMarker, yMarker);
			ocean_assert(boardMarker.coordinate().x() == (unsigned int)(xMarker) && boardMarker.coordinate().y() == (unsigned int)(yMarker));

			size_t localNeighborCounter = 0;

			for (const CV::PixelDirection neighborDirection : {CV::PD_NORTH, CV::PD_WEST, CV::PD_SOUTH, CV::PD_EAST})
			{
				const CV::PixelPosition neighborPosition = boardMarker.coordinate().neighbor(neighborDirection);

				if (neighborPosition.x() < calibrationBoardB.xMarkers() && neighborPosition.y() < calibrationBoardB.yMarkers())
				{
					const BoardMarker& neighborBoardMarker = calibrationBoardB.marker(neighborPosition.x(), neighborPosition.y());

					const uint64_t uniqueValue = uniqueValueOrientedNeighborhood(boardMarker, neighborBoardMarker);

					if (neighborMapA.find(uniqueValue) != neighborMapA.cend())
					{
						++oneIdenticalNeighborCounter;

						++localNeighborCounter;
					}
				}
			}

			if (localNeighborCounter >= 2)
			{
				++twoIdenticalNeighborCounter;
			}

			if (localNeighborCounter >= 3)
			{
				++threeIdenticalNeighborCounter;
			}
		}
	}

	return true;
}

uint64_t CalibrationBoard::hash(const CalibrationBoard& calibrationBoard)
{
	if (!calibrationBoard.isValid())
	{
		return 0ull;
	}

	uint64_t hashValue = 0x9e3779b9ull;

	hashValue ^= std::hash<uint64_t>{}(calibrationBoard.xMarkers_) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);
	hashValue ^= std::hash<uint64_t>{}(calibrationBoard.yMarkers_) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);

	for (const BoardMarker& boardMarker : calibrationBoard.boardMarkers_)
	{
		hashValue ^= std::hash<uint64_t>{}(boardMarker.markerId()) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);

		hashValue ^= std::hash<int>{}(boardMarker.sign() ? 1 : 0) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);

		hashValue ^= std::hash<int>{}(static_cast<int>(boardMarker.orientation())) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);

		hashValue ^= std::hash<unsigned int>{}(boardMarker.coordinate().x()) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);
		hashValue ^= std::hash<unsigned int>{}(boardMarker.coordinate().y()) + 0x9e3779b9ull + (hashValue << 6) + (hashValue >> 2);
	}

	return hashValue;
}

bool CalibrationBoard::containsMarkerTypeIgnoringNeighborhood(const MarkerCandidate& markerCandidate, MarkerCoordinates* markerCoordinates) const
{
	ocean_assert(markerCandidate.isValid());
	ocean_assert(markerCandidate.hasMarkerId());

	const MarkerIdMap::const_iterator iMarker = markerIdMap_.find(markerCandidate.markerType());

	if (iMarker == markerIdMap_.cend())
	{
		return false;
	}

	if (markerCoordinates != nullptr)
	{
		*markerCoordinates = iMarker->second;
	}

	return true;
}

bool CalibrationBoard::createUniqueBoardMarkers(const unsigned int id, const size_t xMarkers, const size_t yMarkers, BoardMarkers& boardMarkers)
{
	ocean_assert(xMarkers != 0 && yMarkers != 0);
	if (xMarkers == 0 || yMarkers == 0)
	{
		return false;
	}

	ocean_assert(boardMarkers.empty());
	boardMarkers.clear();

	const size_t markerIds = Marker::numberMarkerIds();

	Indices32 positiveOptions;
	Indices32 negativeOptions;

	for (size_t nMarkerId = 0; nMarkerId < markerIds; ++nMarkerId)
	{
		for (size_t nOrientation = 0; nOrientation < 4; ++nOrientation)
		{
			ocean_assert(nMarkerId < (1u << 30));
			const Index32 option = Index32(nMarkerId << 2u | nOrientation);

			positiveOptions.push_back(option);
			negativeOptions.push_back(option);
		}
	}

	// let's create a random permutation

	RandomGenerator randomGenerator(id);

	for (unsigned int n = 1u; n < positiveOptions.size(); ++n)
	{
		const unsigned int index = RandomI::random(randomGenerator, n, (unsigned int)(positiveOptions.size() - 1u));
		std::swap(positiveOptions[n - 1u], positiveOptions[index]);
	}

	for (unsigned int n = 1u; n < negativeOptions.size(); ++n)
	{
		const unsigned int index = RandomI::random(randomGenerator, n, (unsigned int)(negativeOptions.size() - 1u));
		std::swap(negativeOptions[n - 1u], negativeOptions[index]);
	}

	// four directions, 2 signs, 26 layouts
	// never two neighbors with identical id

	UnorderedIndexSet64 neighborSet;
	neighborSet.reserve(xMarkers * yMarkers * 4);

	bool rowStartSign = true;

	for (size_t yMarker = 0; yMarker < yMarkers; ++yMarker)
	{
		bool sign = rowStartSign;

		for (size_t xMarker = 0; xMarker < xMarkers; ++xMarker)
		{
			Indices32& options = sign ? positiveOptions : negativeOptions;

			bool optionFound = false;

			for (size_t optionIndex = 0; optionIndex < options.size(); ++optionIndex)
			{
				const Index32 option = options[optionIndex];

				const Index32 orientationValue = option & 0b11;
				const Index32 markerId = option >> 2u;
				ocean_assert(markerId < markerIds);

				uint64_t uniqueNeighborValueLeft = 0ull;
				uint64_t uniqueNeighborValueTop = 0ull;

				if (xMarker > 0u)
				{
					const BoardMarker& leftNeighbor = boardMarkers.back();
					ocean_assert(leftNeighbor.coordinate().x() == (unsigned int)(xMarker - 1u) && leftNeighbor.coordinate().y() == (unsigned int)(yMarker));

					uniqueNeighborValueLeft = uniqueValue(markerId, uint32_t(leftNeighbor.markerId()));

					if (neighborSet.find(uniqueNeighborValueLeft) != neighborSet.cend())
					{
						continue;
					}
				}

				if (yMarker > 0u)
				{
					const BoardMarker& topNeighbor = boardMarkers[xMarker + (yMarker - 1u) * xMarkers];
					ocean_assert(topNeighbor.coordinate().x() == (unsigned int)(xMarker) && topNeighbor.coordinate().y() == (unsigned int)(yMarker - 1u));

					uniqueNeighborValueTop = uniqueValue(markerId, uint32_t(topNeighbor.markerId()));

					if (neighborSet.find(uniqueNeighborValueTop) != neighborSet.cend())
					{
						continue;
					}
				}

				if (uniqueNeighborValueLeft != 0ull)
				{
					neighborSet.insert(uniqueNeighborValueLeft);
				}

				if (uniqueNeighborValueTop != 0ull)
				{
					neighborSet.insert(uniqueNeighborValueTop);
				}

				ocean_assert(orientationValue >= 0 && orientationValue < 4);
				const CV::PixelDirection orientation = CV::PixelDirection(orientationValue * 90);

				const MarkerCoordinate coordinate((unsigned int)(xMarker), (unsigned int)(yMarker));

				boardMarkers.emplace_back(markerId, sign, orientation, coordinate);

				sign = !sign;

				options[optionIndex] = options.back();
				options.pop_back();

				optionFound = true;
				break;
			}

			if (!optionFound)
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}
		}

		rowStartSign = !rowStartSign;
	}

	return true;
}

uint64_t CalibrationBoard::uniqueValueOrientedNeighborhood(const BoardMarker& boardMarker, const BoardMarker& neighborBoardMarker)
{
	ocean_assert(boardMarker.isValid());
	ocean_assert(neighborBoardMarker.isValid());

	ocean_assert(boardMarker.coordinate() != neighborBoardMarker.coordinate());
	ocean_assert(boardMarker.coordinate().isNeighbor4(neighborBoardMarker.coordinate()));

	ocean_assert(boardMarker.sign() != neighborBoardMarker.sign());

	// we combine the type (marker id and the sign), and the relative orientation of the marker to the neighbor

	const int absoluteAngle = CV::PixelPosition::direction(boardMarker.coordinate(), neighborBoardMarker.coordinate());
	const CV::PixelDirection relativeEdge = CV::PixelDirection(modulo<int>(int(boardMarker.orientation()) - absoluteAngle, 360));

	const int neighborAbsoluteAngle = absoluteAngle + 180;
	const CV::PixelDirection neighborRelativeEdge = CV::PixelDirection(modulo<int>(int(neighborBoardMarker.orientation()) - neighborAbsoluteAngle, 360));

	const uint32_t relativeEdgeValue = uint32_t(relativeEdge) / 90u;
	const uint32_t neighborRelativeEdgeValue = uint32_t(neighborRelativeEdge) / 90u;

	ocean_assert(relativeEdgeValue < 4u && neighborRelativeEdgeValue < 4u);

	const BoardMarker::MarkerType markerType = boardMarker.markerType();
	const BoardMarker::MarkerType neighborMarkerType = neighborBoardMarker.markerType();

	ocean_assert(markerType != neighborMarkerType);

	ocean_assert(markerType < (1u << 30u) && neighborMarkerType < (1u << 30u));

	const uint32_t uniqueValueIdSignEdge = (markerType << 2u) | relativeEdgeValue;
	const uint32_t neighborUniqueValueIdSignEdge = (neighborMarkerType << 2u) | neighborRelativeEdgeValue;

	return uniqueValue(uniqueValueIdSignEdge, neighborUniqueValueIdSignEdge);
}

uint64_t CalibrationBoard::uniqueValue(const uint32_t valueA, const uint32_t valueB)
{
	if (valueA < valueB)
	{
		return uint64_t(valueB) << 32u | valueA;
	}
	else
	{
		return uint64_t(valueA) << 32u | valueB;
	}
}

}

}

}
