/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_GRAPHS_QUEST_GRAPHS_APPLICATION_H
#define OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_GRAPHS_QUEST_GRAPHS_APPLICATION_H

#include "application/ocean/demo/platform/meta/quest/openxr/ApplicationDemoPlatformMetaQuestOpenXR.h"

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/platform/meta/quest/openxr/application/VRTableMenu.h"

#include "ocean/math/AutomaticDifferentiation.h"

#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

using namespace Ocean;
using namespace Ocean::Platform::Meta::Quest;

/**
 * Implements a specialization of the VRApplication.
 * @ingroup applicationdemoplatformmetaquestopenxr
 */
class GraphsApplication : public Platform::Meta::Quest::OpenXR::Application::VRNativeApplicationAdvanced
{
	protected:

		/**
		 * Definition of an abbreviation for scalar automatic differentiation values.
		 */
		using ScalarAD = AutomaticDifferentiation;

		/**
		 * Definition of individual render modes.
		 */
		enum RenderMode : uint32_t
		{
			/// Invalid render mode.
			RM_INVALID = 0u,
			/// The graph is rendered with mesh.
			RM_MESH = 1u << 0u,
			/// The graph is rendered with a wireframe.
			RM_WIREFRAME = 1u << 1u,
			/// The graph's normals are rendered
			RM_NORMALS = 1u << 2u,
			/// The graph is opaque.
			RM_OPAQUE = 1u << 3u,
			/// The graph is transparent.
			RM_TRANSPARENT = 1u << 4u
		};

		/**
		 * Definition of a vector holding render modes.
		 */
		using RenderModes = std::vector<RenderMode>;

		/**
		 * Definition of a function pointer to a function which allows to calculate the value of a 3D graph 'y = f(x, z)' at a specified location.
		 * @param x The x value given as an AutomaticDifferentiation value
		 * @param z The z value given as an AutomaticDifferentiation value
		 * @return The resulting value 'y' of the 3D graph at the defined postition
		 */
		using Function = std::function<ScalarAD(const ScalarAD& x, const ScalarAD& z)>;

		/**
		 * This class implements the necessary functionalities to visualize and to intract with a 3D graph.
		 */
		class Graph
		{
			public:

				/**
				 * Creates a new graph object.
				 * @param function The actual function of the graph, must be valid
				 * @param functionText The function written as text, must be valid
				 * @param functionDescription A description of the function, must be valid
				 * @param graphExpansion The expansion of the graph, with range (0, infinity)
				 * @param metric_s_graph The scale between the graph domain and the metric domain, with range (0, infinity)
				 * @param graphBins The number of bins the graph is made of (along both axis), with range [1, infinity)
				 * @param maximalGraphRadius The maximal radius from the origin in which the graph is visualized (allowing to cutoff parts of the graph), with range (0, infinity), 0 to skip using the radius
				 * @param maximalGraphHeight The maximal height (in +/- y direction) in wich the graph is visualized (allowing to cutoff parts of the graph), with range (0, infinity), 0 to skip using the height
				 */
				inline Graph(const Function& function, const std::string& functionText, const std::string& functionDescription, const Scalar graphExpansion, const Scalar metric_s_graph, const size_t graphBins, const Scalar maximalGraphRadius, const Scalar maximalGraphHeight);

			public:

				/**
				 * Returns the graph's function as text.
				 * @return String containing the function
				 */
				inline const std::string& functionText() const;

				/**
				 * Returns a description of the graph's function.
				 * @return String containing a function description
				 */
				inline const std::string& functionDescription() const;

				/**
				 * Creats a 3D mesh for the graph.
				 * @param vertices The resulting 3D vertices of the mesh
				 * @param normals The resulting per-vertex normals of the mesh, one for each vertex
				 * @param triangleFaces The resulting triangle faces defining the mesh
				 * @return True, if succeeded
				 */
				bool createMesh(Vectors3& vertices, Vectors3& normals, Rendering::TriangleFaces& triangleFaces) const;

				/**
				 * Determins the intersection point between a ray and this graph.
				 * @param ray The ray for which the intersection will be determined, defined in the domain of the graph, must be valid
				 * @param intersection The resulting intersection between graph and ray
				 * @return True, if the the ray intersected the graph within the visibile boundary
				 */
				bool intersection(const Line3& ray, Vector3& intersection) const;

