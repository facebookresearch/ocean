/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/VRHandVisualizer.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

VRHandVisualizer::ScopedState::ScopedState(VRHandVisualizer& vrHandVisualizer)
{
	if (vrHandVisualizer.isValid())
	{
		vrHandVisualizer_ = &vrHandVisualizer;

		wasShown_ = vrHandVisualizer.isShown();
		handColor_ = vrHandVisualizer.handColor_;
		transparency_ = vrHandVisualizer.transparency();
		renderMode_ = vrHandVisualizer.renderMode();
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
		vrHandVisualizer_->setRenderMode(renderMode_);

		vrHandVisualizer_ = nullptr;
		handColor_ = RGBAColor(0.7f, 0.7f, 0.7f);
		transparency_ = ScopedState::defaultTransparency_;
		renderMode_ = RM_DEFAULT;
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

		handColor_ = scopedState.handColor_;
		scopedState.handColor_ = RGBAColor(0.7f, 0.7f, 0.7f);

		transparency_ = scopedState.transparency_;
		scopedState.transparency_ = ScopedState::defaultTransparency_;

		renderMode_ = scopedState.renderMode_;
		scopedState.renderMode_ = RM_DEFAULT;
	}

	return *this;
}

const char* VRHandVisualizer::partPlatform_ =
	// very important that '#version' is the very first thing in the string (without any empty line, tab etc in front or at the back), otherwise some devices like Samsung S20 will throw errors when compiling the shaders
	R"SHADER(#version 300 es

		#define OCEAN_LOWP lowp
		#define OCEAN_HIGHP highp
	)SHADER";

const char* VRHandVisualizer::partVertexShaderTexture_ =
	R"SHADER(
		// Projection matrix
		uniform mat4 projectionMatrix;

		// Model view matrix
		uniform mat4 modelViewMatrix;

		// Normal matrix which is the inversed and transposed of the upper left 3x3 model view matrix
		uniform mat3 normalMatrix;

		/// The vertex and normal skin matrices
		uniform mat4 jointMatrices[32];

		// Vertex attribute
		in OCEAN_LOWP vec4 aVertex;

		// Normal attribute
		in OCEAN_LOWP vec3 aNormal;

		in uvec4 aJointIndices;
		in OCEAN_LOWP vec4 aJointWeights;

		out OCEAN_LOWP vec4 vVertexInCamera;

		out OCEAN_LOWP vec3 vNormal;

		vec3 multiplyRotation(mat4 jointMatrix, vec3 normal)
		{
			return vec3(jointMatrix[0].x * normal.x + jointMatrix[1].x * normal.y + jointMatrix[2].x * normal.z,
							jointMatrix[0].y * normal.x + jointMatrix[1].y * normal.y + jointMatrix[2].y * normal.z,
							jointMatrix[0].z * normal.x + jointMatrix[1].z * normal.y + jointMatrix[2].z * normal.z);
		}

		void main(void)
		{
			OCEAN_LOWP vec4 vertex = jointMatrices[aJointIndices.x] * aVertex * aJointWeights.x
										+ jointMatrices[aJointIndices.y] * aVertex * aJointWeights.y
										+ jointMatrices[aJointIndices.z] * aVertex * aJointWeights.z
										+ jointMatrices[aJointIndices.w] * aVertex * aJointWeights.w;

			OCEAN_LOWP vec3 normal = multiplyRotation(jointMatrices[aJointIndices.x], aNormal) * aJointWeights.x
										+ multiplyRotation(jointMatrices[aJointIndices.y], aNormal) * aJointWeights.y
										+ multiplyRotation(jointMatrices[aJointIndices.z], aNormal) * aJointWeights.z
										+ multiplyRotation(jointMatrices[aJointIndices.w], aNormal) * aJointWeights.w;

			gl_Position = projectionMatrix * modelViewMatrix * vertex;

			vVertexInCamera = modelViewMatrix * vertex;
			vNormal = normalize(normalMatrix * normal);
		}
	)SHADER";

