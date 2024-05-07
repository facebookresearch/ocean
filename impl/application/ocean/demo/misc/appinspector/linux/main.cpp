// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"
#include "ocean/base/Messenger.h"
#include "ocean/io/JSONConfig.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;
using namespace Ocean;

const std::string KEY_FIRST_ID = "1st Id";
const std::string KEY_FIRST_SIZE = "1st Size";
const std::string KEY_FIRST_NAME = "1st Name";

const std::string KEY_SECOND_ID = "2nd Id";
const std::string KEY_SECOND_SIZE = "2nd Size";
const std::string KEY_SECOND_NAME = "2nd Name";

const std::string KEY_RESULT = "Result";

bool prepareResultFile(const std::string& filename)
{
	std::ofstream file(filename, std::ios_base::trunc);

	return true;
}

void showRootSymbolsById(const BinaryAnalyzer& binaryAnalyzer, const BinaryAnalyzer::Symbols& symbols, bool onlyShowRoot)
{
	unsigned int minimalSize = 0u;
	double weightedSize = 0.0;
	unsigned int maximalSize = 0u;

	std::stringstream outputStream;
	outputStream
		<< std::endl
		<< setw(6) << left << "Root"
		<< setw(6) << left << "ID"
		<< setw(10) << left << "Size"
		<< setw(10) << left << "(Min)"
		<< setw(10) << left << "(Wgted)"
		<< setw(10) << left << "(Max)"
		<< setw(20) << left << "Name";
	Log::info() << outputStream.str();

	for (const auto& symbol : symbols)
	{
		auto isRoot = symbol.isRootSymbol();
		if (onlyShowRoot && !isRoot)
		{
			continue;
		}

		binaryAnalyzer.determineSizeImpact(symbol.id(), minimalSize, weightedSize, maximalSize);

		std::stringstream localOutputStream;
		localOutputStream
			<< setw(6) << left << (isRoot ? "+" : "-")
			<< setw(6) << left << symbol.id()
			<< setw(10) << left << symbol.size()
			<< setw(10) << left << minimalSize
			<< setw(10) << left << weightedSize
			<< setw(10) << left << maximalSize
			<< setw(20) << left << symbol.readableName();
		Log::info() << localOutputStream.str();
	}
}

bool analyzeBinary(BinaryAnalyzer& binaryAnalyzer, const std::string& binaryPath, const std::string& objdumpPath)
{
	if (!binaryAnalyzer.analyzeBinaryAsynchron(binaryPath, objdumpPath))
	{
		Log::error() << "Error: Analyzing fails!";
		return false;
	}

	while (binaryAnalyzer.isState(BinaryAnalyzer::AS_WORKING))
	{
	}

	if (binaryAnalyzer.isState(BinaryAnalyzer::AS_FAILED))
	{
		Log::error() << "Error: Analyzing fails!";
		return false;
	}
	return true;
}

bool analyzeBinaryAndSave(
	BinaryAnalyzer& binaryAnalyzer,
	const std::string& binaryPath,
	const std::string& objdumpPath,
	const std::string& jsonResultFile,
	Ocean::IO::JSONConfig& config)
{
	if (jsonResultFile.empty())
	{
		return true;
	}

	Log::info() << "\n==============================================";
	if (!analyzeBinary(binaryAnalyzer, binaryPath, objdumpPath))
	{
		return false;
	}

	const auto& symbols = binaryAnalyzer.symbols();
	showRootSymbolsById(binaryAnalyzer, symbols, true);

	// generate Json results
	Log::info() << "Start writing data.";
	binaryAnalyzer.writeToJsonConfig(config);

	if (!prepareResultFile(jsonResultFile) || !config.write())
	{
		Log::error() << "Error: Saving result fails!";
		return false;
	}

	Log::info() << "JSON result saved in file " << jsonResultFile;
	return true;
}

