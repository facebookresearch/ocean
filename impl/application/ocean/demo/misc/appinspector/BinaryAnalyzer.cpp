/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"
#include "application/ocean/demo/misc/appinspector/DisassemblerAndroid.h"
#include "application/ocean/demo/misc/appinspector/DisassemblerIOS.h"

#include "ocean/base/Timestamp.h"

#include "ocean/io/File.h"

using namespace Ocean;

inline const std::string getDefaultObjdumpPath(const std::string& filenameBinary)
{
	if (IO::File(filenameBinary).extension() == "so") // Android
	{
		return "/opt/android_ndk/r17fb2/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-objdump";
	}
	else
	{
		return "objdump";
	}
}

bool BinaryAnalyzer::analyzeBinarySynchron(const std::string& filenameBinary, const std::string& objdumpPath)
{
	const ScopedLock scopedLock(lock_);

	if (analyzerState_ == AS_WORKING)
	{
		return false;
	}

	objdumpPath_ = objdumpPath;
	filenameBinary_ = filenameBinary;

	analyze();

	return analyzerState_ == AS_SUCCEEDED;
}

bool BinaryAnalyzer::analyzeBinarySynchron(const std::string& filenameBinary)
{
	return analyzeBinarySynchron(filenameBinary, getDefaultObjdumpPath(filenameBinary));
}

bool BinaryAnalyzer::analyzeBinaryAsynchron(const std::string& filenameBinary, const std::string& objdumpPath)
{
	const ScopedLock scopedLock(lock_);

	if (analyzerState_ == AS_WORKING)
	{
		return false;
	}

	const bool clearResult = clear();
	ocean_assert(clearResult);

	analyzerState_ = AS_WORKING;

	objdumpPath_ = objdumpPath;
	filenameBinary_ = filenameBinary;

	if (startThread())
	{
		return true;
	}

	analyzerState_ = AS_FAILED;
	return false;
}

bool BinaryAnalyzer::analyzeBinaryAsynchron(const std::string& filenameBinary)
{
	return analyzeBinaryAsynchron(filenameBinary, getDefaultObjdumpPath(filenameBinary));
}

