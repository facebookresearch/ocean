/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/imageannotator/wxw/IAMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterScharr.h"
#include "ocean/cv/FrameInverter.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/cv/detector/LineDetectorULF.h"
#include "ocean/cv/detector/LineEvaluator.h"

#include "ocean/io/File.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/wxwidgets/DnD.h"
#include "ocean/platform/wxwidgets/Utilities.h"

#include "application/ocean/demo/misc/imageannotator/clusterlines.xpm"
#include "application/ocean/demo/misc/imageannotator/finiteline.xpm"
#include "application/ocean/demo/misc/imageannotator/gradient.xpm"
#include "application/ocean/demo/misc/imageannotator/gradient_plus.xpm"
#include "application/ocean/demo/misc/imageannotator/nozoom.xpm"
#include "application/ocean/demo/misc/imageannotator/open.xpm"
#include "application/ocean/demo/misc/imageannotator/select.xpm"
#include "application/ocean/demo/misc/imageannotator/toggledisplay.xpm"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace Ocean::CV::Detector;

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(IAMainWindow, wxFrame)
	EVT_MENU(IAMainWindow::ID_Load_Image, IAMainWindow::onLoadImage)
	EVT_MENU(IAMainWindow::ID_Load_Lines, IAMainWindow::onLoadLines)
	EVT_MENU(IAMainWindow::ID_Load_Image_Or_Lines, IAMainWindow::onLoadImageOrLines)
	EVT_MENU(IAMainWindow::ID_Save_Lines, IAMainWindow::onSaveLines)
	EVT_MENU(IAMainWindow::ID_Save_Groups, IAMainWindow::onSaveGroups)
	EVT_MENU(IAMainWindow::ID_Icon_Line, IAMainWindow::onIconLine)
	EVT_MENU(IAMainWindow::ID_Icon_Select, IAMainWindow::onIconSelect)
	EVT_MENU(IAMainWindow::ID_Nozoom, IAMainWindow::onNoZoom)
	EVT_MENU(IAMainWindow::ID_Toggle_Show_Lines, IAMainWindow::onToggleShowLines)
	EVT_MENU(IAMainWindow::ID_Toggle_Show_Gradients, IAMainWindow::onToggleShowGradients)
	EVT_MENU(IAMainWindow::ID_Toggle_Show_Gradients_Plus, IAMainWindow::onToggleShowGradientsPlus)
	EVT_MENU(IAMainWindow::ID_Detect_Lines_LSD, IAMainWindow::onDetectLinesLSD)
	EVT_MENU(IAMainWindow::ID_Detect_Lines_ULF, IAMainWindow::onDetectLinesULF)
	EVT_MENU(IAMainWindow::ID_Remove_All_Lines, IAMainWindow::onRemoveAllLines)
	EVT_MENU(IAMainWindow::ID_Evaluate_Lines, IAMainWindow::onEvaluateLines)
	EVT_MENU(IAMainWindow::ID_Cluster_Lines, IAMainWindow::onClusterLines)
	EVT_MENU(IAMainWindow::ID_Quit, IAMainWindow::onQuit)
	EVT_MENU(IAMainWindow::ID_About, IAMainWindow::onAbout)
	EVT_CLOSE(IAMainWindow::onClose)
END_EVENT_TABLE()

IAMainWindow::IAMainWindow(const wxString& title, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_Load_Image, L"&Load image\tCtrl-O");
	menuFile->Append(ID_Load_Lines, L"&Load lines\tCtrl-L");
	menuFile->Append(ID_Save_Lines, L"Save lines as\tCtrl-S");
	menuFile->Append(ID_Save_Groups, L"Save groups as\tCtrl-G");
	menuFile->Append(ID_About, L"&About...");
	menuFile->AppendSeparator();
	menuFile->Append(ID_Quit, L"E&xit");

	wxMenu* menuAdvanced = new wxMenu;

	menuAdvanced->Append(ID_Remove_All_Lines, L"Remove all lines");
	menuAdvanced->AppendSeparator();
	menuAdvanced->Append(ID_Detect_Lines_LSD, L"Detect LSD lines");
	menuAdvanced->Append(ID_Detect_Lines_ULF, L"Detect ULF lines");
	menuAdvanced->AppendSeparator();
	menuAdvanced->Append(ID_Evaluate_Lines, L"Evaluate lines");

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuAdvanced, L"&Advanced");

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(0x808080));

	CreateStatusBar(3);
	SetStatusBarPane(2);

#ifdef _WINDOWS
	CreateToolBar(long(wxNO_BORDER) | long(wxHORIZONTAL) | long(wxTB_FLAT), ID_Toolbar);
