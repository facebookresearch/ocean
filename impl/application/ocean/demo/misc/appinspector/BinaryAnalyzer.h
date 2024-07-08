/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_BINARY_ANALYZER_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_BINARY_ANALYZER_H

#include "ocean/base/Base.h"
#include "ocean/base/Thread.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/JSONConfig.h"

#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Forward declaration.
class BinaryAnalyzer;

/**
 * Definition of a shared pointer holding a BinaryAnalyzer object.
 * @see BinaryAnalyzer.
 * @ingroup applicationdemomiscappinspectorosx
 */
typedef std::shared_ptr<BinaryAnalyzer> BinaryAnalyzerRef;

/**
 * This class implements an analyzer for binary files analyzing existing symbols,
 * connecting them to a call graph and gathering the amount of bytes necessary for each symbol.
 * @ingroup applicationdemomiscappsizeanalyzerosx
 */
class BinaryAnalyzer : protected Ocean::Thread
{
	friend class Disassembler;
	friend class DisassemblerAndroid;
	friend class DisassemblerIOS;

	public:

		/**
		 * Definition of individual analyzer states.
		 */
		enum AnalyzerState : unsigned int
		{
			/// The analyzer is empty.
			AS_EMPTY,
			/// The analyer is working.
			AS_WORKING,
			/// The analyzer has failed.
			AS_FAILED,
			/// The analyzer has succeeded.
			AS_SUCCEEDED
		};

		/**
		 * Definition of a unique id for objects.
		 */
		typedef size_t ObjectId;

		/**
		 * Definition of a unique id for symbols.
		 */
		typedef ObjectId SymbolId;

		/**
		 * Definition of an invalid symbol id.
		 */
		static const SymbolId invalidSymbolId = SymbolId(-1);

		/**
		 * Definition of a unique id for strings.
		 */
		typedef ObjectId StringId;

		/**
		 * Definition of an invalid string id.
		 */
		static const SymbolId invalidStringId = StringId(-1);

		/**
		 * Definition of a 64 bit address.
		 */
		typedef unsigned long long Address64;

		/**
		 * Definition of a vector holding symbol ids.
		 */
		typedef std::vector<SymbolId> SymbolIds;

		/**
		 * Definition of a vector holding groups of symbol ids.
		 */
		typedef std::vector<SymbolIds> SymbolIdGroups;

		/**
		 * Definition of a set holding addresses.
		 */
		typedef std::unordered_set<Address64> AddressSet64;

		/**
		 * Definition of a set holding symbol ids.
		 */
		typedef std::unordered_set<SymbolId> SymbolIdSet;

		/**
		 * Definition of a set holding string ids.
		 */
		typedef std::unordered_set<StringId> StringIdSet;

		/**
		 * This class defines a base class for all objects having a location and a size within a binary.
		 */
		class Object
		{
			friend class BinaryAnalyzer;
			friend class DisassemblerAndroid;
			friend class DisassemblerIOS;

			public:

				/**
				 * Returns the address of the symbol.
				 * @return The symbol's address
				 */
				inline Address64 address() const;

				/**
				 * Returns the size of the symbol.
				 * @return The symbol's size
				 */
				inline unsigned int size() const;

			protected:

				/**
				 * Creates a new object with invalid address.
				 */
				inline Object();

				/**
				 * Creates a new object with known address and size
				 * @param address The address of the object (e.g., the address of a symbol in the binary)
				 * @param size The size of the object (e.g., the number of bytes a symbol is composed of, or the number of bytes of a string in memory)
				 */
				inline Object(const Address64 address, const unsigned int size);

				/**
				 * Sets the address of the symbol.
				 * @param address The address to be set
				 */
				inline void setAddress(const Address64 address);

				/**
				 * Sets the size of the symbol.
				 * @param size The size to be set
				 */
				inline void setSize(const unsigned int size);

			protected:

				/// The address of the symbol.
				Address64 address_;

				/// The size of the symbol.
				unsigned int size_;
		};

		/**
		 * This class defines a string object composed of the string's address and the value of the string (the string itself).
		 */
		class TextString : public Object
		{
			public:

				/**
				 * Creates a new string object based on the address, the length of the string and the string itself.
				 * @param address The address of the string
				 * @param size The number of characters (the number of bytes the string covers)
				 * @param value The string itself
				 */
				inline TextString(const Address64 address, const unsigned int size, std::string&& value);

