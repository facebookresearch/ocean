/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/maintenanceviewer/win/MVRenderWindow.h"

#include "ocean/base/Median.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/geometry/Delaunay.h"
#include "ocean/geometry/Utilities.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/rendering/Box.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/Utilities.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/Texture.h"
#include "ocean/rendering/VertexSet.h"
#include "ocean/rendering/Triangles.h"

MVRenderWindow::MVRenderWindow(HINSTANCE applicationInstance, const std::wstring& name, HWND parent) :
	Window(applicationInstance, name, parent),
	meshVisibility_(true),
	pointVisibility_(true),
	sceneExpansion_(100)
{
	// nothing to do here
}

MVRenderWindow::~MVRenderWindow()
{
	// nothing to do here
}

void MVRenderWindow::onInitialized()
{
	try
	{
		// take any rendering engine appyling  an OpenGL API
		engine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGL);

		if (engine_)
		{
			framebuffer_ = engine_->createFramebuffer();
			ocean_assert(framebuffer_);

			framebuffer_->initializeById(size_t(handle()));

			const Rendering::ViewRef view = engine_->factory().createPerspectiveView();

			framebuffer_->setView(view);
			framebuffer_->setViewport(0u, 0u, clientWidth(), clientHeight());

			if (clientHeight() != 0u)
			{
				view->setAspectRatio(float(clientWidth()) / float(clientHeight()));
			}

			framebuffer_->view()->setBackgroundColor(RGBAColor(0, 0, 0));

			framebuffer_->setCullingMode(Rendering::PrimitiveAttribute::CULLING_NONE);

			// set a user defined view position
			framebuffer_->view()->setTransformation(HomogenousMatrix4(true));

			framebuffer_->view()->setBackgroundColor(RGBAColor(0, 0, 0));
		}
	}
	catch (...)
	{
		// the user should be informed about the error
	}
}

void MVRenderWindow::onIdle()
{
	render();
	Thread::sleep(1);
}

void MVRenderWindow::onResize(const unsigned int clientWidth, const unsigned int clientHeight)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_ && clientWidth != 0u && clientHeight != 0u && framebuffer_->view())
	{
		framebuffer_->setViewport(0u, 0u, clientWidth, clientHeight);
		framebuffer_->view()->setAspectRatio(float(clientWidth) / float(clientHeight));
	}

	render();
}

void MVRenderWindow::onKeyDown(const int key)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_)
	{
		switch (key)
		{
			case 'A':
			{
				framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(10, 0, 40)));
				break;
			}

			case 'B':
			{
				framebuffer_->view()->setTransformation(HomogenousMatrix4(Vector3(10, 10, 40)));
				break;
			}

			case '1': // front view
			{
				framebuffer_->view()->setTransformation(HomogenousMatrix4(true));
				framebuffer_->view()->fitCamera();

				break;
			}

			case '2': // left view
			{
				framebuffer_->view()->setTransformation(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(90))));
				framebuffer_->view()->fitCamera();

				break;
			}

			case '3': // top view
			{
				framebuffer_->view()->setTransformation(HomogenousMatrix4(Quaternion(Vector3(1, 0, 0), Numeric::deg2rad(-90))));
				framebuffer_->view()->fitCamera();

				break;
			}

			case '0': // fit view
			{
				framebuffer_->view()->fitCamera();
				break;
			}
		}
	}
}

void MVRenderWindow::onMouseDown(const MouseButton /*button*/, const int x, const int y)
{
	oldCursorX_ = x;
	oldCursorY_ = y;
}

void MVRenderWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_.isNull() || framebuffer_->view().isNull())
	{
		return;
	}

	if (buttons & BUTTON_LEFT)
	{
		ocean_assert(oldCursorX_ != -1 && oldCursorY_ != -1);

		const Scalar xDifference = Scalar(int(oldCursorX_) - int(x));
		const Scalar yDifference = Scalar(int(oldCursorY_) - int(y));

		const Quaternion orientation = framebuffer_->view()->transformation().rotation();

		const Vector3 xAxis(1, 0, 0);
		const Vector3 yAxis(0, 1, 0);

		Scalar factor = Scalar(0.5);
		if (GetKeyState(VK_LCONTROL) & 0x800 || GetKeyState(VK_RCONTROL) & 0xF800)
		{
			factor *= Scalar(10);
		}
		else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
		{
			factor *= Scalar(0.1);
		}

		const Quaternion xRotation(orientation * xAxis, Numeric::deg2rad(Scalar(yDifference)) * factor);
		const Quaternion yRotation(orientation * yAxis, Numeric::deg2rad(Scalar(xDifference)) * factor);

		Quaternion rotation(xRotation * yRotation);
		rotation.normalize();

		const HomogenousMatrix4 newTransform(HomogenousMatrix4(rotation) * framebuffer_->view()->transformation());

		framebuffer_->view()->setTransformation(newTransform);

		InvalidateRect(handle(), nullptr, TRUE);

		oldCursorX_ = x;
		oldCursorY_ = y;
	}
	else if (buttons & BUTTON_RIGHT)
	{
		ocean_assert(oldCursorX_ != -1 && oldCursorY_ != -1);

		const Scalar xDifference = Scalar(int(oldCursorX_) - int(x));
		const Scalar yDifference = Scalar(int(oldCursorY_) - int(y));

		Scalar factor = Scalar(0.001);
		if (GetKeyState(VK_LCONTROL) & 0x800 || GetKeyState(VK_RCONTROL) & 0xF800)
		{
			factor *= Scalar(10);
		}
		else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
		{
			factor *= Scalar(0.1);
		}

		const Vector3 offset(xDifference * sceneExpansion_ * factor, -yDifference * sceneExpansion_ * factor, 0);

		HomogenousMatrix4 transformation = framebuffer_->view()->transformation();

		const Vector3 position = transformation.translation();
		const Quaternion orientation = transformation.rotation();

		transformation.setTranslation(position + orientation * offset);

		framebuffer_->view()->setTransformation(transformation);

		oldCursorX_ = x;
		oldCursorY_ = y;
	}
}

void MVRenderWindow::onKeyUp(const int key)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_)
	{
		if (key == 'L')
		{
			framebuffer_->view()->setUseHeadlight(!framebuffer_->view()->useHeadlight());
		}
		else if (key == 'F')
		{
			Rendering::Framebuffer::FaceMode mode = framebuffer_->faceMode();
			mode = Rendering::Framebuffer::FaceMode(mode + 1);

			if (mode > Rendering::PrimitiveAttribute::MODE_POINT)
			{
				mode = Rendering::PrimitiveAttribute::MODE_DEFAULT;
			}

			framebuffer_->setFaceMode(mode);
		}
		else if (key == 'M')
		{
			meshVisibility_ = !meshVisibility_;

			const Rendering::ObjectRefs objects = engine_->objects("mesh");

			for (Rendering::ObjectRefs::const_iterator i = objects.begin(); i != objects.end(); ++i)
			{
				Rendering::NodeRef node(*i);

				if (node)
				{
					node->setVisible(meshVisibility_);
				}
			}
		}
		else if (key == 'P')
		{
			pointVisibility_ = !pointVisibility_;

			const Rendering::ObjectRefs objects = engine_->objects("point");

			for (Rendering::ObjectRefs::const_iterator i = objects.begin(); i != objects.end(); ++i)
			{
				Rendering::NodeRef node(*i);

				if (node)
				{
					node->setVisible(pointVisibility_);
				}
			}
		}
	}
}

void MVRenderWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	oldCursorX_ = (unsigned int)(-1);
	oldCursorY_ = (unsigned int)(-1);
}

void MVRenderWindow::onMouseWheel(const MouseButton /*buttons*/, const int wheel, const int /*x*/, const int /*y*/)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_)
	{
		Scalar factor = Scalar(0.01);
		if (GetKeyState(VK_LCONTROL) & 0x800 || GetKeyState(VK_RCONTROL) & 0xF800)
		{
			factor *= Scalar(10.0);
		}
		else if (GetKeyState(VK_LMENU) & 0x800 || GetKeyState(VK_RMENU) & 0x800)
		{
			factor *= Scalar(0.1);
		}

		HomogenousMatrix4 transformation = framebuffer_->view()->transformation();

		const Vector3 direction(transformation.rotation() * Vector3(0, 0, -1));
		const Vector3 position(transformation.translation());

		transformation.setTranslation(position + direction * Numeric::copySign(sceneExpansion_ * factor, Scalar(wheel)));

		framebuffer_->view()->setTransformation(transformation);
		InvalidateRect(handle(), nullptr, TRUE);
	}
}

void MVRenderWindow::modifyWindowClass(WNDCLASS& windowClass)
{
	// suppress window background redrawing
	windowClass.hbrBackground = nullptr;
}

void MVRenderWindow::render()
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_)
	{
		ocean_assert(engine_);
		engine_->update(Timestamp(true));

		framebuffer_->makeCurrent();
		framebuffer_->render();
	}
}

