// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/VRHandVisualizer.h"

#include "ocean/rendering/Utilities.h"

#include <VrApi_Input.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

VRHandVisualizer::ScopedState::ScopedState(VRHandVisualizer& vrHandVisualizer)
{
	if (vrHandVisualizer.isValid())
	{
		vrHandVisualizer_ = &vrHandVisualizer;

		wasShown_ = vrHandVisualizer.isShown();
		transparency_ = vrHandVisualizer.transparency();
	}
}

VRHandVisualizer::ScopedState::ScopedState(ScopedState&& scopedState)
{
	*this = std::move(scopedState);
}

VRHandVisualizer::ScopedState::~ScopedState()
{
	release();
}

void VRHandVisualizer::ScopedState::release()
{
	if (vrHandVisualizer_ != nullptr)
	{
		if (wasShown_)
		{
			vrHandVisualizer_->show();
		}
		else
		{
			vrHandVisualizer_->hide();
		}

		vrHandVisualizer_->setTransparency(transparency_);

		vrHandVisualizer_ = nullptr;
		transparency_ = defaultTransparency_;
	}

	wasShown_ = false;
}

VRHandVisualizer::ScopedState& VRHandVisualizer::ScopedState::operator=(ScopedState&& scopedState)
{
	if (this != &scopedState)
	{
		release();

		vrHandVisualizer_ = scopedState.vrHandVisualizer_;
		scopedState.vrHandVisualizer_ = nullptr;

		wasShown_ = scopedState.wasShown_;
		scopedState.wasShown_ = false;

		transparency_ = scopedState.transparency_;
		scopedState.transparency_ = defaultTransparency_;
	}

	return *this;
}

bool VRHandVisualizer::setTransparency(const Scalar transparency)
{
	ocean_assert(transparency >= 0 && transparency <= 1);
	if (transparency < 0 || transparency > 1)
	{
		return false;
	}

	if (transparency_ == transparency)
	{
		return true;
	}

	transparency_ = transparency;

	if (leftMaterial_)
	{
		leftMaterial_->setTransparency(transparency_);
	}

	if (rightMaterial_)
	{
		rightMaterial_->setTransparency(transparency_);
	}

	return true;
}

Scalar VRHandVisualizer::transparency() const
{
	return transparency_;
}

