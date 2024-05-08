/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MATH_CLUSTERING_K_MEANS_H
#define META_OCEAN_MATH_CLUSTERING_K_MEANS_H

#include "ocean/math/Math.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/StaticBuffer.h"
#include "ocean/base/Utilities.h"
#include "ocean/base/Worker.h"

#include <climits>

namespace Ocean
{

/**
 * This class implements the base class for all classes providing clustering algorithms.
 * The data (the observations) that will be distributed into individual clusters can be provided with two different modes.<br>
 * The first mode clusters the data elements by their indices, thus the data elements must be provided as joined data block (an array of elements).<br>
 * Use the first mode by setting tUseIndices to 'True'.<br>
 * The second mode clusters the data elements by their pointers, thus the data elements may lie at arbitrary positions in the memory.<br>
 * Use the second mode by setting tUseIndices to 'False'.
 * @tparam tUseIndices True,
 * @ingroup math
 */
template <bool tUseIndices>
class Clustering
{
	public:

		/**
		 * This class implements the abstract data object which will be specialized for both data modes toggled by tUseIndices.
		 * @tparam T The data type of each element of an observation
		 * @tparam tDimension The dimension of each observation (the number of elements in each observation), with range [1, infinity)
		 */
		template <typename T, size_t tDimension>
		class Data
		{
			// nothing to do here
		};
};

/**
 * Specialization for Clustering<true>::Data<T, tDimension>.
 * This data class implements the first data mode of the clustering class identifying observations by their indices.<br>
 * @tparam T The data type of each element of an observation
 * @tparam tDimension The dimension of each observation (the number of elements in each observation), with range [1, infinity)
 */
template <>
template <typename T, size_t tDimension>
class Clustering<true>::Data
{
	public:

		/**
		 * Definition of an observation object.
		 */
		typedef StaticBuffer<T, tDimension> Observation;

		/**
		 * Definition of an index that addresses one specific observation element in the data object that stores all observations.
		 */
		typedef size_t DataIndex;

		/**
		 * Definition of a vector holding indices to the data object.
		 */
		typedef std::vector<DataIndex> DataIndices;

	public:

		/**
		 * Creates a new empty data object.
		 */
		Data() = default;

		/**
		 * Creates a new data object by observations lying in a joined memory block as array.
		 * Due to performance issues: The given observations can be copied or used directly without any memory copy.<br>
		 * Beware: If no copy of the observations is created, the given observations must exist as long as this data object (or the corresponding clustering object) exists.
		 * @param observations The first observation in the given joined memory block
		 * @param numberObservations The number of observations that are provided, with range [1, infinity)
		 * @param copyObservations True, to copy the given observations; False, to simply use the given observations as reference
		 */
		Data(const Observation* observations, const size_t numberObservations, const bool copyObservations = false);

		/**
		 * Move constructor for an data object.
		 * @param data The data object to be moved
		 */
		inline Data(Data<T, tDimension>&& data) noexcept;

		/**
		 * Returns one specific observation of this data object specified by the data-index of this observation.
		 * @param dataIndex The data-index of the observation that will be returned, ensure that the given data-index is valid
		 * @return The specified observation
		 * @see isValidDataIndex().
		 */
		inline const Observation& observation(const DataIndex& dataIndex) const;

		/**
		 * Returns the number of observations that are stored by this data object.
		 * @return Number of observations
		 */
		inline size_t numberObservations() const;

		/**
		 * Returns whether a given data-index is valid and has a corresponding observation stored in this data object.
		 * @param dataIndex The data-index that will be checked
		 * @return True, if so
		 */
		inline bool isValidDataIndex(const DataIndex& dataIndex) const;

		/**
		 * Returns one specific observation of this data object specified by the data-index of this observation.
		 * @param dataIndex The data-index of the observation that will be returned, ensure that the given data-index is valid
		 * @return The specified observation
		 * @see isValidDataIndex(), observation()..
		 */
		inline const Observation& operator[](const DataIndex& dataIndex) const;

		/**
		 * Move operator.
		 * @param data The data object that will be moved to this object.
		 * @return Reference to this object.
		 */
		inline Data<T, tDimension>& operator=(Data<T, tDimension>&& data) noexcept;

		/**
		 * Returns whether this data object holds at least one observation.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The optional observations that are stored as copy.
		std::vector<Observation> copyObservations_;

		/// The observation objects of this data object.
		const Observation* observations_ = nullptr;

		/// The number of observation elements of this data object.
		size_t numberObservations_ = 0;
};

/**
 * Specialization for Clustering<true>::Data<T, tDimension>.
 * This data class implements the second data mode of the clustering class identifying observations by their pointers.<br>
 * @tparam T The data type of each element of an observation
 * @tparam tDimension The dimension of each observation (the number of elements in each observation), with range [1, infinity)
 */
template <>
template <typename T, size_t tDimension>
class Clustering<false>::Data
{
	public:

		/**
		 * Definition of an observation object.
		 */
		typedef StaticBuffer<T, tDimension> Observation;

		/**
		 * Definition of an index that addresses one specific observation element in the data object that stores all observations.
		 */
		typedef size_t DataIndex;

		/**
		 * Definition of a vector holding indices to the data object.
		 */
		typedef std::vector<DataIndex> DataIndices;

