/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/imageannotator/wxw/IAImageWindow.h"
#include "application/ocean/demo/misc/imageannotator/wxw/IAMainWindow.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/math/Box2.h"
#include "ocean/math/Line2.h"

#include "ocean/platform/wxwidgets/Utilities.h"
#include "ocean/platform/wxwidgets/WxPopupMenu.h"

using namespace Ocean::CV::Detector;

// WxWidgets specific implementation of the event table
BEGIN_EVENT_TABLE(IAImageWindow, Platform::WxWidgets::BitmapWindow)
	EVT_KEY_DOWN(IAImageWindow::onKeyDown)
	EVT_KEY_UP(IAImageWindow::onKeyUp)
END_EVENT_TABLE()

IAImageWindow::IAImageWindow(wxWindow* parent) :
	BitmapWindow(L"Surface", parent)
{
#ifdef _WINDOWS
	SetDoubleBuffered(true);
#endif
}

void IAImageWindow::enableAnnotation(const bool enable)
{
	if (annotationEnabled_ == enable)
	{
		return;
	}

	annotationEnabled_ = enable;

	Refresh();
}

void IAImageWindow::setLineEvaluationMap(CV::Detector::LineEvaluator::LineMatchMap&& lineEvaluationMap)
{
	lineEvaluationMap_ = std::move(lineEvaluationMap);

	lineEvaluationReverseMap_.clear();

	for (LineEvaluator::LineMatchMap::const_iterator i = lineEvaluationMap_.cbegin(); i != lineEvaluationMap_.cend(); ++i)
	{
		switch (i->second->matchType())
		{
			case LineEvaluator::LineMatch::MT_PERFECT:
			{
				const LineEvaluator::PerfectLineMatch& perfectMatch = dynamic_cast<const LineEvaluator::PerfectLineMatch&>(*i->second);

				lineEvaluationReverseMap_[perfectMatch.targetId()].insert(i->first);

				break;
			}

			case LineEvaluator::LineMatch::MT_PARTIAL:
			{
				const LineEvaluator::PartialLineMatch& partialMatch = dynamic_cast<const LineEvaluator::PartialLineMatch&>(*i->second);

				for (const LineEvaluator::Id& targetId : partialMatch.targetIds())
				{
					lineEvaluationReverseMap_[targetId].insert(i->first);
				}

				break;
			}

			case LineEvaluator::LineMatch::MT_COMPLEX:
			{
				const LineEvaluator::ComplexLineMatch& complexMatch = dynamic_cast<const LineEvaluator::ComplexLineMatch&>(*i->second);

				for (const LineEvaluator::Id& targetId : complexMatch.targetIds())
				{
					lineEvaluationReverseMap_[targetId].insert(i->first);
				}

				break;
			}

			default:
				ocean_assert(false && "Missing implementation!");
				break;
		}
	}
}

void IAImageWindow::resetInteractionData()
{
	lineStartPoint_ = VectorD2(0.0, 0.0);

	lineIds_.clear();

	lineState_ = LS_IDLE;
	selectState_ = SS_IDLE;

	closestLineId_ = LineManager::invalidId;
	linePointIndex_ = LineManager::invalidId;

	interactionCursorCurrent_ = VectorD2(0.0, 0.0);
	interactionCursorOffset_ = VectorD2(0.0, 0.0);
}

void IAImageWindow::onMouseLeftDown(wxMouseEvent& event)
{
	BitmapWindow::onMouseLeftDown(event);

	if (bitmap_.IsOk())
	{
		Scalar xBitmap, yBitmap;
		if (window2bitmap(event.GetPosition().x, event.GetPosition().y, xBitmap, yBitmap, true))
		{
			const VectorD2 currentPosition = VectorD2(double(xBitmap), double(yBitmap));

			switch (interactionMode_)
			{
				case IM_LINE:
				{
					switch (lineState_)
					{
						case LS_IDLE:
						{
							if (linePointIndex_ != LineManager::invalidId)
							{
								ocean_assert(lineIds_.size() == 1);

								lineState_ = LS_ADJUSTING_POINT;

								const FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());

								const VectorD2& linePoint = linePointIndex_ == 0u ? line.point0() : line.point1();
								interactionCursorOffset_ = linePoint - currentPosition;

								Refresh();
							}
							else if (lineIds_.size() == 1)
							{
								lineState_ = LS_MOVE_LINE;

								const FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());

								const VectorD2& linePoint = line.point0();
								interactionCursorOffset_ = linePoint - currentPosition;

								Refresh();
							}
							else
							{
								ocean_assert(lineIds_.empty());
								ocean_assert(linePointIndex_ == LineManager::invalidId);

								lineIds_.clear();

								lineState_ = LS_DEFINING_START_POINT;
								interactionCursorOffset_ = VectorD2(0.0, 0.0);
							}

							break;
						}

						default:
							break;
					}

					break;
				}

				case IM_SELECT:
				{
					switch (selectState_)
					{
						case SS_IDLE:
						{
							selectionFirstCorner_ = currentPosition;
							interactionCursorOffset_ = VectorD2(0.0, 0.0);

							lineIds_.clear();

							selectState_ = SS_SELECTION_STARTED;

							break;
						}

						default:
							break;
					}

					break;
				}

				default:
					break;
			}
		}
	}
}

