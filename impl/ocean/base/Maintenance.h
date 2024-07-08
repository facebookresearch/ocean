/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_MAINTENANCE_H
#define META_OCEAN_BASE_MAINTENANCE_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Timestamp.h"

#include <queue>
#include <vector>

namespace Ocean
{

/**
 * This class implements a maintenance manager.
 * The maintenance manager allows to transport maintenance data, maintenance information or maintenance messages from an arbitrary component to a central component handling or forwarding the data.<br>
 * Further, the maintenance manager can receive data, information or massages from a connector that receives arbitrary maintenance information from a remote component so that it can be distributed by this maintenance manager.<br>
 * An application can have at most one instance of a maintenance manager.<br>
 * Beware: The maintenance manager accepts data only if the manager is active (the manager is deactivated by default).<br>
 * Due to performance issues: Check whether the manager is active before preparing maintenance information to save computational time.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Maintenance : public Singleton<Maintenance>
{
	friend class Singleton<Maintenance>;

	public:

		/**
		 * Definition of a vector holding bytes.
		 */
		typedef std::vector<unsigned char> Buffer;

		/**
		 * This class is the base class for all maintenance connectors.
		 * A maintenance connector connects a local maintenance manager with a remote maintenance manager to transmit the information.<br>
		 * The actual implementation of any maintenance connector must be done in a derived class, thus individual connectors with individual capabilities can be implemented.<br>
		 */
		class OCEAN_BASE_EXPORT Connector
		{
			protected:

				/**
				 * Creates a new connector.
				 */
				inline Connector();

				/**
				 * Explicitly places a maintenance data, information, message into the local maintenance manager.
				 * Explicit maintenance data can be set even if the manager is not active.<br>
				 * @param name The name of the maintenance manager belonging to the data which will be placed
				 * @param id The id of the maintenance manager belonging to the data which will be placed
				 * @param tag The tag of the maintenance data
				 * @param buffer The buffer of the maintenance data, which will be moved
				 * @param timestamp The timestamp of the maintenance data
				 * @return True, if succeeded
				 * @see Maintenance::place().
				 */
				static bool place(const std::string& name, const unsigned long long id, const std::string& tag, Buffer&& buffer, const Timestamp timestamp);

				/**
				 * Encodes a maintenance data to one combined package.
				 * @param name The name of the maintenance manager providing the data
				 * @param id The id of the maintenance manager providing the data
				 * @param tag The tag of the maintenance data
				 * @param buffer The maintenance data as buffer
				 * @param timestamp The timestamp of the maintenance data
				 * @param reservedHeaderSize The number of bytes which will be reserved for the header, so that the resulting buffer has an optional header followed by the payload data
				 * @param encodedBuffer The resulting encoded package
				 */
				static void encodeData(const std::string& name, const unsigned long long id, const std::string& tag, const Buffer& buffer, const Timestamp timestamp, const size_t reservedHeaderSize, Buffer& encodedBuffer);

				/**
				 * Decodes a package buffer to maintenance data with corresponding information.
				 * @param encodedBuffer The encoded network package buffer
				 * @param encodedBufferSize The size of the encoded buffer in bytes
				 * @param name The resulting name of the maintenance manager to which the data has been sent originally
				 * @param id The result id of the maintenance manager to which the data has been sent originally
				 * @param tag The tag of the maintenance data
				 * @param buffer The buffer of the maintenance data
				 * @param timestamp The timestamp of the maintenance data
				 * @return True, if succeeded
				 */
				static bool decodeData(const void* encodedBuffer, const size_t encodedBufferSize, std::string& name, unsigned long long& id, std::string& tag, Buffer& buffer, Timestamp& timestamp);
		};

	protected:

		/**
		 * This class implements a maintenance data element.
		 */
		class Element
		{
			public:

				/**
				 * Creates a new empty element.
				 */
				inline Element();

				/**
				 * Copy constructor.
				 * @param element The element to be copied
				 */
				inline Element(const Element& element);

				/**
				 * Move constructor.
				 * @param element The element to be moved
				 */
				inline Element(Element&& element);

