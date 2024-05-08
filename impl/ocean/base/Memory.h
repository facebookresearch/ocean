/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_MEMORY_H
#define META_OCEAN_BASE_MEMORY_H

#include "ocean/base/Base.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

/**
 * This class implements an object able to allocate memory.
 * @ingroup base
 */
class Memory
{
	public:

		/**
		 * Creates a new object without any allocated memory.
		 */
		Memory() = default;

		/**
		 * Move constructor.
		 * @param memory The memory object to be moved
		 */
		inline Memory(Memory&& memory) noexcept;

		/**
		 * Creates a new object and allocates a specified amount of memory.
		 * This function allows to allocated memory with a specific byte alignment, so that the start address of the memory is a multiple of the specified alignment.
		 * @param size The size of the memory to be allocated in bytes, with range [0, infinity)
		 * @param alignment The memory byte alignment of the allocated memory, in bytes, with range [1, infinity)
		 */
		explicit inline Memory(const size_t size, const size_t alignment = size_t(1));

		/**
		 * Creates a new object and uses externally allocated writable memory.
		 * This object will not be the owner of the memory, ensure that the external memory exists as long as this object exists.
		 * @param useData The external allocated memory which will be used, must be valid
		 * @param size The size of the external allocated memory, in bytes, with range [1, infinity)
		 */
		inline Memory(void* useData, const size_t size);

		/**
		 * Creates a new object and uses externally allocated read-only memory.
		 * This object will not be the owner of the memory, ensure that the external memory exists as long as this object exists.
		 * @param useData The external allocated memory which will be used, must be valid
		 * @param size The size of the external allocated memory, in bytes, with range [1, infinity)
		 */
		inline Memory(const void* useData, const size_t size);

		/**
		 * Releases the object and frees the memory if it holds any memory.
		 */
		inline ~Memory();

		/**
		 * Returns the pointer to the read-only memory which is allocated by this object.
		 * @return The memory allocated by this object, nullptr if no memory is allocated
		 */
		[[nodiscard]] inline const void* constdata() const;

		/**
		 * Returns the pointer to the writable memory which is allocated by this object.
		 * @return The memory allocated by this object, nullptr if no memory is allocated
		 */
		[[nodiscard]] inline void* data();

		/**
		 * Returns the pointer to the read-only memory which is allocated by this object.
		 * @param checkAlignment True, to apply an assert checking the byte alignment of the specified data type
		 * @return The memory allocated by this object, nullptr if no memory is allocated
		 * @tparam T The data type of the returning pointer
		 */
		template <typename T>
		[[nodiscard]] inline const T* constdata(const bool checkAlignment = true) const;

		/**
		 * Returns the pointer to the writable memory which is allocated by this object.
		 * @param checkAlignment True, to apply an assert checking the byte alignment of the specified data type
		 * @return The memory allocated by this object, nullptr if no memory is allocated
		 * @tparam T The data type of the returning pointer
		 */
		template <typename T>
		[[nodiscard]] inline T* data(const bool checkAlignment = true);

		/**
		 * Returns whether a specified memory range is entirely enclosed inside the memory managed by this object.
		 * @param start The (inclusive) pointer to the start of the memory range to be checked, must be valid
		 * @param size The size of the memory range to be checked, in bytes, with range [0, infinity)
		 * @return True, if so or if 'size == 0'
		 */
		[[nodiscard]] inline bool isInside(const void* const start, const size_t size) const;

		/**
		 * Returns whether a specified memory range is entirely enclosed inside the memory managed by this object.
		 * @param start The (inclusive) pointer to the start of the memory range to be checked, must be valid
		 * @param end The (exclusive) pointer to the first byte after the memory range to be checked, must be valid, with range [start, infinity)
		 * @return True, if so or if 'start == end'
		 */
		[[nodiscard]] inline bool isInside(const void* const start, const void* const end) const;

		/**
		 * Explicitly frees (releases) the memory before this object is released.
		 */
		inline void free();

		/**
		 * Returns the size of the memory in bytes.
		 * @return The memory's size in bytes, with range [0, infinity)
		 */
		[[nodiscard]] inline size_t size() const;

		/**
		 * Returns whether this object owns the memory.
		 * @return True, if the memory is owned; False, if the memory is owned externally
		 */
		[[nodiscard]] inline bool isOwner() const;

		/**
		 * Returns whether this object provides read-only memory only.
		 * @return True, if the memory is read-only; False, if the memory is writable
		 */
		[[nodiscard]] inline bool isReadOnly() const;

		/**
		 * Returns whether this object holds any memory.
		 * @return True, if so
		 */
		[[nodiscard]] inline bool isNull() const;