void IAImageWindow::onMouseRightDown(wxMouseEvent& event)
{
	BitmapWindow::onMouseRightDown(event);

	switch (interactionMode_)
	{
		case IM_LINE:
		{
			switch (lineState_)
			{
				case LS_DEFINING_END_POINT:
				{
					ocean_assert(lineIds_.empty());
					ocean_assert(linePointIndex_ == LineManager::invalidId);

					// the user is currently defining the end point of a new line,
					// however clicking the right mouse button stopps this interaction

					lineState_ = LS_IDLE;
					lineStartPoint_ = VectorD2(0.0, 0.0);

					Refresh();

					break;
				}

				default:
					break;
			}

			break;
		}

		default:
			break;
	}
}

void IAImageWindow::onMouseRightDblClick(wxMouseEvent& event)
{
	BitmapWindow::onMouseRightDblClick(event);

	// nothing to do here
}

void IAImageWindow::onMouseMove(wxMouseEvent& event)
{
	if (bitmap_.IsOk())
	{
		const LineManager::LineIdSet previousLineIds(lineIds_);
		const unsigned int previousLinePointIndex = linePointIndex_;

		Scalar xBitmap, yBitmap;
		if (window2bitmap(event.GetPosition().x, event.GetPosition().y, xBitmap, yBitmap, true))
		{
			interactionCursorCurrent_ = VectorD2(double(xBitmap), double(yBitmap));

			const double zoomRespectingMaximalInteractionDistance = NumericD::ratio(maximalInteractionDistance_, zoom_);

			const LineManager::LineId previousClosestLineId(closestLineId_);
			closestLineId_ = LineManager::get().findLine(interactionCursorCurrent_, zoomRespectingMaximalInteractionDistance);

			switch (interactionMode_)
			{
				case IM_LINE:
				{
					switch (lineState_)
					{
						case LS_IDLE:
						{
							// the user may want to start a new line close to an endpoint of an already existing line, we support this via the menu key
							if (altKeyDown_ == false)
							{
								lineIds_.clear();

								const LineManager::LineId lineId = LineManager::get().findLine(interactionCursorCurrent_, zoomRespectingMaximalInteractionDistance, &linePointIndex_);

								if (lineId != LineManager::invalidId)
								{
									lineIds_.insert(lineId);
								}
							}

							break;
						}

						default:
							break;
					}

					if (lineState_ != LS_IDLE || (lineIds_ != previousLineIds || linePointIndex_ != previousLinePointIndex))
					{
						Refresh();
					}

					break;
				}

				case IM_SELECT:
				{
					switch (selectState_)
					{
						case SS_IDLE:
						{
							if (previousClosestLineId != closestLineId_)
							{
								Refresh();
							}

							break;
						}

						case SS_SELECTION_STARTED:
						{
							if (selectionFirstCorner_.sqrDistance(interactionCursorCurrent_) > NumericD::sqr(2))
							{
								// the user actually wants to select several lines via a bounding box
								selectState_ = SS_DEFINING_SECOND_CORNER;
							}

							break;
						}

						case SS_DEFINING_SECOND_CORNER:
						{
							const BoxD2 selectionBox(selectionFirstCorner_, interactionCursorCurrent_);

							if (selectionBox.isValid() && !selectionBox.isPoint())
							{
								lineIds_ = LineManager::get().findLines(selectionBox);
							}
							else
							{
								lineIds_.clear();
							}

							Refresh();

							break;
						}

						case SS_SELECTING_INDIVIDUAL_LINES:
						{
							if (previousClosestLineId != closestLineId_)
							{
								Refresh();
							}

							break;
						}

						default:
							break;
					}

					break;
				}

				default:
					break;
			}
		}
	}

	BitmapWindow::onMouseMove(event);
}

