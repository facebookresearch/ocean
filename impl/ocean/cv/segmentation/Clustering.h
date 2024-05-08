/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_CLUSTERING_H
#define META_OCEAN_CV_SEGMENTATION_CLUSTERING_H

#include "ocean/cv/segmentation/Segmentation.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements simple clustering functions for image information.
 * @tparam tChannels The number of data channels of the image information to be clustered, with range [1, infinity)
 * @ingroup cvsegmentation
 */
template <unsigned int tChannels>
class Clustering
{
	public:

		/**
		 * This class implements a single data to be clustered.
		 */
		class Data
		{
			public:

				/**
				 * Creates a default data object.
				 */
				Data() = default;

				/**
				 * Creates a new data object.
				 * @param value The values of the data object
				 * @param id The id of the data
				 */
				inline Data(const uint8_t* const value, const unsigned int id);

				/**
				 * Returns a specified element of this data object.
				 * @param channel The channel of the element to be returned
				 * @return Data value
				 */
				inline uint8_t value(unsigned int channel) const;

				/**
				 * Returns the id of this data object.
				 * @return The object's id
				 */
				inline unsigned int id() const;

				/**
				 * Returns the number of channels this data object stores.
				 * @return Number of channels
				 */
				inline unsigned int channels() const;

				/**
				 * Returns the ssd between two data values.
				 * @param data Second data object.
				 * @return Resulting ssd
				 */
				inline unsigned int ssd(const Data& data) const;

				/**
				 * Returns the ssd between this data value and an array of elements.
				 * @param values Second data object.
				 * @return Resulting ssd
				 */
				inline unsigned int ssd(const uint8_t* values) const;

				/**
				 * Returns whether all per-channel square differences are below a given threshold.
				 * @param data The second data object to be used
				 * @param sqrChannel The per-channel threshold
				 * @return True, if so
				 */
				inline bool ssd(const Data& data, const unsigned int sqrChannel) const;

				/**
				 * Returns whether all per-channel square differences are below a given threshold.
				 * @param values The second values to be used
				 * @param sqrChannel The per-channel threshold
				 * @return True, if so
				 */
				inline bool ssd(const uint8_t* values, const unsigned int sqrChannel) const;

				/**
				 * Returns the data of this object.
				 * @return Encapsulated data of this object
				 */
				inline const uint8_t* operator()() const;

			private:

				/// The values.
				uint8_t values_[tChannels] = {0u};

				/// the id of the data.
				unsigned int id_ = (unsigned int)(-1);
		};

		/**
		 * Definition of a vector holding data object.
		 */
		typedef std::vector<Data> Datas;

		/**
		 * This class implements a single cluster for 3 channel 24 bit data objects.
		 */
		class Cluster
		{
			public:

				/**
				 * Creates a default cluster object.
				 */
				Cluster() = default;

				/**
				 * Copy constructor.
				 * @param cluster The cluster to be copied
				 */
				Cluster(const Cluster& cluster) = default;

				/**
				 * Move constructor.
				 * @param cluster The cluster to be moved
				 */
				Cluster(Cluster&& cluster) noexcept;

				/**
				 * Creates a new cluster object by a given center position.
				 * @param center The center positions
				 * @param expectedElements Optional number of expected elements to reserve memory
				 */
				explicit inline Cluster(const uint8_t* center, const size_t expectedElements = 0);

				/**
				 * Returns the first center value of this cluster.
				 * @param channel The channel to return the center value for, with range [0, tChannels- 1]
				 * @return Center value
				 */
				inline uint8_t center(const unsigned int channel) const;

				/**
				 * Returns the variance of the first value.
				 * @param channel The channel to return the variance for, with range [0, tChannels - 1]
				 * @return First variance
				 */
				inline unsigned int variance(const unsigned int channel) const;

				/**
				 * Returns the centers of this cluster.
				 * @return Cluster centers
				 */
				inline const uint8_t* centers() const;

