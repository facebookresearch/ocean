/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplication.h"

#include "ocean/math/Cylinder3.h"
#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"

#include "ocean/platform/meta/quest/openxr/HandGestures.h"

#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Utilities.h"

GraphsApplication::GraphsApplication(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// let's configure some functions/graphs so that we can use them during the experience
	configureGraphs();

	renderModes_ =
	{
		RenderMode(RM_MESH | RM_OPAQUE),
		RenderMode(RM_MESH | RM_TRANSPARENT),
		RenderMode(RM_MESH | RM_OPAQUE | RM_NORMALS),
		RenderMode(RM_WIREFRAME | RM_OPAQUE),
		RenderMode(RM_WIREFRAME | RM_OPAQUE | RM_NORMALS)
	};

	renderModeIndex_ = 0;
}

XrSpace GraphsApplication::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void GraphsApplication::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	ocean_assert(engine_ && framebuffer_);

	// initializing the VR table menu
	vrTableMenu_ = OpenXR::Application::VRTableMenu(engine_, framebuffer_);

	const Rendering::SceneRef scene = engine_->factory().createScene();
	framebuffer_->addScene(scene);

	renderingTransformGraph_ = engine_->factory().createTransform();
	scene->addChild(renderingTransformGraph_);

	// we create one Transform to simpler update meshes later
	renderingTransformGraphMesh_ = engine_->factory().createTransform();
	renderingTransformGraph_->addChild(renderingTransformGraphMesh_);

	// we create a tiny sphere which will be used to visualize an intersection on the graph
	renderingTransformIntersection_ = Rendering::Utilities::createSphere(engine_, Scalar(0.0025), RGBAColor(1.0f, 0.0, 0.0f));
	renderingTransformIntersection_->setVisible(false);
	renderingTransformGraph_->addChild(renderingTransformIntersection_);

	// we create a small arrow to visualize the pointing direction of the left index finger
	renderingTransformDirection_ = Rendering::Utilities::createArrow(engine_, Scalar(0.05), Scalar(0.005), Scalar(0.002), RGBAColor(1.0f, 1.0f, 1.0f, 0.75f));
	renderingTransformDirection_->setVisible(false);
	scene->addChild(renderingTransformDirection_);

	// we create a Transform node holding all gradient descent paths
	renderingTransformGradientDescent_ = engine_->factory().createTransform();
	renderingTransformGradientDescent_->setVisible(false);
	renderingTransformGraph_->addChild(renderingTransformGradientDescent_);

	// we create a Text node to show which function is actually rendered
	Rendering::TransformRef transformText = Rendering::Utilities::createText(*engine_, " No function selected ", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shared*/, 0 /*fixedWidth*/, Scalar(0.05) /*fixedHeight*/, Scalar(0), Rendering::Text::AlignmentMode::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextFunction_);
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.25), Scalar(-0.5))));
	renderingTransformGraph_->addChild(transformText);

	world_T_graph_ = HomogenousMatrix4(Vector3(0, Scalar(-0.4), Scalar(-0.75))); // we place the graph in front of the user
	renderingTransformGraph_->setTransformation(world_T_graph_);
}

void GraphsApplication::onFramebufferReleasing()
{
	vrTableMenu_ = OpenXR::Application::VRTableMenu();

	renderingTransformGraph_.release();
	renderingTransformGraphMesh_.release();
	renderingTransformIntersection_.release();
	renderingTransformDirection_.release();
	renderingTransformGradientDescent_.release();
	renderingTextFunction_.release();

	VRNativeApplicationAdvanced::onFramebufferReleasing();
}