void IAImageWindow::onMouseLeftUp(wxMouseEvent& event)
{
	BitmapWindow::onMouseLeftUp(event);

	if (bitmap_.IsOk())
	{
		Scalar xBitmap, yBitmap;
		if (window2bitmap(event.GetPosition().x, event.GetPosition().y, xBitmap, yBitmap, true))
		{
			const VectorD2 currentLocation = VectorD2(double(xBitmap), double(yBitmap));

			switch (interactionMode_)
			{
				case IM_LINE:
				{
					switch (lineState_)
					{
						case LS_DEFINING_START_POINT:
						{
							ocean_assert(lineIds_.empty());
							ocean_assert(linePointIndex_ == LineManager::invalidId);

							lineState_ = LS_DEFINING_END_POINT;
							lineStartPoint_ = currentLocation;

							break;
						}

						case LS_DEFINING_END_POINT:
						{
							ocean_assert(lineIds_.empty());
							ocean_assert(linePointIndex_ == LineManager::invalidId);

							lineState_ = LS_IDLE;

							FiniteLineD2 line(lineStartPoint_, currentLocation);

							if (line.isValid())
							{
								line = clampLine(line, (unsigned int)bitmap_.GetWidth(), (unsigned int)bitmap_.GetHeight());

								if (line.isValid() && line.sqrLength() >= NumericD::sqr(minimalLineLength_))
								{
									LineManager::get().addLine(line);
								}
							}

							break;
						}

						case LS_ADJUSTING_POINT:
						{
							ocean_assert(lineIds_.size() == 1);
							ocean_assert(linePointIndex_ <= 1u);

							lineState_ = LS_IDLE;

							const VectorD2 newPostion = currentLocation + interactionCursorOffset_;

							FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());
							ocean_assert(line.isValid());

							if (linePointIndex_ == 0u)
							{
								line = FiniteLineD2(newPostion, line.point1());
							}
							else
							{
								line = FiniteLineD2(line.point0(), newPostion);
							}

							line = clampLine(line, (unsigned int)bitmap_.GetWidth(), (unsigned int)bitmap_.GetHeight());

							if (line.isValid() && line.sqrLength() >= NumericD::sqr(minimalLineLength_))
							{
								LineManager::get().updateLine(*lineIds_.begin(), line);
							}

							lineIds_.clear();
							linePointIndex_ = LineManager::invalidId;

							break;
						}

						case LS_MOVE_LINE:
						{
							ocean_assert(lineIds_.size() == 1);
							ocean_assert(linePointIndex_ == LineManager::invalidId);

							lineState_ = LS_IDLE;

							FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());
							ocean_assert(line.isValid());

							const VectorD2 newPoint0 = currentLocation + interactionCursorOffset_;
							const VectorD2 newPoint1 = newPoint0 + line.point1() - line.point0();

							line = FiniteLineD2(newPoint0, newPoint1);
							ocean_assert(line.isValid());

							// finally we want to ensure that the end points of the lines are located inside the image

							line = clampLine(line, (unsigned int)bitmap_.GetWidth(), (unsigned int)bitmap_.GetHeight());

							if (line.isValid() && line.sqrLength() >= NumericD::sqr(minimalLineLength_))
							{
								LineManager::get().updateLine(*lineIds_.begin(), line);
							}

							lineIds_.clear();
							linePointIndex_ = LineManager::invalidId;

							break;
						}

						default:
							break;
					}

					Refresh();

					break;
				}

				case IM_SELECT:
				{
					switch (selectState_)
					{
						case SS_SELECTION_STARTED:
						{
							if (selectionFirstCorner_.sqrDistance(currentLocation) <= NumericD::sqr(2))
							{
								// the user actually selected a single line by clicking without moving the cursor

								lineIds_.clear();

								if (closestLineId_ != LineManager::invalidId)
								{
									lineIds_.insert(closestLineId_);
								}

								selectState_ = SS_IDLE;

								Refresh();
							}

							break;
						}

						case SS_DEFINING_SECOND_CORNER:
						{
							selectState_ = SS_IDLE;

							Refresh();

							break;
						}

						case SS_SELECTING_INDIVIDUAL_LINES:
						{
							if (closestLineId_ != LineManager::invalidId)
							{
								LineManager::LineIdSet::iterator i = lineIds_.find(closestLineId_);

								if (i != lineIds_.end())
								{
									lineIds_.erase(i);
									closestLineId_ = LineManager::invalidId;
								}
								else
								{
									lineIds_.insert(closestLineId_);
								}

								Refresh();
							}

							break;
						}

						default:
							break;
					}

					break;
				}

				default:
					break;
			}
		}
		else
		{
			// now valid cursor position

			lineState_ = LS_IDLE;

			lineIds_.clear();
			linePointIndex_ = LineManager::invalidId;
		}
	}

	interactionCursorOffset_ = VectorD2(0, 0);
}

