/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_DATABASE_H
#define META_OCEAN_TRACKING_DATABASE_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Accessor.h"
#include "ocean/base/ShiftVector.h"
#include "ocean/base/Subset.h"
#include "ocean/base/Worker.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements a database for 3D object points, 2D image points and 6DOF camera poses.
 * Any 2D image point is located in a camera frame, while any camera frame has an own camera pose.<br>
 * Corresponding image points in consecutive camera frames can belong to the same 3D object point.<br>
 * This database stores ids for image points, object points, priority values of object points, camera poses and field of views of camera poses.<br>
 * Further, the topology between the individual database elements can be defined.<br>
 * The locations of the image points (2D positions within the camera frames) must be valid always.<br>
 * The locations of the object points or the transformation values of the camera poses may be invalid as in this case the location of the transformation has not been determined yet.<br>
 *
 * An id of an image point has the following connections:
 * <pre>
 * image point id  ->  2D point location (always valid)
 *                 ->  camera pose id (the id of the camera pose in which frame the image point is located)
 *                 ->  3D object point id (the id of the object point which projects to the image point)
 * </pre>
 *
 * Due to performance issues object points and camera poses store mappings to their corresponding image points.<br>
 * An id of an object point has the following connections:
 * <pre>
 * object point id  ->  3D point location (may be invalid)
 *                  ->  Priority value
 *                  ->  image point ids (the ids of all image points which are projections of the object points)
 * </pre>
 * An id of a camera pose has the following connections:
 * <pre>
 * camera pose id  ->  6DOF pose (may be invalid)
 *                 ->  Field of View value
 *                 ->  image point ids (the ids of all image points visible in the camera frame belonging to the camera pose)
 * </pre>
 *
 * Finally, the database stores a mapping between a pair of pose ids and object points ids and image points ids:
 * <pre>
 * pose id, object point id  ->  image point id
 * </pre>
 *
 * The internal data structure of this database allows arbitrary element access with almost O(log n).<br>
 * Due to the performance issue connections between the individual objects are necessary.<br>
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT Database
{
	public:

		/**
		 * Definition of an invalid id.
		 */
		const static Index32 invalidId = Index32(-1);

		/**
		 * Returns an invalid object point.
		 * @return Invalid object point
		 */
		static inline Vector3 invalidObjectPoint();

		/**
		 * Definition of a map mapping ids to 2D image point object.
		 */
		typedef std::map<Index32, Vector2> IdPointMap;

		/**
		 * Definition of a pair of ids and 2D image points.
		 */
		typedef std::pair<Index32, Vector2> IdPointPair;

		/**
		 * Definition of a vector holding pairs of ids and 2D image points.
		 */
		typedef std::vector<IdPointPair> IdPointPairs;

		/**
		 * Definition of a map mapping ids to 2D image point id pairs.
		 */
		typedef std::map<Index32, IdPointPairs> IdIdPointPairsMap;

		/**
		 * Definition of a map mapping ids to 2D vectors.
		 */
		typedef std::map<Index32, Vectors2> ImagePointsMap;

		/**
		 * Definition of a vector holding 2D vectors.
		 */
		typedef std::vector<Vectors2> ImagePointGroups;

		/**
		 * This class implements an object storing an id of an image point.
		 */
		class ImagePointObject
		{
			public:

				/**
				 * Creates a new object.
				 * @param imagePointId The id of the image point, may be invalid
				 */
				explicit inline ImagePointObject(const Index32 imagePointId = invalidId);

				/**
				 * Returns the id of the image point of this object.
				 * @return The image point id, may be invalid
				 */
				inline Index32 imagePointId() const;

				/**
				 * Sets or changes the id of the image point of this object.
				 * @param imagePointId The image point id to be set, may be invalid
				 */
				inline void setImagePointId(const Index32 imagePointId);

			protected:

				/// The image point id of this object.
				Index32 objectImagePointId;
		};

		/**
		 * This class implements an object storing an id of an object point.
		 */
		class ObjectPointObject
		{
			public:

				/**
				 * Creates a new object.
				 * @param objectPointId The id of the object point, may be invalid
				 */
				explicit inline ObjectPointObject(const Index32 objectPointId = invalidId);

				/**
				 * Returns the id of the object point of this object.
				 * @return The object point id, may be invalid
				 */
				inline Index32 objectPointId() const;

				/**
				 * Sets or changes the id of the object point of this object.
				 * @param objectPointId The object point id to be set, may be invalid
				 */
				inline void setObjectPointId(const Index32 objectPointId);

			protected:

				/// The object point id of this object.
				Index32 objectObjectPointId;
		};

		/**
		 * This class implements an object storing an id of an pose object.
		 */
		class PoseObject
		{
			public:

				/**
				 * Creates a new object.
				 * @param poseId The id of the pose, may be invalid
				 */
				explicit inline PoseObject(const Index32 poseId = invalidId);

				/**
				 * Returns the id of the camera pose of this object.
				 * @return The camera pose id, may be invalid
				 */
				inline Index32 poseId() const;

				/**
				 * Sets or changes the id of the camera pose of this object.
				 * @param poseId The camera pose id to set, may be invalid
				 */
				inline void setPoseId(const Index32 poseId);

			protected:

				/// The camera pose id of this object.
				Index32 objectPoseId;
		};

		/**
		 * This class defines the topology between a camera pose id, an object point id and an image point id.
		 * An image point can be visible in only one camera frame, while the camera frame belongs to only one camera pose.<br>
		 * The image point can be a projection of only one object point if a valid camera pose is known.<br>
		 */
		class TopologyTriple :
			public PoseObject,
			public ObjectPointObject,
			public ImagePointObject
		{
			public:

				/**
				 * Creates a new topology object.
				 * @param poseId The id of the camera pose of the new object
				 * @param objectPointId The id of the object point of the new object
				 * @param imagePointId The id of the image point of the new object
				 */
				explicit inline TopologyTriple(const Index32 poseId = invalidId, const Index32 objectPointId = invalidId, const Index32 imagePointId = invalidId);
		};

		/**
		 * This class stores a pair of pose id and image point id.
		 */
		class PoseImagePointPair :
			public ImagePointObject,
			public PoseObject
		{
			public:

				/**
				 * Creates a new pair object.
				 * @param poseId The id of the pose, may be invalid
				 * @param imagePointId The id of the image point, may be invalid
				 */
				explicit inline PoseImagePointPair(const Index32 poseId = invalidId, const Index32 imagePointId = invalidId);
		};

		/**
		 * Definition of a vector holding several pairs of pose and image point ids.
		 */
		typedef std::vector<PoseImagePointPair> PoseImagePointTopology;

		/**
		 * Definition of a vector holding several groups of pairs of pose and image point ids.
		 */
		typedef std::vector< std::pair<Index32, PoseImagePointTopology> > PoseImagePointTopologyGroups;

		/**
		 * Definition of a vector holding object of topology triple.
		 */
		typedef std::vector<TopologyTriple> TopologyTriples;

		/**
		 * This class implements an accessor object for image points based on a set of image point ids.
		 * @tparam tThreadSafe True, to call the thread-safe functions of the database
		 */
		template <bool tThreadSafe>
		class ConstImagePointAccessorIds : public ConstIndexedAccessor<Vector2>
		{
			public:

				/**
				 * Creates a new accessor object by providing the references of the database and the image point ids.
				 * Beware: Neither the database nor the image point ids are copied; thus the given references must be valid as long as this accessor object exists.<br>
				 * @param database The database object holding the image points
				 * @param imagePointIds The image point ids of all image points that can be accessed through this object
				 */
				inline ConstImagePointAccessorIds(const Database& database, const Indices32& imagePointIds);

				/**
				 * Returns the number of image points of this accessor.
				 * @return The number of image points
				 */
				virtual size_t size() const;

				/**
				 * Returns a specific image point identified by the index within from the specified image point ids.
				 * @param index The index within the given image point ids, with range [0, size())
				 * @return The reference to the image point
				 */
				virtual const Vector2& operator[](const size_t& index) const;

			protected:

				/// The reference to the database holding the individual image points.
				const Database& accessorDatabase;

				/// The reference to the image point ids.
				const Indices32& accessorImagePointIds;
		};

		/**
		 * This class implements an accessor object for image points based on a topology between poses and image points.
		 * @tparam tThreadSafe True, to call the thread-safe functions of the database
		 */
		template <bool tThreadSafe>
		class ConstImagePointAccessorTopology : public ConstIndexedAccessor<Vector2>
		{
			public:

				/**
				 * Creates a new accessor object by providing the references of the database and the topology.
				 * Beware: Neither the database nor the image point ids are copied; thus the given references must be valid as long as this accessor object exists.<br>
				 * @param database The database object holding the image points
				 * @param topology The topology providing access to the individual image points
				 */
				inline ConstImagePointAccessorTopology(const Database& database, const PoseImagePointTopology& topology);

				/**
				 * Returns the number of image points of this accessor.
				 * @return The number of image points
				 */
				virtual size_t size() const;

				/**
				 * Returns a specific image point identified by the index within from the specified topology.
				 * @param index The index within the given topology, with range [0, size())
				 * @return The reference to the image point
				 */
				virtual const Vector2& operator[](const size_t& index) const;

			protected:

				/// The reference to the database holding the individual image points.
				const Database& accessorDatabase;

				/// The topology between poses and image points.
				const PoseImagePointTopology& accessorTopology;
		};

		/**
		 * This class implements an accessor object for object points based on a set of object point ids.
		 * @tparam tThreadSafe True, to call the thread-safe functions of the database
		 */
		template <bool tThreadSafe>
		class ConstObjectPointAccessorIds : public ConstIndexedAccessor<Vector3>
		{
			public:

				/**
				 * Creates a new accessor object by providing the references of the database and the object point ids.
				 * Beware: Neither the database nor the object point ids are copied; thus the given references must be valid as long as this accessor object exists.<br>
				 * @param database The database object holding the object points
				 * @param objectPointIds The object point ids of all object points that can be accessed through this object
				 */
				inline ConstObjectPointAccessorIds(const Database& database, const Indices32& objectPointIds);

				/**
				 * Returns the number of object points of this accessor.
				 * @return The number of object points
				 */
				virtual size_t size() const;

				/**
				 * Returns a specific object point identified by the index within from the specified object point ids.
				 * @param index The index within the given object point ids, with range [0, size())
				 * @return The reference to the object point
				 */
				virtual const Vector3& operator[](const size_t& index) const;

			protected:

				/// The reference to the database holding the individual object points.
				const Database& accessorDatabase;

				/// The reference to the object point ids.
				const Indices32& accessorObjectPointIds;
		};

		/**
		 * This class implements an accessor object for poses based on a set of pose ids.
		 * @tparam tThreadSafe True, to call the thread-safe functions of the database
		 */
		template <bool tThreadSafe>
		class ConstPoseAccessorIds : public ConstIndexedAccessor<HomogenousMatrix4>
		{
			public:

				/**
				 * Creates a new accessor object by providing the references of the database and the pose ids.
				 * Beware: Neither the database nor the pose ids are copied; thus the given references must be valid as long as this accessor object exists.<br>
				 * @param database The database object holding the object points
				 * @param poseIds The pose ids of all poses that can be accessed through this object
				 */
				inline ConstPoseAccessorIds(const Database& database, const Indices32& poseIds);

				/**
				 * Returns the number of poses of this accessor.
				 * @return The number of poses
				 */
				virtual size_t size() const;

				/**
				 * Returns a specific pose identified by the index within from the specified pose ids.
				 * @param index The index within the given pose ids, with range [0, size())
				 * @return The reference to the pose
				 */
				virtual const HomogenousMatrix4& operator[](const size_t& index) const;

			protected:

				/// The reference to the database holding the individual object points.
				const Database& accessorDatabase;

				/// The reference to the pose ids.
				const Indices32& accessorPoseIds;
		};

		/**
		 * This class implements an accessor object for poses based on a topology between poses and image points.
		 * @tparam tThreadSafe True, to call the thread-safe functions of the database
		 */
		template <bool tThreadSafe>
		class ConstPoseAccessorTopology : public ConstIndexedAccessor<HomogenousMatrix4>
		{
			public:

				/**
				 * Creates a new accessor object by providing the references of the database and the topology.
				 * Beware: Neither the database nor the image point ids are copied; thus the given references must be valid as long as this accessor object exists.<br>
				 * @param database The database object holding the image points
				 * @param topology The topology providing access to the individual poses
				 */
				inline ConstPoseAccessorTopology(const Database& database, const PoseImagePointTopology& topology);

				/**
				 * Returns the number of poses of this accessor.
				 * @return The number of poses
				 */
				virtual size_t size() const;

				/**
				 * Returns a specific pose identified by the index within from the specified topology.
				 * @param index The index within the given topology, with range [0, size())
				 * @return The reference to the pose
				 */
				virtual const HomogenousMatrix4& operator[](const size_t& index) const;

			protected:

				/// The reference to the database holding the individual image points.
				const Database& accessorDatabase;

				/// The topology between poses and image points.
				const PoseImagePointTopology& accessorTopology;
		};

	protected:

		/**
		 * This class implements a data object storing the information connected with an id of an image point.
		 */
		class ImagePointData
		{
			public:

				/**
				 * Creates a default object.
				 */
				inline ImagePointData();

				/**
				 * Creates a new image point object.
				 * @param point The 2D location of the new object
				 * @param poseId The id of the pose which belongs to the new object
				 * @param objectPointId The id of the object point which belongs to the new object
				 */
				inline ImagePointData(const Vector2& point, const Index32 poseId = invalidId, const Index32 objectPointId = invalidId);

				/**
				 * Returns the 2D location of the image point of this object.
				 * @return The 2D image point
				 */
				inline const Vector2& point() const;

				/**
				 * Returns the id of the pose which belongs to the image point of this object.
				 * @return The camera pose id, an invalid id if no pose has been registered
				 */
				inline Index32 poseId() const;

				/**
				 * Returns the ids of the 3D object point which belongs to the image point of this object.
				 * @return The object point id, an invalid id if no object point has been registered
				 */
				inline Index32 objectPointId() const;

				/**
				 * Sets the location of the image point of this object.
				 * @param point The 2D location of the image point
				 */
				inline void setPoint(const Vector2& point);

				/**
				 * Sets the id of the pose belonging to this image point object.
				 * @param poseId The pose id to be set
				 */
				inline void setPoseId(const Index32 poseId);

				/**
				 * Sets the id of the object point belonging to this image point object.
				 * @param objectPointId The object point id to be set
				 */
				inline void setObjectPointId(const Index32 objectPointId);

			protected:

				/// The location of the 2D image point of this object.
				Vector2 dataPoint;

				/// The id of the pose which belongs to this object.
				Index32 dataPoseId;

				/// The id of the object point which belongs to this object.
				Index32 dataObjectPointId;
		};

		/**
		 * The base class for all data object storing a set of image point ids.
		 */
		class Data
		{
			public:

				/**
				 * Returns the image point ids of this object.
				 * @return Image point ids
				 */
				inline const IndexSet32& imagePointIds() const;

				/**
				 * Registers (adds) a new image point id at this data object.
				 * @param imagePointId The new id to be registered, must be valid and must not already be part of this data object
				 */
				inline void registerImagePoint(const Index32 imagePointId);

				/**
				 * Unregisters (removes) an image point id from this data object.
				 * @param imagePointId The id to be unregistered, must be valid and must be part of this data object
				 */
				inline void unregisterImagePoint(const Index32 imagePointId);

			protected:

				/// The set of registered image point ids of this object.
				IndexSet32 dataImagePointIds;
		};

		/**
		 * The data object encapsulating a 6DOF camera pose.
		 */
		class PoseData : public Data
		{
			public:

				/**
				 * Creates a new object with specified pose.
				 * @param world_T_camera The pose of this object, may be invalid if e.g., unknown at this moment
				 * @param fov The fov value of this object, may be invalid if e.g., unknown at this moment
				 */
				explicit inline PoseData(const HomogenousMatrix4& world_T_camera = HomogenousMatrix4(false), const Scalar fov = -1);

				/**
				 * Returns the pose of this object.
				 * @return The object's pose, may be invalid
				 */
				inline const HomogenousMatrix4& pose() const;

				/**
				 * Returns the field of view value of this object.
				 * @return The fov value, may be invalid
				 */
				inline Scalar fov() const;

				/**
				 * Sets (changes) the pose of this object.
				 * @param world_T_camera The pose to be set, may be invalid
				 */
				inline void setPose(const HomogenousMatrix4& world_T_camera);

				/**
				 * Sets (changes) the field of view value of this object.
				 * @param fov The fov value to be set, may be invalid
				 */
				inline void setFov(const Scalar fov);

			protected:

				/// The pose of this object.
				HomogenousMatrix4 world_T_camera_;

				/// The field of view value of this object.
				Scalar dataFov;
		};

		/**
		 * The data object encapsulating a 3D object point.
		 */
		class ObjectPointData : public Data
		{
			public:

				/**
				 * Creates an object with invalid object point.
				 * @param point The 3D object point of this object, may be invalid if e.g., unknown at this moment
				 * @param priority The priority value of this object, may be invalid if e.g., unknown at this moment
				 */
				explicit inline ObjectPointData(const Vector3& point = invalidObjectPoint(), const Scalar priority = -1);

				/**
				 * Returns the 3D object point of this object.
				 * @return The object point, may be invalid
				 */
				inline const Vector3& point() const;

				/**
				 * Returns the priority value of this object.
				 * @return The priority value, may be invalid
				 */
				inline Scalar priority() const;

				/**
				 * Sets (changes) the 3D object point of this object.
				 * @param point The 3D object point to be set, may be invalid
				 */
				inline void setPoint(const Vector3& point);

				/**
				 * Sets (changes) the priority value of this object.
				 * @param priority The priority value to be set, may be invalid
				 */
				inline void setPriority(const Scalar priority);

			protected:

				/// The 3D object point of this object.
				Vector3 dataPoint;

				/// The priority value of this object.
				Scalar dataPriority;
		};

		/**
		 * Definition of an (ordered) map mapping pose ids to pose data objects, we use an ordered map as poses have an order.
		 */
		typedef std::map<Index32, PoseData> PoseMap;

		/**
		 * Definition of an (unordered) map mapping object point ids to object point data objects.
		 */
		typedef std::unordered_map<Index32, ObjectPointData> ObjectPointMap;

		/**
		 * Definition of an (unordered) map mapping image point ids to image point data objects.
		 */
		typedef std::unordered_map<Index32, ImagePointData> ImagePointMap;

		/**
		 * Definition of an (unordered) map mapping 32 bit ids to 32 bit ids.
		 */
		typedef std::unordered_map<Index32, Index32> Index32To32Map;

		/**
		 * Definition of an (unordered) map mapping 64 bit ids to 32 bit ids.
		 */
		typedef std::unordered_map<Index64, Index32> Index64To32Map;

	public:

		/**
		 * Creates a new empty database object.
		 */
		inline Database();

		/**
		 * Copy constructor.
		 * @param database The database object to be copied
		 */
		inline Database(const Database& database);

		/**
		 * Move constructor.
		 * @param database The database object to be moved
		 */
		inline Database(Database&& database) noexcept;

		/**
		 * Returns a reference to the lock object of this database object.
		 * @return The lock object's reference
		 */
		inline Lock& lock();

		/**
		 * Returns whether this database holds at least one image point, one object point or one camera pose.
		 * @return True, if so
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool isEmpty() const;

		/**
		 * Returns the number of poses of this database.
		 * @return The database's pose number
		 */
		template <bool tThreadSafe>
		inline size_t poseNumber() const;

		/**
		 * Returns the number of object point ids in this database.
		 * @return The database's object point id number
		 */
		template <bool tThreadSafe>
		inline size_t objectPointNumber() const;

		/**
		 * Returns the number of image point ids in this database.
		 * @return The database's image point id number
		 */
		template <bool tThreadSafe>
		inline size_t imagePointNumber() const;

		/**
		 * Returns the location of an image point which is specified by the id of the image point.
		 * Beware: The requested image point must exist in this database.<br>
		 * @param imagePointId The unique id of the image point, must be valid
		 * @return The location of the specified image point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline const Vector2& imagePoint(const Index32 imagePointId) const;

		/**
		 * Returns the positions of 2D image points specified by the ids of the image points.
		 * @param imagePointIds The ids of the image points for which the positions will be returned, must be valid
		 * @return The resulting image point positions, one position for each id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Vectors2 imagePoints(const Indices32& imagePointIds) const;

		/**
		 * Returns the positions of 2D image points specified by the ids of the image points.
		 * @param imagePointIds The ids of the image points for which the positions will be returned, must be valid
		 * @return The resulting image point positions, one position for each id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Vectors2 imagePoints(const IndexSet32& imagePointIds) const;

		/**
		 * Returns whether an object point is visible in a specified frame, and optional the location and id of the corresponding image point.
		 * @param poseId The id of the camera pose for which the visibility of the object point is checked, must be valid
		 * @param objectPointId The unique id of the object point, must be valid
		 * @param point Optional resulting location of the image point, if any
		 * @param pointId Optional resulting unique id of the image point, if any
		 * @return True, if the defined object point has a visible image point in the specified camera pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		bool hasObservation(const Index32 poseId, const Index32 objectPointId, Vector2* point = nullptr, Index32* pointId = nullptr) const;

		/**
		 * Returns the location of an object point which is specified by the id of the object point.
		 * Beware: The requested object point must exist in this database.<br>
		 * @param objectPointId The unique id of the object point, must be valid
		 * @return The location of the specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline const Vector3& objectPoint(const Index32 objectPointId) const;

		/**
		 * Returns the location and priority of an object point which is specified by the id of the object point.
		 * Beware: The requested object point must exist in this database.<br>
		 * @param objectPointId The unique id of the object point, must be valid
		 * @param objectPointPriority The resulting priority of the specified object point
		 * @return The location of the specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline const Vector3& objectPoint(const Index32 objectPointId, Scalar& objectPointPriority) const;

		/**
		 * Returns the priority of an object point which is specified by the id of the object point.
		 * Beware: The requested object point must exist in this database.<br>
		 * @param objectPointId The unique id of the object point, must be valid
		 * @return The priority of the specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Scalar objectPointPriority(const Index32 objectPointId) const;

		/**
		 * Returns the positions of all 3D object points.
		 * @return The resulting object point positions
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Vectors3 objectPoints() const;

		/**
		 * Returns the positions of all 3D object points that match or that do not match the position of a specified reference object point and which have a specified minimal priority value.
		 * @return The resulting object point positions
		 * @param referencePosition An object point position which is used to filter the resulting object points
		 * @param objectPointIds Optional resulting ids of the resulting valid object points, on id for each object point
		 * @param minimalPriority The minimal priority value an object points must have to be returned
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined reference position will match the positions of the resulting object points; False, if the defined reference position will not match the positions of the resulting object points
		 * @see objectPointIds().
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		inline Vectors3 objectPoints(const Vector3& referencePosition, Indices32* objectPointIds = nullptr, const Scalar minimalPriority = Scalar(-1)) const;

		/**
		 * Returns the positions of 3D object points specified by the ids of the object points.
		 * @param objectPointIds The ids of the object points for which the positions will be returned, must be valid
		 * @return The resulting object point positions, one position for each id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Vectors3 objectPoints(const Indices32& objectPointIds) const;

		/**
		 * Returns the 6DOF pose of a camera frame which is specified by the id of the pose.
		 * Beware: The requested pose must exist in this database.<br>
		 * @param poseId The unique id of the pose, must be valid
		 * @return The specified pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see poses().
		 */
		template <bool tThreadSafe>
		inline const HomogenousMatrix4& pose(const Index32 poseId) const;

		/**
		 * Returns the 6DOF pose values for all specified pose ids.
		 * @param poseIds The ids of the poses for which the 6DOF values will be returned, each pose id must be valid and must exist
		 * @param size The number of given pose ids
		 * @return The resulting 6DOF pose values, one value for each id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see pose().
		 */
		template <bool tThreadSafe>
		inline HomogenousMatrices4 poses(const Index32* poseIds, const size_t size) const;

		/**
		 * Returns the 3DOF rotational part of the 6DOF pose values for all specified pose ids.
		 * The camera motion of this database must be pure rotational so that the position of each camera pose is in the origin of the coordinate system.<br>
		 * @param poseIds The ids of the poses for which the rotational values will be returned, each pose id must be valid and must exist
		 * @param size The number of given pose ids
		 * @return The resulting 3DOF orientations of the camera, one value for each id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see pose().
		 */
		template <bool tThreadSafe>
		inline SquareMatrices3 rotationalPoses(const Index32* poseIds, const size_t size) const;

		/**
		 * Returns all 6DOF poses which match a given reference pose or which do not match a given reference pose.
		 * @param referencePose A reference pose allowing to filter the resulting poses
		 * @param poseIds Optional resulting ids of the resulting valid poses, on id for each valid pose
		 * @return The resulting valid poses
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPose True, if the defined pose will match the values of the resulting poses; False, if the defined pose will not match the values of the resulting poses
		 * @see poseIds().
		 */
		template <bool tThreadSafe, bool tMatchPose>
		inline HomogenousMatrices4 poses(const HomogenousMatrix4& referencePose, Indices32* poseIds = nullptr) const;

		/**
		 * Returns all 6DOF poses (valid or invalid) lying between a specified range of pose ids.
		 * For unknown frame ids an invalid pose is provided.
		 * @param lowerPoseId The id (index) of the frame defining the lower border of camera poses which will be investigated
		 * @param upperPoseId The id (index) of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @return The poses of this database, the first pose corresponds to the 'lowerFrame' while the last pose corresponds to the 'upperFrame'
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline HomogenousMatrices4 poses(const Index32 lowerPoseId, const Index32 upperPoseId) const;

		/**
		 * Returns the ids of specific 6DOF poses.
		 * @param referencePose A pose allowing to filter the resulting pose ids
		 * @param poses Optional resulting poses, on pose for each id
		 * @return The resulting ids of all poses
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPose True, if the defined pose will match the values of the resulting poses; False, if the defined pose will not match the values of the resulting poses
		 * @see poses().
		 */
		template <bool tThreadSafe, bool tMatchPose>
		inline Indices32 poseIds(const HomogenousMatrix4& referencePose, HomogenousMatrices4* poses = nullptr) const;

		/**
		 * Returns the smallest id (the id of the lower frame border) and the largest id (the id of the upper frame border) of all poses that are known in this database.
		 * This function checks whether the pose (camera frame) is known only, thus the corresponding poses can either be valid or invalid.
		 * @param lowerPoseId Resulting id of the frame defining the lower border of the camera frames which are known
		 * @param upperPoseId Resulting id of the frame defining the upper border of the camera frames which are known
		 * @return True, if at least one pose is known
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see validPoseBorders(), validPoseRange().
		 */
		template <bool tThreadSafe>
		inline bool poseBorders(Index32& lowerPoseId, Index32& upperPoseId) const;

		/**
		 * Returns the smallest id (the id of the lower frame border) and the largest id (the id of the upper frame border) with a valid pose (from all known poses in this database.
		 * Beware: There may be frame ids with invalid poses in-between.
		 * @param rangeLowerPoseId Resulting id of the frame defining the lower border of the camera frames with valid pose
		 * @param rangeUpperPoseId Resulting id of the frame defining the upper border of the camera frames with valid pose
		 * @return True, if at least one valid pose is known
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see poseBorders().
		 */
		template <bool tThreadSafe>
		inline bool validPoseBorders(Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const;

		/**
		 * Determines the pose id range (around a specified start frame) for which the database holds valid poses.
		 * @param lowerPoseId The id of the frame defining the lower border of the camera poses which will be investigated, with range [0, infinity)
		 * @param startPoseId The id of the start frame / start pose, with range [lowerPoseId, upperPoseId]
		 * @param upperPoseId The id of the frame defining the upper border of the camera poses which will be investigated, with range [startPoseId, infinity)
		 * @param rangeLowerPoseId Resulting id of the first camera pose/camera frame with valid camera pose, with range [lowerPoseId, upperPoseId]
		 * @param rangeUpperPoseId Resulting id of the last camera pose/camera frame with valid camera pose, with range [rangeLowerPoseId, upperPoseId]
		 * @return True, if the resulting range holds at least one valid pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see largestValidPoseRange(), poseBorders().
		 */
		template <bool tThreadSafe>
		inline bool validPoseRange(const Index32 lowerPoseId, const Index32 startPoseId, const Index32 upperPoseId, Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const;

		/**
		 * Determines the largest pose id range for which the database holds valid poses.
		 * @param lowerPoseId The id of the frame defining the lower border of the camera poses which will be investigated, with range [0, infinity)
		 * @param upperPoseId The id of the frame defining the upper border of the camera poses which will be investigated, with range [lowerPoseId, infinity)
		 * @param rangeLowerPoseId Resulting id of the first camera pose/camera frame with valid camera pose, with range [lowerPoseId, upperPoseId]
		 * @param rangeUpperPoseId Resulting id of the last camera pose/camera frame with valid camera pose, with range [rangeLowerPoseId, upperPoseId]
		 * @return True, if the resulting range holds at least one valid pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see validPoseRange(), poseBorders().
		 */
		template <bool tThreadSafe>
		inline bool largestValidPoseRange(const Index32 lowerPoseId, const Index32 upperPoseId, Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const;

		/**
		 * Determines the pose id for which the database holds the most number of point correspondences (between e.g., valid or invalid object points and image points).
		 * @param lowerPoseId The id of the frame defining the lower border of the camera poses which will be investigated, with range [0, infinity)
		 * @param upperPoseId The id of the frame defining the upper border of the camera poses which will be investigated, with range [lowerPoseId, infinity)
		 * @param poseId Optional resulting id of the valid pose with most correspondences, with range [lowerPoseId, upperPoseId]
		 * @param correspondences Optional resulting number of correspondences for the resulting pose
		 * @param referenceObjectPoint A reference object point allowing to filter the correspondences to count
		 * @return True, if a pose with at least one correspondence could be found
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the correspondences; False, if the defined position will not match the positions of the correspondences
		 * @tparam tNeedValidPose True, if the pose must be valid so that the number of valid correspondences will be determined
		 * @see poseWithLeastCorrespondences().
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
		inline bool poseWithMostCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, Index32* poseId = nullptr, unsigned int* correspondences = nullptr, const Vector3& referenceObjectPoint = invalidObjectPoint()) const;

		/**
		 * Determines the pose id for which the database holds the least number of point correspondences (between e.g., valid or invalid object points and image points).
		 * @param lowerPoseId The id of the frame defining the lower border of the camera poses which will be investigated, with range [0, infinity)
		 * @param upperPoseId The id of the frame defining the upper border of the camera poses which will be investigated, with range [lowerPoseId, infinity)
		 * @param poseId Optional resulting id of the valid pose with least correspondences, with range [lowerPoseId, upperPoseId]
		 * @param correspondences Optional resulting number of correspondences for the resulting pose
		 * @param referenceObjectPoint A reference object point allowing to filter the correspondences to count
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the correspondences; False, if the defined position will not match the positions of the correspondences
		 * @tparam tNeedValidPose True, if the pose must be valid so that the number of valid correspondences will be determined
		 * @see poseWithMostCorrespondences().
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
		inline bool poseWithLeastCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, Index32* poseId = nullptr, unsigned int* correspondences = nullptr, const Vector3& referenceObjectPoint = invalidObjectPoint()) const;

		/**
		 * Determines the pose id from a set of given pose id candidates for which the database holds the most observations from a set of given object point ids.
		 * The major object point ids are the essential object point for which the most observations will be determined.<br>
		 * If more than one pose with the same number of most major object point observations can be determined the second set of object points (the minor object points) are used to identify the final pose with most observations.<br>
		 * @param poseCandidates The ids of all poses from which the best pose is determined
		 * @param majorObjectPointIds The ids of all major object points which are the essential object points for the resulting pose, at least one
		 * @param minorObjectPointIds The ids of all minor object points
		 * @param poseId The resulting id of the pose with most visible major object points (and minor object points, if more than two poses exist with same number of best visible major object points)
		 * @param visibleMajorObjectPointIds Optional resulting ids of all major object points visible in the resulting pose
		 * @param visibleMinorObjectPointIds Optional resulting ids of all minor object points visible in the resulting pose
		 * @return True, at least one pose exists in which at least one major object point is visible
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool poseWithMostObservations(const IndexSet32& poseCandidates, const IndexSet32& majorObjectPointIds, const IndexSet32& minorObjectPointIds, Index32& poseId, Indices32* visibleMajorObjectPointIds = nullptr, Indices32* visibleMinorObjectPointIds = nullptr) const;

		/**
		 * Counts the number of observations of a given set of object point ids for a specific camera frame.
		 * @param poseId The id of the pose for which the number of visible object points is determined
		 * @param objectPointIds the ids of the object point for which the number of observations is determined
		 * @return The number of object points (from the given set of object points) which are visible in the defined pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline unsigned int numberObservations(const Index32 poseId, const Indices32& objectPointIds) const;

		/**
		 * Counts the number of correspondences (e.g., valid or invalid) between image and object points for a specified pose.
		 * @param poseId The id of the pose for which the number of point correspondences is determined
		 * @param referenceObjectPoint A reference object point allowing to filter the correspondences to count
		 * @param minimalPriority The minimal priority value an object point must have so that is will be investigated
		 * @return The resulting number of correspondences
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the correspondences; False, if the defined position will not match the positions of the correspondences
		 * @tparam tNeedValidPose True, if the pose must be valid so that the number of valid correspondences will be determined, otherwise the number of correspondences will be zero
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
		inline unsigned int numberCorrespondences(const Index32 poseId, const Vector3& referenceObjectPoint, const Scalar minimalPriority = Scalar(-1)) const;

		/**
		 * Counts the number of valid correspondences between image and object points for several poses individually.
		 * @param lowerPoseId The id (index) of the frame defining the lower border of camera poses which will be investigated
		 * @param upperPoseId The id (index) of the frame defining the upper border of camera poses which will be investigated, with range [lowerFrame, infinity)
		 * @param referenceObjectPoint A reference object point allowing to filter the correspondences to count
		 * @param minimalPriority The minimal priority value an object point must have so that is will be investigated
		 * @param worker Optional worker to distribute the computation
		 * @return The number of correspondences for each pose in the range [lowerPoseId, upperPoseId] starting with 'lowerPoseId' (the first entry corresponds to 'lowerPoseId' and so on)
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the correspondences; False, if the defined position will not match the positions of the correspondences
		 * @tparam tNeedValidPose True, if the pose must be valid so that the number of valid correspondences will be determined, otherwise the number of correspondences will be zero
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
		inline Indices32 numberCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, const Vector3& referenceObjectPoint, const Scalar minimalPriority = Scalar(-1), Worker* worker = nullptr) const;

		/**
		 * Returns whether this database holds a specified image point.
		 * @param imagePointId The unique id of the image point which will be checked
		 * @param imagePoint Optional resulting image point value of the defined image point id
		 * @return True, if so
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool hasImagePoint(const Index32 imagePointId, Vector2* imagePoint = nullptr) const;

		/**
		 * Adds a new 2D image point to this database.
		 * @param imagePoint The image point to be added
		 * @return The unique id of the new image point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Index32 addImagePoint(const Vector2& imagePoint);

		/**
		 * Removes an image point from this database.
		 * Beware: The specified image point must exist in this database.<br>
		 * @param imagePointId The id of the image point which will be removed, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void removeImagePoint(const Index32 imagePointId);

		/**
		 * Returns whether this database holds a specified object point.
		 * @param objectPointId The unique id of the object point which will be checked
		 * @param objectPoint Optional resulting object point value of the defined object point id
		 * @return True, if so
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool hasObjectPoint(const Index32 objectPointId, Vector3* objectPoint = nullptr) const;

		/**
		 * Adds a new 3D object point to this database.
		 * This function uses the internal id counter for object points to create a new id.<br>
		 * Beware: Do not mix calls with the add-objectPoint-function not creating the id on its own.
		 * @param objectPoint The object point to be added
		 * @param priority The priority value of the object point
		 * @return The unique id of the new object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Index32 addObjectPoint(const Vector3& objectPoint, const Scalar priority = Scalar(-1));

		/**
		 * Adds a new 3D object point to this database.
		 * This function does not use the internal id counter for object points to create a new id.<br>
		 * Instead, this function takes an explicit object point id.<br>
		 * Beware: Do not mix calls with the add-objectPoint-function creating the id on its own.
		 * @param objectPointId The unique id of the new object point, must not exist already
		 * @param objectPoint The object point to be added
		 * @param priority The priority value of the object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see hasObjectPoint().
		 */
		template <bool tThreadSafe>
		inline void addObjectPoint(const Index32 objectPointId, const Vector3& objectPoint, const Scalar priority = Scalar(-1));

		/**
		 * Adds an object point from another database, adds all connected image points, registers unknown poses, and adds the topology.
		 * Thus, this function mainly merges a track from a second database to this database.
		 * Beware: This function is not thread-safe (as we need to prevent possible dead locks).
		 * @param secondDatabase The second database from which the track (the object point and all connected information) will be copied
		 * @param secondDatabaseObjectPointId The id of the object point in the second database to be copied
		 * @param imagePointTransformation A transformation which will be applied to each connected image point (from the second database) before the image point is added to this database, an identity transformation to keep the image points as they are, the transformation defines: thisDatabaseImagePoint = imagePointTransformation * secondDatabaseImagePoint, must not be singular
		 * @param newObjectPointId Optional explicit id of the new object point in this database, must not exist in this database if defined, an invalid id to generate a new id automatically
		 * @param secondDatabaseLowerPoseId Optional pose id defining the lower border of the pose range from which observations (image points) of the object point will be copied, an invalid id to copy all possible observations, with range [0, secondDatabaseUpperPoseId] or invalidId
		 * @param secondDatabaseUpperPoseId Optional pose id defining the upper border of the pose range from which observations (image points) of the object point will be copied, an invalid id to copy all possible observations, with range [secondDatabaseLowerPoseId, infinity) or invalidId
		 * @param forExistingPosesOnly True, to avoid the creation of new poses in this database (and to skip observations/image points); False, to create new poses in this database if not existing already
		 * @return The id of the new object point in this database, an invalid id if the track could not be copied
		 */
		inline Index32 addObjectPointFromDatabase(const Database& secondDatabase, const Index32 secondDatabaseObjectPointId, const SquareMatrix3& imagePointTransformation = SquareMatrix3(true), const Index32 newObjectPointId = invalidId, const Index32 secondDatabaseLowerPoseId = invalidId, const Index32 secondDatabaseUpperPoseId = invalidId, const bool forExistingPosesOnly = false);

		/**
		 * Removes an object point from this database.
		 * Beware: The specified object point must exist in this database.<br>
		 * @param objectPointId The id of the object point which will be removed, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void removeObjectPoint(const Index32 objectPointId);

		/**
		 * Removes an object point from this database and also removes all image points attached to the object point.
		 * @param objectPointId The id of the object point which will be removed, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		void removeObjectPointAndAttachedImagePoints(const Index32 objectPointId);

		/**
		 * Renames an object point, changes the id of the object point respectively.
		 * Beware: Do not mix calls with the add-objectPoint-function creating the id on its own.
		 * @param oldObjectPointId The old (the current) id of the object point to be changed, must be valid
		 * @param newObjectPointId The new id of the object point, must be valid, must not exist
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void renameObjectPoint(const Index32 oldObjectPointId, const Index32 newObjectPointId);

		/**
		 * Merges two object points together, afterwards one object point will be removed.
		 * Both object points must not be visible in the same camera pose.
		 * @param remainingObjectPointId The id of the object point which will remain after merging both object points, must be valid
		 * @param removingObjectPointId The id of the object point which will be removed after merging both object points, must be valid
		 * @param newPoint The location of the merged object point
		 * @param newPriority The priority of the merged object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void mergeObjectPoints(const Index32 remainingObjectPointId, const Index32 removingObjectPointId, const Vector3& newPoint, const Scalar newPriority);

		/**
		 * Returns whether this database holds a specified camera pose.
		 * @param poseId The unique id of the pose which will be checked
		 * @param pose Optional resulting  pose value of the defined pose id
		 * @return True, if so
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool hasPose(const Index32 poseId, HomogenousMatrix4* pose = nullptr) const;

		/**
		 * Adds a new camera pose by specifying the unique id of the new pose.
		 * Beware: The given unique id must not exist in the database, define the pose id so that it matches to e.g., a unique frame index.<br>
		 * @param poseId The unique id of the new pose, must be valid
		 * @param pose The pose to be set, may be invalid
		 * @return True, if the given pose id does not exist in the database
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline bool addPose(const Index32 poseId, const HomogenousMatrix4& pose = HomogenousMatrix4(false));

		/**
		 * Removes a pose from this database.
		 * Beware: The specified pose must exist in this database.<br>
		 * @param poseId The id of the pose which will be removed, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void removePose(const Index32 poseId);

		/**
		 * Determines the camera pose (camera frame) in which a specified image point is visible (to which the image point has been added).
		 * Beware: The specified image point must exist in this database.<br>
		 * @param imagePointId The id of the image point for which the corresponding camera pose is requested, must be valid
		 * @return The unique id of the camera pose in which the specified image point is visible, an invalid id if the image point has not been added to any camera pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see attachImagePointToPose(), detachImagePointFromPose().
		 */
		template <bool tThreadSafe>
		inline Index32 poseFromImagePoint(const Index32 imagePointId) const;

		/**
		 * Returns the number of image point observations which belong to a given object point.
		 * @param objectPointId The id of the object point for which the number of observations are requested, must be valid
		 * @return The number of image point observations of the given object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline size_t numberImagePointsFromObjectPoint(const Index32 objectPointId) const;

		/**
		 * Returns all observations (combination of poses and image points) which belong to a given object point.
		 * @param objectPointId The id of the object point for which the connected observations are requested, must be valid
		 * @param poseIds The resulting ids of the poses of the observations
		 * @param imagePointIds The resulting ids of the image points of the observations, one id for each pose
		 * @param imagePoints Optional resulting image points, one point for each image point id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void observationsFromObjectPoint(const Index32 objectPointId, Indices32& poseIds, Indices32& imagePointIds, Vectors2* imagePoints = nullptr) const;

		/**
		 * Returns all observations (combination of poses and image points) which belong to a given object point and a set of pose candidates.
		 * @param objectPointId The id of the object point for which the connected observations are requested, must be valid
		 * @param poseIdCandidates The candidates of pose ids for which the observation will be checked
		 * @param validPoseIndices The resulting indices of the valid pose candidates
		 * @param imagePointIds Optional resulting ids of the image points for which a valid observation exists, one id for each valid pose
		 * @param imagePoints Optional resulting image points, one point for each image point id
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void observationsFromObjectPoint(const Index32 objectPointId, const Indices32& poseIdCandidates, Indices32& validPoseIndices, Indices32* imagePointIds, Vectors2* imagePoints = nullptr) const;

		/**
		 * Returns the object point which belongs to a given image point.
		 * Each image point can be the projection of at most one unique object point.<br>
		 * Beware: The specified image point may not be connected to an object point, in this case the resulting id is an invalid id.<br>
		 * @param imagePointId The id of the image point for which the corresponding object point is requested
		 * @return The id of the corresponding object point, may be invalid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline Index32 objectPointFromImagePoint(const Index32 imagePointId) const;

		/**
		 * Returns all image points which belong to a given camera pose.
		 * Beware: The resulting reference is valid as long as the database is not modified.
		 * @param poseId The id of the camera pose for which the connected image points are requested, must be valid
		 * @return The ids of all image points which are connected with the specified camera pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline const IndexSet32& imagePointsFromPose(const Index32 poseId) const;

		/**
		 * Returns all image points which belong to a given object point.
		 * Beware: The resulting reference is valid as long as the database is not modified.
		 * @param objectPointId The id of the object point for which the connected image points are requested, must be valid
		 * @return The ids of all image points which are connected with the specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline const IndexSet32& imagePointsFromObjectPoint(const Index32 objectPointId) const;

		/**
		 * Returns all poses which belong to a given object point.
		 * @param objectPointId The id of the object point for which the connected poses points are requested, must be valid
		 * @return The ids of all poses which are connected with the specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline IndexSet32 posesFromObjectPoint(const Index32 objectPointId) const;

		/**
		 * Attaches an existing image point to an existing object points (defines the topology between an image point and an object point).
		 * @param imagePointId The id of the image point which will be attached to the specified object points, must be valid
		 * @param objectPointId The id of the object points which will receive the connection to the given image point, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void attachImagePointToObjectPoint(const Index32 imagePointId, const Index32 objectPointId);

		/**
		 * Detaches an image point from an object point (withdraws the topology).
		 * @param imagePointId the id of the image point from which the topology to the object point will be removed, must be valid
		 */
		template <bool tThreadSafe>
		inline void detachImagePointFromObjectPoint(const Index32 imagePointId);

		/**
		 * Attaches an existing image point to an existing camera pose (defines the topology between an image point and a camera pose).
		 * @param imagePointId The id of the image point which will be attached to the specified object points, must be valid
		 * @param poseId The id of the pose which will receive the connection to the given image point, must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void attachImagePointToPose(const Index32 imagePointId, const Index32 poseId);

		/**
		 * Detaches an image point from a camera pose (withdraws the topology).
		 * @param imagePointId the id of the image point from which the topology to the camera pose will be removed, must be valid
		 */
		template <bool tThreadSafe>
		inline void detachImagePointFromPose(const Index32 imagePointId);

		/**
		 * Sets (changes) an image point.
		 * @param imagePointId The id of the image point which will be changed, must be valid
		 * @param imagePoint The new 2D position of the image point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void setImagePoint(const Index32 imagePointId, const Vector2& imagePoint);

		/**
		 * Sets (changes) an object point without modifying the priority value of the object point.
		 * @param objectPointId The id of the object point which will be changed, must be valid
		 * @param objectPoint The new 3D position of the object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setObjectPoints().
		 */
		template <bool tThreadSafe>
		inline void setObjectPoint(const Index32 objectPointId, const Vector3& objectPoint);

		/**
		 * Sets (changes) a set of object points without modifying the priority value of the object points.
		 * @param objectPointIds The ids of the object points which will be changed, must all be valid
		 * @param objectPoints The new 3D positions of the object points, one position for each object point id
		 * @param number The number of object points which will be updated
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setObjectPoint().
		 */
		template <bool tThreadSafe>
		inline void setObjectPoints(const Index32* objectPointIds, const Vector3* objectPoints, const size_t number);

		/**
		 * Sets (changes) a set of object points without modifying the priority value of the object points.
		 * All object points receive the same position e.g., an invalid object point position.
		 * @param objectPointIds The ids of the object points which will be changed, must all be valid
		 * @param number The number of object points which will be updated
		 * @param referenceObjectPoint The one unique object point position to set for each specified object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setObjectPoint().
		 */
		template <bool tThreadSafe>
		inline void setObjectPoints(const Index32* objectPointIds, const size_t number, const Vector3& referenceObjectPoint);

		/**
		 * Sets (changes) all object points to one unique position without modifying the priority value of the object points.
		 * @param objectPoint The 3D position of all object points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setObjectPoint().
		 */
		template <bool tThreadSafe>
		inline void setObjectPoints(const Vector3& objectPoint = invalidObjectPoint());

		/**
		 * Sets (changes) an object point.
		 * @param objectPointId The id of the object point which will be changed, must be valid
		 * @param objectPoint The new 3D position of the object point
		 * @param priority The new priority value of the object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void setObjectPoint(const Index32 objectPointId, const Vector3& objectPoint, const Scalar priority);

		/**
		 * Sets (changes) the priority value of an object point.
		 * @param objectPointId The id of the object point which priority value will be changed, must be valid
		 * @param priority The priority value to be set
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void setObjectPointPriority(const Index32 objectPointId, const Scalar priority);

		/**
		 * Sets (changes) a pose.
		 * @param poseId The id of the pose to be changed, must be valid
		 * @param pose The new pose
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void setPose(const Index32 poseId, const HomogenousMatrix4& pose);

		/**
		 * Sets (changes) a set of poses.
		 * @param poseIds The ids of the poses which will be changed, must all be valid
		 * @param poses The new poses, one pose for each pose id
		 * @param number The number of poses which will be updated
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setPose().
		 */
		template <bool tThreadSafe>
		inline void setPoses(const Index32* poseIds, const HomogenousMatrix4* poses, const size_t number);

		/**
		 * Sets (changes) a set of poses.
		 * @param poses The poses to set, the indices of the pose correspond with the ids of the poses, each pose id must be valid
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setPose().
		 */
		template <bool tThreadSafe>
		inline void setPoses(const ShiftVector<HomogenousMatrix4>& poses);

		/**
		 * Sets (changes) all poses to one unique pose value.
		 * @param pose The pose value of all poses
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see setPose().
		 */
		template <bool tThreadSafe>
		inline void setPoses(const HomogenousMatrix4& pose);

		/**
		 * Returns the ids of all image points visible in a specified camera pose (camera frame).
		 * @param poseId The id of the camera pose in which the image points are visible, must be valid
		 * @return The indices of all image points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		const IndexSet32& imagePointIds(const Index32 poseId) const;

		/**
		 * Returns the ids of all image points which are projections of a set of object point in a specific camera frame.
		 * @param poseId The id of the camera pose in which the image points will be located
		 * @param objectPointIds The ids of the object points for which the image points are requested, this set will be modified so that the set finally contains only object points which have a connected image point (in the specified frame)
		 * @return The resulting ids of the image points, one ids for each object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 imagePointIds(const Index32 poseId, Indices32& objectPointIds) const;

		/**
		 * Returns all image points which are located in a specified frame.
		 * @param poseId The id of the camera pose in which frame the image points are requested
		 * @param imagePointIds Optional resulting ids of the resulting image points one id for each point
		 * @return All image points located in the specified frame
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Vectors2 imagePoints(const Index32 poseId, Indices32* imagePointIds = nullptr) const;

		/**
		 * Returns the ids of all image points that are part of this database.
		 * @param imagePoints Optional resulting image points, one for each resulting image point id, nullptr if not of interest
		 * @return The image point ids
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 imagePointIds(Vectors2* imagePoints = nullptr) const;

		/**
		 * Returns the ids of all object points that are part of this database.
		 * @param objectPoints Optional resulting object points, one for each resulting object point id, nullptr if not of interest
		 * @param priorities Optional resulting object point priorities, one for each resulting object point id, nullptr if not of interest
		 * @return The object point ids
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 objectPointIds(Vectors3* objectPoints = nullptr, Scalars* priorities = nullptr) const;

		/**
		 * Returns the ids of all object points that are part of this database and which are not provided by the explicit set of outlier object point ids.
		 * @param outlierObjectPointIds The ids of all object points which will not be returned
		 * @return The object point ids
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 objectPointIds(const IndexSet32& outlierObjectPointIds) const;

		/**
		 * Returns the ids of all poses that are part of this database.
		 * @param world_T_cameras Optional resulting poses, one for each resulting pose id, nullptr if not of interest
		 * @return The pose ids
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 poseIds(HomogenousMatrices4* world_T_cameras = nullptr) const;

		/**
		 * Returns all object points with a specific location and priority value larger or equal to a given threshold.
		 * @param referencePosition A 3D point value allowing to filter the resulting object point ids
		 * @param objectPoints Optional resulting object point positions, one position for each resulting id
		 * @param minimalPriority The minimal priority value an object point must have to that it will be returned (if it matches the reference position)
		 * @return The ids of all object points that have the given position
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 * @see objectPoints().
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		Indices32 objectPointIds(const Vector3& referencePosition, Vectors3* objectPoints = nullptr, const Scalar minimalPriority = Scalar(-1)) const;

		/**
		 * Returns the ids of all object points with a specific location and having a priority value larger or equal to a given threshold as long as the object point is not defined in the explicit set of outlier object point ids.
		 * @param outlierObjectPointIds The ids of all object points which will not be returned
		 * @param referencePosition A 3D point value allowing to filter the resulting object point ids
		 * @param objectPoints Optional resulting object point positions, one position for each resulting id
		 * @param minimalPriority The minimal priority value an object point must have to that it will be returned (if it matches the reference position)
		 * @return The object point ids
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		Indices32 objectPointIds(const IndexSet32& outlierObjectPointIds, const Vector3& referencePosition, Vectors3* objectPoints = nullptr, const Scalar minimalPriority = Scalar(-1)) const;

		/**
		 * Returns pairs of object point ids combined with counts of valid observations.
		 * The ids are id of object points which have a specified 3D position or which do not have a specified 3D position.<br>
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalPriority The minimal priority value an object point must have to be identified as candidate
		 * @param worker Optional worker object to distribute the computation
		 * @return Pairs of object point ids and numbers of valid camera poses for the individual object points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		inline IndexPairs32 objectPointIdsWithNumberOfObservations(const Vector3& referencePosition, const Scalar minimalPriority = Scalar(-1), Worker* worker = nullptr) const;

		/**
		 * Returns all ids of object points which are visible in a specified frame.
		 * @param poseId The id of the camera pose in which frame the object points are visible
		 * @param objectPoints Optional resulting positions of the resulting object point ids
		 * @return All object point ids visible in the specified frame
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 objectPointIds(const Index32 poseId, Vectors3* objectPoints = nullptr) const;

		/**
		 * Returns all ids of object points which are visible in a specified frame and which match or do not match a specified reference position.
		 * @param poseId The id of the camera pose in which frame the object points are visible
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalPriority The minimal priority value an object point must have so that it will be investigated
		 * @param objectPoints Optional resulting positions of the resulting object point ids
		 * @return All object point ids visible in the specified frame
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		Indices32 objectPointIds(const Index32 poseId, const Vector3& referencePosition, const Scalar minimalPriority = Scalar(-1), Vectors3* objectPoints = nullptr) const;

		/**
		 * Returns all ids of object points which are visible in several specified frames.
		 * @param poseIds The ids of the camera poses in which frame the object points are visible
		 * @param objectPoints Optional resulting positions of the resulting object point ids
		 * @return All object point ids visible in the specified frames
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Indices32 objectPointIds(const Indices32 poseIds, Vectors3* objectPoints = nullptr) const;

		/**
		 * Returns all ids of object points which are visible in a specified frame range.
		 * The function allows to determine object points which are visible in all frames of the specified frame range or in any of the frames.
		 * @param lowerPoseId Pose id defining the lower pose id border of all poses which will be investigated
		 * @param upperPoseId Pose id defining the lower pose id border of all poses which will be investigated, with range [lowerPoseId, infinity)
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalPriority The minimal priority value an object point must have so that it will be investigated
		 * @param objectPoints Optional resulting positions of the resulting object point ids
		 * @return All object point ids visible in the specified frames
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 * @tparam tVisibleInAllPoses True, if the object points must be visible in all poses (frames) of the specified pose range; False, if the object point can be visible in any poses (frames) within the specified pose range
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tVisibleInAllPoses>
		Indices32 objectPointIds(const Index32 lowerPoseId, const Index32 upperPoseId, const Vector3& referencePosition = invalidObjectPoint(), const Scalar minimalPriority = Scalar(-1), Vectors3* objectPoints = nullptr) const;

		/**
		 * Returns all ids of object points which are visible in specified keyframes.
		 * The function allows to determine object points which are visible in all keyframes or in any of the keyframes.
		 * @param poseIds The ids of the keyframes which will be investigated
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalPriority The minimal priority value an object point must have so that it will be investigated
		 * @param objectPoints Optional resulting positions of the resulting object point ids
		 * @return All object point ids visible in the specified frames
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 * @tparam tVisibleInAllPoses True, if the object points must be visible in all poses (frames) of the specified pose range; False, if the object point can be visible in any poses (frames) within the specified pose range
		 */
		template <bool tThreadSafe, bool tMatchPosition, bool tVisibleInAllPoses>
		Indices32 objectPointIds(const Indices32& poseIds, const Vector3& referencePosition = invalidObjectPoint(), const Scalar minimalPriority = Scalar(-1), Vectors3* objectPoints = nullptr) const;

		/**
		 * Returns all image points which are located in a specified frame and are projections of object points.
		 * @param poseId The id of the camera pose in which frame the image points are requested
		 * @param objectPointIds Resulting object point ids corresponding to the individual image points
		 * @return The resulting image points located in the specified frame and having a corresponding object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see imagePointsFromObjectPoints().
		 */
		template <bool tThreadSafe>
		Vectors2 imagePointsWithObjectPoints(const Index32 poseId, Indices32& objectPointIds) const;

		/**
		 * Returns all image points which are located in a specified frame and which are projections of a set of given object points.
		 * As not all object points may be visible in the specified frame, the set of given object points will be modified so that set contains only visible object points after calling this function.<br>
		 * @param poseId The id of the camera pose in which frame the image points are requested
		 * @param objectPointIds The ids of the object points for which the corresponding image points are requested
		 * @param imagePointIds Optional resulting image point ids of the resulting image points
		 * @return The resulting image points located in the specified frame
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @see imagePointsWithObjectPoints().
		 */
		template <bool tThreadSafe>
		Vectors2 imagePointsFromObjectPoints(const Index32 poseId, Indices32& objectPointIds, Indices32* imagePointIds = nullptr) const;

		/**
		 * Returns all image points which are located in a specified frame and which are projections of a set of given object points.
		 * As not all object points may be visible in the specified frame, the number of resulting image points may be smaller than the number of specified object points.<br>
		 * The set of specified object points is untouched, however a resulting set of indices return the indices of valid object points (indices as specified in the set of object points).<br>
		 * @param poseId The id of the camera pose in which frame the image points are requested
		 * @param objectPointIds The ids of the object points for which the corresponding image points are requested
		 * @param validIndices The indices of valid object points, !not! the ids of valid object points
		 * @param imagePointIds Optional resulting image point ids of the resulting image points
		 * @return The image points which are visible projections of the specified object points, the number of image points is equal to the resulting set of indices of valid object points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Vectors2 imagePointsFromObjectPoints(const Index32 poseId, const Indices32& objectPointIds, Indices32& validIndices, Indices32* imagePointIds = nullptr) const;

		/**
		 * Returns all image points which are located in a specified frame and which are projections of a set of given object points.
		 * As not all object points may be visible in the specified frame, the number of resulting image points may be smaller than the number of specified object points.<br>
		 * The set of specified object points is untouched, however a resulting set of indices return the indices of valid object points (indices as specified in the set of object points).<br>
		 * @param poseId The id of the camera pose in which frame the image points are requested
		 * @param objectPointIds The ids of the object points for which the corresponding image points are requested
		 * @param numberObjectPointIds The number of given object point ids
		 * @param validIndices The indices of valid object points, !not! the ids of valid object points
		 * @param imagePointIds Optional resulting image point ids of the resulting image points
		 * @return The image points which are visible projections of the specified object points, the number of image points is equal to the resulting set of indices of valid object points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		Vectors2 imagePointsFromObjectPoints(const Index32 poseId, const Index32* objectPointIds, const size_t numberObjectPointIds, Indices32& validIndices, Indices32* imagePointIds = nullptr) const;

		/**
		 * Determines the groups of image points matching to unique object points in individual camera poses.
		 * Image points within one group correspond to one object point while the order of the image points correspond with the order of the given camera poses.<br>
		 * @param poseIds The ids of the camera pose in which the object points are visible which form the groups of image points.<br>
		 * @param objectPointIds Resulting ids of object points which are visible in all camera pose and to which the resulting groups of image points correspond
		 * @return Resulting groups of image points one group for each resulting object point
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		ImagePointGroups imagePointGroups(const Indices32 poseIds, Indices32& objectPointIds) const;

		/**
		 * Returns object points with corresponding image points entirely visible in a specific range of camera poses.
		 * @param poseId The id of the camera pose which is the start position of the range of camera poses
		 * @param previous True, if the range covers the previous camera poses; False, if the range covers the subsequent camera poses
		 * @param minimalObservations The minimal number of successive camera poses in which an object point must be visible
		 * @param maximalObservations Optional the maximal number of successive camera poses (more poses will not be investigated), 0 or with range [minimalObservations, infinity)
		 * @return The map mapping object points to image points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		IdIdPointPairsMap imagePoints(const Index32 poseId, const bool previous, const size_t minimalObservations = 2, const size_t maximalObservations = 0) const;

		/**
		 * Determines the image points which are projections from the same object points and are visible in two individual camera poses.
		 * @param pose0 The id of the first camera pose, must be valid
		 * @param pose1 The id of the second camera pose, must be valid and must not be 'pose0'
		 * @param points0 The resulting image points visible in the first camera pose
		 * @param points1 The resulting image points visible in the second camera pose, each point corresponds to one point from 'points0'
		 * @param objectPointIds Optional resulting ids of the object points which are visible in both camera poses
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		void imagePoints(const Index32 pose0, const Index32 pose1, Vectors2& points0, Vectors2& points1, Indices32* objectPointIds = nullptr) const;

		/**
		 * Returns corresponding object points and image points for a given camera pose.
		 * @param poseId The id of the camera pose for which the object and image points are requested
		 * @param imagePoints The resulting image points located in the specified camera pose
		 * @param objectPoints The resulting object points, each point corresponds to one image points from 'imagePoints'
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalObservations The minimal number of observations a resulting object points must have (in arbitrary sibling camera pose)
		 * @param imagePointIds Optional ids of the resulting image points
		 * @param objectPointIds Optional ids of the resulting object points
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		void imagePointsObjectPoints(const Index32 poseId, Vectors2& imagePoints, Vectors3& objectPoints, const Vector3& referencePosition = invalidObjectPoint(), const size_t minimalObservations = 0, Indices32* imagePointIds = nullptr, Indices32* objectPointIds = nullptr) const;

		/**
		 * Returns two groups of corresponding object points and image points for a given camera pose.
		 * The first group of correspondences have object points from the given set of priority object points
		 * The second group of correspondences have object points not given in the set of priority object points
		 * @param poseId The id of the camera pose for which the object and image points are requested
		 * @param priorityIds The ids of the object points which will belong to the group of priority correspondences
		 * @param priorityImagePoints The resulting image points located in the specified camera pose belonging to the priority group
		 * @param priorityObjectPoints The resulting object points belonging to the priority group, each point corresponds to one image points from 'imagePoints'
		 * @param remainingImagePoints The resulting image points located in the specified camera pose belonging to the remaining group
		 * @param remainingObjectPoints The resulting object points belonging to the remaining group, each point corresponds to one image points from 'imagePoints'
		 * @param referencePosition The 3D reference position which is used to filter the object points
		 * @param minimalObservations The minimal number of observations a resulting object points must have (in arbitrary sibling camera pose)
		 * @param priorityImagePointIds Optional ids of the resulting image points belonging to the priority group
		 * @param priorityObjectPointIds Optional ids of the resulting object points belonging to the priority group
		 * @param remainingImagePointIds Optional ids of the resulting image points belonging to the remaining group
		 * @param remainingObjectPointIds Optional ids of the resulting object points belonging to the remaining group
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 */
		template <bool tThreadSafe, bool tMatchPosition>
		void imagePointsObjectPoints(const Index32 poseId, const IndexSet32& priorityIds, Vectors2& priorityImagePoints, Vectors3& priorityObjectPoints, Vectors2& remainingImagePoints, Vectors3& remainingObjectPoints, const Vector3& referencePosition = invalidObjectPoint(), const size_t minimalObservations = 0, Indices32* priorityImagePointIds = nullptr, Indices32* priorityObjectPointIds = nullptr, Indices32* remainingImagePointIds = nullptr, Indices32* remainingObjectPointIds = nullptr) const;

		/**
		 * Returns corresponding poses and image points for a given object point from the entire range of possible camera poses.
		 * @param objectPointId The id of the object point for which the poses and image points are requested
		 * @param poses The resulting poses in which the object point is visible
		 * @param imagePoints The resulting image points which are the projections of the object points, each image point corresponds with one pose
		 * @param referencePose A pose allowing to filter the resulting poses so that either valid or invalid poses are found
		 * @param poseIds Optional ids of the resulting poses
		 * @param imagePointIds Optional ids of the resulting image points
		 * @param lowerPoseId Optional pose id defining the lower pose id border, invalidId if no lower border is defined
		 * @param upperPoseId Optional pose id defining the upper pose id border, invalidId if no upper border is defined
		 * @tparam tThreadSafe True, to call this function thread-safe
		 * @tparam tMatchPose True, if the defined pose will match the values of the resulting poses; False, if the defined pose will not match the values of the resulting poses
		 */
		template <bool tThreadSafe, bool tMatchPose>
		void posesImagePoints(const Index32 objectPointId, HomogenousMatrices4& poses, Vectors2& imagePoints, const HomogenousMatrix4& referencePose = HomogenousMatrix4(false), Indices32* poseIds = nullptr, Indices32* imagePointIds = nullptr, const Index32 lowerPoseId = invalidId, const Index32 upperPoseId = invalidId) const;

		/**
		 * Returns topology triples with valid image points ids, object points ids and pose ids for a set of given pose ids.
		 * @param poseIds The ids of the camera pose for which the topology triples are requested.
		 * @return The resulting topology triples
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		TopologyTriples topologyTriples(const Indices32& poseIds) const;

		/**
		 * Clears the database including all camera poses, object points, image points and any topology.
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void clear();

		/**
		 * Resets the geometric information of this database for 3D object points and 6DOF camera poses.
		 * However, the 2D image point locations are untouched.
		 * @param referenceObjectPoint The new object point value for each object point of this database
		 * @param referencePose The new pose value for each pose of this database
		 * @tparam tThreadSafe True, to call this function thread-safe
		 */
		template <bool tThreadSafe>
		inline void reset(const Vector3& referenceObjectPoint = invalidObjectPoint(), const HomogenousMatrix4& referencePose = HomogenousMatrix4(false));

		/**
		 * Resets this database with given poses, object points, image points, and topology.
		 * @param numberPoses The number of the provided poses, with range [0, infinity)
		 * @param poseIds The ids of all poses, nullptr if 'numberPoses == 0'
		 * @param poses The poses, one for each pose id, nullptr if 'numberPoses == 0'
		 * @param numberObjectPoints The number of the provided object points, with range [0, infinity)
		 * @param objectPointIds The ids of all object points, nullptr if 'numberObjectPoints == 0'
		 * @param objectPoints The object points, one for each object point id, nullptr if 'numberObjectPoints == 0'
		 * @param objectPointPriorities The priorities of the object points, one for each object point id, nullptr if 'numberObjectPoints == 0'
		 * @param numberImagePoints The number of provided image points, with range [0, infinity)
		 * @param imagePointIds The ids of all image points, nullptr if 'numberImagePoints == 0'
		 * @param imagePoints The image points, one for each image point id, nullptr if 'numberImagePoints == 0'
		 * @param topologyPoseIds The ids of the poses to which an image point belongs, one for each image point, 'invalidId' if unknown
		 * @param topologyObjectPointIds The ids of all object points to which an image point belongs, one for each image point, 'invalidId' if unknown
		 */
		template <typename T, bool tThreadSafe>
		void reset(const size_t numberPoses, const Index32* poseIds, const HomogenousMatrixT4<T>* poses, const size_t numberObjectPoints, const Index32* objectPointIds, const VectorT3<T>* objectPoints, const T* objectPointPriorities, const size_t numberImagePoints, const Index32* imagePointIds, const VectorT2<T>* imagePoints, const Index32* topologyPoseIds, const Index32* topologyObjectPointIds);

		/**
		 * Filters a set of given topology triples due to a set of given pose ids.
		 * @param topologyTriples The set of topology triplies which will be filtered
		 * @param poseIds The ids of the camera pose defining which triples are returned (the indices respectively)
		 * @return The indices of the topology triples which belong to one of the given camera poses
		 */
		static inline Indices32 filterTopologyTriplesPoses(const TopologyTriples& topologyTriples, const IndexSet32& poseIds);

		/**
		 * Filters a set of given topology triples due to a set of given object point ids.
		 * @param topologyTriples The set of topology triplies which will be filtered
		 * @param objectPointIds The ids of the object points defining which triples are returned (the indices respectively)
		 * @return The indices of the topology triples which belong to one of the given object points
		 */
		static inline Indices32 filterTopologyTriplesObjectPoints(const TopologyTriples& topologyTriples, const IndexSet32& objectPointIds);

		/**
		 * Filters a set of given topology triples due to a set of given image point ids.
		 * @param topologyTriples The set of topology triplies which will be filtered
		 * @param imagePointIds The ids of the image points defining which triples are returned (the indices respectively)
		 * @return The indices of the topology triples which belong to one of the given image points
		 */
		static inline Indices32 filterTopologyTriplesImagePoints(const TopologyTriples& topologyTriples, const IndexSet32& imagePointIds);

		/**
		 * Determines reliable object points from a set of given topology triples (by determining all object points with a minimal number of observations).
		 * @param topologyTriples The set of topology triples from which the reliable object points are determined
		 * @param minimalObservations The minimal number of observations (the number of camera poses in which the object point is visible) an object point must have to count as reliable
		 * @return The ids of the reliable object points
		 */
		static inline Indices32 reliableObjectPoints(const TopologyTriples& topologyTriples, const unsigned int minimalObservations);

		/**
		 * Converts the set of topology triples into a representation which is forced/oriented by object points so that the camera poses and image points can be accessed for a specific object points.
		 * @param topologyTriples The set of topology triples which will be converted
		 * @param indices Optional subset of the given topology, the indices of the topology triples that will be added to the resulting (object point forced) data structure, nullptr to use all triples
		 * @return The object point forced data structure of the given topology triples
		 */
		static PoseImagePointTopologyGroups objectPointTopology(const TopologyTriples& topologyTriples, const Indices32* indices = nullptr);

		/**
		 * Assign operator copying a second database to this database object.
		 * @param database The database object to be copied
		 * @return Reference to this object
		 */
		inline Database& operator=(const Database& database);

		/**
		 * Move operator moving a second database to this database object.
		 * @param database The database object to be moved
		 * @return Reference to this object
		 */
		inline Database& operator=(Database&& database) noexcept;

		/**
		 * Returns whether this database holds at least one image point, one object point or one camera pose.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Counts the number of valid correspondences between image and object points for a subset of several poses individually.
		 * @param lowerPoseId The id (index) of the frame defining the lower border of camera poses which will be investigated
		 * @param referenceObjectPoint A reference object point allowing to filter the correspondences to count
		 * @param minimalPriority The minimal priority value an object point must have so that is will be investigated
		 * @param correspondences The resulting correspondences, one for each frame, starting with 'lowerPoseId' (the first entry corresponds to 'lowerPoseId' and so on)
		 * @param firstPose The index (not the id) of the first pose to handle
		 * @param numberPoses The number of poses to handle
		 * @tparam tMatchPosition True, if the defined position will match the positions of the correspondences; False, if the defined position will not match the positions of the correspondences
		 * @tparam tNeedValidPose True, if the pose must be valid so that the number of valid correspondences will be determined, otherwise the number of correspondences will be zero
		 */
		template <bool tMatchPosition, bool tNeedValidPose>
		void numberCorrespondencesSubset(const Index32 lowerPoseId, const Vector3* referenceObjectPoint, const Scalar minimalPriority, unsigned int* correspondences, const unsigned int firstPose, const unsigned int numberPoses) const;

		/**
		 * Returns pairs of object point ids combined with counts of valid observations.
		 * @param objectPointIds The ids of the object points for which the number of observations is determined
		 * @param referencePosition The 3D position of the object points to find or to avoid (e.g., may be an invalid position to identify all invalid object points)
		 * @param minimalPriority The minimal priority value an object point must have to be identified as candidate
		 * @param pairs The resulting pairs of object point ids and numbers of valid camera poses for the individual object points
		 * @param lock Optional lock object, must be defined if the function is executed on several threads in parallel
		 * @param firstObjectPoint The first object point to be handled
		 * @param numberObjectPoints The number of object points to be handled
		 * @tparam tMatchPosition True, if the defined position will match the positions of the resulting object points; False, if the defined position will not match the positions of the resulting object points
		 * @see objectPointIdsWithNumberOfObservations().
		 */
		template <bool tMatchPosition>
		void objectPointIdsWithNumberOfObservationsSubset(const Index32* objectPointIds, const Vector3* referencePosition, const Scalar minimalPriority, IndexPairs32* pairs, Lock* lock, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints) const;

		/**
		 * Counts the number of valid poses of a given object point.
		 * @param objectPointId The id of the object point for which the number of valid poses is determined
		 * @param imagePointIds The ids of the image points which are the projections of the defined object point (must be extracted from the ObjectPointData object of the given object point)
		 * @return The number of valid camera poses
		 */
		inline unsigned int numberValidPoses(const Index32 objectPointId, const IndexSet32& imagePointIds) const;

		/**
		 * Returns the first 32 bit index of a 64 bit index.
		 * @param index The 64 bit index
		 * @return First 32 bit index
		 */
		static inline Index32 firstIndex(const Index64 index);

		/**
		 * Returns the second 32 bit index of a 64 bit index.
		 * @param index The 64 bit index
		 * @return Second 32 bit index
		 */
		static inline Index32 secondIndex(const Index64 index);

		/**
		 * Returns the 64 bit index composed of two 32 bit indices.
		 * @param first The first 32 bit index
		 * @param second The second 32 bit index
		 * @return The resulting 64 bit index
		 */
		static inline Index64 index64(const Index32 first, const Index32 second);

	protected:

		/// The map mapping unique pose ids to pose data instances.
		PoseMap databasePoseMap;

		/// The map mapping unique object point ids to object point data instances.
		ObjectPointMap databaseObjectPointMap;

		/// The map mapping unique image points ids to image point data instances.
		ImagePointMap databaseImagePointMap;

		/// The map mapping a pair of pose id and object point id to image point ids.
		Index64To32Map databasePoseObjectPointMap;

		/// The number of poses.
		unsigned int databasePoses;

		/// The counter for unique object point ids.
		Index32 databaseObjectPointIdCounter;

		/// The counter for unique image point ids.
		Index32 databaseImagePointIdCounter;

		/// The lock for the entire database.
		mutable Lock databaseLock;
};

inline Vector3 Database::invalidObjectPoint()
{
	return Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
}

template <bool tThreadSafe>
inline Database::ConstImagePointAccessorIds<tThreadSafe>::ConstImagePointAccessorIds(const Database& database, const Indices32& imagePointIds) :
	accessorDatabase(database),
	accessorImagePointIds(imagePointIds)
{
	// nothing to do here
}

template <bool tThreadSafe>
size_t Database::ConstImagePointAccessorIds<tThreadSafe>::size() const
{
	return accessorImagePointIds.size();
}

template <bool tThreadSafe>
const Vector2& Database::ConstImagePointAccessorIds<tThreadSafe>::operator[](const size_t& index) const
{
	ocean_assert(index < accessorImagePointIds.size());
	return accessorDatabase.imagePoint<tThreadSafe>(accessorImagePointIds[index]);
}

template <bool tThreadSafe>
inline Database::ConstImagePointAccessorTopology<tThreadSafe>::ConstImagePointAccessorTopology(const Database& database, const PoseImagePointTopology& topology) :
	accessorDatabase(database),
	accessorTopology(topology)
{
	// nothing to do here
}

template <bool tThreadSafe>
size_t Database::ConstImagePointAccessorTopology<tThreadSafe>::size() const
{
	return accessorTopology.size();
}

template <bool tThreadSafe>
const Vector2& Database::ConstImagePointAccessorTopology<tThreadSafe>::operator[](const size_t& index) const
{
	ocean_assert(index < accessorTopology.size());
	return accessorDatabase.imagePoint<tThreadSafe>(accessorTopology[index].imagePointId());
}

template <bool tThreadSafe>
inline Database::ConstObjectPointAccessorIds<tThreadSafe>::ConstObjectPointAccessorIds(const Database& database, const Indices32& objectPointIds) :
	accessorDatabase(database),
	accessorObjectPointIds(objectPointIds)
{
	// nothing to do here
}

template <bool tThreadSafe>
size_t Database::ConstObjectPointAccessorIds<tThreadSafe>::size() const
{
	return accessorObjectPointIds.size();
}

template <bool tThreadSafe>
const Vector3& Database::ConstObjectPointAccessorIds<tThreadSafe>::operator[](const size_t& index) const
{
	ocean_assert(index < accessorObjectPointIds.size());
	return accessorDatabase.objectPoint<tThreadSafe>(accessorObjectPointIds[index]);
}

template <bool tThreadSafe>
inline Database::ConstPoseAccessorIds<tThreadSafe>::ConstPoseAccessorIds(const Database& database, const Indices32& poseIds) :
	accessorDatabase(database),
	accessorPoseIds(poseIds)
{
	// nothing to do here
}

template <bool tThreadSafe>
size_t Database::ConstPoseAccessorIds<tThreadSafe>::size() const
{
	return accessorPoseIds.size();
}

template <bool tThreadSafe>
const HomogenousMatrix4& Database::ConstPoseAccessorIds<tThreadSafe>::operator[](const size_t& index) const
{
	ocean_assert(index < accessorPoseIds.size());
	return accessorDatabase.pose<tThreadSafe>(accessorPoseIds[index]);
}

template <bool tThreadSafe>
inline Database::ConstPoseAccessorTopology<tThreadSafe>::ConstPoseAccessorTopology(const Database& database, const PoseImagePointTopology& topology) :
	accessorDatabase(database),
	accessorTopology(topology)
{
	// nothing to do here
}

template <bool tThreadSafe>
size_t Database::ConstPoseAccessorTopology<tThreadSafe>::size() const
{
	return accessorTopology.size();
}

template <bool tThreadSafe>
const HomogenousMatrix4& Database::ConstPoseAccessorTopology<tThreadSafe>::operator[](const size_t& index) const
{
	ocean_assert(index < accessorTopology.size());
	return accessorDatabase.pose<tThreadSafe>(accessorTopology[index].poseId());
}

inline Database::ImagePointObject::ImagePointObject(const Index32 imagePointId) :
	objectImagePointId(imagePointId)
{
	// nothing to do here
}

inline Index32 Database::ImagePointObject::imagePointId() const
{
	return objectImagePointId;
}

inline void Database::ImagePointObject::setImagePointId(const Index32 imagePointId)
{
	objectImagePointId = imagePointId;
}

inline Database::ObjectPointObject::ObjectPointObject(const Index32 objectPointId) :
	objectObjectPointId(objectPointId)
{
	// nothing to do here
}

inline Index32 Database::ObjectPointObject::objectPointId() const
{
	return objectObjectPointId;
}

inline void Database::ObjectPointObject::setObjectPointId(const Index32 objectPointId)
{
	objectObjectPointId = objectPointId;
}

inline Database::PoseObject::PoseObject(const Index32 poseId) :
	objectPoseId(poseId)
{
	// nothing to do here
}

inline Index32 Database::PoseObject::poseId() const
{
	return objectPoseId;
}

inline void Database::PoseObject::setPoseId(const Index32 poseId)
{
	objectPoseId = poseId;
}

inline Database::TopologyTriple::TopologyTriple(const Index32 poseId, const Index32 objectPointId, const Index32 imagePointId) :
	PoseObject(poseId),
	ObjectPointObject(objectPointId),
	ImagePointObject(imagePointId)
{
	// nothing to do here
}

inline Database::PoseImagePointPair::PoseImagePointPair(const Index32 poseId, const Index32 imagePointId) :
	ImagePointObject(imagePointId),
  PoseObject(poseId)
{
	// nothing to do here
}

inline Database::ImagePointData::ImagePointData() :
	dataPoint(Numeric::minValue(), Numeric::minValue()),
	dataPoseId(invalidId),
	dataObjectPointId(invalidId)
{
	// nothing to do here
}

inline Database::ImagePointData::ImagePointData(const Vector2& point, const Index32 poseId, const Index32 objectPointId) :
	dataPoint(point),
	dataPoseId(poseId),
	dataObjectPointId(objectPointId)
{
	// nothing to do here
}

inline const Vector2& Database::ImagePointData::point() const
{
	return dataPoint;
}

inline Index32 Database::ImagePointData::poseId() const
{
	return dataPoseId;
}

inline Index32 Database::ImagePointData::objectPointId() const
{
	return dataObjectPointId;
}

inline void Database::ImagePointData::setPoint(const Vector2& point)
{
	dataPoint = point;
}

inline void Database::ImagePointData::setPoseId(const Index32 poseId)
{
	dataPoseId = poseId;
}

inline void Database::ImagePointData::setObjectPointId(const Index32 objectPointId)
{
	dataObjectPointId = objectPointId;
}

inline const IndexSet32& Database::Data::imagePointIds() const
{
	return dataImagePointIds;
}

inline void Database::Data::registerImagePoint(const Index32 imagePointId)
{
	ocean_assert(dataImagePointIds.find(imagePointId) == dataImagePointIds.end());
	dataImagePointIds.insert(imagePointId);
}

inline void Database::Data::unregisterImagePoint(const Index32 imagePointId)
{
	ocean_assert(dataImagePointIds.find(imagePointId) != dataImagePointIds.end());
	dataImagePointIds.erase(imagePointId);
}

inline Database::PoseData::PoseData(const HomogenousMatrix4& world_T_camera, const Scalar fov) :
	world_T_camera_(world_T_camera),
	dataFov(fov)
{
	// nothing to do here
}

inline const HomogenousMatrix4& Database::PoseData::pose() const
{
	return world_T_camera_;
}

inline Scalar Database::PoseData::fov() const
{
	return dataFov;
}

inline void Database::PoseData::setPose(const HomogenousMatrix4& world_T_camera)
{
	world_T_camera_ = world_T_camera;
}

inline void Database::PoseData::setFov(const Scalar fov)
{
	dataFov = fov;
}

inline Database::ObjectPointData::ObjectPointData(const Vector3& point, const Scalar priority) :
	dataPoint(point),
	dataPriority(priority)
{
	// nothing to do here
}

inline const Vector3& Database::ObjectPointData::point() const
{
	return dataPoint;
}

inline Scalar Database::ObjectPointData::priority() const
{
	return dataPriority;
}

inline void Database::ObjectPointData::setPoint(const Vector3& point)
{
	dataPoint = point;
}

inline void Database::ObjectPointData::setPriority(const Scalar priority)
{
	dataPriority = priority;
}

inline Database::Database() :
	databasePoses(0u),
	databaseObjectPointIdCounter(invalidId),
	databaseImagePointIdCounter(invalidId)
{
	// nothing to do here
}

inline Database::Database(const Database& database) :
	databasePoseMap(database.databasePoseMap),
	databaseObjectPointMap(database.databaseObjectPointMap),
	databaseImagePointMap(database.databaseImagePointMap),
	databasePoseObjectPointMap(database.databasePoseObjectPointMap),
	databasePoses(database.databasePoses),
	databaseObjectPointIdCounter(database.databaseObjectPointIdCounter),
	databaseImagePointIdCounter(database.databaseImagePointIdCounter)
{
	// nothing to do here
}

inline Database::Database(Database&& database) noexcept :
	databasePoseMap(std::move(database.databasePoseMap)),
	databaseObjectPointMap(std::move(database.databaseObjectPointMap)),
	databaseImagePointMap(std::move(database.databaseImagePointMap)),
	databasePoseObjectPointMap(std::move(database.databasePoseObjectPointMap)),
	databasePoses(database.databasePoses),
	databaseObjectPointIdCounter(database.databaseObjectPointIdCounter),
	databaseImagePointIdCounter(database.databaseImagePointIdCounter)
{
	database.databasePoses = 0u;
	database.databaseObjectPointIdCounter = invalidId;
	database.databaseImagePointIdCounter = invalidId;
}

inline Lock& Database::lock()
{
	return databaseLock;
}

template <bool tThreadSafe>
inline bool Database::isEmpty() const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	return databasePoseMap.empty() && databaseObjectPointMap.empty() && databaseImagePointMap.empty();
}

template <bool tThreadSafe>
inline size_t Database::poseNumber() const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	return databasePoseMap.size();
}

template <bool tThreadSafe>
inline size_t Database::objectPointNumber() const
{
  const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

  return databaseObjectPointMap.size();
}

template <bool tThreadSafe>
inline size_t Database::imagePointNumber() const
{
  const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

  return databaseImagePointMap.size();
}

template <bool tThreadSafe>
inline const Vector2& Database::imagePoint(const Index32 imagePointId) const
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseImagePointMap.find(imagePointId) != databaseImagePointMap.end());
	return databaseImagePointMap.find(imagePointId)->second.point();
}