BinaryAnalyzer::SymbolIdGroups BinaryAnalyzer::determineCallTraces(const SymbolId symbolId, const bool toRoot) const
{
	ocean_assert(symbolId < symbols_.size());

	const Symbol& symbolOfInterest = symbols_[symbolId];

	if (toRoot)
	{
		// if we have a root symbol already, we simply can return the symbol

		if (symbolOfInterest.isRootSymbol())
		{
			return SymbolIdGroups(1, SymbolIds(1, symbolOfInterest.id()));
		}
	}
	else
	{
		// if we have a symbol without children already, we simply can return the symbol

		if (symbolOfInterest.childSymbols().empty())
		{
			return SymbolIdGroups(1, SymbolIds(1, symbolOfInterest.id()));
		}
	}

	// toRoot == true:  we are going along parent symbols until we reach reach root symbols
	// toRoot == false: we are going along child symbols until we reach all leaf symbols

	SymbolIdGroups traces(toRoot ? symbolOfInterest.parentSymbols().size() : symbolOfInterest.childSymbols().size(), SymbolIds(1, symbolOfInterest.id()));
	SymbolIdSetGroup tracesSymbolsVisited(toRoot ? symbolOfInterest.parentSymbols().size() : symbolOfInterest.childSymbols().size(), SymbolIdSet{symbolOfInterest.id()});

	std::unordered_set<size_t> invalidTraces;

	size_t traceIndex = 0;
	while (traceIndex < traces.size())
	{
		ocean_assert(traces[traceIndex].back() < symbols_.size());
		const Symbol& symbol = symbols_[traces[traceIndex].back()];

		if ((toRoot && symbol.isRootSymbol()) || (!toRoot && symbol.childSymbols().empty()))
		{
			// we continue with the next branch
			traceIndex++;
			continue;
		}

		const SymbolIdSet& nextIdSet = toRoot ? symbol.parentSymbols() : symbol.childSymbols();
		ocean_assert(!nextIdSet.empty());

		// we extend our current trace and create new traces for all other parent/child ids

		// first, we create the new traces (starting with the second parent/child symbol)

		SymbolIdSet::const_iterator i = nextIdSet.cbegin();

		while (++i != nextIdSet.cend())
		{
			// we check whether we have seen the recent id already in our current trace
			if (tracesSymbolsVisited[traceIndex].find(*i) == tracesSymbolsVisited[traceIndex].end())
			{
				// we copy the current trace
				traces.push_back(traces[traceIndex]);
				tracesSymbolsVisited.push_back(tracesSymbolsVisited[traceIndex]);

				// and extend it
				traces.back().push_back(*i);
				tracesSymbolsVisited.back().insert(*i);
			}
		}

		// last, we extend our first trace
		// we check whether we have seen the recent id already in our current trace
		if (tracesSymbolsVisited[traceIndex].find(*nextIdSet.begin()) == tracesSymbolsVisited[traceIndex].end())
		{
			traces[traceIndex].push_back(*nextIdSet.begin());
			tracesSymbolsVisited[traceIndex].insert(*nextIdSet.begin());
		}
		else
		{
			// the current trace is a 'dead end' as we have seen an already visited symbol,
			// thus, we just flag this trace as invalid - we will filter the trace later
			invalidTraces.insert(traceIndex);

			// so we continue with the next trace
			traceIndex++;
		}
	}

	// finally we have to return valid traces only

	SymbolIdGroups result;
	result.reserve(traces.size());

	for (size_t n = 0; n < traces.size(); ++n)
	{
		if (invalidTraces.find(n) == invalidTraces.end())
		{
			if (toRoot)
			{
				// we reverse the order so that we start with the root node
				result.emplace_back(SymbolIds(traces[n].crbegin(), traces[n].crend()));
			}
			else
			{
				// we not not need to reverse the order so we can move the trace
				result.emplace_back(std::move(traces[n]));
			}
		}
	}

	return result;
}

BinaryAnalyzer::GraphNodeRefs BinaryAnalyzer::callTraces2callNodes(const SymbolIdGroups& parentTraces)
{
	SymbolIdSet rootSymbols;

	typedef std::unordered_map<SymbolId, SymbolIdSet> SymbolId2ChildIdsMap;

	SymbolId2ChildIdsMap symbolId2ChildIdsMap;

	for (size_t n = 0; n < parentTraces.size(); ++n)
	{
		const SymbolIds& parentTrace = parentTraces[n];

		// the first symbols of the provided parent traces are the root symbols (we may have several identical first symbols)

		ocean_assert(!parentTrace.empty());
		rootSymbols.insert(parentTrace.front());

		for (size_t i = 1; i < parentTrace.size(); ++i)
		{
			const SymbolId& fromId = parentTrace[i - 1];
			const SymbolId& toId = parentTrace[i];

			symbolId2ChildIdsMap[fromId].insert(toId);
		}
	}

	GraphNodeRefs nodeStack;
	nodeStack.reserve(64);

	for (SymbolIdSet::const_iterator i = rootSymbols.cbegin(); i != rootSymbols.cend(); ++i)
	{
		nodeStack.push_back(std::make_shared<GraphNode>(*i));
	}

	GraphNodeRefs callGraphRootNodes(nodeStack);

	while (!nodeStack.empty())
	{
		const GraphNodeRef node = nodeStack.back();
		ocean_assert(node);

		nodeStack.pop_back();

		ocean_assert(node->childNodes().empty());
		const SymbolId2ChildIdsMap::const_iterator i = symbolId2ChildIdsMap.find(node->symbolId());

		// let's check whether we have a child symbol
		if (i != symbolId2ChildIdsMap.end())
		{
			for (const SymbolId& symbolId : i->second)
			{
				const GraphNodeRef childNode = std::make_shared<GraphNode>(symbolId);

				node->childNodes_.push_back(childNode);

				nodeStack.push_back(childNode);
			}
		}
	}

	return callGraphRootNodes;
}