bool MVRenderWindow::addMesh(const Vectors3& objectPoints, const Vectors2& imagePoints, const Frame& frame)
{
	if (engine_.isNull())
	{
		return false;
	}

	if (objectPoints.size() != imagePoints.size())
	{
		return false;
	}

	const Rendering::SceneRef renderingScene = engine_->factory().createScene();

	const Geometry::Delaunay::IndexTriangles triangles = Geometry::Delaunay::triangulation(imagePoints);
	Rendering::TriangleFaces triangleFaces;

	for (size_t n = 0; n < triangles.size(); ++n)
	{
		triangleFaces.push_back(Rendering::TriangleFace(triangles[n].index0(), triangles[n].index1(), triangles[n].index2()));
	}

	Rendering::TextureCoordinates textureCoordinates;
	Frame rgbFrame;

	if (frame)
	{
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			return false;
		}

		rgbFrame.setTimestamp(Timestamp(true));

		for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
		{
			textureCoordinates.push_back(Vector2(i->x() / Scalar(frame.width()), Scalar(1) - i->y() / Scalar(frame.height())));
		}
	}

	renderingScene->addChild(Rendering::Utilities::createMesh(engine_, objectPoints, triangleFaces, RGBAColor(0.7f, 0.7f, 0.7f), Rendering::Normals(), textureCoordinates, std::move(rgbFrame)));

	renderingScene->setName("mesh");
	renderingScene->setVisible(meshVisibility_);

	framebuffer_->addScene(renderingScene);

	return true;
}

bool MVRenderWindow::addCamera(const HomogenousMatrix4& pose)
{
	if (engine_.isNull())
	{
		return false;
	}

	if (pose.isNull() || !pose.isValid())
	{
		return false;
	}

	Rendering::Vertices cameraVertices(5);
	cameraVertices[0] = Vector3(0, 0, 0);
	cameraVertices[1] = Vector3(Scalar(0.1), Scalar(0.08), Scalar(-0.2));
	cameraVertices[2] = Vector3(Scalar(0.1), Scalar(-0.08), Scalar(-0.2));
	cameraVertices[3] = Vector3(Scalar(-0.1), Scalar(-0.08), Scalar(-0.2));
	cameraVertices[4] = Vector3(Scalar(-0.1), Scalar(0.08), Scalar(-0.2));

	Rendering::TriangleFaces cameraTriangleFaces(6);
	cameraTriangleFaces[0] = Rendering::TriangleFace(0u, 2u, 1u);
	cameraTriangleFaces[1] = Rendering::TriangleFace(0u, 1u, 4u);
	cameraTriangleFaces[2] = Rendering::TriangleFace(0u, 4u, 3u);
	cameraTriangleFaces[3] = Rendering::TriangleFace(0u, 3u, 2u);
	cameraTriangleFaces[4] = Rendering::TriangleFace(1u, 2u, 3u);
	cameraTriangleFaces[5] = Rendering::TriangleFace(1u, 3u, 4u);

	Rendering::Normals cameraNormals;
	Rendering::Vertices triangleVertices;
	Rendering::TriangleFaces cameraFaces;
	Rendering::Triangles::createTriangleMesh(cameraVertices, cameraTriangleFaces, triangleVertices, cameraNormals, cameraFaces);

	const Rendering::SceneRef renderingScene = engine_->factory().createScene();
	const Rendering::TransformRef cameraTransform = Rendering::Utilities::createMesh(engine_, triangleVertices, cameraFaces, RGBAColor(0.8f, 0.8f, 0.8f), cameraNormals, Rendering::TextureCoordinates());
	cameraTransform->setTransformation(pose);
	renderingScene->addChild(cameraTransform);

	renderingScene->setName("camera");
	renderingScene->setVisible(meshVisibility_);

	framebuffer_->addScene(renderingScene);
	return true;
}

bool MVRenderWindow::addPoints(const Vectors3& objectPoints, const bool adjustExpansion)
{
	if (engine_.isNull())
	{
		return false;
	}

	if (objectPoints.empty())
	{
		return false;
	}

	if (adjustExpansion)
	{
		sceneExpansion_ = determineMedianDimension(objectPoints.data(), objectPoints.size()) * 2;
	}

	const Scalar pointSize = sceneExpansion_ * Scalar(0.01);

	const Rendering::SceneRef renderingScene = engine_->factory().createScene();

	const Rendering::TransformRef box = Rendering::Utilities::createBox(engine_, Vector3(pointSize, pointSize, pointSize), RGBAColor(0.7f, 0.7f, 0.7f));

	for (Vectors3::const_iterator i = objectPoints.begin(); i != objectPoints.end(); ++i)
	{
		const Rendering::TransformRef transform = engine_->factory().createTransform();
		transform->setTransformation(HomogenousMatrix4(*i));

		transform->addChild(box);
		renderingScene->addChild(transform);
	}

	renderingScene->setName("point");
	renderingScene->setVisible(pointVisibility_);

	framebuffer_->addScene(renderingScene);

	framebuffer_->view()->setNearFarDistance(sceneExpansion_ * Scalar(0.01), sceneExpansion_ * 1000);

	if (adjustExpansion)
	{
		const Vector3 medianPoint = Geometry::Utilities::medianObjectPoint(ConstTemplateArrayAccessor<Vector3>(objectPoints));
		framebuffer_->view()->setTransformation(HomogenousMatrix4(medianPoint - Vector3(0, 0, -1) * sceneExpansion_));
	}

	return true;
}

