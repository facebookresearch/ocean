/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_SCENE_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_SCENE_TRACKER_6DOF_H

#include "ocean/devices/Devices.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Plane3.h"

namespace Ocean
{

namespace Devices
{

// Forward declaration.
class SceneTracker6DOF;

/**
 * Definition of a smart object reference for a 6-DOF scene tracker.
 * @see SceneTracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<SceneTracker6DOF> SceneTracker6DOFRef;

/**
 * This class implements the base for all 6-DOF scene trackers.
 * Scene trackers provides 6-DOF transformations in combination with scene elements like point clouds, meshes, planes, or other content from the environment.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT SceneTracker6DOF : virtual public Tracker6DOF
{
	public:

		/**
		 * This class implements the base class of all scene elements.
		 */
		class SceneElement
		{
			public:

				/**
				 * Definition of individual types of scene elements.
				 */
				enum SceneElementType : uint32_t
				{
					/// The scene element is invalid.
					SET_INVALID = 0u,
					///  The scene element contains 3D object points.
					SET_OBJECT_POINTS,
					///  The scene element contains 2D/3D correspondences.
					SET_FEATURE_CORRESPONDENCES,
					///  The scene element contains 3D planes.
					SET_PLANES,
					///  The scene element contains 3D meshes.
					SET_MESHES,
					///  The scene element contains depth information.
					SET_DEPTH,
					/// The scene element contains room objects.
					SET_ROOM
				};

			public:

				/**
				 * Destructs a scene element.
				 */
				virtual ~SceneElement() = default;

				/**
				 * Returns the type of this scene element.
				 * @return The scene element type
				 */
				inline SceneElementType sceneElementType() const;

			protected:

				/**
				 * Creates a default scene element with specified type.
				 * @param sceneElementType The type of the new scene element
				 */
				explicit inline SceneElement(const SceneElementType sceneElementType);

			protected:

				/// The type of the scene element.
				SceneElementType sceneElementType_ = SET_INVALID;
		};

		/**
		 * Definition of a scene element holding 3D object points.
		 * Each 3D object point may be associated with a corresponding object point id.
		 */
		class SceneElementObjectPoints : public SceneElement
		{
			public:

				/**
				 * Creates a new SceneElement object for 3D object points.
				 * @param objectPoints The 3D object points, at least one
				 * @param objectPointIds Optional ids of the object points, one for each 3D object point, empty if unknown
				 */
				inline SceneElementObjectPoints(Vectors3&& objectPoints, Indices64&& objectPointIds = Indices64());

				/**
				 * Returns the 3D object points of this scene element.
				 * @return The scene element's object points
				 */
				inline const Vectors3& objectPoints() const;

				/**
				 * Returns the unique ids of the object points if known.
				 * @return The object point ids, empty if unknown
				 */
				inline const Indices64& objectPointIds() const;

			protected:

				/// The 3D object points of this scene element.
				Vectors3 objectPoints_;

				/// The unique ids of this scene element.
				Indices64 objectPointIds_;
		};

		/**
		 * This class implements a scene element holding 2D/3D feature correspondences.
		 */
		class SceneElementFeatureCorrespondences : public SceneElement
		{
			public:

				/**
				 * Creates a new scene element for 2D/3D correspondences.
				 * @param objectPoints The 3D object points of the correspondences, at least one
				 * @param imagePoints The 2D image points of the correspondences, one for each 3D object point
				 * @param objectPointIds The optional ids of the object points, one for each 3D obect point, or empty if unknown
				 */
				inline SceneElementFeatureCorrespondences(Vectors3&& objectPoints, Vectors2&& imagePoints, Indices64&& objectPointIds = Indices64());

				/**
				 * Returns the 3D object points of this scene element.
				 * @return The scene element's 3D object points
				 */
				inline const Vectors3& objectPoints() const;

				/**
				 * Returns the 2D image points of this scene element.
				 * @return The scene element's image points, one for each 3D object point
				 */
				inline const Vectors2& imagePoints() const;

				/**
				 * Returns the unique object point ids.
				 * @return The object point ids, empty if unknown
				 */
				inline const Indices64& objectPointIds() const;

			protected:

				/// The 3D object points of this scene element.
				Vectors3 objectPoints_;

				/// The 2D image points of this scene element, one for each 3D object point.
				Vectors2 imagePoints_;

				/// The unique ids of the 3D object points, if known.
				Indices64 objectPointIds_;
		};

		/**
		 * This class implements a scene element holding 3D planes.
		 */
		class SceneElementPlanes : public SceneElement
		{
			public:

				/**
				 * This class stores the relevant information of a 3D plane.
				 * Most properties of the class (e.g., vertices, bounding box) are defined in relation to the plane's coordinate system.<br>
				 * The plane's normal is identical to the y-axis of the plane's coordinate system.
				 */
				class Plane
				{
					public:

						/**
						 * Definition of individual plane types.
						 */
						enum PlaneType : uint32_t
						{
							/// The plane type is unknown.
							PT_UNKNOWN = 0u,
							/// The plane is horizontal and perpendicular to gravity (e.g., a floor/ceiling plane).
							PT_HORIZONTAL,
							/// The plane is vertical and parallel to gravity (e.g., a wall plane).
							PT_VERTICAL
						};

					public:

