/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/DisassemblerAndroid.h"

#include "ocean/base/Timestamp.h"

#include "ocean/io/Scanner.h"

using namespace Ocean;

bool DisassemblerAndroid::disassemble(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	if (!parseExecutableSections(objdumpPath, filename, binaryAnalyzer))
	{
		return false;
	}

	return true;
}

bool DisassemblerAndroid::parseExecutableSections(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	Log::info() << "Start parsing of all executable sections";

	std::string temporaryFilenameDisassembly;
	if (!createDisassemblyFile(objdumpPath, filename, temporaryFilenameDisassembly))
		return false;

	bool result = false;

	std::ifstream stream(temporaryFilenameDisassembly, std::ios::binary);

	if (stream.is_open())
	{
		result = parseExecutableSections(stream, binaryAnalyzer);
		stream.close();
	}

	// we remove the temporary file/pipe again
	const int deleteResult = remove(temporaryFilenameDisassembly.c_str());
	ocean_assert(deleteResult == 0);

	if (result)
	{
		Log::info() << "Parsing of all executable sections succeeded.";
	}
	else
	{
		Log::error() << "Parsing of all executable sections FAILED!";
	}

	return result;
}

bool DisassemblerAndroid::parseExecutableSections(std::istream& stream, BinaryAnalyzer& binaryAnalyzer)
{
	ocean_assert(stream.good());

	/*
	 * For now, we are interested in the .text section.
	 * A symbol within the .text section may look like this:
	 *
	 * Disassembly of section .text:
	 *
	 * 001a6b20 <_ZN14FeatureTrackerC1Ev-0x2fc4>:
	 *   1a6b20:	ldr	r0, [pc, #4]	; 1a6b2c <__gnu_ldivmod_helper@plt+0x20>
	 *   1a6b24:	add	r0, pc, r0
	 *   1a6b28:	b	1a5ad4 <__cxa_finalize@plt>
	 */

	bool textSectionFound = false;

	BinaryAnalyzer::Address64 previousAddressValue = 0ull;

	BinaryAnalyzer::SymbolId pendingAddresSymbolId = BinaryAnalyzer::invalidSymbolId;

	SourceSymbolsTargetNames notResolvedLinksToNames;
	SourceSymbolsTargetAddresses notResolvedLinksToAddresses;

	/// A simple lookup map allowing to find symbol ids based on symbol names (a multimap as some names may occur twice e.g., "___cxx_global_array_dtor")
	SymbolName2SymbolIdsMultiMap symbolName2SymbolIdsMultiMap;

	BinaryAnalyzer::Symbols symbols;
	symbols.reserve(2048);

	BinaryAnalyzer::Address2SymbolIdMap address2SymbolIdMap;

	const unsigned int instructionSize = 2u; // in ARMv7 each instruction has size 2 bytes ???

	/// values needed for parsing adrp/add combinations
	std::string addressPointerRegister;
	unsigned long long addressPointerIntermediate = 0ull;

	unsigned long long lineIndex = 0ull;

	size_t lineBufferSize = 0;
	const char* lineBuffer = nullptr;
	std::string lastLine;
	LineReader lineReader(stream);

	const Timestamp startTimestampDisassembly(true);

	size_t position = 0;
	size_t lastProgress = 0;

	while (true)
	{
		unsigned long long localAddressPointerIntermediate = 0ull;
		lastLine.assign(lineBuffer, lineBufferSize);
		lineBuffer = lineReader.readNextLine(lineBufferSize);

		if (lineBuffer == nullptr)
		{
			// we reached the end of the stream
			break;
		}

		++lineIndex;

		if (lineBuffer[0] == 'D' && lineBufferSize > 25 && lineBuffer[lineBufferSize - 1] == ':')
		{
			// we search for 'Disassembly of section .XXXX:'

			if (memcmp(lineBuffer, "Disassembly of section .", 24) == 0)
			{
				const std::string sectionName(lineBuffer + 24, lineBufferSize - 25);

				if (sectionName == "text")
				{
					ocean_assert(textSectionFound == false);

					textSectionFound = true;
					continue;
				}
				else
				{
					if (textSectionFound)
					{
						// we have seen the .text section already, so that we can stop now, as a new section is comming up
						break;
					}
				}
			}
		}

		if (textSectionFound == false)
		{
			// currently, we are not interested in non-text sections
			continue;
		}

		BinaryAnalyzer::Address64 addressValue = 0ull;

		// symbols are separated by an empty line (with few exceptions)
		// thus, whenever we reach an empty line and we have a pending symbol, we can "finalize" the symbol

		if (lineBufferSize == 0 && pendingAddresSymbolId != BinaryAnalyzer::invalidSymbolId)
		{
			if (lastLine.find("; unpredictable branch in IT block") != std::string::npos)
			{
				continue;
			}
			// now as we have the address of the symbol (from the line above),
			// we are also able to calculate the size of the symbol that lasts to the line above

			ocean_assert(pendingAddresSymbolId < symbols.size());
			BinaryAnalyzer::Symbol& pendingSymbol = symbols[pendingAddresSymbolId];

			ocean_assert(pendingSymbol.address() != 0ull);
			ocean_assert(pendingSymbol.size() == 0u);

			ocean_assert(previousAddressValue >= pendingSymbol.address()); // each symbol must contain at least one instruction
			const unsigned int size = (unsigned int)(previousAddressValue - pendingSymbol.address()) + instructionSize;

			pendingSymbol.setSize(size);

			pendingAddresSymbolId = BinaryAnalyzer::invalidSymbolId;
		}

		// symbols start with the address followed by " <SYMBOL_NAME>:" e.g.,
		// 001ab7a6 <_ZNK5Ocean18HomogenousMatrixT4IfE8invertedEv>:

		// we check whether we have a symbol (the name of the upcomming symbol)
		if (lineBufferSize >= 1 && ((lineBuffer[0] >= '0' && lineBuffer[0] <= '9') || (lineBuffer[0] >= 'a' && lineBuffer[0] <= 'f')))
		{
			if (lineBuffer[lineBufferSize - 1] != ':')
			{
				Log::warning() << "We seem to have a broken line in " << lineIndex << ", we expected a symbol, got \"" << lineBuffer << "\", we just skip it.";
				continue;
			}

			// we determine the address of the symbol

			size_t tokenStart, tokenLength = 0;

			if (IO::Scanner::findNextToken(lineBuffer, 0, tokenStart, tokenLength) == false || tokenStart != 0)
			{
				Log::warning() << "We seem to have a broken line in " << lineIndex << ", we expected a symbol, got \"" << lineBuffer << "\", we just skip it.";
				continue;
			}

			if (String::isHexValue64(lineBuffer, tokenStart + tokenLength, false, &addressValue) == false)
			{
				Log::warning() << "We could not parse the address of a symbol in line " << lineIndex << ", we expected a symbol, got \"" << lineBuffer << "\", we just skip it.";
				continue;
			}

			IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

			if (lineBuffer[tokenStart] != '<' || lineBuffer[lineBufferSize - 2] != '>' || lineBuffer[lineBufferSize - 1] != ':')
			{
				Log::warning() << "We could not parse the a symbol in line " << lineIndex << ", we expected a symbol, got \"" << lineBuffer << "\", we just skip it.";
				continue;
			}

			const char* symbolStringPointer = lineBuffer + tokenStart + 1;
			const size_t symbolStringSize = lineBufferSize - tokenStart - 3; // 3 due to: <SYMBOL_NAME>:

			// we store the symbol in the database

			const BinaryAnalyzer::SymbolId newSymbolId = BinaryAnalyzer::SymbolId(symbols.size());
			ocean_assert(newSymbolId != BinaryAnalyzer::invalidSymbolId);

			std::string symbolName(symbolStringPointer, symbolStringSize);

			symbols.emplace_back(newSymbolId, std::move(symbolName));
			BinaryAnalyzer::Symbol& currentSymbol = symbols.back();

			currentSymbol.setAddress(addressValue);

			ocean_assert(address2SymbolIdMap.find(addressValue) == address2SymbolIdMap.end());
			address2SymbolIdMap.insert(std::make_pair(addressValue, currentSymbol.id()));

			// we may see a symbol several times e.g., "___cxx_global_array_dtor"
			symbolName2SymbolIdsMultiMap.insert(std::make_pair(std::string(symbolStringPointer, symbolStringSize), newSymbolId));

			pendingAddresSymbolId = currentSymbol.id();
		}
		// we check whether we have an address line
		else if (lineBufferSize >= 2)
		{
			if (symbols.empty())
			{
				// we have our first address line but haven't seen a symbol yet
				// thus, we seem to have a binary with stripped symbols, we better stop here

				Log::error() << "The binary seems to come without symbol names (or at least some symbol names may be removed already), we stop here...";
				return false;
			}

			size_t tokenStart, tokenLength = 0;

			// now we can parse the address line for specific instructions

			if (IO::Scanner::findNextToken(lineBuffer, 0, tokenStart, tokenLength) && lineBuffer[tokenStart + tokenLength - 1] == ':' && String::isHexValue64(lineBuffer + tokenStart, tokenLength - 1, false, &addressValue))
			{
				ocean_assert(pendingAddresSymbolId != BinaryAnalyzer::invalidSymbolId);

				if (IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength))
				{
					if (isBranchInstruction(lineBuffer + tokenStart, tokenLength))
					{
						// we have a branch instruction, so we now seek for the target address or target symbol
						// so we are parsing a line like this:
						//   1a6e8c:	blt	ff063838 <_end+0xfeb58903>

						IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

						if (tokenLength >= 4) // for now we want to avoid parsing registers
						{
							BinaryAnalyzer::Address64 linkAddress = 0ull;
							if (String::isHexValue64(lineBuffer + tokenStart, tokenLength, false, &linkAddress))
							{
								// we have a link with hex address
								notResolvedLinksToAddresses.emplace_back(std::make_pair(pendingAddresSymbolId, linkAddress));
							}
						}
					}
				}
			}
			else
			{
				// **TODO** store additional lines to check whether we have to parse them
			}
		}

		addressPointerIntermediate = localAddressPointerIntermediate;
		previousAddressValue = addressValue;

		// let's calculate how much bytes we have processed already

		position += lineBufferSize;

		const size_t progress = position / (1024 * 1024 * 10);

		if (progress != lastProgress)
		{
			lastProgress = progress;

			Log::info() << "Progress: " << progress * 10 << "MB";
		}
	}

	if (lineIndex == 0ull)
	{
		// we could not parse one file, something went wrong
		return false;
	}

	Log::info() << "Parsed/disassembled the section in " << String::toAString(double(Timestamp(true) - startTimestampDisassembly), 2u) << " seconds";

	const Timestamp startTimestampSymbolStructure(true);

	if (pendingAddresSymbolId != BinaryAnalyzer::invalidSymbolId)
	{
		// we have to set the size of the last symbol (which lasts to the current including address)

		ocean_assert(pendingAddresSymbolId < symbols.size());
		BinaryAnalyzer::Symbol& pendingSymbol = symbols[pendingAddresSymbolId];

		ocean_assert(pendingSymbol.address() != 0ull);
		ocean_assert(pendingSymbol.size() == 0u);

		ocean_assert(previousAddressValue >= pendingSymbol.address());
		const unsigned int size = (unsigned int)(previousAddressValue - pendingSymbol.address()) + instructionSize;

		pendingSymbol.setSize(size);
	}

