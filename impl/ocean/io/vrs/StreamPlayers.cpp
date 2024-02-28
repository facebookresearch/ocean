// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/io/vrs/StreamPlayers.h"

namespace Ocean
{

namespace IO
{

namespace VRS
{

PlayableHomogenousMatrix4::PlayableHomogenousMatrix4() :
	homogenousMatrix4Map_(size_t(-1) /* infinity capacity */)
{
	// nothing to do here
}

bool PlayableHomogenousMatrix4::onDataLayoutRead(const vrs::CurrentRecord& /*header*/, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	const vrs::DataPieceValue<vrs::Matrix4Dd>* homogenousMatrix4DataPieceValue = dataLayout.findDataPieceValue<vrs::Matrix4Dd>("homogenous_matrix_4");
	const vrs::DataPieceValue<double>* timestampDataPieceValue = dataLayout.findDataPieceValue<double>("timestamp");

	if (homogenousMatrix4DataPieceValue && timestampDataPieceValue)
	{
		const vrs::Matrix4Dd matrix = homogenousMatrix4DataPieceValue->get();

		const HomogenousMatrixD4 homogenousMatrix4((const double*)&matrix, true /* row aligned */);
		const double timestamp = timestampDataPieceValue->get();

		homogenousMatrix4Map_.insert(homogenousMatrix4, timestamp);

		return true;
	}

	return false;
}

PlayableHomogenousMatrices4::PlayableHomogenousMatrices4()
{
	// nothing to do here
}

bool PlayableHomogenousMatrices4::onDataLayoutRead(const vrs::CurrentRecord& /*header*/, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	const vrs::DataPieceVector<vrs::Matrix4Dd>* homogenousMatrices4DataPieceVector = dataLayout.findDataPieceVector<vrs::Matrix4Dd>("homogenous_matrices_4");
	const vrs::DataPieceValue<double>* timestampDataPieceValue = dataLayout.findDataPieceValue<double>("timestamp");

	if (homogenousMatrices4DataPieceVector && timestampDataPieceValue)
	{
		std::vector<vrs::Matrix4Dd> vrsMatrices4Dd;

		if (homogenousMatrices4DataPieceVector->get(vrsMatrices4Dd))
		{
			HomogenousMatricesD4 homogenousMatricesD4;
			homogenousMatricesD4.reserve(vrsMatrices4Dd.size());

			for (const vrs::Matrix4Dd& matrix : vrsMatrices4Dd)
			{
				homogenousMatricesD4.emplace_back((const double*)&matrix, true /* row aligned */);
			}

			const double timestamp = timestampDataPieceValue->get();

			// TODO Assert that timestamp is not in the map yet?

			homogenousMatrices4Map_.emplace(std::piecewise_construct, std::forward_as_tuple(timestamp), std::forward_as_tuple(homogenousMatricesD4));

			return true;
		}
	}

	return false;
}

PlayableString::PlayableString()
{
	// nothing to do here
}

bool PlayableString::onDataLayoutRead(const vrs::CurrentRecord& /*header*/, size_t /*blockIndex*/, vrs::DataLayout& dataLayout)
{
	// The use of DataLayout::findDataPieceXXX an anti-pattern, please do not clone this bad practice.
	// See D39522991 or D39527762 for proper efficient use of datalayout objects.
	const vrs::DataPieceString* stringDataPieceValue = dataLayout.findDataPieceString("string");
	const vrs::DataPieceValue<double>* timestampDataPieceValue = dataLayout.findDataPieceValue<double>("timestamp");

	if (stringDataPieceValue && timestampDataPieceValue)
	{
		std::string stringValue = stringDataPieceValue->get();

		const double timestamp = timestampDataPieceValue->get();

		stringMap_.insert(std::make_pair(timestamp, std::move(stringValue)));

		return true;
	}

	return false;
}

}

}

}
