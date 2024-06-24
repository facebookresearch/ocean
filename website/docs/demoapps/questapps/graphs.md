---
title: Graphs
sidebar_position: 12
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The Quest [Graph Visualizer](https://github.com/facebookresearch/ocean/blob/bd9406c9a318ca1b259dded4f2fba89602b2039c/impl/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplication.cpp#L20) is a sophisticated VR application designed to render interactive 3D graphs, offering a dynamic and immersive way to explore mathematical functions and partial derivatives. This app is particularly useful for users interested in visualizing complex mathematical landscapes.

<img src={require('@site/static/img/docs/demoapps/questapps/graphs.jpg').default} alt="Image: Screenshot of the Graphs Quest app" width="600" className="center-image"/>

The app presents a menu from which users can select predefined 3D functions, ranging from simple sinusoidal wave functions to more complex shapes like a torus or a hyperbolic surface. Users can select a function using the controllers. Once a menu entry is selected, the mesh of the 3D function is calculated and visualized in front of the user. Users can change the visualization mode, for example, switching from a solid surface to a wireframe mode, or toggling normals on and off ([by clicking the controller buttons](https://github.com/facebookresearch/ocean/blob/964c9f61d12ac9ef9ab429cf808fb0f52007c836/impl/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplication.cpp#L286)). Additionally, after setting the controllers aside, users can visualize a Gradient Descent path at arbitrary locations using simple hand interactions: the left hand points with the index finger to select the intersection point on the surface, while the right hand projects the fingertips onto the function's surface.

Adding a new 3D function for visualization is straightforward — simply provide the [function pointer](https://github.com/facebookresearch/ocean/blob/964c9f61d12ac9ef9ab429cf808fb0f52007c836/impl/application/ocean/demo/platform/meta/quest/openxr/graphs/quest/GraphsApplication.cpp#L306) to the new function to be visualized. The app utilizes Ocean's [AutomaticDifferentiation](https://www.internalfb.com/intern/staticdocs/ocean/doxygen/class_ocean_1_1_automatic_differentiation_t.html) functionality to automatically calculate the function values and their partial derivatives.

Furthermore, a custom scale and visualization boundary can be defined for each graph to ensure that only a specific part of the function is visualized. This demo serves as an excellent introduction to Ocean's mathematical capabilities and demonstrates how to customize and adjust 3D meshes for rendering.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>

## Features
  - **3D Graph Rendering**: Displays mathematical functions in 3D.
  - **Visualization Options**: Switch between solid, wireframe, and normals views.
  - **Hand-Gesture Interaction**: Explore Gradient Descent paths with hand movements.
  - **Automatic Differentiation**: Computes values and derivatives automatically.

## Educational and Immersive
  - **Hands-On Learning**: Direct interaction with mathematical concepts.
  - **Visualizing Complex Concepts**: Makes abstract math tangible and visual.