				/**
				 * Creates a new maintenance element.
				 * @param name The name of the maintenance manager to which the data has been sent
				 * @param id The id of the maintenance manager to which the data has been set
				 * @param timestamp The timestamp of the maintenance data
				 * @param tag The tag of the maintenance data
				 * @param buffer The maintenance data as buffer
				 */
				inline Element(const std::string& name, const unsigned long long id, const Timestamp timestamp, const std::string& tag, const Buffer& buffer);

				/**
				 * Creates a new maintenance element.
				 * @param name The name of the maintenance manager to which the data has been sent
				 * @param id The id of the maintenance manager to which the data has been set
				 * @param timestamp The timestamp of the maintenance data
				 * @param tag The tag of the maintenance data
				 * @param buffer The maintenance data as buffer, will be moved
				 */
				inline Element(const std::string& name, const unsigned long long id, const Timestamp timestamp, const std::string& tag, Buffer&& buffer);

				/**
				 * Returns the name of the maintenance manager to which the maintenance of this element data has been sent.
				 * @return The readable name of the maintenance manager
				 */
				inline const std::string& name() const;

				/**
				 * Returns the name of the maintenance manager to which the maintenance of this element data has been sent.
				 * @return The readable name of the maintenance manager
				 */
				inline std::string& name();

				/**
				 * Return sthe id of the maintenance manager to which the maintenance data of this element has been sent.
				 * @return The id of the maintenance manager
				 */
				inline unsigned long long id() const;

				/**
				 * The timestamp of the maintenance data of this element.
				 * @return The timestamp of the maintenance data
				 */
				inline Timestamp timestamp() const;

				/**
				 * Returns the tag of the maintenance data of this element.
				 * @return The tag of the maintenance data
				 */
				inline const std::string& tag() const;

				/**
				 * Returns the tag of the maintenance data of this element.
				 * @return The tag of the maintenance data
				 */
				inline std::string& tag();

				/**
				 * Returns the buffer of the maintenance data of this element.
				 * @return The buffer of the maintenance data
				 */
				inline const Buffer& buffer() const;

				/**
				 * Returns the buffer of the maintenance data of this element.
				 * @return The buffer of the maintenance data
				 */
				inline Buffer& buffer();

				/**
				 * Assign operator.
				 * @param element The second element to be copied
				 * @return Reference to this element
				 */
				inline Element& operator=(const Element& element);

				/**
				 * Assign operator.
				 * @param element The second element to be moved
				 * @return Reference to this element
				 */
				inline Element& operator=(Element&& element);

			protected:

				/// The name of the manager to which the data has been sent.
				std::string elementName;

				/// The id of the manager to which the data has been sent.
				unsigned long long elementId;

				/// The timestamp of the data.
				Timestamp elementTimestamp;

				/// The tag of the data.
				std::string elementTag;

				/// The buffer of the data.
				Buffer elementBuffer;
		};

		/**
		 * Definition of a vector holding maintenance data elements.
		 */
		typedef std::queue<Element> ElementQueue;

	public:

		/**
		 * Returns whether the maintenance manager is active or not.
		 * Check whether the manager is active before preparing information which will be forwarded to this manager.<br>
		 * @return True, if so
		 * @see setActive().
		 */
		inline bool isActive() const;

		/**
		 * Returns the name of this maintenance manager.
		 * @return The readable name of this manager
		 * @see setName().
		 */
		inline std::string name() const;

		/**
		 * Returns a random id of this maintenance manager.
		 * The id provides a random 64 bit number allowing to distinguish between individual maintenance managers with same name (e.g., distributed in a large system connected by a network).
		 * @return The random id of this manager
		 */
		inline unsigned long long id() const;

		/**
		 * Returns whether this maintenance manager is currently empty (does not hold any maintenance data, information or messages).
		 * @return True, if so
		 */
		inline bool isEmpty() const;

		/**
		 * Returns the number of maintenance data, information or messages which are currently stored in this manager.
		 * @return The number of maintenance messages
		 */
		inline size_t size() const;

		/**
		 * Activates or deactivates this maintenance manager.
		 * By default this manager is deactivates and thus will not accept any data, information or messages.<br>
		 * @param state True, to activate this manager; False; to deactivate this manager
		 * @see active().
		 */
		inline void setActive(const bool state);

		/**
		 * Sets the name of this maintenance manager which should be a readable name of the application in which this manager is used.
		 * The name should be set once at application start.<br>
		 * @param name The readable name to set
		 * @see name().
		 */
		inline void setName(const std::string& name);

