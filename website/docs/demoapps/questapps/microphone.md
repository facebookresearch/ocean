---
title: Microphone
sidebar_position: 9
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

## Description

The Microphone app provides a practical example of how to access the Quest's microphone and handle Android permissions. It processes live microphone samples in real-time and converts them into an audio spectrum.

<img src={require('@site/static/img/docs/demoapps/questapps/microphone.png').default} alt="Image: Screenshot Finger Distance app" width="600" className="center-image"/>

The app utilizes the [*PlatformSDK*](https://developer.oculus.com/downloads/package/oculus-platform-sdk/) to access the Quest's microphone. Once the microphone is activated, the [`onMicrophoneSample()`](https://github.com/facebookresearch/ocean/blob/7372549e039fe1718f364aeac534f919bf45d280/impl/application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.cpp#L212) event function receives real-time audio samples. These samples are then transformed into individual frequencies using a [*Fast Fourier Transform*](https://en.wikipedia.org/wiki/Fast_Fourier_transform) and stored in a member variable of the application.

Before each frame is rendered, the [`updateMicrophoneSpectrum()`](https://github.com/facebookresearch/ocean/blob/7372549e039fe1718f364aeac534f919bf45d280/impl/application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.cpp#L119) function is executed. Whenever new audio frequencies are received, they are visualized using a line strip. Additionally, a per-vertex color is applied to create a visually engaging effect.

Before the microphone can be activated, the app must obtain permission from the user. Therefore, the record audio permission (*android.permission.RECORD_AUDIO*) is included in the app's manifest file and is [additionally](https://github.com/facebookresearch/ocean/blob/7372549e039fe1718f364aeac534f919bf45d280/impl/application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.cpp#L25) requested when the application starts. Once the user grants permission, the [`onAndroidPermissionGranted()`](https://github.com/facebookresearch/ocean/blob/7372549e039fe1718f364aeac534f919bf45d280/impl/application/ocean/demo/platform/meta/quest/openxr/microphone/quest/MicrophoneApplication.cpp#L40) function is invoked, allowing the microphone to be accessed.

## Building

<Tabs groupId="target-os" queryString>

  <TabItem value="quest" label="Quest">
    TODO
  </TabItem>

</Tabs>

## Features
 - **Microphone Access**: Demonstrates how to access the Quest's microphone for audio input.
 - **Android Permission Handling**: Shows how to request and handle Android permissions required for microphone access.
 - **Real-Time Audio Processing**: Processes live microphone samples and converts them into an audio spectrum display.


## Application Integration
 - **Audio-Based Interactions**: Enables developers to integrate audio-based interactions or features into their VR applications.