void GraphsApplication::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (selectedGraphIndex_ == size_t(-1))
	{
		// by default, we select and the first graph

		if (updateGraph(0))
		{
			selectedGraphIndex_ = 0;
			updateMenu_ = true;
		}
	}

	if (updateMenu_)
	{
		// let's update the menu so that the user knows which graph is currently selected

		updateMenu_ = false;

		OpenXR::Application::VRTableMenu::Entries entries;
		entries.reserve(graphs_.size());

		for (size_t n = 0; n < graphs_.size(); ++n)
		{
			std::string selectionString = "[  ] ";

			if (n == selectedGraphIndex_)
			{
				selectionString = "[x] ";
			}

			// the menu entry is first: the menu text, and second: the index of the graph (which can be parsed when the user selects the menu entry)
			entries.emplace_back(selectionString + graphs_[n].functionDescription(), String::toAString(n));
		};

		const OpenXR::Application::VRTableMenu::Group group("Select a graph ...", std::move(entries));

		vrTableMenu_.setMenuEntries(group);

		vrTableMenu_.show(HomogenousMatrix4(Vector3(0, Scalar(0.2), Scalar(-1.4)))); // place the menu 1.4 meter in front of the user
	}

	if (vrTableMenu_.isShown())
	{
		std::string menuEntryUrl;
		if (vrTableMenu_.onPreRender(trackedController(), predictedDisplayTime, menuEntryUrl))
		{
			// the user has made a menu selection, let's figure out which graph we need to visualize

			int graphIndex = -1;
			if (String::isInteger32(menuEntryUrl, &graphIndex) && graphIndex >= 0 && graphIndex < int(graphs_.size()))
			{
				if (updateGraph(size_t(graphIndex)))
				{
					selectedGraphIndex_ = size_t(graphIndex);
				}

				updateMenu_ = true;
			}
		}
	}

	renderGradientDescentPaths();
}

void GraphsApplication::renderGradientDescentPaths()
{
	// here we just check whether the user wants to visualize some Gradient Descent paths

	renderingTransformIntersection_->setVisible(false);
	renderingTransformDirection_->setVisible(false);

	renderingTransformGradientDescent_->clear();

	if (selectedGraphIndex_ >= graphs_.size())
	{
		return;
	}

	const Graph& graph = graphs_[selectedGraphIndex_];

	const HomogenousMatrix4 graph_T_world(world_T_graph_.inverted());

	// first, we check whether the user wants to interact with the left hand

	const Vectors3& worldLeftJointPoints = handPoses_.jointPositions(0);

	if (worldLeftJointPoints.size() == XR_HAND_JOINT_COUNT_EXT)
	{
		Line3 worldRay;
		if (OpenXR::HandGestures::isHandPointing(worldLeftJointPoints, worldRay))
		{
			renderingTransformDirection_->setTransformation(HomogenousMatrix4(worldRay.point(), Rotation(Vector3(0, 1, 0), worldRay.direction())));
			renderingTransformDirection_->setVisible(true);

			const Vector3 graphRayTip = (graph_T_world * worldRay.point(1)) * graph.graph_s_metric_;
			const Vector3 graphRayPoint = (graph_T_world * worldRay.point(0)) * graph.graph_s_metric_;

			const Line3 graphRay = Line3(graphRayPoint, (graphRayTip - graphRayPoint).normalizedOrZero());

			Vector3 graphIntersection;
			if (graph.intersection(graphRay, graphIntersection))
			{
				if (graph.isVertexVisible(graphIntersection))
				{
					Vectors3 gradientDescentPath = graph.determineGradientDescentPath(graphIntersection.x(), graphIntersection.z());

					if (gradientDescentPath.size() >= 2)
					{
						constexpr Scalar pointSize = Scalar(2);

						for (Vector3& pathPoint : gradientDescentPath)
						{
							pathPoint *= graph.metric_s_graph_;

							pathPoint.y() += Scalar(0.001); // adding 1mm to ensure that the path is always visible
						}

						renderingTransformGradientDescent_->addChild(Rendering::Utilities::createPoints(*engine_, gradientDescentPath, RGBAColor(0.0f, 1.0f, 0.0f), pointSize));
						renderingTransformGradientDescent_->setVisible(true);
					}

					renderingTransformIntersection_->setTransformation(HomogenousMatrix4(graphIntersection * graph.metric_s_graph_));
					renderingTransformIntersection_->setVisible(true);
				}
			}
		}
	}

	// now, we check whether the user wants to interact with the right hand

	const Vectors3& worldRightJointPoints = handPoses_.jointPositions(1);

	if (worldRightJointPoints.size() == XR_HAND_JOINT_COUNT_EXT)
	{
		Indices32 fingerTipIndices = {XR_HAND_JOINT_THUMB_TIP_EXT, XR_HAND_JOINT_INDEX_TIP_EXT, XR_HAND_JOINT_MIDDLE_TIP_EXT, XR_HAND_JOINT_RING_TIP_EXT, XR_HAND_JOINT_LITTLE_TIP_EXT};

		Line3 worldRay;
		if (OpenXR::HandGestures::isHandPointing(worldRightJointPoints, worldRay))
		{
			// in case the user is pointing with the index finger, we only use the index finger
			fingerTipIndices = {XR_HAND_JOINT_INDEX_TIP_EXT};
		}

		for (const Index32 fingerTipIndex : fingerTipIndices)
		{
			const Vector3 graphFingerTip = (graph_T_world * worldRightJointPoints[fingerTipIndex]) * graph.graph_s_metric_;

			const Scalar yGraph = graph.determineGraphValue(graphFingerTip.x(), graphFingerTip.z());

			if (graph.isVertexVisible(Vector3(graphFingerTip.x(), yGraph, graphFingerTip.z())))
			{
				Vectors3 gradientDescentPath = graph.determineGradientDescentPath(graphFingerTip.x(), graphFingerTip.z());

				if (gradientDescentPath.size() >= 2)
				{
					constexpr Scalar pointSize = Scalar(2);

					for (Vector3& pathPoint : gradientDescentPath)
					{
						pathPoint *= graph.metric_s_graph_;

						pathPoint.y() += Scalar(0.001); // adding 1mm to ensure that the path is always visible
					}

					renderingTransformGradientDescent_->addChild(Rendering::Utilities::createPoints(*engine_, gradientDescentPath, RGBAColor(0.0f, 1.0f, 0.0f), pointSize));
					renderingTransformGradientDescent_->setVisible(true);
				}
			}
		}
	}
}

