// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/qt/MainWidget.h"

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/interaction/Manager.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/scenedescription/Manager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QMimeData>
#include <QtCore/QTimer>

#include <QtGui/QDragEnterEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QGuiApplication>

#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPlainTextEdit>

namespace Ocean
{

namespace XRPlayground
{

namespace QT
{

MainWidget::MainWidget(const std::string& content, const std::string& vrsRecording) :
	QWidget(nullptr),
	renderTimer_(nullptr),
	lastMousePosition_(Numeric::minValue(), Numeric::minValue()),
	qActionStartRecording_(nullptr),
	qActionStopRecording_(nullptr)
{
	std::string releaseString = Build::releaseType();

	if (!releaseString.empty())
	{
		releaseString = ", " + releaseString;
	}

	setWindowTitle(("XRPlayground" + releaseString).c_str());

	logDialog_ = new LogDialog(this);

	xrPlaygroundCommon_.initialize();

	const Rendering::WindowFramebufferRef windowFramebuffer = xrPlaygroundCommon_.framebuffer();
	ocean_assert(windowFramebuffer);

	windowFramebuffer->initializeById(size_t(this));

	if (windowFramebuffer->view())
	{
		windowFramebuffer->view()->setTransformation(defaultView());
	}

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	layout->setMargin(0);
	layout->setMenuBar(setupMenu());
	layout->addWidget(dynamic_cast<QWidget*>(&*windowFramebuffer));

	resize(1000, 800);

	// set an timer for render updates

	renderTimer_ = new QTimer(this);
	connect(renderTimer_, &QTimer::timeout, this, QOverload<>::of(&MainWidget::onRender));
	renderTimer_->start(0);

	setAcceptDrops(true);

	if (!vrsRecording.empty())
	{
		loadVRSRecording(vrsRecording);
	}

	if (!content.empty())
	{
		loadScene(content);
	}
}

MainWidget::~MainWidget()
{
	renderTimer_->stop();

	xrPlaygroundCommon_.release();
}

QMenuBar* MainWidget::setupMenu()
{
	QMenuBar* menuBar = new QMenuBar(this);

	QMenu* fileMenu = new QMenu("File");
	menuBar->addMenu(fileMenu);

	fileMenu->addAction("Load Scene", this, QOverload<>::of(&MainWidget::onMenuLoadScene), tr("CTRL+L"));
	fileMenu->addAction("Close Scene", this, QOverload<>::of(&MainWidget::onMenuCloseScene), tr("CTRL+C"));
	fileMenu->addAction("Reload Scene", this, QOverload<>::of(&MainWidget::onMenuReloadScene), tr("CTRL+R"));

	QMenu* viewMenu = new QMenu("View");
	menuBar->addMenu(viewMenu);

	viewMenu->addAction("Show Log", this, QOverload<>::of(&MainWidget::onMenuShowLog), tr("CTRL+G"));
	viewMenu->addAction("Fit View", this, QOverload<>::of(&MainWidget::onMenuFitView), tr("CTRL+F"));
	viewMenu->addAction("Reset View", this, QOverload<>::of(&MainWidget::onMenuResetView), tr("CTRL+T"));

	QMenu* background = new QMenu("Background");
	connect(background, &QMenu::aboutToShow, this, QOverload<>::of(&MainWidget::onMenuBackground));
	menuBar->addMenu(background);

	background->addAction(QString("Use VRS recording ..."), this, QOverload<>::of(&MainWidget::onMenuBackgroundEntryVRSRecording), tr("CTRL+V"));
	background->addSeparator();

	QMenu* recording = new QMenu("Recording");
	menuBar->addMenu(recording);

	ocean_assert(qActionStartRecording_ == nullptr);
	qActionStartRecording_ = recording->addAction("Start Recording");
	connect(qActionStartRecording_, &QAction::triggered, this, QOverload<>::of(&MainWidget::onMenuRecordingStartRecording));

	ocean_assert(qActionStopRecording_ == nullptr);
	qActionStopRecording_ = recording->addAction("Stop Recording");
	qActionStopRecording_->setEnabled(false);
	connect(qActionStopRecording_, &QAction::triggered, this, QOverload<>::of(&MainWidget::onMenuRecordingStopRecording));

	QMenu* screen = new QMenu("Screen");
	menuBar->addMenu(screen);

	QAction* rotateScreen = screen->addAction("Rotate 90 counter clockwise");
	rotateScreen->setCheckable(true);
	connect(rotateScreen, &QAction::triggered, this, QOverload<>::of(&MainWidget::onMenuScreenRotate));

	QMenu* experiences = new QMenu("Experiences");
	menuBar->addMenu(experiences);

	ExperiencesManager::SelectableExperienceGroups experienceGroups = ExperiencesManager::selectableExperienceGroups(ExperiencesManager::PT_DESKTOP);

	for (const ExperiencesManager::SelectableExperienceGroup& experienceGroup : experienceGroups)
	{
		bool sectionAdded = false;

		for (const ExperiencesManager::SelectableExperience& experience : experienceGroup.second)
		{
			if (experience.url().find(".experience") == std::string::npos)
			{
				// for now, we allow C++ experiences only
				continue;
			}

			if (!sectionAdded)
			{
				experiences->addSection(QString::fromStdString(experienceGroup.first));
				sectionAdded = true;
			}

			QAction* experienceAction = experiences->addAction(QString::fromStdString(experience.name()));
			experienceAction->setData(QString::fromStdString(experience.url()));

			connect(experienceAction, &QAction::triggered, this, QOverload<>::of(&MainWidget::onMenuExperience));
		}
	}

	return menuBar;
}

bool MainWidget::loadScene(const std::string& filename)
{
	if (!xrPlaygroundCommon_.isInitialized())
	{
		return false;
	}

	if (xrPlaygroundCommon_.loadContent(filename, true /*replace*/))
	{
		// show filename (without path) in window title
		setWindowTitle(("XRPlayground: " + IO::File(filename).name()).c_str());

		lastFilename_ = filename;
		return true;
	}

	return false;
}

bool MainWidget::loadVRSRecording(const std::string& filename)
{
	devicePlayer_.loadRecording(filename);

	const Media::FrameMediumRefs frameMediums(devicePlayer_.frameMediums());

	if (!frameMediums.empty())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();
		ocean_assert(frameMedium);

		xrPlaygroundCommon_.setBackgroundFrameMedium(frameMedium);

		// **TODO** reflect new background in UI
	}

