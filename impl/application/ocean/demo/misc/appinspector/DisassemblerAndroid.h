/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_ANDROID_H
#define FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_ANDROID_H

#include "application/ocean/demo/misc/appinspector/Disassembler.h"

/**
 * This class implements a specialized disassembler for IOS binaries.
 * @ingroup applicationdemomiscappsizeanalyzerosx
 */
class DisassemblerAndroid : public Disassembler
{
	public:
		/**
		 * Disassembles a given binary file.
		 * @see Dissassembler::disassemble().
		 */
		static bool disassemble(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer);

	protected:

		/**
		 * Parses all executable sections of the binary.
		 * This function will invoke the disassembling within a separate thread while the main thread will read the result from a pipe.
		 * @return True, if succeeded
		 */
		static bool parseExecutableSections(const std::string& objdumpPath, const std::string& filename, BinaryAnalyzer& binaryAnalyzer);

		/**
		 * Parses all executable sections of the binary.
		 * This function reads the data from a stream actually getting the data from a pipe.
		 * @param stream The stream providing the actual data
		 * @param binaryAnalyzer The binary analyzer receiving the results
		 * @return True, if succeeded
		 */
		static bool parseExecutableSections(std::istream& stream, BinaryAnalyzer& binaryAnalyzer);

		/**
		 * Invokes a command asynchronously.
		 * @param command The command to be invoked asynchronously
		 */
		static void invokeCommandAsynchron(const std::string command);

		/**
		 * Creates a disassembly file for a specified binary file.
		 * This function will apply the disassembly in a separate thread while the output will be written to a file - actually a pipe.<br>
		 * The function returns immediately.
		 * @param objdumpPath The path of the objdump tool to use
		 * @param filenameBinary The filename of the binary to be analyzed
		 * @param filenameDisassembly The resulting filename of the resulting disassembly file, actually the name of a pipe
		 * @return True, if succeeded
		 */
		static bool createDisassemblyFile(const std::string& objdumpPath, const std::string& filenameBinary, std::string& filenameDisassembly);

		/**
		 * Returns whether a code instruction is a branch instruction.
		 * @param token The token containing the instruction, must be valid
		 * @param tokenLength The length of the token in characters, with range [1, infinity)
		 * @return True, if so
		 */
		static inline bool isBranchInstruction(const char* token, const size_t tokenLength);

		/**
		 * Returns whether a code instruction is an address load instruction.
		 * @param token The token containing the instruction, must be valid
		 * @param tokenLength The length of the token in characters, with range [1, infinity)
		 * @return True, if so
		 */
		static inline bool isAddressInstruction(const char* token, const size_t tokenLength);

		/**
		 * Returns whether a code instruction is an address pointer instruction (adrp).
		 * @param token The token containing the instruction, must be valid
		 * @param tokenLength The length of the token in characters, with range [1, infinity)
		 * @return True, if so
		 */
		static inline bool isAddressPointerInstruction(const char* token, const size_t tokenLength);

		/**
		 * Returns whether a code instruction is an add instruction (add).
		 * @param token The token containing the instruction, must be valid
		 * @param tokenLength The length of the token in characters, with range [1, infinity)
		 * @return True, if so
		 */
		static inline bool isAddInstruction(const char* token, const size_t tokenLength);
};

