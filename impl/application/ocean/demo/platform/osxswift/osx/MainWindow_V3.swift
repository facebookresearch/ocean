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
  NSApplicationDelegate {
  /// The window object.
  @IBOutlet weak var mainWindow: NSWindow!

  /// The view object.
  @IBOutlet weak var mainWindowView: NSView!

  /// The label object of the build string.
  @IBOutlet weak var mainWindowLabelBuildString: NSTextField!

  /// The label object of the build date.
  @IBOutlet weak var mainWindowLabelBuildDate: NSTextField!

  /// The label object of the build time.
  @IBOutlet weak var mainWindowLabelBuildTime: NSTextField!

  /// The label object of the timestamp.
  @IBOutlet weak var mainWindowLabelTimestamp: NSTextField!

  /// The label object of the first struct.
  @IBOutlet weak var mainWindowLabelStruct0: NSTextField!

  /// The label object of the second struct.
  @IBOutlet weak var mainWindowLabelStruct1: NSTextField!

  /// The label object of the third struct.
  @IBOutlet weak var mainWindowLabelStruct2: NSTextField!

  /// The button object
  @IBOutlet weak var mainWindowButton: NSButton!

  /**
   * The initialized event function.
   */
  func applicationDidFinishLaunching(_ aNotification: Notification) {
    mainWindowLabelBuildString.stringValue = String(cString: oceanBaseBuildStringStatic())

    let dateString = oceanBaseDateTimeString("date")
    let timeString = oceanBaseDateTimeString("time")

    mainWindowLabelBuildDate.stringValue = String(cString: dateString!)
    mainWindowLabelBuildTime.stringValue = String(cString: timeString!)

    oceanReleaseString(dateString)
    oceanReleaseString(timeString)

    let timestamp = oceanBaseTimestamp()
    mainWindowLabelTimestamp.stringValue = timestamp.description + " seconds"

    let structObject = oceanBaseStructObject()

    mainWindowLabelStruct0.stringValue = structObject.valueInteger.description + ", " + structObject.valueFloat.description

    let structPointer0 = oceanBaseStructStatic()
    var structPointerMemory0 = UnsafeMutablePointer<Struct>(structPointer0!).pointee

    mainWindowLabelStruct1.stringValue = structPointerMemory0.valueInteger.description + ", " + structPointerMemory0.valueFloat.description

    structPointerMemory0.valueInteger += 1
    structPointerMemory0.valueFloat = 0.64

    let structPointer1 = oceanBaseStructStatic()
    let structPointerMemory1 = UnsafeMutablePointer<Struct>(structPointer1!).pointee

    mainWindowLabelStruct2.stringValue = structPointerMemory1.valueInteger.description + ", " + structPointerMemory1.valueFloat.description

    let textField = NSTextField(frame: NSRect(x: 20, y: 20, width: 200, height: 40))
    mainWindowView.addSubview(textField)
  }

  /**
   * The release event function.
   */
  func applicationWillTerminate(_ aNotification: Notification) {
    // insert code here to tear down your application
  }

  /**
   * The event function for the pressed button.
   * @param sender The sender object
   */
  @IBAction func onButtonPressed(_ sender: NSButton) {
    mainWindow.title = "The button has been pressed!"
    sender.title = "Pressed!"
  }

  /**
   * The event function for the selected new menu.
   * @param sender The sender object
   */
  @IBAction func onMenuNew(_ sender: NSMenuItem) {
    mainWindow.title = "The 'New' menu has been pressed!"
    sender.title = "New pressed!"
  }
}
