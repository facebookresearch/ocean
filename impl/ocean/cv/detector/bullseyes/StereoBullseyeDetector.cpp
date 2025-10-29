/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/detector/bullseyes/StereoBullseyeDetector.h"

#include "ocean/cv/detector/bullseyes/AssignmentSolver.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

StereoBullseyeDetector::Parameters StereoBullseyeDetector::Parameters::defaultParameters()
{
	return Parameters();
}

StereoBullseyeDetector::Candidate::Candidate(const Vector3& center, const Scalar reprojectionErrorA, const Scalar reprojectionErrorB) :
	center_(center), reprojectionErrorA_(reprojectionErrorA), reprojectionErrorB_(reprojectionErrorB)
{
	ocean_assert(isValid());
}

bool StereoBullseyeDetector::Candidate::isValid() const
{
	return center_ != invalidBullseyeCenter() && reprojectionErrorA_ >= 0 && reprojectionErrorB_ >= 0;
}

const Vector3& StereoBullseyeDetector::Candidate::center() const
{
	return center_;
}

Scalar StereoBullseyeDetector::Candidate::reprojectionErrorA() const
{
	return reprojectionErrorA_;
}

Scalar StereoBullseyeDetector::Candidate::reprojectionErrorB() const
{
	return reprojectionErrorB_;
}

Vector3 StereoBullseyeDetector::Candidate::invalidBullseyeCenter()
{
	return Vector3::minValue();
}

size_t StereoBullseyeDetector::IndexPairHash32::operator()(const IndexPair32& indexPair) const
{
	size_t seed = std::hash<Index32>{}(indexPair.first);
	seed ^= std::hash<Index32>{}(indexPair.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

bool StereoBullseyeDetector::detectBullseyes(const SharedAnyCameras& cameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters, const Parameters& parameters, Worker* worker)
{
	ocean_assert(cameras.size() == 2 && yFrames.size() == 2);
	if (cameras.size() != 2 || yFrames.size() != 2)
	{
		return false;
	}

	ocean_assert(cameras[0] != nullptr && cameras[0]->isValid() && cameras[1] != nullptr && cameras[1]->isValid());
	if (cameras[0] == nullptr || !cameras[0]->isValid() || cameras[1] == nullptr || !cameras[1]->isValid())
	{
		return false;
	}

	ocean_assert(yFrames[0].isValid() && yFrames[1].isValid());
	if (!yFrames[0].isValid() || !yFrames[1].isValid())
	{
		return false;
	}

	ocean_assert(cameras[0]->width() == yFrames[0].width() && cameras[0]->height() == yFrames[0].height());
	ocean_assert(cameras[1]->width() == yFrames[1].width() && cameras[1]->height() == yFrames[1].height());
	if (cameras[0]->width() != yFrames[0].width() || cameras[0]->height() != yFrames[0].height() || cameras[1]->width() != yFrames[1].width() || cameras[1]->height() != yFrames[1].height())
	{
		return false;
	}

	ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[0].pixelFormat(), FrameType::FORMAT_Y8));
	ocean_assert(FrameType::arePixelFormatsCompatible(yFrames[1].pixelFormat(), FrameType::FORMAT_Y8));
	if (!FrameType::arePixelFormatsCompatible(yFrames[0].pixelFormat(), FrameType::FORMAT_Y8) || !FrameType::arePixelFormatsCompatible(yFrames[1].pixelFormat(), FrameType::FORMAT_Y8))
	{
		return false;
	}

	ocean_assert(world_T_device.isValid());
	ocean_assert(device_T_cameras.size() == 2 && device_T_cameras[0].isValid() && device_T_cameras[1].isValid());
	if (!world_T_device.isValid() || device_T_cameras.size() != 2 || !device_T_cameras[0].isValid() || !device_T_cameras[1].isValid())
	{
		return false;
	}

	// Clear the output parameters. Will be filled later.
	bullseyePairs.clear();
	bullseyeCenters.clear();

	// Monocular detection of bullseyes
	// If the cameras have different resolutions, start the monocular detection on the camera with lower resolution.
	BullseyeGroup bullseyeGroup;

	const size_t lowerResolutionCameraIndex = (cameras[0]->width() * cameras[0]->height() <= cameras[1]->width() * cameras[1]->height()) ? 0 : 1;
	for (const size_t cameraIndex : {lowerResolutionCameraIndex, 1 - lowerResolutionCameraIndex})
	{
		if (!MonoBullseyeDetector::detectBullseyes(*cameras[cameraIndex], yFrames[cameraIndex], bullseyeGroup[cameraIndex], parameters, worker))
		{
			return false;
		}

		if (bullseyeGroup[cameraIndex].empty())
		{
			// Nothing found in this camera, so no need to continue.
			return true;
		}
	}

	const HomogenousMatrix4 world_T_cameraA = world_T_device * device_T_cameras[0];
	const HomogenousMatrix4 world_T_cameraB = world_T_device * device_T_cameras[1];

	const CandidateMap candidateMap = extractBullseyeCandidates(*cameras[0], *cameras[1], world_T_cameraA, world_T_cameraB, bullseyeGroup[0], bullseyeGroup[1]);

	if (!extractBullseyes(*cameras[0], *cameras[1], bullseyeGroup[0], bullseyeGroup[1], candidateMap, bullseyePairs, bullseyeCenters))
	{
		return false;
	}

	ocean_assert(bullseyePairs.size() == bullseyeCenters.size());

	return true;
}