void IAImageWindow::onMouseRightUp(wxMouseEvent& event)
{
	BitmapWindow::onMouseRightUp(event);

	switch (interactionMode_)
	{
		case IM_SELECT:
		{
			switch (selectState_)
			{
				case SS_IDLE:
				{
					if (!lineIds_.empty())
					{
						Platform::WxWidgets::WxPopupMenu popupMenu;

						popupMenu.Append(1, "Delete line(s)"); // on OSX we must nost use id 0
						popupMenu.AppendSeparator();
						popupMenu.Append(2, "Add to new group");
						popupMenu.Append(3, "Remove from group(s)");

						if (LineManager::get().groups() != 0u)
						{
							popupMenu.AppendSeparator();

							for (unsigned int n = 0u; n < LineManager::get().groups(); ++n)
							{
								popupMenu.Append(4 + int(n), std::string("Add to group ") + String::toAString(n + 1));
							}
						}

						const int menuId = popupMenu.popup(*this);

						if (menuId == -1)
						{
							break;
						}

						switch (menuId)
						{
							case 1:
							{
								LineManager::get().removeLines(lineIds_);
								lineIds_.clear();
								break;
							}

							case 2:
							{
								const unsigned int newGroupIndex = LineManager::get().addGroup();
								LineManager::get().addLinesToGroup(lineIds_, newGroupIndex);
								break;
							}

							case 3:
							{
								LineManager::get().addLinesToGroup(lineIds_, LineManager::invalidId);
								break;
							}

							default:
							{
								ocean_assert(menuId >= 4);
								LineManager::get().addLinesToGroup(lineIds_, (unsigned int)(menuId - 4));
							}
						}

						lineIds_.clear();

						Refresh();
					}

					break;
				}

				default:
					break;
			}

			break;
		}

		default:
			break;
	}
}