				/**
				 * Returns the value of the string (the string itself).
				 * @return The string's value
				 */
				inline const std::string& value() const;

			protected:

				/// The value of the string.
				std::string value_;
		};

		/**
		 * This class defines a symbol composed of the symbol's name, address, size, related data objects, child-symbols, and parent-symbols.
		 */
		class Symbol : public Object
		{
			friend class BinaryAnalyzer;
			friend class Disassembler;
			friend class DisassemblerAndroid;
			friend class DisassemblerIOS;

			public:

				/**
				 * Creates a new symbol defined by the name of the symbol.
				 * @param id The id of this symbol, must be valid
				 * @param name The name of the symbol, must be valid, will be moved
				 * @param readableName The readable (demangled) name of the symbol, can be invalid
				 */
				inline Symbol(const SymbolId id, std::string&& name, std::string&& readableName = std::string());

				/**
				 * Returns the id of this symbol.
				 * @return The symbol's id
				 */
				inline SymbolId id() const;

				/**
				 * Returns the name (not demangled) of the symbol.
				 * @return The symbol's name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the readable (demangled) name of the symbol.
				 * @return The symbols's readable name
				 */
				inline const std::string& readableName() const;

				/**
				 * Returns all alias names of this symbol (not demangled alias names).
				 * @return The alias names, if any
				 */
				inline const std::vector<std::string>& aliasNames() const;

				/**
				 * Returns all readable alias names of this symbol.
				 * @return The alias names, if any
				 */
				inline const std::vector<std::string>& readableAliasNames() const;

				/**
				 * Returns the ids of all parent symbols of this symbol.
				 * Parent symbols are symbols that use (branch to) this symbol.
				 * @return The ids of all parent symbols
				 */
				inline const SymbolIdSet& parentSymbols() const;

				/**
				 * Returns the ids of all child symbols of this symbol.
				 * Child symbols are symbols that are used (branched) within this symbol.
				 * @return The ids of all child symbols
				 */
				inline const SymbolIdSet& childSymbols() const;

				/**
				 * Returns the ids of all strings of this symbol.
				 * @return The ids of all strings used in this symbol
				 */
				inline const StringIdSet& strings() const;

				/**
				 * Returns the all addresses that are loaded in this symbol.
				 * @return The addresses loaded
				 */
				inline const AddressSet64& loadedAddressed() const;

				/**
				 * Returns whether this symbol is a root symbol.
				 * A root symbol is a symbol not invoked by other symbols (via branching) - at least not within one binary.
				 * @return True, if so
				 */
				inline bool isRootSymbol() const;

			protected:

				/**
				 * Creates a new symbol defined by the name and readable name of the symbol.
				 * @param id The id of this symbol, must be valid
				 * @param name The string containing the name of the symbol, will be moved, must be valid
				 * @param readableName The string containing the readable name of the symbol, will be moved, must be valid
				 * @param address The address of the symbol, must be valid
				 * @param size The size of the symbol, must be valid
				 * @param parentSymbols The ids of all parent symbols, will be moved
				 * @param childSymbols The ids of all child synmbols, will be moved
				 * @param loadedAddresses The address that are loaded in this new symbol, will be moved
				 */
				inline Symbol(const SymbolId id, std::string&& name, std::string&& readableName, const Address64 address, const unsigned int size, SymbolIdSet&& parentSymbols, SymbolIdSet&& childSymbols, AddressSet64&& loadedAddresses);

				/**
				 * Adds the id of a symbol which uses this symbol.
				 * @param parentId The id of the parent symbol to be added
				 */
				inline void addParentSymbol(const SymbolId& parentId);

				/**
				 * Adds the id of a symbol which is used in this symbol.
				 * @param childId The id of the child symbol to be added
				 */
				inline void addChildSymbol(const SymbolId& childId);

				/**
				 * Adds the id of a string which is used in this symbol.
				 * @param stringId The id of the string to be added
				 */
				inline void addString(const StringId& stringId);

				/**
				 * Adds an address that is loaded in this symbol.
				 * @param address The address to be added
				 */
				inline void addLoadedAddress(const Address64& address);

			protected:

				/// The id of this symbol.
				SymbolId id_;