template <bool tThreadSafe>
inline Vectors2 Database::imagePoints(const Indices32& imagePointIds) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors2 imagePoints;
	imagePoints.reserve(imagePointIds.size());

	for (Indices32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
	{
		ocean_assert(*i != invalidId);
		ocean_assert(databaseImagePointMap.find(*i) != databaseImagePointMap.end());

		imagePoints.push_back(databaseImagePointMap.find(*i)->second.point());
	}

	return imagePoints;
}

template <bool tThreadSafe>
inline Vectors2 Database::imagePoints(const IndexSet32& imagePointIds) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors2 imagePoints;
	imagePoints.reserve(imagePointIds.size());

	for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
	{
		ocean_assert(*i != invalidId);
		ocean_assert(databaseImagePointMap.find(*i) != databaseImagePointMap.end());

		imagePoints.push_back(databaseImagePointMap.find(*i)->second.point());
	}

	return imagePoints;
}

template <bool tThreadSafe>
inline bool Database::hasObservation(const Index32 poseId, const Index32 objectPointId, Vector2* point, Index32* pointId) const
{
	ocean_assert(objectPointId != invalidId && poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const Index64To32Map::const_iterator i = databasePoseObjectPointMap.find(index64(poseId, objectPointId));

	if (i == databasePoseObjectPointMap.end())
		return false;

	if (!point && !pointId)
		return true;

	const ImagePointMap::const_iterator iI = databaseImagePointMap.find(i->second);
	ocean_assert(iI != databaseImagePointMap.end());

	if (point)
		*point = iI->second.point();

	if (pointId)
		*pointId = iI->first;

	return true;
}

template <bool tThreadSafe>
inline const Vector3& Database::objectPoint(const Index32 objectPointId) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseObjectPointMap.find(objectPointId) != databaseObjectPointMap.end());
	return databaseObjectPointMap.find(objectPointId)->second.point();
}

