/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Factory.h"
#include "ocean/scenedescription/sdx/x3d/Appearance.h"
#include "ocean/scenedescription/sdx/x3d/AudioClip.h"
#include "ocean/scenedescription/sdx/x3d/Background.h"
#include "ocean/scenedescription/sdx/x3d/Billboard.h"
#include "ocean/scenedescription/sdx/x3d/BooleanFilter.h"
#include "ocean/scenedescription/sdx/x3d/BooleanToggle.h"
#include "ocean/scenedescription/sdx/x3d/BooleanTrigger.h"
#include "ocean/scenedescription/sdx/x3d/Box.h"
#include "ocean/scenedescription/sdx/x3d/Color.h"
#include "ocean/scenedescription/sdx/x3d/ColorInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/ComposedShader.h"
#include "ocean/scenedescription/sdx/x3d/Cone.h"
#include "ocean/scenedescription/sdx/x3d/Coordinate.h"
#include "ocean/scenedescription/sdx/x3d/CoordinateInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/Cylinder.h"
#include "ocean/scenedescription/sdx/x3d/DirectionalLight.h"
#include "ocean/scenedescription/sdx/x3d/FontStyle.h"
//#include "ocean/scenedescription/sdx/x3d/FloatVertexAttribute.h"
#include "ocean/scenedescription/sdx/x3d/Group.h"
#include "ocean/scenedescription/sdx/x3d/HeadUpTransform.h"
#include "ocean/scenedescription/sdx/x3d/ImageTexture.h"
#include "ocean/scenedescription/sdx/x3d/IndexedFaceSet.h"
#include "ocean/scenedescription/sdx/x3d/IndexedLineSet.h"
#include "ocean/scenedescription/sdx/x3d/Inline.h"
#include "ocean/scenedescription/sdx/x3d/LiveVideoTexture.h"
#include "ocean/scenedescription/sdx/x3d/LOD.h"
#include "ocean/scenedescription/sdx/x3d/Material.h"
//#include "ocean/scenedescription/sdx/x3d/Matrix3VertexAttribute.h"
//#include "ocean/scenedescription/sdx/x3d/Matrix4VertexAttribute.h"
#include "ocean/scenedescription/sdx/x3d/MovieTexture.h"
#include "ocean/scenedescription/sdx/x3d/MultiTexture.h"
#include "ocean/scenedescription/sdx/x3d/MultiTextureTransform.h"
#include "ocean/scenedescription/sdx/x3d/NavigationInfo.h"
#include "ocean/scenedescription/sdx/x3d/Normal.h"
#include "ocean/scenedescription/sdx/x3d/NormalInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/OrientationInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/PackagedShader.h"
#include "ocean/scenedescription/sdx/x3d/PhantomTextureCoordinate.h"
#include "ocean/scenedescription/sdx/x3d/PointLight.h"
#include "ocean/scenedescription/sdx/x3d/PointSet.h"
#include "ocean/scenedescription/sdx/x3d/PositionInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/PositionInterpolator2D.h"
#include "ocean/scenedescription/sdx/x3d/ProximitySensor.h"
//#include "ocean/scenedescription/sdx/x3d/ProgramShader.h"
#include "ocean/scenedescription/sdx/x3d/ShaderPart.h"
//#include "ocean/scenedescription/sdx/x3d/ShaderProgram.h"
#include "ocean/scenedescription/sdx/x3d/ScalarInterpolator.h"
#include "ocean/scenedescription/sdx/x3d/Shape.h"
#include "ocean/scenedescription/sdx/x3d/Sphere.h"
#include "ocean/scenedescription/sdx/x3d/SpotLight.h"
#include "ocean/scenedescription/sdx/x3d/StaticGroup.h"
#include "ocean/scenedescription/sdx/x3d/Switch.h"
#include "ocean/scenedescription/sdx/x3d/Text.h"
#include "ocean/scenedescription/sdx/x3d/TextureCoordinate.h"
#include "ocean/scenedescription/sdx/x3d/TextureProperties.h"
#include "ocean/scenedescription/sdx/x3d/TextureTransform.h"
#include "ocean/scenedescription/sdx/x3d/TimeSensor.h"
#include "ocean/scenedescription/sdx/x3d/TimeTrigger.h"
#include "ocean/scenedescription/sdx/x3d/TouchSensor.h"
#include "ocean/scenedescription/sdx/x3d/TrackerTransform.h"
#include "ocean/scenedescription/sdx/x3d/Transform.h"
#include "ocean/scenedescription/sdx/x3d/Viewpoint.h"
#include "ocean/scenedescription/sdx/x3d/WorldInfo.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