BinaryAnalyzer::GraphNodeRef BinaryAnalyzer::determineReducedCallGraphForChildren(const SymbolId symbolId, SymbolIdSet* allSymbolsIds) const
{
	ocean_assert(symbolId < symbols_.size());

	GraphNodeRefs nodeStack;
	nodeStack.reserve(64);

	const GraphNodeRef rootNode = std::make_shared<GraphNode>(symbolId);
	nodeStack.push_back(rootNode);

	SymbolIdSet symbolsVisited;

	while (!nodeStack.empty())
	{
		const GraphNodeRef node = nodeStack.back();
		ocean_assert(node);

		nodeStack.pop_back();

		const Symbol& symbol = symbols_[node->symbolId()];

		if (symbolsVisited.find(symbol.id()) != symbolsVisited.cend())
			continue;

		symbolsVisited.insert(symbol.id());

		const SymbolIdSet& symbolIds = symbol.childSymbols();

		for (SymbolIdSet::const_iterator i = symbolIds.cbegin(); i != symbolIds.cend(); ++i)
		{
			const GraphNodeRef newNode = std::make_shared<GraphNode>(*i);

			node->childNodes_.push_back(newNode);
			nodeStack.push_back(newNode);
		}
	}

	if (allSymbolsIds)
	{
		*allSymbolsIds = std::move(symbolsVisited);
	}

	return rootNode;
}

BinaryAnalyzer::SymbolIdSet BinaryAnalyzer::determineChildGraphSymbols(const SymbolId symbolId) const
{
	ocean_assert(symbolId < symbols_.size());

	SymbolIdSet childSymbols;
	childSymbols.insert(symbolId); // we add it here and will remove it later again

	SymbolIdSet symbolsStack;
	symbolsStack.insert(symbolId);

	while (!symbolsStack.empty())
	{
		const SymbolIdSet& localChildSymbols = symbols_[*symbolsStack.begin()].childSymbols();
		symbolsStack.erase(symbolsStack.begin());

		for (const SymbolId& childId : localChildSymbols)
		{
			if (childSymbols.insert(childId).second)
			{
				symbolsStack.insert(childId);
			}
		}
	}

	childSymbols.erase(symbolId); // we remove it again

	return childSymbols;
}

BinaryAnalyzer::SymbolIdSet BinaryAnalyzer::determineChildGraphSymbols(const SymbolIdSet& symbolIds) const
{
	ocean_assert(!symbolIds.empty());

	SymbolIdSet childSymbols;

	SymbolIdSet symbolsStack = symbolIds;

	while (!symbolsStack.empty())
	{
		const SymbolIdSet& localChildSymbols = symbols_[*symbolsStack.begin()].childSymbols();
		symbolsStack.erase(symbolsStack.begin());

		for (const SymbolId& childId : localChildSymbols)
		{
			if (childSymbols.insert(childId).second)
			{
				symbolsStack.insert(childId);
			}
		}
	}

	return childSymbols;
}