void GraphsApplication::onButtonReleased(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp)
{
	// let's react on relase events for the Y and B button

	if ((buttons & OpenXR::TrackedController::BT_LEFT_Y) == OpenXR::TrackedController::BT_LEFT_Y
			|| (buttons & OpenXR::TrackedController::BT_RIGHT_B) == OpenXR::TrackedController::BT_RIGHT_B)
	{
		if (selectedGraphIndex_ != size_t(-1))
		{
			// we simply iterate through all pre-defined render modes

			ocean_assert(!renderModes_.empty());
			renderModeIndex_ = (renderModeIndex_ + 1) % renderModes_.size();

			updateGraph(selectedGraphIndex_);
		}
	}
}

void GraphsApplication::configureGraphs()
{
	// just some selected 3D graphs, y = f(x, z)

	{
		// 1 - 0.5 * (x^2 + z^2)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return Scalar(1) - Scalar(0.5) * (x * x + z * z);
		};

		const std::string functionText = "y = 1 - 0.5 * (x^2 + z^2)";
		const std::string functionDescription = "Parabolic Bowl";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.1);
		constexpr size_t graphBins = 50;
		constexpr Scalar maximalGraphRadius = graphExpansion;
		constexpr Scalar maximalGraphHeight = 0; // any height

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// sin(x)

		const Function function = [](const ScalarAD& x, const ScalarAD& /*z*/) -> ScalarAD
		{
			return ScalarAD::sin(x);
		};

		const std::string functionText = "y = sin(x)";
		const std::string functionDescription = "Sinusoidal Wave";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.1);
		constexpr size_t graphBins = 50;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// sin(x) + sin(-z)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return ScalarAD::sin(x) + ScalarAD::sin(-z);
		};

		const std::string functionText = "y = sin(x) + sin(-z)";
		const std::string functionDescription = "Double Sinusoidal";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.1);
		constexpr size_t graphBins = 50;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// cos(|x| + |z|)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return ScalarAD::cos(ScalarAD::abs(x) + ScalarAD::abs(z));
		};

		const std::string functionText = "y = cos(|x| + |z|)";
		const std::string functionDescription = "Wave Pattern";

		constexpr Scalar graphExpansion = Scalar(5);
		constexpr Scalar metric_s_graph = Scalar(0.05);
		constexpr size_t graphBins = 100;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// 1 - sqrt(x^2 + z^2)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return Scalar(1) - ScalarAD::sqrt(x * x + z * z);
		};

		const std::string functionText = "y = 1 - sqrt(x^2 + z^2)";
		const std::string functionDescription = "Cone";

		constexpr Scalar graphExpansion = Scalar(2);
		constexpr Scalar metric_s_graph = Scalar(0.125);
		constexpr size_t graphBins = 100;
		constexpr Scalar maximalGraphRadius = Scalar(2);
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// 0.4^2 - (0.6 - sqrt((x^2 + z^2) / 2))^2

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			const ScalarAD intermediate = Scalar(0.4 * 0.4) - ScalarAD::sqr(Scalar(0.6) - ScalarAD::sqrt((x * x + z * z) * Scalar(0.5)));

			if (intermediate.value() <= 0)
			{
				return ScalarAD(0);
			}

			return ScalarAD::sqrt(intermediate);
		};

		const std::string functionText = "y = 0.4^2 - (0.6 - sqrt((x^2 + z^2)/2))^2";
		const std::string functionDescription = "Torus";

		constexpr Scalar graphExpansion = Scalar(2);
		constexpr Scalar metric_s_graph = Scalar(0.125);
		constexpr size_t graphBins = 200;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// z^2 + 3 * x^2 * e^(-z^2 - x^2)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return (z * z + 3 * x * x) * ScalarAD::exp(-z * z - x * x);
		};

		const std::string functionText = "y = z^2 + 3 * x^2 * e^(-z^2 - x^2)";
		const std::string functionDescription = "Gaussian Surface";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.1);
		constexpr size_t graphBins = 50;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// sin(5 * (x^2 + z^2)) / 5

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return ScalarAD::sin(Scalar(5) * (x * x + z * z)) * Scalar(0.2);
		};

		const std::string functionText = "y = sin(5 * (x^2 + z^2)) / 5";
		const std::string functionDescription = "Ripple Pattern";

		constexpr Scalar graphExpansion = Scalar(2);
		constexpr Scalar metric_s_graph = Scalar(0.125);
		constexpr size_t graphBins = 200;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// sin(5 * x^2 + z^2) / 5

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return ScalarAD::sin(Scalar(5) * x * x + z * z) * Scalar(0.2);
		};

		const std::string functionText = "y = sin(5 * x^2 + z^2) / 5";
		const std::string functionDescription = "Distorted Pattern";

		constexpr Scalar graphExpansion = Scalar(2);
		constexpr Scalar metric_s_graph = Scalar(0.125);
		constexpr size_t graphBins = 200;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = 0;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// 0.5 + 1 / (x^2 + z^2)

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			ScalarAD denominator = x * x + z * z;

			// we need to ensure that we don't divide by zero
			ocean_assert(denominator.value() >= Scalar(0));
			if (denominator.value() < Numeric::weakEps())
			{
				denominator = ScalarAD(Numeric::weakEps(), denominator.derivative());
			}

			return Scalar(0.5) + Scalar(-1) / denominator;
		};

		const std::string functionText = "y = 0.5 + 1 / (x^2 + z^2)";
		const std::string functionDescription = "Hyperbolic Surface";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.1);
		constexpr size_t graphBins = 100;
		constexpr Scalar maximalGraphRadius = 0;
		constexpr Scalar maximalGraphHeight = Scalar(5);

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}

	{
		// x * z^3 - z * x^3

		const Function function = [](const ScalarAD& x, const ScalarAD& z) -> ScalarAD
		{
			return x * z * z * z - z * x * x * x;
		};

		const std::string functionText = "y = x*z^3 - z*x^3";
		const std::string functionDescription = "Saddle Surface";

		constexpr Scalar graphExpansion = Scalar(2.5);
		constexpr Scalar metric_s_graph = Scalar(0.15);
		constexpr size_t graphBins = 100;
		constexpr Scalar maximalGraphRadius = graphExpansion;
		constexpr Scalar maximalGraphHeight = 1.5;

		graphs_.emplace_back(function, functionText, functionDescription, graphExpansion, metric_s_graph, graphBins, maximalGraphRadius, maximalGraphHeight);
	}
}

