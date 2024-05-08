/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/blob/Loader.h"

#include "ocean/io/FileConfig.h"

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

bool Loader::loadFile(const std::string& file, CV::Detector::Blob::BlobFeatures& features)
{
	IO::FileConfig config(file);

	if (config["Descriptorelements"](0) != 64)
	{
		return false;
	}

	if (!config.exist("Features"))
	{
		return false;
	}

	Config::Value& groupFeatures = config["Features"];
	if (groupFeatures.type() != Config::TYPE_GROUP)
	{
		return false;
	}

	const unsigned int numberFeatures = groupFeatures.values("Feature");

	for (unsigned int nFeature = 0; nFeature < numberFeatures; ++nFeature)
	{
		CV::Detector::Blob::BlobFeature newFeature;

		Config::Value& groupFeature = groupFeatures.value("Feature", nFeature);
		if (groupFeature.type() != Config::TYPE_GROUP)
		{
			continue;
		}

		Config::Value& valuePosition = groupFeature["Position"];
		if (valuePosition.type() != Config::TYPE_MULTI_NUMBER)
		{
			continue;
		}

		const std::vector<double> positions(valuePosition(std::vector<double>()));
		if (positions.size() != 3)
		{
			continue;
		}

		newFeature.setPosition(Vector3(Scalar(positions[0]), Scalar(positions[1]), Scalar(positions[2])));

		Config::Value& valueLaplace = groupFeature["Laplace"];
		if (valueLaplace.type() != Config::TYPE_BOOL)
		{
			continue;
		}

		newFeature.setLaplace(valueLaplace(false));

		Config::Value& valueOrientation = groupFeature["Orientation"];
		if (valueOrientation.type() == Config::TYPE_NUMBER)
		{
			newFeature.setOrientation(Scalar(valueOrientation(0)));
		}

		Config::Value& valueStrength = groupFeature["Strength"];
		if (valueStrength.type() == Config::TYPE_NUMBER)
		{
			newFeature.setStrength(Scalar(valueStrength(0)));
		}

		Config::Value& valueScale = groupFeature["Scale"];
		if (valueScale.type() == Config::TYPE_NUMBER)
		{
			newFeature.setScale(Scalar(valueScale(0)));
		}

		Config::Value& valueDescriptor = groupFeature["Descriptor"];
		if (valueDescriptor.type() == Config::TYPE_MULTI_NUMBER)
		{
			std::vector<double> descriptors(valueDescriptor(std::vector<double>()));
			if (descriptors.size() == newFeature.descriptor().elements())
			{
				for (size_t nElement = 0; nElement < descriptors.size(); ++nElement)
				{
					newFeature.descriptor()[(unsigned int)(nElement)] = CV::Detector::Blob::BlobDescriptor::DescriptorElement(descriptors[nElement]);
				}
			}
			else
			{
				ocean_assert(false && "Missing implementation!");
				continue;
			}
		}

		newFeature.setDescriptorType(CV::Detector::Blob::BlobFeature::DESCRIPTOR_ORIENTED);
		newFeature.setOrientationType(CV::Detector::Blob::BlobFeature::ORIENTATION_SLIDING_WINDOW);

		features.push_back(newFeature);
	}

	return true;
}

}

}

}