	public:

		/**
		 * Creates a new empty data object.
		 */
		Data() = default;

		/**
		 * Creates a new data object by observations lying at individual memory positions.
		 * Due to performance issues: The given observation !pointers! can be copied or used directly without any copy.<br>
		 * Beware: If no copy of the observation pointers is created, the given observation pointers must exist as long as this data object (or the corresponding clustering object) exists.<br>
		 * Beware: In any case, the observations (not the pointers) must exist as long as this data object (or the corresponding clustering object) exists.<br>
		 * @param observationPointers The first pointer of the observations
		 * @param numberObservations The number of observations/pointers that are provided, with range [1, infinity)
		 * @param copyPointers True, to copy the given observation pointers (not the observations); False, to simply use the given observation pointers as reference
		 */
		Data(const Observation** observationPointers, const size_t numberObservations, const bool copyPointers = false);

		/**
		 * Move constructor for an data object.
		 * @param data The data object to be moved
		 */
		inline Data(Data<T, tDimension>&& data) noexcept;

		/**
		 * Returns one specific observation of this data object specified by the data-index of this observation.
		 * @param dataIndex The data-index of the observation that will be returned, ensure that the given data-index is valid
		 * @return The specified observation
		 * @see isValidDataIndex().
		 */
		inline const Observation& observation(const DataIndex& dataIndex) const;

		/**
		 * Returns the number of observations that are stored by this data object.
		 * @return Number of observations
		 */
		inline size_t numberObservations() const;

		/**
		 * Returns whether a given data-index is valid and has a corresponding observation stored in this data object.
		 * @param dataIndex The data-index that will be checked
		 * @return True, if so
		 */
		inline bool isValidDataIndex(const DataIndex& dataIndex) const;

		/**
		 * Returns one specific observation of this data object specified by the data-index of this observation.
		 * @param dataIndex The data-index of the observation that will be returned, ensure that the given data-index is valid
		 * @return The specified observation
		 * @see isValidDataIndex(), observation()..
		 */
		inline const Observation& operator[](const DataIndex& dataIndex) const;

		/**
		 * Move operator.
		 * @param data The data object that will be moved to this object.
		 * @return Reference to this object.
		 */
		inline Data<T, tDimension>& operator=(Data<T, tDimension>&& data) noexcept;

		/**
		 * Returns whether this data object holds at least one observation.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The optional observation pointers that are stored as copy.
		std::vector<const Observation*> copyObservationPointers_;

		/// The observation pointers of this data object.
		const Observation** observationPointers_ = nullptr;

		/// The number of observation elements of this data object.
		size_t numberObservations_;
};

/**
 * This class implements a k-means clustering algorithm.
 * Beware: Due to performance issues, this class does not copy the given observation values, this expects that the given observation values exist as long as the KMean object exists.<br>
 * @tparam T The data type of each element of an observation
 * @tparam tDimension The dimension of each observation (the number of elements in each observation), with range [1, infinity)
 * @tparam TSum The data type of the intermediate sum values, that is necessary to determine e.g. the mean parameters
 * @tparam TSquareDistance The data type of the square distance value, might be different from T
 * @ingroup math
 */
template <typename T, size_t tDimension, typename TSum = T, typename TSquareDistance = T, bool tUseIndices = true>
class ClusteringKMeans : public Clustering<tUseIndices>
{
	public:

		/**
		 * Definition of individual initialization strategies.
		 */
		enum InitializationStrategy
		{
			/// The first cluster is determined by selection of the (euclidean) smallest observation, the remaining clusters are defined by observations with largest distance to the already existing clusters.
			IS_LARGEST_DISTANCE,
			/// All clusters are selected randomly.
			IS_RANDOM
		};

		/**
		 * (Re-)Definition of a data object providing the data which will be clustered.
		 */
		typedef typename Clustering<tUseIndices>::template Data<T, tDimension> Data;

		/**
		 * (Re-)Definition of an index that addresses one specific observation element in the data object that stores all observations.
		 */
		typedef typename Data::DataIndex DataIndex;

		/**
		 * (Re-)Definition of a vector holding (size_t) indices.
		 */
		typedef typename Data::DataIndices DataIndices;

		/**
		 * (Re-)Definition of an observation object.
		 */
		typedef typename Data::Observation Observation;

		/**
		 * This class implements one cluster that holds the mean values of all observations belonging to this cluster and the indices of all observations belonging to this cluster.
		 */
		class Cluster
		{
			// Two friend class declaration, necessary as the constructors of this class are all protected.
			friend class ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>;
			friend class std::allocator<Cluster>;

			public:

				/**
				 * Move constructor for another cluster object.
				 * @param cluster The cluster object to be moved
				 */
				inline Cluster(Cluster&& cluster) noexcept;

				/**
				 * Returns the mean observation value of this cluster.
				 * @return The cluster's mean observation value
				 */
				inline const Observation& mean() const;

				/**
				 * Returns the indices of the observations that belong to this cluster.
				 * @return The cluster's observation indices
				 */
				inline const DataIndices& dataIndices() const;

				/**
				 * Returns the square distance between a given observation and this cluster (the mean observation value of this cluster).
				 * @param observation The observation for that the distance is returned
				 * @return Resulting square distance
				 */
				inline TSquareDistance sqrDistance(const Observation& observation) const;