				/**
				 * Returns the elements of this cluster.
				 * @return Cluster elements
				 */
				inline const Datas& datas() const;

				/**
				 * Returns the number of elements this cluster holds.
				 * @return Cluster size
				 */
				inline size_t size() const;

				/**
				 * Adds a new data value object to this cluster.
				 * @param data The data object to be added
				 */
				inline void addData(const Data& data);

				/**
				 * Calculates or updates the value variance of this cluster.
				 */
				void calculateVariance();

				/**
				 * Returns whether this cluster holds at least one element.
				 * @return True if so
				 */
				explicit inline operator bool() const;

				/**
				 * Returns whether this cluster holds more elements than a second one.
				 * @param cluster Second cluster object to compare
				 * @return True, if so
				 */
				inline bool operator<(const Cluster& cluster) const;

				/**
				 * Move operator
				 * @param right The right cluster to assign
				 * @return Reference to this cluster
				 */
				Cluster& operator=(Cluster&& right) noexcept;

			private:

				// Center values.
				uint8_t centers_[tChannels] = {0u};

				/// Variance values.
				unsigned int variances_[tChannels] = {0u};

				/// Data values of this cluster.
				Datas datas_;
		};

		/// Definition of a vector holding cluster objects.
		typedef std::vector<Cluster> Clusters;

		/**
		 * This class implements the management of clusters.
		 */
		class Segmentation
		{
			public:

				/**
				 * Creates a new segmentation object.
				 */
				Segmentation() = default;

				/**
				 * Creates a new segmentation object.
				 * @param clusters The clusters defining the segmentation
				 */
				explicit Segmentation(const Clusters& clusters);

				/**
				 * Move constructor.
				 * @param segmentation The segmentation to be moved
				 */
				Segmentation(Segmentation&& segmentation) noexcept;

				/**
				 * Returns the clusters defined by this segmentation.
				 * @return Segmentation clusters
				 */
				inline const Clusters& clusters() const;

				/**
				 * Returns the average cluster size.
				 * @return Average cluster size
				 */
				inline float averageClusterSize() const;

				/**
				 * Returns the maximal cluster size of this segmentation.
				 * @return Maximal cluster size
				 */
				inline size_t maximalClusterSize() const;

				/**
				 * Compares two segmentation regarding to the maximal cluster size.
				 * @param first The first segmentation object
				 * @param second the second segmentation object
				 */
				static inline bool compareMaximalClusterSize(const Segmentation& first, const Segmentation& second);

				/**
				 * Move operator
				 * @param right The right segmentation to assign
				 * @return Reference to this segmentation
				 */
				Segmentation& operator=(Segmentation&& right) noexcept;

			private:

				/// Clusters of this segmentation object.
				Clusters clusters_;

				/// Average cluster size of this segmentation.
				float averageClusterSize_ = -1.0f;

				/// Maximal cluster size of this segmentation.
				size_t maximalClusterSize_ = 0;
		};

	public:

		/**
		 * Determines a random cluster for data elements by application of one seeking iteration.
		 * @param datas The data objects to distribute into clusters
		 * @param clusterRadius Radius of each cluster
		 * @param randomGenerator Random number generator
		 * @param expectedClusters Optional number of expected clusters allowing to reserve enough space in the beginning
		 * @return Resulting segmentation with clusters
		 */
		static Segmentation findRandomClusteringOneIteration(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const size_t expectedClusters = 20);

		/**
		 * Determines a random cluster for data elements by application of two seeking iterations.
		 * The first iteration determines all elements belonging to a randomly selected cluster element.<br>
		 * The second iteration seeks all element belonging to the average data as determined by the first iteration.<br>
		 * @param datas The data objects to distribute into clusters
		 * @param clusterRadius Radius of each cluster
		 * @param randomGenerator Random number generator
		 * @param expectedClusters Optional number of expected clusters allowing to reserve enough space in the beginning
		 * @return Resulting segmentation with clusters
		 */
		static Segmentation findRandomClusteringTwoIterations(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const size_t expectedClusters = 20);