bool GraphsApplication::updateGraph(const size_t graphIndex)
{
	ocean_assert(graphIndex < graphs_.size());
	if (graphIndex >= graphs_.size())
	{
		return false;
	}

	const Graph& graph = graphs_[graphIndex];

	Vectors3 vertices;
	Rendering::TriangleFaces triangleFaces;

	Vectors3 normals;

	if (!graph.createMesh(vertices, normals, triangleFaces))
	{
		return false;
	}

	ocean_assert(vertices.size() == normals.size());

	// the graph should be scaled for better visibility, let's check whether we need to scale the vertices

	if (graph.metric_s_graph_ != Scalar(1))
	{
		for (Vector3& vertex : vertices)
		{
			vertex *= graph.metric_s_graph_;
		}
	}

	ocean_assert(renderingTransformGraphMesh_);
	renderingTransformGraphMesh_->clear();

	const RenderMode renderMode = renderModes_[renderModeIndex_];

	if (renderMode & RM_MESH)
	{
		const RGBAColor color = (renderMode & RM_TRANSPARENT) ? RGBAColor(1.0f, 1.0f, 1.0f, 0.65f) : RGBAColor(0.7f, 0.7f, 0.7f); // either transparent white, or opaque gray

		Rendering::AttributeSetRef attributeSet;
		Rendering::TransformRef transform = Rendering::Utilities::createMesh(engine_, vertices, triangleFaces, color, normals, Rendering::TextureCoordinates(), Frame(), Media::FrameMediumRef(), &attributeSet);

		// be default, the resulting mesh will not be rendered from both side, thus we need to define a primitive attribute with disabled culling and two-sided lighting enabled
		Rendering::PrimitiveAttributeRef primitiveAttribute = engine_->factory().createPrimitiveAttribute();
		primitiveAttribute->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);
		primitiveAttribute->setLightingMode(Rendering::PrimitiveAttribute::LM_TWO_SIDED_LIGHTING);
		attributeSet->addAttribute(primitiveAttribute);

		renderingTransformGraphMesh_->addChild(std::move(transform));
	}

	if (renderMode & RM_WIREFRAME)
	{
		ocean_assert((renderMode & RM_TRANSPARENT) == 0u);

		const RGBAColor color = RGBAColor(0.25f, 0.25f, 1.0f); // a lighter blue

		Rendering::TransformRef transform = Rendering::Utilities::createLines(*engine_, vertices, triangleFaces, color);
		renderingTransformGraphMesh_->addChild(std::move(transform));
	}

	if (renderMode & RM_NORMALS)
	{
		const Scalar normalLength = Scalar(0.01); // 1cm
		const RGBAColor normalColor = RGBAColor(1.0f, 1.0f, 1.0f); // white

		Rendering::TransformRef transform = Rendering::Utilities::createLines(*engine_, vertices.data(), normals.data(), vertices.size(), normalLength, normalColor);
		renderingTransformGraphMesh_->addChild(std::move(transform));
	}

	// in addition, we simply render a minimalistic coordiante system for the origin of the graph
	renderingTransformGraphMesh_->addChild(Rendering::Utilities::createCoordinateSystems(*engine_, HomogenousMatrices4(1, HomogenousMatrix4(true)), Scalar(0.3))); // with 30cm axis length

	ocean_assert(renderingTextFunction_);
	renderingTextFunction_->setText(" " + graph.functionText() + " " );

	return true;
}

