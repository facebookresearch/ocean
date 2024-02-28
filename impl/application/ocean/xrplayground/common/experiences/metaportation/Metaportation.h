// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/maptexturing/NewTextureGenerator.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the base for all Metaportation experiences.
 * @ingroup xrplayground
 */
class Metaportation
{
	protected:

		/// Re-definition of a textured mesh.
		using TexturedMesh = Tracking::MapTexturing::NewTextureGenerator::TexturedMesh;

		/// Re-definition of a textured mesh map.
		using TexturedMeshMap = Tracking::MapTexturing::NewTextureGenerator::TexturedMeshMap;

		/**
		 * This class holds the relevant information for textured meshes.
		 */
		class Meshes
		{
			public:

				/**
				 * Default constructor.
				 */
				Meshes() = default;

				/**
				 * Creates a new meshes object.
				 * @param world_T_meshes The transformation between meshes and world
				 * @param texturedMeshMap The map mapping block ids to meshes
				 * @param textureFrame The texture assocated with the meshes
				 */
				inline Meshes(const HomogenousMatrix4& world_T_meshes, TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame);

				/**
				 * Returns whether this object holds valid meshes.
				 * @return True, if so
				 */
				inline bool isValid() const;

			public:

				/// The transformation between meshes and world.
				HomogenousMatrix4 world_T_meshes_ = HomogenousMatrix4(false);

				/// The map mapping block ids to meshes.
				TexturedMeshMap texturedMeshMap_;

				/// The texture associated with the meshes.
				Frame textureFrame_;
		};

		/**
		 * This class implements a manager for textured meshes.
		 */
		class MeshesManager
		{
			protected:

				/**
				 * Definition of an unordered map mapping texture ids to textures.
				 */
				typedef std::unordered_map<Index32, Frame> TextureMap;

				/**
				 * Definition of an unordered map mapping block ids to texture ids.
				 */
				typedef std::unordered_map<VectorI3, Index32, VectorI3> TextureIdMap;

				/**
				 * Definition of an unordered map mapping texture ids to usage counters.
				 */
				typedef std::unordered_map<Index32, Index32> TextureUsageMap;

			public:

				/**
				 * Updates several meshes.
				 * @param world_T_meshes The latest transformation between meshes and world
				 * @param texturedMeshMap The map mapping block ids to meshes
				 * @param textureFrame The texture assocated with the meshes
				 */
				void updateMeshes(const HomogenousMatrix4& world_T_meshes, TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame);

				/**
				 * Returns the textured meshes and their corresponding textures.
				 * @param meshesGroup The resulting group of meshes
				 */
				void latestMeshes(std::vector<Meshes>& meshesGroup) const;

				/**
				 * Returns the number of textures the manager currently holds.
				 * @return The manager's number of textures
				 */
				size_t numberTextures() const;

				/**
				 * Returns the number of meshes the manager currently holds.
				 * @return The manager's number of meshes
				 */
				size_t numberMeshes() const;

			protected:

				/// The latest transformation between meshes and world.
				HomogenousMatrix4 world_T_meshes_ = HomogenousMatrix4(false);

				/// Unique counter for texture ids.
				Index32 textureIdCounter_ = 0u;

				/// The map mapping texture ids to textures.
				TextureMap textureMap_;

				/// The map mapping block ids to texture ids.
				TextureIdMap textureIdMap_;

				/// The map mapping texture ids to usage counters.
				TextureUsageMap textureUsageMap_;

				/// The map mapping block ids to texture meshes.
				TexturedMeshMap texturedMeshMap_;

				/// The manager's lock.
				mutable Lock lock_;
		};

		/**
		 * This class provides the memory pointer to a given memory block which can optional be compressed.
		 */
		class ScopedUncompressedMemory
		{
			public:

				/**
				 * Creates a new memory object.
				 * @param data The memory data, may be compressed, must be valid as long as this object exists
				 * @param size The size of the memory in bytes, with range [1, infinity)
				 */
				ScopedUncompressedMemory(const void* data, const size_t size);

				/**
				 * Returns the memory pointer to the uncompressed memory.
				 * @return The pointer to the uncompressed memory
				 */
				inline const void* data() const;

				/**
				 * Returns the size of the uncompressed memory.
				 * @return The uncompressed memory size, in bytes
				 */
				inline size_t size() const;

			protected:

				/// The pointer to the uncompressed memory
				const void* data_;

				/// The uncompressed memory size, in bytes.
				size_t size_;

				/// The buffer of the uncompressed memory, in case the input memory was compressed.
				std::vector<uint8_t> uncompressedMemoryBuffer_;
		};

		/// The unique tag for a transformation.
		static constexpr uint64_t transformationTag_ = IO::Tag::string2tag("_OCNHTR_");

		/// The unique tag for the translation between world and floor.
		static constexpr uint64_t worldFloorTag_ = IO::Tag::string2tag("_OCNWFL_");

		/// The unique tag for a textured mesh.
		static constexpr uint64_t meshTag_ = IO::Tag::string2tag("_OCNMES_");

		/// The unique tag for a map.
		static constexpr uint64_t mapTag_ = IO::Tag::string2tag("_OCNMAP_");

		/// The unique tag for object points.
		static constexpr uint64_t objectPointsTag_ = IO::Tag::string2tag("_OCNOPT_");

