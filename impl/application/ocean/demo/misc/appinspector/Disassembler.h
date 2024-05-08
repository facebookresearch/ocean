/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_H

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"

#include "ocean/base/Base.h"

/**
 * This class implements a disassembler for binary files.
 * @ingroup applicationdemomiscappsizeanalyzerosx
 */
class Disassembler
{
	protected:

		/**
		 * This class implements a simple reader for lines using a stream as data input.
		 */
		class LineReader
		{
			public:

				/**
				 * Creates a new line reader object based on an input stream.
				 * @param inputStream The input stream that provides the data to be read
				 */
				LineReader(std::istream& inputStream);

				/**
				 * Disabled copy constructor.
				 * @param lineReader The line reader to be copied
				 */
				LineReader(const LineReader& lineReader) = delete;

				/**
				 * Reads the next line from the data input and provides the buffer to the entire line.
				 * Each call a new line is read, while the buffer from the previous call will be reused.<br>
				 * The buffer is managed internally; thus, the caller must not free the buffer.
				 * @param size Resulting number of characters the line has, 0 if no further line could be read
				 * @return The pointer to the buffer providing the line, nullptr if no further line could be read
				 */
				const char* readNextLine(size_t& size);

			protected:

				/**
				 * Adds a given buffer to the end of the internal dynamic line buffer.
				 * @param buffer The buffer to be added, may be nullptr if 'size' is 0
				 * @param size The size of the buffer to be added, in characters, with range [0, infinity)
				 */
				inline void extendDynamicBuffer(const char* buffer, const size_t size);

				/**
				 * Disabled assign operator.
				 * @param lineReader The line reader object to be assigned
				 * @return The reference to this object
				 */
				LineReader& operator=(const LineReader& lineReader) = delete;

			protected:

				/// The input stream providing the input data.
				std::istream& inputStream_;

				/// The internal static line buffer, for lines with normal length.
				char lineBufferStatic_[1025];

				/// The internal dynamic line buffer, for lines with extreme size.
				std::vector<char> lineBufferDynamic_;

				/// The number of valid characters the dynamic line buffer stores (not the capacity of the dynamic buffer)s.
				size_t lineBufferDynamicSize_;
		};

		/**
		 * Definition of a vector holding pairs of source symbol ids and target symbol names.
		 */
		typedef std::vector<std::pair<BinaryAnalyzer::SymbolId, std::string>> SourceSymbolsTargetNames;

		/**
		 * Definition of a map mapping symbol ids to addresses.
		 */
		typedef std::vector<std::pair<BinaryAnalyzer::SymbolId, BinaryAnalyzer::Address64>> SourceSymbolsTargetAddresses;

		/**
		 * Definition of an unordered map mapping symbol names to symbol ids.
		 */
		typedef std::unordered_multimap<std::string, BinaryAnalyzer::SymbolId> SymbolName2SymbolIdsMultiMap;

	protected:

		/**
		 * The disabled constructor.
		 */
		Disassembler() = delete;

		/**
		 * The disabled copy constructor.
		 * @param disassembler The disassembler object to be copied
		 */
		Disassembler(const Disassembler& disassembler) = delete;

		/**
		 * The disabled assign operator.
		 * @param disassembler The disassembler object to be assigned
		 * @return Reference to this object
		 */
		Disassembler& operator=(const Disassembler& disassembler) = delete;

		/**
		 * Invokes a command asynchronously.
		 * @param command The command to be invoked asynchronously
		 */
		static void invokeCommandAsynchron(const std::string command);

		/**
		 * Creates a disassembly file by using a specified tool and by specifying the binary to analyze - actually a pipe.
		 * This function will apply the disassembly in a separate thread while the output will be written to a file.<br>
		 * The function returns immediately.
		 * @param tool The tool to be used to disassembly the binary file (including any optional command line parameters)
		 * @param filenameBinary The filename of the binary to be analyzed
		 * @param filenameDisassembly The resulting filename of the resulting disassembly file, actually the name of a pipe
		 * @return True, if succeeded
		 */
		static bool createDisassemblyFile(const std::string& tool, const std::string& filenameBinary, std::string& filenameDisassembly);

		/**
		 * Resolves static links to symbols which the target symbol is known by name.
		 * This function is intended as post-processing function, once all symbols have been parsed.
		 * @param notResolvedLinksToNames All names to symbols that haven't been resolved during parsing already
		 * @param symbolName2SymbolIdsMultiMap The multi-map which has been created during parsing that maps symbol names to ids of symbols
		 * @param symbols The symbols for which the links will be resolved
		 * @see resolveStaticLinksToAddresses(), addDynamicLinksFromLoadedAddresses().
		 */
		static void resolveStaticLinksToNames(const SourceSymbolsTargetNames& notResolvedLinksToNames, const SymbolName2SymbolIdsMultiMap& symbolName2SymbolIdsMultiMap, BinaryAnalyzer::Symbols& symbols);

		/**
		 * Resolves static links to symbols which the target symbol is known by address.
		 * This function is intended as post-processing function, once all symbols have been parsed.
		 * @param notResolvedLinksToAddresses All pairs of source symbol and target addresses that haven't been resolved during parsing
		 * @param symbols The symbols for which the links will be resolved
		 * @param address2SymbolIdMap The map mapping address values to symbol ids
		 * @see resolveStaticLinksToNames(), addDynamicLinksFromLoadedAddresses().
		 */
		static void resolveStaticLinksToAddresses(const SourceSymbolsTargetAddresses& notResolvedLinksToAddresses, BinaryAnalyzer::Symbols& symbols, BinaryAnalyzer::Address2SymbolIdMap& address2SymbolIdMap);

		/**
		 * Adds dynamic links to symbols in which addresses to other symbols are loaded dynamically.
		 * This function is intended as post-processing function, once all symbols have been parsed.
		 * @param symbols The symbols for which the links will be resolved
		 * @param address2SymbolIdMap The map mapping address values to symbol ids
		 * @see resolveStaticLinksToNames(), resolveStaticLinksToAddresses().
		 */
		static void addDynamicLinksFromLoadedAddresses(BinaryAnalyzer::Symbols& symbols, BinaryAnalyzer::Address2SymbolIdMap& address2SymbolIdMap);
};

inline void Disassembler::LineReader::extendDynamicBuffer(const char* buffer, const size_t size)
{
	ocean_assert(buffer != nullptr || size == 0);

	if (size == 0)
	{
		return;
	}

	if (lineBufferDynamicSize_ + size > lineBufferDynamic_.size())
	{
		// we need to extend the dynamic buffer
		lineBufferDynamic_.resize(lineBufferDynamic_.size() + 1024 * 64);
	}

	memcpy(lineBufferDynamic_.data() + lineBufferDynamicSize_, buffer, size);
	lineBufferDynamicSize_ += size;
}

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_H
