// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#pragma once

#include "ocean/unifiedfeatures/UnifiedFeatures.h"

namespace Ocean
{

namespace UnifiedFeatures
{

/**
 * Definition of a class holding generic information about implementations
 * @ingroup unifiedfeatures
 */
class UnifiedObject
{
	public:

		/**
		 * Returns the name of the underlying implementation
		 * @return The name
		 */
		inline virtual const std::string& libraryName() const;

		/**
		 * Returns the name of this object
		 * @return The name
		 */
		inline virtual const std::string& name() const;

		/**
		 * Returns the version of the library object
		 * @return The version number
		 */
		inline virtual unsigned int objectVersion() const;

	protected:

		/**
		 * Constructor
		 * @param libraryName The name of the underlying implementation, must be valid
		 * @param objectName The name of the library object, must be valid
		 * @param objectVersion Optional object version, range: [0, infinity)
		 */
		inline UnifiedObject(const std::string& libraryName, const std::string& objectName, const unsigned int objectVersion = 0u);

		/**
		 * Default destructor
		 */
		virtual ~UnifiedObject() = default;

	protected:

		/// The name of the underlying implementation
		const std::string libraryName_;

		/// The name of the library component
		const std::string objectName_;

		/// The version number of the library component
		const unsigned int objectVersion_;
};

inline const std::string& UnifiedObject::libraryName() const
{
	return libraryName_;
}

inline const std::string& UnifiedObject::name() const
{
	return objectName_;
}

inline unsigned int UnifiedObject::objectVersion() const
{
	return objectVersion_;
}

UnifiedObject::UnifiedObject(const std::string& libraryName, const std::string& objectName, const unsigned int objectVersion) :
	libraryName_(libraryName),
	objectName_(objectName),
	objectVersion_(objectVersion)
{
	// Nothing else to do.
}

} // namespace UnifiedFeatures

} // namespace Ocean
