/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/Disassembler.h"

#include "ocean/base/TaskQueue.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

using namespace Ocean;

Disassembler::LineReader::LineReader(std::istream& inputStream) :
	inputStream_(inputStream),
	lineBufferDynamicSize_(0)
{
	// nothing to do here
}

const char* Disassembler::LineReader::readNextLine(size_t& size)
{
	size = 0;

	// we read an entire line (hopefully not longer than 1024 characters)
	inputStream_.getline(lineBufferStatic_, 1024);

	// we want to ensure that we never read more than 1024 characters, +1 for the end of string character
	ocean_assert(inputStream_.gcount() <= 1024);

	if (inputStream_.eof())
	{
		// we have reached the end of the file
		return nullptr;
	}

	if (inputStream_.fail())
	{
		// the line seems to be larger than 1024 characters (e.g., because of a large documentation)
		// thus we simply need to read the line by hand

		// we have to clear the error state of the stream
		inputStream_.clear();
		ocean_assert(!inputStream_.fail());

		ocean_assert(lineBufferDynamicSize_ == 0);

		while (true)
		{
			const size_t charactersRead = inputStream_.gcount();

			extendDynamicBuffer(lineBufferStatic_, charactersRead);

			if (charactersRead < 1023)
			{
				// we have reach the end of the line, or the end of the file

				if (!inputStream_.eof())
				{
					if (charactersRead == 0 && inputStream_.fail())
					{
						// the previous read call fitted exactly into our last buffer, so we have to clear the error state
						inputStream_.clear();
					}

					// in the case we reached the end of the line we have to remove the '\n' character from the stream
					const char endOfLineCharacter = inputStream_.get();
					ocean_assert(endOfLineCharacter == '\n');
				}

				break;
			}

			inputStream_.get(lineBufferStatic_, 1024, '\n');

			// we want to ensure that we never read more than 1024 characters, +1 for the end of string character
			ocean_assert(inputStream_.gcount() <= 1024);
		}

		size = lineBufferDynamicSize_;
		lineBufferDynamicSize_ = 0;

		return lineBufferDynamic_.data();
	}

	// the line we read fits into the static buffer with 1024 characters

	size = inputStream_.gcount() - size_t(1); // size not including the following '\0' character
	ocean_assert(lineBufferStatic_[size] == '\0');

	return lineBufferStatic_;
}

void Disassembler::invokeCommandAsynchron(const std::string command)
{
	if (system(command.c_str()) != 0)
	{
		Log::error() << "Asynchron command failed!";
	}
}

bool Disassembler::createDisassemblyFile(const std::string& tool, const std::string& filenameBinary, std::string& filenameDisassembly)
{
	const IO::Directory temporaryDirector(IO::Directory::createTemporaryDirectory());

	const IO::File fileBinary(filenameBinary);

	const IO::File temporaryOutputFile(temporaryDirector + IO::File("disassembly" + fileBinary.baseName() + ".txt"));


	// we create a pipe for our output file
	const std::string makePipeCommand = std::string("mkfifo ") + temporaryOutputFile();

	if (system(makePipeCommand.c_str()) != 0)
		return false;

	const std::string command = tool + " \"" + filenameBinary + "\" > \"" + temporaryOutputFile() + "\"";

	TaskQueue::get().pushTask(TaskQueue::Task::createStatic(invokeCommandAsynchron, command));

	filenameDisassembly = temporaryOutputFile();

	return true;
}