#else
	CreateToolBar(wxTB_DEFAULT_STYLE, ID_Toolbar);
	GetToolBar()->SetToolBitmapSize(wxSize(16, 16));
#endif

	GetToolBar()->SetMargins(2, 2);
	GetToolBar()->AddTool(ID_Load_Image_Or_Lines, L"", wxBitmap(xpm_open), wxNullBitmap, wxITEM_NORMAL, L"Load image or lines");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Icon_Line, L"", wxBitmap(xpm_finiteline), wxNullBitmap, wxITEM_CHECK, L"Create lines");
	GetToolBar()->AddTool(ID_Icon_Select, L"", wxBitmap(xpm_select), wxNullBitmap, wxITEM_CHECK, L"Select lines");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Nozoom, L"", wxBitmap(xpm_nozoom), wxNullBitmap, wxITEM_NORMAL, L"No zoom");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Toggle_Show_Lines, L"", wxBitmap(toggledisplay_xpm), wxNullBitmap, wxITEM_CHECK, L"Toggle displaying lines");
	GetToolBar()->AddTool(ID_Toggle_Show_Gradients, L"", wxBitmap(gradient_xpm), wxNullBitmap, wxITEM_CHECK, L"Toggle between image and gradients");
	GetToolBar()->AddTool(ID_Toggle_Show_Gradients_Plus, L"", wxBitmap(gradient_plus_xpm), wxNullBitmap, wxITEM_CHECK, L"Toggle between image and gradients (plus)");
	GetToolBar()->AddSeparator();
	GetToolBar()->AddTool(ID_Cluster_Lines, L"", wxBitmap(clusterlines_xpm), wxNullBitmap, wxITEM_NORMAL, L"Cluster lines according to common vanishing point");
	GetToolBar()->Realize();

	GetToolBar()->ToggleTool(ID_Icon_Line, true);
	GetToolBar()->ToggleTool(ID_Toggle_Show_Lines, true);
	GetToolBar()->ToggleTool(ID_Toggle_Show_Gradients, false);
	GetToolBar()->ToggleTool(ID_Toggle_Show_Gradients_Plus, false);

	imageWindow_ = new IAImageWindow(this);
	imageWindow_->Show();

	Platform::WxWidgets::FileDropTarget* dropTarget = new Platform::WxWidgets::FileDropTarget(Platform::WxWidgets::FileDropTarget::Callback::create(*this, &IAMainWindow::onFileDragAndDrop));
	SetDropTarget(dropTarget);
}