		/**
		 * Determines an optimal cluster from a set of random clusters for 3 channel 24 bit data elements
		 * @param datas The data objects to distribute into clusters
		 * @param clusterRadius Radius of a single cluster
		 * @param randomGenerator Random number generator
		 * @param iterations The number of clustering steps, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param oneIteration State determining whether one ore two seeking iterations are applied for each individual clustering step
		 * @return Resulting optimal segmentation with clusters
		 */
		static Segmentation findOptimalRandomClustering(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const unsigned int iterations = 10u, Worker* worker = nullptr, const bool oneIteration = true);

	private:

		/**
		 * Determines an optimal cluster from a set of random clusters for 3 channel 24 bit data elements
		 * @param datas The data objects to distribute into clusters
		 * @param clusterRadius Radius of a single cluster
		 * @param randomGenerator Random number generator
		 * @param segmentation Resulting optimal segmentation
		 * @param oneIteration State determining whether one or two seeking iterations are executed for each clustering
		 * @param firstIteration First iteration to be handled
		 * @param numberIterations Number iterations to be handled
		 * @param iterationIndex Iteration index
		 */
		static void findOptimalRandomClusteringSubset(const Datas* datas, const unsigned int clusterRadius, RandomGenerator* randomGenerator, Segmentation* segmentation, const bool oneIteration, const unsigned int firstIteration, const unsigned int numberIterations, const unsigned int iterationIndex);
};

template <unsigned int tChannels>
inline Clustering<tChannels>::Data::Data(const uint8_t* const value, const unsigned int id) :
	id_(id)
{
	ocean_assert(value != nullptr);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		values_[n] = value[n];
	}
}

template <unsigned int tChannels>
inline uint8_t Clustering<tChannels>::Data::value(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);

	return values_[channel];
}

template <unsigned int tChannels>
inline unsigned int Clustering<tChannels>::Data::id() const
{
	return id_;
}

template <unsigned int tChannels>
inline unsigned int Clustering<tChannels>::Data::channels() const
{
	return tChannels;
}

template <unsigned int tChannels>
inline unsigned int Clustering<tChannels>::Data::ssd(const Data& data) const
{
	unsigned int result = 0u;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		result += sqrDistance(values_[n], data.values_[n]);
	}

	return result;
}

template <unsigned int tChannels>
inline unsigned int Clustering<tChannels>::Data::ssd(const uint8_t* values) const
{
	ocean_assert(values);

	unsigned int result = 0u;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		result += sqrDistance(values_[n], values[n]);
	}

	return result;
}

template <unsigned int tChannels>
inline const uint8_t* Clustering<tChannels>::Data::operator()() const
{
	return values_;
}

template <unsigned int tChannels>
inline bool Clustering<tChannels>::Data::ssd(const Data& data, const unsigned int sqrChannel) const
{
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (sqrDistance(values_[n], data.values_[n]) > sqrChannel)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline bool Clustering<tChannels>::Data::ssd(const uint8_t* values, const unsigned int sqrChannel) const
{
	ocean_assert(values != nullptr);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (sqrDistance(values_[n], values[n]) > sqrChannel)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline Clustering<tChannels>::Cluster::Cluster(Cluster&& cluster) noexcept
{
	*this = std::move(cluster);
}

template <unsigned int tChannels>
inline Clustering<tChannels>::Cluster::Cluster(const uint8_t* center, const size_t expectedElements)
{
	ocean_assert(center != nullptr);

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		centers_[n] = center[n];
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		variances_[n] = (unsigned int)(-1);
	}

	if (expectedElements != 0)
	{
		datas_.reserve(expectedElements);
	}
}

template <unsigned int tChannels>
inline uint8_t Clustering<tChannels>::Cluster::center(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);
	return centers_[channel];
}