Scalar GraphsApplication::Graph::determineGraphValue(const Scalar x, const Scalar z) const
{
	// y = f(x, z)

	return function_(ScalarAD(x, false), ScalarAD(z, false)).value();
}

Scalar GraphsApplication::Graph::determineGraphValueAndJacobian(const Scalar x, const Scalar z, Vector2& jacobian) const
{
	// y = f(x, z)

	// dx = df/dx, partial derivative for x
	const ScalarAD dx = function_(ScalarAD(x, true), ScalarAD(z, false));

	// dz = df/dz, partial derivative for z
	const ScalarAD dz = function_(ScalarAD(x, false), ScalarAD(z, true));

	// jacobian is a 2x1 matrix:
	// [ df/dx ]
	// [ df/dz ]

	jacobian = Vector2(dx.derivative(), dz.derivative());

#ifdef OCEAN_DEBUG
	if (Numeric::abs(dx.value()) < 10)
	{
		ocean_assert(Numeric::isWeakEqual(dx.value(), dz.value()));
	}
#endif
	return dx.value();
}

Scalar GraphsApplication::Graph::determineGraphValueAndNormal(const Scalar x, const Scalar z, Vector3& normal) const
{
	Vector2 jacobian;

	const Scalar value = determineGraphValueAndJacobian(x, z, jacobian);

	if (!jacobian.isNull())
	{
		// jacobian is a 2x1 matrix:
		// [ df/dx ]   [ slope of graph along x-axis ]
		// [ df/dz ] = [ slope of graph along z-axis ]

		const Vector3 xTangent = Vector3(1, jacobian[0], 0);
		const Vector3 zTangent = Vector3(0, jacobian[1], 1);

		normal = zTangent.cross(xTangent).normalizedOrZero();
	}
	else
	{
		normal = Vector3(0, 1, 0);
	}

	return value;
}

