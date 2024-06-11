---
title: VRNativeApplication
sidebar_position: 3
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

Building on the [NativeApplication](nativeapplication.md), the VRNativeApplication introduces a basic VR environment. This demo integrates an OpenXR session and sets up Ocean's OpenGL ES rendering engine, ready to render simple primitive objects.

### Features
- **Integrated OpenXR Session**: Unlike the NativeApplication, this demo includes an OpenXR session setup.
- **Integrated Rendering Engine**: Ready to render simple primitive objects using Ocean's OpenGL ES rendering engine.


### Usage
- **Log Verification**: Similar to the NativeApplication, this app does not provide visual feedback upon starting. Use the following commaned to verify its operation:
```
adb logcat -s Ocean
```

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