void BinaryAnalyzer::determineSizeImpact(const SymbolId& symbolId, unsigned int& minimalSize, double& weightedSize, unsigned int& maximalSize) const
{
	ocean_assert(symbolId < symbols_.size());

	// first, we need to add the binary size of the symbol itself
	minimalSize = symbols_[symbolId].size();
	weightedSize = double(symbols_[symbolId].size());
	maximalSize = symbols_[symbolId].size();

	const SymbolIdSet childGraphSymbols(determineChildGraphSymbols(symbolId));

	for (SymbolIdSet::const_iterator i = childGraphSymbols.cbegin(); i != childGraphSymbols.cend(); ++i)
	{
		const Symbol& symbol = symbols_[*i];

		// we must count the size of the symbol for the maximal case
		maximalSize += symbol.size();

		// now we check whether this symbol is used outside our graph

		const SymbolIdSet& localParents = symbol.parentSymbols();

		size_t parentsWithinGraph = 0;
		for (const SymbolId& parentId : localParents)
		{
			if (parentId == symbolId || childGraphSymbols.find(parentId) != childGraphSymbols.cend())
			{
				parentsWithinGraph++;
			}
		}

		ocean_assert(parentsWithinGraph <= localParents.size());
		if (parentsWithinGraph == localParents.size())
		{
			// the symbol is not used outside of our graph, so we have to count the size for the minimal case
			minimalSize += symbol.size();

			// and we have to add it to the shared size by 100%
			weightedSize += double(symbol.size());
		}
		else
		{
			ocean_assert(parentsWithinGraph >= 1u && parentsWithinGraph < localParents.size());

			// we count the size in relation to the parents within our graph and outside our graph

			weightedSize += double(symbol.size() * parentsWithinGraph) / double(localParents.size());
		}
	}
}

void BinaryAnalyzer::determineSizeImpact(const SymbolIdSet& symbolIds, unsigned int& minimalSize, double& weightedSize, unsigned int& maximalSize) const
{
	ocean_assert(!symbolIds.empty());

	const SymbolIdSet childGraphSymbols(determineChildGraphSymbols(symbolIds));

	minimalSize = 0u;
	weightedSize = 0.0;
	maximalSize = 0u;

	for (const SymbolId& symbolId : symbolIds)
	{
		// we do not count the binary size of the source symbols if they appear in the child graph
		if (childGraphSymbols.find(symbolId) == childGraphSymbols.end())
		{
			const Symbol& symbol = symbols_[symbolId];

			minimalSize += symbol.size();
			weightedSize += double(symbol.size());
			maximalSize += symbol.size();
		}
	}

	for (SymbolIdSet::const_iterator i = childGraphSymbols.cbegin(); i != childGraphSymbols.cend(); ++i)
	{
		const Symbol& symbol = symbols_[*i];

		// we must count the size of the symbol for the maximal case
		maximalSize += symbol.size();

		// now we check whether this symbol is used outside our graph

		const SymbolIdSet& localParents = symbol.parentSymbols();

		size_t parentsWithinGraph = 0;
		for (const SymbolId& parentId : localParents)
		{
			if (symbolIds.find(parentId) != symbolIds.end() || childGraphSymbols.find(parentId) != childGraphSymbols.cend())
			{
				parentsWithinGraph++;
			}
		}

		ocean_assert(parentsWithinGraph <= localParents.size());
		if (parentsWithinGraph == localParents.size())
		{
			// the symbol is not used outside of our graph, so we have to count the size for the minimal case
			minimalSize += symbol.size();

			// and we have to add it to the shared size by 100%
			weightedSize += double(symbol.size());
		}
		else
		{
			ocean_assert(parentsWithinGraph >= 1u && parentsWithinGraph < localParents.size());

			// we count the size in relation to the parents within our graph and outside our graph

			weightedSize += double(symbol.size() * parentsWithinGraph) / double(localParents.size());
		}
	}
}

std::string BinaryAnalyzer::determineSizeImpactString(const SymbolIdSet& symbolIds) const
{
	if (symbolIds.empty())
	{
		return std::string();
	}

	unsigned int minimalSize = 0u;
	double weightedSize = 0.0;
	unsigned int maximalSize = 0u;

	if (symbolIds.size() == 1)
	{
		determineSizeImpact(*symbolIds.begin(), minimalSize, weightedSize, maximalSize);
	}
	else
	{
		determineSizeImpact(symbolIds, minimalSize, weightedSize, maximalSize);
	}

	std::string text("Size impact: ");
	text += std::string(" at least: ") + sizeString(double(minimalSize)) + std::string(",");
	text += std::string(" weighted: ") + sizeString(weightedSize) + std::string(",");
	text += std::string(" at most: ") + sizeString(double(maximalSize));

	return text;
}

bool BinaryAnalyzer::clear()
{
	const ScopedLock scopedLock(lock_);

	if (analyzerState_ == AS_WORKING)
	{
		return false;
	}

	filenameBinary_.clear();
	textStrings_.clear();
	symbols_.clear();
	address2SymbolIdMap_.clear();
	address2StringIdMap_.clear();

	return true;
}

bool BinaryAnalyzer::writeToDataFile(const std::string& filename)
{
	if (filename.empty())
		return false;

	std::ofstream stream;
	stream.open(filename.c_str(), std::ios::binary);

	if (!stream.is_open())
		return false;

	IO::OutputBitstream bitstream(stream);

	// version 2 (now with alias names):
	if (!bitstream.write<unsigned long long>(2ull))
		return false;

	if (!bitstream.write<unsigned long long>((unsigned long long)symbols_.size()))
		return false;

	for (size_t n = 0; n < symbols_.size(); ++n)
	{
		const Symbol& symbol = symbols_[n];

		if (!bitstream.write<std::string>(symbol.name()))
			return false;

		if (!bitstream.write<std::string>(symbol.readableName()))
			return false;

		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.id()))
			return false;

		if (!bitstream.write<unsigned long long>(symbol.address()))
			return false;

		if (!bitstream.write<unsigned int>(symbol.size()))
			return false;

		// child symbols
		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.childSymbols().size()))
			return false;

		for (const SymbolId& symbolId : symbol.childSymbols())
		{
			if (!bitstream.write<unsigned long long>((unsigned long long)symbolId))
				return false;
		}

		// parent symbols
		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.parentSymbols().size()))
			return false;

		for (const SymbolId& symbolId : symbol.parentSymbols())
		{
			if (!bitstream.write<unsigned long long>((unsigned long long)symbolId))
				return false;
		}

		// loaded addresses
		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.loadedAddressed().size()))
			return false;

		for (const Address64& loadedAddress : symbol.loadedAddressed())
		{
			if (!bitstream.write<unsigned long long>((unsigned long long)loadedAddress))
				return false;
		}

		// alias names
		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.aliasNames().size()))
			return false;

		for (const std::string& aliasName : symbol.aliasNames())
		{
			if (!bitstream.write<std::string>(aliasName))
				return false;
		}

		// readable alias names
		if (!bitstream.write<unsigned long long>((unsigned long long)symbol.readableAliasNames().size()))
			return false;

		for (const std::string& readableAliasName : symbol.readableAliasNames())
		{
			if (!bitstream.write<std::string>(readableAliasName))
				return false;
		}
	}

	return true;
}

bool BinaryAnalyzer::writeToJsonFile(const std::string& filename)
{
	if (filename.empty())
		return false;

	IO::JSONConfig config(filename, false);
	writeToJsonConfig(config);
	return config.write();
}

