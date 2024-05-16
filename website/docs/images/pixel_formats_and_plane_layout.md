---
title: Pixel formats and plane layout
sidebar_position: 2
---

import useBaseUrl from '@docusaurus/useBaseUrl';
import styles from './images.css';

This section will discuss some examples of important pixel formats and explain their differences as well as their similarities.

## Format: *RGB24*

An image with pixel format [`RGB24`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L312) is composed of one plane and the plane has three channels (for red, green, and blue color values). The image memory is based on elements with data type `uint8_t`, with three elements representing a pixel so that each pixel needs 24 bits in memory. The image width in pixels is identical to the plane width in pixels. The plane width in elements is three times the width in pixels. The plane may contain padding elements at the end of each row which then increases the plane’s stride accordingly.

<img src={useBaseUrl('img/docs/images/Frame_RGB24.png')} alt="The pixel format FORMAT_RGB24" width="700" className="center-image"/>

## Format: *Y8*

An image with format [`Y8`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L591) is very similar to `RGB24` but the plane has only one channel. Therefore, the width of the image in pixels is identical to the plane's width in elements.

<img src={useBaseUrl('img/docs/images/Frame_Y8.png')} alt="The pixel format FORMAT_Y8" width="700" className="center-image"/>

## Format: *Y_UV12*

A common pixel format with two planes is e.g., [`Y_UV12`](https://fburl.com/diffusion/tfyc4yzm). In Ocean, the  underscore (`_`) between `Y` and `UV` is used to denote that the image information is separated into two planes. The first plane contains the luminance channel of the image, while the second plane contains the two chrominance channels of the image. As a way to reduce bandwidth, the second plane does not define chrominance values for each pixel in the first plane. Instead it defines one for every second pixel only. Thus, the height of the first plane is two times the height of the second plane. In average, the image data is stored with 12 bits per pixels. It's important to note that due to the 2x2 downsampling of the second plane, this pixel format does not allow image dimensions with odd values.

<img src={useBaseUrl('img/docs/images/Frame_Y_UV12.png')} alt="The pixel format FORMAT_Y_UV12" width="700" className="center-image"/>

## Format: *Y_U_V24*

Images with the format `Y_U_V24` are composed of three planes. Each plane holds one image channel without any sub-sampling.

<img src={useBaseUrl('img/docs/images/Frame_Y_U_V24.png')} alt="The pixel format FORMAT_Y_U_V24" width="700" className="center-image"/>