template <unsigned int tChannels>
inline unsigned int Clustering<tChannels>::Cluster::variance(const unsigned int channel) const
{
	ocean_assert(channel < tChannels);
	ocean_assert(variances_[channel] != (unsigned int)(-1));

	return variances_[channel];
}

template <unsigned int tChannels>
inline const uint8_t* Clustering<tChannels>::Cluster::centers() const
{
	return centers_;
}

template <unsigned int tChannels>
inline const typename Clustering<tChannels>::Datas& Clustering<tChannels>::Cluster::datas() const
{
	return datas_;
}

template <unsigned int tChannels>
inline size_t Clustering<tChannels>::Cluster::size() const
{
	return datas_.size();
}

template <unsigned int tChannels>
inline void Clustering<tChannels>::Cluster::addData(const Data& data)
{
	datas_.push_back(data);
}

template <unsigned int tChannels>
void Clustering<tChannels>::Cluster::calculateVariance()
{
	if (datas_.empty())
	{
		return;
	}

	uint64_t means[tChannels] = {0u};
	uint64_t sqrMeans[tChannels] = {0u};

	for (const Data& data : datas_)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			means[n] += uint64_t(data.value(n));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			sqrMeans[n] += uint64_t(sqr(data.value(n)));
		}
	}

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		variances_[n] = (unsigned int)((sqrMeans[n] * uint64_t(datas_.size()) - sqr(means[n])) / sqr(uint64_t(datas_.size())));
	}
}

template <unsigned int tChannels>
inline Clustering<tChannels>::Cluster::operator bool() const
{
	return !datas_.empty();
}

template <unsigned int tChannels>
inline bool Clustering<tChannels>::Cluster::operator<(const Cluster& cluster) const
{
	return datas_.size() > cluster.datas_.size();
}

template <unsigned int tChannels>
inline typename Clustering<tChannels>::Cluster& Clustering<tChannels>::Cluster::operator=(Cluster&& right) noexcept
{
	if (this != &right)
	{
		datas_ = std::move(right.datas_);

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			centers_[n] = right.centers_[n];
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			variances_[n] = right.variances_[n];
		}
	}

	return *this;
}

template <unsigned int tChannels>
Clustering<tChannels>::Segmentation::Segmentation(const Clusters& clusters) :
	clusters_(clusters),
	averageClusterSize_(-1.0f),
	maximalClusterSize_(0u)
{
	size_t total = 0u;

	for (const Cluster& cluster : clusters)
	{
		if (cluster.size() > maximalClusterSize_)
		{
			maximalClusterSize_ = cluster.size();
		}

		total += cluster.size();
	}

	if (!clusters.empty())
	{
		averageClusterSize_ = float(total) / float(clusters.size());
	}
}

template <unsigned int tChannels>
inline Clustering<tChannels>::Segmentation::Segmentation(Segmentation&& segmentation) noexcept
{
	*this = std::move(segmentation);
}

template <unsigned int tChannels>
inline const typename Clustering<tChannels>::Clusters& Clustering<tChannels>::Segmentation::clusters() const
{
	return clusters_;
}

template <unsigned int tChannels>
inline float Clustering<tChannels>::Segmentation::averageClusterSize() const
{
	return averageClusterSize_;
}

template <unsigned int tChannels>
inline size_t Clustering<tChannels>::Segmentation::maximalClusterSize() const
{
	return maximalClusterSize_;
}

template <unsigned int tChannels>
inline bool Clustering<tChannels>::Segmentation::compareMaximalClusterSize(const Segmentation& first, const Segmentation& second)
{
	return first.maximalClusterSize() > second.maximalClusterSize();
}

template <unsigned int tChannels>
inline typename Clustering<tChannels>::Segmentation& Clustering<tChannels>::Segmentation::operator=(Segmentation&& right) noexcept
{
	if (this != &right)
	{
		clusters_ = std::move(right.clusters_);
		averageClusterSize_ = right.averageClusterSize_;
		maximalClusterSize_ = right.maximalClusterSize_;
	}

	return *this;
}