				/**
				 * Calculates the maximal square distance between the mean observation value of this cluster and all observations which belong to this cluster.
				 * @param observationIndex Optional resulting index of the observation with maximal distance to the mean observation value, if defined
				 * @return Maximal square distance
				 */
				TSquareDistance maximalSqrDistance(DataIndex* observationIndex = nullptr) const;

				/**
				 * Calculates the average square distance between the mean observation value of this cluster and all observations which belong to this cluster.
				 * @return Average square distance
				 */
				TSquareDistance averageSqrDistance() const;

				/**
				 * Move operator moving a cluster object to this object.
				 * @param cluster The cluster object to be moved
				 * @return Reference to this object
				 */
				inline Cluster& operator=(Cluster&& cluster) noexcept;

				/**
				 * Returns whether the left cluster has less elements than the right cluster.
				 * @param cluster The right cluster to compare
				 * @return True, if so
				 */
				inline bool operator<(const Cluster& cluster) const;

			protected:

				/**
				 * Creates a new cluster object by a given mean value.
				 * @param owner The owner of this cluster object
				 * @param mean The mean observation value that will define this cluster
				 */
				inline Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean);

				/**
				 * Creates a new cluster object by a given mean value and several indices of observations that belong to this cluster.
				 * @param owner The owner of this cluster object
				 * @param mean The mean observation value that will define this cluster
				 * @param dataIndices The data-indices of the observations that belong to this cluster
				 */
				inline Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean, const DataIndices& dataIndices);

				/**
				 * Creates a new cluster object by a given mean value and several indices of observations that belong to this cluster.
				 * @param owner The owner of this cluster object
				 * @param mean The mean observation value that will define this cluster
				 * @param dataIndices The data-indices of the observations that belong to this cluster; beware: the indices will be moved to this cluster object
				 */
				inline Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean, DataIndices&& dataIndices);

				/**
				 * Returns the data-indices of the observations that belong to this cluster.
				 * @return The cluster's observation indices
				 */
				inline DataIndices& dataIndices();

				/**
				 * Updates the mean observation value of this cluster by application of the stored indices of all observations that belong to this cluster.
				 */
				void updateMean();

			protected:

				/// The owner of this cluster.
				const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>* owner_;

				/// The mean observation value of this cluster.
				Observation mean_;

				/// The data indices of all observation that belong to this cluster.
				DataIndices dataIndices_;
		};

		/**
		 * Definition of a vector holding cluster objects.
		 */
		typedef std::vector<Cluster> Clusters;

	public:

		/**
		 * Creates an empty k-means object.
		 */
		inline ClusteringKMeans();

		/**
		 * Move constructor.
		 * @param clustering The clustering object to be moved
		 */
		inline ClusteringKMeans(ClusteringKMeans&& clustering) noexcept;

		/**
		 * Creates a new k-means object by a given data object.
		 * @param data The data object to be used to determine the clusters.
		 * @see determineClusters().
		 */
		inline explicit ClusteringKMeans(const Data &data);

		/**
		 * Creates a new k-means object by a given data object.
		 * @param data The data object that will be moved and used to determine the clusters.
		 * @see determineClusters().
		 */
		inline explicit ClusteringKMeans(Data &&data);

		/**
		 * Returns the clusters of this k-means clustering object.
		 * @return The determined k-means clusters
		 */
		inline const Clusters& clusters() const;

		/**
		 * Sorts the clusters regarding their number of elements.
		 */
		void sortClusters();

		/**
		 * Calculates the maximal square distance between the mean observation value of each clusters and all observations belonging to the cluster.
		 * @return Maximal square distance for all clusters
		 */
		TSquareDistance maximalSqrDistance() const;

		/**
		 * Determines the clusters for this object, ensure that this object has been initialized with a valid set of observations.
		 * @param numberClusters The number of clusters that will be created, with range [1, numberObservations())
		 * @param strategy The initialization strategy for the first clusters
		 * @param iterations The number of optimization iterations that are applied after the initial clusters have been determined, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @see clusters().
		 */
		void determineClustersByNumber(const size_t numberClusters, const InitializationStrategy strategy = IS_LARGEST_DISTANCE, const size_t iterations = 5, Worker* worker = nullptr);

		/**
		 * Determines the clusters for this object, ensure that this object has been initialized with a valid set of observations.
		 * This function adds new clusters within several iterations until the defined maximalSqrDistance is larger than the distance within all clusters or until the defined maximal number of clusters is reached.<br>
		 * @param maximalSqrDistance The maximal square distance in the final clusters between the clusters' mean observation values and the observations in the clusters
		 * @param maximalClusters The maximal number of clusters that will be created (even if maximalSqrDistance is not reached), with range [0, infinity), define 0 to ignore this parameter
		 * @param iterations The number of optimization iterations that are applied after each time a new cluster is added [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		void determineClustersByDistance(const TSquareDistance maximalSqrDistance, size_t maximalClusters = 0, const size_t iterations = 5, Worker* worker = nullptr);

		/**
		 * Adds a new clusters for this object.
		 * @param iterations The number of optimization iterations that are applied after the new cluster has been added, with range [1, infinity)
		 * @param sqrDistance The minimal square distance between the cluster's mean and an observation of this cluster so that this cluster is divided into two clusters
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if a new cluster have been added, False if no further cluster could be added or if the provided distance was too large
		 */
		bool addCluster(const size_t iterations = 5, TSquareDistance sqrDistance = TSquareDistance(0), Worker* worker = nullptr);

