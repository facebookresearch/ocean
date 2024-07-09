---
title: Semantics
sidebar_position: 5
---

The default semantic of the `Frame` class ensures that the memory ownership is preserved when creating a new instance of a `Frame` object. That means that the new `Frame` object will be the owner of the image content (by creating a copy) if the source `Frame` is also the owner of the image content. The new `Frame` object will not be the owner of the image content if the source `Frame` is also not the owner of the image content. For example:

```cpp
Frame sourceFrameOwningTheMemory = ...;
Frame newFrameWithCopiedMemory = sourceFrameOwningTheMemory;

Frame sourceFrameNotOwningTheMemory = ...;
Frame newFrameAlsoNotOwningTheMemory = sourceFrameNotOwningTheMemory;

// memory ownership of source and new frames will be identical
assert(sourceFrameOwningTheMemory.isOwner() == newFrameWithCopiedMemory.isOwner());
assert(sourceFrameNotOwningTheMemory.isOwner() == newFrameAlsoNotOwningTheMemory.isOwner());
```

However, when using the copy constructor of `Frame` with explicit copy mode, the copy behavior can be customized.

```cpp
Frame sourceFrame = ...;

// Using memory, not making a copy, the padding layout is preserved.
Frame newFrame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

// Make a copy of the memory,
// but the new memory will be continuous and the source padding data is not touched
Frame newFrame(sourceFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

// Make a copy of the memory,
// the new memory will have the same padding layout as the source image,
// but the actual padding data is not copied
Frame newFrame(sourceFrame, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);

// Make a copy of the memory,
// the new memory will have the same padding layout as the source image,
// the padding data is copied as well
Frame newFrame(sourceFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
```