StereoBullseyeDetector::CandidateMap StereoBullseyeDetector::extractBullseyeCandidates(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraB.isValid());

	if (bullseyesA.empty() || bullseyesB.empty())
	{
		// No matches possible.
		return CandidateMap();
	}

	// Triangulate all combinations of left and right monocular bullseyes
	CandidateMap candidateMap;
	candidateMap.reserve(bullseyesA.size() * bullseyesB.size());

	for (size_t a = 0; a < bullseyesA.size(); ++a)
	{
		for (size_t b = 0; b < bullseyesB.size(); ++b)
		{
			Vector3 bullseyeCenter;
			Scalar reprojectionErrorA;
			Scalar reprojectionErrorB;

			if (triangulateBullseye(cameraA, cameraB, world_T_cameraA, world_T_cameraB, bullseyesA[a], bullseyesB[b], bullseyeCenter, reprojectionErrorA, reprojectionErrorB))
			{
				ocean_assert(NumericT<Index32>::isInsideValueRange(a) && NumericT<Index32>::isInsideValueRange(b));
				ocean_assert(!candidateMap.contains(IndexPair32{Index32(a), Index32(b)}));
				candidateMap.emplace(IndexPair32{Index32(a), Index32(b)}, Candidate{bullseyeCenter, reprojectionErrorA, reprojectionErrorB});
			}
		}
	}

	return candidateMap;
}

bool StereoBullseyeDetector::extractBullseyes(const AnyCamera& cameraA, const AnyCamera& cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const CandidateMap& candidateMap, BullseyePairs& bullseyePairs, Vectors3& bullseyeCenters)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());

	if (bullseyesA.empty() || bullseyesB.empty() || candidateMap.empty())
	{
		return false;
	}

	bullseyePairs.clear();
	bullseyeCenters.clear();

	Matrix costMatrix;
	if (!computeCostMatrix(cameraA, cameraB, bullseyesA, bullseyesB, candidateMap, costMatrix))
	{
		return false;
	}

	AssignmentSolver::Assignments assignments;
	if (!AssignmentSolver::solve(std::move(costMatrix), assignments))
	{
		return false;
	}

	if (!assignments.empty())
	{
		bullseyePairs.reserve(assignments.size());
		bullseyeCenters.reserve(assignments.size());

		for (const AssignmentSolver::Assignment& assignment : assignments)
		{
			const Index32 indexA = assignment.first;
			const Index32 indexB = assignment.second;

			const IndexPair32 indexPair(indexA, indexB);

			ocean_assert(candidateMap.contains(indexPair));
			if (!candidateMap.contains(indexPair))
			{
				return false;
			}

			const Candidate& candidate = candidateMap.at(indexPair);

			ocean_assert(size_t(indexA) < bullseyesA.size() && size_t(indexB) < bullseyesB.size());
			if (size_t(indexA) >= bullseyesA.size() || size_t(indexB) >= bullseyesB.size())
			{
				return false;
			}

			bullseyeCenters.emplace_back(candidate.center());
			bullseyePairs.emplace_back(bullseyesA[indexA], bullseyesB[indexB]);
		}
	}

	return true;
}