		/**
		 * Removes one cluster from this object.
		 * The cluster with smallest maximal distance of all observations to the mean observation value of the clusters is removed.
		 * @param iterations The number of optimization iterations that are applied after the cluster has been removed, with range [1, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		void removeCluster(const size_t iterations = 5, Worker* worker = nullptr);

		/**
		 * Finds a best matching cluster for a given independent observation.
		 * However, the observation is not added to this cluster, it's simply a lookup for the best matching cluster.
		 * @param observation The observation for that the best matching cluster is determined
		 * @return The index of the best matching cluster, -1 if no cluster could be found
		 * @see clusters().
		 */
		size_t findCluster(const Observation& observation);

		/**
		 * Explicitly applies one further optimization iteration for an existing set of clusters.
		 * Do not call this function before initial clusters have been found.
		 * @see clusters(), determineCluster().
		 */
		void applyOptimizationIteration();

		/**
		 * Explicitly applies one further optimization iteration for an existing set of clusters.
		 * Do not call this function before initial clusters have been found.
		 * @param worker The worker object to distribute the computation
		 * @see clusters(), determineCluster().
		 */
		void applyOptimizationIteration(Worker* worker);

		/**
		 * Clears all determined clusters but registered the data information is untouched.
		 */
		void clear();

		/**
		 * Returns whether this object holds a valid set of observations.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object holds a valid set of observations.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param clustering The clustering object to be moved
		 * @return Reference to this object
		 */
		inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& operator=(ClusteringKMeans&& clustering);

	protected:

		/**
		 * Determines the initial clusters for this object with the IS_LARGEST_DISTANCE strategy.
		 * First the smallest observation object is selected as first cluster,<br>
		 * all following clusters are determined by observations that have the largest distance to the already existing clusters.<br>
		 * @param numberClusters The number of initial clusters that will be created.
		 */
		void determineInitialClustersLargestDistance(const size_t numberClusters);

		/**
		 * Determines the initial clusters for this object with the IS_RANDOM strategy.
		 * All clusters are created randomly.br>
		 * @param numberClusters The number of initial clusters that will be created.
		 */
		void determineInitialClustersRandom(const size_t numberClusters);

		/**
		 * Explicitly applies one further optimization iteration for an existing set of clusters.
		 * This functions operates on a subset of all observations.<br>
		 * @param lock Optional lock object if this function is executed on multiple threads in parallel
		 * @param firstObservation The first observation that will be handled
		 * @param numberObservations The number of observations that will be handled
		 * @see clusters(), determineCluster().
		 */
		void applyOptimizationIterationSubset(Lock* lock, const unsigned int firstObservation, const unsigned int numberObservations);

		/**
		 * Determines the smallest observation (euclidean distance to origin) from a set of observations.
		 * @param data The observation data in which the smallest observation is determined, must be valid
		 * @return The index of the smallest observation
		 */
		static inline DataIndex smallestObservation(const Data& data);

		/**
		 * Returns the square distance between an observation and the origin.
		 * @param observation The observation for that the square distance is determined
		 * @return Resulting square distance
		 */
		static inline TSquareDistance sqrDistance(const Observation& observation);

	protected:

		/// The data that stores the observations of this clustering object, either with index-access or pointer-access.
		Data data_;

		/// The current clusters of this object.
		Clusters clusters_;
};

template <>
template <typename T, size_t tDimension>
Clustering<true>::Data<T, tDimension>::Data(const Observation* observations, const size_t numberObservations, const bool copyObservations) :
	copyObservations_(copyObservations ? numberObservations : 0),
	observations_(nullptr),
	numberObservations_(0)
{
	ocean_assert(observations && numberObservations > 0);

	if (copyObservations)
	{
		ocean_assert(copyObservations_.size() == numberObservations);
		memcpy(copyObservations_.data(), observations, sizeof(Observation) * numberObservations);

		observations_ = copyObservations_.data();
		numberObservations_ = copyObservations_.size();
	}
	else
	{
		observations_ = observations;
		numberObservations_ = numberObservations;
	}
}

template <>
template <typename T, size_t tDimension>
inline Clustering<true>::Data<T, tDimension>::Data(Data<T, tDimension>&& data) noexcept :
	copyObservations_(std::move(data.copyObservations_)),
	observations_(data.observations_),
	numberObservations_(data.numberObservations_)
{
	data.observations_ = nullptr;
	data.numberObservations_ = 0;
}

template <>
template <typename T, size_t tDimension>
inline const typename Clustering<true>::Data<T, tDimension>::Observation& Clustering<true>::Data<T, tDimension>::observation(const DataIndex& dataIndex) const
{
	ocean_assert(isValidDataIndex(dataIndex));
	return observations_[dataIndex];
}

template <>
template <typename T, size_t tDimension>
inline size_t Clustering<true>::Data<T, tDimension>::numberObservations() const
{
	return numberObservations_;
}