						/**
						 * Creates a new plane object.
						 * @param planeId The plane's unique id, must be valid
						 * @param planeType The type of the plane
						 * @param world_T_plane The transformation between plane and world, with y-axis identical to the plane's normal, must be valid
						 * @param boundingBox The bounding box of the plane, defined in the plane's coordinate system, must be valid
						 * @param boundaryVertices The vertices of the boundary of the plane, defined in the plane's coordinate system
						 */
						inline Plane(const Index32 planeId, const PlaneType planeType, const HomogenousMatrix4& world_T_plane, const Box3& boundingBox, Vectors3&& boundaryVertices);

						/**
						 * Creates a new plane object.
						 * @param planeId The plane's unique id, must be valid
						 * @param planeType The type of the plane
						 * @param world_T_plane The transformation between plane and world, with y-axis identical to the plane's normal, must be valid
						 * @param boundingBox The bounding box of the plane, defined in the plane's coordinate system, must be valid
						 * @param vertices The plane's vertices, defined in the plane's coordinate system, at least 3
						 * @param textureCoordinates The vertices' texture coordinates, one for each vertex, empty if unknown
						 * @param triangleIndices The indices of the vertices representing the plane's surface triangles, three indices define one triangle, always a multiple of three
						 * @param boundaryVertices The vertices of the boundary of the plane, defined in the plane's coordinate system
						 */
						inline Plane(const Index32 planeId, const PlaneType planeType, const HomogenousMatrix4& world_T_plane, const Box3& boundingBox, Vectors3&& vertices, Vectors2&& textureCoordinates, Indices32&& triangleIndices, Vectors3&& boundaryVertices);

						/**
						 * Returns the unique id of the plane.
						 * @return The plane's unique id
						 */
						inline Index32 planeId() const;

						/**
						 * Returns the type of the plane.
						 * @return The plane's type
						 */
						inline PlaneType planeType() const;

						/**
						 * Returns the transformation of the plane in relation to world.
						 * The plane's normal is identical to the y-axis of the plane's coordinate system.
						 * @return The transformation between plane and world
						 */
						inline const HomogenousMatrix4& world_T_plane() const;

						/**
						 * Returns the 3D plane as defined in world.
						 * @return The plane object which can be used e.g. for advanced math calculations, defined in world
						 */
						inline Plane3 worldPlane() const;

						/**
						 * Returns the plane's bounding box (the extent of the plane).
						 * @return The bounding box of the plane, defined in the plane's coordinate system
						 */
						inline const Box3& boundingBox() const;

						/**
						 * Returns the vertices representing the plane.
						 * @return The plane's vertices, defined in the plane's coordinate system, empty if unknown
						 */
						inline const Vectors3& vertices() const;

						/**
						 * Returns the texture coordinates of the vertices, if known.
						 * @return The vertices' texture coordinates, one for each vertex, empty if unknown
						 */
						inline const Vectors2& textureCoordinates() const;

						/**
						 * Returns the indices of the vertices representing the plane's surface triangles, three indices define one triangle, always a multiple of three.
						 * @return The indices of the plane's triangles, empty if unknown
						 */
						inline const Indices32& triangleIndices() const;

						/**
						 * Returns the vertices of the boundary of the plane.
						 * @return The plane's boundary vertices, defined in the plane's coordinate system
						 */
						inline const Vectors3& boundaryVertices() const;

					protected:

						/// The unique id of this plane.
						Index32 planeId_ = Index32(-1);

						/// The type of the plane.
						PlaneType planeType_ = PT_UNKNOWN;

						/// The transformation between plane and world, with y-axis as the plane's normal.
						HomogenousMatrix4 world_T_plane_ = HomogenousMatrix4(false);

						/// The bounding box of the plane, defined in the plane's origin.
						Box3 boundingBox_;

						/// The vertices representing the plane, defined in the plane's coordinate system.
						Vectors3 vertices_;

						/// The vertices' texture coordinates, one for each vertex, empty if unknown.
						Vectors2 textureCoordinates_;

						/// The indices of the vertices representing the plane's surface triangles, three indices define one triangle, always a multiple of three.
						Indices32 triangleIndices_;

						/// The vertices of the boundaries of the plane, defined in the plane's coordinate system.
						Vectors3 boundaryVertices_;
				};

				/**
				 * Definition of a vector holding planes.
				 */
				typedef std::vector<Plane> Planes;

			public:

				/**
				 * Creates a new scene element object with several given planes.
				 * @param planes The planes of the new scene element, at least one
				 */
				explicit inline SceneElementPlanes(Planes&& planes);

				/**
				 * Returns all planes of this scene element object, may be empty.
				 * @return The scene element's planes
				 */
				inline const Planes& planes() const;

			protected:

				/// The planes stores in this scene element object.
				Planes planes_;
		};

		/**
		 * This class implements a scene element holding 3D meshes.
		 */
		class SceneElementMeshes : public SceneElement
		{
			public:

				/**
				 * This class stores the relevant information of a 3D mesh.
				 * Most properties of the class (e.g., vertices, bounding box) are defined in relation to the mesh's coordinate system.<br>
				 */
				class Mesh
				{
					public:

						/**
						 * Definition of individual mesh types.
						 */
						enum MeshType : uint32_t
						{
							/// The mesh type is unknown.
							MT_UNKNOWN = 0u,
							/// The mesh is representing a ceiling.
							MT_CEILING,
							/// The mesh is representing a door.
							MT_DOOR,
							/// The mesh is representing a floor.
							MT_FLOOR,
							/// The mesh is representing a seat.
							MT_SEAT,
							/// The mesh is representing a table.
							MT_TABLE,
							/// The mesh is representing a wall.
							MT_WALL,
							/// The mesh is representing a window.
							MT_WINDOW
						};

					public:

						/**
						 * Creates a new mesh object.
						 * @param meshId The mesh's unique id, must be valid
						 * @param world_T_mesh The transformation between mesh and world, must be valid
						 * @param vertices The mesh's vertices, defined in the mesh's coordinate system, at least 3
						 * @param perVertexNormals The per-vertex normals of the mesh's faces, defined in the mesh's coordinate system, one for each vertex
						 * @param triangleIndices The indices of the vertices representing the mesh's surface triangles, three indices define one triangle, always a multiple of three
						 */
						inline Mesh(const Index32 meshId, const HomogenousMatrix4& world_T_mesh, Vectors3&& vertices, Vectors3&& perVertexNormals, Indices32&& triangleIndices);

						/**
						 * Returns the unique id of the mesh.
						 * @return The mesh's unique id
						 */
						inline Index32 meshId() const;

						/**
						 * Returns the transformation of the mesh in relation to world.
						 * @return The transformation between mesh and world
						 */
						inline const HomogenousMatrix4& world_T_mesh() const;

						/**
						 * Returns the vertices representing the mesh.
						 * @return The mesh's vertices, defined in the mesh's coordinate system
						 */
						inline const Vectors3& vertices() const;

						/**
						 * Returns the per-vertex normals of the mesh's faces.
						 * @return The mesh's normals, defined in the mesh's coordinate system, one for each vertex
						 */
						inline const Vectors3& perVertexNormals() const;

						/**
						 * Returns the indices of the vertices representing the mesh's surface triangles, three indices define one triangle, always a multiple of three.
						 * @return The indices of the mesh's triangles, empty if unknown
						 */
						inline const Indices32& triangleIndices() const;

					protected:

						/// The unique id of this mesh.
						Index32 meshId_ = Index32(-1);

						/// The type of the mesh.
						MeshType meshType_ = MT_UNKNOWN;

						/// The transformation between mesh and world.
						HomogenousMatrix4 world_T_mesh_ = HomogenousMatrix4(false);

						/// The vertices representing the mesh, defined in the mesh's coordinate system.
						Vectors3 vertices_;

						/// The per-vertex normals of the mesh, one for each vertex.
						Vectors3 perVertexNormals_;

						/// The indices of the vertices representing the mesh's surface triangles, three indices define one triangle, always a multiple of three.
						Indices32 triangleIndices_;
				};

				/**
				 * Definition of a shared pointer for Mesh objects.
				 */
				typedef std::shared_ptr<Mesh> SharedMesh;

				/**
				 * Definition of a vector holding meshes.
				 */
				typedef std::vector<SharedMesh> SharedMeshes;

			public:

				/**
				 * Creates a new scene element object with several given meshes.
				 * @param meshes The meshes of the new scene element, at least one
				 */
				explicit inline SceneElementMeshes(SharedMeshes meshes);

				/**
				 * Returns all meshes of this scene element object, may be empty.
				 * @return The scene element's meshes
				 */
				inline const SharedMeshes& meshes() const;

			protected:

				/// The meshes stores in this scene element object.
				SharedMeshes meshes_;
		};

		/**
		 * This class implements a scene element holding depth information.
		 */
		class SceneElementDepth : public SceneElement
		{
			public:

				/**
				 * Creates a new scene element object with depth information.
				 * @param camera The camera profile defining the projection of the depth image, must be valid
				 * @param device_T_depth The transformation between depth image and device, must be valid
				 * @param depth The depth image, must be valid
				 * @param confidence The confidence map, one entry for each pixel in the depth image, nullptr if unknown
				 */
				inline SceneElementDepth(SharedAnyCamera camera, const HomogenousMatrix4& device_T_depth, std::shared_ptr<Frame> depth, std::shared_ptr<Frame> confidence = nullptr);

				/**
				 * Returns the camera profile of the depth image.
				 * @return The depth image's camera profile
				 */
				inline SharedAnyCamera camera() const;

				/**
				 * Returns the transformation between depth image and the device.
				 * @return The scene element's transformation between depth image and device
				 */
				inline const HomogenousMatrix4& device_T_depth() const;

				/**
				 * Returns the depth image.
				 * @param confidence Optional returning confidence map, will be invalid if unknown; nullptr if not of interest
				 * @return The scene element's depth image
				 */
				inline std::shared_ptr<Frame> depth(std::shared_ptr<Frame>* confidence = nullptr) const;

			protected:

				/// The camera profile defining the projection of the depth image.
				SharedAnyCamera camera_;

				/// The transformation between depth image and the device.
				HomogenousMatrix4 device_T_depth_ = HomogenousMatrix4(false);

				/// The scene element's depth image.
				std::shared_ptr<Frame> depth_;

				/// The scene element's confidence map, one entry for each pixel in the depth image, if known.
				std::shared_ptr<Frame> confidence_;
		};

		/**
		 * This class implements a scene element holding room objects.
		 */
		class SceneElementRoom : public SceneElement
		{
			public:

				/**
				 * This class implemenets the base class for all room objects.
				 */
				class OCEAN_DEVICES_EXPORT RoomObject
				{
					public:

						/**
						 * Definition of individual room object types.
						 */
						enum ObjectType : uint32_t
						{
							/// Undefined type.
							OT_UNDEFINED = 0u,
							/// The object is a planar room object.
							OT_PLANAR,
							/// The object is a volumetric room object.
							OT_VOLUMETRIC
						};

					protected:

						/**
						 * Creates a new room object.
						 * @param objectType The type of the object, must be valid
						 * @param identifier The unique identifier of the object
						 * @param confidence The confidence of the object, with range [0, 1], higher is better
						 * @param world_T_object The transformation between object and world, must be valid
						 * @param dimension The dimension of the object
						 */
						inline RoomObject(const ObjectType objectType, std::string&& identifier, const float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension);

					public:

						/**
						 * Returns the type of this object.
						 * @return The object's type
						 */
						inline ObjectType objectType() const;

						/**
						 * Returns the unique identifier of this object.
						 * @return The object's identifier
						 */
						inline const std::string& identifier() const;

						/**
						 * Returns the confidence of this object.
						 * @return The object's confidence, with range [0, 1], higher is better
						 */
						inline float confidence() const;

						/**
						 * Returns the transformation between this object and world.
						 * @return The object's transformation
						 */
						inline const HomogenousMatrix4& world_T_object() const;

						/**
						 * Sets or updates the transformation between this object and world.
						 * @param world_T_object The transformation to be set, must be valid
						 */
						inline void setWorld_T_object(const HomogenousMatrix4& world_T_object);

						/**
						 * Returns the dimension of this object.
						 * @return The object's dimension
						 */
						inline const Vector3& dimension() const;

						/**
						 * Translates the type of an object to a readable string.
						 * @param objectType The object type to translate
						 * @return The readable string
						 */
						static std::string translateObjectType(const ObjectType objectType);

						/**
						 * Translates the readable string of an object type to an object type value.
						 * @param objectType The readable string to translate
						 * @return The object type value, OT_UNDEFINED if invalid
						 */
						static ObjectType translateObjectType(const std::string& objectType);

					protected:

						/// The type of this object.
						ObjectType objectType_ = OT_UNDEFINED;

						/// The unique identifier of this object.
						std::string identifier_;

						/// The confidence of this object, with range [0, 1], higher is better.
						float confidence_ = 0.0f;

						/// The transformation between this object and world.
						HomogenousMatrix4 world_T_object_ = HomogenousMatrix4(false);

						/// The dimension of this object.
						Vector3 dimension_ = Vector3(0, 0, 0);
				};

				/**
				 * This class implements a room object which is planar/flat.
				 */
				class OCEAN_DEVICES_EXPORT PlanarRoomObject : public RoomObject
				{
					public:

						/**
						 * Definition of individual types of planar objects.
						 */
						enum PlanarType : uint32_t
						{
							/// The type is unknown.
							PT_UNKNOWN = 0u,
							/// The object is a planar wall.
							PT_WALL,
							/// The object is a door.
							PT_DOOR,
							/// The object is a window.
							PT_WINDOW,
							/// The object is an opening.
							PT_OPENING,
							/// The object is a floor.
							PT_FLOOR,
							/// The end type.
							PT_END
						};

					public:

						/**
						 * Creates a new room object.
						 * @param identifier The unique identifier of the object
						 * @param planarType The type of the planar object, must be valid
						 * @param confidence The confidence of the object, with range [0, 1], higher is better
						 * @param world_T_object The transformation between object and world, must be valid
						 * @param dimension The dimension of the object
						 */
						inline PlanarRoomObject(std::string&& identifier, const PlanarType planarType, const float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension);

						/**
						 * Returns the planar type of this object.
						 * @return The object's planar type
						 */
						inline PlanarType planarType() const;

						/**
						 * Translates the planar type to a readable string.
						 * @param planarType The type to translate
						 * @return The translated type
						 */
						static std::string translatePlanarType(const PlanarType planarType);

						/**
						 * Translates the readable stirng of a planar type to the corresponding value.
						 * @param planarType The type to translate
						 * @return The translated type
						 */
						static PlanarType translatePlanarType(const std::string& planarType);

					protected:

						/// The planar type of this object.
						PlanarType planarType_ = PT_UNKNOWN;
				};

				/**
				 * This class implements a room object which is volumetric.
				 */
				class OCEAN_DEVICES_EXPORT VolumetricRoomObject : public RoomObject
				{
					public:

						/**
						 * Definition of individual types of volumetric objects.
						 */
						enum VolumetricType : uint32_t
						{
							/// The type is unknown.
							VT_UNKNOWN = 0u,
							/// The object is a storage.
							VT_STORAGE,
							/// The object is a refrigerator.
							VT_REFRIGERATOR,
							/// The object is a stove.
							VT_STOVE,
							/// The object is a bed.
							VT_BED,
							/// The object is a sink.
							VT_SINK,
							/// The object is a washer/driver.
							VT_WASHER_DRYER,
							/// The object is a toilet.
							VT_TOILET,
							/// The object is a bathtub.
							VT_BATHTUB,
							/// The object is an oven.
							VT_OVEN,
							/// The object is a dishwasher.
							VT_DISHWASHER,
							/// The object is a table.
							VT_TABLE,
							/// The object is a sofa.
							VT_SOFA,
							/// The object is a char.
							VT_CHAIR,
							/// The object is a fire place.
							VT_FIREPLACE,
							/// The object is a television.
							VT_TELEVISION,
							/// The object is stairs.
							VT_STAIRS,
							/// The end type.
							VT_END
						};

					protected:

						/**
						 * Definition of an unordered map mapping readable strings to volumetric types.
						 */
						typedef std::unordered_map<std::string, VolumetricType> VolumetricTypeMap;

					public:

						/**
						 * Creates a new room object.
						 * @param identifier The unique identifier of the object
						 * @param volumetricType The type of the volumetric object, must be valid
						 * @param confidence The confidence of the object, with range [0, 1], higher is better
						 * @param world_T_object The transformation between object and world, must be valid
						 * @param dimension The dimension of the object
						 */
						inline VolumetricRoomObject(std::string&& identifier, const VolumetricType volumetricType, float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension);

						/**
						 * Returns the volumetric type of this object.
						 * @return The object's volumetric type
						 */
						inline VolumetricType volumetricType() const;

						/**
						 * Translates the volumetric type to a readable string.
						 * @param volumetricType The volumetric type to translate
						 * @return The translated type
						 */
						static std::string translateVolumetricType(const VolumetricType volumetricType);

						/**
						 * Translates the readable string of a volumetric type to the corresponding value.
						 * @param volumetricType The volumetric type to translate
						 * @return The translated type
						 */
						static VolumetricType translateVolumetricType(const std::string& volumetricType);

					protected:

						/// The volumetric type of this object.
						VolumetricType volumetricType_ = VT_UNKNOWN;
				};

				/**
				 * Definition of a shared pointer holding a room object.
				 */
				typedef std::shared_ptr<RoomObject> SharedRoomObject;

				/**
				 * Definition of a shared pointer holding a planar room object.
				 */
				typedef std::shared_ptr<PlanarRoomObject> SharedPlanarRoomObject;

				/**
				 * Definition of a shared pointer holding a volumetric room object.
				 */
				typedef std::shared_ptr<VolumetricRoomObject> SharedVolumetricRoomObject;

				/**
				 * Definition of a vector holding room objects.
				 */
				typedef std::vector<SharedRoomObject> SharedRoomObjects;

				/**
				 * Definition of a vector holding planar room objects.
				 */
				typedef std::vector<SharedPlanarRoomObject> SharedPlanarRoomObjects;

				/**
				 * Definition of a vector holding volumetric room objects.
				 */
				typedef std::vector<SharedVolumetricRoomObject> SharedVolumetricRoomObjects;

				/**
				 * Definition of an unordered set holding object identifiers.
				 */
				typedef std::unordered_set<std::string> RoomObjectIdentifierSet;

				/**
				 * Definitio of an unordered map mapping object identifiers to room objects.
				 */
				typedef std::unordered_map<std::string, SharedRoomObject> RoomObjectMap;

			public:

				/**
				 * Creates a new scene element object with several given room objects.
				 * @param roomObjectMap The room objects of the new scene element, may be empty.
				 * @param addedRoomObjects The identifiers of all added room objects
				 * @param removedRoomObjects The identifiers of all removed room objects
				 * @param changedRoomObjects The identifiers of all changed room objects
				 * @param updatedRoomObjects The identifiers of all updated room objects
				 */
				explicit inline SceneElementRoom(RoomObjectMap&& roomObjectMap, RoomObjectIdentifierSet&& addedRoomObjects, RoomObjectIdentifierSet&& removedRoomObjects, RoomObjectIdentifierSet&& changedRoomObjects, RoomObjectIdentifierSet&& updatedRoomObjects);

				/**
				 * Returns all room objects of this scene element object, may be empty.
				 * @return The scene element's room objects
				 */
				inline const RoomObjectMap& roomObjectMap() const;

				/**
				 * Returns the identifiers of all room object which have been added.
				 * @return Added room objects
				 */
				inline const RoomObjectIdentifierSet& addedRoomObjects() const;

				/**
				 * Returns the identifiers of all room object which have been removed.
				 * @return Removed room objects
				 */
				inline const RoomObjectIdentifierSet& removedRoomObjects() const;

				/**
				 * Returns the identifiers of all room object which have been changed.
				 * @return Changed room objects
				 */
				inline const RoomObjectIdentifierSet& changedRoomObjects() const;

				/**
				 * Returns the identifiers of all room object which have been updated.
				 * @return Updated room objects
				 */
				inline const RoomObjectIdentifierSet& updatedRoomObjects() const;

			protected:

				/// The room objects in this scene element object.
				RoomObjectMap roomObjectMap_;

				/// The identifiers of all added room objects.
				RoomObjectIdentifierSet addedRoomObjects_;

				/// The identifiers of all removed room objects.
				RoomObjectIdentifierSet removedRoomObjects_;

				/// The identifiers of all changed room objects.
				RoomObjectIdentifierSet changedRoomObjects_;

				/// The identifiers of all changed room objects.
				RoomObjectIdentifierSet updatedRoomObjects_;
		};

		/**
		 * Definition of a shared pointer holding a scene element.
		 */
		typedef std::shared_ptr<SceneElement> SharedSceneElement;

		/**
		 * Definition of a vector holding scene elements.
		 */
		typedef std::vector<SharedSceneElement> SharedSceneElements;

		/**
		 * Definition of a sample holding one single 6DOF tracker measurement.
		 */
		class OCEAN_DEVICES_EXPORT SceneTracker6DOFSample : virtual public Tracker6DOFSample
		{
			public:

				/**
				 * Creates a new 6DOF tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation and position measurement
				 * @param orientations Sample orientation measurements
				 * @param positions Sample position measurements in meter
				 * @param sceneElements The scene elements of the new sample
				 * @param metadata Optional metadata of the new sample
				 */
				SceneTracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Positions& positions, const SharedSceneElements& sceneElements, const Metadata& metadata = Metadata());

				/**
				 * Creates a new 6DOF tracker sample.
				 * @param timestamp Sample timestamp
				 * @param referenceSystem Tracking reference system used by the underlying tracker
				 * @param objectIds Measurement unit object ids each id corresponds to a different orientation and position measurement
				 * @param orientations Sample orientation measurements
				 * @param positions Sample position measurements in meter
				 * @param sceneElements The scene elements of the new sample
				 * @param metadata Optional metadata of the new sample
				 */
				SceneTracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Positions&& positions, SharedSceneElements&& sceneElements, Metadata&& metadata = Metadata());

				/**
				 * Returns the scene elements of this sample.
				 * Scene elements can be invalid in case a pure 6-DOF pose is provided.
				 * @return The sample's scene elements, some may be invalid
				 */
				inline const SharedSceneElements& sceneElements() const;

			protected:

				/// The scene elements.
				SharedSceneElements sceneElements_;
		};

		/**
		 * Definition of a smart object reference for 6-DOF scene tracker samples.
		 */
		typedef SmartObjectRef<SceneTracker6DOFSample, Sample> SceneTracker6DOFSampleRef;

	public:

		/**
		 * Exports the determined scene elements.
		 * @param format The format of the exported data
		 * @param outputStream The output stream to which the data will be exported
		 * @param options Optional options to configure the export result
		 * @return True, if succeeded; False, if not supported
		 */
		virtual bool exportSceneElements(const std::string& format, std::ostream& outputStream, const std::string& options = std::string()) const;

		/**
		 * Definition of this device type.
		 */
		static inline DeviceType deviceTypeSceneTracker6DOF();

	protected:

		/**
		 * Creates a new 6-DOF scene tracker object.
		 * @param name The name of the 6DOF tracker, must be valid
		 */
		explicit SceneTracker6DOF(const std::string& name);

		/**
		 * Destructs a 6-DOF tracker object.
		 */
		~SceneTracker6DOF() override;
};

inline SceneTracker6DOF::SceneElement::SceneElement(const SceneElementType sceneElementType) :
	sceneElementType_(sceneElementType)
{
	// nothing to do here
}

inline SceneTracker6DOF::SceneElement::SceneElementType SceneTracker6DOF::SceneElement::sceneElementType() const
{
	return sceneElementType_;
}

inline SceneTracker6DOF::SceneElementObjectPoints::SceneElementObjectPoints(Vectors3&& objectPoints, Indices64&& objectPointIds) :
	SceneElement(SET_OBJECT_POINTS),
	objectPoints_(std::move(objectPoints)),
	objectPointIds_(std::move(objectPointIds))
{
	ocean_assert(!objectPoints_.empty());
	ocean_assert(objectPointIds_.empty() || objectPointIds_.size() == objectPoints_.size());
}

inline const Vectors3& SceneTracker6DOF::SceneElementObjectPoints::objectPoints() const
{
	ocean_assert(sceneElementType_ == SET_FEATURE_CORRESPONDENCES || sceneElementType_ == SET_OBJECT_POINTS);
	return objectPoints_;
}

inline const Indices64& SceneTracker6DOF::SceneElementObjectPoints::objectPointIds() const
{
	return objectPointIds_;
}

inline SceneTracker6DOF::SceneElementFeatureCorrespondences::SceneElementFeatureCorrespondences(Vectors3&& objectPoints, Vectors2&& imagePoints, Indices64&& objectPointIds) :
	SceneElement(SET_FEATURE_CORRESPONDENCES),
	objectPoints_(std::move(objectPoints)),
	imagePoints_(std::move(imagePoints)),
	objectPointIds_(std::move(objectPointIds))
{
	ocean_assert(!objectPoints_.empty());
	ocean_assert(imagePoints_.size() == objectPoints_.size());

	ocean_assert(objectPointIds_.empty() || objectPointIds_.size() == objectPoints_.size());
}

inline const Vectors3& SceneTracker6DOF::SceneElementFeatureCorrespondences::objectPoints() const
{
	ocean_assert(sceneElementType_ == SET_FEATURE_CORRESPONDENCES || sceneElementType_ == SET_OBJECT_POINTS);
	return objectPoints_;
}

inline const Vectors2& SceneTracker6DOF::SceneElementFeatureCorrespondences::imagePoints() const
{
	ocean_assert(sceneElementType_ == SET_FEATURE_CORRESPONDENCES);
	return imagePoints_;
}

inline const Indices64& SceneTracker6DOF::SceneElementFeatureCorrespondences::objectPointIds() const
{
	return objectPointIds_;
}

inline SceneTracker6DOF::SceneElementPlanes::Plane::Plane(const Index32 planeId, const PlaneType planeType, const HomogenousMatrix4& world_T_plane, const Box3& boundingBox, Vectors3&& boundaryVertices) :
	planeId_(planeId),
	planeType_(planeType),
	world_T_plane_(world_T_plane),
	boundingBox_(std::move(boundingBox)),
	boundaryVertices_(std::move(boundaryVertices))
{
	ocean_assert(world_T_plane_.isValid());
	ocean_assert(boundingBox_.isValid());
}

