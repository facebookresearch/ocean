/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/DisassemblerIOS.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Scanner.h"

#include <cxxabi.h>

using namespace Ocean;

bool DisassemblerIOS::disassemble(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	if (!parseTextString(objdumpPath, filename, binaryAnalyzer))
	{
		return false;
	}

	if (!parseRodataString(objdumpPath, filename, binaryAnalyzer))
	{
		return false;
	}

	if (!parseTextText(objdumpPath, filename, binaryAnalyzer))
	{
		return false;
	}

	return true;
}

std::string DisassemblerIOS::demangleSymbol(const std::string& mangledSymbol)
{
	if (mangledSymbol.size() <= 1)
	{
		return std::string();
	}

	int status = 0;
	char* demangledName = abi::__cxa_demangle(mangledSymbol.c_str() + 1, 0, 0, &status);

	if (status == 0 && demangledName != nullptr)
	{
		std::string result(demangledName);
		free(demangledName);

		return result;
	}

	if (demangledName != nullptr)
	{
		free(demangledName);
	}

	return mangledSymbol;
}

bool DisassemblerIOS::parseTextString(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	Log::info() << "Start parsing of __TEXT, __cstring";

	std::string temporaryFilenameDisassembly;
	if (!createDisassemblyFile(objdumpPath, filename, "__TEXT", "__cstring", temporaryFilenameDisassembly))
		return false;

	bool result = false;

	std::ifstream stream(temporaryFilenameDisassembly, std::ios::binary);

	if (stream.is_open())
	{
		result = parseString(stream, binaryAnalyzer);
		stream.close();
	}

	// we remove the temporary file/pipe again
	const int deleteResult = remove(temporaryFilenameDisassembly.c_str());
	ocean_assert(deleteResult == 0);

	if (result)
	{
		Log::info() << "Parsing of __TEXT, __cstring succeeded.";
	}
	else
	{
		Log::error() << "Parsing of __TEXT, __cstring FAILED!";
	}

	return result;
}

bool DisassemblerIOS::parseRodataString(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	// RODATA contains read-only data

	Log::info() << "Start parsing of __RODATA, __cstring";

	std::string temporaryFilenameDisassembly;
	if (!createDisassemblyFile(objdumpPath, filename, "__RODATA", "__cstring", temporaryFilenameDisassembly))
		return false;

	bool result = false;

	std::ifstream stream(temporaryFilenameDisassembly, std::ios::binary);

	if (stream.is_open())
	{
		result = parseString(stream, binaryAnalyzer);
		stream.close();
	}

	// we remove the temporary file/pipe again
	const int deleteResult = remove(temporaryFilenameDisassembly.c_str());
	ocean_assert(deleteResult == 0);

	if (result)
	{
		Log::info() << "Parsing of __RODATA, __cstring succeeded.";
	}
	else
	{
		Log::error() << "Parsing of __RODATA, __cstring FAILED!";
	}

	return result;
}

bool DisassemblerIOS::parseString(std::istream& stream, BinaryAnalyzer& binaryAnalyzer)
{
	BinaryAnalyzer::TextStrings textStrings;
	textStrings.reserve(2048);

	BinaryAnalyzer::Address2StringIdMap address2StringIdMap;

	BinaryAnalyzer::Address64 addressValue = 0ull;

	size_t lineBufferSize = 0;
	const char* lineBuffer = nullptr;
	LineReader lineReader(stream);

	const Timestamp startTimestampDisassembly(true);

	while (true)
	{
		lineBuffer = lineReader.readNextLine(lineBufferSize);

		if (lineBuffer == nullptr)
		{
			// we reached the end of the stream
			break;
		}

		// we check whether we have an address line
		else if (lineBufferSize >= 18 && lineBuffer[0] == '0' && IO::Scanner::isWhitespace(lineBuffer[16]) && String::isHexValue64(lineBuffer, 16, false, &addressValue))
		{
			// now we can parse the address line for specific instructions

			size_t tokenStart, tokenLength = 0;

			IO::Scanner::findNextToken(lineBuffer, 16, tokenStart, tokenLength);

			ocean_assert(address2StringIdMap.find(addressValue) == address2StringIdMap.end());
			address2StringIdMap.insert(std::make_pair(addressValue, BinaryAnalyzer::StringId(textStrings.size())));

			textStrings.emplace_back(addressValue, tokenLength, std::string(lineBuffer + tokenStart, tokenLength));
		}
	}

	Log::info() << "Parsed/disassembled the section in " << String::toAString(double(startTimestampDisassembly), 2u) << " seconds";

	binaryAnalyzer.setTextStrings(std::move(textStrings), std::move(address2StringIdMap));

	return true;
}