				/// The name (not demangled) of the symbol.
				std::string name_;

				/// The readable name (demangled) of the symbol.
				std::string readableName_;

				/// The alias names of this symbol.
				std::vector<std::string> aliasNames_;

				/// The readable alias names of this symbol.
				std::vector<std::string> readableAliasNames_;

				/// The set holding the ids of all parent symbols.
				SymbolIdSet parentSymbols_;

				/// The set holding the ids of all child symbols.
				SymbolIdSet childSymbols_;

				/// The set holding the ids of all strings.
				StringIdSet strings_;

				/// The set holding all addresses loaded within this symbol.
				AddressSet64 loadedAddresses_;
		};

		// Forward declaration.
		class GraphNode;

		/**
		 * Definition of a shared pointer reference for GraphNode objects.
		 */
		typedef std::shared_ptr<GraphNode> GraphNodeRef;

		/**
		 * Definition of a vector holding graph node references.
		 */
		typedef std::vector<GraphNodeRef> GraphNodeRefs;

		/**
		 * This class implements a simple node of a call graph.
		 */
		class GraphNode
		{
			friend class BinaryAnalyzer;

			public:

				/**
				 * Creates a new graph node object for a specified symbol.
				 * @param symbolId The id of the symbol for which the node will be created, must be valid
				 */
				explicit inline GraphNode(const SymbolId symbolId);

				/**
				 * Returns the id of the symbol corresponding to this node.
				 * @return The symbol's id
				 */
				inline SymbolId symbolId() const;

				/**
				 * Returns the child nodes of this node.
				 * @return All child nodes
				 */
				const inline GraphNodeRefs& childNodes() const;

			protected:

				/// The id of the symbol to which this node belongs.
				SymbolId symbolId_;

				/// The child nodes of this node.
				GraphNodeRefs childNodes_;
		};

		/**
		 * Definition of a vector storing all text strings, the index is equivalent with the id of the string.
		 */
		typedef std::vector<TextString> TextStrings;

		/**
		 * Definition of a vector storing all symbols, the index is equivalent with the id of the symbol.
		 */
		typedef std::vector<Symbol> Symbols;

		/**
		 * Definition of a map mapping symbol addresses to symbol ids.
		 */
		typedef std::map<Address64, SymbolId> Address2SymbolIdMap;

		/**
		 * Definition of a map mapping symbol addresses to symbol ids.
		 */
		typedef std::map<Address64, StringId> Address2StringIdMap;

	protected:

		/**
		 * This struct allows to sort symbols based on their readable names.
		 * The struct can sort entries of symbol ids.
		 * @tparam tAscending True, if this struct creates an ascending order; False, to create a descending order
		 */
		template <bool tAscending>
		struct CompareSymbolNames
		{
			/**
			 * Creates a new sorting object.
			 * @param symbols All existing symbols providing the necessary data.
			 */
			inline CompareSymbolNames(const BinaryAnalyzer::Symbols& symbols);

			/**
			 * Compares two symbol ids and returns whether the first symbol belongs 'left' of the second symbol.
			 * @param firstId The id of the first symbol to compare, with range [0, symbols_.size())
			 * @param secondId The id of the second symbol to compare, with range [0, symbols_.size())
			 * @return True, if so
			 */
			inline bool operator()(const BinaryAnalyzer::SymbolId firstId, const BinaryAnalyzer::SymbolId secondId) const;

			/// All existing symbols.
			const BinaryAnalyzer::Symbols& symbols_;
		};

		/**
		 * This struct allows to sort symbols based on their sizes.
		 * The struct can sort entries of symbol ids.
		 * @tparam tAscending True, if this struct creates an ascending order; False, to create a descending order
		 */
		template <bool tAscending>
		struct CompareSymbolSizes
		{
			/**
			 * Creates a new sorting object.
			 * @param symbols All existing symbols providing the necessary data.
			 */
			inline CompareSymbolSizes(const BinaryAnalyzer::Symbols& symbols);

			/**
			 * Compares two symbol ids and returns whether the first symbol belongs 'left' of the second symbol.
			 * @param firstId The id of the first symbol to compare, with range [0, symbols_.size())
			 * @param secondId The id of the second symbol to compare, with range [0, symbols_.size())
			 * @return True, if so
			 */
			inline bool operator()(const BinaryAnalyzer::SymbolId firstId, const BinaryAnalyzer::SymbolId secondId) const;