		/**
		 * Sends new maintenance data to this manager.
		 * Check whether this manager is active before.<br>
		 * @param tag A tag specifying purpose of the provided data
		 * @param data The data to be sent
		 * @param size The size of the data to be sent, in bytes
		 * @param timestamp The timestamp of the maintenance data
		 * @return True, if the data could be sent
		 */
		bool send(const std::string& tag, const void* data, const size_t size, const Timestamp timestamp = Timestamp(true));

		/**
		 * Sends new maintenance data to this manager.
		 * Check whether this manager is active before.<br>
		 * @param tag A tag specifying purpose of the provided data
		 * @param buffer The data buffer to be sent
		 * @param timestamp The timestamp of the maintenance data
		 * @return True, if the data could be sent
		 */
		bool send(const std::string& tag, const Buffer& buffer, const Timestamp timestamp = Timestamp(true));

		/**
		 * Sends new maintenance data to this manager.
		 * Check whether this manager is active before.<br>
		 * @param tag A tag specifying purpose of the provided data
		 * @param buffer The data buffer to be sent, which will be moved
		 * @param timestamp The timestamp of the maintenance data
		 * @return True, if the data could be sent
		 */
		bool send(const std::string& tag, Buffer&& buffer, const Timestamp timestamp = Timestamp(true));

		/**
		 * Receives the oldest maintenance data from this manager and pops it from the manager.
		 * Maintenance data can be received even if the manager is not active.<br>
		 * @param name The name of the maintenance manager to which the data has been sent
		 * @param id The id of the maintenance manager to which the data has been sent
		 * @param tag The tag of the maintenance data
		 * @param buffer The maintenance data as buffer
		 * @param timestamp The timestamp of the maintenance data
		 * @return True, this manager had data which has been received
		 */
		bool receive(std::string& name, unsigned long long& id, std::string& tag, Buffer& buffer, Timestamp& timestamp);

		/**
		 * Combines two buffers.
		 * @param firstBuffer The first buffer
		 * @param secondBuffer The second buffer
		 * @return The resulting combined buffer
		 */
		static inline Buffer combine(const Buffer& firstBuffer, const Buffer& secondBuffer);

		/**
		 * Appends a second buffer to a first buffer.
		 * @param firstBuffer The first buffer to which the second buffer will be appended
		 * @param secondBuffer The second buffer
		 */
		static inline void appendBuffer(Buffer& firstBuffer, const Buffer& secondBuffer);

	protected:

		/**
		 * Creates a new maintenance manager object.
		 */
		inline Maintenance();

		/**
		 * Explicitly places a maintenance data, information, message into this manager.
		 * Explicit maintenance data can be set even if the manager is not active.<br>
		 * @param name The name of the maintenance manager belonging to the data which will be placed
		 * @param id The id of the maintenance manager belonging to the data which will be placed
		 * @param tag The tag of the maintenance data
		 * @param buffer The buffer of the maintenance data, which will be moved
		 * @param timestamp The timestamp of the maintenance data
		 * @return True, if succeeded
		 * @see Connector::place().
		 */
		bool place(const std::string& name, const unsigned long long id, const std::string& tag, Buffer&& buffer, const Timestamp timestamp);

	protected:

		/// The activation statement of this manager.
		bool maintenanceActive;

		/// The readable name of this manager.
		std::string maintenanceName;

		/// The random id of this manager.
		unsigned long long maintenanceId;

		/// The maintenance element queue.
		ElementQueue maintenanceElementQueue;

		/// The maintenance lock.
		mutable Lock maintenanceLock;
};

inline Maintenance::Connector::Connector()
{
	// nothing to do here
}

inline Maintenance::Element::Element() :
	elementId(0ull),
	elementTimestamp(false)
{
	// nothing to do here
}

inline Maintenance::Element::Element(const Element& element) :
	elementName(element.elementName),
	elementId(element.elementId),
	elementTimestamp(element.elementTimestamp),
	elementTag(element.elementTag),
	elementBuffer(element.elementBuffer)
{
	// nothing to do here
}

inline Maintenance::Element::Element(Element&& element) :
	elementName(std::move(element.elementName)),
	elementId(element.elementId),
	elementTimestamp(element.elementTimestamp),
	elementTag(std::move(element.elementTag)),
	elementBuffer(std::move(element.elementBuffer))
{
	element.elementId = 0ull;
	element.elementTimestamp.toInvalid();
}