		/// The unique tag for a network port.
		static constexpr uint64_t portTag_ = IO::Tag::string2tag("_OCNPRT_");

		/// The unique tag for a game name.
		static constexpr uint64_t gameTag_ = IO::Tag::string2tag("_OCNGAM_");

		/**
		 * Definition of a map mapping bock ids to rendering objects.
		 */
		typedef std::unordered_map<VectorI3, Rendering::TransformRef, VectorI3> RenderingBlockMap;

	protected:

		/**
		 * Writes a 6-DOF transformation to a bitstream.
		 * @param transformation The transformation to write, must be valid
		 * @param bitstream The output stream to which the transformation will be written
		 * @return True, if succeeded
		 */
		static bool writeTransformationToStream(const HomogenousMatrix4& transformation, IO::OutputBitstream& bitstream);

		/**
		 * Writes a mesh to a bitstream.
		 * @param meshes The meshes to write
		 * @param bitstream The bitstream to which the mesh will be written
		 * @return True, if succeeded
		 */
		static bool writeMeshesToStream(const Meshes& meshes, IO::OutputBitstream& bitstream);

		/**
		 * Writes 3D object points to a bitstream.
		 * @param objectPoints The 3D object points
		 * @param objectPointIds The ids of the 3D object points, one for each object point
		 * @param bitstream The bitstream to which the points will be written
		 * @return True, if succeeded
		 */
		static bool writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream);

		/**
		 * Writes a 3D vector to a bitstream.
		 * @param vector The vector to write, must be valid
		 * @param tag The tag to be used, must be valid
		 * @param bitstream The output stream to which the transformation will be written
		 * @return True, if succeeded
		 */
		static bool writeVector3(const Vector3& vector, const unsigned long long tag, IO::OutputBitstream& bitstream);

		/**
		 * Writes 2D vectors to a bitstream.
		 * @param vectors The vectors to write
		 * @param bitstream The bitstream to which the vectors will be written
		 * @return True, if succeeded
		 */
		static bool writeVectors2(const Vectors2& vectors, IO::OutputBitstream& bitstream);

		/**
		 * Writes 3D vectors to a bitstream.
		 * @param vectors The vectors to write
		 * @param bitstream The bitstream to which the vectors will be written
		 * @return True, if succeeded
		 */
		static bool writeVectors3(const Vectors3& vectors, IO::OutputBitstream& bitstream);

		/**
		 * Reads a 6-DOF transformation from a bitstream.
		 * @param bitstream The input stream from which the transformation will be read
		 * @param transformation The resulting transformation
		 * @return True, if succeeded
		 */
		static bool readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& transformation);

		/**
		 * Reads a mesh from a bitstream.
		 * @param bitstream The input stream from which the pose will be read
		 * @param meshes The resulting meshes
		 * @return True, if succeeded
		 */
		static bool readMeshesFromStream(IO::InputBitstream& bitstream, Meshes& meshes);

		/**
		 * Returns the id of the mesh which in a bitstream.
		 * @param bitstream The input stream in which the mesh id is determined, the position in the stream will not change
		 * @param meshId The resulting unique id of the mesh
		 * @return True, if succeeded
		 */
		static bool determineMeshIdInStream(IO::InputBitstream& bitstream, Index32& meshId);

		/**
		 * Reads 3D object points from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the object points, one for each object point
		 * @return True, if succeeded
		 */
		static bool readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds);

		/**
		 * Reads a 3D vector from a bitstream.
	 	 * @param bitstream The bitstream from which the vector will be read
		 * @param tag The expected tag
		 * @param vector The resulting vector
		 * @return True, if succeeded
		 */
		static bool readVector3(IO::InputBitstream& bitstream, const unsigned long long tag, Vector3& vector);

		/**
		 * Reads 2D vectors from a bitstream.
		 * @param bitstream The bitstream from which the vectors will be read
		 * @param vectors The resulting vectors
		 * @param maximalVectors The maximal number of vectors the bitstream is allowed to store, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool readVectors2(IO::InputBitstream& bitstream, Vectors2& vectors, const unsigned int maximalVectors = (unsigned int)(-1));

		/**
		 * Reads 3D vectors from a bitstream.
		 * @param bitstream The bitstream from which the vectors will be read
		 * @param vectors The resulting vectors
		 * @param maximalVectors The maximal number of vectors the bitstream is allowed to store, with range [0, infinity)
		 * @return True, if succeeded
		 */
		static bool readVectors3(IO::InputBitstream& bitstream, Vectors3& vectors, const unsigned int maximalVectors = (unsigned int)(-1));
};

inline Metaportation::Meshes::Meshes(const HomogenousMatrix4& world_T_meshes, Tracking::MapTexturing::NewTextureGenerator::TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame) :
	world_T_meshes_(world_T_meshes),
	texturedMeshMap_(std::move(texturedMeshMap)),
	textureFrame_(std::move(textureFrame))
{
	// nothing to do here
}

inline bool Metaportation::Meshes::isValid() const
{
	ocean_assert(world_T_meshes_.isValid() == !texturedMeshMap_.empty());

	return !texturedMeshMap_.empty();
}

inline const void* Metaportation::ScopedUncompressedMemory::data() const
{
	return data_;
}

inline size_t Metaportation::ScopedUncompressedMemory::size() const
{
	return size_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_METAPORTATION_H