bool IAMainWindow::loadImage(const std::string& filename)
{
	if (!LineManager::get().isEmpty() && LineManager::get().hasContentChanged())
	{
		if (wxMessageBox(std::wstring(L"Do you really want to load a new image, all existing lines will be removed."), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxNO)
		{
			return false;
		}
	}

	ocean_assert(imageWindow_ != nullptr);
	imageWindow_->resetInteractionData();

	LineManager::get().clear();

	imageWindow_->setLineEvaluationMap(LineEvaluator::LineMatchMap());

	image_ = Media::Utilities::loadImage(filename);

	if (!image_.isValid())
	{
		wxMessageBox(std::wstring(L"Failed to open the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	if (!createGradientImage(image_, gradientImage_, gradientImagePlus_))
	{
		wxMessageBox(std::wstring(L"Failed to create a gradient image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}
	ocean_assert(gradientImage_ && gradientImagePlus_);

	const Frame* image = &image_;

	if (GetToolBar()->GetToolState(ID_Toggle_Show_Gradients))
	{
		image = &gradientImage_;
	}
	else if (GetToolBar()->GetToolState(ID_Toggle_Show_Gradients_Plus))
	{
		image = &gradientImagePlus_;
	}

	ocean_assert(imageWindow_);
	if (!imageWindow_->setFrame(*image, true))
	{
		wxMessageBox(std::wstring(L"Failed to convert the image\n\"") + String::toWString(filename) + std::wstring(L"\""), L"Error", wxOK | wxICON_ERROR, this);
		return false;
	}

	imageFile_ = filename;
	const IO::File file(filename);

	SetStatusText(String::toWString(file.name()) + std::wstring(L", ") + String::toWString(image_.width()) + std::wstring(L"x") + String::toWString(image_.height()));

	// let's check whether we can load the corresponding line file

	const IO::File lineFile(file.base() + std::string(".lns"));
	if (lineFile.exists())
	{
		if (wxMessageBox(std::wstring(L"Do you want to load the corresponding file of lines '") + String::toWString(lineFile.name()) + std::wstring(L"'?"), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxYES)
		{
			loadLines(lineFile());
		}
	}

	// let's check whether we can load the corresponding group file

	if (!LineManager::get().isEmpty())
	{
		const IO::File groupFile(file.base() + std::string(".grs"));
		if (groupFile.exists())
		{
			if (wxMessageBox(std::wstring(L"Do you want to load the corresponding file of line groupss '") + String::toWString(groupFile.name()) + std::wstring(L"'?"), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxYES)
			{
				loadGroups(groupFile());
			}
		}
	}

	return true;
}

bool IAMainWindow::loadLines(const std::string& filename)
{
	ocean_assert(imageWindow_ != nullptr);
	imageWindow_->resetInteractionData();

	LineManager::get().clear();

	const bool result = LineManager::get().loadLines(filename);

	imageWindow_->Refresh();

	return result;
}

bool IAMainWindow::loadGroups(const std::string& filename)
{
	ocean_assert(imageWindow_ != nullptr);
	imageWindow_->resetInteractionData();

	const bool result = LineManager::get().loadGroups(filename);

	imageWindow_->Refresh();

	return result;
}

bool IAMainWindow::saveLines(const std::string& filename)
{
	return LineManager::get().saveLines(filename);
}

bool IAMainWindow::saveLines()
{
	if (LineManager::get().isEmpty())
	{
		wxMessageBox(L"Currently, no lines exist that could be saved.", L"Information", wxOK | wxICON_INFORMATION, this);
		return true;
	}

	const std::wstring allWildcard(L"All supported files|*.lns");
	const std::wstring lnsWildcard(L"Lines files (*.lns)|*.lns");
	const std::wstring wildcard = allWildcard + std::wstring(L"|") + lnsWildcard;

	wxFileDialog dialog(this, L"Save line file...", L"", L"", wildcard.c_str(), wxFD_SAVE);

	if (wxID_OK != dialog.ShowModal())
	{
		return false;
	}

	return saveLines(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
}

bool IAMainWindow::saveGroups()
{
	if (LineManager::get().isEmpty())
	{
		wxMessageBox(L"Currently, no lines exist for which information could be saved.", L"Information", wxOK | wxICON_INFORMATION, this);
		return true;
	}

	const std::wstring allWildcard(L"All supported files|*.grs");
	const std::wstring lnsWildcard(L"Gruops files (*.grs)|*.grs");
	const std::wstring wildcard = allWildcard + std::wstring(L"|") + lnsWildcard;

	wxFileDialog dialog(this, L"Save group file...", L"", L"", wildcard.c_str(), wxFD_SAVE);

	if (wxID_OK != dialog.ShowModal())
	{
		return false;
	}

	return LineManager::get().saveGroups(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
}

void IAMainWindow::onSaveLines(wxCommandEvent& /*event*/)
{
	saveLines();
}

void IAMainWindow::onSaveGroups(wxCommandEvent& /*event*/)
{
	saveGroups();
}

void IAMainWindow::onLoadImage(wxCommandEvent& /*event*/)
{
	const std::wstring allWildcard(L"All supported files|*.bmp;*.gif;*.jpg;*.jpeg;*.png");
	const std::wstring bmpWildcard(L"Bitmap files (*.bmp)|*.bmp");
	const std::wstring gifWildcard(L"GIF files (*.gif)|*.gif");
	const std::wstring jpgWildcard(L"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
	const std::wstring pngwildCard(L"PNG files (*.png)|*.png");

	const std::wstring wildcard = allWildcard + std::wstring(L"|") + bmpWildcard + std::wstring(L"|")
																	+ gifWildcard + std::wstring(L"|") + jpgWildcard + std::wstring(L"|") + pngwildCard;

	wxFileDialog dialog(this, L"Select image file...", L"", L"", wildcard.c_str(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	loadImage(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
}

void IAMainWindow::onLoadLines(wxCommandEvent& /*event*/)
{
	const std::wstring allWildcard(L"All supported files|*.lns");
	const std::wstring lnsWildcard(L"Lines files (*.lns)|*.lns");

	const std::wstring wildcard = allWildcard + std::wstring(L"|") + lnsWildcard;

	wxFileDialog dialog(this, L"Select line file...", L"", L"", wildcard.c_str(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	loadLines(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
}

void IAMainWindow::onLoadImageOrLines(wxCommandEvent& /*event*/)
{
	const std::wstring bmpWildcard(L"Bitmap files (*.bmp)|*.bmp");
	const std::wstring gifWildcard(L"GIF files (*.gif)|*.gif");
	const std::wstring jpgWildcard(L"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg");
	const std::wstring pngwildCard(L"PNG files (*.png)|*.png");

	std::wstring allWildcard(L"All supported files|*.bmp;*.gif;*.jpg;*.jpeg;*.png");
	std::wstring wildcard = bmpWildcard + std::wstring(L"|") + gifWildcard + std::wstring(L"|") + jpgWildcard + std::wstring(L"|") + pngwildCard;

	if (image_)
	{
		const std::wstring lnsWildcard(L"Line files (*.lns)|*.lns");

		allWildcard += L";*.lns";
		wildcard += std::wstring(L"|") + lnsWildcard;

		if (!LineManager::get().isEmpty())
		{
			const std::wstring grsWildcard(L"Group files (*.grs)|*.grs");

			allWildcard += L";*.grs";
			wildcard += std::wstring(L"|") + grsWildcard;
		}
	}

	const std::wstring combinedWildcard = allWildcard + std::wstring(L"|") + wildcard;

	wxFileDialog dialog(this, L"Select file...", L"", L"", combinedWildcard.c_str(), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

	if (wxID_OK != dialog.ShowModal())
	{
		return;
	}

	const IO::File file(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));

	if (file.extension() == "lns")
	{
		loadLines(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
	}
	else if (file.extension() == "grs")
	{
		loadGroups(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
	}
	else
	{
		loadImage(Platform::WxWidgets::Utilities::toAString(dialog.GetPath()));
	}
}

void IAMainWindow::onIconLine(wxCommandEvent& /*event*/)
{
	if (interactionMode_ == IM_LINE)
	{
		GetToolBar()->ToggleTool(ID_Icon_Line, false);
		interactionMode_ = IM_NONE;
	}
	else
	{
		GetToolBar()->ToggleTool(ID_Icon_Select, false);
		GetToolBar()->ToggleTool(ID_Icon_Line, true);
		interactionMode_ = IM_LINE;
	}

	imageWindow_->setInteractionMode(interactionMode_);
}

void IAMainWindow::onIconSelect(wxCommandEvent& /*event*/)
{
	if (interactionMode_ == IM_SELECT)
	{
		GetToolBar()->ToggleTool(ID_Icon_Select, false);
		interactionMode_ = IM_NONE;
	}
	else
	{
		GetToolBar()->ToggleTool(ID_Icon_Line, false);
		GetToolBar()->ToggleTool(ID_Icon_Select, true);
		interactionMode_ = IM_SELECT;
	}

	imageWindow_->setInteractionMode(interactionMode_);
}

void IAMainWindow::onNoZoom(wxCommandEvent& /*event*/)
{
	ocean_assert(imageWindow_);
	imageWindow_->setZoom(1);
}

void IAMainWindow::onToggleShowLines(wxCommandEvent& /*event*/)
{
	const bool showLines = GetToolBar()->GetToolState(ID_Toggle_Show_Lines);

	ocean_assert(imageWindow_);
	imageWindow_->enableAnnotation(showLines);
}

void IAMainWindow::onToggleShowGradients(wxCommandEvent& /*event*/)
{
	const bool showOriginal = !GetToolBar()->GetToolState(ID_Toggle_Show_Gradients);
	GetToolBar()->ToggleTool(ID_Toggle_Show_Gradients_Plus, false);

	ocean_assert(imageWindow_);
	if (!imageWindow_->setFrame(showOriginal ? image_ : gradientImage_, false))
	{
		wxMessageBox(std::wstring(L"Failed to update the image"), L"Error", wxOK | wxICON_ERROR, this);
	}
}

void IAMainWindow::onToggleShowGradientsPlus(wxCommandEvent& /*event*/)
{
	const bool showOriginal = !GetToolBar()->GetToolState(ID_Toggle_Show_Gradients_Plus);
	GetToolBar()->ToggleTool(ID_Toggle_Show_Gradients, false);

	ocean_assert(imageWindow_);
	if (!imageWindow_->setFrame(showOriginal ? image_ : gradientImagePlus_, false))
	{
		wxMessageBox(std::wstring(L"Failed to update the image"), L"Error", wxOK | wxICON_ERROR, this);
	}
}

void IAMainWindow::onDetectLinesLSD(wxCommandEvent& /*event*/)
{
	if (wxMessageBox(std::wstring(L"Do you really want to add automatically detected lines?"), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxYES)
	{
		Frame frameY8;
		if (!CV::FrameConverter::Comfort::convert(image_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, frameY8, false, WorkerPool::get().scopedWorker()()))
		{
			wxMessageBox(std::wstring(L"Load a valid image first"), L"Error", wxOK | wxICON_ERROR, this);
			return;
		}

		// let's first add the currently existing lines to a new group

		LineManager::LineIds previousLineIds;
		LineManager::get().allLinesInGroup((unsigned int)(-1), LineManager::invalidId, &previousLineIds);

		if (!previousLineIds.empty())
		{
			const unsigned int newGroupIndex = LineManager::get().addGroup();
			LineManager::get().addLinesToGroup(previousLineIds.data(), previousLineIds.size(), newGroupIndex);
		}

		ocean_assert(frameY8);
		cv::Mat cvImage = CV::OpenCVUtilities::toCvMat(frameY8, true);

		cv::Ptr<cv::LineSegmentDetector> lineSegmentDetector = cv::createLineSegmentDetector(cv::LSD_REFINE_STD);

		std::vector<cv::Vec4f> cvLines;
		lineSegmentDetector->detect(cvImage, cvLines);

		unsigned int linesSkipped = 0u;

		std::vector<LineManager::LineId> newLineIds;
		newLineIds.reserve(cvLines.size());

		for (const cv::Vec4f& cvLine : cvLines)
		{
			const FiniteLineD2 line(VectorD2(cvLine[0], cvLine[1]), VectorD2(cvLine[2], cvLine[3]));

			if (line.sqrLength() >= NumericD::sqr(10))
			{
				newLineIds.push_back(LineManager::get().addLine(line));
			}
			else
			{
				linesSkipped++;
			}
		}

		// let's add the detected lines to another new group

		ocean_assert(imageWindow_);
		imageWindow_->resetInteractionData();

		const unsigned int newGroupIndex = LineManager::get().addGroup();
		LineManager::get().addLinesToGroup(newLineIds.data(), newLineIds.size(), newGroupIndex);

		imageWindow_->Refresh();

		wxMessageBox(String::toWString(cvLines.size()) + std::wstring(L" lines have been detected, ") + String::toWString(linesSkipped) + std::wstring(L" have been skipped as shorter than 10 pixels"), L"Information", wxOK | wxICON_INFORMATION, this);
	}
}

void IAMainWindow::onDetectLinesULF(wxCommandEvent& /*event*/)
{
	if (wxMessageBox(std::wstring(L"Do you really want to add automatically detected lines?"), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxYES)
	{
		Frame frameY8;

		if (!CV::FrameConverter::Comfort::convert(image_, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, frameY8, false, WorkerPool::get().scopedWorker()()))
		{
			wxMessageBox(std::wstring(L"Load a valid image first"), L"Error", wxOK | wxICON_ERROR, this);
			return;
		}

		// let's first add the currently existing lines to a new group

		LineManager::LineIds previousLineIds;
		LineManager::get().allLinesInGroup((unsigned int)(-1), LineManager::invalidId, &previousLineIds);

		if (!previousLineIds.empty())
		{
			const unsigned int newGroupIndex = LineManager::get().addGroup();
			LineManager::get().addLinesToGroup(previousLineIds.data(), previousLineIds.size(), newGroupIndex);
		}

		ocean_assert(frameY8);
		const FiniteLines2 linesULF = CV::Detector::LineDetectorULF::detectLines(frameY8.constdata<uint8_t>(), frameY8.width(), frameY8.height(), frameY8.paddingElements(), CV::Detector::LineDetectorULF::defaultEdgeDetectors(), 50u, 5u);

		unsigned int linesSkipped = 0u;

		std::vector<LineManager::LineId> newLineIds;
		newLineIds.reserve(linesULF.size());

		for (const FiniteLine2& lineULF : linesULF)
		{
			if (lineULF.sqrLength() >= NumericD::sqr(10))
			{
				newLineIds.push_back(LineManager::get().addLine(FiniteLineD2(lineULF)));
			}
			else
			{
				linesSkipped++;
			}
		}

		// let's add the detected lines to another new group

		const unsigned int newGroupIndex = LineManager::get().addGroup();
		LineManager::get().addLinesToGroup(newLineIds.data(), newLineIds.size(), newGroupIndex);

		ocean_assert(imageWindow_);
		imageWindow_->Refresh();

		wxMessageBox(String::toWString(linesULF.size()) + std::wstring(L" lines have been detected, ") + String::toWString(linesSkipped) + std::wstring(L" have been skipped as shorter than 10 pixels"), L"Information", wxOK | wxICON_INFORMATION, this);
	}
}

void IAMainWindow::onRemoveAllLines(wxCommandEvent& /*event*/)
{
	if (LineManager::get().isEmpty())
	{
		wxMessageBox(L"Currently, no lines exist that could be removed.", L"Information", wxOK | wxICON_INFORMATION, this);
	}
	else
	{
		if (wxMessageBox(std::wstring(L"Do you really want to remove all lines?"), L"Information", wxICON_INFORMATION | wxYES_NO, this) == wxYES)
		{
			ocean_assert(imageWindow_);
			imageWindow_->resetInteractionData();

			LineManager::get().clear();

			imageWindow_->setLineEvaluationMap(LineEvaluator::LineMatchMap());
			imageWindow_->Refresh();
		}
	}
}

void IAMainWindow::onEvaluateLines(wxCommandEvent& /*event*/)
{
	if (LineManager::get().groups() != 2u)
	{
		wxMessageBox(L"You need two individual groups of lines before the evaluation can be started.", L"Information", wxOK | wxICON_INFORMATION, this);
		return;
	}

	LineManager::LineIds lineIdsGroundTruth;
	FiniteLinesD2 linesGroundTruth = LineManager::get().allLinesInGroup(0u, LineManager::invalidId, &lineIdsGroundTruth);

	// we remove all ground truth lines short than 10 pixels (as we have not detected them via LSD/ULF)
	for (size_t n = 0; n < linesGroundTruth.size(); ++n)
	{
		if (linesGroundTruth[n].sqrLength() < NumericD::sqr(10))
		{
			LineManager::get().removeLine(lineIdsGroundTruth[n]);
		}
	}

	// we extract the ground truth lines once again

	lineIdsGroundTruth.clear();
	linesGroundTruth = LineManager::get().allLinesInGroup(0u, LineManager::invalidId, &lineIdsGroundTruth);

	LineManager::LineIds lineIdsEvaluation;
	const FiniteLinesD2 linesEvaluation = LineManager::get().allLinesInGroup(1u, LineManager::invalidId, &lineIdsEvaluation);

	if (linesGroundTruth.empty() || linesEvaluation.empty())
	{
		wxMessageBox(L"Each group of lines must contain at least one line before the evaluation can be started.", L"Information", wxOK | wxICON_INFORMATION, this);
		return;
	}

	// we connect each line id with the corresponding line

	std::unordered_map<LineEvaluator::Id, FiniteLineD2> lineMapGroundTruth;
	for (size_t n = 0; n < lineIdsGroundTruth.size(); ++n)
	{
		lineMapGroundTruth.insert(std::make_pair(lineIdsGroundTruth[n], linesGroundTruth[n]));
	}

	std::unordered_map<LineEvaluator::Id, FiniteLineD2> lineMapEvaluation;
	for (size_t n = 0; n < lineIdsEvaluation.size(); ++n)
	{
		lineMapEvaluation.insert(std::make_pair(lineIdsEvaluation[n], linesEvaluation[n]));
	}

	ocean_assert(linesGroundTruth.size() == lineMapGroundTruth.size());
	ocean_assert(linesEvaluation.size() == lineMapEvaluation.size());

	LineEvaluator::LineMatchMap lineMatches = LineEvaluator::evaluateLineSegments(lineMapGroundTruth, lineMapEvaluation);

	if (!lineMatches.empty())
	{
		double coverage;
		double medianAngle;
		double medianDistance;

		size_t countPerfectMatches;
		size_t countPartialMatches;
		size_t countComplexMatches;

		size_t notCoveredGroundTruthLines;
		size_t notCoveredEvaluationLines;

		if (LineEvaluator::evaluateLineMatches(lineMapGroundTruth, lineMapEvaluation, lineMatches, coverage, medianAngle, medianDistance, countPerfectMatches, countPartialMatches, countComplexMatches, notCoveredGroundTruthLines, notCoveredEvaluationLines))
		{
			std::string message = "The result of the evaluation:\n\n";

			message += "Ground truth lines: " + String::toAString(linesGroundTruth.size()) + "\n";
			message += "Evaluation lines: " + String::toAString(lineMapEvaluation.size()) + "\n\n";

			message += "Overall coverage: " + String::toAString(coverage * 100.0, 1u) + "%\n",
			message += "Median angle: " + String::toAString(NumericD::rad2deg(medianAngle), 1u) + "deg\n",
			message += "Median distance: " + String::toAString(medianDistance, 1u) + "px\n",
			message += "Unmatched (ground truth): " + String::toAString(notCoveredGroundTruthLines) + ", (" + String::toAString(double(notCoveredGroundTruthLines) * 100.0 / double(linesGroundTruth.size()), 1u) + "%)\n";
			message += "Unmatched evaluation lines: " + String::toAString(notCoveredEvaluationLines) + ", (" + String::toAString(double(notCoveredEvaluationLines) * 100.0 / double(lineMapEvaluation.size()), 1u) + "%)\n\n";

			message += "Number perfect matches: " + String::toAString(countPerfectMatches) + ", (" + String::toAString(double(countPerfectMatches) * 100.0 / double(linesGroundTruth.size()), 1u) + "%)\n";
			message += "Number partial matches: " + String::toAString(countPartialMatches) + ", (" + String::toAString(double(countPartialMatches) * 100.0 / double(linesGroundTruth.size()), 1u) + "%)\n";
			message += "Number complex matches: " + String::toAString(countComplexMatches) + ", (" + String::toAString(double(countComplexMatches) * 100.0 / double(linesGroundTruth.size()), 1u) + "%)";

			wxMessageBox(String::toWString(message), L"Summary", wxOK | wxICON_INFORMATION, this);
		}
	}
	else
	{
		wxMessageBox(L"No matches found.", L"Information", wxOK | wxICON_INFORMATION, this);
	}

	ocean_assert(imageWindow_);
	imageWindow_->setLineEvaluationMap(std::move(lineMatches));
}

void IAMainWindow::onClusterLines(wxCommandEvent& /*event*/)
{
	ocean_assert(imageWindow_);
	const LineManager::LineIdSet selectedLineIds = imageWindow_->interactionLineIds();

	const VectorD3 vanishingPoint = determineVanishingPoint(LineManager::get().lines(selectedLineIds));

	LineManager::LineIds lineIds;
	const FiniteLinesD2 lines = LineManager::get().allLinesInGroup((unsigned int)(-1), LineManager::invalidId, &lineIds);

	// the selected lines are part of the new cluster in any way
	LineManager::LineIdSet clusteredLineIds = selectedLineIds;

	const double parallelLinesTresholdAngleCos = NumericD::cos(NumericD::deg2rad(3));

	if (vanishingPoint.z() == 1.0)
	{
		// finite vanishing point

		const VectorD2 finitePointDirection(vanishingPoint.xy());

		for (size_t n = 0; n < lines.size(); ++n)
		{
			const FiniteLineD2& line = lines[n];

			const VectorD2 lineCenter((line.point0() + line.point1()) * 0.5);

			if (Numeric::abs((lineCenter - finitePointDirection).normalizedOrZero() * line.direction())  >= parallelLinesTresholdAngleCos)
			{
				clusteredLineIds.insert(lineIds[n]);
			}
		}
	}
	else
	{
		// infinite vanishing point

		const VectorD2 vanishingPointDirection(vanishingPoint.xy());

		for (size_t n = 0; n < lines.size(); ++n)
		{
			if (Numeric::abs(lines[n].direction() * vanishingPointDirection) >= parallelLinesTresholdAngleCos)
			{
				clusteredLineIds.insert(lineIds[n]);
			}
		}
	}

	const unsigned int newGroupIndex = LineManager::get().addGroup();

	LineManager::get().addLinesToGroup(clusteredLineIds, newGroupIndex);

	ocean_assert(imageWindow_);
	imageWindow_->Update();
	imageWindow_->Refresh();
}

void IAMainWindow::onQuit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void IAMainWindow::onClose(wxCloseEvent& event)
{
	if (!LineManager::get().isEmpty() && LineManager::get().hasContentChanged())
	{
		const int selection = wxMessageBox(std::wstring(L"You have unsaved changed, do you want to save the modifications before closing the application?"), L"Information", wxICON_INFORMATION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT, this);

		if (selection == wxCANCEL)
		{
			return;
		}

		if (selection == wxYES)
		{
			if (saveLines() == false)
			{
				return;
			}

			if (LineManager::get().groups() != 0u)
			{
				if (saveGroups() == false)
				{
					return;
				}
			}
		}
	}

	// proceed closing the application
	event.Skip();
}

void IAMainWindow::onAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox(L"This is simple WxWidgets-based application allowing to annotate images", L"About ImageAnnotator", wxOK | wxICON_INFORMATION, this);
}

bool IAMainWindow::onFileDragAndDrop(const std::vector<std::string>& files)
{
	if (files.empty())
	{
		return false;
	}

	const IO::File file(files.front());

	if (file.extension() == "lns")
	{
		return loadLines(file());
	}
	else if (file.extension() == "grs")
	{
		return loadGroups(file());
	}
	else
	{
		return loadImage(file());
	}
}

bool IAMainWindow::createGradientImage(const Frame& frame, Frame& gradientFrame, Frame& gradientFramePlus)
{
	ocean_assert(frame.isValid());
	ocean_assert(&frame != &gradientFrame);
	ocean_assert(&frame != &gradientFramePlus);
	ocean_assert(&gradientFrame != &gradientFramePlus);

	Frame yFrame;

	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		return false;
	}

	if (!gradientFrame.set(yFrame.frameType(), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	CV::FrameFilterScharr::filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t, 1u>(yFrame.constdata<uint8_t>(), gradientFrame.data<uint8_t>(), gradientFrame.width(), gradientFrame.height(), yFrame.paddingElements(), gradientFrame.paddingElements(), WorkerPool::get().scopedWorker()());

	// let's invert the gradients to improve visibility
	CV::FrameInverter::invert(gradientFrame, WorkerPool::get().scopedWorker()());

	// we create an additional gradient image with minimal gradients

	gradientFramePlus.copy(gradientFrame);
	uint8_t* gradientFramePlusData = gradientFramePlus.data<uint8_t>();

	for (unsigned int n = 0u; n < gradientFramePlus.pixels(); ++n)
	{
		if (gradientFramePlusData[n] > 255u - 25u) // so we display gradients >= 25 only
		{
			gradientFramePlusData[n] = 255u;
		}
	}

	return true;
}

VectorD3 IAMainWindow::determineVanishingPoint(const FiniteLinesD2& lines, const double parallelLinesTresholdAngle)
{
	ocean_assert(lines.size() >= 2);

	ocean_assert(parallelLinesTresholdAngle > 0.0 && parallelLinesTresholdAngle <= NumericD::pi_2());

	const double parallelLinesTresholdAngleCos = NumericD::cos(parallelLinesTresholdAngle);

	LinesD2 infiniteLines;
	infiniteLines.reserve(lines.size());

	for (const FiniteLineD2& line : lines)
	{
		ocean_assert(line.isValid());
		infiniteLines.emplace_back(line.point0(), line.direction());
	}

	VectorsD2 intersections;
	intersections.reserve((infiniteLines.size() * (infiniteLines.size() - 1)) / 2);

	std::vector<double> parallelAngles;
	parallelAngles.reserve(infiniteLines.size() * (infiniteLines.size() - 1));

	for (size_t nOuter = 0; nOuter < infiniteLines.size() - 1; ++nOuter)
	{
		const LineD2& outerInfiniteLine = infiniteLines[nOuter];
		const FiniteLineD2& outerFiniteLine = lines[nOuter];

		for (size_t nInner = nOuter + 1; nInner < infiniteLines.size(); ++nInner)
		{
			const LineD2& innerInfiniteLine = infiniteLines[nInner];
			const FiniteLineD2& innerFiniteLine = lines[nInner];

			const double absCosValue = NumericD::abs(outerFiniteLine.direction() * innerFiniteLine.direction());

			if (absCosValue >= parallelLinesTresholdAngleCos)
			{
				// angles with range [0, PI)
				parallelAngles.push_back(NumericD::angleAdjustPositiveHalf(NumericD::atan2(outerFiniteLine.direction().y(), outerFiniteLine.direction().x())));
				parallelAngles.push_back(NumericD::angleAdjustPositiveHalf(NumericD::atan2(innerFiniteLine.direction().y(), innerFiniteLine.direction().x())));
			}
			else
			{
				VectorD2 intersection;
				if (outerInfiniteLine.intersection(innerInfiniteLine, intersection))
				{
					intersections.push_back(intersection);
				}
				else
				{
					ocean_assert(false && "Should never happen!");
				}
			}
		}
	}

	if (intersections.size() >= parallelAngles.size())
	{
		// we seem to have a finite vanishing point

		ocean_assert(!intersections.empty());

		VectorD2 bestIntersection(NumericD::minValue(), NumericD::minValue());
		double bestSumSqrDistances = NumericD::maxValue();

		for (const VectorD2& intersection : intersections)
		{
			double sumSqrDistances = 0.0;

			for (const LineD2& infiniteLine : infiniteLines)
			{
				sumSqrDistances += infiniteLine.sqrDistance(intersection);
			}

			if (sumSqrDistances < bestSumSqrDistances)
			{
				bestSumSqrDistances = sumSqrDistances;
				bestIntersection = intersection;
			}
		}

		ocean_assert(bestSumSqrDistances != NumericD::maxValue());

		return VectorD3(bestIntersection, 1.0);
	}
	else
	{
		// we seem to have an infinite vanishing point

		ocean_assert(!parallelAngles.empty());

		const size_t initialAnglesCounter = parallelAngles.size();
		for (size_t n = 0; n < initialAnglesCounter; ++n)
		{
			if (parallelAngles[n] <= 5)
			{
				parallelAngles.push_back(parallelAngles[n] + NumericD::pi());
			}
			else if (parallelAngles[n] >= NumericD::pi() - 5)
			{
				parallelAngles.push_back(parallelAngles[n] - NumericD::pi());
			}
		}

		const double angle = Median::median(parallelAngles.data(), parallelAngles.size());

		const VectorD2 lineDirection = VectorD2(NumericD::cos(angle), NumericD::sin(angle));

		return VectorD3(lineDirection, 0);
	}
}
