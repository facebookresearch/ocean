/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/wxwidgets/WxDialog.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

#ifdef _WINDOWS

WxDialog::EventLoop::EventLoop(wxWindow *winModal) :
	wxModalEventLoop(winModal)
{
	// nothing to do here
}

void WxDialog::EventLoop::Exit(int rc)
{
	exitTimestamp_.toNow();

	wxModalEventLoop::Exit(rc);
}

bool WxDialog::EventLoop::Pending() const
{
	if (m_shouldExit)
	{
		if (exitTimestamp_.isValid() && exitTimestamp_ + 0.1 < Timestamp(true))
		{
			return false;
		}

		MSG msg;
		return ::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0;
	}
	else
	{
		MSG msg;
		return ::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE) != 0;
	}
}

#endif

WxDialog::WxDialog(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
	wxDialog(parent, id, title, pos, size, style, name),
	endModalCalled_(false)
{
	// nothing to do here
}

WxDialog::~WxDialog()
{
	// nothing to do here
}

#ifdef _WINDOWS

bool WxDialog::IsModal() const
{
	return !eventLoop_.isNull();
}

bool WxDialog::Show(bool show)
{
	if (show == IsShown())
		return false;

	TemporaryScopedLock temporaralScopedLock(eventLoopLock_);

	if (!show && eventLoop_)
	{
		// we need to do this before calling wxDialogBase version because if we
		// had disabled other app windows, they must be reenabled right now as
		// if they stay disabled Windows will activate another window (one
		// which is enabled, anyhow) when we're hidden in the base class Show()
		// and we will lose activation
		eventLoop_->Exit();
	}

	temporaralScopedLock.release();

	if (show)
	{
		// this usually will result in TransferDataToWindow() being called
		// which will change the controls values so do it before showing as
		// otherwise we could have some flicker
		InitDialog();
	}

	wxDialogBase::Show(show);

	if (show)
	{
		// dialogs don't get WM_SIZE message after creation unlike most (all?)
		// other windows and so could start their life non laid out correctly
		// if we didn't call Layout() from here
		//
		// NB: normally we should call it just the first time but doing it
		//	 every time is simpler than keeping a flag
		Layout();
	}

	return true;
}

int WxDialog::ShowModal()
{
	 wxASSERT_MSG( !IsModal(), _T("wxDialog::ShowModal() reentered?") );

	endModalCalled_ = false;

	Show();

	// EndModal may have been called from InitDialog handler (called from
	// inside Show()), which would cause an infinite loop if we didn't take it
	// into account
	if (!endModalCalled_)
	{
#if defined(wxABI_VERSION) && wxABI_VERSION < 30000
		// modal dialog needs a parent window, so try to find one
		wxWindow *parent = GetParent();
		if (!parent)
		{
			parent = FindSuitableParent();
		}

		// remember where the focus was
		wxWindow *oldFocus = parent;

		// We have to remember the HWND because we need to check
		// the HWND still exists (oldFocus can be garbage when the dialog
		// exits, if it has been destroyed)
		HWND hwndOldFocus = oldFocus ? (HWND)oldFocus->GetHWND() : nullptr;
#endif

		// enter and run the modal loop
		{
			TemporaryScopedLock temporalScopedLock(eventLoopLock_);
				eventLoop_ = EventLoopRef(new EventLoop(this));
			temporalScopedLock.release();

			eventLoop_->Run();

			const ScopedLock scopedLock(eventLoopLock_);
			eventLoop_.release();
		}

#if defined(wxABI_VERSION) && wxABI_VERSION < 30000
		// and restore focus
		// Note that this code MUST NOT access the dialog object's data
		// in case the object has been deleted (which will be the case
		// for a modal dialog that has been destroyed before calling EndModal).
		if (oldFocus && (oldFocus != this) && ::IsWindow(hwndOldFocus))
		{
			//ocean_assert(false);
			// This is likely to prove that the object still exists
			//if (wxFindWinFromHandle((OCEAN_WXWIDGETS_HANDLE) hwndOldFocus) == oldFocus)
			//	oldFocus->SetFocus();
		}
#endif
	}

	return GetReturnCode();
}

void WxDialog::EndModal(int retCode)
{
	wxASSERT_MSG( IsModal(), _T("EndModal() called for non modal dialog") );

	endModalCalled_ = true;
	SetReturnCode(retCode);

	Hide();
}

#endif

}

}

}