bool GraphsApplication::Graph::createMesh(Vectors3& vertices, Vectors3& normals, Rendering::TriangleFaces& triangleFaces) const
{
	ocean_assert(isValid());

	const size_t gridPoints = graphBins_ + 1;

	vertices.clear();
	vertices.reserve(gridPoints * gridPoints);

	normals.clear();
	normals.reserve(gridPoints * gridPoints);

	const Scalar graphBinSize = graphExpansion_ * Scalar(2) / Scalar(graphBins_);

	Scalar zGraph = -graphExpansion_;

	for (size_t zGrid = 0; zGrid < gridPoints; ++zGrid)
	{
		Scalar xGraph = -graphExpansion_;

		for (size_t xGrid = 0; xGrid < gridPoints; ++xGrid)
		{
			Vector3 normal;
			const Scalar yGraph = determineGraphValueAndNormal(xGraph, zGraph, normal);

			vertices.emplace_back(xGraph, yGraph, zGraph);
			normals.emplace_back(normal);

			xGraph += graphBinSize;
		}

		zGraph += graphBinSize;
	}

	ocean_assert(vertices.size() == gridPoints * gridPoints);

	triangleFaces.clear();
	triangleFaces.reserve(2 * graphBins_ * graphBins_);

	ocean_assert(vertices.size() == normals.size());

	Vectors3 visibleVertices;
	visibleVertices.reserve(vertices.size());

	Vectors3 visibleNormals;
	visibleNormals.reserve(vertices.size());

	Indices32 visibleVertexIndices(vertices.size(), Index32(-1));

	for (size_t n = 0; n < vertices.size(); ++n)
	{
		if (isVertexVisible(vertices[n]))
		{
			visibleVertexIndices[n] = Index32(visibleVertices.size());
			visibleVertices.emplace_back(vertices[n]);

			visibleNormals.emplace_back(normals[n]);
		}
	}

	ocean_assert(visibleVertexIndices.size() == vertices.size());
	ocean_assert(visibleVertices.size() == visibleNormals.size());

	for (size_t zGrid = 1; zGrid < gridPoints; ++zGrid)
	{
		Index32 indexTopRow = Index32((zGrid - 1) * gridPoints);
		Index32 indexBottomRow = indexTopRow + Index32(gridPoints);

		for (size_t xGrid = 1; xGrid < gridPoints; ++xGrid)
		{
			/*   T+0   T+1   T+2   T+3
			 *    X------X------X------X---
			 *    |\     |\     |\     |\
			 *    |  \   |  \   |  \   |  \
			 *    |    \ |    \ |    \ |
			 *    X------X------X------X---
			 *   B+0   B+1   B+2   B+3
			 */

			// counter clock wise, first trinagle of quad
			const Rendering::TriangleFace triangleFace0(indexTopRow + 0u, indexBottomRow + 0u, indexBottomRow + 1u);

			// counter clock wise, second trinagle of quad
			const Rendering::TriangleFace triangleFace1(indexTopRow + 0u, indexBottomRow + 1u, indexTopRow + 1u);

			ocean_assert(vertices.size() == normals.size());

			// now, because we some of the triangles may be invisible (or partially visible), we may need to adjust the triangle to ensure that we get a smooth edge at the visibility boundary

			checkTriangleVisibility(vertices, normals, visibleVertexIndices, triangleFace0, visibleVertices, visibleNormals, triangleFaces);
			checkTriangleVisibility(vertices, normals, visibleVertexIndices, triangleFace1, visibleVertices, visibleNormals, triangleFaces);

			++indexTopRow;
			++indexBottomRow;
		}
	}

	ocean_assert(visibleVertices.size() == visibleNormals.size());

	vertices = std::move(visibleVertices);
	normals = std::move(visibleNormals);

	return !triangleFaces.empty();
}

bool GraphsApplication::Graph::intersection(const Line3& ray, Vector3& intersection) const
{
	Scalar t = 0;

	const Scalar step = graph_s_metric_ * Scalar(0.001); // we check the intersection with a resolution of 1mm

	Scalar previousMetricDistance = Numeric::maxValue();

	for (size_t n = 0; n < 1000; ++n)
	{
		const Vector3 graphPoint = ray.point(t);

		const Scalar yGraph = determineGraphValue(graphPoint.x(), graphPoint.z());

		const Scalar metricDistance = metric_s_graph_ * Numeric::abs(graphPoint.y() - yGraph);

		if (metricDistance >= previousMetricDistance && metricDistance < Scalar(0.01))
		{
			intersection = Vector3(graphPoint.x(), yGraph, graphPoint.z());
			return true;
		}

		previousMetricDistance = metricDistance;

		t += step;
	}

	return false;
}