inline Maintenance::Element::Element(const std::string& name, const unsigned long long id, const Timestamp timestamp, const std::string& tag, const Buffer& buffer) :
	elementName(name),
	elementId(id),
	elementTimestamp(timestamp),
	elementTag(tag),
	elementBuffer(buffer)
{
	// nothing to do here
}

inline Maintenance::Element::Element(const std::string& name, const unsigned long long id, const Timestamp timestamp, const std::string& tag, Buffer&& buffer) :
	elementName(name),
	elementId(id),
	elementTimestamp(timestamp),
	elementTag(tag),
	elementBuffer(std::move(buffer))
{
	// nothing to do here
}

inline const std::string& Maintenance::Element::name() const
{
	return elementName;
}

inline std::string& Maintenance::Element::name()
{
	return elementName;
}

inline unsigned long long Maintenance::Element::id() const
{
	return elementId;
}

inline Timestamp Maintenance::Element::timestamp() const
{
	return elementTimestamp;
}

inline const std::string& Maintenance::Element::tag() const
{
	return elementTag;
}

inline std::string& Maintenance::Element::tag()
{
	return elementTag;
}

inline const Maintenance::Buffer& Maintenance::Element::buffer() const
{
	return elementBuffer;
}

inline Maintenance::Buffer& Maintenance::Element::buffer()
{
	return elementBuffer;
}

inline Maintenance::Element& Maintenance::Element::operator=(const Element& element)
{
	elementName = element.elementName;
	elementId = element.elementId;
	elementTimestamp = element.elementTimestamp;
	elementTag = element.elementTag;
	elementBuffer = element.elementBuffer;

	return *this;
}

inline Maintenance::Element& Maintenance::Element::operator=(Element&& element)
{
	if (this != &element)
	{
		elementName = std::move(element.elementName);
		elementId = element.elementId;
		elementTimestamp = element.elementTimestamp;
		elementTag = std::move(element.elementTag);
		elementBuffer = std::move(element.elementBuffer);

		element.elementId = 0ull;
		element.elementTimestamp.toInvalid();
	}

	return *this;
}

inline Maintenance::Maintenance() :
	maintenanceActive(false),
	maintenanceId(RandomI::random64())
{
	while (maintenanceId == 0ull)
		maintenanceId = RandomI::random64();
}

inline bool Maintenance::isActive() const
{
	const ScopedLock scopedLock(maintenanceLock);
	return maintenanceActive;
}

inline std::string Maintenance::name() const
{
	const ScopedLock scopedLock(maintenanceLock);
	return maintenanceName;
}

inline unsigned long long Maintenance::id() const
{
	const ScopedLock scopedLock(maintenanceLock);
	return maintenanceId;
}

inline bool Maintenance::isEmpty() const
{
	const ScopedLock scopedLock(maintenanceLock);
	return maintenanceElementQueue.empty();
}

inline size_t Maintenance::size() const
{
	const ScopedLock scopedLock(maintenanceLock);
	return maintenanceElementQueue.size();
}

inline void Maintenance::setActive(const bool state)
{
	const ScopedLock scopedLock(maintenanceLock);
	maintenanceActive = state;
}

inline void Maintenance::setName(const std::string& name)
{
	const ScopedLock scopedLock(maintenanceLock);
	maintenanceName = name;
}

inline Maintenance::Buffer Maintenance::combine(const Buffer& firstBuffer, const Buffer& secondBuffer)
{
	Buffer result(firstBuffer.size() + secondBuffer.size());
	memcpy(result.data(), firstBuffer.data(), firstBuffer.size());
	memcpy(result.data() + firstBuffer.size(), secondBuffer.data(), secondBuffer.size());

	return result;
}

inline void Maintenance::appendBuffer(Buffer& firstBuffer, const Buffer& secondBuffer)
{
	const size_t firstBufferSize = firstBuffer.size();

	firstBuffer.resize(firstBuffer.size() + secondBuffer.size());
	memcpy(firstBuffer.data() + firstBufferSize, secondBuffer.data(), secondBuffer.size());
}

}

#endif // META_OCEAN_BASE_MAINTENANCE_H