template <>
template <typename T, size_t tDimension>
inline bool Clustering<true>::Data<T, tDimension>::isValidDataIndex(const DataIndex& dataIndex) const
{
	return dataIndex < numberObservations_;
}

template <>
template <typename T, size_t tDimension>
inline const typename Clustering<true>::Data<T, tDimension>::Observation& Clustering<true>::Data<T, tDimension>::operator[](const DataIndex& dataIndex) const
{
	ocean_assert(isValidDataIndex(dataIndex));
	return observations_[dataIndex];
}

template <>
template <typename T, size_t tDimension>
inline typename Clustering<true>::Data<T, tDimension>& Clustering<true>::Data<T, tDimension>::operator=(Data<T, tDimension>&& data) noexcept
{
	if (this != &data)
	{
		copyObservations_ = std::move(data.copyObservations_);
		observations_ = data.observations_;
		numberObservations_ = data.numberObservations_;

		data.observations_ = nullptr;
		data.numberObservations_ = 0;
	}

	return *this;
}

template <>
template <typename T, size_t tDimension>
inline Clustering<true>::Data<T, tDimension>::operator bool() const
{
	return observations_ != nullptr;
}

template <>
template <typename T, size_t tDimension>
Clustering<false>::Data<T, tDimension>::Data(const Observation** observationPointers, const size_t numberObservations, const bool copyPointers) :
	copyObservationPointers_(copyPointers ? numberObservations : 0),
	observationPointers_(nullptr),
	numberObservations_(0)
{
	ocean_assert(observationPointers && numberObservations > 0);

	if (copyPointers)
	{
		ocean_assert(copyObservationPointers_.size() == numberObservations);
		memcpy(copyObservationPointers_.data(), observationPointers, sizeof(Observation*) * numberObservations);

		observationPointers_ = copyObservationPointers_.data();
		numberObservations_ = copyObservationPointers_.size();
	}
	else
	{
		observationPointers_ = observationPointers;
		numberObservations_ = numberObservations;
	}
}

template <>
template <typename T, size_t tDimension>
inline Clustering<false>::Data<T, tDimension>::Data(Data<T, tDimension>&& data) noexcept :
	copyObservationPointers_(std::move(data.copyObservationPointers_)),
	observationPointers_(data.observationPointers_),
	numberObservations_(data.numberObservations_)
{
	data.observationPointers_ = nullptr;
	data.numberObservations_ = 0;
}

template <>
template <typename T, size_t tDimension>
inline const typename Clustering<false>::Data<T, tDimension>::Observation& Clustering<false>::Data<T, tDimension>::observation(const DataIndex& dataIndex) const
{
	ocean_assert(isValidDataIndex(dataIndex));
	return *(observationPointers_[dataIndex]);
}

template <>
template <typename T, size_t tDimension>
inline size_t Clustering<false>::Data<T, tDimension>::numberObservations() const
{
	return numberObservations_;
}

template <>
template <typename T, size_t tDimension>
inline bool Clustering<false>::Data<T, tDimension>::isValidDataIndex(const DataIndex& dataIndex) const
{
	return dataIndex < numberObservations_;
}

template <>
template <typename T, size_t tDimension>
inline const typename Clustering<false>::Data<T, tDimension>::Observation& Clustering<false>::Data<T, tDimension>::operator[](const DataIndex& dataIndex) const
{
	ocean_assert(isValidDataIndex(dataIndex));
	return *(observationPointers_[dataIndex]);
}

template <>
template <typename T, size_t tDimension>
inline Clustering<false>::Data<T, tDimension>& Clustering<false>::Data<T, tDimension>::operator=(Data<T, tDimension>&& data) noexcept
{
	if (this != &data)
	{
		copyObservationPointers_ = std::move(data.copyObservationPointers_);
		observationPointers_ = data.observationPointers_;
		numberObservations_ = data.numberObservations_;

		data.observationPointers_ = nullptr;
		data.numberObservations_ = 0;
	}

	return *this;
}