template <bool tThreadSafe>
inline const Vector3& Database::objectPoint(const Index32 objectPointId, Scalar& objectPointPriority) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.end());

	objectPointPriority = i->second.priority();
	return i->second.point();
}

template <bool tThreadSafe>
inline Scalar Database::objectPointPriority(const Index32 objectPointId) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseObjectPointMap.find(objectPointId) != databaseObjectPointMap.end());
	return databaseObjectPointMap.find(objectPointId)->second.priority();
}

template <bool tThreadSafe>
inline Vectors3 Database::objectPoints() const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors3 objectPoints;
	objectPoints.reserve(databaseObjectPointMap.size());

	for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
		objectPoints.push_back(i->second.point());

	return objectPoints;
}

template <bool tThreadSafe, bool tMatchPosition>
inline Vectors3 Database::objectPoints(const Vector3& referencePosition, Indices32* objectPointIds, const Scalar minimalPriority) const
{
	ocean_assert(!objectPointIds || objectPointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors3 objectPoints;
	objectPoints.reserve(databaseObjectPointMap.size());

	if (objectPointIds)
	{
		objectPointIds->clear();
		objectPointIds->reserve(databaseObjectPointMap.size());

		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition)))
			{
				objectPoints.push_back(i->second.point());
				objectPointIds->push_back(i->first);
			}
	}
	else
	{
		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition)))
				objectPoints.push_back(i->second.point());
	}

	return objectPoints;
}

