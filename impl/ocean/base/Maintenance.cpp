/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Maintenance.h"

namespace Ocean
{

bool Maintenance::Connector::place(const std::string& name, const unsigned long long id, const std::string& tag, Buffer&& buffer, const Timestamp timestamp)
{
	return Maintenance::get().place(name, id, tag, std::move(buffer), timestamp);
}

void Maintenance::Connector::encodeData(const std::string& name, const unsigned long long id, const std::string& tag, const Buffer& buffer, const Timestamp timestamp, const size_t headerSize, Buffer& encodedBuffer)
{
	// header: headerSize byte
	// timestamp: 8 byte
	// name: 8 byte + 1 byte * name.length()
	// id: 8 byte
	// tag: 8 byte + 1 byte * tag.length()
	// buffer: 8 byte + 1 byte * buffer.size()

	static_assert(sizeof(Timestamp) == 8, "Invalid data type!");
	static_assert(sizeof(unsigned char) == 1, "Invalid data type!");
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

	encodedBuffer.resize(headerSize + 8 + 8 + name.length() + 8 + 8 + 8 + tag.length() + buffer.size());
	unsigned char* data = encodedBuffer.data() + headerSize;

	ocean_assert(data + 8 < encodedBuffer.data() + encodedBuffer.size());
	*((Timestamp*)data) = timestamp;
	data += 8;

	ocean_assert(data + 8 < encodedBuffer.data() + encodedBuffer.size());
	*((unsigned long long*)data) = (unsigned long long)name.length();
	data += 8;

	ocean_assert(data + name.length() < encodedBuffer.data() + encodedBuffer.size());
	memcpy(data, name.c_str(), name.length());
	data += name.length();

	ocean_assert(data + 8 < encodedBuffer.data() + encodedBuffer.size());
	*((unsigned long long*)data) = id;
	data += 8;

	ocean_assert(data + 8 < encodedBuffer.data() + encodedBuffer.size());
	*((unsigned long long*)data) = (unsigned long long)tag.length();
	data += 8;

	ocean_assert(data + tag.length() < encodedBuffer.data() + encodedBuffer.size());
	memcpy(data, tag.c_str(), tag.length());
	data += tag.length();

	ocean_assert(data + 8 < encodedBuffer.data() + encodedBuffer.size());
	*((unsigned long long*)data) = (unsigned long long)buffer.size();
	data += 8;

	ocean_assert(data + buffer.size() <= encodedBuffer.data() + encodedBuffer.size());
	memcpy(data, buffer.data(), buffer.size());
	data += buffer.size();

	ocean_assert(data == encodedBuffer.data() + encodedBuffer.size());
}

bool Maintenance::Connector::decodeData(const void* encodedBuffer, const size_t encodedBufferSize, std::string& name, unsigned long long& id, std::string& tag, Buffer& buffer, Timestamp& timestamp)
{
	ocean_assert(encodedBuffer != nullptr && encodedBufferSize != 0);

	// timestamp: 8 byte
	// name: 8 byte + 1 byte * name.length()
	// id: 8 byte
	// tag: 8 byte + 1 byte * tag.length()
	// buffer: 8 byte + 1 byte * buffer.size()

	static_assert(sizeof(Timestamp) == 8, "Invalid data type!");
	static_assert(sizeof(unsigned char) == 1, "Invalid data type!");
	static_assert(sizeof(unsigned long long) == 8, "Invalid data type!");

	if (encodedBufferSize < 8 + 8 + 8 + 8 + 8)
		return false;

	const unsigned char* data = (unsigned char*)encodedBuffer;
	const unsigned char* const dataEnd = data + encodedBufferSize;

	unsigned long long size;

	timestamp = *((Timestamp*)data);
	data += 8;

	if (data + 8 > dataEnd)
		return false;

	size = *((unsigned long long*)data);
	data += 8;

	if (data + size > dataEnd || size > encodedBufferSize)
		return false;

	name = std::string((const char*)data, size_t(size));
	data += size;

	if (data + 8 > dataEnd)
		return false;

	id = *((unsigned long long*)data);
	data += 8;

	if (data + 8 > dataEnd)
		return false;

	size = *((unsigned long long*)data);
	data += 8;

	if (data + size > dataEnd || size > encodedBufferSize)
		return false;

	tag = std::string((const char*)data, size_t(size));
	data += size;

	if (data + 8 > dataEnd)
		return false;

	size = *((unsigned long long*)data);
	data += 8;

	if (data + size > dataEnd || size > encodedBufferSize)
		return false;

	buffer.resize(size_t(size));
	memcpy(buffer.data(), data, size_t(size));
	data += size;

	return data == dataEnd;
}

bool Maintenance::send(const std::string& tag, const void* data, const size_t size, const Timestamp timestamp)
{
	ocean_assert(data && size != 0);

	const ScopedLock scopedLock(maintenanceLock);

	if (!maintenanceActive || data == nullptr || size == 0)
		return false;

	Buffer buffer(size);
	memcpy(buffer.data(), data, size);

	maintenanceElementQueue.push(Element(maintenanceName, maintenanceId, timestamp, tag, std::move(buffer)));
	return true;
}

bool Maintenance::send(const std::string& tag, const Buffer& buffer, const Timestamp timestamp)
{
	const ScopedLock scopedLock(maintenanceLock);

	if (!maintenanceActive || buffer.empty())
		return false;

	maintenanceElementQueue.push(Element(maintenanceName, maintenanceId, timestamp, tag, buffer));
	return true;
}

bool Maintenance::send(const std::string& tag, Buffer&& buffer, const Timestamp timestamp)
{
	const ScopedLock scopedLock(maintenanceLock);

	if (!maintenanceActive || buffer.empty())
		return false;

	maintenanceElementQueue.push(Element(maintenanceName, maintenanceId, timestamp, tag, std::move(buffer)));
	return true;
}

bool Maintenance::place(const std::string& name, const unsigned long long id, const std::string& tag, Buffer&& buffer, const Timestamp timestamp)
{
	const ScopedLock scopedLock(maintenanceLock);

	if (buffer.empty())
		return false;

	maintenanceElementQueue.push(Element(name, id, timestamp, tag, std::move(buffer)));
	return true;
}

bool Maintenance::receive(std::string& name, unsigned long long& id, std::string& tag, Buffer& buffer, Timestamp& timestamp)
{
	const ScopedLock scopedLock(maintenanceLock);

	if (maintenanceElementQueue.empty())
		return false;

	Element& element = maintenanceElementQueue.front();

	name = std::move(element.name());
	id = element.id();
	tag = std::move(element.tag());
	buffer = std::move(element.buffer());
	timestamp = element.timestamp();

	maintenanceElementQueue.pop();

	return true;
}

}