				/**
				 * Determines the gradient descent path for a specific point on the graph.
				 * @param x The x location of the point, defined in the domain of the graph
				 * @param z The z location of the point, defined in the domain of the graph
				 * @return The resulting 3D points of the gradient descent path, defined in the domain of the graph, empty if no path could be calculated
				 */
				Vectors3 determineGradientDescentPath(const Scalar x, const Scalar z) const;

				/**
				 * Determines the function value at a specified (x, z) location.
				 * @param x The x coordinate of the location, with range (-infinity, infinity)
				 * @param z The z coordinate of the location, with range (-infinity, infinity)
				 * @return The resulting function value (the y value)
				 * @see determineGraphValueAndNormal().
				 */
				Scalar determineGraphValue(const Scalar x, const Scalar z) const;

				/**
				 * Returns whether a given vertex is visible or not (visible because inside the visiblity boundary).
				 * @param vertex The vertex to be checked, defined in the domain of the graph
				 * @return True, if the vertex is visible
				 */
				bool isVertexVisible(const Vector3& vertex) const;

				/**
				 * Returns whether this graph is valid and ready to be used.
				 */
				inline bool isValid() const;

			protected:

				/**
				 * Calculates the border location between two connected vertices while one vertex is inside the visibility boundary while the other is outside.
				 * The border location is the intersection between the ray (between both vertices) and the visibility boundary.
				 * @param visibleVertex The vertex which is inside the visibility boundary
				 * @param invisibleVertex The vertex which is outside the visibility boundary
				 * @return The resulting vertex located at the visibility boundary
				 */
				Vector3 calculateBorderVertex(const Vector3& visibleVertex, const Vector3& invisibleVertex) const;

				/**
				 * Determines the function value at a specified (x, z) location and further the 2x1 Jacobian [df/dx, df/dz] for the location.
				 * The function is defined as y = f(x, z).<br>
				 * @param x The x coordinate of the location, with range (-infinity, infinity)
				 * @param z The z coordinate of the location, with range (-infinity, infinity)
				 * @param jacobian The resulting 2x1 jacobian [df/dx, df/dz]
				 * @return The resulting function value (the y value)
				 * @see determineGraphValue(), determineGraphValueAndNormal().
				 */
				Scalar determineGraphValueAndJacobian(const Scalar x, const Scalar z, Vector2& jacobian) const;

				/**
				 * Determines the function value at a specified (x, z) location.
				 * The function is defined as y = f(x, z).<br>
				 * Further, the normal is calculated for the location as well.
				 * @param x The x coordinate of the location, with range (-infinity, infinity)
				 * @param z The z coordinate of the location, with range (-infinity, infinity)
				 * @param normal The resulting normal with unit length
				 * @return The resulting function value (the y value)
				 * @see determineGraphValue().
				 */
				Scalar determineGraphValueAndNormal(const Scalar x, const Scalar z, Vector3& normal) const;

				/**
				 * Verifies the visibility of a given trinagle.
				 * In case the triangle is partially visible, two new triangles will be generated to create a smooth border at the visibility boundary.
				 * @param vertices The original vertices of the mesh, may contain invisible vertices (vertices outside the visibility boundary)
				 * @param normals The original normals of the mesh
				 * @param visibleVertexIndices The mapping between all vetices and visibile vertices
				 * @param triangleFace The triangle face to be checked, must be valid
				 * @param visibleVertices The adjusted vertices not containing invisible vertices anymore
				 * @param visibleNormals The adjusted normals not containing normals of invisible vertices
				 * @param visibleTriangleFaces The adjusted triangle faces not containing (partially) invisible triangles anymore
				 */
				void checkTriangleVisibility(const Vectors3& vertices, const Vectors3& normals, const Indices32& visibleVertexIndices, const Rendering::TriangleFace& triangleFace, Vectors3& visibleVertices, Vectors3& visibleNormals, Rendering::TriangleFaces& visibleTriangleFaces) const;

			public:

				/// The actual 3D function of the graph, y = f(x, z).
				Function function_;

				/// The function written as a text.
				std::string functionText_;

				/// A description of the function.
				std::string functionDescription_;

				/// The expansion of the graph, with range (0, infinity)
				Scalar graphExpansion_ = Scalar(0);

				/// The scale between graph dimension and metric (world) dimenion.
				Scalar metric_s_graph_ = Scalar(0);

				/// The scale between metric (world) dimension and graph dimension.
				Scalar graph_s_metric_ = Scalar(0);

				/// The number of bins the graph is made of (along both axis).
				size_t graphBins_ = 0;

