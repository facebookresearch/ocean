// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignment.h"

namespace Ocean
{

namespace XRPlayground
{

bool MapAlignment::writeTransformationToStream(const HomogenousMatrix4& world_T_camera, const bool slamBased, IO::OutputBitstream& bitstream)
{
	const uint64_t tag = slamBased ? transformationSlamTag_ : transformationRelocTag_;

	if (!bitstream.write<unsigned long long>(tag))
	{
		return false;
	}

	const HomogenousMatrixD4 worldD_T_cameraD(world_T_camera);

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	return true;
}

bool MapAlignment::writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.empty() || objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(objectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPoints.size())))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF;
		objectPointsF.reserve(objectPoints.size());
		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsF.emplace_back(float(objectPoint.x()), float(objectPoint.y()), float(objectPoint.z()));
		}

		if (!bitstream.write(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!bitstream.write(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool MapAlignment::readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& transformation, bool& slamBased)
{
	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag))
	{
		return false;
	}

	if (tag == transformationSlamTag_)
	{
		slamBased = true;
	}
	else if (tag == transformationRelocTag_)
	{
		slamBased = false;
	}
	else
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	HomogenousMatrixD4 transformationD;
	if (!bitstream.read(transformationD.data(), sizeof(double) * 16))
	{
		return false;
	}

	transformation = HomogenousMatrix4(transformationD);

	return transformation.isValid();
}

bool MapAlignment::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	constexpr unsigned int maximalObjectPoints = 100u * 1000u;

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > maximalObjectPoints)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		objectPoints.resize(numberObjectPoints);

		if (!bitstream.read(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF(numberObjectPoints);
		if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		objectPoints.clear();
		objectPoints.reserve(objectPointsF.size());

		for (VectorF3& objectPointF : objectPointsF)
		{
			objectPoints.emplace_back(Scalar(objectPointF.x()), Scalar(objectPointF.y()), Scalar(objectPointF.z()));
		}
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

}

}