const char* VRHandVisualizer::partFragmentShader_ =
	R"SHADER(
		in OCEAN_LOWP vec4 vVertexInCamera;

		in OCEAN_LOWP vec3 vNormal;

		/// The vertex and normal skin matrices
		uniform vec4 diffuseColor;

		// The out fragment color
		out OCEAN_LOWP vec4 fragColor;

		void main()
		{
			OCEAN_LOWP vec3 lightVector = vec3(0, 0, 1);

			OCEAN_LOWP vec3 resultingColor = diffuseColor.xyz * max(0.0, dot(vNormal, lightVector));

			fragColor = vec4(resultingColor, diffuseColor.a);
		}
	)SHADER";

bool VRHandVisualizer::setTransparency(const Scalar transparency)
{
	ocean_assert(transparency >= 0 && transparency <= 1);
	if (transparency < 0 || transparency > 1)
	{
		return false;
	}

	transparency_ = transparency;

	return true;
}

Scalar VRHandVisualizer::transparency() const
{
	return transparency_;
}

bool VRHandVisualizer::setRenderMode(const RenderMode renderMode)
{
	const ScopedLock scopedLock(lock_);

	renderMode_ = renderMode;

	return true;
}

VRHandVisualizer::RenderMode VRHandVisualizer::renderMode() const
{
	const ScopedLock scopedLock(lock_);

	return renderMode_;
}

bool VRHandVisualizer::visualizeHands(const HandPoses& handPoses)
{
	const ScopedLock scopedLock(lock_);

	if (engine_.isNull() || framebuffer_.isNull())
	{
		ocean_assert(false && "VRHandVisualizer not initialized");
		return false;
	}

	if (scene_.isNull())
	{
		scene_ = engine_->factory().createScene();
		framebuffer_->addScene(scene_);
	}

	scene_->setVisible(isShown_);

	if (transformJoints_)
	{
		transformJoints_->setVisible(false);
	}

	if (renderMode_ & RM_JOINTS)
	{
		if (!visalizeJoints(handPoses))
		{
			return false;
		}
	}

	if (groupWireFrame_)
	{
		groupWireFrame_->setVisible(false);
	}

	if (renderMode_ & RM_WIREFRAME)
	{
		if (!visualizeWireframe(handPoses))
		{
			return false;
		}
	}

	if (groupMesh_)
	{
		groupMesh_->setVisible(false);
	}

	if (renderMode_ & RM_MESH)
	{
		if (!visualizeMesh(handPoses))
		{
			return false;
		}
	}

	return true;
}

void VRHandVisualizer::release()
{
	groupMesh_.release();

	for (size_t handIndex = 0; handIndex < HandPoses::numberHands_; ++handIndex)
	{
		geometriesMesh_[handIndex].release();
		shaderProgramsMesh_[handIndex].release();
	}

	groupWireFrame_.release();

	transformJoints_.release();
	vertexSetJoints_.release();

	scene_.release();

	framebuffer_.release();
	engine_.release();
}

