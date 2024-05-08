/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DBackgroundNode.h"

#include "ocean/rendering/SkyBackground.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DBackgroundNode::X3DBackgroundNode(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBindableNode(environment),
	skyColor_(RGBAColor(0, 0, 0))
{
	// nothing to do here
}

void X3DBackgroundNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "groundAngle", groundAngle_, ACCESS_GET_SET);
	registerField(specification, "groundColor", groundColor_, ACCESS_GET_SET);
	registerField(specification, "skyAngle", skyAngle_, ACCESS_GET_SET);
	registerField(specification, "skyColor", skyColor_, ACCESS_GET_SET);
	registerField(specification, "transparency", transparency_, ACCESS_GET_SET);

	X3DBindableNode::registerFields(specification);
}

void X3DBackgroundNode::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DBindableNode::onInitialize(scene, timestamp);

	try
	{
		Rendering::SkyBackgroundRef skyBackground(renderingObject());
		if (skyBackground)
		{
			if (groundAngle_.values().size() + 1 == groundColor_.values().size())
			{
				bool valid = true;

				Rendering::SkyBackground::ColorPairs pairs;
				pairs.push_back(Rendering::SkyBackground::ColorPair(Scalar(0), groundColor_.values().front()));

				for (size_t n = 0; n < groundAngle_.values().size(); ++n)
				{
					if (pairs.back().first >= groundAngle_.values()[n])
					{
						valid = false;
						break;
					}

					pairs.push_back(Rendering::SkyBackground::ColorPair(groundAngle_.values()[n], groundColor_.values()[n + 1]));
				}

				if (valid)
					skyBackground->setGroundColors(pairs);
			}

			if (skyAngle_.values().size() + 1 == skyColor_.values().size())
			{
				bool valid = true;

				Rendering::SkyBackground::ColorPairs pairs;
				pairs.push_back(Rendering::SkyBackground::ColorPair(Scalar(0), skyColor_.values().front()));

				for (size_t n = 0; n < skyAngle_.values().size(); ++n)
				{
					if (pairs.back().first >= skyAngle_.values()[n])
					{
						valid = false;
						break;
					}

					pairs.push_back(Rendering::SkyBackground::ColorPair(skyAngle_.values()[n], skyColor_.values()[n + 1]));
				}

				if (valid)
					skyBackground->setSkyColors(pairs);
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}
}

void X3DBackgroundNode::onFieldChanged(const std::string& fieldName)
{
	try
	{
		Rendering::SkyBackgroundRef skyBackground(renderingObject());
		if (skyBackground)
		{
			if ((fieldName == "groundAngle" || fieldName == "groundColor") && groundAngle_.values().size() + 1 == groundColor_.values().size())
			{
				bool valid = true;

				Rendering::SkyBackground::ColorPairs pairs;
				pairs.push_back(Rendering::SkyBackground::ColorPair(Scalar(0), groundColor_.values().front()));

				for (size_t n = 0; n < groundAngle_.values().size(); ++n)
				{
					if (pairs.back().first <= groundAngle_.values()[n])
					{
						valid = false;
						break;
					}

					pairs.push_back(Rendering::SkyBackground::ColorPair(groundAngle_.values()[n], groundColor_.values()[n + 1]));
				}

				if (valid)
					skyBackground->setGroundColors(pairs);

				return;
			}

			if ((fieldName == "skyAngle" || fieldName == "skyColor") && skyAngle_.values().size() + 1 == skyColor_.values().size())
			{
				bool valid = true;

				Rendering::SkyBackground::ColorPairs pairs;
				pairs.push_back(Rendering::SkyBackground::ColorPair(Scalar(0), skyColor_.values().front()));

				for (size_t n = 0; n < skyAngle_.values().size(); ++n)
				{
					if (pairs.back().first <= skyAngle_.values()[n])
					{
						valid = false;
						break;
					}

					pairs.push_back(Rendering::SkyBackground::ColorPair(skyAngle_.values()[n], skyColor_.values()[n + 1]));
				}

				if (valid)
					skyBackground->setSkyColors(pairs);

				return;
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DBindableNode::onFieldChanged(fieldName);
}

}

}

}

}