template <bool tThreadSafe>
inline Vectors3 Database::objectPoints(const Indices32& objectPointIds) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors3 objectPoints;
	objectPoints.reserve(objectPointIds.size());

	for (Indices32::const_iterator i = objectPointIds.begin(); i != objectPointIds.end(); ++i)
	{
		ocean_assert(*i != invalidId);
		ocean_assert(databaseObjectPointMap.find(*i) != databaseObjectPointMap.end());

		objectPoints.push_back(databaseObjectPointMap.find(*i)->second.point());
	}

	return objectPoints;
}

template <bool tThreadSafe>
inline const HomogenousMatrix4& Database::pose(const Index32 poseId) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databasePoseMap.find(poseId) != databasePoseMap.end());
	return databasePoseMap.find(poseId)->second.pose();
}

template <bool tThreadSafe>
inline HomogenousMatrices4 Database::poses(const Index32* poseIds, const size_t size) const
{
	ocean_assert(poseIds != nullptr && size != 0);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	HomogenousMatrices4 result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		ocean_assert(databasePoseMap.find(poseIds[n]) != databasePoseMap.end());
		result.push_back(databasePoseMap.find(poseIds[n])->second.pose());
	}

	return result;
}

template <bool tThreadSafe>
inline SquareMatrices3 Database::rotationalPoses(const Index32* poseIds, const size_t size) const
{
	ocean_assert(poseIds != nullptr && size != 0);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	SquareMatrices3 result;
	result.reserve(size);

	for (size_t n = 0; n < size; ++n)
	{
		ocean_assert(databasePoseMap.find(poseIds[n]) != databasePoseMap.end());

		const HomogenousMatrix4& pose = databasePoseMap.find(poseIds[n])->second.pose();

		ocean_assert(pose.translation().isNull());
		result.push_back(pose.rotationMatrix());
	}

	return result;
}