bool StereoBullseyeDetector::computeCostMatrix(const AnyCamera& cameraA, const AnyCamera& cameraB, const Bullseyes& bullseyesA, const Bullseyes& bullseyesB, const CandidateMap& candidateMap, Matrix& costMatrix)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());

	if (bullseyesA.empty() || bullseyesB.empty() || candidateMap.empty())
	{
		return false;
	}

	// The camera resolutions can be different. To compare similarity of
	// bullseyes using something like their radii, their size has to be
	// normalized to the same scale.
	ocean_assert(cameraA.width() != 0u && cameraB.width() != 0u);
	const Scalar cameraB_s_cameraA = Scalar(cameraB.width()) / Scalar(cameraA.width());
	ocean_assert(cameraB_s_cameraA > 0);

	costMatrix = Matrix(bullseyesA.size(), bullseyesB.size(), invalidMatchingCost());

	for (const CandidateMap::value_type& iterator : candidateMap)
	{
		const IndexPair32& indexPair = iterator.first;

		const size_t indexA = size_t(indexPair.first);
		const size_t indexB = size_t(indexPair.second);
		ocean_assert(indexA < bullseyesA.size() && indexB < bullseyesB.size());

		const Candidate& candidate = iterator.second;

		// Reprojection cost
		const Scalar maxReprojectionError = std::max(candidate.reprojectionErrorA(), candidate.reprojectionErrorB());

		constexpr Scalar reprojectionCostOffset = 0;
		const Scalar reprojectionCost = Scalar(1) / (Scalar(1) + Numeric::exp(reprojectionCostOffset - maxReprojectionError));
		ocean_assert(reprojectionCost >= 0 && reprojectionCost <= 1);

		// Radius cost (similarity)

		const Scalar radiusA = cameraB_s_cameraA * bullseyesA[indexA].radius(); // scaled to match
		const Scalar radiusB = bullseyesB[indexB].radius();

		const Scalar radiusDifference = std::abs(radiusA - radiusB);

		constexpr Scalar radiusOffset = 0;
		const Scalar radiusCost = Scalar(1) / (Scalar(1) + Numeric::exp(radiusOffset - radiusDifference));
		ocean_assert(radiusCost >= 0 && radiusCost <= 1);

		const Scalar totalCost = Scalar(0.5) * reprojectionCost + Scalar(0.5) * radiusCost;
		ocean_assert(totalCost >= 0 && totalCost <= 1);

		costMatrix(indexA, indexB) = totalCost;
	}

	return true;
}

bool StereoBullseyeDetector::triangulateBullseye(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& world_T_cameraA, const HomogenousMatrix4& world_T_cameraB, const Bullseye& bullseyeA, const Bullseye& bullseyeB, Vector3& bullseyeCenter, Scalar& reprojectionErrorA, Scalar& reprojectionErrorB)
{
	ocean_assert(cameraA.isValid() && cameraB.isValid());
	ocean_assert(world_T_cameraA.isValid() && world_T_cameraA.isValid());
	ocean_assert(bullseyeA.isValid() && bullseyeB.isValid());

	ocean_assert(cameraA.isInside(bullseyeA.position()));
	ocean_assert(cameraB.isInside(bullseyeB.position()));

	const Line3 rayA = cameraA.ray(bullseyeA.position(), world_T_cameraA);
	const Line3 rayB = cameraB.ray(bullseyeB.position(), world_T_cameraB);

	Vector3 objectPoint;
	if (rayA.nearestPoint(rayB, objectPoint))
	{
		if (AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraA), objectPoint)
			&& AnyCamera::isObjectPointInFrontIF(AnyCamera::standard2InvertedFlipped(world_T_cameraB), objectPoint))
		{
			const Vector2 projectedObjectPointA = cameraA.projectToImage(world_T_cameraA, objectPoint);
			const Scalar projectionErrorA = projectedObjectPointA.distance(bullseyeA.position());

			const Vector2 projectedObjectPointB = cameraB.projectToImage(world_T_cameraB, objectPoint);
			const Scalar projectionErrorB = projectedObjectPointB.distance(bullseyeB.position());

			bullseyeCenter = objectPoint;
			reprojectionErrorA = projectionErrorA;
			reprojectionErrorB = projectionErrorB;

			return true;
		}
	}

	return false;
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean
