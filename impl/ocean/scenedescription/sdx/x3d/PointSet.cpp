/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/PointSet.h"

#include "ocean/rendering/Points.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

PointSet::PointSet(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createPoints();
	renderingVertexSet_ = engine()->factory().createVertexSet();
}

PointSet::NodeSpecification PointSet::specifyNode()
{
	NodeSpecification specification("PointSet");

	registerField(specification, "attrib", attrib_, ACCESS_GET_SET);
	registerField(specification, "color", color_, ACCESS_GET_SET);
	registerField(specification, "coord", coord_, ACCESS_GET_SET);
	registerField(specification, "fogCoord", fogCoord_, ACCESS_GET_SET);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void PointSet::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	apply();
}

void PointSet::onFieldChanged(const std::string& fieldName)
{
	try
	{
		const Rendering::PointsRef renderingPoints(renderingObject_);
		if (renderingPoints)
		{
			if (fieldName == "coord")
			{
				apply();
				return;
			}
		}
	}
	catch(const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	X3DGeometryNode::onFieldChanged(fieldName);
}

void PointSet::apply()
{
	if (renderingObject_.isNull())
	{
		renderingObject_ = engine()->factory().createPoints();
	}

	const Rendering::PointsRef renderingPoints(renderingObject_);
	if (renderingPoints.isNull() || renderingVertexSet_.isNull())
	{
		return;
	}

	if (coord_.value().isNull() || coord_.value()->hasField("point") == false)
	{
		Log::warning() << "PointSet holds no coordinates.";
		return;
	}

	const MultiVector3::Values& points = coord_.value()->field<MultiVector3>("point").values();

	const bool hasColors = bool(color_.value());
	const MultiColor::Values* colors = nullptr;
	if (hasColors)
	{
		colors = &color_.value()->field<MultiColor>("color").values();
	}

	Rendering::Vertices renderingVertices;
	RGBAColors renderingColors;

	try
	{
		renderingVertices.reserve(points.size());
		renderingVertices.insert(renderingVertices.begin(), points.begin(), points.end());

		if (hasColors)
		{
			ocean_assert(colors != nullptr);

			if (colors->size() < points.size())
			{
				Log::warning() << "PointSet: Not enough defined color values.";
			}

			renderingColors.reserve(colors->size());
			renderingColors.insert(renderingColors.begin(), colors->begin(), colors->end());
		}
	}
	catch (const Exception& exception)
	{
		Log::error() << exception.what();
		renderingObject_.release();
		return;
	}

	if (renderingVertices.empty())
	{
		renderingObject_.release();
		return;
	}

	Rendering::VertexIndices indices;
	indices.reserve(renderingVertices.size());

	for (unsigned int n = 0u; n < renderingVertices.size(); ++n)
	{
		indices.push_back(n);
	}

	renderingPoints->setIndices(indices);

	renderingVertexSet_->setVertices(renderingVertices);

	if (!renderingColors.empty())
	{
		renderingVertexSet_->setColors(renderingColors);
	}

	renderingPoints->setVertexSet(renderingVertexSet_);
}

size_t PointSet::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