template <bool tThreadSafe, bool tMatchPose>
inline HomogenousMatrices4 Database::poses(const HomogenousMatrix4& referencePose, Indices32* poseIds) const
{
	ocean_assert(!poseIds || poseIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	HomogenousMatrices4 poses;
	poses.reserve(databasePoseMap.size());

	if (poseIds)
	{
		poseIds->clear();
		poseIds->reserve(databasePoseMap.size());

		for (PoseMap::const_iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
			if ((tMatchPose && i->second.pose() == referencePose) || (!tMatchPose && i->second.pose() != referencePose))
			{
				poses.push_back(i->second.pose());
				poseIds->push_back(i->first);
			}
	}
	else
	{
		for (PoseMap::const_iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
			if ((tMatchPose && i->second.pose() == referencePose) || (!tMatchPose && i->second.pose() != referencePose))
				poses.push_back(i->second.pose());
	}

	return poses;
}

template <bool tThreadSafe>
inline HomogenousMatrices4 Database::poses(const Index32 lowerPoseId, const Index32 upperPoseId) const
{
	ocean_assert(lowerPoseId <= upperPoseId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	HomogenousMatrices4 poses;
	poses.reserve(upperPoseId - lowerPoseId + 1u);

	for (unsigned int n = lowerPoseId; n <= upperPoseId; ++n)
	{
		// **TODO** the performance can be improved if we iterate through the map
		PoseMap::const_iterator i = databasePoseMap.find(n);
		if (i != databasePoseMap.end())
			poses.push_back(i->second.pose());
		else
			poses.push_back(HomogenousMatrix4(false));
	}

	return poses;
}

template <bool tThreadSafe, bool tMatchPose>
inline Indices32 Database::poseIds(const HomogenousMatrix4& referencePose, HomogenousMatrices4* poses) const
{
	ocean_assert(!poses || poses->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 poseIds;
	poseIds.reserve(databasePoseMap.size());

	if (poses)
	{
		poses->clear();
		poses->reserve(databasePoseMap.size());

		for (PoseMap::const_iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
			if ((tMatchPose && i->second.pose() == referencePose) || (!tMatchPose && i->second.pose() != referencePose))
			{
				poseIds.push_back(i->first);
				poses->push_back(i->second.pose());
			}
	}
	else
	{
		for (PoseMap::const_iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
			if ((tMatchPose && i->second.pose() == referencePose) || (!tMatchPose && i->second.pose() != referencePose))
				poseIds.push_back(i->first);
	}

	return poseIds;
}

template <bool tThreadSafe>
inline bool Database::poseBorders(Index32& lowerPoseId, Index32& upperPoseId) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	if (databasePoseMap.empty())
		return false;

	lowerPoseId = databasePoseMap.begin()->first;
	upperPoseId = databasePoseMap.rbegin()->first;

	return true;
}

template <bool tThreadSafe>
inline bool Database::validPoseBorders(Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	if (databasePoseMap.empty())
		return false;

	rangeLowerPoseId = invalidId;
	rangeUpperPoseId = invalidId;

	for (PoseMap::const_iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
		if (i->second.pose().isValid())
		{
			rangeLowerPoseId = i->first;
			break;
		}

	if (rangeLowerPoseId == invalidId)
		return false;

	for (PoseMap::const_reverse_iterator i = databasePoseMap.rbegin(); i != databasePoseMap.rend(); ++i)
		if (i->second.pose().isValid())
		{
			rangeUpperPoseId = i->first;
			break;
		}

	ocean_assert(rangeLowerPoseId <= rangeUpperPoseId);
	return true;
}

template <bool tThreadSafe>
inline bool Database::validPoseRange(const Index32 lowerPoseId, const Index32 startPoseId, const Index32 upperPoseId, Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const
{
	ocean_assert(startPoseId != invalidId);
	ocean_assert_and_suppress_unused(lowerPoseId <= startPoseId && startPoseId <= upperPoseId, lowerPoseId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	PoseMap::const_iterator i = databasePoseMap.find(startPoseId);
	if (i == databasePoseMap.end() || !(i->second.pose().isValid()))
		return false;

	rangeLowerPoseId = startPoseId;
	rangeUpperPoseId = startPoseId;

	for (unsigned int id = startPoseId - 1; id != (unsigned int)(-1); --id)
	{
		i = databasePoseMap.find(id);

		if (i == databasePoseMap.end() || !(i->second.pose().isValid()))
			break;

		rangeLowerPoseId = id;
	}

	for (unsigned int id = startPoseId + 1u; id <= upperPoseId; ++id)
	{
		i = databasePoseMap.find(id);

		if (i == databasePoseMap.end() || !(i->second.pose().isValid()))
			break;

		rangeUpperPoseId = id;
	}

	return true;
}

template <bool tThreadSafe>
inline bool Database::largestValidPoseRange(const Index32 lowerPoseId, const Index32 upperPoseId, Index32& rangeLowerPoseId, Index32& rangeUpperPoseId) const
{
	ocean_assert(lowerPoseId <= upperPoseId);

	if (lowerPoseId > upperPoseId)
		return false;

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const HomogenousMatrices4 rangePoses(poses<false>(lowerPoseId, upperPoseId));

	unsigned int bestRangeSize = 0u;
	unsigned int firstIndex = (unsigned int)(-1);

	for (unsigned int n = 0u; n < rangePoses.size(); ++n)
		if (firstIndex == (unsigned int)(-1))
		{
			if (rangePoses[n].isValid())
				firstIndex = n;
		}
		else if (!rangePoses[n].isValid())
		{
			const unsigned int lastIndex = n - 1u;
			ocean_assert(firstIndex >= 0u && lastIndex < (unsigned int)rangePoses.size());

			const unsigned int rangeSize = lastIndex - firstIndex + 1u;

			if (rangeSize > bestRangeSize)
			{
				bestRangeSize = rangeSize;
				rangeLowerPoseId = firstIndex + lowerPoseId;
				rangeUpperPoseId = lastIndex + lowerPoseId;

				// check whether the remaining part is too small to be larger than the currently best range
				if (rangePoses.size() - n < bestRangeSize)
					return true;
			}

			firstIndex = (unsigned int)(-1);
		}

	if (firstIndex == (unsigned int)(-1))
		return bestRangeSize != 0u;

	const unsigned int lastIndex =  (unsigned int)rangePoses.size() - 1u;
	ocean_assert(firstIndex >= 0u && lastIndex < (unsigned int)rangePoses.size());

	const unsigned int rangeSize = lastIndex - firstIndex + 1u;
	if (rangeSize > bestRangeSize)
	{
		rangeLowerPoseId = firstIndex + lowerPoseId;
		rangeUpperPoseId = lastIndex + lowerPoseId;

		bestRangeSize = rangeSize;
	}

	ocean_assert(bestRangeSize != 0u);
	return true;
}

template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
inline bool Database::poseWithMostCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, Index32* poseId, unsigned int* correspondences, const Vector3& referenceObjectPoint) const
{
	ocean_assert(lowerPoseId != invalidId && upperPoseId != invalidId);
	ocean_assert(lowerPoseId <= upperPoseId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Index32 bestPoseId = invalidId;
	unsigned int bestCorrespondences = 0u;

	for (unsigned int id = lowerPoseId; id <= upperPoseId; ++id)
	{
		const unsigned int value = numberCorrespondences<false, tMatchPosition, tNeedValidPose>(id, referenceObjectPoint);
		if (value > bestCorrespondences)
		{
			bestCorrespondences = value;
			bestPoseId = id;
		}
	}

	if (poseId)
		*poseId = bestPoseId;

	if (correspondences)
		*correspondences = bestCorrespondences;

	return bestCorrespondences != 0u;
}

template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
inline bool Database::poseWithLeastCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, Index32* poseId, unsigned int* correspondences, const Vector3& referenceObjectPoint) const
{
	ocean_assert(lowerPoseId != invalidId && upperPoseId != invalidId);
	ocean_assert(lowerPoseId <= upperPoseId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Index32 worstPoseId = invalidId;
	unsigned int worstCorrespondences = (unsigned int)(-1);

	HomogenousMatrix4 pose;

	for (unsigned int id = lowerPoseId; id <= upperPoseId; ++id)
	{
		if (tNeedValidPose && (!hasPose<false>(id, &pose) || !pose.isValid()))
			continue;

		const unsigned int value = numberCorrespondences<false, tMatchPosition, false>(id, referenceObjectPoint);
		if (value < worstCorrespondences)
		{
			worstCorrespondences = value;
			worstPoseId = id;
		}
	}

	if (poseId)
		*poseId = worstPoseId;

	if (correspondences)
		*correspondences = worstCorrespondences;

	return worstCorrespondences != (unsigned int)(-1);
}

template <bool tThreadSafe>
inline bool Database::poseWithMostObservations(const IndexSet32& poseCandidates, const IndexSet32& majorObjectPointIds, const IndexSet32& minorObjectPointIds, Index32& pose, Indices32* visibleMajorObjectPointIds, Indices32* visibleMinorObjectPointIds) const
{
	ocean_assert(!poseCandidates.empty());
	ocean_assert(!majorObjectPointIds.empty());

	if (majorObjectPointIds.empty())
		return false;

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	unsigned int bestMajorCount = 0u;
	unsigned int bestMinorCount = 0u;

	Index32 bestPoseId = invalidId;

	for (IndexSet32::const_iterator iP = poseCandidates.begin(); iP != poseCandidates.end(); ++iP)
	{
		const Index32 poseId = *iP;

		unsigned int majorCount = 0u;
		unsigned int remaining = (unsigned int)majorObjectPointIds.size();

		for (IndexSet32::const_iterator i = majorObjectPointIds.begin(); majorCount + remaining >= bestMajorCount && i != majorObjectPointIds.end(); ++i)
		{
			if (databasePoseObjectPointMap.find(index64(poseId, *i)) != databasePoseObjectPointMap.end())
				majorCount++;

			remaining--;
		}

		if (majorCount >= bestMajorCount)
		{
			unsigned int minorCount = 0u;
			remaining = (unsigned int)minorObjectPointIds.size();

			for (IndexSet32::const_iterator i = minorObjectPointIds.begin(); minorCount + remaining >= bestMinorCount && i != minorObjectPointIds.end(); ++i)
			{
				if (databasePoseObjectPointMap.find(index64(poseId, *i)) != databasePoseObjectPointMap.end())
					minorCount++;

				remaining--;
			}

			if (majorCount > bestMajorCount || minorCount > bestMinorCount)
			{
				bestPoseId = poseId;
				bestMajorCount = majorCount;
				bestMinorCount = minorCount;
			}
		}
	}

	if (bestPoseId == invalidId)
		return false;

	pose = bestPoseId;

	if (visibleMajorObjectPointIds)
	{
		ocean_assert(visibleMajorObjectPointIds->empty());
		visibleMajorObjectPointIds->clear();
		visibleMajorObjectPointIds->reserve(bestMajorCount);

		for (IndexSet32::const_iterator i = majorObjectPointIds.begin(); i != majorObjectPointIds.end(); ++i)
			if (databasePoseObjectPointMap.find(index64(bestPoseId, *i)) != databasePoseObjectPointMap.end())
				visibleMajorObjectPointIds->push_back(*i);

		ocean_assert(bestMajorCount == visibleMajorObjectPointIds->size());
	}

	if (visibleMinorObjectPointIds)
	{
		ocean_assert(visibleMinorObjectPointIds->empty());
		visibleMinorObjectPointIds->clear();
		visibleMinorObjectPointIds->reserve(minorObjectPointIds.size());

		for (IndexSet32::const_iterator i = minorObjectPointIds.begin(); i != minorObjectPointIds.end(); ++i)
			if (databasePoseObjectPointMap.find(index64(bestPoseId, *i)) != databasePoseObjectPointMap.end())
				visibleMinorObjectPointIds->push_back(*i);

		ocean_assert(bestMinorCount == visibleMinorObjectPointIds->size());
	}

	return true;
}

template <bool tThreadSafe>
inline unsigned int Database::numberObservations(const Index32 poseId, const Indices32& objectPointIds) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	unsigned int number = 0u;

	for (Indices32::const_iterator i = objectPointIds.begin(); i != objectPointIds.end(); ++i)
		if (databasePoseObjectPointMap.find(index64(poseId, *i)) != databasePoseObjectPointMap.end())
			number++;

	return number;
}

template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
inline unsigned int Database::numberCorrespondences(const Index32 poseId, const Vector3& referenceObjectPoint, const Scalar minimalPriority) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	if (iP == databasePoseMap.end() || (tNeedValidPose && !(iP->second.pose().isValid())))
		return 0u;

	unsigned int count = 0u;

	const IndexSet32& imagePointIds = iP->second.imagePointIds();
	for (IndexSet32::const_iterator iI = imagePointIds.begin(); iI != imagePointIds.end(); ++iI)
	{
		const ImagePointMap::const_iterator i = databaseImagePointMap.find(*iI);
		ocean_assert(i != databaseImagePointMap.end());

		const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(i->second.objectPointId());
		ocean_assert(iO != databaseObjectPointMap.end());

		if (iO->second.priority() >= minimalPriority && ((tMatchPosition && iO->second.point() == referenceObjectPoint) || (!tMatchPosition && iO->second.point() != referenceObjectPoint)))
			count++;
	}

	return count;
}

template <bool tThreadSafe, bool tMatchPosition, bool tNeedValidPose>
inline Indices32 Database::numberCorrespondences(const Index32 lowerPoseId, const Index32 upperPoseId, const Vector3& referenceObjectPoint, const Scalar minimalPriority, Worker* worker) const
{
	ocean_assert(lowerPoseId <= upperPoseId);

	const unsigned int frames = upperPoseId - lowerPoseId + 1u;

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 result;

	if (worker && frames >= 20u)
	{
		result.resize(frames);

		worker->executeFunction(Worker::Function::create(*this, &Database::numberCorrespondencesSubset<tMatchPosition, tNeedValidPose>, lowerPoseId, &referenceObjectPoint, minimalPriority, result.data(), 0u, 0u), 0u, frames);
	}
	else
	{
		result.reserve(frames);

		for (unsigned int n = lowerPoseId; n <= upperPoseId; ++n)
			result.push_back(numberCorrespondences<false, tMatchPosition, tNeedValidPose>(n, referenceObjectPoint, minimalPriority));
	}

	return result;
}

template <bool tThreadSafe>
inline bool Database::hasImagePoint(const Index32 imagePointId, Vector2* imagePoint) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ImagePointMap::const_iterator i = databaseImagePointMap.find(imagePointId);

	if (i == databaseImagePointMap.end())
		return false;

	if (imagePoint)
		*imagePoint = i->second.point();

	return true;
}

template <bool tThreadSafe>
inline Index32 Database::addImagePoint(const Vector2& imagePoint)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	databaseImagePointMap.insert(std::make_pair(++databaseImagePointIdCounter, ImagePointData(imagePoint)));
	return databaseImagePointIdCounter;
}

template <bool tThreadSafe>
inline void Database::removeImagePoint(const Index32 imagePointId)
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ImagePointMap::iterator i = databaseImagePointMap.find(imagePointId);
	ocean_assert(i != databaseImagePointMap.end());

	// we need to remove all connections of the specified image point
	const ImagePointData& data = i->second;

	if (data.poseId() != invalidId)
	{
		PoseMap::iterator iP = databasePoseMap.find(data.poseId());
		ocean_assert(iP != databasePoseMap.end());

		iP->second.unregisterImagePoint(imagePointId);
	}

	if (data.objectPointId() != invalidId)
	{
		ObjectPointMap::iterator iO = databaseObjectPointMap.find(data.objectPointId());
		ocean_assert(iO != databaseObjectPointMap.end());

		iO->second.unregisterImagePoint(imagePointId);
	}

	databaseImagePointMap.erase(i);
}

template <bool tThreadSafe>
inline bool Database::hasObjectPoint(const Index32 objectPointId, Vector3* objectPoint) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator i = databaseObjectPointMap.find(objectPointId);

	if (i == databaseObjectPointMap.end())
		return false;

	if (objectPoint)
		*objectPoint = i->second.point();

	return true;
}

template <bool tThreadSafe>
inline Index32 Database::addObjectPoint(const Vector3& objectPoint, const Scalar priority)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseObjectPointMap.find(databaseObjectPointIdCounter + 1u) == databaseObjectPointMap.end() && "You mixed calls with the add-objectPoint-function using external object point ids!");

	databaseObjectPointMap.insert(std::make_pair(++databaseObjectPointIdCounter, ObjectPointData(objectPoint, priority)));
	return databaseObjectPointIdCounter;
}

template <bool tThreadSafe>
inline void Database::addObjectPoint(const Index32 objectPointId, const Vector3& objectPoint, const Scalar priority)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseObjectPointMap.find(objectPointId) == databaseObjectPointMap.end());
	ocean_assert((databaseObjectPointIdCounter == invalidId || objectPointId + 1u <= databaseObjectPointIdCounter) && "You mixed calls with the add-objectPoint-function using external object point ids!");

	databaseObjectPointMap[objectPointId] = ObjectPointData(objectPoint, priority);
}