void VRHandVisualizer::visualizeHandsInWorld(const HandPoses& handPoses, const bool show)
{
	for (bool leftHand : {true, false})
	{
		// Disable visibility for now and re-enable again later, if applicable
		Rendering::GeometryRef& handMeshGeometry = leftHand ? leftHandMeshGeometry_ : rightHandMeshGeometry_;
		handMeshGeometry->setVisible(false);

		if (isShown_ == false || show == false)
		{
			continue;
		}

		// The current pose of the hand joints
		HomogenousMatrices4 world_T_jointsCurrent;
		if (!handPoses.getHandPose(leftHand, world_T_jointsCurrent))
		{
			continue;
		}

		// The inverse of the rest poses (bind matrices) of the hand joints which are required to compute the updates of the mesh vertices
		HomogenousMatrices4& jointRestPoses_T_rootJoint = leftHand ? leftJointRestPoses_T_rootJoint_ : rightJointRestPoses_T_rootJoint_;
		if (jointRestPoses_T_rootJoint.empty())
		{
			const HomogenousMatrices4& world_T_jointRestPoses = handPoses.getHandRestPose(leftHand);

			jointRestPoses_T_rootJoint.reserve(world_T_jointRestPoses.size());
			for (const HomogenousMatrix4& world_T_jointRestPose : world_T_jointRestPoses)
			{
				jointRestPoses_T_rootJoint.emplace_back(world_T_jointRestPose.inverted());
			}
		}

		if (world_T_jointsCurrent.empty() || world_T_jointsCurrent.size() > size_t(ovrHand_MaxBones) || world_T_jointsCurrent.size() != jointRestPoses_T_rootJoint.size())
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		// Skinning step:
		// * prepare the transformations which map from the current i-th joint, to which the origin/root is set, to world coordinates
		// * compute the locations of the mesh vertices as a linear combination of 1 to 4 joint transformations (from step above)
		HomogenousMatrices4 world_T_currentJointAsRoot;
		world_T_currentJointAsRoot.reserve(jointRestPoses_T_rootJoint.size());
		for (size_t i = 0; i < jointRestPoses_T_rootJoint.size(); ++i)
		{
			world_T_currentJointAsRoot.emplace_back(world_T_jointsCurrent[i] * jointRestPoses_T_rootJoint[i]);
		}

		const HandMesh& handMesh = leftHand ? leftHandMesh_ : rightHandMesh_;

		if (!handMesh.isValid())
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		const Vectors3& vertices = handMesh.vertices();
		const Vectors3& normals = handMesh.normals();
		const BlendIndices4 blendIndices = handMesh.blendIndices();
		const Vectors4& blendWeights = handMesh.blendWeights();

		Vectors3 updatedVertices;
		updatedVertices.reserve(vertices.size());

		Vectors3 updatedNormals;
		updatedNormals.reserve(normals.size());

		ocean_assert(vertices.size() == normals.size());
		for (size_t iN = 0; iN < vertices.size(); ++iN)
		{
			Vector3 updatedVertex(0, 0, 0);
			Vector3 updatedNormal(0, 0, 0);

			const BlendIndex4& blendIndex = blendIndices[iN];
			const Vector4& blendWeight = blendWeights[iN];

			for (size_t iD = 0; iD < 4; ++iD)
			{
				if (blendIndex[iD] == invalidBlendIndex || size_t(blendIndex[iD]) >= world_T_currentJointAsRoot.size() || blendWeight[iD] <= 0)
				{
					// The current blend index isn't used, so skip it
					continue;
				}

				updatedVertex += world_T_currentJointAsRoot[blendIndex[iD]] * vertices[iN] * blendWeight[iD];
				updatedNormal += world_T_currentJointAsRoot[blendIndex[iD]] * normals[iN] * blendWeight[iD];
			}

			updatedVertices.emplace_back(updatedVertex);
			updatedNormals.emplace_back(updatedNormal);
		}

		ocean_assert(updatedVertices.size() == vertices.size());
		ocean_assert(updatedNormals.size() == normals.size());

		Rendering::VertexSetRef& vertexSet = leftHand ? leftVertexSet_ : rightVertexSet_;
		vertexSet->setVertices(updatedVertices);
		vertexSet->setNormals(updatedNormals);

		handMeshGeometry->setVisible(true);
	}
}