template <>
template <typename T, size_t tDimension>
inline Clustering<false>::Data<T, tDimension>::operator bool() const
{
	return observationPointers_ != nullptr;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::Cluster(Cluster&& cluster) noexcept :
	owner_(cluster.owner_),
	mean_(std::move(cluster.mean_)),
	dataIndices_(std::move(cluster.dataIndices_))
{
	cluster.owner_ = nullptr;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean) :
	owner_(&owner),
	mean_(mean)
{
	// nothing to do here
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean, const DataIndices& dataIndices) :
	owner_(&owner),
	mean_(mean),
	dataIndices_(dataIndices)
{
	static_assert(tDimension != 0, "Invalid observation dimension!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::Cluster(const ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& owner, const Observation& mean, DataIndices&& dataIndices) :
	owner_(&owner),
	mean_(mean),
	dataIndices_(dataIndices)
{
	static_assert(tDimension != 0, "Invalid observation dimension!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline TSquareDistance ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::sqrDistance(const Observation& observation) const
{
	TSquareDistance result = TSquareDistance(0);

	for (size_t n = 0; n < tDimension; ++n)
	{
		result += (mean_[n] - observation[n]) * (mean_[n] - observation[n]);
	}

	return result;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
TSquareDistance ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::maximalSqrDistance(DataIndex* observationIndex) const
{
	ocean_assert(owner_);

	if (dataIndices_.empty())
	{
		return TSquareDistance(0);
	}

	const Data& data = owner_->data_;
	TSquareDistance result = TSquareDistance(0);

	for (typename DataIndices::const_iterator i = dataIndices_.begin(); i != dataIndices_.end(); ++i)
	{
		const TSquareDistance localDistance = sqrDistance(data[*i]);

		if (localDistance > result)
		{
			result = localDistance;

			if (observationIndex)
			{
				*observationIndex = *i;
			}
		}
	}

	return result;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
TSquareDistance ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::averageSqrDistance() const
{
	ocean_assert(owner_);

	if (dataIndices_.empty())
	{
		return TSquareDistance(0);
	}

  const Data& data = owner_->data_;
	TSquareDistance result = TSquareDistance(0);

	for (typename DataIndices::const_iterator i = dataIndices_.begin(); i != dataIndices_.end(); ++i)
	{
		result += sqrDistance(data[*i]);
	}

	ocean_assert(dataIndices_.size() != 0);
	return result / TSquareDistance(dataIndices_.size());
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline const typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Observation& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::mean() const
{
	return mean_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline const typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::DataIndices& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::dataIndices() const
{
	return dataIndices_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::DataIndices& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::dataIndices()
{
	return dataIndices_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::operator=(Cluster&& cluster) noexcept
{
	if (this != &cluster)
	{
		mean_ = std::move(cluster.mean_);
		dataIndices_ = std::move(cluster.dataIndices_);
		owner_ = cluster.owner_;

		cluster.owner_ = nullptr;
	}

	return *this;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline bool ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::operator<(const Cluster& cluster) const
{
	return dataIndices_.size() < cluster.dataIndices_.size();
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Cluster::updateMean()
{
	if (dataIndices_.empty())
	{
		for (size_t d = 0; d < tDimension; ++d)
			mean_[d] = T();

		return;
	}

	StaticBuffer<TSum, tDimension> sumObservation(tDimension, TSum());

	ocean_assert(owner_);
	const Data& data = owner_->data_;

	for (typename DataIndices::const_iterator i = dataIndices_.begin(); i != dataIndices_.end(); ++i)
	{
		ocean_assert(*i < data.numberObservations());

		for (size_t d = 0; d < tDimension; ++d)
		{
			sumObservation[d] += data[*i][d];
		}
	}

	const TSum count = TSum(dataIndices_.size());

	for (size_t d = 0; d < tDimension; ++d)
	{
		mean_[d] = T(sumObservation[d] / count);
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::ClusteringKMeans()
{
	static_assert(tDimension != 0, "Invalid observation dimension!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::ClusteringKMeans(ClusteringKMeans&& clustering) noexcept :
	data_(std::move(clustering.data_)),
	clusters_(std::move(clustering.clusters_))
{
	static_assert(tDimension != 0, "Invalid observation dimension!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::ClusteringKMeans(const Data& data) :
	data_(data)
{
	static_assert(tDimension != 0, "Invalid observation dimension!");

	ocean_assert(data_ && "The data element is invalid!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::ClusteringKMeans(Data&& data) :
	data_(std::move(data))
{
	static_assert(tDimension != 0, "Invalid observation dimension!");

	ocean_assert(data_ && "The data element is invalid!");
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::determineClustersByNumber(const size_t numberClusters, const InitializationStrategy strategy, const size_t iterations, Worker* worker)
{
	ocean_assert(clusters_.empty());

	if (strategy == IS_LARGEST_DISTANCE)
	{
		determineInitialClustersLargestDistance(numberClusters);
	}
	else
	{
		ocean_assert(strategy == IS_RANDOM);
		determineInitialClustersRandom(numberClusters);
	}

	ocean_assert(iterations >= 1);
	applyOptimizationIteration(worker);

	for (size_t i = 0; i < iterations; i++)
	{
		applyOptimizationIteration(worker);
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::determineClustersByDistance(const TSquareDistance maximalSqrDistance, size_t maximalClusters, size_t iterations, Worker* worker)
{
	ocean_assert(data_);
	ocean_assert(clusters_.empty());

	// find the smallest observation (euclidean distance to the origin)
	const size_t firstDataIndex = smallestObservation(data_);
	ocean_assert(firstDataIndex != size_t(-1));

	clusters_.push_back(Cluster(*this, data_[firstDataIndex], std::move(createIndices<size_t>(data_.numberObservations(), 0))));

	while (maximalClusters == 0 || clusters_.size() < maximalClusters)
	{
		if (!addCluster(iterations, maximalSqrDistance, worker))
		{
			break;
		}
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
bool ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::addCluster(const size_t iterations, TSquareDistance sqrDistance, Worker* worker)
{
	size_t maximalIndex = size_t(-1);
	TSquareDistance maximalDistance = TSquareDistance(0);

	for (size_t c = 0; c < clusters_.size(); ++c)
	{
		size_t localIndex = size_t(-1);
		const TSquareDistance localDistance = clusters_[c].maximalSqrDistance(&localIndex);

		if (localDistance > maximalDistance)
		{
			maximalDistance = localDistance;
			maximalIndex = localIndex;
		}
	}

	if (maximalIndex != size_t(-1) && maximalDistance >= sqrDistance)
	{
		clusters_.push_back(Cluster(*this, data_[maximalIndex]));

		ocean_assert(iterations >= 1);
		applyOptimizationIteration(worker);

		for (size_t n = 1; n < iterations; ++n)
		{
			applyOptimizationIteration(worker);
		}

		return true;
	}

	return false;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::removeCluster(const size_t iterations, Worker* worker)
{
	ocean_assert(!clusters_.empty());

	if (clusters_.size() <= 1)
	{
		clusters_.clear();
	}
	else
	{
		size_t minimalCluster = 0;
		TSquareDistance minimalDistance = NumericT<TSquareDistance>::maxValue();

		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			const TSquareDistance localDistance = clusters_[c].maximalSqrDistance();

			if (localDistance < minimalDistance)
			{
				minimalDistance = localDistance;
				minimalCluster = c;
			}
		}

		Clusters tmpClusters(std::move(clusters_));
		ocean_assert(clusters_.empty());

		ocean_assert(tmpClusters.size() >= 1);
		clusters_.reserve(tmpClusters.size() - 1);

		for (size_t c = 0; c < tmpClusters.size(); ++c)
		{
			if (c != minimalCluster)
			{
				clusters_.push_back(std::move(tmpClusters[c]));
			}
		}

		ocean_assert(!clusters_.empty());

		ocean_assert(iterations >= 1);
		applyOptimizationIteration(worker);

		for (size_t n = 1; n < iterations; ++n)
		{
			applyOptimizationIteration(worker);
		}
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
size_t ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::findCluster(const Observation& observation)
{
	TSquareDistance minimalDistance = NumericT<TSquareDistance>::maxValue();
	size_t minimalIndex = size_t(-1);

	for (size_t n = 0; n < clusters_.size(); ++n)
	{
		const TSquareDistance localDistance = clusters_[n].sqrDistance(observation);

		if (localDistance < minimalDistance)
		{
			minimalDistance = localDistance;
			minimalIndex = n;
		}
	}

	return minimalIndex;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline const typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::Clusters& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::clusters() const
{
	return clusters_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::sortClusters()
{
	std::sort(clusters_.rbegin(), clusters_.rend());
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
TSquareDistance ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::maximalSqrDistance() const
{
	TSquareDistance maximalDistance = TSquareDistance(0);

	for (typename Clusters::const_iterator i = clusters_.begin(); i != clusters_.end(); ++i)
	{
		maximalDistance = max(maximalDistance, i->maximalSqrDistance());
	}

	return maximalDistance;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::determineInitialClustersLargestDistance(const size_t numberClusters)
{
	ocean_assert(data_.numberObservations() != 0);
	ocean_assert(clusters_.empty());

	// find the smallest observation (euclidean distance to the origin)
	const DataIndex firstDataIndex = smallestObservation(data_);
	ocean_assert(firstDataIndex != DataIndex(-1));

	clusters_.push_back(Cluster(*this, data_[firstDataIndex]));

	while (clusters_.size() < numberClusters)
	{
		TSquareDistance largestDistance = TSquareDistance(0);
		size_t largestIndex = size_t(-1);

		for (size_t o = 0; o < data_.numberObservations(); ++o)
		{
			const Observation& observation = data_[o];

			TSquareDistance localDistance = NumericT<TSquareDistance>::maxValue();

			for (size_t c = 0; c < clusters_.size(); ++c)
			{
				localDistance = min(localDistance, clusters_[c].sqrDistance(observation));
			}

			if (localDistance > largestDistance)
			{
				largestDistance = localDistance;
				largestIndex = o;
			}
		}

		// check whether no observation is left
		if (largestIndex == size_t(-1))
		{
			break;
		}

		ocean_assert(largestDistance != TSquareDistance(0));

#ifdef OCEAN_DEBUG
		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			ocean_assert(clusters_[c].mean() != data_[largestIndex]);
		}
#endif

		clusters_.push_back(Cluster(*this, data_[largestIndex]));
	}

	ocean_assert(!clusters_.empty());

	for (size_t c = 0; c < clusters_.size(); ++c)
	{
		ocean_assert(clusters_[c].dataIndices().empty());
		clusters_[c].dataIndices().reserve(data_.numberObservations() * 2 / clusters_.size());
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::determineInitialClustersRandom(const size_t numberClusters)
{
	ocean_assert(data_.numberObservations() != 0);
	ocean_assert(clusters_.empty());

	const bool random64 = data_.numberObservations() > NumericT<unsigned int>::maxValue();

	// find the smallest observation (euclidean distance to the origin)
	const size_t firstDataIndex = smallestObservation(data_);
	ocean_assert(firstDataIndex != size_t(-1));

	clusters_.push_back(Cluster(*this, data_[firstDataIndex]));

	size_t iterations = 0;
	while (clusters_.size() < numberClusters && iterations++ < numberClusters * 100)
	{
		TSquareDistance largestDistance = 0u;
		size_t largestIndex = size_t(-1);

		for (size_t n = 0; n < max<size_t>(1, data_.numberObservations() / 128); ++n)
		{
			const size_t index = random64 ? RandomI::random64() % data_.numberObservations() : RandomI::random32() % (unsigned int)data_.numberObservations();
			const Observation& candidate = data_[index];

			TSquareDistance smallestDistance = NumericT<TSquareDistance>::maxValue();

			for (size_t c = 0; c < clusters_.size(); ++c)
			{
				smallestDistance = min(smallestDistance, clusters_[c].sqrDistance(candidate));
			}

			if (smallestDistance > largestDistance)
			{
				largestDistance = smallestDistance;
				largestIndex = index;
			}
		}

		// check whether no observation is left
		if (largestIndex == size_t(-1))
		{
			break;
		}

		ocean_assert(largestDistance != TSquareDistance(0));

#ifdef OCEAN_DEBUG
		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			ocean_assert(clusters_[c].mean() != data_[largestIndex]);
		}
#endif

		clusters_.push_back(Cluster(*this, data_[largestIndex]));
	}

	ocean_assert(!clusters_.empty());

	for (size_t c = 0; c < clusters_.size(); ++c)
	{
		ocean_assert(clusters_[c].dataIndices().empty());
		clusters_[c].dataIndices().reserve(data_.numberObservations() * 2 / clusters_.size());
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::applyOptimizationIteration()
{
	ocean_assert(!clusters_.empty());

	// remove the old indices, we determine the new distribution
	for (size_t c = 0; c < clusters_.size(); ++c)
	{
		clusters_[c].dataIndices().clear();
	}

	// assign each observation to the best fitting cluster
	for (size_t o = 0; o < data_.numberObservations(); ++o)
	{
		const Observation& observation = data_[o];

		TSquareDistance bestDistance = NumericT<TSquareDistance>::maxValue();
		size_t bestCluster = size_t(-1);

		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			const TSquareDistance localDistance = clusters_[c].sqrDistance(observation);

			if (localDistance < bestDistance)
			{
				bestDistance = localDistance;
				bestCluster = c;
			}
		}

		ocean_assert(bestCluster != size_t(-1));

		clusters_[bestCluster].dataIndices().push_back(o);
	}

	// update the mean values for each cluster
	for (size_t c = 0; c < clusters_.size(); ++c)
	{
		clusters_[c].updateMean();
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::applyOptimizationIteration(Worker* worker)
{
	if (worker == nullptr)
	{
		applyOptimizationIteration();
	}
	else
	{
		ocean_assert(!clusters_.empty());

		// remove the old indices, we determine the new distribution
		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			clusters_[c].dataIndices().clear();
		}

		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::applyOptimizationIterationSubset, &lock, 0u, 0u), 0u, (unsigned int)data_.numberObservations(), 1u, 2u);

		// update the mean values for each cluster
		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			clusters_[c].updateMean();
		}
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::applyOptimizationIterationSubset(Lock* lock, const unsigned int firstObservation, const unsigned int numberObservations)
{
	ocean_assert(!clusters_.empty());

	std::vector<DataIndices> localClusters(clusters_.size());

	// assign each observation to the best fitting cluster
	for (size_t o = firstObservation; o < firstObservation + numberObservations; ++o)
	{
		const Observation& observation = data_[o];

		TSquareDistance bestDistance = NumericT<TSquareDistance>::maxValue();
		size_t bestCluster = size_t(-1);

		for (size_t c = 0; c < clusters_.size(); ++c)
		{
			const TSquareDistance localDistance = clusters_[c].sqrDistance(observation);

			if (localDistance < bestDistance)
			{
				bestDistance = localDistance;
				bestCluster = c;
			}
		}

		ocean_assert(bestCluster != size_t(-1));

		localClusters[bestCluster].push_back(o);
	}

	const OptionalScopedLock scopedLock(lock);

	for (size_t c = 0; c < localClusters.size(); ++c)
	{
		clusters_[c].dataIndices().insert(clusters_[c].dataIndices().end(), localClusters[c].begin(), localClusters[c].end());
	}
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
void ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::clear()
{
	clusters_.clear();
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline bool ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::isValid() const
{
	return data_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::operator bool() const
{
	return data_;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>& ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::operator=(ClusteringKMeans&& clustering)
{
	if (this != &clustering)
	{
		data_ = std::move(clustering.data_);
		clusters_ = std::move(clustering.clusters_);
	}

	return *this;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline typename ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::DataIndex ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::smallestObservation(const Data& data)
{
	ocean_assert(data);

	TSquareDistance smallestDistance = NumericT<TSquareDistance>::maxValue();
	DataIndex smallestIndex = size_t(-1);

	for (size_t o = 0; o < data.numberObservations(); ++o)
	{
		const TSquareDistance localDistance = sqrDistance(data[o]);

		if (localDistance < smallestDistance)
		{
			smallestDistance = localDistance;
			smallestIndex = o;
		}
	}

	return smallestIndex;
}

template <typename T, size_t tDimension, typename TSum, typename TSquareDistance, bool tUseIndices>
inline TSquareDistance ClusteringKMeans<T, tDimension, TSum, TSquareDistance, tUseIndices>::sqrDistance(const Observation& observation)
{
	TSquareDistance result = TSquareDistance(0);

	for (size_t n = 0; n < tDimension; ++n)
	{
		result += observation[n] * observation[n];
	}

	return result;
}


}

#endif // META_OCEAN_MATH_CLUSTERING_K_MEANS_H