#ifdef OCEAN_DEBUG
	// we check the consistency
	ocean_assert(symbols.size() == symbolName2SymbolIdsMultiMap.size());
	ocean_assert(symbols.size() == address2SymbolIdMap.size());

	for (size_t n = 0; n < symbols.size(); ++n)
	{
		const BinaryAnalyzer::SymbolId symbolId = BinaryAnalyzer::SymbolId(n);
		const BinaryAnalyzer::Symbol& symbol = symbols[n];

		const BinaryAnalyzer::Address2SymbolIdMap::const_iterator iA = address2SymbolIdMap.find(symbol.address());
		ocean_assert(iA != address2SymbolIdMap.cend());

		ocean_assert(symbolId == iA->second);
	}
#endif

	// now we resolve the links we gathered above, we start with links to symbol names
	resolveStaticLinksToNames(notResolvedLinksToNames, symbolName2SymbolIdsMultiMap, symbols);

	// next, we resolve links to addresses
	resolveStaticLinksToAddresses(notResolvedLinksToAddresses, symbols, address2SymbolIdMap);

	// and we have to check whether a loaded address falls outside a symbol
	// a loaded address is generated e.g., for a call to a function pointer; thus, a dynamic branch/link
	addDynamicLinksFromLoadedAddresses(symbols, address2SymbolIdMap);

	Log::info() << "Created the symbol structure in " << String::toAString(double(Timestamp(true) - startTimestampSymbolStructure), 2u) << " seconds";

	binaryAnalyzer.setSymbols(std::move(symbols), std::move(address2SymbolIdMap));

	return true;
}

bool DisassemblerAndroid::createDisassemblyFile(const std::string& objdumpPath, const std::string& filenameBinary, std::string& filenameDisassembly)
{
	// command line parameters:
	//
	// -d:                    NDK's objdump does not allow to specify a specific segment/section so that we have to parse the all "executable sections"
	// --no-show-raw-insn:    Hide raw/hex code, we just concentrate on symbolic disassembly
	// --wide:                Format output for more than 80 columns
	// --disassemble-zeroes:  Do not skip blocks of zeroes when disassembling
	// --demangle=:           Decode mangled/processed symbol names

	const std::string tool = objdumpPath + " -d --no-show-raw-insn --wide --disassemble-zeroes --demangle=auto";

	return Disassembler::createDisassemblyFile(tool, filenameBinary, filenameDisassembly);
}