bool VRHandVisualizer::loadHandMeshes(ovrMobile* ovr)
{
	if (!engine_ || !framebuffer_ || ovr == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (!scene_.isNull() || !leftHandMeshGeometry_.isNull() || !rightHandMeshGeometry_.isNull())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(scene_.isNull());
	scene_ = engine_->factory().createScene();
	framebuffer_->addScene(scene_);

	const float alpha = 1.0f - float(transparency_);

	for (const ovrHandedness handedness : {VRAPI_HAND_LEFT, VRAPI_HAND_RIGHT})
	{
		ovrHandMesh ovrMesh;
		ovrMesh.Header.Version = ovrHandVersion_1;
		if (vrapi_GetHandMesh(ovr, handedness, &ovrMesh.Header) != ovrSuccess)
		{
			ocean_assert(false && "Never be here");
			return false;
		}

		ocean_assert(ovrMesh.NumVertices != 0u);
		ocean_assert(ovrMesh.NumVertices <= ovrHand_MaxVertices);

		Vectors3 vertices;
		Vectors3 normals;
		Rendering::TextureCoordinates textureCoordinates;
		BlendIndices4 blendIndices;
		Vectors4 blendWeights;

		vertices.reserve(ovrMesh.NumVertices);
		normals.reserve(ovrMesh.NumVertices);
		blendIndices.reserve(ovrMesh.NumVertices);
		blendWeights.reserve(ovrMesh.NumVertices);

		for (uint32_t iN = 0u; iN < ovrMesh.NumVertices; ++iN)
		{
			vertices.emplace_back(Scalar(ovrMesh.VertexPositions[iN].x), Scalar(ovrMesh.VertexPositions[iN].y), Scalar(ovrMesh.VertexPositions[iN].z));

			normals.emplace_back(Scalar(ovrMesh.VertexNormals[iN].x), Scalar(ovrMesh.VertexNormals[iN].y), Scalar(ovrMesh.VertexNormals[iN].z));
			ocean_assert(normals.back().isUnit());

			textureCoordinates.emplace_back(Scalar(ovrMesh.VertexUV0[iN].x), Scalar(ovrMesh.VertexUV0[iN].y));

			// Blend indices can be < 0, which means that index isn't used
			blendIndices.emplace_back(
				ovrMesh.BlendIndices[iN].x < 0 ? invalidBlendIndex : Index32(ovrMesh.BlendIndices[iN].x),
				ovrMesh.BlendIndices[iN].y < 0 ? invalidBlendIndex : Index32(ovrMesh.BlendIndices[iN].y),
				ovrMesh.BlendIndices[iN].z < 0 ? invalidBlendIndex : Index32(ovrMesh.BlendIndices[iN].z),
				ovrMesh.BlendIndices[iN].w < 0 ? invalidBlendIndex : Index32(ovrMesh.BlendIndices[iN].w)
			);

			blendWeights.emplace_back(Scalar(ovrMesh.BlendWeights[iN].x), Scalar(ovrMesh.BlendWeights[iN].y), Scalar(ovrMesh.BlendWeights[iN].z), Scalar(ovrMesh.BlendWeights[iN].w));
		}

		ocean_assert(ovrMesh.NumIndices != 0u && ovrMesh.NumIndices % 3u == 0u);

		Rendering::TriangleFaces triangles;
		triangles.reserve(ovrMesh.NumIndices / 3u);
		for (uint32_t iN = 0u; iN < ovrMesh.NumIndices; iN += 3u)
		{
			ocean_assert(ovrMesh.Indices[iN + 0u] != ovrMesh.Indices[iN + 1u]);
			ocean_assert(ovrMesh.Indices[iN + 1u] != ovrMesh.Indices[iN + 2u]);
			ocean_assert(ovrMesh.Indices[iN + 2u] != ovrMesh.Indices[iN + 0u]);
			triangles.emplace_back(uint32_t(ovrMesh.Indices[iN + 0u]), uint32_t(ovrMesh.Indices[iN + 1u]), uint32_t(ovrMesh.Indices[iN + 2u]));
		}

		HandMesh& handMesh = handedness == VRAPI_HAND_LEFT ? leftHandMesh_ : rightHandMesh_;
		ocean_assert(!handMesh.isValid());

		handMesh = HandMesh(std::move(vertices), std::move(normals), std::move(triangles), std::move(textureCoordinates), std::move(blendIndices), std::move(blendWeights));
		ocean_assert(handMesh.isValid());

		// Create the mesh; the resulting TransformRef (return value) isn't required and, hence, ignored.
		Rendering::VertexSetRef& vertexSet = handedness == VRAPI_HAND_LEFT ? leftVertexSet_ : rightVertexSet_;
		Rendering::GeometryRef& handMeshGeometry = handedness == VRAPI_HAND_LEFT ? leftHandMeshGeometry_ : rightHandMeshGeometry_;

		Rendering::AttributeSetRef attributeSet;

		ocean_assert(handMeshGeometry.isNull());
		Rendering::Utilities::createMesh(engine_, handMesh.vertices(), handMesh.triangles(), RGBAColor(0.75f, 0.75f, 0.75f, alpha), handMesh.normals(), Rendering::TextureCoordinates(), Frame(), Media::FrameMediumRef(), &attributeSet, &handMeshGeometry, &vertexSet);

		Rendering::MaterialRef& handMaterial = handedness == VRAPI_HAND_LEFT ? leftMaterial_ : rightMaterial_;

		handMaterial = attributeSet->attribute(Rendering::Object::TYPE_MATERIAL);
		ocean_assert(handMaterial);

		if (!handMeshGeometry.isNull())
		{
			handMeshGeometry->setVisible(false);
			scene_->addChild(handMeshGeometry);
		}
		else
		{
			ocean_assert(false && "Never be here!");
			return false;
		}
	}

	return true;
}

}

}

}

}

}

}