inline Index32 Database::addObjectPointFromDatabase(const Database& secondDatabase, const Index32 secondDatabaseObjectPointId, const SquareMatrix3& imagePointTransformation, const Index32 newObjectPointId, const Index32 secondDatabaseLowerPoseId, const Index32 secondDatabaseUpperPoseId, const bool forExistingPosesOnly)
{
	ocean_assert(secondDatabase.hasObjectPoint<false>(secondDatabaseObjectPointId));
	ocean_assert(!imagePointTransformation.isSingular());
	ocean_assert(secondDatabaseLowerPoseId == invalidId || secondDatabaseUpperPoseId == invalidId || secondDatabaseLowerPoseId <= secondDatabaseUpperPoseId);

	// first we copy the location of the 3D object point

	Scalar objectPointPriority = Scalar(-1);
	const Vector3& objectPoint = secondDatabase.objectPoint<false>(secondDatabaseObjectPointId, objectPointPriority);

	Index32 thisDatabaseObjectPointId = invalidId;

	// we want to ensure that an explicit id of the new object point does not exist in this database
	ocean_assert(newObjectPointId == invalidId || hasObjectPoint<false>(newObjectPointId) == false);
	if (newObjectPointId != invalidId)
	{
		if (hasObjectPoint<false>(newObjectPointId))
		{
			return invalidId;
		}

		addObjectPoint<false>(newObjectPointId, objectPoint, objectPointPriority);
		thisDatabaseObjectPointId = newObjectPointId;
	}
	else
	{
		thisDatabaseObjectPointId = addObjectPoint<false>(objectPoint, objectPointPriority);
	}

	// now we have add the corresponding image points (and ensure that a pose exists in this database)

	const IndexSet32& secondDatabaseImagePointIds = secondDatabase.imagePointsFromObjectPoint<false>(secondDatabaseObjectPointId);

	for (IndexSet32::const_iterator i = secondDatabaseImagePointIds.cbegin(); i != secondDatabaseImagePointIds.cend(); ++i)
	{
		const Index32& secondDatabaseImagePointId = *i;

		const Index32 poseId = secondDatabase.poseFromImagePoint<false>(secondDatabaseImagePointId);

		// the pose id in the second database is identical to the pose id in this database

		// we check whether the user had specified a pose range

		if ((secondDatabaseLowerPoseId != invalidId && poseId < secondDatabaseLowerPoseId)
				|| (secondDatabaseUpperPoseId != invalidId && poseId > secondDatabaseUpperPoseId))
		{
			// the pose id is outside the specified pose range, so we skip this image point (this observation)
			continue;
		}

		if (!hasPose<false>(poseId))
		{
			if (forExistingPosesOnly)
			{
				// the user does not want us to create a new pose, so we simply skip this image point (this observation)
				continue;
			}

			// we need to create a pose in this database

			const HomogenousMatrix4& pose = secondDatabase.pose<false>(poseId);

			const bool addPoseResult = addPose<false>(poseId, pose);
			ocean_assert_and_suppress_unused(addPoseResult, addPoseResult);
		}

		// now, as we know that the pose exists in this database, we simply add the image point and register/connect it with the pose

		const Vector2& imagePoint = secondDatabase.imagePoint<false>(secondDatabaseImagePointId);

		// we apply the provided transformation before adding the image point to this database

		const Index32 thisDatabaseImagePointId = addImagePoint<false>(imagePointTransformation * imagePoint);

		attachImagePointToObjectPoint<false>(thisDatabaseImagePointId, thisDatabaseObjectPointId);
		attachImagePointToPose<false>(thisDatabaseImagePointId, poseId);
	}

	return thisDatabaseObjectPointId;
}

template <bool tThreadSafe>
inline void Database::removeObjectPoint(const Index32 objectPointId)
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.end());

	// we need to remove all connections of the specified object point

	const ObjectPointData& data = i->second;

	for (IndexSet32::const_iterator iI = data.imagePointIds().begin(); iI != data.imagePointIds().end(); ++iI)
	{
		ImagePointMap::iterator iP = databaseImagePointMap.find(*iI);
		ocean_assert(iP != databaseImagePointMap.end());

		if (iP->second.poseId() != invalidId)
		{
			ocean_assert(databasePoseObjectPointMap.find(index64(iP->second.poseId(), objectPointId)) != databasePoseObjectPointMap.end());
			databasePoseObjectPointMap.erase(index64(iP->second.poseId(), objectPointId));
		}

		iP->second.setObjectPointId(invalidId);
	}

	databaseObjectPointMap.erase(i);
}

template <bool tThreadSafe>
void Database::removeObjectPointAndAttachedImagePoints(const Index32 objectPointId)
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::iterator iObjectPoint = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iObjectPoint != databaseObjectPointMap.cend());

	// we need to remove all connections of the specified object point

	const ObjectPointData& objectPointData = iObjectPoint->second;

	for (const Index32& imagePointId : objectPointData.imagePointIds())
	{
		const ImagePointMap::iterator iImagePoint = databaseImagePointMap.find(imagePointId);
		ocean_assert(iImagePoint != databaseImagePointMap.cend());

		if (iImagePoint->second.poseId() != invalidId)
		{
			ocean_assert(databasePoseObjectPointMap.find(index64(iImagePoint->second.poseId(), objectPointId)) != databasePoseObjectPointMap.cend());
			databasePoseObjectPointMap.erase(index64(iImagePoint->second.poseId(), objectPointId));
		}

		const Index32 poseId = iImagePoint->second.poseId();

		PoseMap::iterator iPose = databasePoseMap.find(poseId);
		ocean_assert(iPose != databasePoseMap.cend());

		iPose->second.unregisterImagePoint(imagePointId);

		databaseImagePointMap.erase(iImagePoint);
	}

	databaseObjectPointMap.erase(iObjectPoint);
}

template <bool tThreadSafe>
inline void Database::renameObjectPoint(const Index32 oldObjectPointId, const Index32 newObjectPointId)
{
	ocean_assert(oldObjectPointId != invalidId && newObjectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ocean_assert(databaseObjectPointMap.find(newObjectPointId) == databaseObjectPointMap.end());

	ObjectPointMap::iterator iOld = databaseObjectPointMap.find(oldObjectPointId);
	ocean_assert(iOld != databaseObjectPointMap.end());

	const IndexSet32& imagePointIds = iOld->second.imagePointIds();

	for (IndexSet32::const_iterator iI = imagePointIds.begin(); iI != imagePointIds.end(); ++iI)
	{
		ImagePointMap::iterator iIData = databaseImagePointMap.find(*iI);
		ocean_assert(iIData != databaseImagePointMap.end());

		ocean_assert(iIData->second.objectPointId() == oldObjectPointId);
		iIData->second.setObjectPointId(newObjectPointId);

		ocean_assert(databasePoseObjectPointMap.find(index64(iIData->second.poseId(), oldObjectPointId)) != databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.erase(index64(iIData->second.poseId(), oldObjectPointId));

		ocean_assert(databasePoseObjectPointMap.find(index64(iIData->second.poseId(), newObjectPointId)) == databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.insert(std::make_pair(index64(iIData->second.poseId(), newObjectPointId), newObjectPointId));
	}

	databaseObjectPointMap.insert(std::make_pair(newObjectPointId, std::move(iOld->second)));
	databaseObjectPointMap.erase(iOld);
}

template <bool tThreadSafe>
inline void Database::mergeObjectPoints(const Index32 remainingObjectPointId, const Index32 removingObjectPointId, const Vector3& newPoint, const Scalar newPriority)
{
	ocean_assert(remainingObjectPointId != invalidId && removingObjectPointId != invalidId && remainingObjectPointId != removingObjectPointId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ObjectPointMap::iterator iObjectPointRemaining = databaseObjectPointMap.find(remainingObjectPointId);
	ocean_assert(iObjectPointRemaining != databaseObjectPointMap.cend());

	ObjectPointMap::const_iterator iObjectPointRemoving = databaseObjectPointMap.find(removingObjectPointId);
	ocean_assert(iObjectPointRemoving != databaseObjectPointMap.cend());

#ifdef OCEAN_DEBUG
	const IndexSet32 debugPoseIdsRemaining = posesFromObjectPoint<false>(remainingObjectPointId);
	const IndexSet32 debugPoseIdsRemoving = posesFromObjectPoint<false>(removingObjectPointId);
	ocean_assert(!Subset::hasIntersectingElement(debugPoseIdsRemaining, debugPoseIdsRemoving));
#endif

	for (const Index32& imagePointIdRemoving : iObjectPointRemoving->second.imagePointIds())
	{
		iObjectPointRemaining->second.registerImagePoint(imagePointIdRemoving);

		ImagePointMap::iterator iImagePointRemoving = databaseImagePointMap.find(imagePointIdRemoving);
		ocean_assert(iImagePointRemoving != databaseImagePointMap.cend());

		const Index32 poseIdRemoving = iImagePointRemoving->second.poseId();

		ocean_assert(databasePoseObjectPointMap.find(index64(poseIdRemoving, removingObjectPointId)) != databasePoseObjectPointMap.cend());
		databasePoseObjectPointMap.erase(index64(poseIdRemoving, removingObjectPointId));

		ocean_assert(databasePoseObjectPointMap.find(index64(poseIdRemoving, remainingObjectPointId)) == databasePoseObjectPointMap.cend());
		databasePoseObjectPointMap.emplace(index64(poseIdRemoving, remainingObjectPointId), imagePointIdRemoving);

		iImagePointRemoving->second.setObjectPointId(remainingObjectPointId);
	}

	iObjectPointRemaining->second.setPoint(newPoint);
	iObjectPointRemaining->second.setPriority(newPriority);

	databaseObjectPointMap.erase(iObjectPointRemoving);
}

template <bool tThreadSafe>
inline bool Database::hasPose(const Index32 poseId, HomogenousMatrix4* pose) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator i = databasePoseMap.find(poseId);
	if (i == databasePoseMap.end())
		return false;

	if (pose)
		*pose = i->second.pose();

	return true;
}

template <bool tThreadSafe>
inline bool Database::addPose(const Index32 poseId, const HomogenousMatrix4& pose)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator i = databasePoseMap.find(poseId);
	if (i != databasePoseMap.end())
	{
		ocean_assert(false && "Invalid pose id!");
		return false;
	}

	databasePoseMap.insert(std::make_pair(poseId, PoseData(pose)));

	databasePoses = max(databasePoses, poseId + 1u);

	return true;
}

template <bool tThreadSafe>
inline void Database::removePose(const Index32 poseId)
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::iterator i = databasePoseMap.find(poseId);
	ocean_assert(i != databasePoseMap.end());

	// we need to remove all connections of the specified pose

	const PoseData& data = i->second;

	for (IndexSet32::const_iterator iI = data.imagePointIds().begin(); iI != data.imagePointIds().end(); ++iI)
	{
		ImagePointMap::iterator iP = databaseImagePointMap.find(*iI);
		ocean_assert(iP != databaseImagePointMap.end());

		iP->second.setPoseId(invalidId);
	}

	databasePoseMap.erase(i);
}

template <bool tThreadSafe>
inline Index32 Database::poseFromImagePoint(const Index32 imagePointId) const
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ImagePointMap::const_iterator i = databaseImagePointMap.find(imagePointId);
	ocean_assert(i != databaseImagePointMap.end());

	return i->second.poseId();
}

template <bool tThreadSafe>
inline size_t Database::numberImagePointsFromObjectPoint(const Index32 objectPointId) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iO != databaseObjectPointMap.end());

	const IndexSet32& ids = iO->second.imagePointIds();

	return ids.size();
}

template <bool tThreadSafe>
inline void Database::observationsFromObjectPoint(const Index32 objectPointId, Indices32& poseIds, Indices32& imagePointIds, Vectors2* imagePoints) const
{
	ocean_assert(objectPointId != invalidId);
	ocean_assert(poseIds.empty() && imagePointIds.empty());
	ocean_assert(imagePoints == nullptr || imagePoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iO != databaseObjectPointMap.end());

	const IndexSet32& ids = iO->second.imagePointIds();

	poseIds.reserve(ids.size());
	imagePointIds.reserve(ids.size());

	if (imagePoints)
		imagePoints->reserve(ids.size());

	for (IndexSet32::const_iterator i = ids.begin(); i != ids.end(); ++i)
	{
		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		if (iI->second.poseId() != invalidId)
		{
			poseIds.push_back(iI->second.poseId());
			imagePointIds.push_back(*i);

			if (imagePoints)
				imagePoints->push_back(iI->second.point());
		}
	}
}

template <bool tThreadSafe>
inline void Database::observationsFromObjectPoint(const Index32 objectPointId, const Indices32& poseIdCandidates, Indices32& validPoseIndices, Indices32* imagePointIds, Vectors2* imagePoints) const
{
	ocean_assert(objectPointId != invalidId);
	ocean_assert(!poseIdCandidates.empty() && validPoseIndices.empty());

	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());
	ocean_assert(imagePoints == nullptr || imagePoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (size_t n = 0; n < poseIdCandidates.size(); ++n)
	{
		const Index32 poseId = poseIdCandidates[n];

		const Index64To32Map::const_iterator i = databasePoseObjectPointMap.find(index64(poseId, objectPointId));

		if (i != databasePoseObjectPointMap.end())
		{
			validPoseIndices.push_back((unsigned int)n);

			if (imagePointIds)
				imagePointIds->push_back(i->second);

			if (imagePoints)
			{
				ocean_assert(databaseImagePointMap.find(i->second) != databaseImagePointMap.end());
				imagePoints->push_back(databaseImagePointMap.find(i->second)->second.point());
			}
		}
	}
}

template <bool tThreadSafe>
inline Index32 Database::objectPointFromImagePoint(const Index32 imagePointId) const
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ImagePointMap::const_iterator i = databaseImagePointMap.find(imagePointId);
	ocean_assert(i != databaseImagePointMap.end());

	return i->second.objectPointId();
}

template <bool tThreadSafe>
inline const IndexSet32& Database::imagePointsFromPose(const Index32 poseId) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator i = databasePoseMap.find(poseId);
	ocean_assert(i != databasePoseMap.end());

	return i->second.imagePointIds();
}

template <bool tThreadSafe>
inline const IndexSet32& Database::imagePointsFromObjectPoint(const Index32 objectPointId) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.cend());

	return i->second.imagePointIds();
}

template <bool tThreadSafe>
inline IndexSet32 Database::posesFromObjectPoint(const Index32 objectPointId) const
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator iObjectPoint = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iObjectPoint != databaseObjectPointMap.cend());

	IndexSet32 result;

	for (const Index32& imagePointId : iObjectPoint->second.imagePointIds())
	{
		const ImagePointMap::const_iterator iImagePoint = databaseImagePointMap.find(imagePointId);
		ocean_assert(iImagePoint != databaseImagePointMap.cend());

		result.emplace(iImagePoint->second.poseId());
	}

	return result;
}

template <bool tThreadSafe>
inline void Database::attachImagePointToObjectPoint(const Index32 imagePointId, const Index32 objectPointId)
{
	ocean_assert(imagePointId != invalidId && objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ImagePointMap::iterator iI = databaseImagePointMap.find(imagePointId);
	ocean_assert(iI != databaseImagePointMap.end());
	ocean_assert(iI->second.objectPointId() == invalidId);

	iI->second.setObjectPointId(objectPointId);

	ObjectPointMap::iterator iO = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iO != databaseObjectPointMap.end());

	iO->second.registerImagePoint(imagePointId);

	if (iI->second.poseId() != invalidId)
	{
		const Index64 poseObjectPointId(index64(iI->second.poseId(), objectPointId));

		ocean_assert(databasePoseObjectPointMap.find(poseObjectPointId) == databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.insert(std::make_pair(poseObjectPointId, imagePointId));
	}
}

template <bool tThreadSafe>
inline void Database::detachImagePointFromObjectPoint(const Index32 imagePointId)
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ImagePointMap::iterator iI = databaseImagePointMap.find(imagePointId);
	ocean_assert(iI != databaseImagePointMap.end());

	const Index32 objectPointId = iI->second.objectPointId();
	ocean_assert(objectPointId != invalidId);

	iI->second.setObjectPointId(invalidId);

	ObjectPointMap::iterator iO = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iO != databaseObjectPointMap.end());

	iO->second.unregisterImagePoint(imagePointId);

	if (iI->second.poseId() != invalidId)
	{
		const Index64 poseObjectPointId(index64(iI->second.poseId(), objectPointId));

		ocean_assert(databasePoseObjectPointMap.find(poseObjectPointId) != databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.erase(poseObjectPointId);
	}
}

template <bool tThreadSafe>
inline void Database::attachImagePointToPose(const Index32 imagePointId, const Index32 poseId)
{
	ocean_assert(imagePointId != invalidId && poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ImagePointMap::iterator iI = databaseImagePointMap.find(imagePointId);
	ocean_assert(iI != databaseImagePointMap.end());
	ocean_assert(iI->second.poseId() == invalidId);

	iI->second.setPoseId(poseId);

	PoseMap::iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	iP->second.registerImagePoint(imagePointId);

	if (iI->second.objectPointId() != invalidId)
	{
		const Index64 poseObjectPointId(index64(poseId, iI->second.objectPointId()));

		ocean_assert(databasePoseObjectPointMap.find(poseObjectPointId) == databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.insert(std::make_pair(poseObjectPointId, imagePointId));
	}
}

template <bool tThreadSafe>
inline void Database::detachImagePointFromPose(const Index32 imagePointId)
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ImagePointMap::iterator iI = databaseImagePointMap.find(imagePointId);
	ocean_assert(iI != databaseImagePointMap.end());

	const Index32 poseId = iI->second.poseId();
	ocean_assert(poseId != invalidId);

	iI->second.setPoseId(invalidId);

	PoseMap::iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	iP->second.unregisterImagePoint(imagePointId);

	if (iI->second.objectPointId() != invalidId)
	{
		const Index64 poseObjectPointId(index64(poseId, iI->second.objectPointId()));

		ocean_assert(databasePoseObjectPointMap.find(poseObjectPointId) != databasePoseObjectPointMap.end());
		databasePoseObjectPointMap.erase(poseObjectPointId);
	}
}

template <bool tThreadSafe>
inline void Database::setImagePoint(const Index32 imagePointId, const Vector2& imagePoint)
{
	ocean_assert(imagePointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ImagePointMap::iterator i = databaseImagePointMap.find(imagePointId);
	ocean_assert(i != databaseImagePointMap.end());

	i->second.setPoint(imagePoint);
}

template <bool tThreadSafe>
inline void Database::setObjectPoint(const Index32 objectPointId, const Vector3& objectPoint)
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.end());

	i->second.setPoint(objectPoint);
}

template <bool tThreadSafe>
inline void Database::setObjectPoints(const Index32* objectPointIds, const Vector3* objectPoints, const size_t number)
{
	ocean_assert(objectPointIds && objectPoints);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (size_t n = 0; n < number; ++ n)
	{
		const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointIds[n]);
		ocean_assert(i != databaseObjectPointMap.end());

		i->second.setPoint(objectPoints[n]);
	}
}

template <bool tThreadSafe>
inline void Database::setObjectPoints(const Index32* objectPointIds, const size_t number, const Vector3& referenceObjectPoint)
{
	ocean_assert(objectPointIds);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (size_t n = 0; n < number; ++ n)
	{
		const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointIds[n]);
		ocean_assert(i != databaseObjectPointMap.end());

		i->second.setPoint(referenceObjectPoint);
	}
}

template <bool tThreadSafe>
inline void Database::setObjectPoints(const Vector3& objectPoint)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (ObjectPointMap::iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
		i->second.setPoint(objectPoint);
}

template <bool tThreadSafe>
inline void Database::setObjectPoint(const Index32 objectPointId, const Vector3& objectPoint, const Scalar priority)
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.end());

	i->second.setPoint(objectPoint);
	i->second.setPriority(priority);
}

template <bool tThreadSafe>
inline void Database::setObjectPointPriority(const Index32 objectPointId, const Scalar priority)
{
	ocean_assert(objectPointId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::iterator i = databaseObjectPointMap.find(objectPointId);
	ocean_assert(i != databaseObjectPointMap.end());

	i->second.setPriority(priority);
}

template <bool tThreadSafe>
inline void Database::setPose(const Index32 poseId, const HomogenousMatrix4& pose)
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::iterator i = databasePoseMap.find(poseId);
	ocean_assert(i != databasePoseMap.end());

	i->second.setPose(pose);
}

template <bool tThreadSafe>
inline void Database::setPoses(const Index32* poseIds, const HomogenousMatrix4* poses, const size_t number)
{
	ocean_assert(poseIds && poses);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (size_t n = 0; n < number; ++n)
	{
		const PoseMap::iterator i = databasePoseMap.find(poseIds[n]);
		ocean_assert(i != databasePoseMap.end());

		i->second.setPose(poses[n]);
	}
}

template <bool tThreadSafe>
inline void Database::setPoses(const ShiftVector<HomogenousMatrix4>& poses)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (ShiftVector<HomogenousMatrix4>::Index n = poses.firstIndex(); n < poses.endIndex(); ++n)
	{
		ocean_assert(n >= 0);
		const unsigned int poseId = (unsigned int)n;

		const PoseMap::iterator i = databasePoseMap.find(poseId);
		ocean_assert(i != databasePoseMap.end());

		i->second.setPose(poses[n]);
	}
}

template <bool tThreadSafe>
inline void Database::setPoses(const HomogenousMatrix4& pose)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (PoseMap::iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
		i->second.setPose(pose);
}

template <bool tThreadSafe>
const IndexSet32& Database::imagePointIds(const Index32 poseId) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator i = databasePoseMap.find(poseId);
	ocean_assert(i != databasePoseMap.end());

	return i->second.imagePointIds();
}

