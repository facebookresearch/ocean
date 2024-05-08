/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_ACCESSOR_H
#define META_OCEAN_BASE_ACCESSOR_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"

namespace Ocean
{

/**
 * This class implements a base class for all accessors.
 * Accessors provide access to any kind of data elements which are stored in any kind of data structure by any kind of access method.<br>
 * @ingroup base
 */
class Accessor
{
	public:

		/**
		 * Default destructor
		 */
		 virtual ~Accessor() = default;

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @return The number of elements
		 */
		virtual size_t size() const = 0;

		/**
		 * Returns whether this accessor provides no elements.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns all elements of a given accessor (as a block).
		 * @param accessor The accessor from which all elements are extracted
		 * @return The elements as a block
		 */
		template <typename TAccessor>
		static std::vector<typename TAccessor::Type> accessor2elements(const TAccessor& accessor);

		/**
		 * Returns all elements of a given accessor as a map with key and elements.
		 * @param accessor The accessor from which all elements are extracted
		 * @return The elements as a map
		 */
		template <typename TAccessor>
		static std::unordered_map<typename TAccessor::KeyType, typename TAccessor::Type> accessor2map(const TAccessor& accessor);

		/**
		 * Returns a subset of all elements of a given accessor (as a block).
		 * @param accessor The accessor from which all sub elements are extracted.
		 * @param subset The individual indices of the subset's elements
		 * @return Subset elements as a block
		 */
		template <typename TAccessor, typename TIndex>
		static std::vector<typename TAccessor::Type> accessor2subsetElements(const TAccessor& accessor, const std::vector<TIndex>& subset);

	protected:

		/**
		 * Protected default constructor.
		 */
		Accessor() = default;

		/**
		 * Protected copy constructor.
		 * @param accessor Accessor to copy
		 */
		Accessor(const Accessor& accessor) = default;

		/**
		 * Deleted assign operator.
		 * @param accessor Accessor which would be assigned
		 * @return Reference to this object
		 */
		Accessor& operator=(const Accessor& accessor) = delete;
};

/**
 * This class implements a base class for accessors allowing a constant reference access.
 * @tparam T The data type of the elements of the accessor
 * @tparam TKey The data type of the keys of the accessor
 * @ingroup base
 */
template <typename T, typename TKey>
class ConstAccessor : public Accessor
{
	public:

		/**
		 * Definition of the element type of this accessor.
		 */
		typedef T Type;

		/**
		 * Definition of the key (or e.g., index) type of this accessor.
		 */
		typedef TKey KeyType;

	public:

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * Beware: There is not guarantee that the data of an accessor is stored as one memory block, thus be prepared that the resulting pointer is nullptr.
		 * @return The pointer to the memory block of this accessor, if available
		 */
		virtual const T* data() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @param key The key of the element to be checked
		 * @return True, if the element exists
		 */
		virtual bool canAccess(const TKey& key) const = 0;

		/**
		 * Returns the first element of this accessor.
		 * @param element The resulting first element by copying the element (by using the assign operator)
		 * @param key The resulting key of the first element
		 * @return True, if at least one element exists
		 */
		virtual bool firstElement(T& element, TKey& key) const = 0;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @param previousKey The previous key for which the next following element is requested
		 * @param nextElement The resulting next element by copying the element (by using the assign operator)
		 * @param nextKey The resulting key of the next element
		 * @return True, if a next element exists
		 */
		virtual bool nextElement(const TKey& previousKey, T& nextElement, TKey& nextKey) const = 0;

		/**
		 * Returns one element of this accessor object by a given key.
		 * @param key The key of element to be accessed, must be valid
		 * @return The requested element
		 */
		virtual const T& operator[](const TKey& key) const = 0;

	protected:

		/**
		 * Protected default constructor.
		 */
		ConstAccessor() = default;
};

/**
 * This class implements a base class for accessors allowing a non-constant reference access.
 * @tparam T The data type of the elements of the accessor
 * @tparam TKey The data type of the keys of the accessor
 * @ingroup base
 */
template <typename T, typename TKey>
class NonconstAccessor : public ConstAccessor<T, TKey>
{
	// we want to keep the const data function from the base class
	using ConstAccessor<T, TKey>::data;

	public:

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * Beware: There is not guarantee that the data of an accessor is stored as one memory block, thus be prepared that the resulting pointer is nullptr.
		 * @return The pointer to the memory block of this accessor, if available
		 */
		virtual T* data();

		/**
		 * Returns one element of this accessor object by a given key.
		 * @param key The key of element to be accessed, must be valid
		 * @return The requested element
		 */
		virtual T& operator[](const TKey& key) = 0;

	protected:

		/**
		 * Creates a new indexed-based accessor object.
		 */
		NonconstAccessor() = default;
};

/**
 * This class implements a base class for all accessors allowing to access temporary elements.
 * @tparam T The data type of the elements of the accessor
 * @tparam TKey The data type of the keys of the accessor
 * @ingroup base
 */
template <typename T, typename TKey>
class TemporaryAccessor : public Accessor
{
	public:

		/**
		 * Definition of the element type of this accessor.
		 */
		typedef T Type;

	public:

		/**
		 * Returns one element of this accessor object by a given index.
		 * @param key The key of element to be accessed, must be valid
		 * @return The requested element
		 */
		virtual T operator[](const TKey& key) const = 0;

		/**
		 * Returns whether this accessor has a specific element.
		 * @param key The key of the element to be checked
		 * @return True, if the element exists
		 */
		virtual bool canAccess(const TKey& key) const = 0;

	protected:

		/**
		 * Creates a new indexed-based accessor object.
		 */
		TemporaryAccessor() = default;
};

/**
 * This class implements a base class for all indexed-based accessors allowing a constant reference access only.
 * @tparam T The data type of the elements of the accessor
 * @ingroup base
 */
template <typename T>
class ConstIndexedAccessor : public ConstAccessor<T, size_t>
{
	public:

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		virtual bool canAccess(const size_t& index) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor:firstElement().
		 */
		virtual bool firstElement(T& element, size_t& index) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		virtual bool nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const;

		/**
		 * Returns one element of this accessor object by a given index.
		 * @param index The index of element to be accessed, with range [0, size())
		 * @return The requested element
		 */
		virtual const T& operator[](const size_t& index) const = 0;

	protected:

		/**
		 * Creates a new indexed-based accessor object.
		 */
		ConstIndexedAccessor() = default;
};

/**
 * This class implements a base class for all indexed-based accessors allowing a non-constant reference access.
 * @tparam T The data type of the elements of the accessor
 * @ingroup base
 */
template <typename T>
class NonconstIndexedAccessor : public NonconstAccessor<T, size_t>
{
	public:

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		virtual bool canAccess(const size_t& index) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor:firstElement().
		 */
		virtual bool firstElement(T& element, size_t& index) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		virtual bool nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const;

		/**
		 * Returns the pointer to this object if this accessor holds at least one element (if this accessor is not empty).
		 * This function can be used to simplify code fragments in which an optional pointer to a non-const accessor is used depending on the factor whether the accessor holds elements or not.
		 * @return The accessor's pointer, otherwise nullptr
		 */
		inline NonconstIndexedAccessor<T>* pointer();

		/**
		 * Returns one element of this accessor object by a given key.
		 * @param index The index of element to be accessed, with range [0, size())
		 * @return The requested element
		 */
		virtual const T& operator[](const size_t& index) const = 0;

		/**
		 * Returns one element of this accessor object by a given index.
		 * @param index The index of element to be accessed, with range [0, size())
		 * @return The requested element
		 */
		virtual T& operator[](const size_t& index) = 0;

	protected:

		/**
		 * Creates a new accessor object.
		 */
		NonconstIndexedAccessor() = default;
};

/**
 * This class implements a base class for all indexed-based accessors allowing to access temporary elements.
 * @tparam T The data type of the elements of the accessor
 * @ingroup base
 */
template <typename T>
class TemporaryIndexedAccessor : public TemporaryAccessor<T, size_t>
{
	public:

		/**
		 * Returns one element of this accessor object by a given index.
		 * @param index The index of element to be accessed, with range [0, size())
		 * @return The requested element
		 */
		virtual T operator[](const size_t& index) const = 0;

		/**
		 * Returns whether this accessor has a specific element.
		 * @param index The index of the element to be checked
		 * @return True, if the element exists
		 */
		virtual bool canAccess(const size_t& index) const;

	protected:

		/**
		 * Creates a new indexed-based accessor object.
		 */
		TemporaryIndexedAccessor() = default;
};

/**
 * This class implements an accessor providing direct access to a constant array of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 *
 * The application of the this class is demonstrated in the following code example (compare the code example for ConstTemplateArrayAccessor):
 * @code
 * // the parameter type is a base class of ConstArrayAccessor<Object> allowing a more flexible application of this function
 * void iterate(const ConstIndexedAccessor<Object>& accessor)
 * {
 *     for (size_t n = 0; n < accessor.size(); ++n)
 *     {
 *         // access the object by application of the !virtual! index operator
 *         const Object& object = accessor[n];
 *
 *         // ... do something with the object ...
 *     }
 * }
 *
 * void main()
 * {
 *     std::vector<Object> objects;
 *
 *     // ... add some objects ...
 *
 *     // iterate over all objects
 *     iterate(ConstArrayAccessor<Object>(objects));
 * }
 * @endcode
 * @tparam T The data type of the elements of the accessor
 * @see ConstTemplateArrayAccessor, ConstElementAccessor.
 * @ingroup base
 */
template <typename T>
class ConstArrayAccessor : public ConstIndexedAccessor<T>
{
	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstArrayAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstArrayAccessor(ConstArrayAccessor<T>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param size The number of elements that can be accessed, may be 0 if elements is nullptr
		 */
		inline ConstArrayAccessor(const T* elements, const size_t size);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements A vector holding all elements
		 */
		explicit inline ConstArrayAccessor(const std::vector<T>& elements);

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see ConstAccessor::size().
		 */
		virtual const T* data() const;

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline ConstArrayAccessor<T>& operator=(ConstArrayAccessor<T>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		const T* elements_ = nullptr;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor providing direct access to std::shared_ptr<T> elements returned as const T* pointers.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * This class is mainly a helper class to avoid extracting pointer from a shared_ptr objects and using a normal `ConstArrayAccessor`.
 *
 * The application of the this class is demonstrated in the following code example:
 * @code
 * void iterate(const ConstIndexedAccessor<const Object*>& accessor)
 * {
 *     for (size_t n = 0; n < accessor.size(); ++n)
 *     {
 *         // access the object by application of the !virtual! index operator
 *         const Object* object = accessor[n];
 *
 *         // ... do something with the object ...
 *     }
 * }
 *
 * void main()
 * {
 *     std::vector<std::shared_ptr<Object>> objects;
 *
 *     // ... add some objects ...
 *
 *     // iterate over all objects
 *     iterate(SharedPointerConstArrayAccessor<Object>(objects));
 * }
 * @endcode
 * @tparam T The data type of the shared_ptr's element_type
 * @see ConstArrayAccessor.
 * @ingroup base
 */
template <typename T>
class SharedPointerConstArrayAccessor : public ConstIndexedAccessor<const T*>
{
	public:

		/**
		 * Definition of the shared pointer object.
		 */
		using SharedPointer = std::shared_ptr<T>;

		/**
		 * Definition of a vector holding the shared pointer objects.
		 */
		using SharedPointers = std::vector<SharedPointer>;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		SharedPointerConstArrayAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		SharedPointerConstArrayAccessor(SharedPointerConstArrayAccessor<T>&& accessor) = default;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param size The number of elements that can be accessed, may be 0 if elements is nullptr
		 */
		inline SharedPointerConstArrayAccessor(const SharedPointer* elements, const size_t size);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements A vector holding all elements
		 */
		explicit inline SharedPointerConstArrayAccessor(const SharedPointers& elements);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T* const & operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		SharedPointerConstArrayAccessor<T>& operator=(SharedPointerConstArrayAccessor<T>&& accessor) = default;

	protected:

		/// The pointers to the actual elements wrapped in the shared pointers.
		std::vector<const T*> elements_;
};

/**
 * This class implements an accessor providing direct access to a constant array of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * This class is not derived from Accessor (or any other Accessor class) to avoid virtual functions ensuring higher element-access performances.<br>
 * Thus, this class will mainly be applied in template functions where the type of the accessor is defined at compile time.
 *
 * The application of the this class is demonstrated in the following code example (compare the code example for ConstArrayAccessor):
 * @code
 * // the parameter type is a template type allowing for fast but not flexible application of this function
 * template <typename TAccessor>
 * void iterate(const TAccessor& accessor)
 * {
 *     for (size_t n = 0; n < accessor.size(); ++n)
 *     {
 *         // access the object by application of the !inlined! index operator
 *         const Object& object = accessor[n];
 *
 *         // ... do something with the object ...
 *     }
 * }
 *
 * void main()
 * {
 *     std::vector<Object> objects;
 *
 *     // ... add some objects ...
 *
 *     // iterate over all objects
 *     iterate(ConstTemplateArrayAccessor<Object>(objects));
 * }
 * @endcode
 * @tparam T The data type of the elements of the accessor
 * @see ConstArrayAccessor, NonconstTemplateArrayAccessor.
 * @ingroup base
 */
template <typename T>
class ConstTemplateArrayAccessor
{
	public:

		/**
		 * Definition of the element type of this accessor.
		 */
		typedef T Type;

		/**
		 * Definition of the key (or e.g., index) type of this accessor.
		 */
		typedef size_t KeyType;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstTemplateArrayAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstTemplateArrayAccessor(ConstTemplateArrayAccessor<T>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param size The number of elements that can be accessed, may be 0 if elements is nullptr
		 */
		inline ConstTemplateArrayAccessor(const T* elements, const size_t size);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements A vector holding all elements
		 */
		explicit inline ConstTemplateArrayAccessor(const std::vector<T>& elements);

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see ConstAccessor::data().
		 */
		inline const T* data() const;

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		inline size_t size() const;

		/**
		 * Returns whether this accessor provides no elements.
		 * @see Accessor::isEmpty().
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		inline bool canAccess(const size_t& index) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor:firstElement().
		 */
		inline bool firstElement(T& element, size_t& index) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		inline bool nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		inline const T& operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline ConstTemplateArrayAccessor<T>& operator=(ConstArrayAccessor<T>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		const T* elements_ = nullptr;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor providing direct access to an array of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * @tparam T The data type of the elements of the accessor
 * @ingroup base
 */
template <typename T>
class NonconstArrayAccessor : public NonconstIndexedAccessor<T>
{
	public:

		/**
		 * Creates a new empty accessor.
		 */
		NonconstArrayAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline NonconstArrayAccessor(NonconstArrayAccessor<T>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param size The number of elements that can be accessed, may be 0 if elements is nullptr
		 */
		inline NonconstArrayAccessor(T* elements, const size_t size);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements A vector holding all elements
		 */
		explicit inline NonconstArrayAccessor(std::vector<T>& elements);

		/**
		 * Creates a new accessor object.
		 * This constructor simplifies the creation of an optional NonconstArrayAccessor object by allowing to provide an empty vector object while defining an explicit resize value.<br>
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements A vector holding all elements
		 * @param resizeSize An explicit resize value that will invoke a resizing of the given vector before the vector is connected with the accessor, with range [1, infinity), 0 to avoid any resizing
		 */
		inline NonconstArrayAccessor(std::vector<T>& elements, const size_t resizeSize);

		/**
		 * Creates a new accessor object.
		 * This constructor simplifies the creation of an optional NonconstArrayAccessor object.<br>
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements An optional vector holding all elements, nullptr to create an empty invalid accessor
		 * @param resizeSize An optional possibility to resize the provided elements vector (if defined - otherwise nothing happens), with range [1, infinity), 0 to avoid any resizing
		 */
		explicit inline NonconstArrayAccessor(std::vector<T>* elements, const size_t resizeSize = 0);

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see NonconstAccessor::data().
		 */
		virtual T* data();

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const size_t& index) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual T& operator[](const size_t& index);

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline NonconstArrayAccessor<T>& operator=(NonconstArrayAccessor<T>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		T* elements_ = nullptr;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor providing direct access to an array of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * This class is not derived from Accessor (or any other Accessor class) to avoid virtual functions ensuring higher element-access performances.<br>
 * Thus, this class will mainly be applied in template functions where the type of the accessor is defined at compile time.
 * @tparam T The data type of the elements of the accessor
 * @see NonconstArrayAccessor, ConstTemplateArrayAccessor.
 * @ingroup base
 */
template <typename T>
class NonconstTemplateArrayAccessor
{
	public:

		/**
		 * Definition of the element type of this accessor.
		 */
		typedef T Type;

		/**
		 * Definition of the key (or e.g., index) type of this accessor.
		 */
		typedef size_t KeyType;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		NonconstTemplateArrayAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline NonconstTemplateArrayAccessor(NonconstTemplateArrayAccessor<T>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param size The number of elements that can be accessed, may be 0 if elements is nullptr
		 */
		inline NonconstTemplateArrayAccessor(T* elements, const size_t size);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements A vector holding all elements
		 */
		explicit inline NonconstTemplateArrayAccessor(std::vector<T>& elements);

		/**
		 * Creates a new accessor object.
		 * This constructor simplifies the creation of an optional NonconstTemplateArrayAccessor object by allowing to provide an empty vector object while defining an explicit resize value.<br>
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements A vector holding all elements
		 * @param resizeSize An explicit resize value that will invoke a resizing of the given vector before the vector is connected with the accessor, with range [1, infinity), 0 to avoid any resizing
		 */
		inline NonconstTemplateArrayAccessor(std::vector<T>& elements, const size_t resizeSize);

		/**
		 * Creates a new accessor object.
		 * This constructor simplifies the creation of an optional NonconstTemplateArrayAccessor object.<br>
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param elements An optional vector holding all elements, nullptr to create an empty invalid accessor
		 * @param resizeSize An optional possibility to resize the provided elements vector (if defined - otherwise nothing happens), with range [1, infinity), 0 to avoid any resizing
		 */
		explicit inline NonconstTemplateArrayAccessor(std::vector<T>* elements, const size_t resizeSize = 0);

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see ConstAccessor::data().
		 */
		inline const T* data() const;

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see NonconstAccessor::data().
		 */
		inline T* data();

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see NonconstAccessor::size().
		 */
		inline size_t size() const;

		/**
		 * Returns whether this accessor provides no elements.
		 * @see NonconstAccessor::isEmpty().
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @see NonconstAccessor::canAccess().
		 */
		inline bool canAccess(const size_t& index) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor:firstElement().
		 */
		inline bool firstElement(T& element, size_t& index) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		inline bool nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		inline const T& operator[](const size_t& index) const;

		/**
		 * Returns one element of this accessor object.
		 * @see NonconstAccessor::operator[].
		 */
		inline T& operator[](const size_t& index);

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline NonconstTemplateArrayAccessor<T>& operator=(NonconstTemplateArrayAccessor<T>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		T* elements_ = nullptr;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor providing direct access to a constant array of elements while all elements are identical (all elements point to one unique object).
 * This accessor can be used to simulate a large array with several identical entries.
 * @see ConstArrayAccessor.
 * @ingroup base
 */
template <typename T>
class ConstElementAccessor : public ConstIndexedAccessor<T>
{
	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstElementAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstElementAccessor(ConstElementAccessor<T>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.
		 * @param size The number of elements that can be accessed, with range [1, infinity)
		 * @param element The element that can be accessed though any index
		 */
		inline ConstElementAccessor(const size_t size, const T& element);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline ConstElementAccessor<T>& operator=(ConstElementAccessor<T>&& accessor) noexcept;

	protected:

		/// The element of this accessor.
		const T* element_ = nullptr;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor providing direct access to a constant (unordered) map of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * @tparam T The data type of the elements of the accessor
 * @tparam TKey The data type of the keys of the accessor
 * @ingroup base
 */
template <typename T, typename TKey>
class ConstMapAccessor : public ConstAccessor<T, TKey>
{
	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstMapAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstMapAccessor(ConstMapAccessor<T, TKey>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements A map holding all elements
		 */
		explicit inline ConstMapAccessor(const std::unordered_map<TKey, T>& elements);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		virtual bool canAccess(const TKey& key) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor::firstElement().
		 */
		virtual bool firstElement(T& element, TKey& key) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		virtual bool nextElement(const TKey& previousKey, T& nextElement, TKey& nextKey) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const TKey& key) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this accessor
		 */
		inline ConstMapAccessor<T, TKey>& operator=(ConstMapAccessor<T, TKey>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		const std::unordered_map<TKey, T>* elementMap_ = nullptr;
};

/**
 * This class implements an accessor providing direct access to an (unordered) map of elements.
 * An instance of this accessor does not copy the elements, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * @tparam T The data type of the elements of the accessor
 * @tparam TKey The data type of the keys of the accessor
 * @ingroup base
 */
template <typename T, typename TKey>
class NonconstMapAccessor : public NonconstAccessor<T, TKey>
{
	public:

		/**
		 * Creates a new empty accessor.
		 */
		NonconstMapAccessor() = default;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements A map holding all elements
		 */
		explicit inline NonconstMapAccessor(std::unordered_map<TKey, T>& elements);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		virtual bool canAccess(const TKey& key) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor::firstElement().
		 */
		virtual bool firstElement(T& element, TKey& key) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		virtual bool nextElement(const TKey& previousKey, T& nextElement, TKey& nextKey) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const TKey& key) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual T& operator[](const TKey& key);

	protected:

		/// The elements of this accessor.
		std::unordered_map<TKey, T>* elementMap_ = nullptr;
};

/**
 * This class implements an indexed-based constant accessor providing access to a subset of elements stored in an array.
 * The subset is defined by a set of indices defining also the order of the accessible elements.<br>
 * An instance of this accessor does not copy the elements or the subset indices, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * @tparam T The data type of the elements of the accessor
 * @tparam TIndex The data type of the indices defining the subset of elements
 * @ingroup base
 */
template <typename T, typename TIndex>
class ConstArraySubsetAccessor : public ConstIndexedAccessor<T>
{
	public:

		/**
		 * Definition of the data type of the indices.
		 */
		typedef TIndex IndexType;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstArraySubsetAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstArraySubsetAccessor(ConstArraySubsetAccessor<T, TIndex>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 * @param subsetSize The number of indices (not the number of elements) that define the subset, may be 0 if indices is nullptr
		 */
		inline ConstArraySubsetAccessor(const T* elements, const TIndex* subsetIndices, const size_t subsetSize);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 */
		inline ConstArraySubsetAccessor(const T* elements, const std::vector<TIndex>& subsetIndices);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 */
		inline ConstArraySubsetAccessor(const std::vector<T>& elements, const std::vector<TIndex>& subsetIndices);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this object
		 */
		inline ConstArraySubsetAccessor<T, TIndex>& operator=(ConstArraySubsetAccessor<T, TIndex>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		const T* elements_ = nullptr;

		/// The subset indices of this accessor.
		const TIndex* subsetIndices_ = nullptr;

		/// The number of elements that can be accessed.
		size_t subsetSize_ = 0;
};

/**
 * This class implements an indexed-based constant accessor providing access to a subset of elements stored in an array.
 * The subset is defined by a set of indices defining also the order of the accessible elements.<br>
 * An instance of this accessor does not copy the elements or the subset indices, thus the caller has to ensure that the actual elements exist as long as the instance of the accessor exists.<br>
 * This class is not derived from Accessor (or any other Accessor class) to avoid virtual functions ensuring higher element-access performances.<br>
 * Thus, this class will mainly be applied in template functions where the type of the accessor is defined at compile time.
 * @tparam T The data type of the elements of the accessor
 * @tparam TIndex The data type of the indices defining the subset of elements
 * @see ConstTemplateArrayAccessor.
 * @ingroup base
 */
template <typename T, typename TIndex>
class ConstTemplateArraySubsetAccessor
{
	public:

		/**
		 * Definition of the element type of this accessor.
		 */
		typedef T Type;

		/**
		 * Definition of the key (or e.g., index) type of this accessor.
		 */
		typedef size_t KeyType;

		/**
		 * Definition of the data type of the indices.
		 */
		typedef TIndex IndexType;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstTemplateArraySubsetAccessor() = default;

		/**
		 * Move constructor.
		 * @param accessor Accessor to be moved
		 */
		inline ConstTemplateArraySubsetAccessor(ConstTemplateArraySubsetAccessor<T, TIndex>&& accessor) noexcept;

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 * @param subsetSize The number of indices (not the number of elements) that define the subset, may be 0 if indices is nullptr
		 */
		inline ConstTemplateArraySubsetAccessor(const T* elements, const TIndex* subsetIndices, const size_t subsetSize);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 */
		inline ConstTemplateArraySubsetAccessor(const T* elements, const std::vector<TIndex>& subsetIndices);

		/**
		 * Creates a new accessor object.
		 * Beware: The given elements are not copied, they must not be deleted before the accessor is disposed.<br>
		 * @param elements The elements that can be accessed, may be nullptr if size is equal 0
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of given elements
		 */
		inline ConstTemplateArraySubsetAccessor(const std::vector<T>& elements, const std::vector<TIndex>& subsetIndices);

		/**
		 * Returns a pointer to the elements of this accessor if the data exists within one memory block without gaps.
		 * @see ConstAccessor::data().
		 */
		inline const T* data() const;

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		inline size_t size() const;

		/**
		 * Returns whether this accessor provides no elements.
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns whether this accessor has a specific element.
		 * @see ConstAccessor::canAccess().
		 */
		inline bool canAccess(const size_t& index) const;

		/**
		 * Returns the first element of this accessor.
		 * @see ConstAccessor:firstElement().
		 */
		inline bool firstElement(T& element, size_t& index) const;

		/**
		 * Returns the next element which follows a given key of the previous element.
		 * @see ConstAccessor::nextElement().
		 */
		inline bool nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const;

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		inline const T& operator[](const size_t& index) const;

		/**
		 * Move operator.
		 * @param accessor Accessor to be moved
		 * @return Reference to this object
		 */
		inline ConstTemplateArraySubsetAccessor<T, TIndex>& operator=(ConstTemplateArraySubsetAccessor<T, TIndex>&& accessor) noexcept;

	protected:

		/// The elements of this accessor.
		const T* elements_ = nullptr;

		/// The subset indices of this accessor.
		const TIndex* subsetIndices_ = nullptr;

		/// The number of elements that can be accessed.
		size_t subsetSize_ = 0;
};

/**
 * This class implements an indexed-based constant accessor providing access to a subset of elements stored in a specified indexed-based child accessor.
 * @tparam T The data type of the elements of the accessor
 * @tparam TIndex The data type of the indices defining the subset of elements
 * @ingroup base
 */
template <typename T, typename TIndex>
class ConstIndexedAccessorSubsetAccessor : public ConstIndexedAccessor<T>
{
	public:

		/**
		 * Creates a new accessor which uses an accessor as base.
		 * @param child The child accessor
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of elements in the child accessor
		 * @param subsetSize The number of indices (not the number of elements) that define the subset, may be 0 if indices is nullptr
		 */
		inline ConstIndexedAccessorSubsetAccessor(const ConstIndexedAccessor<T>& child, const TIndex* subsetIndices, const size_t subsetSize);

		/**
		 * Creates a new accessor which uses an accessor as base.
		 * @param child The child accessor
		 * @param subsetIndices The indices that define a subset of the elements to be accessed, the maximal index must be smaller than the number of elements in the child accessor
		 */
		inline ConstIndexedAccessorSubsetAccessor(const ConstIndexedAccessor<T>& child, const std::vector<TIndex>& subsetIndices);

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see Accessor::size().
		 */
		virtual size_t size() const;

		/**
		 * Returns one element of this accessor object.
		 * @param index The index of element to be accessed, with range [0, size())
		 * @return The requested element
		 */
		virtual const T& operator[](const size_t& index) const;

	protected:

		// The child accessor of this accessor.
		const ConstIndexedAccessor<T>& child_;

		/// The subset indices of this accessor.
		const TIndex* subsetIndices_ = nullptr;

		/// The number of elements that can be accessed.
		size_t subsetSize_ = 0;
};

/**
 * This class implements an accessor providing access to a elements by using a callback function.
 * @tparam T The data type of the elements of the accessor
 * @ingroup base
 */
template <typename T>
class ConstCallbackIndexedAccessor : public ConstIndexedAccessor<T>
{
	public:

		/**
		 * Definition of a callback function providing access to individual elements.
		 * The first parameter is the index of the elements to be accessed
		 * The return parameter is the constant reference to the requested element
		 */
		typedef Callback<const T&, const size_t> CallbackFunction;

	public:

		/**
		 * Creates a new empty accessor.
		 */
		ConstCallbackIndexedAccessor() = default;

		/**
		 * Creates a new accessor object.
		 * @param callback The callback function providing access to individual elements
		 * @param size The number of elements of this accessor
		 */
		inline ConstCallbackIndexedAccessor(const CallbackFunction& callback, const size_t size);

		/**
		 * Returns one element of this accessor object.
		 * @see ConstAccessor::operator[].
		 */
		virtual const T& operator[](const size_t& index) const;

		/**
		 * Returns the number of accessible elements of this accessor object.
		 * @see ConstAccessor::size().
		 */
		virtual size_t size() const;

	protected:

		/// The callback function of this accessor.
		CallbackFunction callback_;

		/// The number of elements that can be accessed.
		size_t size_ = 0;
};

/**
 * This class implements an accessor that guarantees memory access to the elements of an indexed accessor object until the scope ends.
 * The memory access ensures that the entire data of the indexed accessor objects is provided as one memory block without any gaps.<br>
 * Beware: The data of the given indexed accessor may be copied (if necessary) to ensure the functionality.<br>
 * Thus, the accessible elements of this object may be clones/copies of the actual provided accessor object.
 */
template <typename T>
class ScopedConstMemoryAccessor
{
	public:

		/**
		 * Creates a new scoped accessor object by a given indexed accessor object.
		 * @param accessor The accessor object providing the data for this scoped object, ensure that the provided accessor exists at least until the scope ends
		 * @tparam TAccessor The data type of the accessor providing the data, must be of type ConstIndexedAccessor<T>, or ConstTemplateArrayAccessor<T, size_t> or ConstTemplateArraySubsetAccessor<T, size_t>
		 */
		template <typename TAccessor>
		inline explicit ScopedConstMemoryAccessor(const TAccessor& accessor);

		/**
		 * Returns the pointer to the memory block providing the data of the accessor.
		 * @return The memory block's pointer, nullptr if the accessor does not provide any data
		 */
		inline const T* data() const;

		/**
		 * Returns the number of elements the accessor provides.
		 * @return The accessor's number of elements, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Returns one element of this accessor.
		 * @param index The index of the element, with range [0, size())
		 * @return The requested element
		 */
		inline const T& operator[](const size_t index) const;

		/**
		 * Returns whether this object provides access to at least one element of the accessor.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The pointer to the memory block of the accessor.
		const T* data_ = nullptr;

		/// The number of elements the accessor provides.
		size_t size_ = 0;

		/// The individual elements of the accessor, if necessary.
		std::vector<T> intermediateValues_;
};

/**
 * This class implements an accessor that guarantees memory access to the elements of an indexed accessor object until the scope ends.
 * The memory access ensures that the entire data of the indexed accessor objects is provided as one memory block without any gaps.<br>
 * Beware: The data of the given indexed accessor may be copied (if necessary) to ensure the functionality.<br>
 * Thus, the accessible elements of this object may be clones/copies of the actual provided accessor object.
 * The connected indexed accessor object will hold the data of this accessor object after this object is disposed.
 */
template <typename T>
class ScopedNonconstMemoryAccessor
{
	public:

		/**
		 * Creates a new scoped accessor object by a given indexed accessor object.
		 * @param accessor The accessor object providing the data for this scoped object, the provided accessor needs to exist at least until the scope ends
		 */
		explicit inline ScopedNonconstMemoryAccessor(NonconstIndexedAccessor<T>& accessor);

		/**
		 * Creates a new scoped accessor object by an optional indexed accessor object or creates an internal temporary memory with specified size.
		 * @param accessor The accessor object providing the data for this scoped object, the provided accessor needs to exist at least until the scope ends, nullptr to create an internal temporary memory without connected accessor
		 * @param temporarySize The explicit size of the internal memory if no accessor is provided, does not have any meaning if an accessor is provided
		 */
		explicit inline ScopedNonconstMemoryAccessor(NonconstIndexedAccessor<T>* accessor, const size_t temporarySize = 0);

		/**
		 * Destructs the scoped accessor object.
		 * Further, the intermediate values of this object may be copied back to the accessor object, if necessary.
		 */
		inline ~ScopedNonconstMemoryAccessor();

		/**
		 * Returns the pointer to the memory block providing the data of the accessor.
		 * @return The memory block's pointer, nullptr if the accessor does not provide any data
		 */
		inline T* data();

		/**
		 * Returns the number of elements the accessor provides.
		 * @return The accessor's number of elements, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Returns one element of this accessor.
		 * @param index The index of the element, with range [0, size())
		 * @return The requested element
		 */
		inline T& operator[](const size_t index);

		/**
		 * Returns whether this object provides access to at least one element of the accessor.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/// The given accessor object providing the data for this object, nullptr if no accessor object was provided during creation.
		NonconstIndexedAccessor<T>* accessor_ = nullptr;

		/// The pointer to the memory block of the accessor.
		T* data_ = nullptr;

		/// The number of elements the accessor provides.
		size_t size_ = 0;

		/// The individual elements of the accessor, if necessary.
		std::vector<T> intermediateValues_;
};

inline bool Accessor::isEmpty() const
{
	return size() == 0;
}

template <typename TAccessor>
std::vector<typename TAccessor::Type> Accessor::accessor2elements(const TAccessor& accessor)
{
	std::vector<typename TAccessor::Type> result;
	result.reserve(accessor.size());

	for (size_t n = 0; n < accessor.size(); ++n)
	{
		result.emplace_back(accessor[n]);
	}

	return result;
}

template <typename TAccessor>
std::unordered_map<typename TAccessor::KeyType, typename TAccessor::Type> Accessor::accessor2map(const TAccessor& accessor)
{
	std::unordered_map<typename TAccessor::KeyType, typename TAccessor::Type> result;

	typename TAccessor::Type element;
	typename TAccessor::KeyType key;

	if (accessor.firstElement(element, key))
	{
		do
		{
			result[key] = element;
		}
		while (accessor.nexteElement(key, element, key));
	}

	return result;
}

template <typename TAccessor, typename TIndex>
std::vector<typename TAccessor::Type> Accessor::accessor2subsetElements(const TAccessor& accessor, const std::vector<TIndex>& subset)
{
	std::vector<typename TAccessor::Type> result;
	result.reserve(subset.size());

	for (size_t n = 0; n < subset.size(); ++n)
	{
		result.emplace_back(accessor[subset[n]]);
	}

	return result;
}

template <typename T, typename TKey>
const T* ConstAccessor<T, TKey>::data() const
{
	return nullptr;
}

template <typename T, typename TKey>
T* NonconstAccessor<T, TKey>::data()
{
	return nullptr;
}

template <typename T>
bool ConstIndexedAccessor<T>::canAccess(const size_t& index) const
{
	return index < this->size();
}

template <typename T>
bool ConstIndexedAccessor<T>::firstElement(T& element, size_t& index) const
{
	if (this->size() == 0)
	{
		return false;
	}

	element = (*this)[0];
	index = 0;

	return true;
}

template <typename T>
bool ConstIndexedAccessor<T>::nextElement(const size_t& previousIndex, T& element, size_t& nextIndex) const
{
	if (previousIndex + 1 < this->size())
	{
		nextIndex = previousIndex + 1;
		element = (*this)[nextIndex];

		return true;
	}

	return false;
}

template <typename T>
bool NonconstIndexedAccessor<T>::canAccess(const size_t& index) const
{
	return index < this->size();
}

template <typename T>
bool NonconstIndexedAccessor<T>::firstElement(T& element, size_t& index) const
{
	if (this->size() == 0)
	{
		return false;
	}

	element = (*this)[0];
	index = 0;
	return true;
}

template <typename T>
bool NonconstIndexedAccessor<T>::nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const
{
	if (previousIndex + 1 < this->size())
	{
		nextIndex = previousIndex + 1;
		nextElement = (*this)[nextIndex];
		return true;
	}

	return false;
}

template <typename T>
inline NonconstIndexedAccessor<T>* NonconstIndexedAccessor<T>::pointer()
{
	if (this->isEmpty())
	{
		return nullptr;
	}
	else
	{
		return this;
	}
}

template <typename T>
bool TemporaryIndexedAccessor<T>::canAccess(const size_t& index) const
{
	return index < this->size();
}

template <typename T, typename TIndex>
inline ConstIndexedAccessorSubsetAccessor<T, TIndex>::ConstIndexedAccessorSubsetAccessor(const ConstIndexedAccessor<T>& child, const TIndex* subsetIndices, const size_t subsetSize) :
	child_(child),
	subsetIndices_(subsetIndices),
	subsetSize_(subsetSize)
{
	// nothing to do here
}

template <typename T, typename TIndex>
inline ConstIndexedAccessorSubsetAccessor<T, TIndex>::ConstIndexedAccessorSubsetAccessor(const ConstIndexedAccessor<T>& child, const std::vector<TIndex>& subsetIndices) :
	child_(child),
	subsetIndices_(subsetIndices.data()),
	subsetSize_(subsetIndices.size())
{
	// nothing to do here
}

template <typename T, typename TIndex>
size_t ConstIndexedAccessorSubsetAccessor<T, TIndex>::size() const
{
	return subsetSize_;
}

template <typename T, typename TIndex>
const T& ConstIndexedAccessorSubsetAccessor<T, TIndex>::operator[](const size_t& index) const
{
	ocean_assert(index < subsetSize_);

	return child_[subsetIndices_[index]];
}

template <typename T>
inline ConstArrayAccessor<T>::ConstArrayAccessor(ConstArrayAccessor<T>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T>
inline ConstArrayAccessor<T>::ConstArrayAccessor(const T* elements, const size_t size) :
	elements_(elements),
	size_(size)
{
	// nothing to do here
}

template <typename T>
inline ConstArrayAccessor<T>::ConstArrayAccessor(const std::vector<T>& elements) :
	elements_(elements.data()),
	size_(elements.size())
{
	// nothing to do here
}

template <typename T>
const T* ConstArrayAccessor<T>::data() const
{
	return elements_;
}

template <typename T>
size_t ConstArrayAccessor<T>::size() const
{
	return size_;
}

template <typename T>
const T& ConstArrayAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < size_);
	ocean_assert(elements_ != nullptr);

	return elements_[index];
}

template <typename T>
inline ConstArrayAccessor<T>& ConstArrayAccessor<T>::operator=(ConstArrayAccessor<T>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		size_ = accessor.size_;

		accessor.elements_ = nullptr;
		accessor.size_ = 0;
	}

	return *this;
}

template <typename T>
inline SharedPointerConstArrayAccessor<T>::SharedPointerConstArrayAccessor(const SharedPointer* elements, const size_t size) :
	elements_(size)
{
	for (size_t n = 0; n < size; ++n)
	{
		elements_[n] = elements[n]->get();
	}
}

template <typename T>
inline SharedPointerConstArrayAccessor<T>::SharedPointerConstArrayAccessor(const SharedPointers& elements) :
	elements_(elements.size())
{
	for (size_t n = 0; n < elements.size(); ++n)
	{
		elements_[n] = elements[n].get();
	}
}

template <typename T>
size_t SharedPointerConstArrayAccessor<T>::size() const
{
	return elements_.size();
}

template <typename T>
const T* const & SharedPointerConstArrayAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < elements_.size());

	return elements_[index];
}

template <typename T>
inline ConstTemplateArrayAccessor<T>::ConstTemplateArrayAccessor(ConstTemplateArrayAccessor<T>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T>
inline ConstTemplateArrayAccessor<T>::ConstTemplateArrayAccessor(const T* elements, const size_t size) :
	elements_(elements),
	size_(size)
{
	// nothing to do here
}

template <typename T>
inline ConstTemplateArrayAccessor<T>::ConstTemplateArrayAccessor(const std::vector<T>& elements) :
	elements_(elements.data()),
	size_(elements.size())
{
	// nothing to do here
}

template <typename T>
const T* ConstTemplateArrayAccessor<T>::data() const
{
	return elements_;
}

template <typename T>
inline size_t ConstTemplateArrayAccessor<T>::size() const
{
	return size_;
}

template <typename T>
inline bool ConstTemplateArrayAccessor<T>::isEmpty() const
{
	return size_ == 0;
}

template <typename T>
inline bool ConstTemplateArrayAccessor<T>::canAccess(const size_t& index) const
{
	return index < size_;
}

template <typename T>
inline bool ConstTemplateArrayAccessor<T>::firstElement(T& element, size_t& index) const
{
	if (size_ == 0)
	{
		return false;
	}

	element = elements_[0];
	index = 0;
	return true;
}

template <typename T>
inline bool ConstTemplateArrayAccessor<T>::nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const
{
	if (previousIndex + 1 >= size_)
	{
		return false;
	}

	nextIndex = previousIndex + 1;
	nextElement = elements_[nextIndex];
	return true;
}

template <typename T>
inline const T& ConstTemplateArrayAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < size_);
	return elements_[index];
}

template <typename T>
inline ConstTemplateArrayAccessor<T>& ConstTemplateArrayAccessor<T>::operator=(ConstArrayAccessor<T>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		size_ = accessor.size_;

		accessor.elements_ = nullptr;
		accessor.size_ = 0;
	}

	return *this;
}

template <typename T>
inline NonconstArrayAccessor<T>::NonconstArrayAccessor(NonconstArrayAccessor<T>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T>
inline NonconstArrayAccessor<T>::NonconstArrayAccessor(T* elements, const size_t size) :
	elements_(elements),
	size_(size)
{
	// nothing to do here
}

template <typename T>
inline NonconstArrayAccessor<T>::NonconstArrayAccessor(std::vector<T>& elements) :
	elements_(elements.data()),
	size_(elements.size())
{
	// nothing to do here
}

template <typename T>
inline NonconstArrayAccessor<T>::NonconstArrayAccessor(std::vector<T>& elements, const size_t resizeSize) :
	elements_(nullptr),
	size_(0)
{
	if (resizeSize != 0)
	{
		elements.resize(resizeSize);
	}

	elements_ = elements.data();
	size_ = elements.size();
}

template <typename T>
inline NonconstArrayAccessor<T>::NonconstArrayAccessor(std::vector<T>* elements, const size_t resizeSize) :
	elements_(nullptr),
	size_(0)
{
	if (elements)
	{
		if (resizeSize != 0)
		{
			elements->resize(resizeSize);
		}

		elements_ = elements->data();
		size_ = elements->size();
	}
}

template <typename T>
const T& NonconstArrayAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < size_);
	ocean_assert(elements_);

	return elements_[index];
}

template <typename T>
T& NonconstArrayAccessor<T>::operator[](const size_t& index)
{
	ocean_assert(index < size_);
	ocean_assert(elements_);

	return elements_[index];
}

template <typename T>
T* NonconstArrayAccessor<T>::data()
{
	return elements_;
}

template <typename T>
size_t NonconstArrayAccessor<T>::size() const
{
	return size_;
}

template <typename T>
inline NonconstArrayAccessor<T>& NonconstArrayAccessor<T>::operator=(NonconstArrayAccessor<T>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		size_ = accessor.size_;

		accessor.elements_ = nullptr;
		accessor.size_ = 0;
	}

	return *this;
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>::NonconstTemplateArrayAccessor(NonconstTemplateArrayAccessor<T>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>::NonconstTemplateArrayAccessor(T* elements, const size_t size) :
	elements_(elements),
	size_(size)
{
	// nothing to do here
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>::NonconstTemplateArrayAccessor(std::vector<T>& elements) :
	elements_(elements.data()),
	size_(elements.size())
{
	// nothing to do here
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>::NonconstTemplateArrayAccessor(std::vector<T>& elements, const size_t resizeSize) :
	elements_(nullptr),
	size_(0)
{
	if (resizeSize != 0)
	{
		elements.resize(resizeSize);
	}

	elements_ = elements.data();
	size_ = elements.size();
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>::NonconstTemplateArrayAccessor(std::vector<T>* elements, const size_t resizeSize) :
	elements_(nullptr),
	size_(0)
{
	if (elements)
	{
		if (resizeSize != 0)
		{
			elements->resize(resizeSize);
		}

		elements_ = elements->data();
		size_ = elements->size();
	}
}

template <typename T>
const T* NonconstTemplateArrayAccessor<T>::data() const
{
	return elements_;
}

template <typename T>
T* NonconstTemplateArrayAccessor<T>::data()
{
	return elements_;
}

template <typename T>
inline size_t NonconstTemplateArrayAccessor<T>::size() const
{
	return size_;
}

template <typename T>
inline bool NonconstTemplateArrayAccessor<T>::isEmpty() const
{
	return size_ == 0;
}

template <typename T>
inline bool NonconstTemplateArrayAccessor<T>::canAccess(const size_t& index) const
{
	return index < size_;
}

template <typename T>
inline bool NonconstTemplateArrayAccessor<T>::firstElement(T& element, size_t& index) const
{
	if (size_ == 0)
	{
		return false;
	}

	element = elements_[0];
	index = 0;
	return true;
}

template <typename T>
inline bool NonconstTemplateArrayAccessor<T>::nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const
{
	if (previousIndex + 1 >= size_)
	{
		return false;
	}

	nextIndex = previousIndex + 1;
	nextElement = elements_[nextIndex];
	return true;
}

template <typename T>
inline const T& NonconstTemplateArrayAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < size_);
	return elements_[index];
}

template <typename T>
inline T& NonconstTemplateArrayAccessor<T>::operator[](const size_t& index)
{
	ocean_assert(index < size_);
	return elements_[index];
}

template <typename T>
inline NonconstTemplateArrayAccessor<T>& NonconstTemplateArrayAccessor<T>::operator=(NonconstTemplateArrayAccessor<T>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		size_ = accessor.size_;

		accessor.elements_ = nullptr;
		accessor.size_ = 0;
	}

	return *this;
}

template <typename T>
inline ConstElementAccessor<T>::ConstElementAccessor(ConstElementAccessor<T>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T>
inline ConstElementAccessor<T>::ConstElementAccessor(const size_t size, const T& element) :
	element_(&element),
	size_(size)
{
	ocean_assert(size_ != 0);
	ocean_assert(element_ != nullptr);
}

template <typename T>
size_t ConstElementAccessor<T>::size() const
{
	return size_;
}

template <typename T>
const T& ConstElementAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert_and_suppress_unused(index < size_, index);
	ocean_assert(element_ != nullptr);

	return *element_;
}

template <typename T>
ConstElementAccessor<T>& ConstElementAccessor<T>::operator=(ConstElementAccessor<T>&& accessor) noexcept
{
	if (this != &accessor)
	{
		element_ = accessor.element_;
		size_ = accessor.size_;

		accessor.element_ = nullptr;
		accessor.size_ = 0;
	}

	return *this;
}

template <typename T, typename TKey>
inline ConstMapAccessor<T, TKey>::ConstMapAccessor(ConstMapAccessor<T, TKey>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T, typename TKey>
inline ConstMapAccessor<T, TKey>::ConstMapAccessor(const std::unordered_map<TKey, T>& elements) :
	elementMap_(&elements)
{
	// nothing to do here
}

template <typename T, typename TKey>
size_t ConstMapAccessor<T, TKey>::size() const
{
	return elementMap_ ? elementMap_->size() : 0;
}

template <typename T, typename TKey>
bool ConstMapAccessor<T, TKey>::canAccess(const TKey& key) const
{
	if (elementMap_ == nullptr)
	{
		return false;
	}

	return elementMap_->find(key) != elementMap_->end();
}

template <typename T, typename TKey>
bool ConstMapAccessor<T, TKey>::firstElement(T& element, TKey& key) const
{
	if (elementMap_ == nullptr || elementMap_->empty())
	{
		return false;
	}

	key = elementMap_->begin()->first;
	element = elementMap_->begin()->second;

	return true;
}

template <typename T, typename TKey>
bool ConstMapAccessor<T, TKey>::nextElement(const TKey& previousKey, T& element, TKey& nextKey) const
{
	if (elementMap_ == nullptr)
	{
		return false;
	}

	const typename std::unordered_map<TKey, T>::const_iterator i = elementMap_->find(previousKey);
	if (i == elementMap_->cend() || (++i) == elementMap_->cend())
	{
		return false;
	}

	nextKey = i->first;
	element = i->second;

	return true;
}

template <typename T, typename TKey>
const T& ConstMapAccessor<T, TKey>::operator[](const TKey& key) const
{
	ocean_assert(elementMap_);
	ocean_assert(elementMap_->find(key) != elementMap_->end());

	return elementMap_->find(key)->second;
}

template <typename T, typename TKey>
inline ConstMapAccessor<T, TKey>& ConstMapAccessor<T, TKey>::operator=(ConstMapAccessor<T, TKey>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elementMap_ = accessor.elementMap_;
		accessor.elementMap_ = nullptr;
	}

	return *this;
}

template <typename T, typename TKey>
inline NonconstMapAccessor<T, TKey>::NonconstMapAccessor(std::unordered_map<TKey, T>& elements) :
	elementMap_(&elements)
{
	// nothing to do here
}

template <typename T, typename TKey>
bool NonconstMapAccessor<T, TKey>::canAccess(const TKey& key) const
{
	if (elementMap_ == nullptr)
	{
		return false;
	}

	return elementMap_->find(key) != elementMap_->end();
}

template <typename T, typename TKey>
bool NonconstMapAccessor<T, TKey>::firstElement(T& element, TKey& key) const
{
	if (elementMap_ == nullptr || elementMap_->empty())
	{
		return false;
	}

	key = elementMap_->begin()->first;
	element = elementMap_->begin()->second;

	return true;
}

template <typename T, typename TKey>
bool NonconstMapAccessor<T, TKey>::nextElement(const TKey& previousKey, T& element, TKey& nextKey) const
{
	if (elementMap_ == nullptr)
	{
		return false;
	}

	const typename std::unordered_map<TKey, T>::const_iterator i = elementMap_->find(previousKey);
	if (i == elementMap_->cend() || (++i) == elementMap_->cend())
	{
		return false;
	}

	nextKey = i->first;
	element = i->second;

	return true;
}

template <typename T, typename TKey>
size_t NonconstMapAccessor<T, TKey>::size() const
{
	return elementMap_ ? elementMap_->size() : 0;
}

template <typename T, typename TKey>
const T& NonconstMapAccessor<T, TKey>::operator[](const TKey& key) const
{
	ocean_assert(elementMap_);
	ocean_assert(elementMap_->find(key) != elementMap_->end());

	return elementMap_->find(key)->second;
}

template <typename T, typename TKey>
T& NonconstMapAccessor<T, TKey>::operator[](const TKey& key)
{
	ocean_assert(elementMap_);
	ocean_assert(elementMap_->find(key) != elementMap_->end());

	return elementMap_->find(key)->second;
}

template <typename T, typename TIndex>
inline ConstArraySubsetAccessor<T, TIndex>::ConstArraySubsetAccessor(ConstArraySubsetAccessor<T, TIndex>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T, typename TIndex>
inline ConstArraySubsetAccessor<T, TIndex>::ConstArraySubsetAccessor(const T* elements, const TIndex* subsetIndices, const size_t subsetSize) :
	elements_(elements),
	subsetIndices_(subsetIndices),
	subsetSize_(subsetSize)
{
	// nothing to do here
}

template <typename T, typename TIndex>
inline ConstArraySubsetAccessor<T, TIndex>::ConstArraySubsetAccessor(const T* elements, const std::vector<TIndex>& subsetIndices) :
	elements_(elements),
	subsetIndices_(subsetIndices.data()),
	subsetSize_(subsetIndices.size())
{
	// nothing to do here
}

template <typename T, typename TIndex>
inline ConstArraySubsetAccessor<T, TIndex>::ConstArraySubsetAccessor(const std::vector<T>& elements, const std::vector<TIndex>& subsetIndices) :
	elements_(elements.data()),
	subsetIndices_(subsetIndices.data()),
	subsetSize_(subsetIndices.size())
{
#ifdef OCEAN_DEBUG
	ocean_assert(subsetIndices.size() <= elements.size());
	for (size_t n = 0; n < subsetIndices.size(); ++n)
	{
		ocean_assert(subsetIndices[n] < elements.size());
	}
#endif
}

template <typename T, typename TIndex>
size_t ConstArraySubsetAccessor<T, TIndex>::size() const
{
	return subsetSize_;
}

template <typename T, typename TIndex>
const T& ConstArraySubsetAccessor<T, TIndex>::operator[](const size_t& index) const
{
	ocean_assert(index < subsetSize_);
	ocean_assert(elements_ && subsetIndices_);

	return elements_[subsetIndices_[index]];
}

template <typename T, typename TIndex>
inline ConstArraySubsetAccessor<T, TIndex>& ConstArraySubsetAccessor<T, TIndex>::operator=(ConstArraySubsetAccessor<T, TIndex>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		subsetIndices_ = accessor.subsetIndices_;
		subsetSize_ = accessor.subsetSize_;

		accessor.elements_ = nullptr;
		accessor.subsetIndices_ = nullptr;
		accessor.subsetSize_ = 0;
	}

	return *this;
}

template <typename T, typename TIndex>
inline ConstTemplateArraySubsetAccessor<T, TIndex>::ConstTemplateArraySubsetAccessor(ConstTemplateArraySubsetAccessor<T, TIndex>&& accessor) noexcept
{
	*this = std::move(accessor);
}

template <typename T, typename TIndex>
inline ConstTemplateArraySubsetAccessor<T, TIndex>::ConstTemplateArraySubsetAccessor(const T* elements, const TIndex* subsetIndices, const size_t subsetSize) :
	elements_(elements),
	subsetIndices_(subsetIndices),
	subsetSize_(subsetSize)
{
	ocean_assert(subsetSize_ == 0 || (elements_ != nullptr && subsetIndices_ != nullptr));
}

template <typename T, typename TIndex>
inline ConstTemplateArraySubsetAccessor<T, TIndex>::ConstTemplateArraySubsetAccessor(const T* elements, const std::vector<TIndex>& subsetIndices) :
	elements_(elements),
	subsetIndices_(subsetIndices.data()),
	subsetSize_(subsetIndices.size())
{
	ocean_assert(subsetSize_ == 0 || (elements_ != nullptr && subsetIndices_ != nullptr));
}

template <typename T, typename TIndex>
inline ConstTemplateArraySubsetAccessor<T, TIndex>::ConstTemplateArraySubsetAccessor(const std::vector<T>& elements, const std::vector<TIndex>& subsetIndices) :
	elements_(elements.data()),
	subsetIndices_(subsetIndices.data()),
	subsetSize_(subsetIndices.size())
{
#ifdef OCEAN_DEBUG
	ocean_assert(subsetIndices.size() <= elements.size());
	for (size_t n = 0; n < subsetIndices.size(); ++n)
	{
		ocean_assert(subsetIndices[n] < elements.size());
	}
#endif
}

template <typename T, typename TIndex>
inline const T* ConstTemplateArraySubsetAccessor<T, TIndex>::data() const
{
	return nullptr;
}

template <typename T, typename TIndex>
inline size_t ConstTemplateArraySubsetAccessor<T, TIndex>::size() const
{
	return subsetSize_;
}

template <typename T, typename TIndex>
inline bool ConstTemplateArraySubsetAccessor<T, TIndex>::isEmpty() const
{
	return subsetSize_ == 0;
}

template <typename T, typename TIndex>
inline bool ConstTemplateArraySubsetAccessor<T, TIndex>::canAccess(const size_t& index) const
{
	return index < subsetSize_;
}

template <typename T, typename TIndex>
inline bool ConstTemplateArraySubsetAccessor<T, TIndex>::firstElement(T& element, size_t& index) const
{
	if (subsetSize_ == 0)
	{
		return false;
	}

	element = (*this)[0];
	index = 0;

	return true;
}

template <typename T, typename TIndex>
inline bool ConstTemplateArraySubsetAccessor<T, TIndex>::nextElement(const size_t& previousIndex, T& nextElement, size_t& nextIndex) const
{
	if (previousIndex + 1 >= subsetSize_)
	{
		return false;
	}

	nextIndex = previousIndex + 1;
	nextElement = (*this)[nextIndex];

	return true;
}

template <typename T, typename TIndex>
inline const T& ConstTemplateArraySubsetAccessor<T, TIndex>::operator[](const size_t& index) const
{
	ocean_assert(index < subsetSize_);

	return elements_[subsetIndices_[index]];
}

template <typename T, typename TIndex>
inline ConstTemplateArraySubsetAccessor<T, TIndex>& ConstTemplateArraySubsetAccessor<T, TIndex>::operator=(ConstTemplateArraySubsetAccessor<T, TIndex>&& accessor) noexcept
{
	if (this != &accessor)
	{
		elements_ = accessor.elements_;
		subsetIndices_ = accessor.subsetIndices_;
		subsetSize_ = accessor.subsetSize_;

		accessor.elements_ = nullptr;
		accessor.subsetIndices_ = nullptr;
		accessor.subsetSize_ = 0;
	}

	return *this;
}

template <typename T>
inline ConstCallbackIndexedAccessor<T>::ConstCallbackIndexedAccessor(const CallbackFunction& callback, const size_t size) :
	callback_(callback),
	size_(size)
{
	// nothing to do here
}

template <typename T>
const T& ConstCallbackIndexedAccessor<T>::operator[](const size_t& index) const
{
	ocean_assert(index < size_);
	ocean_assert(callback_);

	return callback_(index);
}

template <typename T>
size_t ConstCallbackIndexedAccessor<T>::size() const
{
	return size_;
}

template <typename T>
template <typename TAccessor>
inline ScopedConstMemoryAccessor<T>::ScopedConstMemoryAccessor(const TAccessor& accessor) :
	data_(nullptr),
	size_(0)
{
	data_ = accessor.data();
	size_ = accessor.size();

	if (data_ == nullptr && size_ != 0)
	{
		intermediateValues_.reserve(size_);

		for (size_t n = 0; n < size_; ++n)
		{
			intermediateValues_.emplace_back(accessor[n]);
		}

		data_ = intermediateValues_.data();
	}
}

template <typename T>
inline const T* ScopedConstMemoryAccessor<T>::data() const
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return data_;
}

template <typename T>
inline size_t ScopedConstMemoryAccessor<T>::size() const
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return size_;
}

template <typename T>
inline const T& ScopedConstMemoryAccessor<T>::operator[](const size_t index) const
{
	ocean_assert(data_ && index < size());

	return data_[index];
}

template <typename T>
inline ScopedConstMemoryAccessor<T>::operator bool() const
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return data_ != nullptr;
}

template <typename T>
inline ScopedNonconstMemoryAccessor<T>::ScopedNonconstMemoryAccessor(NonconstIndexedAccessor<T>& accessor) :
	accessor_(&accessor),
	data_(nullptr),
	size_(0)
{
	data_ = accessor.data();
	size_ = accessor.size();

	if (data_ == nullptr && size_ != 0)
	{
		// the provided accessor does not allow to access the elements as a joined memory block, so we create out own intermediate memory block

		intermediateValues_.reserve(size_);

		for (size_t n = 0; n < size_; ++n)
		{
			intermediateValues_.emplace_back(accessor[n]);
		}

		data_ = intermediateValues_.data();
	}
}

template <typename T>
inline ScopedNonconstMemoryAccessor<T>::ScopedNonconstMemoryAccessor(NonconstIndexedAccessor<T>* accessor, const size_t temporarySize) :
	accessor_(accessor),
	data_(nullptr),
	size_(0)
{
	if (accessor_)
	{
		data_ = accessor_->data();
		size_ = accessor_->size();

		if (data_ == nullptr && size_ != 0)
		{
			// the provided accessor does not allow to access the elements as a joined memory block, so we create out own intermediate memory block

			intermediateValues_.reserve(size_);

			for (size_t n = 0; n < size_; ++n)
			{
				intermediateValues_.emplace_back((*accessor_)[n]);
			}

			data_ = intermediateValues_.data();
		}
	}
	else
	{
		// no provided accessor so that we create our own intermediate memory block

		intermediateValues_.resize(temporarySize);

		data_ = intermediateValues_.data();
		size_ = intermediateValues_.size();
	}
}

template <typename T>
inline ScopedNonconstMemoryAccessor<T>::~ScopedNonconstMemoryAccessor()
{
	if (accessor_ && !intermediateValues_.empty())
	{
		ocean_assert(accessor_->data() == nullptr);
		ocean_assert(size_ == accessor_->size());

		for (size_t n = 0; n < size_; ++n)
		{
			(*accessor_)[n] = std::move(intermediateValues_[n]);
		}
	}
}

template <typename T>
inline T* ScopedNonconstMemoryAccessor<T>::data()
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return data_;
}

template <typename T>
inline size_t ScopedNonconstMemoryAccessor<T>::size() const
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return size_;
}

template <typename T>
inline T& ScopedNonconstMemoryAccessor<T>::operator[](const size_t index)
{
	ocean_assert(data_ && index < size());

	return data_[index];
}

template <typename T>
inline ScopedNonconstMemoryAccessor<T>::operator bool() const
{
	ocean_assert((data_ != nullptr && size_ != 0) || (data_ == nullptr && size_ == 0));

	return data_ != nullptr;
}

}

#endif // META_OCEAN_BASE_ACCESSOR_H