void BinaryAnalyzer::writeToJsonConfig(IO::JSONConfig& config)
{
	// version 2 (now with alias names):
	config["Version"] = 2;

	config["SymbolSize"] = (int)symbols_.size();

	IO::JSONConfig::Value& symbolsNode = config.add("Symbols");

	for (size_t n = 0; n < symbols_.size(); ++n)
	{
		const Symbol& symbol = symbols_[n];

		IO::JSONConfig::Value& symbolNode = symbolsNode.add("Symbol");

		symbolNode["Name"] = symbol.name();
		symbolNode["ReadableName"] = symbol.readableName();
		symbolNode["Id"] = (int)symbol.id();
		symbolNode["Address"] = std::to_string(symbol.address());
		symbolNode["Size"] = (int)symbol.size();

		symbolNode["ChildSymbolsSize"] = (int)symbol.childSymbols().size();
		symbolNode["ChildSymbols"] = std::vector<int>(symbol.childSymbols().begin(), symbol.childSymbols().end());

		symbolNode["ParentSymbolsSize"] = (int)symbol.parentSymbols().size();
		symbolNode["ParentSymbols"] = std::vector<int>(symbol.parentSymbols().begin(), symbol.parentSymbols().end());

		symbolNode["LoadedAddressesSize"] = (int)symbol.loadedAddressed().size();
		std::vector<std::string> addresses;
		std::transform(std::begin(symbol.loadedAddressed()), std::end(symbol.loadedAddressed()), std::back_inserter(addresses), [](Address64 d) { return std::to_string(d); });
		symbolNode["LoadedAddress"] = addresses;

		symbolNode["AliasNamesSize"] = int(symbol.aliasNames().size()); // since version 2
		symbolNode["AliasNames"] = symbol.aliasNames();

		symbolNode["ReadableAliasNamesSize"] = int(symbol.readableAliasNames().size()); // since version 2
		symbolNode["ReadableAliasNames"] = symbol.readableAliasNames();

		unsigned int minimalSize = 0u;
		double weightedSize = 0.0;
		unsigned int maximalSize = 0u;
		determineSizeImpact(symbol.id(), minimalSize, weightedSize, maximalSize);
		symbolNode["SizeImpact(Minimal)"] = (int)minimalSize;
		symbolNode["SizeImpact(Weighted)"] = weightedSize;
		symbolNode["SizeImpact(Maximal)"] = (int)maximalSize;
	}
}

bool BinaryAnalyzer::readFromDataFile(const std::string& filename)
{
	if (filename.empty())
		return false;

	clear();

	const Timestamp startTimestamp(true);

	std::ifstream stream;
	stream.open(filename.c_str(), std::ios::binary);

	if (!stream.is_open())
		return false;

	IO::InputBitstream bitstream(stream);

	// version
	unsigned long long version = 0ull;
	if (!bitstream.look<unsigned long long>(version))
		return false;

	switch (version)
	{
		case 1ull:
		{
			if (!readFromDataFileV1(bitstream))
			{
				return false;
			}

			break;
		}

		case 2ull:
		{
			if (!readFromDataFileV2(bitstream))
			{
				return false;
			}

			break;
		}

		default:
		{
			ocean_assert(false && "Version not supported!");
			return false;
		}
	}

	Log::info() << "Finished reading data file within " << String::toAString(double(Timestamp(true) - startTimestamp), 2u) << " seconds";

	return true;
}

bool BinaryAnalyzer::readFromDataFileV1(IO::InputBitstream& bitstream)
{
	// version
	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
		return false;

	unsigned long long symbolsSize = 0ull;
	if (!bitstream.read<unsigned long long>(symbolsSize))
		return false;

	symbols_.reserve(symbolsSize);

	for (unsigned long long n = 0ull; n < symbolsSize; ++n)
	{
		std::string name;
		if (!bitstream.read<std::string>(name) || name.empty())
			return false;

		std::string readableName;
		if (!bitstream.read<std::string>(readableName) || readableName.empty())
			return false;

		unsigned long long symbolId = (unsigned long long)(-1);
		if (!bitstream.read<unsigned long long>(symbolId) || symbolId >= symbolsSize)
			return false;

		unsigned long long address = 0ull;
		if (!bitstream.read<unsigned long long>(address))
			return false;

		unsigned int size = (unsigned int)(-1);
		if (!bitstream.read<unsigned int>(size))
			return false;

		SymbolIdSet childSymbolIdSet;

		unsigned long long childSymbolSize = 0ull;
		if (!bitstream.read<unsigned long long>(childSymbolSize) || childSymbolSize >= symbolsSize)
			return false;

		for (unsigned long long i = 0ull; i < childSymbolSize; ++i)
		{
			unsigned long long childId = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(childId) || childId >= symbolsSize)
				return false;

			childSymbolIdSet.insert(SymbolId(childId));
		}

		SymbolIdSet parentSymbolIdSet;

		unsigned long long parentSymbolSize = 0ull;
		if (!bitstream.read<unsigned long long>(parentSymbolSize) || parentSymbolSize >= symbolsSize)
			return false;

		for (unsigned long long i = 0ull; i < parentSymbolSize; ++i)
		{
			unsigned long long parentId = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(parentId) || parentId >= symbolsSize)
				return false;

			parentSymbolIdSet.insert(parentId);
		}

		AddressSet64 loadedAddressSet;

		unsigned long long loadedAddressSize = 0ull;
		if (!bitstream.read<unsigned long long>(loadedAddressSize))
			return false;

		for (unsigned long long i = 0ull; i < loadedAddressSize; ++i)
		{
			unsigned long long loadedAddress = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(loadedAddress))
				return false;

			loadedAddressSet.insert(loadedAddress);
		}

		if (symbols_.size() != size_t(symbolId))
		{
			return false;
		}

		Symbol newSymbol(SymbolId(symbolId), std::move(name), std::move(readableName), address, size, std::move(parentSymbolIdSet), std::move(childSymbolIdSet), std::move(loadedAddressSet));

		if (!address2SymbolIdMap_.insert(std::make_pair(newSymbol.address(), newSymbol.id())).second)
		{
			return false;
		}

		symbols_.push_back(std::move(newSymbol));
	}

	return true;
}