bool VRHandVisualizer::visalizeJoints(const HandPoses& handPoses)
{
	ocean_assert(engine_ && scene_);

	const Scalar axisLength = Scalar(0.01); // 1cm

	TemporaryScopedLock scopedLock(handPoses.lock());

	HomogenousMatrices4 world_T_joints;

	for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
	{
		const HandPoses::Pose& pose = handPoses.pose(handIndex);

		if (!pose.isValid())
		{
			continue;
		}

		HomogenousMatrices4 world_T_handJoints;
		if (pose.jointTransformations(world_T_handJoints))
		{
			if (world_T_joints.empty())
			{
				std::swap(world_T_joints, world_T_handJoints);
			}
			else
			{
				world_T_joints.insert(world_T_joints.cend(), world_T_handJoints.cbegin(), world_T_handJoints.cend());
			}
		}
	}

	scopedLock.release();

	if (world_T_joints.empty())
	{
		return true;
	}

	if (transformJoints_.isNull())
	{
		transformJoints_ = Rendering::Utilities::createCoordinateSystems(*engine_, world_T_joints, axisLength, &vertexSetJoints_);

		scene_->addChild(transformJoints_);
	}
	else
	{
		Vectors3 vertices;
		vertices.reserve(world_T_joints.size() * 6);

		for (const HomogenousMatrix4& world_T_joint : world_T_joints)
		{
			const Vector3 translation = world_T_joint.translation();

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_joint * Vector3(axisLength, 0, 0));

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_joint * Vector3(0, axisLength, 0));

			vertices.emplace_back(translation);
			vertices.emplace_back(world_T_joint * Vector3(0, 0, axisLength));
		}

		ocean_assert(vertexSetJoints_);
		vertexSetJoints_->setVertices(vertices);
	}

	transformJoints_->setVisible(true);

	return true;
}

bool VRHandVisualizer::visualizeWireframe(const HandPoses& handPoses)
{
	ocean_assert(engine_ && scene_);

	if (groupWireFrame_.isNull())
	{
		groupWireFrame_ = engine_->factory().createGroup();
		scene_->addChild(groupWireFrame_);
	}

	groupWireFrame_->clear();

	const ScopedLock scopedLock(handPoses.lock());

	Vectors3 vertices;

	int16_t secondHandIndexOffset = 0;

	for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
	{
		const HandPoses::Mesh& mesh = handPoses.mesh(handIndex);
		const HandPoses::Pose& pose = handPoses.pose(handIndex);

		if (!mesh.isValid())
		{
			return false;
		}

		if (!pose.isValid())
		{
			continue;
		}

		Vectors3 handVertices;
		if (pose.meshVertices(mesh, handVertices))
		{
			if (vertices.empty())
			{
				std::swap(vertices, handVertices);
			}
			else
			{
				ocean_assert(vertices.size() == handVertices.size());

				secondHandIndexOffset = int16_t(handVertices.size());

				vertices.insert(vertices.cend(), handVertices.cbegin(), handVertices.cend());
			}
		}
	}

	if (vertices.empty())
	{
		return true;
	}

	Rendering::TriangleFaces triangleFaces;

	for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
	{
		const int16_t indexOffset = handIndex == 0 ? 0 : secondHandIndexOffset;

		const HandPoses::Mesh& mesh = handPoses.mesh(handIndex);

		triangleFaces.reserve(mesh.indices().size() * 2);

		ocean_assert(mesh.indices().size() % 3 == 0);
		for (size_t n = 0; n < mesh.indices().size(); n += 3)
		{
			const Index32 index0 = indexOffset + mesh.indices()[n + 0];
			const Index32 index1 = indexOffset + mesh.indices()[n + 1];
			const Index32 index2 = indexOffset + mesh.indices()[n + 2];

			ocean_assert(index0 < vertices.size());
			ocean_assert(index1 < vertices.size());
			ocean_assert(index2 < vertices.size());

			triangleFaces.emplace_back(index0, index1, index2);
		}
	}

	const float alpha = 1.0f - float(transparency_);
	const RGBAColor handColor(handColor_.red(), handColor_.green(), handColor_.blue(), alpha);

	groupWireFrame_->addChild(Rendering::Utilities::createLines(*engine_, vertices, triangleFaces, handColor));
	groupWireFrame_->setVisible(true);

	return true;
}