void IAImageWindow::onPaintOverlay(wxPaintEvent& /*event*/, wxPaintDC& dc)
{
	std::unique_ptr<wxGraphicsContext> graphicsContext(wxGraphicsContext::Create(dc));

	if (!graphicsContext)
	{
		return;
	}

	if (annotationEnabled_)
	{
		// first we draw all lines existing in the manager

		LineManager::LineId additionalExcludedLineId = LineManager::invalidId;

		if (closestLineId_ != LineManager::invalidId && interactionMode_ == IM_SELECT && selectState_ == SS_SELECTING_INDIVIDUAL_LINES)
		{
			// we also want to highlight this line
			additionalExcludedLineId = closestLineId_;

			drawLine(*graphicsContext, LineManager::get().line(closestLineId_), DM_HIGHLIGHT);
		}

		if (LineManager::get().groups() == 0u)
		{
			drawLines(*graphicsContext, LineManager::get().allLines(lineIds_, additionalExcludedLineId), DM_NORMAL);
		}
		else
		{
			// first all lines not associated with a group
			drawLines(*graphicsContext, LineManager::get().allLinesInGroup(LineManager::invalidId, lineIds_, additionalExcludedLineId), DM_NORMAL);

			// now all lines associated with a group

			const wxColour black(0x00, 0x00, 0x00);

			for (unsigned int n = 0u; n < LineManager::get().groups(); ++n)
			{
				drawLines(*graphicsContext, LineManager::get().allLinesInGroup(n, lineIds_, additionalExcludedLineId), black, groupColor(n));
			}
		}
	}

	switch (interactionMode_)
	{
		case IM_LINE:
		{
			switch (lineState_)
			{
				case LS_IDLE:
				{
					if (lineIds_.size() == 1)
					{
						if (lineEvaluationMap_.empty())
						{
							const FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());
							ocean_assert(line.isValid());

							drawLine(*graphicsContext, line, DM_HIGHLIGHT);

							if (linePointIndex_ == 0u)
							{
								drawAnchor(*graphicsContext, line.point0(), DM_HIGHLIGHT);
							}
							else if (linePointIndex_ == 1u)
							{
								drawAnchor(*graphicsContext, line.point1(), DM_HIGHLIGHT);
							}
						}
						else
						{
							LineManager::LineId lineOfInterestId = *lineIds_.begin();

							if (LineManager::get().isLineInGroup(lineOfInterestId, 0u))
							{
								// the line is a ground truth line, thus we have the correct id already
							}
							else
							{
								// the line is an evaluation line, so that we have to get the corresponding ground truth line (in case we have a match)

								const LineEvaluator::IdToIdSetMap::const_iterator i = lineEvaluationReverseMap_.find(lineOfInterestId);

								if (i != lineEvaluationReverseMap_.end())
								{
									ocean_assert(!i->second.empty());

									lineOfInterestId = *i->second.begin();
								}
								else
								{
									lineOfInterestId = LineManager::invalidId;

								}
							}

							const LineEvaluator::LineMatchMap::const_iterator i = lineEvaluationMap_.find(lineOfInterestId);

							if (i != lineEvaluationMap_.cend())
							{
								switch (i->second->matchType())
								{
									case LineEvaluator::LineMatch::MT_PERFECT:
									{
										const LineEvaluator::PerfectLineMatch& perfectMatch = dynamic_cast<const LineEvaluator::PerfectLineMatch&>(*i->second);

										const std::vector<std::string> texts =
										{
											"Perfect Match:",
											"",
											"Angle: " + String::toAString(NumericD::rad2deg(perfectMatch.angle()), 1u) + "deg",
											"Max. distance: " + String::toAString(perfectMatch.maximalDistance(), 2u) + "px"
										};
										drawText(dc, texts);

										ocean_assert(LineManager::get().isLineInGroup(lineOfInterestId, 0u)); // the line is a ground truth line
										drawLine(*graphicsContext,  LineManager::get().line(lineOfInterestId), DM_HIGHLIGHT);

										const LineManager::LineId targetId = perfectMatch.targetId();

										ocean_assert(LineManager::get().isLineInGroup(targetId, 1u)); // the line is not a ground truth line
										drawLine(*graphicsContext,  LineManager::get().line(targetId), DM_HIGHLIGHT_SECOND);

										break;
									}

									case CV::Detector::LineEvaluator::LineMatch::MT_PARTIAL:
									{
										const LineEvaluator::PartialLineMatch& partialMatch = dynamic_cast<const LineEvaluator::PartialLineMatch&>(*i->second);

										const std::vector<std::string> texts =
										{
											"Partial Match:",
											"",
											"Coverage: " + String::toAString(partialMatch.coverage() * 100.0, 1u) + "%",
											"Median angle: " + String::toAString(NumericD::rad2deg(partialMatch.medianAngle()), 1u) + "deg",
											"Median distance: " + String::toAString(partialMatch.medianDistance(), 1u) + "px"
										};
										drawText(dc, texts);

										ocean_assert(LineManager::get().isLineInGroup(lineOfInterestId, 0u)); // the line is a ground truth line
										drawLine(*graphicsContext,  LineManager::get().line(lineOfInterestId), DM_HIGHLIGHT);

										const LineEvaluator::IdSet& targetIds = partialMatch.targetIds();

										for (const LineEvaluator::Id& targetId : targetIds)
										{
											ocean_assert(LineManager::get().isLineInGroup(targetId, 1u)); // the line is not a ground truth line
											drawLine(*graphicsContext,  LineManager::get().line(targetId), DM_HIGHLIGHT_SECOND);
										}

										break;
									}

									case CV::Detector::LineEvaluator::LineMatch::MT_COMPLEX:
									{
										const LineEvaluator::ComplexLineMatch& complexMatch = dynamic_cast<const LineEvaluator::ComplexLineMatch&>(*i->second);

										const std::vector<std::string> texts =
										{
											"Complex Match:",
											"",
											"Coverage: " + String::toAString(complexMatch.coverage() * 100.0, 1u) + "%",
											"Median angle: " + String::toAString(NumericD::rad2deg(complexMatch.medianAngle()), 1u) + "deg",
											"Median distance: " + String::toAString(complexMatch.medianDistance(), 1u) + "px",
											"Connected ground truth lines: " + String::toAString(complexMatch.connectedSourceIds().size()),
											"Connected evaluation lines: " + String::toAString(complexMatch.connectedTargetIds().size()),
										};
										drawText(dc, texts);

										ocean_assert(LineManager::get().isLineInGroup(lineOfInterestId, 0u)); // the line is a ground truth line
										drawLine(*graphicsContext,  LineManager::get().line(lineOfInterestId), DM_HIGHLIGHT);

										const LineEvaluator::IdSet& targetIds = complexMatch.targetIds();

										for (const LineEvaluator::Id& targetId : targetIds)
										{
											ocean_assert(LineManager::get().isLineInGroup(targetId, 1u)); // the line is not a ground truth line
											drawLine(*graphicsContext,  LineManager::get().line(targetId), DM_HIGHLIGHT_SECOND);
										}

										break;
									}

									default:
										ocean_assert(false && "Missing implementation!");
										break;
								}
							}
						}
					}

					break;
				}

				case LS_DEFINING_END_POINT:
				{
					const VectorD2 lineEndPoint(interactionCursorCurrent_ + interactionCursorOffset_);

					if (lineStartPoint_ != lineEndPoint)
					{
						drawLine(*graphicsContext, FiniteLineD2(lineStartPoint_, lineEndPoint), DM_HIGHLIGHT);
					}

					break;
				}

				case LS_ADJUSTING_POINT:
				{
					ocean_assert(lineIds_.size() == 1);
					ocean_assert(linePointIndex_ <= 1u);

					const VectorD2 newPostion = interactionCursorCurrent_ + interactionCursorOffset_;

					FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());
					ocean_assert(line.isValid());

					if (linePointIndex_ == 0u)
					{
						line = FiniteLineD2(newPostion, line.point1());
					}
					else
					{
						line = FiniteLineD2(line.point0(), newPostion);
					}

					drawLine(*graphicsContext, line, DM_HIGHLIGHT);

					break;
				}

				case LS_MOVE_LINE:
				{
					ocean_assert(lineIds_.size() == 1);
					ocean_assert(linePointIndex_ == LineManager::invalidId);

					FiniteLineD2 line = LineManager::get().line(*lineIds_.begin());
					ocean_assert(line.isValid());

					const VectorD2 newPoint0 = interactionCursorCurrent_ + interactionCursorOffset_;
					const VectorD2 newPoint1 = newPoint0 + line.point1() - line.point0();

					line = FiniteLineD2(newPoint0, newPoint1);

					drawLine(*graphicsContext, line, DM_HIGHLIGHT);

					break;
				}

				default:
					break;
			}

			break;
		}

		case IM_SELECT:
		{
			switch (selectState_)
			{
				case SS_DEFINING_SECOND_CORNER:
				{
					const BoxD2 selectionBox(selectionFirstCorner_, interactionCursorCurrent_);

					if (selectionBox.isValid() && !selectionBox.isPoint())
					{
						drawSelection(*graphicsContext, selectionBox);
					}

					break;
				}

				default:
				{
					if (closestLineId_ != LineManager::invalidId)
					{
						drawLine(*graphicsContext, LineManager::get().line(closestLineId_), DM_HIGHLIGHT);
					}

					break;
				}
			}

			const FiniteLinesD2 selectedLines = LineManager::get().lines(lineIds_);
			drawLines(*graphicsContext, selectedLines, DM_HIGHLIGHT);

			break;
		}

		default:
			break;
	}
}