bool BinaryAnalyzer::readFromDataFileV2(IO::InputBitstream& bitstream)
{
	// version
	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 2ull)
		return false;

	unsigned long long symbolsSize = 0ull;
	if (!bitstream.read<unsigned long long>(symbolsSize))
		return false;

	symbols_.reserve(symbolsSize);

	for (unsigned long long n = 0ull; n < symbolsSize; ++n)
	{
		std::string name;
		if (!bitstream.read<std::string>(name) || name.empty())
			return false;

		std::string readableName;
		if (!bitstream.read<std::string>(readableName) || readableName.empty())
			return false;

		unsigned long long symbolId = (unsigned long long)(-1);
		if (!bitstream.read<unsigned long long>(symbolId) || symbolId >= symbolsSize)
			return false;

		unsigned long long address = 0ull;
		if (!bitstream.read<unsigned long long>(address))
			return false;

		unsigned int size = (unsigned int)(-1);
		if (!bitstream.read<unsigned int>(size))
			return false;


		// child symbols
		SymbolIdSet childSymbolIdSet;

		unsigned long long childSymbolSize = 0ull;
		if (!bitstream.read<unsigned long long>(childSymbolSize) || childSymbolSize >= symbolsSize)
			return false;

		for (unsigned long long i = 0ull; i < childSymbolSize; ++i)
		{
			unsigned long long childId = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(childId) || childId >= symbolsSize)
				return false;

			childSymbolIdSet.insert(SymbolId(childId));
		}


		// parent symbols
		SymbolIdSet parentSymbolIdSet;

		unsigned long long parentSymbolSize = 0ull;
		if (!bitstream.read<unsigned long long>(parentSymbolSize) || parentSymbolSize >= symbolsSize)
			return false;

		for (unsigned long long i = 0ull; i < parentSymbolSize; ++i)
		{
			unsigned long long parentId = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(parentId) || parentId >= symbolsSize)
				return false;

			parentSymbolIdSet.insert(parentId);
		}


		// loaded addresses
		AddressSet64 loadedAddressSet;

		unsigned long long loadedAddressSize = 0ull;
		if (!bitstream.read<unsigned long long>(loadedAddressSize))
			return false;

		for (unsigned long long i = 0ull; i < loadedAddressSize; ++i)
		{
			unsigned long long loadedAddress = (unsigned long long)(-1);
			if (!bitstream.read<unsigned long long>(loadedAddress))
				return false;

			loadedAddressSet.insert(loadedAddress);
		}


		// alias names
		std::vector<std::string> aliasNames;

		unsigned long long aliasNamesSize = 0ull;
		if (!bitstream.read<unsigned long long>(aliasNamesSize))
			return false;

		aliasNames.reserve(aliasNamesSize);

		for (unsigned long long i = 0ull; i < aliasNamesSize; ++i)
		{
			std::string aliasName;
			if (!bitstream.read<std::string>(aliasName) || aliasName.empty())
				return false;

			aliasNames.push_back(std::move(aliasName));
		}


		// readable alias names
		std::vector<std::string> readableAliasNames;

		unsigned long long readableAliasNamesSize = 0ull;
		if (!bitstream.read<unsigned long long>(readableAliasNamesSize))
			return false;

		readableAliasNames.reserve(readableAliasNamesSize);

		for (unsigned long long i = 0ull; i < readableAliasNamesSize; ++i)
		{
			std::string readableAliasName;
			if (!bitstream.read<std::string>(readableAliasName) || readableAliasName.empty())
				return false;

			readableAliasNames.push_back(std::move(readableAliasName));
		}

		if (symbols_.size() != size_t(symbolId))
		{
			return false;
		}

		Symbol newSymbol(SymbolId(symbolId), std::move(name), std::move(readableName), address, size, std::move(parentSymbolIdSet), std::move(childSymbolIdSet), std::move(loadedAddressSet));

		newSymbol.aliasNames_ = std::move(aliasNames);
		newSymbol.readableAliasNames_ = std::move(readableAliasNames);

		if (!address2SymbolIdMap_.insert(std::make_pair(newSymbol.address(), newSymbol.id())).second)
		{
			return false;
		}

		symbols_.push_back(std::move(newSymbol));
	}

	return true;
}