inline bool DisassemblerAndroid::isBranchInstruction(const char* token, const size_t tokenLength)
{
	ocean_assert(token != nullptr && tokenLength != 0);

	if (token[0] != 'b')
		return false;

	switch (tokenLength)
	{
		case 1:
		{
			// b Branch
			return true;
		}

		case 2:
		{
			// bl Branch with link.
			if (token[1] == 'l')
			{
				return true;
			}

			// br Branch to register.
			if (token[1] == 'r') // **TODO** parse register?
			{
				return false;
			}

			// bx Branch and exchange instruction set.
			if (token[1] == 'x')
			{
				return true;
			}

			break;
		}

		case 3:
		{
			// blr Branch with link to register
			if (token[1] == 'l' && token[2] == 'r') // **TODO** parse register?
			{
				return false;
			}

			// blx Branch with Link and exchange instruction set.
			if (token[1] == 'l' && token[2] == 'x')
			{
				return true;
			}

			// ble
			if (token[1] == 'l' && token[2] == 'e')
			{
				return true;
			}

			// blt
			if (token[1] == 'l' && token[2] == 't')
			{
				return true;
			}

			// bpl
			if (token[1] == 'p' && token[2] == 'l')
			{
				return true;
			}

			// bcc
			if (token[1] == 'c' && token[2] == 'c')
			{
				return true;
			}

			// bcs
			if (token[1] == 'c' && token[2] == 's')
			{
				return true;
			}

			// bge
			if (token[1] == 'g' && token[2] == 'e')
			{
				return true;
			}

			// bgt
			if (token[1] == 'g' && token[2] == 't')
			{
				return true;
			}

			// bhi
			if (token[1] == 'h' && token[2] == 'i')
			{
				return true;
			}

			// bmi
			if (token[1] == 'm' && token[2] == 'i')
			{
				return true;
			}

			// bne
			if (token[1] == 'n' && token[2] == 'e')
			{
				return true;
			}

			// beq
			if (token[1] == 'e' && token[2] == 'q')
			{
				return true;
			}

			// b.n
			if (token[1] == '.' && token[2] == 'n')
			{
				return true;
			}

			// b.w
			if (token[1] == '.' && token[2] == 'w')
			{
				return true;
			}

			break;
		}

		case 4:
		{
			if (token[1] == '.')
			{
				// b.eq Branch equal
				if (token[2] == 'e' && token[3] == 'q')
				{
					return true;
				}

				// b.ne Branch not equal
				if (token[2] == 'n' && token[3] == 'e')
				{
					return true;
				}

				// b.cs Branch carry set
				if (token[2] == 'c' && token[3] == 's')
				{
					return true;
				}

				// b.cc Branch carry clear
				if (token[2] == 'c' && token[3] == 'c')
				{
					return true;
				}

				// b.hs Branch higher or same (unsigned comparision)
				if (token[2] == 'h' && token[3] == 's')
				{
					return true;
				}

				// b.lo Branch lower (unsigned comparision)
				if (token[2] == 'l' && token[3] == 'o')
				{
					return true;
				}

				// b.mi Branch minus (minus or negative)
				if (token[2] == 'm' && token[3] == 'i')
				{
					return true;
				}

				// b.pl Branch plus (plus or zero)
				if (token[2] == 'p' && token[3] == 'l')
				{
					return true;
				}

				// b.vs Branch overflow set
				if (token[2] == 'v' && token[3] == 's')
				{
					return true;
				}

				// b.vc Branch overflow clear
				if (token[2] == 'v' && token[3] == 'c')
				{
					return true;
				}

				// b.hi Branch higher (unsigned)
				if (token[2] == 'h' && token[3] == 'i')
				{
					return true;
				}

				// b.ls Branch lower or same (unsigned comparision)
				if (token[2] == 'l' && token[3] == 's')
				{
					return true;
				}

				// b.ge Branch greater or equal (signed comparision)
				if (token[2] == 'g' && token[3] == 'e')
				{
					return true;
				}

				// b.lt Branch lesser than
				if (token[2] == 'l' && token[3] == 't')
				{
					return true;
				}

				// b.gt Branch greater than
				if (token[2] == 'g' && token[3] == 't')
				{
					return true;
				}

				// b.le Branch lesser or equal
				if (token[2] == 'l' && token[3] == 'e')
				{
					return true;
				}
			}

			// bxeq
			if (token[1] == 'x' && token[2] == 'e' && token[3] == 'q') // **TODO**
			{
				return true;
			}

			// bxne
			if (token[1] == 'x' && token[2] == 'n' && token[3] == 'e') // **TODO**
			{
				return true;
			}

			// bleq
			if (token[1] == 'l' && token[2] == 'e' && token[3] == 'q') // **TODO**
			{
				return true;
			}

			break;
		}

		case 5:
		{
			// bhi.n
			if (token[1] == 'h' && token[2] == 'i' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bne.n
			if (token[1] == 'n' && token[2] == 'e' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// beq.n
			if (token[1] == 'e' && token[2] == 'q' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bgt.n
			if (token[1] == 'g' && token[2] == 't' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bls.n
			if (token[1] == 'l' && token[2] == 's' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// blt.n
			if (token[1] == 'l' && token[2] == 't' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bge.n
			if (token[1] == 'g' && token[2] == 'e' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// beq.w
			if (token[1] == 'e' && token[2] == 'q' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bcc.n
			if (token[1] == 'c' && token[2] == 'c' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bgt.w
			if (token[1] == 'g' && token[2] == 't' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bcs.n
			if (token[1] == 'c' && token[2] == 's' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// ble.n
			if (token[1] == 'l' && token[2] == 'e' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bne.w
			if (token[1] == 'n' && token[2] == 'e' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bpl.n
			if (token[1] == 'p' && token[2] == 'l' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bmi.n
			if (token[1] == 'm' && token[2] == 'i' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bcs.w
			if (token[1] == 'c' && token[2] == 's' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bcc.w
			if (token[1] == 'c' && token[2] == 'c' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bhi.w
			if (token[1] == 'h' && token[2] == 'i' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bpl.n
			if (token[1] == 'p' && token[2] == 'l' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bls.w
			if (token[1] == 'l' && token[2] == 's' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			// bvc.n
			if (token[1] == 'v' && token[2] == 'c' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bvs.n
			if (token[1] == 'v' && token[2] == 's' && token[3] == '.' && token[4] == 'n')
			{
				return true;
			}

			// bmi.w
			if (token[1] == 'm' && token[2] == 'i' && token[3] == '.' && token[4] == 'w')
			{
				return true;
			}

			break;
		}
	}

	if (std::string(token, tokenLength) == "bit.16b") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "bfi") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength).find("bic") == 0) // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "bfxil") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "bsl.8b") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "bsl.16b") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "brk") // **TODO**
	{
		return false;
	}

	if (std::string(token, tokenLength) == "bkpt") // The BKPT instruction causes the processor to enter Debug state
	{
		return false;
	}

	//ocean_assert(false && "**TODO** Missing check?");
	return false;
}

inline bool DisassemblerAndroid::isAddressInstruction(const char* token, const size_t tokenLength)
{
	ocean_assert(token != nullptr && tokenLength != 0);

	switch (tokenLength)
	{
		case 3:
		{
			// adr: Load a program-relative or register-relative address into a register
			return token[0] == 'a' && token[1] == 'd' && token[2] == 'r';
		}

		case 4:
		{
			// adrl: Load a program-relative or register-relative address into a register with wide range (long edition)
			return token[0] == 'a' && token[1] == 'd' && token[2] == 'r' && token[3] == 'l';
		}

		default:
			return false;
	}
}

inline bool DisassemblerAndroid::isAddressPointerInstruction(const char* token, const size_t tokenLength)
{
	ocean_assert(token != nullptr && tokenLength != 0);

	if (tokenLength != 4)
	{
		return false;
	}

	// adrp: Address of 4KB page at a PC-relative offset (also 4K aligned)
	return token[0] == 'a' && token[1] == 'd' && token[2] == 'r' && token[3] == 'p';
}

inline bool DisassemblerAndroid::isAddInstruction(const char* token, const size_t tokenLength)
{
	ocean_assert(token != nullptr && tokenLength != 0);

	if (tokenLength != 3)
	{
		return false;
	}

	// simple add instruction:
	return token[0] == 'a' && token[1] == 'd' && token[2] == 'd';
}

#endif // FACEBOOK_OCEAN_APPLICATION_OCEAN_DEMO_MISC_APPINSPECTOR_DISASSEMBLER_ANDROID_H