Vectors3 GraphsApplication::Graph::determineGradientDescentPath(const Scalar x, const Scalar z) const
{
	const size_t maxPathLength = 2048;

	Vectors3 path;
	path.reserve(maxPathLength);

	Scalar xIteration = x;
	Scalar zIteration = z;
	Vector2 jacobianIteration;

	constexpr Scalar stepSize = Scalar(0.005);
	constexpr Scalar minimalJacobian = Scalar(0.0001);

	while (path.size() < maxPathLength)
	{
		const Scalar yIteration = determineGraphValueAndJacobian(xIteration, zIteration, jacobianIteration);

		if (!isVertexVisible(Vector3(xIteration, yIteration, zIteration)))
		{
			break;
		}

		path.emplace_back(xIteration, yIteration, zIteration);

		// no we need to determine the new (x, z) location based on the gradient descent

		if (Numeric::abs(jacobianIteration[0]) < minimalJacobian && Numeric::abs(jacobianIteration[1]) < minimalJacobian)
		{
			// we stop if the jacobian (the partial derivatives) are close to zero - which means we have found a local minimum
			break;
		}

		xIteration -= jacobianIteration[0] * stepSize; // df/dx
		zIteration -= jacobianIteration[1] * stepSize; // df/dz
	}

	return path;
}

bool GraphsApplication::Graph::isVertexVisible(const Vector3& vertex) const
{
	if (Numeric::abs(vertex.x()) > graphExpansion_ || Numeric::abs(vertex.z()) > graphExpansion_)
	{
		return false;
	}

	if (maximalGraphHeight_ > Scalar(0))
	{
		if (Numeric::abs(vertex.y()) > maximalGraphHeight_)
		{
			return false;
		}
	}

	if (maximalGraphRadius_ > Scalar(0))
	{
		if (Vector2(vertex.x(), vertex.z()).sqr() > Numeric::sqr(maximalGraphRadius_))
		{
			return false;
		}
	}

	return true;
}

Vector3 GraphsApplication::Graph::calculateBorderVertex(const Vector3& visibleVertex, const Vector3& invisibleVertex) const
{
	ocean_assert(isVertexVisible(visibleVertex));
	ocean_assert(!isVertexVisible(invisibleVertex));

	const Vector3 direction = (visibleVertex - invisibleVertex).normalizedOrZero();
	ocean_assert(!direction.isNull());

	const Line3 ray(invisibleVertex, direction); // ray starting at the invisible vertex and pointing towards the visible vertex

	Vector3 borderVertex = invisibleVertex;

	if (maximalGraphHeight_ > Scalar(0))
	{
		if (invisibleVertex.y() > maximalGraphHeight_)
		{
			const Plane3 plane(Vector3(0, maximalGraphHeight_, 0), Vector3(0, 1, 0));

			if (!plane.intersection(ray, borderVertex))
			{
				ocean_assert(false && "This should never happen!");
			}

			return borderVertex;
		}

		if (invisibleVertex.y() < -maximalGraphHeight_)
		{
			const Plane3 plane(Vector3(0, -maximalGraphHeight_, 0), Vector3(0, 1, 0));

			if (!plane.intersection(ray, borderVertex))
			{
				ocean_assert(false && "This should never happen!");
			}

			return borderVertex;
		}
	}

	// we know that one point is invisible, so the following condition must hold
	ocean_assert(maximalGraphRadius_ > Scalar(0));
	ocean_assert(Vector2(invisibleVertex.x(), invisibleVertex.z()).sqr() > Numeric::sqr(maximalGraphRadius_));

	const Cylinder3 cylinder(Vector3(0, 0, 0), Vector3(0, 1, 0), maximalGraphRadius_, -500, 500);

	if (!cylinder.nearestIntersection(ray, borderVertex))
	{
		// this should normally not happen (unless the border vertex is almost on the boundary)
		ocean_assert_accuracy(false && "This should never happen!");
	}

	return borderVertex;
}

