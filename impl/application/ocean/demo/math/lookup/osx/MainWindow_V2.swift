/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Cocoa

@NSApplicationMain
class AppDelegate:
  NSObject,
  NSApplicationDelegate,
  NSWindowDelegate {
  /// The window object.
  @IBOutlet weak var mainWindow: NSWindow!

  /// The view object.
  @IBOutlet weak var mainWindowView: NSView!

  /// The image object.
  @IBOutlet weak var mainWindowImage: ImageView!

  /**
   * The initialized event function.
   * @param aNotification The notification object
   */
  func applicationDidFinishLaunching(aNotification: NSNotification) {
    wrapperRandomInitialize()

    mainWindow.title = mainWindow.title + " " + String.fromCString(oceanBaseBuildStringStatic())!

    // set the image scaling of the ImageView object to scale the image proportional to the view size
    mainWindowImage.imageScaling = NSImageScaling.ScaleProportionallyUpOrDown

    mainWindowImage.toggleView()

    // set the window delegate to the application to handle the windowDidResize function
    mainWindow.delegate = self
  }

  /**
   * The release event function.
   * @param aNotification The notification object
   */
  func applicationWillTerminate(aNotification: NSNotification) {
    // insert code here to tear down your application
  }

  /**
   * The event function that returns whether the application should be terminated after closing the last window.
   * @return True to terminate the application
   */
  func applicationShouldTerminateAfterLastWindowClosed(theApplication: NSApplication) -> Bool {
    return true
  }

  /**
   * The resize event function.
   * @param aNotification The notification object
   */
  func windowDidResize(notification: NSNotification) {
    mainWindowImage.setFrameSize(NSSize(width: mainWindow!.contentView!.frame.size.width, height: mainWindow!.contentView!.frame.size.height))
    mainWindowImage.setFrameOrigin(NSPoint(x: 0, y: 0))
  }

  /**
   * The event function for the selected toggle menu.
   * @param sender The sender object
   */
  @IBAction func onManuToggle(sender: NSMenuItem) {
    mainWindowImage.toggleView()
  }
}