			/// All existing symbols.
			const BinaryAnalyzer::Symbols& symbols_;
		};

		/**
		 * Definition of a group of sets of symbol ids.
		 */
		typedef std::vector<SymbolIdSet> SymbolIdSetGroup;

	public:

		/**
		 * Creates a new binary analyzer object.
		 */
		BinaryAnalyzer() = default;

		/**
		 * Returns the current state of this analyzer.
		 * @return The analyzer's state
		 * @see isState().
		 */
		inline AnalyzerState state() const;

		/**
		 * Returns whether the analyzer is currently in a specific state.
		 * @param state The state to check
		 * @return True, if so
		 * @see state().
		 */
		inline bool isState(const AnalyzerState state) const;

		/**
		 * Analyzes a given binary file (or reads a data file) synchronous.
		 * This function does not return before the file has been analyzed.
		 * @param filename The name of the binary file (or data file), must be valid
		 * @return True, if analyzing succeeded
		 * @see analyzeBinaryAsynchron().
		 */
		bool analyzeBinarySynchron(const std::string& filename);

		/**
		 * Analyzes a given binary file (or reads a data file) synchronous.
		 * This function does not return before the file has been analyzed.
		 * @param filenameBinary The name of the binary file (or data file), must be valid
		 * @param objdumpPath The path of the objdump tool user provides, must be valid
		 * @return True, if analyzing succeeded
		 * @see analyzeBinaryAsynchron().
		 */
		bool analyzeBinarySynchron(const std::string& filenameBinary, const std::string& objdumpPath);

		/**
		 * Analyzes a given binary file (or reads a data file) asynchronous.
		 * This function returns immediately.
		 * @param filename The name of the binary file (or data file), must be valid
		 * @return True, if analyzing could be started
		 * @see analyzeBinarySynchron().
		 */
		bool analyzeBinaryAsynchron(const std::string& filename);

		/**
		 * Analyzes a given binary file (or reads a data file) asynchronous.
		 * This function returns immediately.
		 * @param filename The name of the binary file (or data file), must be valid
		 * @param objdumpPath The path of the objdump tool user provides, must be valid
		 * @return True, if analyzing could be started
		 * @see analyzeBinarySynchron().
		 */
		bool analyzeBinaryAsynchron(const std::string& filename, const std::string& objdumpPath);

		/**
		 * Returns all strings (from segment "__TEXT", section "__cstring") that have been analyzed.
		 * Those strings are constant read-only strings.
		 * @return The string objects
		 */
		inline const TextStrings& textStrings() const;

		/**
		 * Returns the number of symbols this analyzer provides.
		 * @return The number of symbols
		 */
		inline size_t symbolSize() const;

		/**
		 * Returns a symbol of this analyzer.
		 * @param symbolId The id of the symbol to be returned, with range [0, symbolSize())
		 * @return The requested symbol
		 */
		inline const Symbol& symbol(const SymbolId symbolId) const;

		/**
		 * Returns all symbols (from segment "__TEXT", section "__text") that have been analyzed.
		 * @return All synbols, with indices corresponding to symbol ids
		 */
		inline const Symbols& symbols() const;

		/**
		 * Sets all strings that have been analyzed.
		 * @param textStrings The strings to be set to this analyzer object, will be moved
		 * @param address2StringIdMap The map mapping address values to string ids, one entry for each provided string, will be moved
		 */
		inline void setTextStrings(TextStrings&& textStrings, Address2StringIdMap&& address2StringIdMap);

		/**
		 * Sets all symbols that have been analyzed.
		 * @param symbols The symbols to be set to this analyzer object, will be moved
		 * @param address2SymbolIdMap The map mapping address values to symbol ids, one entry for each provided symbol, will be moved
		 */
		inline void setSymbols(Symbols&& symbols, Address2SymbolIdMap&& address2SymbolIdMap);

		/**
		 * Determines call traces for a specified symbol.
		 * One call trace contains all symbols between the specified symbol and the corresponding root symbol or leaf symbol (depending on 'toRoot").<br>
		 * A trace does not contain any branches so that the resulting traces may share a significant amount of similar routes.
		 * @param symbolId The id of the symbol for which the call traces will be determined
		 * @param toRoot True, to determined call traces starting at root (global) symbols and ending at the specified symbol; False, to determined traces starting at the specified symbol and ending at leaf symbols
		 * @return The resulting call traces, simply several groups of symbol ids
		 */
		SymbolIdGroups determineCallTraces(const SymbolId symbolId, const bool toRoot) const;

		/**
		 * Determines the call graph for all child symbols for a specified symbol.
		 * The specified symbol will be the root node of the call graph with child symbols beeing the branch and leaf nodes.<br>
		 * However, due to performance reasons, instead of providing the entire call graph, the resulting call graph will not add children to nodes already visited in a previous/neighboring branch.<br>
		 * Thus, the reduced call graph will not contain children of already visibed symbols.
		 * @param symbolId The id of the symbol for which the 'children' call graph will be determined
		 * @param allSymbolIds The ids of all symbols that are part of the resulting call graph
		 * @return The requested call graph with root node holding the specified symbol
		 */
		GraphNodeRef determineReducedCallGraphForChildren(const SymbolId symbolId, SymbolIdSet* allSymbolIds = nullptr) const;

		/**
		 * Determines all child symbols that are part of the call graph of a specified symbol.
		 * @param symbolId The id of the symbol for which all direct and indirect child symbols will be determined
		 * @return The ids of all child symbols, will not include the symbol itself
		 */
		SymbolIdSet determineChildGraphSymbols(const SymbolId symbolId) const;

		/**
		 * Determines all child symbols that are part of the call graph of several specified symbols.
		 * @param symbolIds The ids of the symbols for which all direct and indirect child symbols will be determined
		 * @return The ids of all child symbols, may contain a subset of the provided symbol in case they are direct or indirect child symbols of each other
		 */
		SymbolIdSet determineChildGraphSymbols(const SymbolIdSet& symbolIds) const;

		/**
		 * Determines the size that a symbol, connected resources, and child symbols need.
		 * @param symbolId The id of the symbol for which the size will be calculated
		 * @param minimalSize Resulting number of bytes, the specified symbol needs at least, with range [0, maximalSize]
		 * @param weightedSize Resulting weighted number of bytes, the specified symbol needs in relation, with range [minimalSize, maximalSize]
		 * @param maximalSize Resulting number of bytes, the specified symbol needs at most, with range [minimalSize, infinity)
		 */
		void determineSizeImpact(const SymbolId& symbolId, unsigned int& minimalSize, double& weightedSize, unsigned int& maximalSize) const;

		/**
		 * Determines the size that several symbols, connected resources, and child symbols need.
		 * @param symbolIds The ids of the symbols for which the size will be calculated, at least one
		 * @param minimalSize Resulting number of bytes, the specified symbol needs at least, with range [0, maximalSize]
		 * @param weightedSize Resulting weighted number of bytes, the specified symbol needs in relation, with range [minimalSize, maximalSize]
		 * @param maximalSize Resulting number of bytes, the specified symbol needs at most, with range [minimalSize, infinity)
		 */
		void determineSizeImpact(const SymbolIdSet& symbolIds, unsigned int& minimalSize, double& weightedSize, unsigned int& maximalSize) const;

		/**
		 * Determines the string that contains the binary size information about several specified symbols.
		 * @param symbolIds The ids of the symbols for which the binary size will be determined
		 * @return The resulting string containing the information
		 */
		std::string determineSizeImpactString(const SymbolIdSet& symbolIds) const;

		/**
		 * Clears all information of this analyzer.
		 * Ensure that this analyzer is not working if calling this function.
		 * @return True, if succeeded
		 */
		bool clear();

		/**
		 * Writes the information of this binary analyzer to a data file.
		 * @param filename The name of the file to which the information will be written
		 * @return True, if succeeded
		 * @see readFromDataFile().
		 */
		bool writeToDataFile(const std::string& filename);

		/**
		 * Writes the information of this binary analyzer to a JSON file.
		 * @param filename The name of the JSON file to which the information will be written
		 * @return True, if succeeded
		 * @see readFromDataFile().
		 */
		bool writeToJsonFile(const std::string& filename);

		/**
		 * Writes the information of this binary analyzer to a JSON config.
		 * @param config The JSON config to which the information will be written
		 * @see readFromDataFile().
		 */
		void writeToJsonConfig(Ocean::IO::JSONConfig& config);

		/**
		 * Sorts symbol ids based on their readable name.
		 * @param symbols All existing symbolds
		 * @param subsetSymbolIds The ids of the symbolds to be sorted, can be a subset of all possible symbol ids
		 * @param ascending True, to create an ascending order; False, to create a descending order
		 * @tparam T The container data type of the given symbol ids
		 */
		template <typename T>
		static inline void sortSymboldsByReadableName(const Symbols& symbols, T& subsetSymbolIds, const bool ascending);

		/**
		 * Sorts symbol ids based on their size.
		 * @param symbols All existing symbolds
		 * @param subsetSymbolIds The ids of the symbolds to be sorted, can be a subset of all possible symbol ids
		 * @param ascending True, to create an ascending order; False, to create a descending order
		 * @tparam T The container data type of the given symbol ids
		 */
		template <typename T>
		static inline void sortSymboldsBySize(const Symbols& symbols, T& subsetSymbolIds, const bool ascending);

		/**
		 * Converts traces of symbols all ending at identical or individual root nodes to nodes with graph hierarchy.
		 * @param parentTraces The symbol traces to root nodes
		 * @return The resulting graph nodes
		 */
		static GraphNodeRefs callTraces2callNodes(const SymbolIdGroups& parentTraces);

		/**
		 * Returns the file extension for data files.
		 * @return Data file's extension
		 */
		static inline std::string dataFileExtension();

		/**
		 * Returns the string for a size with corresponding KB suffix.
		 * @param size The size for which a string is created
		 */
		static std::string sizeString(const double size);

	protected:

		/**
		 * The disabled copy constructor.
		 * @param binaryAnalyzer The binary analyzer object to be copied
		 */
		BinaryAnalyzer(const BinaryAnalyzer& binaryAnalyzer) = delete;

		/**
		 * Reads the already analyzed information from a data file.
		 * @param filename The name of the file from which the information will be read
		 * @return True, if succeeded
		 * @see readFromDataFileV1(), readFromDataFileV2(), writeToDataFile().
		 */
		bool readFromDataFile(const std::string& filename);

		/**
		 * Reads the already analyzed information from a data file given with format version 1.
		 * @param bitstream The bitstream from which the information will be read, must start with the version number, must be valid
		 * @return True, if succeeded
		 * @see readFromDataFile(), writeToDataFile().
		 */
		bool readFromDataFileV1(Ocean::IO::InputBitstream& bitstream);

		/**
		 * Reads the already analyzed information from a data file given with format versin 2
		 * @param bitstream The bitstream from which the information will be read, must start with the version number, must be valid
		 * @return True, if succeeded
		 * @see readFromDataFile(), writeToDataFile().
		 */
		bool readFromDataFileV2(Ocean::IO::InputBitstream& bitstream);

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Analyzes the binary or extracts already analyzed information from a data file.
		 */
		void analyze();

		/**
		 * The disabled assign operator.
		 * @param binaryAnalyzer The binary analyzer object to be assigned
		 * @return Reference to this object
		 */
		BinaryAnalyzer& operator=(const BinaryAnalyzer& binaryAnalyzer) = delete;

	protected:

		/// The state of this analyzer.
		AnalyzerState analyzerState_ = AS_EMPTY;

		/// The binary file on which this analyzer works.
		std::string filenameBinary_;

		/// The path of the objdump tool user provides.
		std::string objdumpPath_;

		/// The strings (from segment "__TEXT", section "__cstring") that have been analyzed.
		TextStrings textStrings_;

		/// A simple lookup map allowing to find string ids based on the address of the string.
		Address2StringIdMap address2StringIdMap_;

		/// The symbols (from segment "__TEXT", section "__text") that have been analyzed, the indices of the symbols are used/interpreted as symbol id.
		Symbols symbols_;

		/// A simple lookup map allowing to find symbol ids based on the addresses of symbols.
		Address2SymbolIdMap address2SymbolIdMap_;

		/// The lock object of this analyzer.
		mutable Ocean::Lock lock_;
};

inline BinaryAnalyzer::Object::Object() :
	address_(0ull),
	size_(0u)
{
	// nothing to do here
}