	devicePlayer_.start();

	return true;
}

void MainWidget::mousePressEvent(QMouseEvent* qMouseEvent)
{
	ocean_assert(qMouseEvent != nullptr);
	ocean_assert(lastMousePosition_.x() == Numeric::minValue());

	lastMousePosition_ = Vector2(Scalar(qMouseEvent->pos().x()), Scalar(qMouseEvent->pos().y()));
}

void MainWidget::mouseMoveEvent(QMouseEvent* qMouseEvent)
{
	ocean_assert(qMouseEvent != nullptr);

	const Vector2 currentMousePosition = Vector2(Scalar(qMouseEvent->pos().x()), Scalar(qMouseEvent->pos().y()));

	const Rendering::FramebufferRef framebuffer = xrPlaygroundCommon_.framebuffer();

	if (framebuffer && framebuffer->view())
	{
		const Rendering::ViewRef view = framebuffer->view();

		const Qt::KeyboardModifiers keyboardModifiers = QGuiApplication::keyboardModifiers();

		Scalar impactFactor = Scalar(1);
		if (keyboardModifiers & Qt::ControlModifier)
		{
			impactFactor *= Scalar(10);
		}
		else if (keyboardModifiers & Qt::AltModifier)
		{
			impactFactor *= Scalar(0.1);
		}

		if (qMouseEvent->buttons() & Qt::RightButton)
		{
			impactFactor *= Scalar(0.01);

			const Vector3 offsetTranslation(lastMousePosition_.x() - currentMousePosition.x(), currentMousePosition.y() - lastMousePosition_.y(), 0);

			HomogenousMatrix4 world_T_view = view->transformation();
			world_T_view.setTranslation(world_T_view.translation() + world_T_view.rotation() * offsetTranslation * impactFactor);

			view->setTransformation(world_T_view);
		}
		else if (qMouseEvent->buttons() & Qt::LeftButton)
		{
			// "trackball" rotation, adapted from cvkit's plyv functionality https://github.com/roboception/cvkit/blob/master/gvr/glcamera.cc#L292-L305
			// TODO: like in plyv, amount of rotation is limited to 180 degrees (at window edge); meshlab allows more rotation, is that better?
			// but note you can use the keyboard modifiers to increase / decrease amount of rotation (Control: * 10, Alt: * 0.1)

			const Scalar vector1X(Scalar(1) - Scalar(2) * lastMousePosition_.x() / Scalar(width()));
			const Scalar vector1Y(Scalar(1) - Scalar(2) * lastMousePosition_.y() / Scalar(height()));
			const Scalar vector1Z(sqrt(std::max(Scalar(0), Scalar(1) - (vector1X * vector1X + vector1Y * vector1Y))));
			Vector3 vector1(-vector1X, vector1Y, vector1Z);
			vector1.normalize();

			const Scalar vector2X(Scalar(1) - Scalar(2) * currentMousePosition.x() / Scalar(width()));
			const Scalar vector2Y(Scalar(1) - Scalar(2) * currentMousePosition.y() / Scalar(height()));
			const Scalar vector2Z(sqrt(std::max(Scalar(0), Scalar(1) - (vector2X * vector2X + vector2Y * vector2Y))));
			Vector3 vector2(-vector2X, vector2Y, vector2Z);
			vector2.normalize();

			Vector3 axis = vector2.cross(vector1);
			const Scalar angle(2.0 * acos(std::min(1.0, vector1 * vector2)));
			if (!axis.isNull() && (angle != 0))
			{
				axis.normalize();
				const SquareMatrix3 rotationDelta(Rotation(axis, angle * impactFactor));

				const SquareMatrix3 orientationOld(view->transformation().rotationMatrix());
				const Vector3 positionOld = view->transformation().translation();

				const SquareMatrix3 orientationNew = orientationOld * rotationDelta;

				const Vector3 center(0, 0, 0); // rotation center; TODO: allow changing via double click into the scene
				const Vector3 positionNew = center - orientationOld * rotationDelta * orientationOld.transposed() * (center - positionOld);

				view->setTransformation(HomogenousMatrix4(positionNew, orientationNew));
			}
		}
	}

	lastMousePosition_ = currentMousePosition;
}

void MainWidget::mouseReleaseEvent(QMouseEvent* qMouseEvent)
{
	ocean_assert(qMouseEvent != nullptr);
	ocean_assert(lastMousePosition_.x() != Numeric::minValue());

	lastMousePosition_ = Vector2(Numeric::minValue(), Numeric::minValue());
}

void MainWidget::wheelEvent(QWheelEvent* qWheelEvent)
{
	ocean_assert(qWheelEvent != nullptr);

	const Rendering::FramebufferRef framebuffer = xrPlaygroundCommon_.framebuffer();

	if (framebuffer && framebuffer->view())
	{
		const Rendering::ViewRef view = framebuffer->view();

		const Qt::KeyboardModifiers keyboardModifiers = QGuiApplication::keyboardModifiers();

		Scalar impactFactor = Scalar(0.01);
		if (keyboardModifiers & Qt::ControlModifier)
		{
			impactFactor *= Scalar(10);
		}
		else if (keyboardModifiers & Qt::AltModifier)
		{
			impactFactor *= Scalar(0.1);
		}

		HomogenousMatrix4 world_T_view = view->transformation();

		const Vector3 direction(world_T_view.rotation() * Vector3(0, 0, -1));

		world_T_view.setTranslation(world_T_view.translation() + direction * Scalar(qWheelEvent->delta()) * impactFactor);

		view->setTransformation(world_T_view);
	}
}

void MainWidget::dragEnterEvent(QDragEnterEvent* qDragEnterEvent)
{
	ocean_assert(qDragEnterEvent != nullptr);

	if (qDragEnterEvent->mimeData()->hasUrls())
	{
		qDragEnterEvent->acceptProposedAction();
	}
}

void MainWidget::dropEvent(QDropEvent* qDropEvent)
{
	ocean_assert(qDropEvent != nullptr);

	const QList<QUrl>& urls = qDropEvent->mimeData()->urls();

	if (urls.size() >= 1)
	{
		const std::string file = urls[0].path().toUtf8().constData();

		if (file.find(".vrs") == std::string::npos)
		{
			loadScene(file);
		}
		else
		{
			loadVRSRecording(file);
		}
	}
}

void MainWidget::keyPressEvent(QKeyEvent* qKeyEvent)
{
	if (!qKeyEvent->isAutoRepeat())
	{
		xrPlaygroundCommon_.keyEventPressed(Interaction::UserInterface(), qKeyEvent->text().toStdString());
	}
}

void MainWidget::keyReleaseEvent(QKeyEvent* qKeyEvent)
{
	if (!qKeyEvent->isAutoRepeat())
	{
		xrPlaygroundCommon_.keyEventReleased(Interaction::UserInterface(), qKeyEvent->text().toStdString());
	}
}

void MainWidget::onMenuLoadScene()
{
	const IO::FileManager::FileExtensions fileExtensions = SceneDescription::Manager::get().supportedExtensions();

	std::string filter = "Scene files (";

	bool first = true;
	for (IO::FileManager::FileExtensions::const_iterator i = fileExtensions.cbegin(); i != fileExtensions.cend(); ++i)
	{
		if (first)
		{
			first = false;
			filter += "*." + i->first;
		}
		else
		{
			filter += " *." + i->first;
		}
	}

	filter += " *.vrs *.zip";
	filter += ")";

	const QString filename = QFileDialog::getOpenFileName(this, "Load new scene", QString(), QString(filter.c_str()));

	if (filename.size() > 0)
	{
		const std::string file = filename.toUtf8().constData();

		loadScene(file);
	}
}

void MainWidget::onMenuReloadScene()
{
	if (lastFilename_.size() > 0)
	{
		loadScene(lastFilename_);
	}
}

void MainWidget::onMenuCloseScene()
{
	xrPlaygroundCommon_.unloadContent();

	setWindowTitle("XRPlayground");

}

void MainWidget::onMenuShowLog()
{
	logDialog_->show();
}

void MainWidget::onMenuResetView()
{
	const Rendering::FramebufferRef framebuffer = xrPlaygroundCommon_.framebuffer();

	if (framebuffer && framebuffer->view())
	{
		framebuffer->view()->setTransformation(defaultView());
	}
}

void MainWidget::onMenuFitView()
{
	const Rendering::FramebufferRef framebuffer = xrPlaygroundCommon_.framebuffer();

	if (framebuffer && framebuffer->view())
	{
		framebuffer->view()->fitCamera();
	}
}

void MainWidget::onMenuBackground()
{
	QMenu* backgroundMenu = dynamic_cast<QMenu*>(sender());
	ocean_assert(backgroundMenu != nullptr);

	const Media::Library::Definitions liveVideoDefinitions = Media::Manager::get().selectableMedia(Media::Medium::LIVE_VIDEO);

	std::set<std::string> urls;
	for (const Media::Library::Definition& definition : liveVideoDefinitions)
	{
		urls.emplace(definition.url);
	}

	ocean_assert(backgroundMenu->actions().size() >= 2);

	for (int n = 2; n < backgroundMenu->actions().size(); ++n)
	{
		QAction* action = backgroundMenu->actions()[n];

		const std::string text = action->text().toUtf8().constData();

		if (urls.find(text) == urls.cend())
		{
			backgroundMenu->removeAction(action);
		}
		else
		{
			urls.erase(text);
		}
	}

	for (const std::string& url : urls)
	{
		QAction* action = backgroundMenu->addAction(url.c_str());
		action->setCheckable(true);

		connect(action, &QAction::triggered, this, &MainWidget::onMenuBackgroundEntryTriggered);
	}
}

void MainWidget::onMenuBackgroundEntryVRSRecording()
{
	const QString filename = QFileDialog::getOpenFileName(this, "Load VRS recording", QString(), "VRS recording (*.vrs)");

	if (filename.size() > 0)
	{
		const std::string file = filename.toUtf8().constData();

		loadVRSRecording(file);
	}
}

void MainWidget::onMenuBackgroundEntryTriggered(bool checked)
{
	QAction* backgroundMenuEntry = dynamic_cast<QAction*>(sender());
	ocean_assert(backgroundMenuEntry != nullptr);

	if (checked)
	{
		const std::string backgroundUrl = backgroundMenuEntry->text().toUtf8().constData();

		const Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(backgroundUrl);

		if (frameMedium)
		{
			frameMedium->start();

			if (xrPlaygroundCommon_.setBackgroundFrameMedium(frameMedium))
			{
				backgroundMenuEntry->setChecked(true);
			}
		}
	}
	else
	{
		xrPlaygroundCommon_.setBackgroundFrameMedium(Media::FrameMediumRef());
	}
}

void MainWidget::onMenuRecordingStartRecording()
{
	ocean_assert(qActionStartRecording_ == sender());

	if (xrPlaygroundCommon_.isVRSRecordingActive())
	{
		Log::warning() << "VRS recording already active";
		return;
	}

	const IO::Directory directory(QDir::currentPath().toUtf8().constData());

	const IO::File vrsFilename(directory + IO::File("XRPlayground_Recording__" + DateTime::localStringDate('-') + "__" + DateTime::localStringTime(false, '-') + ".vrs"));

	xrPlaygroundCommon_.startVRSRecording(vrsFilename());
}

void MainWidget::onMenuRecordingStopRecording()
{
	ocean_assert(qActionStopRecording_ == sender());

	if (!xrPlaygroundCommon_.isVRSRecordingActive())
	{
		Log::warning() << "VRS recording not active";
		return;
	}

	xrPlaygroundCommon_.stopVRSRecording();
}

void MainWidget::onMenuScreenRotate()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	ocean_assert(action != nullptr);

	Quaternion device_T_display(true);

	if (action->isChecked())
	{
		device_T_display = Quaternion(Vector3(0, 0, 1), -Numeric::pi_2());
	}

	xrPlaygroundCommon_.setDevice_T_display(HomogenousMatrix4(device_T_display));
}

void MainWidget::onMenuExperience()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	ocean_assert(action != nullptr);

	const std::string url = action->data().toString().toUtf8().constData();

	if (url.empty())
	{
		xrPlaygroundCommon_.unloadContent();
	}
	else
	{
		xrPlaygroundCommon_.loadContent(url, true);
	}
}

void MainWidget::onRender()
{
	xrPlaygroundCommon_.preRender(Interaction::UserInterface());

	xrPlaygroundCommon_.render();
}

HomogenousMatrix4 MainWidget::defaultView()
{
	return HomogenousMatrix4(Vector3(0, 0, 2)); // 2 meters in front of origin
}

}

}

}