		/**
		 * Returns whether this object holds any memory.
		 * @return True, if so
		 */
		[[nodiscard]] explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param memory The memory object to be moved
		 */
		inline Memory& operator=(Memory&& memory) noexcept;

		/**
		 * Creates a new object and allocates enough memory necessary for 'elements' of type T.
		 * @param elements The number of elements of type T for which the new object will allocate memory, with range [0, infinity)
		 * @return The new memory object
		 * @tparam T The data type of the each element
		 */
		template <typename T>
		[[nodiscard]] static inline Memory create(const size_t elements);

		/**
		 * Copies a block of memory using a worker object to speed up the process.
		 * @param target The target memory receiving the memory
		 * @param source The source memory block
		 * @param size Number of bytes to be copied
		 * @param worker Optional worker object
		 */
		static inline void memcpy(void* target, const void* source, const unsigned int size, Worker* worker = nullptr);

		/**
		 * Sets the value of a given memory block using a worker object to speed up the process.
		 * @param data Memory block to be set
		 * @param value The value to be set, only the first byte will be used as value
		 * @param size Number of bytes to be set
		 * @param worker Optional worker object
		 */
		static inline void memset(void* data, const int value, const unsigned int size, Worker* worker = nullptr);

	protected:

		/**
		 * The disabled copy constructor.
		 * @param memory The memory object that would be copied
		 */
		Memory(const Memory& memory) = delete;

		/**
		 * The disabled assign operator.
		 * @param memory The memory object that would be assigned
		 */
		Memory& operator=(const Memory& memory) = delete;

		/**
		 * Copies a subset of the a memory block.
		 * @param target The target memory
		 * @param source The source memory
		 * @param firstByte First byte to be copied
		 * @param numberBytes Number of bytes to be copied
		 */
		static inline void memcpySubset(uint8_t* target, const uint8_t* source, const unsigned int firstByte, const unsigned int numberBytes);

		/**
		 * Sets a subset of a given memory block.
		 * @param data Memory block to be set
		 * @param value the value to be set
		 * @param firstByte First byte to be set
		 * @param numberBytes Number of bytes to be set
		 */
		static inline void memsetSubset(uint8_t* data, const int value, const unsigned int firstByte, const unsigned int numberBytes);

	protected:

		/// The pointer to the memory which is allocated and owned by this object, this pointer is pointing to the memory which needs to be freed when disposing the memory object.
		void* allocatedData_ = nullptr;

		/// The pointer to the read-only aligned memory which is reported to be the actual memory pointer, this memory pointer must not be freed when disposing the memory object.
		const void* constAlignedData_ = nullptr;

		/// The pointer to the writable aligned memory which is reported to be the actual memory pointer, this memory pointer must not be freed when disposing the memory object.
		void* alignedData_ = nullptr;

		/// The size of the actual usable memory in bytes, with range [0, infinity)
		size_t size_ = 0;
};

inline Memory::Memory(Memory&& memory) noexcept :
	Memory()
{
	*this = std::move(memory);
}

inline Memory::Memory(const size_t size, const size_t alignment) :
	Memory()
{
	ocean_assert(alignment >= size_t(1));

	if (size != size_t(0))
	{
		static_assert(sizeof(uint8_t) == 1, "Invalid data type!");

		allocatedData_ = malloc(size + alignment);
		ocean_assert(allocatedData_ != nullptr);

		if (allocatedData_ != nullptr)
		{
			const size_t alignmentOffset = (alignment - (size_t(allocatedData_) % alignment)) % alignment;

			ocean_assert(alignmentOffset < alignment);
			ocean_assert((size_t(allocatedData_) + alignmentOffset) % alignment == size_t(0));

			alignedData_ = (void*)(((uint8_t*)allocatedData_) + alignmentOffset);
			ocean_assert(alignedData_ >= allocatedData_);

			constAlignedData_ = (const void*)(alignedData_);

			size_ = size;
		}

		ocean_assert(isOwner());
		ocean_assert(!isReadOnly());
	}
}

inline Memory::Memory(void* useData, const size_t size) :
	alignedData_(useData),
	size_(size)
{
	ocean_assert(useData != nullptr);
	ocean_assert(size > 0);

	constAlignedData_ = (const void*)(alignedData_);

	ocean_assert(allocatedData_ == nullptr);

	ocean_assert(!isOwner());
	ocean_assert(!isReadOnly());
}

inline Memory::Memory(const void* useData, const size_t size) :
	constAlignedData_(useData),
	size_(size)
{
	ocean_assert(useData != nullptr);
	ocean_assert(size > 0);

	ocean_assert(allocatedData_ == nullptr);
	ocean_assert(alignedData_ == nullptr);

	ocean_assert(!isOwner());
	ocean_assert(isReadOnly());
}

