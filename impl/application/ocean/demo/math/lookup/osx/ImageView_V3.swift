/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import Cocoa

/**
 * This class implements an custom image view object.
 * It holds two frame datas. One is allocated internally and the other is allocated externally from a wrapper function.
 * For the determined lookup frame with an odd view id, the external data is used; the internal otherwise.
 */
class ImageView: NSImageView {
  /// The current view id.
  var viewId: UInt32

  /// The internal allocated frame data.
  var viewInternalFrameData: UnsafeMutablePointer<UInt8>

  // The external allocated frame data.
  var viewExternalFrameData: UnsafeMutablePointer<UInt8>

  /**
   * Creates a new image view object.
   * @param frame A frame rectangle that describe the size of the view
   */
  override init(frame: NSRect) {
    viewId = 0

    viewInternalFrameData = UnsafeMutablePointer<UInt8>.allocate(capacity: 800 * 800 * 3)
    viewExternalFrameData = UnsafeMutablePointer<UInt8>(wrapperAllocFrameData())

    super.init(frame: frame)
  }

  /**
   * Creates a new image view object.
   * @param coder An unarchiver object
   */
  required init?(coder: NSCoder) {
    viewId = 0

    viewInternalFrameData = UnsafeMutablePointer<UInt8>.allocate(capacity: 800 * 800 * 3)
    viewExternalFrameData = UnsafeMutablePointer<UInt8>(wrapperAllocFrameData())

    super.init(coder: coder)
  }

  /**
   * Destroy a image view object.
   */
  deinit {
    viewInternalFrameData.deallocate()

    wrapperReleaseFrameData(viewExternalFrameData)
  }

  /**
   * Toggle the current view.
   * A new Frame based on the view id is determined and set to the image view.
   */
  func toggleView() {
    var rgbData: CFData!

    if viewId % 2 == 0 {
      wrapperDetermineLookupFrame(viewInternalFrameData, viewId)
      rgbData = CFDataCreateWithBytesNoCopy(nil, viewInternalFrameData, 800 * 800 * 3, kCFAllocatorNull)
    } else {
      wrapperDetermineLookupFrame(viewExternalFrameData, viewId)
      rgbData = CFDataCreateWithBytesNoCopy(nil, viewExternalFrameData, 800 * 800 * 3, kCFAllocatorNull)
    }

    viewId = (viewId + 1) % 5

    let provider = CGDataProvider(data: rgbData)

    let rgbImageRef = CGImage(width: 800, height: 800, bitsPerComponent: 8, bitsPerPixel: 24, bytesPerRow: 800 * 3, space: CGColorSpaceCreateDeviceRGB(), bitmapInfo: CGBitmapInfo(), provider: provider!, decode: nil, shouldInterpolate: false, intent: .defaultIntent)

    let image = NSImage(cgImage: rgbImageRef!, size: NSSize(width: 800, height: 800))

    self.image = image
  }

  /**
   * The mouse down event function.
   * @param theEvent The event object
   */
  override func mouseDown(with theEvent: NSEvent) {
    toggleView()
  }
}