				/// The maximal radius from the origin in which the graph is visualized (allowing to cutoff parts of the graph), with range (0, infinity), 0 to skip using the radius.
				Scalar maximalGraphRadius_;

				/// The maximal height (in +/- y direction) in wich the graph is visualized (allowing to cutoff parts of the graph), with range (0, infinity), 0 to skip using the height.
				Scalar maximalGraphHeight_;
		};

		/**
		 * Definition of a vector holding graph objects.
		 */
		using Graphs = std::vector<Graph>;

	public:

		/**
		 * Creates a new application object.
		 * @param androidApp The android app object as provided in the main function of the native activity, must be valid
		 */
		explicit GraphsApplication(struct android_app* androidApp);

	protected:

		/**
		 * Returns the application's base space.
		 * @see VRNativeApplication::baseSpace().
		 */
		XrSpace baseSpace() const override;

		/**
		 * Event function called after the framebuffer has been initialized.
		 * @see VRNativeApplicationAdvanced::onFramebufferInitialized().
		 */
		void onFramebufferInitialized() override;

		/**
		 * Event function called before the framebuffer will be released.
		 * @see VRNativeApplicationAdvanced::onFramebufferReleasing().
		 */
		void onFramebufferReleasing() override;

		/**
		 * Events function called before the scene is rendered.
		 * @see VRNativeApplicationAdvanced::onPreRender().
		 */
		void onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime) override;

		/**
		 * Event functions for released buttons (e.g., from a tracked controller).
		 * @see VRNativeApplication::onButtonReleased().
		 */
		void onButtonReleased(const OpenXR::TrackedController::ButtonType buttons, const Timestamp& timestamp) override;

		/**
		 * Renders the gradient descent paths.
		 */
		void renderGradientDescentPaths();

		/**
		 * Configures a list of available graphs.
		 */
		void configureGraphs();

		/**
		 * Updates the graph.
		 * @param graphIndex The index of the graph to be shown.
		 */
		bool updateGraph(const size_t graphIndex);

	protected:

		/// The rendering Transform node holding the graph.
		Rendering::TransformRef renderingTransformGraph_;

		/// The rendering Transform node holding the mesh of the graph.
		Rendering::TransformRef renderingTransformGraphMesh_;

		/// The rendering Transform node holding the intersection sphere.
		Rendering::TransformRef renderingTransformIntersection_;

		/// The rendering Transform node holding an array for the pointing interaction.
		Rendering::TransformRef renderingTransformDirection_;

		/// The rendering Transform node holding all gradient descent paths.
		Rendering::TransformRef renderingTransformGradientDescent_;

		/// The rendering Text node to display the function.
		Rendering::TextRef renderingTextFunction_;

		/// The table menu allowing to select individual graphs.
		OpenXR::Application::VRTableMenu vrTableMenu_;

		/// The transformation between graph and world.
		HomogenousMatrix4 world_T_graph_ = HomogenousMatrix4(false);

		/// True, if the menu needs to be updated.
		bool updateMenu_ = false;

		/// The index of the currently selected render mode.
		size_t renderModeIndex_ = size_t(-1);

		/// The available render modes.
		RenderModes renderModes_;

		/// The graphs which can be used.
		Graphs graphs_;

		/// The index of the current selected graph.
		size_t selectedGraphIndex_ = size_t(-1);
};

inline GraphsApplication::Graph::Graph(const Function& function, const std::string& functionText, const std::string& functionDescription, const Scalar graphExpansion, const Scalar metric_s_graph, const size_t graphBins, const Scalar maximalGraphRadius, const Scalar maximalGraphHeight) :
	function_(function),
	functionText_(functionText),
	functionDescription_(functionDescription),
	graphExpansion_(graphExpansion),
	metric_s_graph_(metric_s_graph),
	graphBins_(graphBins),
	maximalGraphRadius_(maximalGraphRadius),
	maximalGraphHeight_(maximalGraphHeight)
{
	graph_s_metric_ = Numeric::ratio(Scalar(1), metric_s_graph_, 0);
}

inline const std::string& GraphsApplication::Graph::functionText() const
{
	return functionText_;
}

inline const std::string& GraphsApplication::Graph::functionDescription() const
{
	return functionDescription_;
}

inline bool GraphsApplication::Graph::isValid() const
{
	return function_ != nullptr;
}

#endif // OCEAN_META_APPLICATION_DEMO_PLATFORM_META_QUEST_OPENXR_GRAPHS_QUEST_GRAPHS_APPLICATION_H