inline BinaryAnalyzer::Object::Object(const Address64 address, const unsigned int size) :
	address_(address),
	size_(size)
{
	// nothing to do here
}

inline BinaryAnalyzer::Address64 BinaryAnalyzer::Object::address() const
{
	return address_;
}

inline unsigned int BinaryAnalyzer::Object::size() const
{
	return size_;
}

inline void BinaryAnalyzer::Object::setAddress(const Address64 address)
{
	ocean_assert(address_ == 0ull);
	ocean_assert(address != 0ull);

	address_ = address;
}

inline void BinaryAnalyzer::Object::setSize(const unsigned int size)
{
	ocean_assert(size_ == 0u);
	ocean_assert(size != 0u);

	size_ = size;
}

inline BinaryAnalyzer::TextString::TextString(const Address64 address, const unsigned int size, std::string&& value) :
	Object(address, size),
	value_(std::move(value))
{
	// nothing to do here
}

inline const std::string& BinaryAnalyzer::TextString::value() const
{
	return value_;
}

inline BinaryAnalyzer::Symbol::Symbol(const SymbolId id, std::string&& name, std::string&& readableName) :
	Object(),
	id_(id),
	name_(std::move(name)),
	readableName_(std::move(readableName))
{
	ocean_assert(!name_.empty());
}

inline BinaryAnalyzer::Symbol::Symbol(const SymbolId id, std::string&& name, std::string&& readableName, const Address64 address, const unsigned int size, SymbolIdSet&& parentSymbols, SymbolIdSet&& childSymbols, AddressSet64&& loadedAddresses) :
	Object(address, size),
	id_(id),
	name_(std::move(name)),
	readableName_(std::move(readableName)),
	parentSymbols_(std::move(parentSymbols)),
	childSymbols_(std::move(childSymbols)),
	loadedAddresses_(std::move(loadedAddresses))
{
	// nothing to do here
}

inline BinaryAnalyzer::SymbolId BinaryAnalyzer::Symbol::id() const
{
	return id_;
}

inline const std::string& BinaryAnalyzer::Symbol::name() const
{
	return name_;
}

inline const std::string& BinaryAnalyzer::Symbol::readableName() const
{
	if (readableName_.empty())
	{
		return name_;
	}
	else
	{
		return readableName_;
	}
}

inline const std::vector<std::string>& BinaryAnalyzer::Symbol::aliasNames() const
{
	ocean_assert(aliasNames_.size() == readableAliasNames_.size());

	return aliasNames_;
}

inline const std::vector<std::string>& BinaryAnalyzer::Symbol::readableAliasNames() const
{
	ocean_assert(aliasNames_.size() == readableAliasNames_.size());

	return readableAliasNames_;
}

inline const BinaryAnalyzer::SymbolIdSet& BinaryAnalyzer::Symbol::parentSymbols() const
{
	return parentSymbols_;
}

inline const BinaryAnalyzer::SymbolIdSet& BinaryAnalyzer::Symbol::childSymbols() const
{
	return childSymbols_;
}

inline const BinaryAnalyzer::AddressSet64& BinaryAnalyzer::Symbol::loadedAddressed() const
{
	return loadedAddresses_;
}

inline bool BinaryAnalyzer::Symbol::isRootSymbol() const
{
	return parentSymbols_.empty();
}

inline const BinaryAnalyzer::TextStrings& BinaryAnalyzer::textStrings() const
{
	return textStrings_;
}

inline size_t BinaryAnalyzer::symbolSize() const
{
	return symbols_.size();
}

inline const BinaryAnalyzer::Symbol& BinaryAnalyzer::symbol(const SymbolId symbolId) const
{
	ocean_assert(symbolId < symbols_.size());
	return symbols_[symbolId];
}

inline const BinaryAnalyzer::Symbols& BinaryAnalyzer::symbols() const
{
	return symbols_;
}

inline void BinaryAnalyzer::setTextStrings(TextStrings&& textStrings, Address2StringIdMap&& address2StringIdMap)
{
	ocean_assert(textStrings.size() == address2StringIdMap.size());

	textStrings_ = std::move(textStrings);
	address2StringIdMap_ = std::move(address2StringIdMap);
}

inline void BinaryAnalyzer::setSymbols(Symbols&& symbols, Address2SymbolIdMap&& address2SymbolIdMap)
{
	ocean_assert(symbols.size() == address2SymbolIdMap.size());

	symbols_ = std::move(symbols);
	address2SymbolIdMap_ = std::move(address2SymbolIdMap);
}