void Disassembler::resolveStaticLinksToNames(const SourceSymbolsTargetNames& notResolvedLinksToNames, const SymbolName2SymbolIdsMultiMap& symbolName2SymbolIdsMultiMap, BinaryAnalyzer::Symbols& symbols)
{
	unsigned long long sizeMultipleSymbols = 0ull;

	for (SourceSymbolsTargetNames::const_iterator iL = notResolvedLinksToNames.cbegin(); iL != notResolvedLinksToNames.cend(); ++iL)
	{
		const BinaryAnalyzer::SymbolId sourceSymbolId = iL->first;
		const std::string& targetSymbolName = iL->second;

		SymbolName2SymbolIdsMultiMap::const_iterator iN = symbolName2SymbolIdsMultiMap.find(targetSymbolName);
		if (iN == symbolName2SymbolIdsMultiMap.cend())
		{
			Log::warning() << "Could not resolve symbol \"" << targetSymbolName << "\"";
			continue;
		}

		const BinaryAnalyzer::SymbolId* symbolId = nullptr;
		while (iN != symbolName2SymbolIdsMultiMap.cend() && iN->first == targetSymbolName)
		{
			if (symbolId != nullptr)
			{
				Log::warning() << "We found a symbol name several times: \"" << targetSymbolName << "\" - we use the first one";

				const unsigned int sizeA = symbols[*symbolId].size();
				const unsigned int sizeB = symbols[iN->second].size();

				if (sizeA == sizeB)
				{
					Log::warning() << "Both symbols have the same binary size " << sizeA << " bytes";
				}
				else
				{
					Log::warning() << "Both symbols have individual binary sizes " << sizeA << "/" << sizeB << " bytes";
				}

				sizeMultipleSymbols += (unsigned long long)sizeB;

				break;
			}

			symbolId = &iN->second;

			++iN;
		}

		if (symbolId)
		{
			const BinaryAnalyzer::SymbolId targetSymbolId = *symbolId;

			ocean_assert(sourceSymbolId < symbols.size() && targetSymbolId < symbols.size());

			symbols[sourceSymbolId].addChildSymbol(targetSymbolId);
			symbols[targetSymbolId].addParentSymbol(sourceSymbolId);
		}
		else
		{
			// **TODO**
			ocean_assert(false && "Could not resolve address!");
		}
	}

	if (sizeMultipleSymbols != 0ull)
	{
		Log::info() << "We found symbols with identical names, they cover " << BinaryAnalyzer::sizeString(double(sizeMultipleSymbols));
	}
}

void Disassembler::resolveStaticLinksToAddresses(const SourceSymbolsTargetAddresses& notResolvedLinksToAddresses, BinaryAnalyzer::Symbols& symbols, BinaryAnalyzer::Address2SymbolIdMap& address2SymbolIdMap)
{
	for (SourceSymbolsTargetAddresses::const_iterator iL = notResolvedLinksToAddresses.cbegin(); iL != notResolvedLinksToAddresses.cend(); ++iL)
	{
		const BinaryAnalyzer::SymbolId sourceSymbolId = iL->first;
		const BinaryAnalyzer::Address64 targetAddress = iL->second;

		ocean_assert(sourceSymbolId < symbols.size());
		BinaryAnalyzer::Symbol& sourceSymbol = symbols[sourceSymbolId];

		const BinaryAnalyzer::Address2SymbolIdMap::const_iterator iA = address2SymbolIdMap.find(targetAddress);

		if (iA != address2SymbolIdMap.end())
		{
			const BinaryAnalyzer::SymbolId targetSymbolId = iA->second;

			sourceSymbol.addChildSymbol(targetSymbolId);

			ocean_assert(targetSymbolId < symbols.size());
			symbols[targetSymbolId].addParentSymbol(sourceSymbolId);
		}
		else
		{
			// we may have a branch within our own symbol
			if (targetAddress >= sourceSymbol.address() && targetAddress < sourceSymbol.address() + sourceSymbol.size())
			{
				// perhaps we can use this information for some visual output
			}
			else
			{
				// we have a branch to some function outside our segment ?!?

				const std::string add = String::toAStringHex(targetAddress);
				// **TODO**
			}
		}
	}
}

void Disassembler::addDynamicLinksFromLoadedAddresses(BinaryAnalyzer::Symbols& symbols, BinaryAnalyzer::Address2SymbolIdMap& address2SymbolIdMap)
{
	for (BinaryAnalyzer::Symbol& symbol : symbols)
	{
		const BinaryAnalyzer::AddressSet64& loadedAddresses = symbol.loadedAddressed();

		for (const BinaryAnalyzer::Address64& address : loadedAddresses)
		{
			const BinaryAnalyzer::Address2SymbolIdMap::const_iterator iSymbol = address2SymbolIdMap.find(address);

			// let's check whether we have a symbol at this address
			if (iSymbol != address2SymbolIdMap.end())
			{
				// we are not interested in this branch/link if the links points into the symbol itself (e.g., a jump within the function)
				if (iSymbol->second != symbol.id())
				{
					// so in this case we can add our symbol as parent to the symbol to which the address belongs
					// (and vice vesa)

					ocean_assert(iSymbol->second < symbols.size());

					symbols[iSymbol->second].addParentSymbol(symbol.id());
					symbol.addChildSymbol(iSymbol->second);
				}
			}
			else
			{
				// **TODO** what else can we do?
			}

			// do we need to handles addresses/strings?
			/*const BinaryAnalyzer::Address2StringIdMap::const_iterator iString = address2StringIdMap.find(address);

			if (iString != address2StringIdMap.end())
			{
				ocean_assert(iString->second < symbol.size());

				symbol.addString(iString->second);
			}*/
		}
	}
}