void MVRenderWindow::setPose(const HomogenousMatrix4& pose)
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_ && framebuffer_->view())
	{
		framebuffer_->view()->setTransformation(pose);
	}
}

void MVRenderWindow::updateCoordinateSystem(const HomogenousMatrix4& transformation)
{
	if (engine_.isNull())
	{
		return;
	}

	Rendering::TransformRef coordinateSystem = engine_->object("coordinatesystem");
	if (!coordinateSystem)
	{
		const Scalar radius = sceneExpansion_ * Scalar(0.01);
		coordinateSystem = Rendering::Utilities::createCoordinateSystem(engine_, radius * 10, radius * 2, radius);

		if (coordinateSystem)
		{
			coordinateSystem->setName("coordinatesystem");
		}

		Rendering::SceneRef scene(engine_->factory().createScene());
		scene->addChild(coordinateSystem);

		framebuffer_->addScene(scene);
	}

	if (coordinateSystem)
	{
		coordinateSystem->setTransformation(transformation);
	}
}

void MVRenderWindow::updatePlane(const HomogenousMatrix4& transformation)
{
	if (engine_.isNull())
	{
		return;
	}

	Rendering::TransformRef planeTransform = engine_->object("plane");
	if (!planeTransform)
	{
		planeTransform = Rendering::Utilities::createBox(engine_, Vector3(sceneExpansion_ * 1, sceneExpansion_ * 1, Scalar(0.001)), RGBAColor(0.7f, 0.7f, 0.7f, 0.7f));

		if (planeTransform)
		{
			planeTransform->setName("plane");
		}

		Rendering::SceneRef scene(engine_->factory().createScene());
		scene->addChild(planeTransform);

		framebuffer_->addScene(scene);
	}

	if (planeTransform)
	{
		planeTransform->setTransformation(transformation);
	}
}

void MVRenderWindow::updateHighlightObjectPoints(const Vectors3& objectPoints)
{
	if (engine_.isNull())
	{
		return;
	}

	Rendering::SceneRef highlightsScene = engine_->object("highlights");
	if (!highlightsScene)
	{
		highlightsScene = engine_->factory().createScene();
		highlightsScene->setName("highlights");
	}

	highlightsScene->clear();

	const Scalar pointSize = sceneExpansion_ * Scalar(0.0101);
	const Rendering::TransformRef box = Rendering::Utilities::createBox(engine_, Vector3(pointSize, pointSize, pointSize), RGBAColor(1, 0, 0));

	for (Vectors3::const_iterator i = objectPoints.begin(); i != objectPoints.end(); ++i)
	{
		const Rendering::TransformRef transform = engine_->factory().createTransform();
		transform->setTransformation(HomogenousMatrix4(*i));

		transform->addChild(box);
		highlightsScene->addChild(transform);
	}

	framebuffer_->addScene(highlightsScene);
}

void MVRenderWindow::clear()
{
	if (engine_.isNull())
	{
		return;
	}

	if (framebuffer_)
	{
		framebuffer_->clearScenes();
	}
}

Scalar MVRenderWindow::determineMedianDimension(const Vector3* objectPoints, const size_t size) const
{
	Scalars xPoints, yPoints, zPoints;
	xPoints.reserve(size);
	yPoints.reserve(size);
	zPoints.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		xPoints.push_back(objectPoint.x());
		yPoints.push_back(objectPoint.y());
		zPoints.push_back(objectPoint.z());
	}

	const Vector3 medianPoint(Median::median(xPoints.data(), xPoints.size()), Median::median(yPoints.data(), yPoints.size()), Median::median(zPoints.data(), zPoints.size()));

	Scalars sqrDistances;
	sqrDistances.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		sqrDistances.push_back(medianPoint.sqrDistance(objectPoints[n]));
	}

	return Numeric::sqrt(Median::median(sqrDistances.data(), sqrDistances.size()));
}