inline SceneTracker6DOF::SceneElementPlanes::Plane::Plane(const Index32 planeId, const PlaneType planeType, const HomogenousMatrix4& world_T_plane, const Box3& boundingBox, Vectors3&& vertices, Vectors2&& textureCoordinates, Indices32&& triangleIndices, Vectors3&& boundaryVertices) :
	planeId_(planeId),
	planeType_(planeType),
	world_T_plane_(world_T_plane),
	boundingBox_(std::move(boundingBox)),
	vertices_(std::move(vertices)),
	textureCoordinates_(std::move(textureCoordinates)),
	triangleIndices_(std::move(triangleIndices)),
	boundaryVertices_(std::move(boundaryVertices))
{
	ocean_assert(world_T_plane_.isValid());
	ocean_assert(boundingBox_.isValid());
	ocean_assert(textureCoordinates.empty() || textureCoordinates.size() == vertices_.size());
	ocean_assert(triangleIndices.empty() || triangleIndices.size() % 3 == 0);
}

inline Index32 SceneTracker6DOF::SceneElementPlanes::Plane::planeId() const
{
	return planeId_;
}

inline SceneTracker6DOF::SceneElementPlanes::Plane::PlaneType SceneTracker6DOF::SceneElementPlanes::Plane::planeType() const
{
	return planeType_;
}

inline const HomogenousMatrix4& SceneTracker6DOF::SceneElementPlanes::Plane::world_T_plane() const
{
	ocean_assert(world_T_plane_.isValid());
	return world_T_plane_;
}

inline Plane3 SceneTracker6DOF::SceneElementPlanes::Plane::worldPlane() const
{
	ocean_assert(world_T_plane_.isValid());

	ocean_assert(world_T_plane_.yAxis().isUnit());
	return Plane3(world_T_plane_.translation(), world_T_plane_.yAxis());
}

inline const Box3& SceneTracker6DOF::SceneElementPlanes::Plane::boundingBox() const
{
	ocean_assert(boundingBox_.isValid());
	return boundingBox_;
}

inline const Vectors3& SceneTracker6DOF::SceneElementPlanes::Plane::vertices() const
{
	return vertices_;
}

inline const Vectors2& SceneTracker6DOF::SceneElementPlanes::Plane::textureCoordinates() const
{
	return textureCoordinates_;
}

inline const Indices32& SceneTracker6DOF::SceneElementPlanes::Plane::triangleIndices() const
{
	return triangleIndices_;
}

inline const Vectors3& SceneTracker6DOF::SceneElementPlanes::Plane::boundaryVertices() const
{
	return boundaryVertices_;
}

inline SceneTracker6DOF::SceneElementPlanes::SceneElementPlanes(Planes&& planes) :
	SceneElement(SET_PLANES),
	planes_(std::move(planes))
{
	ocean_assert(!planes_.empty());
}

inline const SceneTracker6DOF::SceneElementPlanes::Planes& SceneTracker6DOF::SceneElementPlanes::planes() const
{
	return planes_;
}

inline SceneTracker6DOF::SceneElementMeshes::Mesh::Mesh(const Index32 meshId, const HomogenousMatrix4& world_T_mesh, Vectors3&& vertices, Vectors3&& perVertexNormals, Indices32&& triangleIndices) :
	meshId_(meshId),
	world_T_mesh_(world_T_mesh),
	vertices_(std::move(vertices)),
	perVertexNormals_(std::move(perVertexNormals)),
	triangleIndices_(std::move(triangleIndices))
{
	ocean_assert(triangleIndices_.size() % 3 == 0);
	ocean_assert(vertices_.size() == perVertexNormals_.size());
}

inline Index32 SceneTracker6DOF::SceneElementMeshes::Mesh::meshId() const
{
	return meshId_;
}

inline const HomogenousMatrix4& SceneTracker6DOF::SceneElementMeshes::Mesh::world_T_mesh() const
{
	return world_T_mesh_;
}

inline const Vectors3& SceneTracker6DOF::SceneElementMeshes::Mesh::vertices() const
{
	return vertices_;
}

inline const Vectors3& SceneTracker6DOF::SceneElementMeshes::Mesh::perVertexNormals() const
{
	return perVertexNormals_;
}

inline const Indices32& SceneTracker6DOF::SceneElementMeshes::Mesh::triangleIndices() const
{
	return triangleIndices_;
}

inline SceneTracker6DOF::SceneElementMeshes::SceneElementMeshes(SharedMeshes meshes) :
	SceneElement(SET_MESHES),
	meshes_(std::move(meshes))
{
	ocean_assert(!meshes_.empty());
}

inline const SceneTracker6DOF::SceneElementMeshes::SharedMeshes& SceneTracker6DOF::SceneElementMeshes::meshes() const
{
	return meshes_;
}

inline SceneTracker6DOF::SceneElementRoom::RoomObject::RoomObject(const ObjectType objectType, std::string&& identifier, const float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension) :
	objectType_(objectType),
	identifier_(std::move(identifier)),
	confidence_(confidence),
	world_T_object_(world_T_object),
	dimension_(dimension)
{
	ocean_assert(objectType != OT_UNDEFINED);
	ocean_assert(!identifier_.empty());
	ocean_assert(confidence >= 0.0f && confidence <= 1.0f);
	ocean_assert(world_T_object_.isValid());
}

