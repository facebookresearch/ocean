/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/misc/appinspector/BinaryAnalyzer.h"
#include "ocean/base/Messenger.h"

#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

void showRootSymbolsById(const BinaryAnalyzer& binaryAnalyzer, const BinaryAnalyzer::Symbols& symbols, bool onlyShowRoot)
{
	unsigned int minimalSize = 0u;
	double weightedSize = 0.0;
	unsigned int maximalSize = 0u;

	std::cout << "Number of symbols = " << symbols.size() << std::endl;
	std::cout
		<< setw(6) << left << "Root"
		<< setw(6) << left << "ID"
		<< setw(10) << left << "Size"
		<< setw(10) << left << "(Min)"
		<< setw(10) << left << "(Wgted)"
		<< setw(10) << left << "(Max)"
		<< setw(20) << left << "Name"
		<< std::endl;

	for (const auto& symbol : symbols)
	{
		auto isRoot = symbol.isRootSymbol();
		if (onlyShowRoot && !isRoot)
		{
			continue;
		}

		binaryAnalyzer.determineSizeImpact(symbol.id(), minimalSize, weightedSize, maximalSize);

		std::cout
			<< setw(6) << left << (isRoot ? "+" : "-")
			<< setw(6) << left << symbol.id()
			<< setw(10) << left << symbol.size()
			<< setw(10) << left << minimalSize
			<< setw(10) << left << weightedSize
			<< setw(10) << left << maximalSize
			<< setw(20) << left << symbol.readableName()
			<< std::endl;
	}
}

int main(int argc, char* argv[])
{
	// Parse args, get path to stuff
	if (argc < 4)
	{
		std::cout << "Ocean Appinspector (CLI version):" << std::endl
				  << std::endl;
		std::cout << "Usage: ocean_app_appinspector_linux binary_path result_directory objdump_path" << std::endl
				  << std::endl;
		std::cout << "Required parameters:" << std::endl;
		std::cout << "Parameter 1: [Binary file to be analyzed, e.g., \"Facebook.app/Facebook.txt\"] " << std::endl;
		std::cout << "Parameter 2: [Directory to save the result, e.g., \"your/path/to/save/\"] " << std::endl;
		std::cout << "Parameter 3: [File of the objdump tool. For ios builds, you need to install & input the jackalope's objdump tool " << std::endl;
		std::cout << "				e.g., \"/opt/iosbuild/xcode_9.4.1/Developer/Toolchains/osmeta-stable.xctoolchain/usr/bin/objdump\"] " << std::endl
				  << std::endl;
		std::cout << "Optional parameters:" << std::endl;
		std::cout << "L, log\t\t\tDisplay all logging information" << std::endl;

		return 1;
	}

	std::string binaryPath = argv[1];
	std::string resultDir = argv[2];
	std::string objdumpPath = argv[3];

	for (int i = 4; i < argc; ++i)
	{
		if (std::string(argv[i]) == "log" || std::string(argv[i]) == "L")
		{
			Ocean::Messenger::get().setOutputType(Ocean::Messenger::OUTPUT_STANDARD);
			break;
		}
	}

	// Prepare result files
	std::string resultPath = resultDir + std::string("result.txt");
	if (system(std::string("> " + resultPath).c_str()) != 0)
	{
		std::cerr
			<< "Error: Creating result file fails!, file = "
			<< resultPath
			<< std::endl;
		return 1;
	}

	// Prepare Ocean analyzer
	BinaryAnalyzer binaryAnalyzer;

	// OPEN
	std::cout << "Start analyzing binary." << std::endl;
	if (!binaryAnalyzer.analyzeBinaryAsynchron(binaryPath, objdumpPath))
	{
		std::cerr
			<< "Error: Analyzing fails!"
			<< std::endl;
		return 1;
	}

	while (binaryAnalyzer.isState(BinaryAnalyzer::AS_WORKING))
	{
	}

	if (binaryAnalyzer.isState(BinaryAnalyzer::AS_FAILED))
	{
		std::cerr
			<< "Error: Analyzing fails!"
			<< std::endl;
		return 1;
	}

	std::cout << "Start writing data." << std::endl;

	const auto& symbols = binaryAnalyzer.symbols();
	showRootSymbolsById(binaryAnalyzer, symbols, true);

	// SAVE
	if (!binaryAnalyzer.writeToJsonFile(resultPath))
	{
		std::cerr
			<< "Error: Saving result fails!"
			<< std::endl;
		return 1;
	}

	std::cout << "Detailed result saved in file " << resultPath << std::endl;

	return 0;
}