void IAImageWindow::onKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
		case WXK_ALT:
		{
			altKeyDown_ = true;

			if (interactionMode_ == IM_LINE && lineState_ == LS_IDLE)
			{
				// the user does not want to adjust an end point of a line, so that we invalidate a possible end point

				lineIds_.clear();
				linePointIndex_ = LineManager::invalidId;

				Refresh();
			}

			break;
		}

		case WXK_CONTROL:
		{
			if (interactionMode_ == IM_SELECT && selectState_ == SS_IDLE)
			{
				selectState_ = SS_SELECTING_INDIVIDUAL_LINES;

				Refresh();
			}

			break;
		}

		case WXK_DELETE:
		case WXK_BACK:
		{
			if (interactionMode_ == IM_SELECT && selectState_ == SS_IDLE)
			{
				LineManager::get().removeLines(lineIds_);
				lineIds_.clear();

				Refresh();
			}

			break;
		}

		default:
			break;
	}
}

void IAImageWindow::onKeyUp(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
		case WXK_ALT:
		{
			altKeyDown_ = false;

			Refresh();
			break;
		}

		case WXK_CONTROL:
		{
			if (interactionMode_ == IM_SELECT && selectState_ == SS_SELECTING_INDIVIDUAL_LINES)
			{
				selectState_ = SS_IDLE;

				Refresh();
			}

			break;
		}

		default:
			break;
	}
}

void IAImageWindow::drawLines(wxGraphicsContext& graphicsContext, const FiniteLinesD2& lines, const DrawingMode drawingMode)
{
	drawLines(graphicsContext, lines, backgroundColor(drawingMode), foregroundColor(drawingMode));
}

void IAImageWindow::drawLines(wxGraphicsContext& graphicsContext, const FiniteLinesD2& lines, const wxColour& backgroundColor, const wxColour& foregroundColor)
{
	FiniteLinesD2 virtualLines;
	virtualLines.reserve(lines.size());

	for (const FiniteLineD2& line : lines)
	{
		FiniteLineD2 virtualLine;
		if (image2virtualWindow(line, virtualLine))
		{
			virtualLines.push_back(virtualLine);
		}
	}

	// background element

	graphicsContext.SetPen(wxPen(backgroundColor, 3, wxPENSTYLE_SOLID));

	for (FiniteLinesD2::const_iterator i = virtualLines.begin(); i != virtualLines.end(); ++i)
	{
		graphicsContext.StrokeLine(i->point0().x(), i->point0().y(), i->point1().x(), i->point1().y());
	}

	// foreground element

	graphicsContext.SetPen(wxPen(foregroundColor, 1, wxPENSTYLE_SOLID));

	for (FiniteLinesD2::const_iterator i = virtualLines.begin(); i != virtualLines.end(); ++i)
	{
		graphicsContext.StrokeLine(i->point0().x(), i->point0().y(), i->point1().x(), i->point1().y());
	}
}