inline void BinaryAnalyzer::Symbol::addParentSymbol(const SymbolId& parentId)
{
	ocean_assert(parentId != invalidSymbolId);
	parentSymbols_.insert(parentId);
}

inline void BinaryAnalyzer::Symbol::addChildSymbol(const SymbolId& childId)
{
	ocean_assert(childId != invalidSymbolId);
	childSymbols_.insert(childId);
}

inline void BinaryAnalyzer::Symbol::addString(const StringId& stringId)
{
	ocean_assert(stringId != invalidStringId);
	strings_.insert(stringId);
}

inline void BinaryAnalyzer::Symbol::addLoadedAddress(const Address64& address)
{
	loadedAddresses_.insert(address);
}

inline BinaryAnalyzer::GraphNode::GraphNode(const SymbolId symbolId) :
	symbolId_(symbolId)
{
	// nothing to do here
}

inline BinaryAnalyzer::SymbolId BinaryAnalyzer::GraphNode::symbolId() const
{
	return symbolId_;
}

const inline BinaryAnalyzer::GraphNodeRefs& BinaryAnalyzer::GraphNode::childNodes() const
{
	return childNodes_;
}

template <bool tAscending>
inline BinaryAnalyzer::CompareSymbolNames<tAscending>::CompareSymbolNames(const BinaryAnalyzer::Symbols& symbols) :
	symbols_(symbols)
{
	// nothing to do here
}

template <bool tAscending>
inline bool BinaryAnalyzer::CompareSymbolNames<tAscending>::operator()(const BinaryAnalyzer::SymbolId firstId, const BinaryAnalyzer::SymbolId secondId) const
{
	ocean_assert(firstId < symbols_.size() && secondId < symbols_.size());

	if constexpr (tAscending)
	{
		return symbols_[firstId].readableName() < symbols_[secondId].readableName();
	}
	else
	{
		return symbols_[firstId].readableName() > symbols_[secondId].readableName();
	}
}

template <bool tAscending>
inline BinaryAnalyzer::CompareSymbolSizes<tAscending>::CompareSymbolSizes(const BinaryAnalyzer::Symbols& symbols) :
	symbols_(symbols)
{
	// nothing to do here
}

template <bool tAscending>
inline bool BinaryAnalyzer::CompareSymbolSizes<tAscending>::operator()(const BinaryAnalyzer::SymbolId firstId, const BinaryAnalyzer::SymbolId secondId) const
{
	ocean_assert(firstId < symbols_.size() && secondId < symbols_.size());

	if constexpr (tAscending)
	{
		return symbols_[firstId].size() < symbols_[secondId].size();
	}
	else
	{
		return symbols_[firstId].size() > symbols_[secondId].size();
	}
}

inline BinaryAnalyzer::AnalyzerState BinaryAnalyzer::state() const
{
	const Ocean::ScopedLock scopedLock(lock_);

	return analyzerState_;
}

inline bool BinaryAnalyzer::isState(const AnalyzerState state) const
{
	const Ocean::ScopedLock scopedLock(lock_);

	return analyzerState_ == state;
}

template <typename T>
inline void BinaryAnalyzer::sortSymboldsByReadableName(const Symbols& symbols, T& subsetSymbolIds, const bool ascending)
{
	if (ascending)
		std::sort(subsetSymbolIds.begin(), subsetSymbolIds.end(), CompareSymbolNames<true>(symbols));
	else
		std::sort(subsetSymbolIds.begin(), subsetSymbolIds.end(), CompareSymbolNames<false>(symbols));
}

template <typename T>
inline void BinaryAnalyzer::sortSymboldsBySize(const Symbols& symbols, T& subsetSymbolIds, const bool ascending)
{
	if (ascending)
		std::sort(subsetSymbolIds.begin(), subsetSymbolIds.end(), CompareSymbolSizes<true>(symbols));
	else
		std::sort(subsetSymbolIds.begin(), subsetSymbolIds.end(), CompareSymbolSizes<false>(symbols));
}

inline std::string BinaryAnalyzer::dataFileExtension()
{
	return std::string("asa");
}

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_BINARY_ANALYZER_H
