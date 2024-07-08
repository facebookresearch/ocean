/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/segmentation/clustering/win/ClusteringMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/segmentation/Clustering.h"

#include "ocean/math/ClusteringKMeans.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

ClusteringMainWindow::ClusteringMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	randomWindow_(instance, L"Random Clustering"),
	kMeansWindowIndices_(instance, L"K-means Clustering (indices)"),
	kMeansWindowPointers_(instance, L"K-means Clustering (pointers)"),
	mediaFile_(media)
{
	// nothing to do here
}

ClusteringMainWindow::~ClusteringMainWindow()
{
	// nothing to do here
}

void ClusteringMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::IMAGE_SEQUENCE);

		if (frameMedium_)
		{
			const Media::ImageSequenceRef imageSequence(frameMedium_);
			ocean_assert(imageSequence);

			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}

		if (frameMedium_.isNull())
		{
			frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
		}
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1", Media::Medium::LIVE_VIDEO);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0", Media::Medium::LIVE_VIDEO);
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);

	if (finiteMedium)
	{
		finiteMedium->setSpeed(1);
	}

	if (frameMedium_)
	{
		frameMedium_->start();
	}

	randomWindow_.setParent(handle());
	randomWindow_.initialize();
	randomWindow_.show();

	kMeansWindowIndices_.setParent(handle());
	kMeansWindowIndices_.initialize();
	kMeansWindowIndices_.show();

	kMeansWindowPointers_.setParent(handle());
	kMeansWindowPointers_.initialize();
	kMeansWindowPointers_.show();
}

void ClusteringMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && (*frame && frame->timestamp() != frameTimestamp_))
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void ClusteringMainWindow::onMouseUp(const MouseButton /*button*/, const int /*x*/, const int /*y*/)
{
	const FrameRef frame(frameMedium_->frame());

	if (frame && *frame && frame->numberPlanes() == 1u)
	{
		switch (frame->channels())
		{
			case 1u:
				determineRandomClustering<1u>(*frame);
				determineKMeansClusteringIndices<1u>(*frame);
				determineKMeansClusteringPointers<1u>(*frame);
				break;

			case 3u:
				determineRandomClustering<3u>(*frame);
				determineKMeansClusteringIndices<3u>(*frame);
				determineKMeansClusteringPointers<3u>(*frame);
				break;

			default:
				ocean_assert(false && "Not supported frame type!");
				break;
		}
	}

	const Media::ImageSequenceRef imageSequence(frameMedium_);

	if (imageSequence)
	{
		imageSequence->forceNextFrame();
	}
}

void ClusteringMainWindow::onFrame(const Frame& frame)
{
	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker_))
	{
		return;
	}

	setFrame(topLeft);

	repaint();
}

template <unsigned int tChannels>
void ClusteringMainWindow::determineRandomClustering(const Frame& frame)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	typedef CV::Segmentation::Clustering<tChannels> Clustering;

	HighPerformanceTimer timer;

	typename Clustering::Datas datas;
	datas.reserve(frame.pixels());

	unsigned int id = 0u;

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		const uint8_t* const frameRow = frame.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			datas.emplace_back(frameRow + x * tChannels, id++);
		}
	}

	const Clustering::Segmentation segmentation(Clustering::findOptimalRandomClustering(datas, 30u, randomGenerator_, 200u, &worker_));
	const double time = timer.mseconds();

	typename Clustering::Clusters clusters(segmentation.clusters());
	std::sort(clusters.begin(), clusters.end());

	RandomGenerator randomGenerator(0u);

	Frame clusteringFrame(frame.frameType());

	for (const Clustering::Cluster& cluster : clusters)
	{
		uint8_t colors[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			colors[n] = uint8_t(randomGenerator.rand() % 0xFF);
		}

		for (const Clustering::Data& data : cluster.datas())
		{
			const unsigned int pixelIndex = data.id();

			ocean_assert(clusteringFrame.isContinuous());
			uint8_t* const clusteringPixel = clusteringFrame.data<uint8_t>() + pixelIndex * tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				clusteringPixel[n] = colors[n];
			}
		}
	}

	randomWindow_.setFrame(clusteringFrame);
	randomWindow_.setText(std::wstring(L"Random Clustering performance: ") + String::toWString(time) + std::wstring(L"ms"));

	randomWindow_.repaint();
}

template <unsigned int tChannels>
void ClusteringMainWindow::determineKMeansClusteringIndices(const Frame& frame)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	typedef ClusteringKMeans<uint8_t, tChannels, uint32_t, uint32_t, true> Clustering;

	HighPerformanceTimer timer;

	std::vector<typename Clustering::Observation> observations;
	observations.reserve(frame.pixels());

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			observations.emplace_back(frame.constpixel<uint8_t>(x, y));
		}
	}

	Clustering clustering(Clustering::Data(observations.data(), observations.size(), false));
	clustering.determineClustersByNumber(10, Clustering::IS_LARGEST_DISTANCE, 5, &worker_);
	clustering.sortClusters();

	const double time = timer.mseconds();

	const Clustering::Clusters& clusters = clustering.clusters();

	RandomGenerator randomGenerator(5000u);

	Frame clusteringFrame(frame.frameType());

	for (const Clustering::Cluster& cluster : clusters)
	{
		uint8_t colors[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			colors[n] = uint8_t(randomGenerator.rand() % 0xFF);
		}

		for (const size_t pixelIndex : cluster.dataIndices())
		{
			ocean_assert(clusteringFrame.isContinuous());
			uint8_t* const clusteringPixel = clusteringFrame.data<uint8_t>() + pixelIndex * tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				clusteringPixel[n] = colors[n];
			}
		}
	}

	kMeansWindowIndices_.setFrame(clusteringFrame);
	kMeansWindowIndices_.setText(std::wstring(L"K-Means Clustering (indices) performance: ") + String::toWString(time) + std::wstring(L"ms"));
	kMeansWindowIndices_.repaint();
}

template <unsigned int tChannels>
void ClusteringMainWindow::determineKMeansClusteringPointers(const Frame& frame)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	typedef ClusteringKMeans<uint8_t, tChannels, uint32_t, uint32_t, false> Clustering;

	HighPerformanceTimer timer;

	std::vector<const Clustering::Observation*> observationPointers;
	observationPointers.reserve(frame.pixels());

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			observationPointers.emplace_back((const Clustering::Observation*)(frame.constpixel<uint8_t>(x, y)));
		}
	}

	Clustering clustering(Clustering::Data(observationPointers.data(), observationPointers.size(), false));
	clustering.determineClustersByNumber(10, Clustering::IS_LARGEST_DISTANCE, 5, &worker_);
	clustering.sortClusters();

	const double time = timer.mseconds();

	RandomGenerator randomGenerator(5000u);

	Frame clusteringFrame(frame.frameType());

	for (const Clustering::Cluster& cluster : clustering.clusters())
	{
		uint8_t colors[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			colors[n] = uint8_t(randomGenerator.rand() % 0xFF);
		}

		for (const size_t pixelIndex : cluster.dataIndices())
		{
			ocean_assert(clusteringFrame.isContinuous());
			uint8_t* const clusteringPixel = clusteringFrame.data<uint8_t>() + pixelIndex * tChannels;

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				clusteringPixel[n] = colors[n];
			}
		}
	}

	kMeansWindowPointers_.setFrame(clusteringFrame);
	kMeansWindowPointers_.setText(std::wstring(L"K-Means Clustering (pointers) performance: ") + String::toWString(time) + std::wstring(L"ms"));
	kMeansWindowPointers_.repaint();
}