void IAImageWindow::drawLine(wxGraphicsContext& graphicsContext, const FiniteLineD2& line, const DrawingMode drawingMode)
{
	const wxColour& backgroundColorValue = backgroundColor(drawingMode);
	const wxColour& foregroundColorValue = foregroundColor(drawingMode);

	FiniteLineD2 virtualLine;
	if (image2virtualWindow(line, virtualLine))
	{
		// background element

		graphicsContext.SetPen(wxPen(backgroundColorValue, 3, wxPENSTYLE_SOLID));
		graphicsContext.StrokeLine(virtualLine.point0().x(), virtualLine.point0().y(), virtualLine.point1().x(), virtualLine.point1().y());

		// foreground element

		graphicsContext.SetPen(wxPen(foregroundColorValue, 1, wxPENSTYLE_SOLID));
		graphicsContext.StrokeLine(virtualLine.point0().x(), virtualLine.point0().y(), virtualLine.point1().x(), virtualLine.point1().y());
	}
}

void IAImageWindow::drawAnchor(wxGraphicsContext& graphicsContext, const VectorD2& position, const DrawingMode drawingMode)
{
	const wxColour& backgroundColorValue = backgroundColor(drawingMode);
	const wxColour& foregroundColorValue = foregroundColor(drawingMode);

	Scalar xWindow, yWindow;
	if (!bitmap2virtualWindow(Scalar(position.x()), Scalar(position.y()), xWindow, yWindow, true))
	{
		return;
	}

	const wxPoint2DDouble positions[5] =
	{
		wxPoint2DDouble(double(xWindow) - 10.0, double(yWindow) - 10.0),
		wxPoint2DDouble(double(xWindow) - 10.0, double(yWindow) + 10.0),
		wxPoint2DDouble(double(xWindow) + 10.0, double(yWindow) + 10.0),
		wxPoint2DDouble(double(xWindow) + 10.0, double(yWindow) - 10.0),
		wxPoint2DDouble(double(xWindow) - 10.0, double(yWindow) - 10.0)
	};

	// background element

	graphicsContext.SetPen(wxPen(backgroundColorValue, 3, wxPENSTYLE_SOLID));
	graphicsContext.DrawLines(5, positions);

	// foreground element

	graphicsContext.SetPen(wxPen(foregroundColorValue, 1, wxPENSTYLE_SOLID));
	graphicsContext.DrawLines(5, positions);
}

void IAImageWindow::drawSelection(wxGraphicsContext& graphicsContext, const BoxD2& boundingBox)
{
	ocean_assert(boundingBox.isValid() && !boundingBox.isPoint());

	const wxColour black(0x00, 0x00, 0x00);
	const wxColour white(0xFF, 0xFF, 0xFF);

	wxPoint2DDouble corners[5];

	for (unsigned int n = 0u; n < 5u; ++n)
	{
		const VectorD2 corner = boundingBox.corner(n % 4u);

		Scalar xWindow, yWindow;
		if (!bitmap2virtualWindow(Scalar(corner.x()), Scalar(corner.y()), xWindow, yWindow, true))
		{
			return;
		}

		corners[n] = wxPoint2DDouble(double(xWindow), double(yWindow));
	}

	// background element

	graphicsContext.SetPen(wxPen(black, 3, wxPENSTYLE_SHORT_DASH));
	graphicsContext.DrawLines(5, corners);

	// foreground element

	graphicsContext.SetPen(wxPen(white, 1, wxPENSTYLE_SHORT_DASH));
	graphicsContext.DrawLines(5, corners);
}