void GraphsApplication::Graph::checkTriangleVisibility(const Vectors3& vertices, const Vectors3& normals, const Indices32& visibleVertexIndices, const Rendering::TriangleFace& triangleFace, Vectors3& visibleVertices, Vectors3& visibleNormals, Rendering::TriangleFaces& visibleTriangleFaces) const
{
	ocean_assert(vertices.size() == normals.size());
	ocean_assert(vertices.size() == visibleVertexIndices.size());
	ocean_assert(triangleFace[0] < vertices.size() && triangleFace[1] < vertices.size() && triangleFace[2] < vertices.size());

	const Index32 index0 = triangleFace[0];
	const Index32 index1 = triangleFace[1];
	const Index32 index2 = triangleFace[2];

	const Vector3& vertex0 = vertices[index0];
	const Vector3& vertex1 = vertices[index1];
	const Vector3& vertex2 = vertices[index2];

	const Index32 visibleIndex0 = visibleVertexIndices[index0];
	const Index32 visibleIndex1 = visibleVertexIndices[index1];
	const Index32 visibleIndex2 = visibleVertexIndices[index2];

	const uint8_t isVisible0 = visibleIndex0 == Index32(-1) ? 0u : 1u;
	const uint8_t isVisible1 = visibleIndex1 == Index32(-1) ? 0u : 1u;
	const uint8_t isVisible2 = visibleIndex2 == Index32(-1) ? 0u : 1u;

	const uint8_t visibility = (isVisible0 << 0u) | (isVisible1 << 1u) | (isVisible2 << 2u);

	switch (visibility)
	{
		case 0b111:
		{
			// all vertices of the triangle are visible

			visibleTriangleFaces.emplace_back(visibleIndex0, visibleIndex1, visibleIndex2);

			break;
		}

		case 0b000:
		{
			// none of the vertices is visible

			break;
		}

		case 0b001:
		{
			// only vertex 0 is visible

			visibleTriangleFaces.emplace_back(visibleIndex0, Index32(visibleVertices.size()), Index32(visibleVertices.size() + 1));

			// simple to preserve ccw triangle: first new vertex 1, then new vertex 2
			visibleVertices.emplace_back(calculateBorderVertex(vertex0, vertex1));
			visibleVertices.emplace_back(calculateBorderVertex(vertex0, vertex2));

			visibleNormals.emplace_back(normals[index1]); // we simply use the normals from the invisible vertices
			visibleNormals.emplace_back(normals[index2]);

			break;
		}

		case 0b010:
		{
			// only vertex 1 is visible

			visibleTriangleFaces.emplace_back(visibleIndex1, Index32(visibleVertices.size()), Index32(visibleVertices.size() + 1));

			// to preserve ccw triangle: first new vertex 2, then new vertex 0
			visibleVertices.emplace_back(calculateBorderVertex(vertex1, vertex2));
			visibleVertices.emplace_back(calculateBorderVertex(vertex1, vertex0));

			visibleNormals.emplace_back(normals[index0]);
			visibleNormals.emplace_back(normals[index2]);

			break;
		}

		case 0b100:
		{
			// only vertex 2 is visible

			visibleTriangleFaces.emplace_back(visibleIndex2, Index32(visibleVertices.size()), Index32(visibleVertices.size() + 1));

			// to preserve ccw triangle: first new vertex 0, then new vertex 1
			visibleVertices.emplace_back(calculateBorderVertex(vertex2, vertex0));
			visibleVertices.emplace_back(calculateBorderVertex(vertex2, vertex1));

			visibleNormals.emplace_back(normals[index0]);
			visibleNormals.emplace_back(normals[index1]);

			break;
		}

		case 0b011:
		{
			// vertex 0 and 1 are visible

			visibleTriangleFaces.emplace_back(visibleIndex0, visibleIndex1, Index32(visibleVertices.size()));
			visibleTriangleFaces.emplace_back(visibleIndex0, Index32(visibleVertices.size()), Index32(visibleVertices.size() + 1));

			visibleVertices.emplace_back(calculateBorderVertex(vertex1, vertex2));
			visibleVertices.emplace_back(calculateBorderVertex(vertex0, vertex2));

			visibleNormals.emplace_back(normals[index2]);
			visibleNormals.emplace_back(normals[index2]);

			break;
		}

		case 0b101:
		{
			// only vertex 0 and 2 is visible

			visibleTriangleFaces.emplace_back(visibleIndex0, Index32(visibleVertices.size()), visibleIndex2);
			visibleTriangleFaces.emplace_back(visibleIndex2, Index32(visibleVertices.size()), Index32(visibleVertices.size() + 1));

			visibleVertices.emplace_back(calculateBorderVertex(vertex0, vertex1));
			visibleVertices.emplace_back(calculateBorderVertex(vertex2, vertex1));

			visibleNormals.emplace_back(normals[index1]);
			visibleNormals.emplace_back(normals[index1]);

			break;
		}

		case 0b110:
		{
			// only vertex 1 and 2 is visible

			visibleTriangleFaces.emplace_back(Index32(visibleVertices.size()), visibleIndex1, visibleIndex2);
			visibleTriangleFaces.emplace_back(Index32(visibleVertices.size()), visibleIndex2, Index32(visibleVertices.size() + 1));

			visibleVertices.emplace_back(calculateBorderVertex(vertex1, vertex0));
			visibleVertices.emplace_back(calculateBorderVertex(vertex2, vertex0));

			visibleNormals.emplace_back(normals[index0]);
			visibleNormals.emplace_back(normals[index0]);

			break;
		}

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}
}