std::string BinaryAnalyzer::sizeString(const double size)
{
	const std::string sizeKBs = String::toAString(double(size) / 1024.0, 2u);

	const std::string::size_type pos = sizeKBs.find('.');

	if (pos == std::string::npos)
	{
		return String::insertCharacter(sizeKBs, ',', 3, false) + std::string(" KB");
	}
	else
	{
		return String::insertCharacter(sizeKBs.substr(0, pos), ',', 3, false) + sizeKBs.substr(pos) + std::string(" KB");
	}
}

void BinaryAnalyzer::threadRun()
{
	analyze();
}

void BinaryAnalyzer::analyze()
{
	ocean_assert(analyzerState_ == AS_WORKING);

	const IO::File file(filenameBinary_);

	if (file.extension() == dataFileExtension())
	{
		Log::info() << "Reading data file: " << filenameBinary_;

		if (!readFromDataFile(filenameBinary_))
		{
			Log::error() << "Reading data file FAILED!";
			return;
		}

		Log::info() << "Reading data file succeeded.";
	}
	else
	{
		Log::info() << "Start analyzing binary: " << filenameBinary_;

		// we simply use the file's extension to decide whether we parse an Android binary or an iOS binary - that should be improved

		if (IO::File(filenameBinary_).extension() == "so")
		{
			Log::info() << "We analyze an binary for Android platforms...";

			if (!DisassemblerAndroid::disassemble(objdumpPath_, filenameBinary_, *this))
			{
				const ScopedLock scopedLock(lock_);

				analyzerState_ = AS_FAILED;
				return;
			}
		}
		else
		{
			Log::info() << "We analyze an binary for iOS platforms...";

			if (!DisassemblerIOS::disassemble(objdumpPath_, filenameBinary_, *this))
			{
				const ScopedLock scopedLock(lock_);

				analyzerState_ = AS_FAILED;
				return;
			}
		}

		Log::info() << "Parsing binary succeeded.";
	}

	unsigned long long textStringSize = 0ull;
	for (const TextString& textString : textStrings_)
	{
		textStringSize += (unsigned long long)textString.size();
	}

	Log::info() << "We found " << textStrings_.size() << " strings with together " << sizeString(textStringSize);

	unsigned long long symbolsSize = 0ull;
	for (const Symbol& symbol : symbols_)
	{
		symbolsSize += (unsigned long long)symbol.size();
	}

	Log::info() << "We found " << symbols_.size() << " symbols with together " << sizeString(symbolsSize);

	const ScopedLock scopedLock(lock_);

	analyzerState_ = AS_SUCCEEDED;
}