bool compareJSONResults(Ocean::IO::JSONConfig& config, Ocean::IO::JSONConfig& config2, const std::string& compareResultFile)
{
	Log::info() << "\n==============================================";
	Log::info() << "Start comparing binaries ";

	Ocean::IO::JSONConfig compareConfig(compareResultFile, false);
	Ocean::IO::JSONConfig::Value& SymbolPairs = compareConfig.add("SymbolPairs");

	// Compare
	auto& symbolsConfig1 = config.value("Symbols", 0);
	auto& symbolsConfig2 = config2.value("Symbols", 0);
	const int symbolsNum1 = symbolsConfig1.values("Symbol");
	const int symbolsNum2 = symbolsConfig2.values("Symbol");
	std::vector<bool> symbols2Matched = std::vector<bool>(symbolsNum2, false);
	const std::string emptyString = "";

	std::stringstream outputStream;
	outputStream
		<< std::endl
		<< setw(6) << left << "Result"
		<< setw(6) << left << "ID1"
		<< setw(10) << left << "Size1"
		<< setw(6) << left << "ID2"
		<< setw(10) << left << "Size2";
	Log::info() << outputStream.str();

	for (int i = 0; i < symbolsNum1; ++i)
	{
		auto& symbol1 = symbolsConfig1.value("Symbol", i);
		const auto& name1 = symbol1.value("Name", 0);
		if (name1(emptyString).empty())
		{
			continue;
		}
		const auto& id1 = symbol1.value("Id", 0);
		const auto& size1 = symbol1.value("Size", 0);
		// Default 1st > 2nd for the case when we don't find match for 2nd file.
		std::string resultChar = ">";

		auto& symbolPair = SymbolPairs.add("SymbolPair");
		symbolPair[KEY_FIRST_ID] = id1(-1);
		symbolPair[KEY_FIRST_SIZE] = size1(-1);
		symbolPair[KEY_FIRST_NAME] = name1(emptyString);

		string id2Output = "-";
		string size2Output = "-";

		for (int j = 0; j < symbolsNum2; ++j)
		{
			if (symbols2Matched[j])
			{
				continue;
			}

			auto& symbol2 = symbolsConfig2.value("Symbol", j);
			const auto& name2 = symbol2.value("Name", 0);

			if (name1(emptyString).compare(name2(emptyString)) == 0)
			{
				symbols2Matched[j] = true;
				const auto& id2 = symbol2.value("Id", 0);
				const auto& size2 = symbol2.value("Size", 0);
				symbolPair[KEY_SECOND_ID] = id2(-1);
				symbolPair[KEY_SECOND_SIZE] = size2(-1);
				symbolPair[KEY_SECOND_NAME] = name2(emptyString);

				id2Output = to_string(id2(-1));
				size2Output = to_string(size2(-1));

				if (size1(0) == size2(0))
				{
					resultChar = "=";
				}
				else if (size1(0) < size2(0))
				{
					resultChar = "<";
				}
				// Found a match - no need to check the remaining items.
				break;
			}
		}
		symbolPair[KEY_RESULT] = resultChar;

		// Logging
		std::stringstream localOutputStream;
		localOutputStream
			<< setw(6) << left << resultChar
			<< setw(6) << left << id1(-1)
			<< setw(10) << left << size1(-1)
			<< setw(6) << left << id2Output
			<< setw(10) << left << size2Output;
		Log::info() << localOutputStream.str();
	}

	for (int j = 0; j < symbolsNum2; ++j)
	{
		if (symbols2Matched[j])
		{
			continue;
		}

		auto& symbolPair = SymbolPairs.add("SymbolPair");
		auto& symbol2 = symbolsConfig2.value("Symbol", j);
		const auto& name2 = symbol2.value("Name", 0);
		const auto& id2 = symbol2.value("Id", 0);
		const auto& size2 = symbol2.value("Size", 0);
		symbolPair[KEY_SECOND_ID] = id2(-1);
		symbolPair[KEY_SECOND_SIZE] = size2(-1);
		symbolPair[KEY_SECOND_NAME] = name2(emptyString);
		symbolPair[KEY_RESULT] = "<";

		// Logging
		std::stringstream localOutputStream;
		localOutputStream
			<< setw(6) << left << "<"
			<< setw(6) << left << "-"
			<< setw(10) << left << "-"
			<< setw(6) << left << id2(-1)
			<< setw(10) << left << size2(-1);
		Log::info() << localOutputStream.str();
	}

	// Save result
	if (!prepareResultFile(compareResultFile) || !compareConfig.write())
	{
		Log::error() << "Error: Saving compare result fails!";
		return false;
	}

	Log::info() << "Detailed JSON comparision result saved in file " << compareResultFile;
	return true;
}

int main(int argc, char* argv[])
{
	// Parse args, get path to stuff
	if (argc < 4)
	{
		Log::info() << "Ocean Appinspector (CLI version):";
		Log::info() << "Usage: ocean_app_appinspector_linux binary_path result_directory objdump_path [binary_path_to_compare]";
		Log::info() << "Required parameters:";
		Log::info() << "Parameter 1: [Binary file to be analyzed, e.g., \"Facebook.app/Facebook.txt\"] ";
		Log::info() << "Parameter 2: [Directory to save the result, e.g., \"your/path/to/save/\"] ";
		Log::info() << "Parameter 3: [File of the objdump tool. For ios builds, you need to install & input the jackalope's objdump tool ";
		Log::info() << "				e.g., \"/usr/bin/objdump\" or \"/opt/iosbuild/xcode_9.4.1/Developer/Toolchains/osmeta-stable.xctoolchain/usr/bin/objdump\"] ";
		Log::info() << "Parameter 4 (Optional): [Binary file to be compared, e.g., \"Facebook.app/Facebook.txt\"] ";

		return 1;
	}

	std::string binaryPath = argv[1];
	std::string resultDir = argv[2];
	std::string objdumpPath = argv[3];
	std::string binaryToComparePath = "";

	if (argc > 4)
	{
		binaryToComparePath = argv[4];
	}

	Ocean::Messenger::get().setOutputType(Ocean::Messenger::OUTPUT_STANDARD);

	// Analyze First Binary
	BinaryAnalyzer binaryAnalyzer;
	std::string jsonResultFile = resultDir + std::string("json.txt");
	Ocean::IO::JSONConfig config(jsonResultFile, false);
	if (!analyzeBinaryAndSave(binaryAnalyzer, binaryPath, objdumpPath, jsonResultFile, config))
	{
		return 1;
	}

	// Analyze Second Binary if exists
	if (!binaryToComparePath.empty())
	{
		BinaryAnalyzer binaryAnalyzer2;
		std::string jsonResultFile2 = resultDir + std::string("json2.txt");
		Ocean::IO::JSONConfig config2(jsonResultFile2, false);
		if (!analyzeBinaryAndSave(binaryAnalyzer2, binaryToComparePath, objdumpPath, jsonResultFile2, config2))
		{
			return 1;
		}

		// Compare result
		std::string jsonCompareResultFile = resultDir + std::string("jsonCompare.txt");
		if (!compareJSONResults(config, config2, jsonCompareResultFile))
		{
			return 1;
		}
	}

	return 0;
}
