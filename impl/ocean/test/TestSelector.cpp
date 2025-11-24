/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/TestSelector.h"

#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"

namespace Ocean
{

namespace Test
{

TestSelector::TestSelector(const std::string& testFunctions)
{
	const std::vector<std::string> tests = Utilities::separateValues(String::toLower(testFunctions), ',', true, true);

	patterns_ = Patterns(tests.begin(), tests.end());

	shouldExecute_ = true;
}

TestSelector::TestSelector(Patterns&& patterns, bool shouldExecute) :
	patterns_(std::move(patterns)),
	shouldExecute_(shouldExecute)
{
	// Nothing to do here
}

TestSelector TestSelector::shouldRun(const std::string& testName) const
{
	ocean_assert(!testName.empty());
	ocean_assert(testName.find('*') == std::string::npos);

	if (patterns_.empty())
	{
		// no patterns specified, so we run all tests

		return TestSelector(Patterns(), true /*shouldExecute*/);
	}

	const std::string testNameLower = String::toLower(testName);

	if (patterns_.find(testNameLower) != patterns_.end())
	{
		// we have a perfect match, so we run this test

		return TestSelector(Patterns(), true /*shouldExecute*/);
	}

	// let's collect all patterns that match this test or are hierarchical children

	Patterns subPatterns;
	bool foundWildcardMatch = false;

	const std::string prefix = testNameLower + '.';

	for (const std::string& pattern : patterns_)
	{
		if (matches(pattern, testNameLower))
		{
			foundWildcardMatch = true;
		}
		else if (pattern.size() > prefix.size() && pattern.substr(0, prefix.size()) == prefix)
		{
			// we have a hierarchical match, so we add the sub-pattern to the list of sub-patterns

			subPatterns.insert(pattern.substr(prefix.size()));
		}
	}

	if (foundWildcardMatch)
	{
		// matched via wildcard - run all sub-tests

		return TestSelector(Patterns(), true /*shouldExecute*/);
	}
	else if (!subPatterns.empty())
	{
		// hierarchical matches - run sub-tests

		return TestSelector(std::move(subPatterns), true /*shouldExecute*/);
	}

	// no match found, so we do not run this test

	return TestSelector(Patterns(), false /*shouldExecute*/);
}

TestSelector::operator bool() const
{
	return shouldExecute_;
}

bool TestSelector::matches(const std::string& pattern, const std::string& testName)
{
	ocean_assert(!pattern.empty());
	ocean_assert(!testName.empty());

	if (!pattern.empty() && pattern.back() == '*')
	{
		// we have a wildcard pattern, so we check if the test name starts with the prefix

		const std::string prefix = pattern.substr(0, pattern.size() - 1);

		if (testName.size() >= prefix.size() && testName.substr(0, prefix.size()) == prefix)
		{
			return true;
		}
	}

	// we have a normal pattern, so we check whether the pattern is a hierarchical match

	if (testName.size() > pattern.size() && testName.substr(0, pattern.size()) == pattern && testName[pattern.size()] == '.')
	{
		return true;
	}

	return false;
}

}

}
