// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSLibrary.h"
#include "ocean/media/vrs/StereoImageProvider.h"

#include "ocean/io/vrs/Reader.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

std::string nameVRSLibrary()
{
	return std::string("VRS");
}

std::shared_ptr<StereoImageProviderInterface> createStereoImageProvider(const std::string& vrsFilename, const std::string& posesFilename)
{
	return std::make_shared<Media::VRS::StereoImageProvider>(vrsFilename, posesFilename);
}

int translateRecordableTypeid(const std::string& recordableTypeId)
{
	if (recordableTypeId == std::string("PoseRecordableClass"))
	{
		return int(vrs::RecordableTypeId::PoseRecordableClass);
	}

	if (recordableTypeId == std::string("AnnotationRecordableClass"))
	{
		return int(vrs::RecordableTypeId::AnnotationRecordableClass);
	}

	ocean_assert(false && "Missing implementation!");
	return -1;
}

bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, std::map<double, std::string>& values)
{
	IO::VRS::Reader reader;

	IO::VRS::PlayableString playableValues;

	if (!reader.addPlayable(&playableValues, vrs::RecordableTypeId(recordableTypeId), name))
	{
		return false;
	}

	if (reader.read(vrsFilename) < 1)
	{
		return false;
	}

	values = std::move(playableValues.stringMap());

	return true;
}

bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, SampleMap<HomogenousMatrixD4>& values)
{
	IO::VRS::Reader reader;

	IO::VRS::PlayableHomogenousMatrix4 playableValues;

	if (!reader.addPlayable(&playableValues, vrs::RecordableTypeId(recordableTypeId), name))
	{
		return false;
	}

	if (reader.read(vrsFilename) < 1)
	{
		return false;
	}

	values = std::move(playableValues.homogenousMatrix4Map());

	return true;
}

bool extractValuesFromVRS(const std::string& vrsFilename, const int recordableTypeId, const std::string& name, std::map<double, HomogenousMatricesD4>& values)
{
	IO::VRS::Reader reader;

	IO::VRS::PlayableHomogenousMatrices4 playableValues;

	if (!reader.addPlayable(&playableValues, vrs::RecordableTypeId(recordableTypeId), name))
	{
		return false;
	}

	if (reader.read(vrsFilename) < 1)
	{
		return false;
	}

	values = std::move(playableValues.homogenousMatrices4Map());

	return true;
}

#ifdef OCEAN_RUNTIME_STATIC

void registerVRSLibrary()
{
	VRSLibrary::registerLibrary();
}

bool unregisterVRSLibrary()
{
	return VRSLibrary::unregisterLibrary();
}

#endif // OCEAN_RUNTIME_STATIC

}

}

}