inline SceneTracker6DOF::SceneElementRoom::RoomObject::ObjectType SceneTracker6DOF::SceneElementRoom::RoomObject::objectType() const
{
	return objectType_;
}

inline const std::string& SceneTracker6DOF::SceneElementRoom::RoomObject::identifier() const
{
	return identifier_;
}

inline float SceneTracker6DOF::SceneElementRoom::RoomObject::confidence() const
{
	return confidence_;
}

inline const HomogenousMatrix4& SceneTracker6DOF::SceneElementRoom::RoomObject::world_T_object() const
{
	return world_T_object_;
}

inline void SceneTracker6DOF::SceneElementRoom::RoomObject::setWorld_T_object(const HomogenousMatrix4& world_T_object)
{
	ocean_assert(world_T_object.isValid());
	world_T_object_ = world_T_object;
}

inline const Vector3& SceneTracker6DOF::SceneElementRoom::RoomObject::dimension() const
{
	return dimension_;
}

inline SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarRoomObject(std::string&& identifier, const PlanarType planarType, const float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension) :
	RoomObject(OT_PLANAR, std::move(identifier), confidence, world_T_object, dimension),
	planarType_(planarType)
{
	ocean_assert(planarType_ != PT_UNKNOWN);
}

inline SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::planarType() const
{
	return planarType_;
}

inline SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricRoomObject(std::string&& identifier, const VolumetricType volumetricType, const float confidence, const HomogenousMatrix4& world_T_object, const Vector3& dimension) :
	RoomObject(OT_VOLUMETRIC, std::move(identifier), confidence, world_T_object, dimension),
	volumetricType_(volumetricType)
{
	ocean_assert(volumetricType_ != VT_UNKNOWN);
}

inline SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::volumetricType() const
{
	return volumetricType_;
}

inline SceneTracker6DOF::SceneElementRoom::SceneElementRoom(RoomObjectMap&& roomObjectMap, RoomObjectIdentifierSet&& addedRoomObjects, RoomObjectIdentifierSet&& removedRoomObjects, RoomObjectIdentifierSet&& changedRoomObjects, RoomObjectIdentifierSet&& updatedRoomObjects) :
	SceneElement(SET_ROOM),
	roomObjectMap_(std::move(roomObjectMap)),
	addedRoomObjects_(std::move(addedRoomObjects)),
	removedRoomObjects_(std::move(removedRoomObjects)),
	changedRoomObjects_(std::move(changedRoomObjects)),
	updatedRoomObjects_(std::move(updatedRoomObjects))
{
	// nothing to do here
}

inline const SceneTracker6DOF::SceneElementRoom::RoomObjectMap& SceneTracker6DOF::SceneElementRoom::roomObjectMap() const
{
	return roomObjectMap_;
}

inline const SceneTracker6DOF::SceneElementRoom::RoomObjectIdentifierSet& SceneTracker6DOF::SceneElementRoom::addedRoomObjects() const
{
	return addedRoomObjects_;
}

inline const SceneTracker6DOF::SceneElementRoom::RoomObjectIdentifierSet& SceneTracker6DOF::SceneElementRoom::removedRoomObjects() const
{
	return removedRoomObjects_;
}

inline const SceneTracker6DOF::SceneElementRoom::RoomObjectIdentifierSet& SceneTracker6DOF::SceneElementRoom::changedRoomObjects() const
{
	return changedRoomObjects_;
}

inline const SceneTracker6DOF::SceneElementRoom::RoomObjectIdentifierSet& SceneTracker6DOF::SceneElementRoom::updatedRoomObjects() const
{
	return updatedRoomObjects_;
}

inline SceneTracker6DOF::SceneElementDepth::SceneElementDepth(SharedAnyCamera camera, const HomogenousMatrix4& device_T_depth, std::shared_ptr<Frame> depth, std::shared_ptr<Frame> confidence) :
	SceneElement(SET_DEPTH),
	camera_(std::move(camera)),
	device_T_depth_(device_T_depth),
	depth_(std::move(depth)),
	confidence_(std::move(confidence))
{
	ocean_assert(camera_ && depth_ && device_T_depth_.isValid());
}

inline SharedAnyCamera SceneTracker6DOF::SceneElementDepth::camera() const
{
	ocean_assert(camera_);
	return camera_;
}

inline const HomogenousMatrix4& SceneTracker6DOF::SceneElementDepth::device_T_depth() const
{
	ocean_assert(device_T_depth_.isValid());
	return device_T_depth_;
}

inline std::shared_ptr<Frame> SceneTracker6DOF::SceneElementDepth::depth(std::shared_ptr<Frame>* confidence) const
{
	if (confidence != nullptr)
	{
		*confidence = confidence_;
	}

	ocean_assert(depth_);
	return depth_;
}

inline const SceneTracker6DOF::SharedSceneElements& SceneTracker6DOF::SceneTracker6DOFSample::sceneElements() const
{
	return sceneElements_;
}

inline SceneTracker6DOF::DeviceType SceneTracker6DOF::deviceTypeSceneTracker6DOF()
{
	return Device::DeviceType(DEVICE_TRACKER, SCENE_TRACKER_6DOF);
}

}

}

#endif // META_OCEAN_DEVICES_SCENE_TRACKER_6DOF_H
