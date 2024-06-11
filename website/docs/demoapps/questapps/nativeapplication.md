---
title: NativeApplication
sidebar_position: 2
---

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The NativeApplication demo is the most basic example of a native application that can be executed on Meta's Quest headsets. It's designed to be OpenXR-ready, providing a foundational template for developers to build upon.

### Features
- **OpenXR-Ready**: The application is prepared for OpenXR integration, although the session itself needs to be manually added by the developer.
- **Basic Template**: Serves as a starting point for more complex applications.

### Usage
- **Log Verification**: No visual feedback is provided when launching the app on Quest. Developers can verify functionality by accessing the Quest's log using the command:
```
adb logcat -s Ocean
```

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>