void IAImageWindow::drawText(wxPaintDC& dc, const std::vector<std::string>& texts)
{
	const wxFont font(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	dc.SetFont(font);

	dc.SetBackgroundMode(wxSOLID);
	dc.SetTextBackground(wxColor(0xFF, 0xFF, 0xFF));

	for (size_t n = 0; n < texts.size(); ++n)
	{
		dc.DrawText(texts[n], 5, 5 + int(n * 14));
	}
}

bool IAImageWindow::image2virtualWindow(const FiniteLineD2& imageLine, FiniteLineD2& virtualLine)
{
	ocean_assert(imageLine.isValid());

	const double bitmapWidth = Scalar(bitmap_.GetWidth());
	const double bitmapHeight = Scalar(bitmap_.GetHeight());

	if (bitmapWidth <= NumericD::eps() || bitmapHeight <= NumericD::eps())
	{
		return false;
	}

	const double xFactor = double(displayBitmapWidth_) / bitmapWidth;
	const double yFactor = double(displayBitmapHeight_) / bitmapHeight;

	const VectorD2 point0(double(displayBitmapLeft_) + imageLine.point0().x() * xFactor, double(displayBitmapTop_) + imageLine.point0().y() * yFactor);
	const VectorD2 point1(double(displayBitmapLeft_) + imageLine.point1().x() * xFactor, double(displayBitmapTop_) + imageLine.point1().y() * yFactor);

	virtualLine = FiniteLineD2(point0, point1);

	return true;
}

FiniteLineD2 IAImageWindow::clampLine(const FiniteLineD2& line, const unsigned int width, const unsigned int height)
{
	ocean_assert(line.isValid());
	ocean_assert(width != 0u && height != 0u);

	// first we check whether the given line is completely outside the region

	if ((line.point0().x() < 0.0 && line.point1().x() < 0.0)
		|| (line.point0().x() >= double(width) && line.point1().x() >= double(width))
		|| (line.point0().y() < 0.0 && line.point1().y() < 0.0)
		|| (line.point0().y() >= double(height) && line.point1().y() >= double(height)))
	{
		return FiniteLineD2();
	}

	FiniteLineD2 resultLine(line);

	// now lets check whether parts of the line are outside

	const double eps = 0.001;

	const VectorD2 corners[4] =
	{
		VectorD2(0, 0),
		VectorD2(0, double(height) - eps),
		VectorD2(double(width) - eps, double(height) - eps),
		VectorD2(double(width) - eps, 0),
	};

	VectorD2 point;

	// left edge
	if (FiniteLineD2(corners[0], corners[1]).intersection(resultLine, point))
	{
		if (resultLine.point0().x() <= corners[0].x() + Numeric::eps())
		{
			resultLine = FiniteLineD2(point, resultLine.point1());
		}
		else
		{
			resultLine = FiniteLineD2(resultLine.point0(), point);
		}
	}

	// bottom edge
	if (FiniteLineD2(corners[1], corners[2]).intersection(resultLine, point))
	{
		if (resultLine.point0().y() >= corners[1].y() - Numeric::eps())
		{
			resultLine = FiniteLineD2(point, resultLine.point1());
		}
		else
		{
			resultLine = FiniteLineD2(resultLine.point0(), point);
		}
	}

	// right edge
	if (FiniteLineD2(corners[2], corners[3]).intersection(resultLine, point))
	{
		if (resultLine.point0().x() >= corners[2].x() - Numeric::eps())
		{
			resultLine = FiniteLineD2(point, resultLine.point1());
		}
		else
		{
			resultLine = FiniteLineD2(resultLine.point0(), point);
		}
	}

	// top edge
	if (FiniteLineD2(corners[3], corners[0]).intersection(resultLine, point))
	{
		if (resultLine.point0().y() <= corners[0].y() + Numeric::eps())
		{
			resultLine = FiniteLineD2(point, resultLine.point1());
		}
		else
		{
			resultLine = FiniteLineD2(resultLine.point0(), point);
		}
	}

	return resultLine;
}

const wxColour& IAImageWindow::foregroundColor(const DrawingMode drawingMode)
{
	ocean_assert((unsigned int)drawingMode <= 2u);

	const static wxColour colors[] =
	{
		// DM_NORMAL
		wxColour(0xFF, 0xFF, 0xFF), // white
		// DM_HIGHLIGHT
		wxColour(0x40, 0xFF, 0x40), // light green
		// DM_HIGHLIGHT_SECOND
		wxColour(0xFF, 0x40, 0x40) // light red
	};

	return colors[(unsigned int)drawingMode];
}

const wxColour& IAImageWindow::backgroundColor(const DrawingMode drawingMode)
{
	ocean_assert((unsigned int)drawingMode <= 2u);

	const static wxColour colors[] =
	{
		// DM_NORMAL
		wxColour(0x00, 0x00, 0x00), // black
		// DM_HIGHLIGHT
		wxColour(0x40, 0x40, 0xFF), // light blue
		// DM_HIGHLIGHT_SECOND
		wxColour(0xFF, 0xFF, 0x40) // light yellow
	};

	return colors[(unsigned int)drawingMode];
}

const wxColour& IAImageWindow::groupColor(const unsigned int groupIndex)
{
	const static wxColour groups[] =
	{
		wxColour(0xFF, 0x00, 0x00), // red
		wxColour(0x00, 0xFF, 0x00), // green
		wxColour(0x00, 0x00, 0xFF), // blue
		wxColour(0xFF, 0xFF, 0x00), // yellow
		wxColour(0xFF, 0x00, 0xFF), // magenta
		wxColour(0x00, 0xFF, 0xFF), // cyan
	};

	return groups[groupIndex % 6u];
}