SDXNodeRef Factory::createNode(const std::string& type, const SDXNode::SDXEnvironment* environment)
{
	ocean_assert(!type.empty());
	ocean_assert(environment != nullptr);

	static const CreateNodeMap createNodeMap =
	{
		{"Appearance", &createNode<Appearance>},
		{"AudioClip", &createNode<AudioClip>},
		{"Background", &createNode<Background>},
		{"Billboard", &createNode<Billboard>},
		{"Box", &createNode<Box>},
		{"BooleanFilter", &createNode<BooleanFilter>},
		{"BooleanToggle", &createNode<BooleanToggle>},
		{"BooleanTrigger", &createNode<BooleanTrigger>},
		{"Color", &createNode<Color>},
		{"ColorInterpolator", &createNode<ColorInterpolator>},
		{"ComposedShader", &createNode<ComposedShader>},
		{"Cone", &createNode<Cone>},
		{"Coordinate", &createNode<Coordinate>},
		{"CoordinateInterpolator", &createNode<CoordinateInterpolator>},
		{"Cylinder", &createNode<Cylinder>},
		{"DirectionalLight", &createNode<DirectionalLight>},
		{"FontStyle", &createNode<FontStyle>},
		{"Group", &createNode<Group>},
		{"HeadUpTransform", &createNode<HeadUpTransform>},
		{"ImageTexture", &createNode<ImageTexture>},
		{"IndexedFaceSet", &createNode<IndexedFaceSet>},
		{"IndexedLineSet", &createNode<IndexedLineSet>},
		{"Inline", &createNode<Inline>},
		{"LiveVideoTexture", &createNode<LiveVideoTexture>},
		{"LOD", &createNode<LOD>},
		{"Material", &createNode<Material>},
		{"MovieTexture", &createNode<MovieTexture>},
		{"MultiTexture", &createNode<MultiTexture>},
		{"MultiTextureTransform", &createNode<MultiTextureTransform>},
		{"NavigationInfo", &createNode<NavigationInfo>},
		{"Normal", &createNode<Normal>},
		{"NormalInterpolator", &createNode<NormalInterpolator>},
		{"OrientationInterpolator", &createNode<OrientationInterpolator>},
		{"PackagedShader", &createNode<PackagedShader>},
		{"PhantomTextureCoordinate", &createNode<PhantomTextureCoordinate>},
		{"PointLight", &createNode<PointLight>},
		{"PointSet", &createNode<PointSet>},
		{"PositionInterpolator", &createNode<PositionInterpolator>},
		{"PositionInterpolator2D", &createNode<PositionInterpolator2D>},
		{"ProximitySensor", &createNode<ProximitySensor>},
		{"ShaderPart", &createNode<ShaderPart>},
		{"ScalarInterpolator", &createNode<ScalarInterpolator>},
		{"Shape", &createNode<Shape>},
		{"Sphere", &createNode<Sphere>},
		{"SpotLight", &createNode<SpotLight>},
		{"StaticGroup", &createNode<StaticGroup>},
		{"Switch", &createNode<Switch>},
		{"Text", &createNode<Text>},
		{"TextureCoordinate", &createNode<TextureCoordinate>},
		{"TextureProperties", &createNode<TextureProperties>},
		{"TextureTransform", &createNode<TextureTransform>},
		{"TimeSensor", &createNode<TimeSensor>},
		{"TimeTrigger", &createNode<TimeTrigger>},
		{"TouchSensor", &createNode<TouchSensor>},
		{"TrackerTransform", &createNode<TrackerTransform>},
		{"Transform", &createNode<Transform>},
		{"Viewpoint", &createNode<Viewpoint>},
		{"WorldInfo", &createNode<WorldInfo>}
	};

	const CreateNodeMap::const_iterator i = createNodeMap.find(type);

	if (i != createNodeMap.cend())
	{
		return i->second(environment);
	}

	return SDXNodeRef();
}

template <typename T>
SDXNodeRef Factory::createNode(const SDXNode::SDXEnvironment* environment)
{
	ocean_assert(environment != nullptr);

	T* node = new T(environment);
	ocean_assert(node != nullptr);

	return SDXNodeRef(environment->library()->nodeManager().registerNode(node));
}

}

}

}

}
