/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/IndexedLineSet.h"
#include "ocean/scenedescription/sdx/x3d/Coordinate.h"

#include "ocean/rendering/LineStrips.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

IndexedLineSet::IndexedLineSet(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DGeometryNode(environment),
	colorPerVertex_(true)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;

	renderingObject_ = engine()->factory().createLineStrips();

	vertexSet_ = engine()->factory().createVertexSet();
}

IndexedLineSet::NodeSpecification IndexedLineSet::specifyNode()
{
	NodeSpecification specification("IndexedLineSet");

	registerField(specification, "set_colorIndex", setColorIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "set_coordIndex", setCoordIndex_, FieldAccessType(ACCESS_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "attrib", attrib_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "color", color_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "coord", coord_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "fogCoord", fogCoord_, FieldAccessType(ACCESS_GET_SET | ACCESS_EXPLICIT_NOTIFICATION));
	registerField(specification, "colorIndex", colorIndex_, ACCESS_NONE);
	registerField(specification, "colorPerVertex", colorPerVertex_, ACCESS_NONE);
	registerField(specification, "coordIndex", coordIndex_, ACCESS_NONE);

	X3DGeometryNode::registerFields(specification);

	return specification;
}

void IndexedLineSet::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DGeometryNode::onInitialize(scene, timestamp);

	apply();
}

void IndexedLineSet::onFieldChanged(const std::string& /*fieldName*/)
{
	ocean_assert(false && "Missing implementation.");
}

bool IndexedLineSet::onFieldChanging(const std::string& /*fieldName*/, const Field& /*field*/)
{
	ocean_assert(false && "Missing implementation.");
	return false;
}

void IndexedLineSet::apply()
{
	if (renderingObject_.isNull())
	{
		renderingObject_ = engine()->factory().createLineStrips();
	}

	const Rendering::LineStripsRef renderingLineStrips(renderingObject_);
	if (renderingLineStrips.isNull() || vertexSet_.isNull())
	{
		return;
	}

	if (coord_.value().isNull() || coord_.value()->hasField("point") == false)
	{
		Log::warning() << "IndexedFaceSet holds no coordinates.";
		return;
	}

	const MultiVector3::Values& points = coord_.value()->field<MultiVector3>("point").values();

	Rendering::VertexIndices strip;
	strip.reserve(16);

	Rendering::VertexIndexGroups strips;
	strips.reserve(16);

	const MultiInt::Values& coordinateIndices = coordIndex_.values();

	for (size_t n = 0; n < coordinateIndices.size(); ++n)
	{
		const int coordinateIndex = coordinateIndices[n];

		if (coordinateIndex >= int(points.size()))
		{
			Log::error() << "Invalid coordinate index \"" << coordinateIndex << "\" in IndexLineSet, there are only " << points.size() << " vertices defined.";
			renderingObject_.release();

			return;
		}
		else if (coordinateIndex < 0)
		{
			if (!strip.empty())
			{
				strips.emplace_back(std::move(strip));
			}

			strip.clear();
		}
		else
		{
			strip.emplace_back(Rendering::VertexIndex(coordinateIndex));
		}
	}

	if (!strip.empty())
	{
		strips.emplace_back(std::move(strip));
	}

	if (strips.empty())
	{
		renderingObject_.release();
		return;
	}

	vertexSet_->setVertices(points);

	if (color_.value())
	{
		vertexSet_->setColors(color_.value()->field<MultiColor>("color").values());
	}

	renderingLineStrips->setVertexSet(vertexSet_);
	renderingLineStrips->setStrips(strips);
}

size_t IndexedLineSet::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