template <unsigned int tChannels>
typename Clustering<tChannels>::Segmentation Clustering<tChannels>::findRandomClusteringOneIteration(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const size_t expectedClusters)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	Datas remainingDatas(datas);
	const unsigned int sqrClusterRadius = sqr(clusterRadius);

	Datas tmpDatas;
	tmpDatas.reserve(datas.size());

	Clusters clusters;
	clusters.reserve(expectedClusters);

	while (!remainingDatas.empty())
	{
		const unsigned int randomFingerprintIndex = RandomI::random(randomGenerator, min((unsigned int)remainingDatas.size() - 1u, randomGenerator.randMax()));
		const Data& randomData = remainingDatas[randomFingerprintIndex];

		clusters.push_back(Cluster(randomData(), remainingDatas.size()));
		Cluster& newCluster = clusters.back();

		tmpDatas.clear();

		// create new cluster
		for (typename Datas::const_iterator i = remainingDatas.begin(); i != remainingDatas.end(); ++i)
			if (randomData.ssd(*i, sqrClusterRadius))
				newCluster.addData(*i);
			else
				tmpDatas.push_back(*i);

		std::swap(remainingDatas, tmpDatas);
	}

	return Segmentation(std::move(clusters));
};

template <unsigned int tChannels>
typename Clustering<tChannels>::Segmentation Clustering<tChannels>::findRandomClusteringTwoIterations(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const size_t expectedClusters)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	Datas copyDatas(datas);

	const unsigned int sqrClusterRadius = sqr(clusterRadius);

	Datas remainingDatas;
	remainingDatas.reserve(datas.size());

	Clusters clusters;
	clusters.reserve(expectedClusters);

	while (!copyDatas.empty())
	{
		const unsigned int randomFingerprintIndex = RandomI::random(randomGenerator, (unsigned int)(copyDatas.size()) - 1u);

		const Data& randomData = copyDatas[randomFingerprintIndex];

		// create new cluster

		unsigned int total[tChannels] = {0u};
		unsigned int totalNumber = 0u;

		// front
		for (const Data& copyData : copyDatas)
		{
			if (randomData.ssd(copyData, sqrClusterRadius))
			{
				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					// check that enough space is left
					ocean_assert(uint64_t(total[n]) + uint64_t(copyData.value(n)) < uint64_t((unsigned int)(-1)));

					total[n] += copyData.value(n);
				}

				++totalNumber;
			}
		}

		ocean_assert(totalNumber != 0u);

		uint8_t centers[tChannels];

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			const unsigned int center = (total[n] + totalNumber / 2u) / totalNumber;
			ocean_assert(center <= 255u);

			centers[n] = uint8_t(center);
		}

		clusters.emplace_back(centers, copyDatas.size());
		Cluster& newCluster = clusters.back();

		remainingDatas.clear();

		for (const Data& copyData : copyDatas)
		{
			if (copyData.ssd(newCluster.centers(), sqrClusterRadius))
			{
				newCluster.addData(copyData);
			}
			else
			{
				remainingDatas.emplace_back(copyData);
			}
		}

		std::swap(copyDatas, remainingDatas);
	}

	return Segmentation(std::move(clusters));
};