template <bool tThreadSafe>
inline Indices32 Database::imagePointIds(const Index32 poseId, Indices32& objectPointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(!objectPointIds.empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 ids;
	ids.reserve(objectPointIds.size());

	Indices32 validObjectPointIds;
	validObjectPointIds.reserve(objectPointIds.size());

	for (Indices32::const_iterator i = objectPointIds.begin(); i != objectPointIds.end(); ++i)
	{
		const Index64To32Map::const_iterator iPO = databasePoseObjectPointMap.find(index64(poseId, *i));

		if (iPO != databasePoseObjectPointMap.end())
		{
			ocean_assert(databaseImagePointMap.find(iPO->second) != databaseImagePointMap.end());

			ids.push_back(iPO->second);
			validObjectPointIds.push_back(*i);
		}
	}

	objectPointIds = std::move(validObjectPointIds);
	return ids;
}

template <bool tThreadSafe>
Indices32 Database::imagePointIds(Vectors2* imagePoints) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 result;
	result.reserve(databaseImagePointMap.size());

	if (imagePoints != nullptr)
	{
		for (ImagePointMap::const_iterator i = databaseImagePointMap.cbegin(); i != databaseImagePointMap.cend(); ++i)
		{
			result.emplace_back(i->first);

			imagePoints->emplace_back(i->second.point());
		}
	}
	else
	{
		for (ImagePointMap::const_iterator i = databaseImagePointMap.cbegin(); i != databaseImagePointMap.cend(); ++i)
		{
			result.emplace_back(i->first);
		}
	}

	return result;
}

template <bool tThreadSafe>
Indices32 Database::objectPointIds(Vectors3* objectPoints, Scalars* priorities) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 result;
	result.reserve(databaseObjectPointMap.size());

	if (objectPoints != nullptr)
	{
		objectPoints->clear();
		objectPoints->reserve(databaseObjectPointMap.size());

		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.cbegin(); i != databaseObjectPointMap.cend(); ++i)
		{
			result.emplace_back(i->first);

			objectPoints->emplace_back(i->second.point());
		}
	}
	else
	{
		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.cbegin(); i != databaseObjectPointMap.cend(); ++i)
		{
			result.emplace_back(i->first);
		}
	}

	if (priorities != nullptr)
	{
		priorities->clear();
		priorities->reserve(databaseObjectPointMap.size());

		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.cbegin(); i != databaseObjectPointMap.cend(); ++i)
		{
			priorities->emplace_back(i->second.priority());
		}
	}

	return result;
}

template <bool tThreadSafe>
Indices32 Database::objectPointIds(const IndexSet32& outlierObjectPointIds) const
{
	if (outlierObjectPointIds.empty())
		return objectPointIds<tThreadSafe>();

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 result;
	result.reserve(databaseObjectPointMap.size());

	for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
		if (outlierObjectPointIds.find(i->first) == outlierObjectPointIds.end())
			result.push_back(i->first);

	return result;
}

template <bool tThreadSafe>
Indices32 Database::poseIds(HomogenousMatrices4* world_T_cameras) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 result;
	result.reserve(databasePoseMap.size());

	if (world_T_cameras != nullptr)
	{
		world_T_cameras->clear();
		world_T_cameras->reserve(databasePoseMap.size());

		for (PoseMap::const_iterator i = databasePoseMap.cbegin(); i != databasePoseMap.cend(); ++i)
		{
			result.emplace_back(i->first);

			world_T_cameras->emplace_back(i->second.pose());
		}
	}
	else
	{
		for (PoseMap::const_iterator i = databasePoseMap.cbegin(); i != databasePoseMap.cend(); ++i)
		{
			result.emplace_back(i->first);
		}
	}

	return result;
}

template <bool tThreadSafe>
Vectors2 Database::imagePoints(const Index32 poseId, Indices32* imagePointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	Vectors2 result;
	result.reserve(iP->second.imagePointIds().size());

	if (imagePointIds)
		imagePointIds->reserve(iP->second.imagePointIds().size());

	for (IndexSet32::const_iterator i = iP->second.imagePointIds().begin(); i != iP->second.imagePointIds().end(); ++i)
	{
		ocean_assert(*i != invalidId);

		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		result.push_back(iI->second.point());

		if (imagePointIds)
			imagePointIds->push_back(*i);
	}

	return result;
}

template <bool tThreadSafe, bool tMatchPosition>
Indices32 Database::objectPointIds(const Vector3& referencePosition, Vectors3* objectPoints, const Scalar minimalPriority) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 objectPointIds;

	if (objectPoints)
	{
		ocean_assert(objectPoints->empty());
		objectPoints->clear();

		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition)))
			{
				objectPointIds.push_back(i->first);
				objectPoints->push_back(i->second.point());
			}
	}
	else
	{
		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition)))
				objectPointIds.push_back(i->first);
	}

	return objectPointIds;
}

template <bool tThreadSafe, bool tMatchPosition>
Indices32 Database::objectPointIds(const IndexSet32& outlierObjectPointIds, const Vector3& referencePosition, Vectors3* objectPoints, const Scalar minimalPriority) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 objectPointIds;

	if (objectPoints)
	{
		ocean_assert(objectPoints->empty());
		objectPoints->clear();

		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition))
					&& outlierObjectPointIds.find(i->first) == outlierObjectPointIds.end())
			{
				objectPointIds.push_back(i->first);
				objectPoints->push_back(i->second.point());
			}
	}
	else
	{
		for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
			if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == referencePosition) || (!tMatchPosition && i->second.point() != referencePosition))
					&& outlierObjectPointIds.find(i->first) == outlierObjectPointIds.end())
				objectPointIds.push_back(i->first);
	}

	return objectPointIds;
}

template <bool tThreadSafe, bool tMatchPosition>
inline IndexPairs32 Database::objectPointIdsWithNumberOfObservations(const Vector3& referencePosition, const Scalar minimalPriority, Worker* worker) const
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Indices32 objectPointIds;
	objectPointIds.reserve(databaseObjectPointMap.size());

	for (ObjectPointMap::const_iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
		objectPointIds.push_back(i->first);

	IndexPairs32 result;
	result.reserve(objectPointIds.size());

	if (worker)
	{
		Lock lock;
		worker->executeFunction(Worker::Function::create(*this, &Database::objectPointIdsWithNumberOfObservationsSubset<tMatchPosition>, (const Index32*)objectPointIds.data(), &referencePosition, minimalPriority, &result, &lock, 0u, 0u), 0u, (unsigned int)objectPointIds.size());
	}
	else
		objectPointIdsWithNumberOfObservationsSubset<tMatchPosition>((const Index32*)objectPointIds.data(), &referencePosition, minimalPriority, &result, nullptr, 0u, (unsigned int)objectPointIds.size());

	return result;
}

template <bool tThreadSafe>
Indices32 Database::objectPointIds(const Index32 poseId, Vectors3* objectPoints) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(objectPoints == nullptr || objectPoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	const IndexSet32& imagePointIds = iP->second.imagePointIds();

	Indices32 result;
	result.reserve(imagePointIds.size());

	if (objectPoints)
		objectPoints->reserve(imagePointIds.size());

	for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
	{
		ocean_assert(*i != invalidId);

		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		const Index32 objectPointId = iI->second.objectPointId();

		if (objectPointId != invalidId)
		{
			result.push_back(objectPointId);

			if (objectPoints)
			{
				ocean_assert(databaseObjectPointMap.find(*i) != databaseObjectPointMap.end());
				objectPoints->push_back(databaseObjectPointMap.find(*i)->second.point());
			}
		}
	}

	ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());

	return result;
}

template <bool tThreadSafe, bool tMatchPosition>
Indices32 Database::objectPointIds(const Index32 poseId, const Vector3& referencePosition, const Scalar minimalPriority, Vectors3* objectPoints) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(objectPoints == nullptr || objectPoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	const IndexSet32& imagePointIds = iP->second.imagePointIds();

	Indices32 result;
	result.reserve(imagePointIds.size());

	if (objectPoints)
		objectPoints->reserve(imagePointIds.size());

	for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
	{
		ocean_assert(*i != invalidId);

		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		const Index32 objectPointId = iI->second.objectPointId();

		if (objectPointId != invalidId)
		{
			const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
			ocean_assert(iO != databaseObjectPointMap.end());

			const Vector3& objectPoint = iO->second.point();

			if (iO->second.priority() >= minimalPriority && ((tMatchPosition && objectPoint == referencePosition) || (!tMatchPosition && objectPoint != referencePosition)))
			{
				result.push_back(objectPointId);

				if (objectPoints)
					objectPoints->push_back(objectPoint);
			}
		}
	}

	ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());

	return result;
}

template <bool tThreadSafe>
Indices32 Database::objectPointIds(const Indices32 poseIds, Vectors3* objectPoints) const
{
	ocean_assert(!poseIds.empty());
	ocean_assert(objectPoints == nullptr || objectPoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	IndexSet32 objectPointIds;

	for (Indices32::const_iterator iP = poseIds.begin(); iP != poseIds.end(); ++iP)
	{
		const PoseMap::const_iterator iPM = databasePoseMap.find(*iP);
		ocean_assert(iPM != databasePoseMap.end());

		const IndexSet32& imagePointIds = iPM->second.imagePointIds();

		for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(*i != invalidId);

			const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
			ocean_assert(iI != databaseImagePointMap.end());

			const Index32 objectPointId = iI->second.objectPointId();

			if (objectPointId != invalidId)
				objectPointIds.insert(objectPointId);
		}
	}

	Indices32 result;

	if (objectPoints)
	{
		result.reserve(objectPointIds.size());
		objectPoints->reserve(objectPointIds.size());

		for (IndexSet32::const_iterator i = objectPointIds.begin(); i != objectPointIds.end(); ++i)
		{
			ocean_assert(databaseObjectPointMap.find(*i) != databaseObjectPointMap.end());

			result.push_back(*i);
			objectPoints->push_back(databaseObjectPointMap.find(*i)->second.point());
		}
	}
	else
		result = Indices32(objectPointIds.begin(), objectPointIds.end());

	return result;
}

template <bool tThreadSafe, bool tMatchPosition, bool tVisibleInAllPoses>
Indices32 Database::objectPointIds(const Index32 lowerPoseId, const Index32 upperPoseId, const Vector3& referencePosition, const Scalar minimalPriority, Vectors3* objectPoints) const
{
	ocean_assert(lowerPoseId <= upperPoseId);
	ocean_assert(objectPoints == nullptr || objectPoints->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	if constexpr (tVisibleInAllPoses)
	{
		Indices32 result;

		const PoseMap::const_iterator iPM = databasePoseMap.find(lowerPoseId);

		// if the lower pose does not exist the object points cannot be visible in all poses anymore
		if (iPM == databasePoseMap.end())
			return Indices32();

		const IndexSet32& imagePointIds = iPM->second.imagePointIds();

		for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(*i != invalidId);

			const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
			ocean_assert(iI != databaseImagePointMap.end());

			const Index32 objectPointId = iI->second.objectPointId();

			if (objectPointId != invalidId)
			{
				const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
				ocean_assert(iO != databaseObjectPointMap.end());

				if (iO->second.priority() >= minimalPriority && ((tMatchPosition && iO->second.point() == referencePosition) || (!tMatchPosition && iO->second.point() != referencePosition)))
				{
					bool visibleInAllPoses = true;
					for (unsigned int n = lowerPoseId + 1u; visibleInAllPoses && n <= upperPoseId; ++n)
						visibleInAllPoses = databasePoseObjectPointMap.find(index64(n, objectPointId)) != databasePoseObjectPointMap.end();

					if (visibleInAllPoses)
					{
						result.push_back(objectPointId);

						if (objectPoints)
							objectPoints->push_back(iO->second.point());
					}
				}
			}
		}

		ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());
		ocean_assert(objectPoints == nullptr || objectPoints->size() == result.size());

		return result;
	}
	else
	{
		Indices32 result;
		IndexSet32 objectPointIds;

		for (unsigned int n = lowerPoseId; n <= upperPoseId; ++n)
		{
			const PoseMap::const_iterator iPM = databasePoseMap.find(n);

			if (iPM != databasePoseMap.end())
			{
				const IndexSet32& imagePointIds = iPM->second.imagePointIds();

				for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
				{
					ocean_assert(*i != invalidId);

					const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
					ocean_assert(iI != databaseImagePointMap.end());

					const Index32 objectPointId = iI->second.objectPointId();

					if (objectPointId != invalidId && objectPointIds.find(objectPointId) == objectPointIds.end())
					{
						objectPointIds.insert(objectPointId);

						const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
						ocean_assert(iO != databaseObjectPointMap.end());

						if (iO->second.priority() >= minimalPriority && ((tMatchPosition && iO->second.point() == referencePosition) || (!tMatchPosition && iO->second.point() != referencePosition)))
						{
							result.push_back(objectPointId);

							if (objectPoints)
								objectPoints->push_back(iO->second.point());
						}
					}
				}
			}
		}

		ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());
		ocean_assert(objectPoints == nullptr || objectPoints->size() == result.size());

		return result;
	}
}

template <bool tThreadSafe, bool tMatchPosition, bool tVisibleInAllPoses>
Indices32 Database::objectPointIds(const Indices32& poseIds, const Vector3& referencePosition, const Scalar minimalPriority, Vectors3* objectPoints) const
{
	ocean_assert(Indices32(poseIds.begin(), poseIds.end()).size() == poseIds.size());
	ocean_assert(objectPoints == nullptr || objectPoints->empty());

	if (poseIds.empty())
		return Indices32();

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	if constexpr (tVisibleInAllPoses)
	{
		Indices32 result;

		const PoseMap::const_iterator iPM = databasePoseMap.find(poseIds.front());

		// if the first pose does not exist the object points cannot be visible in all poses anymore
		if (iPM == databasePoseMap.end())
			return Indices32();

		const IndexSet32& imagePointIds = iPM->second.imagePointIds();

		for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
		{
			ocean_assert(*i != invalidId);

			const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
			ocean_assert(iI != databaseImagePointMap.end());

			const Index32 objectPointId = iI->second.objectPointId();

			if (objectPointId != invalidId)
			{
				const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
				ocean_assert(iO != databaseObjectPointMap.end());

				if (iO->second.priority() >= minimalPriority && ((tMatchPosition && iO->second.point() == referencePosition) || (!tMatchPosition && iO->second.point() != referencePosition)))
				{
					bool visibleInAllPoses = true;
					for (size_t n = 1; n < poseIds.size(); ++n)
						visibleInAllPoses = databasePoseObjectPointMap.find(index64(poseIds[n], objectPointId)) != databasePoseObjectPointMap.end();

					if (visibleInAllPoses)
					{
						result.push_back(objectPointId);

						if (objectPoints)
							objectPoints->push_back(iO->second.point());
					}
				}
			}
		}

		ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());
		ocean_assert(objectPoints == nullptr || objectPoints->size() == result.size());

		return result;
	}
	else
	{
		Indices32 result;
		IndexSet32 objectPointIds;

		for (size_t n = 0; n < poseIds.size(); ++n)
		{
			const PoseMap::const_iterator iPM = databasePoseMap.find(poseIds[n]);

			if (iPM != databasePoseMap.end())
			{
				const IndexSet32& imagePointIds = iPM->second.imagePointIds();

				for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
				{
					ocean_assert(*i != invalidId);

					const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
					ocean_assert(iI != databaseImagePointMap.end());

					const Index32 objectPointId = iI->second.objectPointId();

					if (objectPointId != invalidId && objectPointIds.find(objectPointId) == objectPointIds.end())
					{
						objectPointIds.insert(objectPointId);

						const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
						ocean_assert(iO != databaseObjectPointMap.end());

						if (iO->second.priority() >= minimalPriority && ((tMatchPosition && iO->second.point() == referencePosition) || (!tMatchPosition && iO->second.point() != referencePosition)))
						{
							result.push_back(objectPointId);

							if (objectPoints)
								objectPoints->push_back(iO->second.point());
						}
					}
				}
			}
		}

		ocean_assert(IndexSet32(result.begin(), result.end()).size() == result.size());
		ocean_assert(objectPoints == nullptr || objectPoints->size() == result.size());

		return result;
	}
}

template <bool tThreadSafe>
Vectors2 Database::imagePointsWithObjectPoints(const Index32 poseId, Indices32& objectPointIds) const
{
	ocean_assert(poseId != invalidId);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);

	ocean_assert(iP != databasePoseMap.end());
	if (iP == databasePoseMap.end())
		return Vectors2();

	const IndexSet32& imagePointIds = iP->second.imagePointIds();

	Vectors2 result;
	result.reserve(imagePointIds.size());

	ocean_assert(objectPointIds.empty());
	objectPointIds.clear();
	objectPointIds.reserve(imagePointIds.size());

	for (IndexSet32::const_iterator i = imagePointIds.cbegin(); i != imagePointIds.cend(); ++i)
	{
		ocean_assert(*i != invalidId);

		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		if (iI->second.objectPointId() != invalidId)
		{
			result.push_back(iI->second.point());
			objectPointIds.push_back(iI->second.objectPointId());
		}
	}

	ocean_assert(result.size() == objectPointIds.size());

	return result;
}

template <bool tThreadSafe>
Vectors2 Database::imagePointsFromObjectPoints(const Index32 poseId, Indices32& objectPointIds, Indices32* imagePointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(!objectPointIds.empty());
	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors2 points;
	points.reserve(objectPointIds.size());

	Indices32 validObjectPointIds;
	validObjectPointIds.reserve(objectPointIds.size());

	for (Indices32::const_iterator i = objectPointIds.begin(); i != objectPointIds.end(); ++i)
	{
		const Index64To32Map::const_iterator iPO = databasePoseObjectPointMap.find(index64(poseId, *i));

		if (iPO != databasePoseObjectPointMap.end())
		{
			ocean_assert(iPO->second != invalidId);
			const ImagePointMap::const_iterator iI = databaseImagePointMap.find(iPO->second);
			ocean_assert(iI != databaseImagePointMap.end());

			points.push_back(iI->second.point());
			validObjectPointIds.push_back(*i);

			if (imagePointIds)
				imagePointIds->push_back(iPO->second);
		}
	}

	objectPointIds = std::move(validObjectPointIds);
	return points;
}

template <bool tThreadSafe>
Vectors2 Database::imagePointsFromObjectPoints(const Index32 poseId, const Indices32& objectPointIds, Indices32& validIndices, Indices32* imagePointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(!objectPointIds.empty());
	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());

	return imagePointsFromObjectPoints<tThreadSafe>(poseId, objectPointIds.data(), objectPointIds.size(), validIndices, imagePointIds);
}

template <bool tThreadSafe>
Vectors2 Database::imagePointsFromObjectPoints(const Index32 poseId, const Index32* objectPointIds, const size_t numberObjectPointIds, Indices32& validIndices, Indices32* imagePointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(objectPointIds && numberObjectPointIds != 0);
	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	Vectors2 points;
	points.reserve(numberObjectPointIds);

	for (size_t n = 0; n < numberObjectPointIds; ++n)
	{
		const Index32 objectPointId = objectPointIds[n];

		const Index64To32Map::const_iterator iPO = databasePoseObjectPointMap.find(index64(poseId, objectPointId));

		if (iPO != databasePoseObjectPointMap.end())
		{
			ocean_assert(iPO->second != invalidId);
			const ImagePointMap::const_iterator iI = databaseImagePointMap.find(iPO->second);
			ocean_assert(iI != databaseImagePointMap.end());

			points.push_back(iI->second.point());
			validIndices.push_back((unsigned int)n);

			if (imagePointIds)
				imagePointIds->push_back(iPO->second);
		}
	}

	return points;
}

template <bool tThreadSafe>
Database::ImagePointGroups Database::imagePointGroups(const Indices32 poseIds, Indices32& objectPointIds) const
{
	ocean_assert(!poseIds.empty());
	ocean_assert(IndexSet32(poseIds.begin(), poseIds.end()).size() == poseIds.size());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	ImagePointsMap intermediate;

	for (Indices32::const_iterator iP = poseIds.begin(); iP != poseIds.end(); ++iP)
	{
		ocean_assert(*iP != invalidId);
		ocean_assert(databasePoseMap.find(*iP) != databasePoseMap.end());

		const PoseData& poseData = databasePoseMap.find(*iP)->second;

		for (IndexSet32::const_iterator iI = poseData.imagePointIds().begin(); iI != poseData.imagePointIds().end(); ++iI)
		{
			const ImagePointMap::const_iterator i = databaseImagePointMap.find(*iI);
			ocean_assert(i != databaseImagePointMap.end());

			if (i->second.objectPointId() != invalidId)
				intermediate[i->second.objectPointId()].push_back(i->second.point());
		}
	}

	ImagePointGroups result(poseIds.size());

	for (ImagePointsMap::iterator i = intermediate.begin(); i != intermediate.end(); ++i)
		if (i->second.size() == poseIds.size())
		{
			objectPointIds.push_back(i->first);

			for (size_t n = 0; n < poseIds.size(); ++n)
				result[n].push_back(i->second[n]);
		}

	return result;
}

template <bool tThreadSafe>
Database::IdIdPointPairsMap Database::imagePoints(const Index32 poseId, const bool previous, const size_t minimalObservations, const size_t maximalObservations) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(maximalObservations == 0 || minimalObservations <= maximalObservations);

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	IdIdPointPairsMap result;

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	const PoseData& poseData = iP->second;

	for (IndexSet32::const_iterator iI = poseData.imagePointIds().begin(); iI != poseData.imagePointIds().end(); ++iI)
	{
		const ImagePointMap::const_iterator i = databaseImagePointMap.find(*iI);
		ocean_assert(i != databaseImagePointMap.end());

		const Index32 objectPointId = i->second.objectPointId();

		if (objectPointId != invalidId)
		{
			IdPointPairs imagePointPairs;
			imagePointPairs.push_back(std::make_pair(*iI, i->second.point()));

			// now find the consecutive image points
			Index32 pId = poseId;

			while (((previous && pId-- != 0u) || (!previous && ++pId < databasePoses)) && (maximalObservations == 0 || imagePointPairs.size() < maximalObservations))
			{
				const Index64To32Map::const_iterator iPO = databasePoseObjectPointMap.find(index64(pId, objectPointId));

				if (iPO == databasePoseObjectPointMap.end())
					break;

				const ImagePointMap::const_iterator otherPO = databaseImagePointMap.find(iPO->second);
				ocean_assert(otherPO != databaseImagePointMap.end());

				imagePointPairs.push_back(std::make_pair(iPO->second, otherPO->second.point()));
			}

			if (minimalObservations == 0 || imagePointPairs.size() >= minimalObservations)
				result[objectPointId] = imagePointPairs;
		}
	}

	return result;
}