inline Memory::~Memory()
{
	free();
}

inline const void* Memory::constdata() const
{
	return constAlignedData_;
}

inline void* Memory::data()
{
	return alignedData_;
}

template<typename T>
inline const T* Memory::constdata(const bool checkAlignment) const
{
	if (checkAlignment)
	{
		ocean_assert((size_t(constAlignedData_) % sizeof(T)) == 0);
	}

	return reinterpret_cast<const T*>(constAlignedData_);
}

template<typename T>
inline T* Memory::data(const bool checkAlignment)
{
	if (checkAlignment)
	{
		ocean_assert((size_t(alignedData_) % sizeof(T)) == 0);
	}

	return reinterpret_cast<T*>(alignedData_);
}

inline bool Memory::isInside(const void* const start, const size_t size) const
{
	ocean_assert(start != nullptr);

	if (size == 0)
	{
		return true;
	}

	if (constAlignedData_ == nullptr)
	{
		return false;
	}

	const uint8_t* const start_u8 = (const uint8_t*)(start);
	const uint8_t* const constAlignedData_u8 = (const uint8_t*)(constAlignedData_);

	return start_u8 >= constAlignedData_u8 && start_u8 + size <= constAlignedData_u8 + size_;
}

inline bool Memory::isInside(const void* const start, const void* const end) const
{
	ocean_assert(start != nullptr && end != nullptr);
	ocean_assert(start <= end);

	if (start == end)
	{
		return true;
	}

	if (constAlignedData_ == nullptr)
	{
		return false;
	}

	const void* const constAlignedDataEnd = (const void*)((const uint8_t*)(constAlignedData_) + size_);

	return start >= constAlignedData_ && end <= constAlignedDataEnd && start < end;
}

inline void Memory::free()
{
	if (allocatedData_ != nullptr)
	{
		ocean_assert(alignedData_ != nullptr);

		::free(allocatedData_);

		allocatedData_ = nullptr;
	}

	constAlignedData_ = nullptr;
	alignedData_ = nullptr;
	size_ = size_t(0);
}

inline size_t Memory::size() const
{
	return size_;
}

inline bool Memory::isOwner() const
{
	return allocatedData_ != nullptr;
}

inline bool Memory::isReadOnly() const
{
	return alignedData_ == nullptr;
}

inline bool Memory::isNull() const
{
	ocean_assert((constAlignedData_ == nullptr && size_ == size_t(0)) || (constAlignedData_ != nullptr && size_ != size_t(0)));

	return constAlignedData_ == nullptr;
}

inline Memory::operator bool() const
{
	ocean_assert((constAlignedData_ == nullptr && size_ == size_t(0)) || (constAlignedData_ != nullptr && size_ != size_t(0)));

	return constAlignedData_ != nullptr;
}

inline Memory& Memory::operator=(Memory&& memory) noexcept
{
	if (this != &memory)
	{
		free();

		allocatedData_ = memory.allocatedData_;
		constAlignedData_ = memory.constAlignedData_;
		alignedData_ = memory.alignedData_;
		size_ = memory.size_;

		memory.allocatedData_ = nullptr;
		memory.constAlignedData_ = nullptr;
		memory.alignedData_ = nullptr;
		memory.size_ = size_t(0);
	}

	return *this;
}

template <typename T>
inline Memory Memory::create(const size_t elements)
{
	return Memory(sizeof(T) * elements, sizeof(T) /* we fore the data-type specific alignment */);
}

inline void Memory::memcpy(void* target, const void* source, const unsigned int size, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&Memory::memcpySubset, (uint8_t*)target, (const uint8_t*)source, 0u, 0u), 0u, size, 2u, 3u, 1024u);
	}
	else
	{
		::memcpy(target, source, size);
	}
}

inline void Memory::memset(void* data, const int value, const unsigned int size, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&Memory::memsetSubset, (uint8_t*)data, value, 0u, 0u), 0u, size, 2u, 3u, 1024u);
	}
	else
	{
		::memset(data, value, size);
	}
}

inline void Memory::memcpySubset(uint8_t* target, const uint8_t* source, const unsigned int firstByte, const unsigned int numberBytes)
{
	ocean_assert(target != nullptr);
	ocean_assert(source != nullptr);

	::memcpy(target + firstByte, source + firstByte, numberBytes);
}

inline void Memory::memsetSubset(uint8_t* data, const int value, const unsigned int firstByte, const unsigned int numberBytes)
{
	ocean_assert(data != nullptr);

	::memset(data + firstByte, value, numberBytes);
}

}

#endif // META_OCEAN_BASE_MEMORY_H