template <unsigned int tChannels>
typename Clustering<tChannels>::Segmentation Clustering<tChannels>::findOptimalRandomClustering(const Datas& datas, const unsigned int clusterRadius, RandomGenerator& randomGenerator, const unsigned int iterations, Worker* worker, const bool oneIteration)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(!datas.empty() && iterations > 0u);

	if (datas.empty() || iterations == 0u)
	{
		return Segmentation();
	}

	if (worker)
	{
		std::vector<Segmentation> results(worker->threads());
		ocean_assert(!results.empty());

		worker->executeFunction(Worker::Function::createStatic(&Clustering<tChannels>::findOptimalRandomClusteringSubset, &datas, clusterRadius, &randomGenerator, &results[0], oneIteration, 0u, 0u, 0u), 0u, iterations, 5u, 6u, 1u, 7u);

		unsigned int bestIndex = 0u;

		for (unsigned int n = 1u; n < results.size(); ++n)
		{
			if (results[n].clusters().size() < results[bestIndex].clusters().size())
			{
				bestIndex = n;
			}
			else if (results[n].clusters().size() == results[bestIndex].clusters().size())
			{
				/*unsigned int maxVariance = 0u;
				for (Clusters::const_iterator i = clusters.begin(); i != clusters.end(); ++i)
				{
					maxVariance = max(maxVariance, max(i->clusterVariance0, max(i->clusterVariance1, i->clusterVariance2)));
				}

				unsigned int testMaxVariance = 0u;
				for (Clusters::const_iterator i = testClusters.begin(); i != testClusters.end(); ++i)
				{
					testMaxVariance = max(maxVariance, max(i->clusterVariance0, max(i->clusterVariance1, i->clusterVariance2)));
				}

				if (testMaxVariance < maxVariance)
				{
					clusters = testClusters;
				}*/

				if (results[n].maximalClusterSize() > results[n].maximalClusterSize())
				{
					bestIndex = n;
				}
			}
		}

		return std::move(results[bestIndex]);
	}
	else
	{
		Segmentation result;
		findOptimalRandomClusteringSubset(&datas, clusterRadius, &randomGenerator, &result, oneIteration, 0u, iterations, 0u);

		return result;
	}
}

template <unsigned int tChannels>
void Clustering<tChannels>::findOptimalRandomClusteringSubset(const Datas* datas, const unsigned int clusterRadius, RandomGenerator* randomGenerator, Segmentation* segmentation, const bool oneIteration, const unsigned int /*firstIteration*/, const unsigned int numberIterations, const unsigned int iterationIndex)
{
	ocean_assert(datas != nullptr && randomGenerator != nullptr && segmentation != nullptr);
	ocean_assert(numberIterations > 0u);

	RandomGenerator generator(*randomGenerator);

	Segmentation& result = segmentation[iterationIndex];

	result = oneIteration ? findRandomClusteringOneIteration(*datas, clusterRadius, generator, 20u)
							: findRandomClusteringTwoIterations(*datas, clusterRadius, generator, 20u);

	for (unsigned int n = 1u; n < numberIterations; ++n)
	{
		Segmentation testSegmentation(oneIteration ? findRandomClusteringOneIteration(*datas, clusterRadius, generator, max(20u, (unsigned int)(result.clusters().size()) * 2u))
														: findRandomClusteringTwoIterations(*datas, clusterRadius, generator, max(20u, (unsigned int)(result.clusters().size()) * 2u)));

		if (testSegmentation.clusters().size() < result.clusters().size())
		{
			result = std::move(testSegmentation);
		}
		else if (testSegmentation.clusters().size() == result.clusters().size())
		{
			/*unsigned int maxVariance = 0u;
			for (Clusters::const_iterator i = clusters.begin(); i != clusters.end(); ++i)
			{
				maxVariance = max(maxVariance, max(i->clusterVariance0, max(i->clusterVariance1, i->clusterVariance2)));
			}

			unsigned int testMaxVariance = 0u;
			for (Clusters::const_iterator i = testClusters.begin(); i != testClusters.end(); ++i)
			{
				testMaxVariance = max(maxVariance, max(i->clusterVariance0, max(i->clusterVariance1, i->clusterVariance2)));
			}

			if (testMaxVariance < maxVariance)
			{
				clusters = testClusters;
			}*/

			if (testSegmentation.maximalClusterSize() > result.maximalClusterSize())
			{
				result = std::move(testSegmentation);
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_CLUSTERING_H