template <bool tThreadSafe>
void Database::imagePoints(const Index32 pose0, const Index32 pose1, Vectors2& points0, Vectors2& points1, Indices32* objectPointIds) const
{
	ocean_assert(pose0 != invalidId && pose1 != invalidId);
	ocean_assert(pose0 != pose1);

	ocean_assert(points0.size() == points1.size());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP0 = databasePoseMap.find(pose0);
	ocean_assert(iP0 != databasePoseMap.end());

	const PoseData& poseData0 = iP0->second;

	for (IndexSet32::const_iterator i = poseData0.imagePointIds().begin(); i != poseData0.imagePointIds().end(); ++i)
	{
		const ImagePointMap::const_iterator iI0 = databaseImagePointMap.find(*i);
		ocean_assert(iI0 != databaseImagePointMap.end());

		ocean_assert(iI0->second.poseId() == pose0);
		if (iI0->second.objectPointId() != invalidId)
		{
			const Index64To32Map::const_iterator iPO = databasePoseObjectPointMap.find(index64(pose1, iI0->second.objectPointId()));

			if (iPO != databasePoseObjectPointMap.end())
			{
				const ImagePointMap::const_iterator iI1 = databaseImagePointMap.find(iPO->second);
				ocean_assert(iI1 != databaseImagePointMap.end());

				points0.push_back(iI0->second.point());
				points1.push_back(iI1->second.point());

				if (objectPointIds)
					objectPointIds->push_back(iI0->second.objectPointId());
			}
		}
	}
}

template <bool tThreadSafe, bool tMatchPosition>
void Database::imagePointsObjectPoints(const Index32 poseId, Vectors2& imagePoints, Vectors3& objectPoints, const Vector3& referencePosition, const size_t minimalObservations, Indices32* imagePointIds, Indices32* objectPointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(imagePoints.empty() && objectPoints.empty());

	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());
	ocean_assert(objectPointIds == nullptr || objectPointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iPose = databasePoseMap.find(poseId);
	ocean_assert(iPose != databasePoseMap.end());

	const PoseData& poseData = iPose->second;

	imagePoints.reserve(poseData.imagePointIds().size());
	objectPoints.reserve(poseData.imagePointIds().size());

	if (imagePointIds)
	{
		imagePointIds->reserve(poseData.imagePointIds().size());
	}

	if (objectPointIds)
	{
		objectPointIds->reserve(poseData.imagePointIds().size());
	}

	for (const Index32& imagePointId : poseData.imagePointIds())
	{
		const ImagePointMap::const_iterator iImagePoint = databaseImagePointMap.find(imagePointId);
		ocean_assert(iImagePoint != databaseImagePointMap.end());

		if (iImagePoint->second.objectPointId() != invalidId)
		{
			const ObjectPointMap::const_iterator iObjectPoint = databaseObjectPointMap.find(iImagePoint->second.objectPointId());
			ocean_assert(iObjectPoint != databaseObjectPointMap.end());

			if (((tMatchPosition && iObjectPoint->second.point() == referencePosition) || (!tMatchPosition && iObjectPoint->second.point() != referencePosition)) && (minimalObservations == 0 || iObjectPoint->second.imagePointIds().size() >= minimalObservations))
			{
				imagePoints.push_back(iImagePoint->second.point());
				objectPoints.push_back(iObjectPoint->second.point());

				if (imagePointIds)
				{
					imagePointIds->push_back(iImagePoint->first);
				}

				if (objectPointIds)
				{
					objectPointIds->push_back(iImagePoint->second.objectPointId());
				}
			}
		}
	}
}

template <bool tThreadSafe, bool tMatchPosition>
void Database::imagePointsObjectPoints(const Index32 poseId, const IndexSet32& priorityIds, Vectors2& priorityImagePoints, Vectors3& priorityObjectPoints, Vectors2& remainingImagePoints, Vectors3& remainingObjectPoints, const Vector3& referencePosition, const size_t minimalObservations, Indices32* priorityImagePointIds, Indices32* priorityObjectPointIds, Indices32* remainingImagePointIds, Indices32* remainingObjectPointIds) const
{
	ocean_assert(poseId != invalidId);
	ocean_assert(priorityImagePoints.empty() && priorityObjectPoints.empty());
	ocean_assert(remainingImagePoints.empty() && remainingObjectPoints.empty());

	ocean_assert(priorityImagePointIds == nullptr || priorityImagePointIds->empty());
	ocean_assert(priorityObjectPointIds == nullptr || priorityObjectPointIds->empty());
	ocean_assert(remainingImagePointIds == nullptr || remainingImagePointIds->empty());
	ocean_assert(remainingObjectPointIds == nullptr || remainingObjectPointIds->empty());

	ocean_assert(!priorityIds.empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
	ocean_assert(iP != databasePoseMap.end());

	const PoseData& poseData = iP->second;

	priorityImagePoints.reserve(poseData.imagePointIds().size());
	priorityObjectPoints.reserve(poseData.imagePointIds().size());

	remainingImagePoints.reserve(poseData.imagePointIds().size());
	remainingObjectPoints.reserve(poseData.imagePointIds().size());

	if (priorityImagePointIds)
		priorityImagePointIds->reserve(poseData.imagePointIds().size());

	if (priorityObjectPointIds)
		priorityObjectPointIds->reserve(poseData.imagePointIds().size());

	if (remainingImagePointIds)
		remainingImagePointIds->reserve(poseData.imagePointIds().size());

	if (remainingObjectPointIds)
		remainingObjectPointIds->reserve(poseData.imagePointIds().size());

	for (IndexSet32::const_iterator i = poseData.imagePointIds().begin(); i != poseData.imagePointIds().end(); ++i)
	{
		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		if (iI->second.objectPointId() != invalidId)
		{
			const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(iI->second.objectPointId());
			ocean_assert(iO != databaseObjectPointMap.end());

			if (((tMatchPosition && iO->second.point() == referencePosition) || (!tMatchPosition && iO->second.point() != referencePosition)) && (minimalObservations == 0 || iO->second.imagePointIds().size() >= minimalObservations))
			{
				ocean_assert(iO->first == iI->second.objectPointId());

				if (priorityIds.find(iO->first) != priorityIds.end())
				{
					priorityImagePoints.push_back(iI->second.point());
					priorityObjectPoints.push_back(iO->second.point());

					if (priorityImagePointIds)
						priorityImagePointIds->push_back(iI->first);

					if (priorityObjectPointIds)
						priorityObjectPointIds->push_back(iI->second.objectPointId());
				}
				else
				{
					remainingImagePoints.push_back(iI->second.point());
					remainingObjectPoints.push_back(iO->second.point());

					if (remainingImagePointIds)
						remainingImagePointIds->push_back(iI->first);

					if (remainingObjectPointIds)
						remainingObjectPointIds->push_back(iI->second.objectPointId());
				}
			}
		}
	}
}

template <bool tThreadSafe, bool tMatchPose>
void Database::posesImagePoints(const Index32 objectPointId, HomogenousMatrices4& poses, Vectors2& imagePoints, const HomogenousMatrix4& referencePose, Indices32* poseIds, Indices32* imagePointIds, const Index32 lowerPoseId, const Index32 upperPoseId) const
{
	ocean_assert(objectPointId != invalidId);
	ocean_assert(poses.empty() && imagePoints.empty());

	ocean_assert(poseIds == nullptr || poseIds->empty());
	ocean_assert(imagePointIds == nullptr || imagePointIds->empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	const ObjectPointMap::const_iterator iO = databaseObjectPointMap.find(objectPointId);
	ocean_assert(iO != databaseObjectPointMap.end());

	const IndexSet32& imagePointCandidateIds = iO->second.imagePointIds();

	poses.reserve(imagePointCandidateIds.size());
	imagePoints.reserve(imagePointCandidateIds.size());

	if (poseIds)
		poseIds->reserve(imagePointCandidateIds.size());

	if (imagePointIds)
		imagePointIds->reserve(imagePointCandidateIds.size());

	for (IndexSet32::const_iterator i = imagePointCandidateIds.begin(); i != imagePointCandidateIds.end(); ++i)
	{
		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		const Vector2& imagePoint = iI->second.point();
		const Index32 poseId = iI->second.poseId();

		if (poseId == invalidId || (lowerPoseId != invalidId && poseId < lowerPoseId) || (upperPoseId != invalidId && poseId > upperPoseId))
			continue;

		const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
		ocean_assert(iP != databasePoseMap.end());

		const HomogenousMatrix4& pose = iP->second.pose();

		if ((tMatchPose && pose == referencePose) || (!tMatchPose && pose != referencePose))
		{
			ocean_assert(pose.isValid());

			imagePoints.push_back(imagePoint);
			poses.push_back(pose);

			if (poseIds)
				poseIds->push_back(poseId);

			if (imagePointIds)
				imagePointIds->push_back(*i);
		}
	}
}

template <bool tThreadSafe>
Database::TopologyTriples Database::topologyTriples(const Indices32& poseIds) const
{
	ocean_assert(!poseIds.empty());

	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	TopologyTriples result;

	for (Indices32::const_iterator iiP = poseIds.begin(); iiP != poseIds.end(); ++iiP)
	{
		const PoseMap::const_iterator iP = databasePoseMap.find(*iiP);
		ocean_assert(iP != databasePoseMap.end());

		const IndexSet32& poseImagePoints = iP->second.imagePointIds();

		for (IndexSet32::const_iterator iI = poseImagePoints.begin(); iI != poseImagePoints.end(); ++iI)
		{
			const ImagePointMap::const_iterator i = databaseImagePointMap.find(*iI);
			ocean_assert(i != databaseImagePointMap.end());

			const Index32 objectPointId = i->second.objectPointId();

			if (objectPointId != invalidId)
				result.push_back(TopologyTriple(*iiP, objectPointId, *iI));
		}
	}

	return result;
}

template <bool tThreadSafe>
inline void Database::clear()
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	databasePoseMap.clear();
	databaseObjectPointMap.clear();
	databaseImagePointMap.clear();
	databasePoseObjectPointMap.clear();

	databasePoses = 0u;

	databaseObjectPointIdCounter = invalidId;
	databaseImagePointIdCounter = invalidId;
}

template <bool tThreadSafe>
inline void Database::reset(const Vector3& referenceObjectPoint, const HomogenousMatrix4& referencePose)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	for (ObjectPointMap::iterator i = databaseObjectPointMap.begin(); i != databaseObjectPointMap.end(); ++i)
	{
		i->second.setPoint(referenceObjectPoint);
	}

	for (PoseMap::iterator i = databasePoseMap.begin(); i != databasePoseMap.end(); ++i)
	{
		i->second.setPose(referencePose);
	}
}

template <typename T, bool tThreadSafe>
void Database::reset(const size_t numberPoses, const Index32* poseIds, const HomogenousMatrixT4<T>* poses, const size_t numberObjectPoints, const Index32* objectPointIds, const VectorT3<T>* objectPoints, const T* objectPointPriorities, const size_t numberImagePoints, const Index32* imagePointIds, const VectorT2<T>* imagePoints, const Index32* topologyPoseIds, const Index32* topologyObjectPointIds)
{
	const TemplatedScopedLock<tThreadSafe> scopedLock(databaseLock);

	clear<false>();

	databasePoses = 0u;
	databaseObjectPointIdCounter = 0u;
	databaseImagePointIdCounter = 0u;

	for (size_t n = 0; n < numberPoses; ++n)
	{
		const Index32& poseId = poseIds[n];
		const HomogenousMatrixT4<T>& pose = poses[n];

		ocean_assert(databasePoseMap.find(poseId) == databasePoseMap.cend());
		databasePoseMap.emplace(poseId, PoseData(HomogenousMatrix4(pose)));

		ocean_assert(poseId != invalidId);
		databasePoses = max(databasePoses, poseId + 1u);
	}

	databaseObjectPointMap.reserve(numberObjectPoints);

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		const Index32& objectPointId = objectPointIds[n];
		const VectorT3<T>& objectPoint = objectPoints[n];
		const T& objectPointPriority = objectPointPriorities[n];

		ocean_assert(databaseObjectPointMap.find(objectPointId) == databaseObjectPointMap.cend());

		if (objectPoint == VectorT3<T>(NumericT<T>::minValue(), NumericT<T>::minValue(), NumericT<T>::minValue()))
		{
			databaseObjectPointMap.emplace(objectPointId, ObjectPointData(invalidObjectPoint(), Scalar(objectPointPriority)));
		}
		else
		{
			databaseObjectPointMap.emplace(objectPointId, ObjectPointData(Vector3(objectPoint), Scalar(objectPointPriority)));
		}

		ocean_assert(objectPointId != invalidId);
		databaseObjectPointIdCounter = max(databaseObjectPointIdCounter, objectPointId);
	}

	databaseImagePointMap.reserve(numberImagePoints);
	databasePoseObjectPointMap.reserve(numberImagePoints);

	for (size_t n = 0; n < numberImagePoints; ++n)
	{
		const Index32& imagePointId = imagePointIds[n];
		const VectorT2<T>& imagePoint = imagePoints[n];

		const Index32& topologyPoseId = topologyPoseIds[n];
		const Index32& topologyObjectPointId = topologyObjectPointIds[n];

		ocean_assert(databaseImagePointMap.find(imagePointId) == databaseImagePointMap.cend());
		databaseImagePointMap.emplace(imagePointId, ImagePointData(Vector2(imagePoint), topologyPoseId, topologyObjectPointId));

		ocean_assert((topologyPoseId == invalidId && topologyObjectPointId == invalidId) || (topologyPoseId != invalidId && topologyObjectPointId != invalidId));

		if (topologyPoseId != invalidId)
		{
			databasePoseObjectPointMap.emplace(index64(topologyPoseId, topologyObjectPointId), imagePointId);

			ocean_assert(databasePoseMap.find(topologyPoseId) != databasePoseMap.cend());
			databasePoseMap[topologyPoseId].registerImagePoint(imagePointId);

			ocean_assert(databaseObjectPointMap.find(topologyObjectPointId) != databaseObjectPointMap.cend());
			databaseObjectPointMap[topologyObjectPointId].registerImagePoint(imagePointId);
		}

		ocean_assert(imagePointId != invalidId);
		databaseImagePointIdCounter = max(databaseImagePointIdCounter, imagePointId);
	}
}

inline Indices32 Database::filterTopologyTriplesPoses(const TopologyTriples& topologyTriples, const IndexSet32& poseIds)
{
	ocean_assert(!poseIds.empty());

	Indices32 result;
	result.reserve(topologyTriples.size());

	for (unsigned int n = 0u; n < topologyTriples.size(); ++n)
		if (poseIds.find(topologyTriples[n].poseId()) != poseIds.end())
			result.push_back(n);

	return result;
}

inline Indices32 Database::filterTopologyTriplesObjectPoints(const TopologyTriples& topologyTriples, const IndexSet32& objectPointIds)
{
	ocean_assert(!objectPointIds.empty());

	Indices32 result;
	result.reserve(topologyTriples.size());

	for (unsigned int n = 0u; n < topologyTriples.size(); ++n)
		if (objectPointIds.find(topologyTriples[n].objectPointId()) != objectPointIds.end())
			result.push_back(n);

	return result;
}

inline Indices32 Database::filterTopologyTriplesImagePoints(const TopologyTriples& topologyTriples, const IndexSet32& imagePointIds)
{
	ocean_assert(!imagePointIds.empty());

	Indices32 result;
	result.reserve(topologyTriples.size());

	for (unsigned int n = 0u; n < topologyTriples.size(); ++n)
		if (imagePointIds.find(topologyTriples[n].imagePointId()) != imagePointIds.end())
			result.push_back(n);

	return result;
}

inline Indices32 Database::reliableObjectPoints(const TopologyTriples& topologyTriples, const unsigned int minimalObservations)
{
	ocean_assert(!topologyTriples.empty());

	Index32To32Map objectPointCounterMap;
	for (TopologyTriples::const_iterator i = topologyTriples.begin(); i != topologyTriples.end(); ++i)
	{
		ocean_assert(i->objectPointId() != invalidId);
		objectPointCounterMap[i->objectPointId()]++;
	}

	Indices32 objectPointIds;
	objectPointIds.reserve(objectPointCounterMap.size());

	for (Index32To32Map::const_iterator i = objectPointCounterMap.begin(); i != objectPointCounterMap.end(); ++i)
		if (i->second >= minimalObservations)
			objectPointIds.push_back(i->first);

	return objectPointIds;
}

inline Database& Database::operator=(const Database& database)
{
	if (this != &database)
	{
		databasePoseMap = database.databasePoseMap;
		databaseObjectPointMap = database.databaseObjectPointMap;
		databaseImagePointMap = database.databaseImagePointMap;
		databasePoseObjectPointMap = database.databasePoseObjectPointMap;

		databasePoses = database.databasePoses;
		databaseObjectPointIdCounter = database.databaseObjectPointIdCounter;
		databaseImagePointIdCounter = database.databaseImagePointIdCounter;
	}

	return *this;
}

inline Database& Database::operator=(Database&& database) noexcept
{
	if (this != &database)
	{
		databasePoseMap = std::move(database.databasePoseMap);
		databaseObjectPointMap = std::move(database.databaseObjectPointMap);
		databaseImagePointMap = std::move(database.databaseImagePointMap);
		databasePoseObjectPointMap = std::move(database.databasePoseObjectPointMap);

		databasePoses = database.databasePoses;
		databaseObjectPointIdCounter = database.databaseObjectPointIdCounter;
		databaseImagePointIdCounter = database.databaseImagePointIdCounter;

		database.databasePoses = 0u;
		database.databaseObjectPointIdCounter = invalidId;
		database.databaseImagePointIdCounter = invalidId;
	}

	return *this;
}

inline Database::operator bool() const
{
	return !isEmpty<false>();
}

template <bool tMatchPosition, bool tNeedValidPose>
inline void Database::numberCorrespondencesSubset(const Index32 lowerPoseId, const Vector3* referenceObjectPoint, const Scalar minimalPriority, unsigned int* correspondences, const unsigned int firstPose, const unsigned int numberPoses) const
{
	ocean_assert(numberPoses >= 1u);
	ocean_assert(referenceObjectPoint && correspondences);

	for (unsigned int n = firstPose; n < firstPose + numberPoses; ++n)
		correspondences[n] = numberCorrespondences<false, tMatchPosition, tNeedValidPose>(lowerPoseId + n, *referenceObjectPoint, minimalPriority);
}

template <bool tMatchPosition>
void Database::objectPointIdsWithNumberOfObservationsSubset(const Index32* objectPointIds, const Vector3* referencePosition, const Scalar minimalPriority, IndexPairs32* pairs, Lock* lock, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints) const
{
	ocean_assert(objectPointIds && referencePosition && pairs);

	IndexPairs32 localPairs;
	localPairs.reserve(numberObjectPoints);

	for (unsigned int n = firstObjectPoint; n < firstObjectPoint + numberObjectPoints; ++n)
	{
		const ObjectPointMap::const_iterator i = databaseObjectPointMap.find(objectPointIds[n]);
		if (i->second.priority() >= minimalPriority && ((tMatchPosition && i->second.point() == *referencePosition) || (!tMatchPosition && i->second.point() != *referencePosition)))
			localPairs.push_back(std::make_pair(i->first, numberValidPoses(i->first, i->second.imagePointIds())));
	}

	if (lock)
	{
		const ScopedLock scopedLock(*lock);
		pairs->insert(pairs->end(), localPairs.begin(), localPairs.end());
	}
	else
		*pairs = std::move(localPairs);
}

inline unsigned int Database::numberValidPoses(const Index32 objectPointId, const IndexSet32& imagePointIds) const
{
	ocean_assert_and_suppress_unused(objectPointId != invalidId, objectPointId);

	ocean_assert(databaseObjectPointMap.find(objectPointId) != databaseObjectPointMap.end());
	ocean_assert(databaseObjectPointMap.find(objectPointId)->second.imagePointIds() == imagePointIds);

	unsigned int validPoses = 0u;

	for (IndexSet32::const_iterator i = imagePointIds.begin(); i != imagePointIds.end(); ++i)
	{
		const ImagePointMap::const_iterator iI = databaseImagePointMap.find(*i);
		ocean_assert(iI != databaseImagePointMap.end());

		const Index32 poseId = iI->second.poseId();

		const PoseMap::const_iterator iP = databasePoseMap.find(poseId);
		ocean_assert(iP != databasePoseMap.end());

		const HomogenousMatrix4& pose = iP->second.pose();

		if (pose.isValid())
			validPoses++;
	}

	return validPoses;
}

inline Index32 Database::firstIndex(const Index64 index)
{
	return Index32(index & 0xFFFFFFFFull);
}

inline Index32 Database::secondIndex(const Index64 index)
{
	return Index32(index >> 32);
}

inline Index64 Database::index64(const Index32 first, const Index32 second)
{
	return Index64(first) | (Index64(second) << 32);
}

}

}

#endif // META_OCEAN_TRACKING_DATABASE_H