bool VRHandVisualizer::visualizeMesh(const HandPoses& handPoses)
{
	ocean_assert(engine_ && scene_);

	const ScopedLock scopedLock(handPoses.lock());

	if (groupMesh_.isNull())
	{
		for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
		{
			const HandPoses::Mesh& mesh = handPoses.mesh(handIndex);

			if (!mesh.isValid())
			{
				return false;
			}
		}

		groupMesh_ = engine_->factory().createGroup();
		scene_->addChild(groupMesh_);

		for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
		{
			const HandPoses::Mesh& mesh = handPoses.mesh(handIndex);

			Rendering::VertexSetRef vertexSet = engine_->factory().createVertexSet();

			vertexSet->setVertices(mesh.vertices());
			vertexSet->setNormals(mesh.normals());

			static_assert(sizeof(VectorT4<uint16_t>) == sizeof(XrVector4sFB), "Invalid data type!");
			static_assert(sizeof(VectorF4) == sizeof(XrVector4f), "Invalid data type!");

			vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute<VectorT4<uint16_t>>("aJointIndices", (const VectorT4<uint16_t>*)(mesh.blendIndices().data()), mesh.blendIndices().size());
			vertexSet.force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute<VectorF4>("aJointWeights", (const VectorF4*)(mesh.blendWeights().data()), mesh.blendWeights().size());

			Rendering::TrianglesRef triangles = engine_->factory().createTriangles();
			triangles->setVertexSet(std::move(vertexSet));

			Rendering::TriangleFaces triangleFaces;
			triangleFaces.reserve(mesh.indices().size() / 3);

			ocean_assert(mesh.indices().size() % 3 == 0);
			for (size_t n = 0; n < mesh.indices().size(); n += 3)
			{
				triangleFaces.emplace_back(Index32(mesh.indices()[n + 0]), Index32(mesh.indices()[n + 1]), Index32(mesh.indices()[n + 2]));
			}

			triangles->setFaces(triangleFaces);

			ocean_assert(!shaderProgramsMesh_[handIndex]);
			shaderProgramsMesh_[handIndex] = engine_->factory().createShaderProgram();

			const std::vector<const char*> vertexShaderCode = {partPlatform_, partVertexShaderTexture_};
			const std::vector<const char*> fragmentShaderCode = {partPlatform_, partFragmentShader_};

			std::string errorMessage;
			if (!shaderProgramsMesh_[handIndex]->setShader(Rendering::ShaderProgram::SL_GLSL, vertexShaderCode, fragmentShaderCode, errorMessage))
			{
				Log::error() << "Failed to create shader: " << errorMessage;

				shaderProgramsMesh_[handIndex].release();
				groupMesh_.release();
				return false;
			}

			Rendering::AttributeSetRef attributeSet = engine_->factory().createAttributeSet();
			attributeSet->addAttribute(shaderProgramsMesh_[handIndex]);
			attributeSet->addAttribute(engine_->factory().createBlendAttribute());

			ocean_assert(!geometriesMesh_[handIndex]);
			geometriesMesh_[handIndex] = engine_->factory().createGeometry();

			geometriesMesh_[handIndex]->addRenderable(triangles, std::move(attributeSet));

			groupMesh_->addChild(geometriesMesh_[handIndex]);
		}
	}

	for (size_t handIndex = 0; handIndex < handPoses.numberHands_; ++handIndex)
	{
		geometriesMesh_[handIndex]->setVisible(false);

		const HandPoses::Pose& pose = handPoses.pose(handIndex);

		if (pose.isValid())
		{
			const HandPoses::Mesh& mesh = handPoses.mesh(handIndex);

			HomogenousMatrices4 world_T_handJoints;
			if (pose.jointTransformations(mesh, world_T_handJoints))
			{
				geometriesMesh_[handIndex]->setVisible(true);

				ocean_assert(world_T_handJoints.size() <= 32);
				if (!shaderProgramsMesh_[handIndex]->setParameter("jointMatrices", world_T_handJoints))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				const float alpha = 1.0f - float(transparency_);
				const VectorF4 handColor(handColor_.red(), handColor_.green(), handColor_.blue(), alpha);

				if (!shaderProgramsMesh_[handIndex]->setParameter("diffuseColor", handColor))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}
			}

			groupMesh_->setVisible(true);
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