bool DisassemblerIOS::parseTextText(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer)
{
	Log::info() << "Start parsing of __TEXT, __text";

	std::string temporaryFilenameDisassembly;
	if (!createDisassemblyFile(objdumpPath, filename, "__TEXT", "__text", temporaryFilenameDisassembly))
		return false;

	bool result = false;

	std::ifstream stream(temporaryFilenameDisassembly, std::ios::binary);

	if (stream.is_open())
	{
		result = parseTextText(stream, binaryAnalyzer);
		stream.close();
	}

	// we remove the temporary file/pipe again
	const int deleteResult = remove(temporaryFilenameDisassembly.c_str());
	ocean_assert(deleteResult == 0);

	if (result)
	{
		Log::info() << "Parsing of __TEXT, __text succeeded.";
	}
	else
	{
		Log::error() << "Parsing of __TEXT, __text FAILED!";
	}

	return result;
}

bool DisassemblerIOS::parseTextText(std::istream& stream, BinaryAnalyzer& binaryAnalyzer)
{
	BinaryAnalyzer::Address64 addressValue = 0ull;

	BinaryAnalyzer::SymbolId previousLineWasSymbolId = BinaryAnalyzer::invalidSymbolId;
	BinaryAnalyzer::SymbolId pendingAddresSymbolId = BinaryAnalyzer::invalidSymbolId;

	SourceSymbolsTargetNames notResolvedLinksToNames;
	SourceSymbolsTargetAddresses notResolvedLinksToAddresses;

	/// A simple lookup map allowing to find symbol ids based on symbol names (a multimap as some names may occur twice e.g., "___cxx_global_array_dtor")
	SymbolName2SymbolIdsMultiMap symbolName2SymbolIdsMultiMap;

	BinaryAnalyzer::Symbols symbols;
	symbols.reserve(2048);

	BinaryAnalyzer::Address2SymbolIdMap address2SymbolIdMap;

	/// values needed for parsing adrp/add combinations
	std::string addressPointerRegister;
	unsigned long long addressPointerIntermediate = 0ull;

	unsigned long long lineIndex = 0ull;

	size_t lineBufferSize = 0;
	const char* lineBuffer = nullptr;
	LineReader lineReader(stream);

	const Timestamp startTimestampDisassembly(true);

	size_t position = 0;
	size_t lastProgress = 0;

	while (true)
	{
		unsigned long long localAddressPointerIntermediate = 0ull;

		lineBuffer = lineReader.readNextLine(lineBufferSize);

		if (lineBuffer == nullptr)
		{
			// we reached the end of the stream
			break;
		}

		++lineIndex;

		// we check whether we have a symbol (the name of the upcomming symbol)
		if (lineBuffer[0] == '_' || lineBuffer[0] == '-' || lineBuffer[0] == '+') // TODO -/+ for Apple's Objc - any problems with Android?
		{
			// we determine addresses of symbols by the following pattern:

			// ...
			// 00000001001cd254 last cpu instruction of previous symbol
			// _NEXT_CPP_SYMBOL_NAME:      or      -NEXT_OBJC_SYMBOL_NAME:
			// 00000001001cd258 first cpu intstruction of next symbol (and addressof the symbol)
			// ...

			if (lineBuffer[lineBufferSize - 1] != ':')
			{
				Log::warning() << "We seem to have a broken line in " << lineIndex << ", got \"" << lineBuffer << "\", we just skip it.";
				continue;
			}

			std::string symbolName(lineBuffer, lineBufferSize - 1);
			std::string demangledSymbolName(demangleSymbol(symbolName));

			if (previousLineWasSymbolId != BinaryAnalyzer::invalidSymbolId)
			{
				// we have an alias symbol name

				// ...
				// 00000001001cd254 last cpu instruction of previous symbol
				// _NEXT_CPP_SYMBOL_NAME_A:
				// _NEXT_CPP_SYMBOL_NAME_B:
				// 00000001001cd258 first cpu intstruction of next symbol (and addressof the symbol)
				// ...

				ocean_assert(symbols.size() > previousLineWasSymbolId);

				symbols[previousLineWasSymbolId].aliasNames_.push_back(std::move(symbolName));
				symbols[previousLineWasSymbolId].readableAliasNames_.push_back(std::move(demangledSymbolName));

				continue;
			}

			// we store the symbol in the database

			const BinaryAnalyzer::SymbolId newSymbolId = BinaryAnalyzer::SymbolId(symbols.size());
			ocean_assert(newSymbolId != BinaryAnalyzer::invalidSymbolId);

			symbols.emplace_back(newSymbolId, std::move(symbolName), std::move(demangledSymbolName));

			// we may see a symbol several times e.g., "___cxx_global_array_dtor"
			symbolName2SymbolIdsMultiMap.insert(std::make_pair(std::string(lineBuffer, lineBufferSize - 1), newSymbolId));

			previousLineWasSymbolId = newSymbolId;
		}
		// we check whether we have an address line
		else if (lineBufferSize >= 18 && lineBuffer[0] == '0' && IO::Scanner::isWhitespace(lineBuffer[16]) && String::isHexValue64(lineBuffer, 16, false, &addressValue))
		{
			if (symbols.empty())
			{
				// we have our first address line but haven't seen a symbol yet
				// thus, we seem to have a binary with stripped symbols, we better stop here

				Log::error() << "The binary seems to come without symbol names (or at least some symbol names may be removed already), we stop here...";
				return false;
			}

			BinaryAnalyzer::Symbol& currentSymbol = symbols.back();

			if (previousLineWasSymbolId != BinaryAnalyzer::invalidSymbolId)
			{
				// the previous line was a line providing the name of the upcomming symbol
				// so we now have the corresponding address for the symbol

				ocean_assert(previousLineWasSymbolId == symbols.size() - 1);

				ocean_assert(!currentSymbol.name().empty());
				ocean_assert(currentSymbol.address() == 0ull);
				currentSymbol.setAddress(addressValue);

				ocean_assert(address2SymbolIdMap.find(addressValue) == address2SymbolIdMap.end());
				address2SymbolIdMap.insert(std::make_pair(addressValue, previousLineWasSymbolId));

				if (pendingAddresSymbolId != BinaryAnalyzer::invalidSymbolId)
				{
					// now as we have the address of the symbol (from the line above),
					// we are also able to calculate the size of the symbol that lasts to the line above

					ocean_assert(pendingAddresSymbolId < symbols.size());
					BinaryAnalyzer::Symbol& previousSymbol = symbols[pendingAddresSymbolId];

					ocean_assert(previousSymbol.address() != 0ull);
					ocean_assert(previousSymbol.size() == 0u);

					ocean_assert(addressValue > previousSymbol.address());
					const unsigned int size = (unsigned int)(addressValue - previousSymbol.address());

					previousSymbol.setSize(size);
				}

				pendingAddresSymbolId = previousLineWasSymbolId;
				previousLineWasSymbolId = BinaryAnalyzer::invalidSymbolId;
			}

			ocean_assert(pendingAddresSymbolId != BinaryAnalyzer::invalidSymbolId);

			// now we can parse the address line for specific instructions

			size_t tokenStart, tokenLength = 0;

			if (IO::Scanner::findNextToken(lineBuffer, 16, tokenStart, tokenLength))
			{
				if (isBranchInstruction(lineBuffer + tokenStart, tokenLength))
				{
					// we have a branch instruction, so we now seek for the target address or target symbol
					// so we are parsing a line like this:
					// 00000001000081b4	bl	__ZN5Ocean8Geometry3P3P5posesIdEEjPKNS_8VectorT3IT_EES7_PNS_18HomogenousMatrixT4IS4_EE
					// 00000001000081b4	bl	"__ZZ30-[FBHttpLowerStack configure:]EN3$_1C1ERKS_"

					bool pendingToken = IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

					if (tokenLength >= 2)
					{
						BinaryAnalyzer::Address64 linkAddress;
						if (lineBuffer[tokenStart] == '0' && lineBuffer[tokenStart + 1] == 'x' && tokenLength <= 18 && String::isHexValue64(lineBuffer + tokenStart, tokenLength, true, &linkAddress))
						{
							// we have a link with hex address
							notResolvedLinksToAddresses.emplace_back(std::make_pair(pendingAddresSymbolId, linkAddress));
						}
						else if (lineBuffer[tokenStart] == '"')
						{
							// the name of the symbol seems to contain white character so that we have to find the end of the string

							std::string symbolName = std::string(lineBuffer + tokenStart, tokenLength);
							size_t previousTokenEnd = tokenStart + tokenLength;

							while (pendingToken && symbolName.back() != '"')
							{
								pendingToken = IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

								if (tokenLength > 0)
								{
									ocean_assert(tokenStart > previousTokenEnd);
									symbolName.append(tokenStart - previousTokenEnd, ' ');

									symbolName.append(lineBuffer + tokenStart, tokenLength);

									previousTokenEnd = tokenStart + tokenLength;
								}
							}

							symbolName = (symbolName.back() == '"') ? symbolName.substr(1, symbolName.size() - 2) : symbolName.substr(1);

							notResolvedLinksToNames.emplace_back(std::make_pair(pendingAddresSymbolId, std::move(symbolName)));
						}
						else
						{
							// we have a normal symbol name
							notResolvedLinksToNames.emplace_back(std::make_pair(pendingAddresSymbolId, std::string(lineBuffer + tokenStart, tokenLength)));
						}
					}
				}
				else if (isAddressInstruction(lineBuffer + tokenStart, tokenLength))
				{
					// we are loading data, so let's store the address so that we can resolve the address later
					// so we are parsing a line like this (adr/adrl specifies relative addresses):
					// 0000000100008504	adr	x1, #0x8a9c
					// 0000000100008506	adr	x3, #-0x8a9c

					if (IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength) && tokenLength > 0 && lineBuffer[tokenStart + tokenLength - 1] == ',')
					{
						IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

						BinaryAnalyzer::Address64 loadedRelativeAddress = BinaryAnalyzer::Address64(0);
						if (tokenLength > 3 && lineBuffer[tokenStart] == '#' && String::isHexValue64(lineBuffer + tokenStart + 1, tokenLength - 1, true, &loadedRelativeAddress))
						{
							// we have a positive relative address
							currentSymbol.addLoadedAddress(addressValue + loadedRelativeAddress);
						}
						else if (tokenLength > 4 && lineBuffer[tokenStart] == '#' && lineBuffer[tokenStart + 1] == '-' && String::isHexValue64(lineBuffer + tokenStart + 2, tokenLength - 2, true, &loadedRelativeAddress))
						{
							// we have a negative relative address
							ocean_assert(addressValue >= loadedRelativeAddress);
							currentSymbol.addLoadedAddress(addressValue - loadedRelativeAddress);
						}
						else
						{
							ocean_assert(false && "Can this case happen?");
						}
					}
					else
					{
						ocean_assert(false && "Can this case happen?");
					}
				}
				else if (isAddressPointerInstruction(lineBuffer + tokenStart, tokenLength))
				{
					// we are loading data (first a 4K aligned address which will be refined in the second call)
					// so we are parsing a line like this (adrp defines a 4K offset relative to the 4K-aligned CP location):
					// 0000000100008380	adrp	x8, 198 ; 0x100008000

					if (IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength) && tokenLength > 0 && lineBuffer[tokenStart + tokenLength - 1] == ',')
					{
						IO::Scanner::findNextToken(lineBuffer, tokenStart + tokenLength, tokenStart, tokenLength);

						addressPointerRegister = std::string(lineBuffer + tokenStart, tokenLength);

						int intValue = 0;
						if (tokenLength >=1 && String::isInteger32(std::string(lineBuffer + tokenStart, tokenLength), &intValue))
						{
							// current (CP / 4K) * 4K + addressOffset * 4K + 'pending offset in next add instruction'

							localAddressPointerIntermediate = (unsigned long long)((((long long)addressValue) / 0x1000ll) * 0x1000ll + (long long)intValue * 0x1000ll);
						}
						else
						{
							ocean_assert(false && "Can this case happen?");
						}
					}
					else
					{
						ocean_assert(false && "Can this case happen?");
					}
				}
				/*else if (addressPointerIntermediate != 0ull && isAddInstruction(lineBuffer + tokenStart, tokenLength))
				{
					**TODO**
				}*/
			}
		}
		else
		{
			// **TODO** store additional lines to check whether we have to parse them
		}

		addressPointerIntermediate = localAddressPointerIntermediate;

		// let's calculate how much bytes we have processed already

		position += lineBufferSize;

		const size_t progress = position / (1024 * 1024 * 10);

		if (progress != lastProgress)
		{
			lastProgress = progress;

			Log::info() << "Progress: " << progress * 10 << "MB";
		}
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

		ocean_assert(addressValue > pendingSymbol.address());
		const unsigned int size = (unsigned int)(addressValue - pendingSymbol.address()) + 4u; // +4: the address is including, in ARM64 each instruction has size 4

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

bool DisassemblerIOS::createDisassemblyFile(const std::string& objdumpPath, const std::string& filenameBinary, const std::string& segment, const std::string& section, std::string& filenameDisassembly)
{
	// otool will be deprecated, objdump will replace otool, thus we use objdump

	// here is the otool call for e.g., __TEST __text
	// const std::string tool = "otool -v -V -s " + segment + " " + section;

	// here the corresponding objdump call:
	// --macho:                 Use MachO specific object file parser
	// --no-symbolic-operands:  do not symbolic operands when disassembling (we do not want to have e.g., target symbols by name, as we may have several targets with same name)
	// --print-imm-hex:         Use hex format for immediate values (simplifies the parsing)
	// --full-leading-addr:     Print full leading address (simplifies the parsing)
	// --no-show-raw-insn:      When disassembling instructions, do not print the instruction bytes.
	// --arch=arm64:            We handle arm64 binaries
	// --section                We disassemble a specific section only
	const std::string tool = objdumpPath + " --macho --no-symbolic-operands --print-imm-hex --full-leading-addr --no-show-raw-insn --arch=arm64 --section " + segment + "," + section;

	return Disassembler::createDisassemblyFile(tool, filenameBinary, filenameDisassembly);
}
