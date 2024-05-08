/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_PROCESSOR_H
#define META_OCEAN_BASE_PROCESSOR_H

#include "ocean/base/Base.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

/**
 * Definition of individual processor instruction types.
 * @ingroup base
 */
enum ProcessorInstructions : uint32_t
{
	/// Unknown processor instruction set.
	PI_NONE = 0u,
	/// SEE instructions.
	PI_SSE = 1u << 0u,
	/// SEE2 instructions.
	PI_SSE_2 = 1u << 1u,
	/// SSE3 instructions.
	PI_SSE_3 = 1u << 2u,
	/// SSSE3 instructions.
	PI_SSSE_3 = 1u << 3u,
	/// SSE_4.1 instructions.
	PI_SSE_4_1 = 1u << 4u,
	/// SSE 4.2 instructions.
	PI_SSE_4_2 = 1u << 5u,
	/// Any SSE instructions.
	PI_SSE_ANY = PI_SSE | PI_SSE_2 | PI_SSE_3 | PI_SSSE_3 | PI_SSE_4_1 | PI_SSE_4_2,

	/// AVX instructions.
	PI_AVX = 1u << 6u,
	/// AVX2 instructions.
	PI_AVX_2 = 1u << 7u,
	/// AVX 512 instructions.
	PI_AVX_512 = 1u << 8u,
	/// Any AVX instructions.
	PI_AVX_ANY = PI_AVX | PI_AVX_2 | PI_AVX_512,

	/// NEON instructions.
	PI_NEON = 1 << 9u,
	/// Any NEON instructions.
	PI_NEON_ANY = PI_NEON,

	/// AES instructions.
	PI_AES = 1 << 10u,

	/// All SSE instructions between (including) SSE and SSE2.
	PI_GROUP_SSE_2 = PI_SSE | PI_SSE_2,
	/// All SSE instructions between (including) SSE and SSE4.1.
	PI_GROUP_SSE_4_1 = PI_SSE | PI_SSE_2 | PI_SSE_3 | PI_SSSE_3 | PI_SSE_4_1,
	/// All AVX instructions between (including) AVX and AVX2.
	PI_GROUP_AVX_2 = PI_AVX | PI_AVX_2,
	/// All AVX instructions between (including) AVX and AVX2 and SSE instructions between (including) SSE and SSE2, e.g., for processors supporting SSSE3 but not SSE3.
	PI_GROUP_AVX_2_SSE_2 = PI_GROUP_SSE_2 | PI_GROUP_AVX_2,
	/// All NEON instructions (which is currently NEON only).
	PI_GROUP_NEON = PI_NEON,
	/// All AVX instructions between (including) AVX and AVX2 and SSE instructions between (including) SSE and SSE4.1.
	PI_GROUP_AVX_2_SSE_4_1 = PI_GROUP_SSE_4_1 | PI_GROUP_AVX_2
};

/**
 * This helper class allows to determine a compile-time known boolean statement whether a set of available instruction contains (included) a specified (minimal requirement) instruction.
 * See this tutorial:
 * @code
 * template <ProcessorInstructions tHighestInstructions>
 * void function()
 * {
 *      static_assert((InstructionChecker<tHighestInstructions, SSE_2>::value), "This function needs at least SSE2 instructions");
 *
 *      if (InstructionChecker<tHighestInstructions, SSE_4_1>::value)
 *      {
 *           // place code needing (at most) SSE4.1 instructions here
 *      }
 *      else if (InstructionChecker<tHighestInstructions, SSE_2>::value)
 *      {
 *           // place an alternative code using (at most) SS2 instructions here
 *      }
 * }
 * @endcode
 * @ingroup base
 * @tparam tHighestInstructions The set of available instructions, may be any combination of instructions
 * @tparam tNecessaryInstruction The instruction that is required for a specific function (the minimal requirement), must be one specific instruction (not a set of several instructions)
 */
template <ProcessorInstructions tHighestInstructions, ProcessorInstructions tNecessaryInstruction>
class ProcessorInstructionChecker
{
	public:

		/**
		 * True, if the requested instruction is part of the provided set of instructions.
		 * Here we disable the definition to ensure that the specialized template classes are used, as otherwise the parameter 'tNecessaryInstruction' covers a combination of several instructions.
		 */
		// const static bool value = true;
};

/**
 * This class implements basic functions relating the system processor.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Processor : public Singleton<Processor>
{
	friend class Singleton<Processor>;

	public:

		/**
		 * Returns the number of available processor cores.
		 * If an explicit number of  processors has been forced by the user, the user defined number will be returned.
		 * @return Number of processor cores
		 * @see realCores(), forceCores().
		 */
		inline unsigned int cores() const;

		/**
		 * Returns the supported instruction set of the processor.
		 * If an explicit instruction set has been forced by the user, the user defined instruction set will be returned.
		 * @return Instruction set of the processor
		 * @see realInstructions(), forceInstructions().
		 */
		inline ProcessorInstructions instructions();

		/**
		 * Forces a user defined number of processor cores.
		 * The forced number will be returned instead of the real cores using the cores() function.
		 * @param cores Number of cores to be forced, 0 to remove the previously forced core number
		 * @return True, if succeeded
		 * @see cores().
		 */
		bool forceCores(const unsigned int cores);

		/**
		 * Forces a user-defined processor instruction set.
		 * The forced instruction set will be returned instead of the real instruction set using the instructions() function.
		 * @param instructions The instruction set to be forced, -1 to remove the previously forced instruction set
		 * @return True, if succeeded
		 * @see instructions().
		 */
		bool forceInstructions(const ProcessorInstructions instructions);

		/**
		 * Returns the processor's brand.
		 * @return The processor's brand
		 */
		static std::string brand();

		/**
		 * Returns the number of available processor cores currently detectable.
		 * @return Number of processor cores
		 * @see cores().
		 */
		static unsigned int realCores();

		/**
		 * Returns the supported instruction set of the processor.
		 * @return The supported set of instructions
		 * @see instructions().
		 */
		static ProcessorInstructions realInstructions();

		/**
		 * Translates a set of processor instructions to a readable string.
		 * @param instructions The instructions to be translated
		 * @return The resulting string containing the instruction names, 'No SIMD Instructions' if no instruction is specified
		 */
		static std::string translateInstructions(const ProcessorInstructions instructions);

		/**
		 * Returns the best group of instructions value for a set of given processor instructions.
		 * The function may return the following groups in the following order: PI_GROUP_AVX_2_SSE_4_1, PI_GROUP_SSE_4_1, PI_GROUP_AVX_2_SSE_2, PI_GROUP_AVX_2, PI_GROUP_SSE_2, PI_GROUP_NEON.
		 * @param instructions The set of instructions for which the best group will be returned
		 * @return The best group of instructions, PI_NONE if no group is matching
		 * @tparam tIndependentOfBinary True, to return the best group without checking the binaries capabilities; False, to return groups which are supported by the current binary only
		 */
		template <bool tIndependentOfBinary>
		static inline ProcessorInstructions bestInstructionGroup(const ProcessorInstructions instructions);

		/**
		 * Returns whether the processor/system is using the little endian convention (like e.g., x86) or whether the big endian convention is used.
		 * @return True, if the little endian convention is used
		 */
		static inline bool isLittleEndian();

	private:

		/**
		 * Constructs a new processor object.
		 */
		Processor();

#if defined(__APPLE__)

		/**
		 * Returns the number of available processor cores currently detectable.
		 * @return Number of processor cores
		 * @see realCores().
		 */
		static unsigned int realCoresApple();

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1

		/**
		 * Returns the device name of the Apple iOS device.
		 * @return The device name
		 */
		static std::string deviceModelAppleIOS();

	#endif // TARGET_OS_IPHONE==1

#endif // __APPLE__

		/**
		 * Returns invalid processor instructions.
		 * @return Invalid instructions
		 */
		static constexpr ProcessorInstructions invalidProcessorInstructions();

	private:

		/// Explicitly forced number of processor cores.
		unsigned int forcedCores_ = 0u;

		/// Explicitly forced CPU instructions.
		ProcessorInstructions forcedProcessorInstructions_ = invalidProcessorInstructions();

		/// The real instructions of the processor.
		ProcessorInstructions processorInstructions_ = invalidProcessorInstructions();

		/// The lock of the processor class.
		mutable Lock lock_;
};

inline unsigned int Processor::cores() const
{
	const ScopedLock scopedLock(lock_);

	if (forcedCores_ > 0u)
	{
		return forcedCores_;
	}

	return realCores();
}

inline ProcessorInstructions Processor::instructions()
{
	const ScopedLock scopedLock(lock_);

	if (forcedProcessorInstructions_ != invalidProcessorInstructions())
	{
		return forcedProcessorInstructions_;
	}

	ocean_assert(processorInstructions_ != invalidProcessorInstructions());
	return processorInstructions_;
}

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSE>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSE;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSE_2>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSE_2;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSE_3>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSE_3;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSSE_3>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSSE_3;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSE_4_1>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSE_4_1;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_SSE_4_2>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_SSE_ANY) >= PI_SSE_4_2;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_AVX>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_AVX_ANY) >= PI_AVX;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_AVX_2>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_AVX_ANY) >= PI_AVX_2;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_AVX_512>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_AVX_ANY) >= PI_AVX_512;
};

/**
 * Specialization for one specific instruction.
 * @see ProcessorInstructionChecker.
 * @ingroup base
 */
template <ProcessorInstructions tHighestInstructions>
class ProcessorInstructionChecker<tHighestInstructions, PI_NEON>
{
	public:

		/// True, if the requested instruction is part of the provided set of instructions.
		constexpr static bool value = (tHighestInstructions & PI_NEON_ANY) >= PI_NEON;
};

template <bool tIndependentOfBinary>
inline ProcessorInstructions Processor::bestInstructionGroup(const ProcessorInstructions instructions)
{
	if ((instructions & PI_GROUP_AVX_2_SSE_4_1) == PI_GROUP_AVX_2_SSE_4_1)
	{
		return PI_GROUP_AVX_2_SSE_4_1;
	}

	if ((instructions & PI_GROUP_SSE_4_1) == PI_GROUP_SSE_4_1)
	{
		return PI_GROUP_SSE_4_1;
	}

	if ((instructions & PI_GROUP_AVX_2_SSE_2) == PI_GROUP_AVX_2_SSE_2)
	{
		return PI_GROUP_AVX_2_SSE_2;
	}

	if ((instructions & PI_GROUP_AVX_2) == PI_GROUP_AVX_2)
	{
		return PI_GROUP_AVX_2;
	}

	if ((instructions & PI_GROUP_SSE_2) == PI_GROUP_SSE_2)
	{
		return PI_GROUP_SSE_2;
	}

	if ((instructions & PI_GROUP_NEON) == PI_GROUP_NEON)
	{
		return PI_GROUP_NEON;
	}

	return PI_NONE;
}

template <>
inline ProcessorInstructions Processor::bestInstructionGroup<false>(const ProcessorInstructions instructions)
{
#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41 && defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 20
	if ((instructions & PI_GROUP_AVX_2_SSE_4_1) == PI_GROUP_AVX_2_SSE_4_1)
	{
		return PI_GROUP_AVX_2_SSE_4_1;
	}
#endif

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41
	if ((instructions & PI_GROUP_SSE_4_1) == PI_GROUP_SSE_4_1)
	{
		return PI_GROUP_SSE_4_1;
	}
#endif

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20 && defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 20
	if ((instructions & PI_GROUP_AVX_2_SSE_2) == PI_GROUP_AVX_2_SSE_2)
	{
		return PI_GROUP_AVX_2_SSE_2;
	}
#endif

#if defined(OCEAN_HARDWARE_AVX_VERSION) && OCEAN_HARDWARE_AVX_VERSION >= 20
	if ((instructions & PI_GROUP_AVX_2) == PI_GROUP_AVX_2)
	{
		return PI_GROUP_AVX_2;
	}
#endif

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 20
	if ((instructions & PI_GROUP_SSE_2) == PI_GROUP_SSE_2)
	{
		return PI_GROUP_SSE_2;
	}
#endif

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	if ((instructions & PI_GROUP_NEON) == PI_GROUP_NEON)
	{
		return PI_GROUP_NEON;
	}
#endif

	OCEAN_SUPPRESS_UNUSED_WARNING(instructions);

	return PI_NONE;
}

inline bool Processor::isLittleEndian()
{
	const int32_t littleEndianValue = 1;

	const bool result = (*(int8_t*)(&littleEndianValue)) == int8_t(1);

#ifdef OCEAN_LITTLE_ENDIAN
	ocean_assert(result);
#else
	ocean_assert(!result);
#endif

	return result;
}

constexpr ProcessorInstructions Processor::invalidProcessorInstructions()
{
	return ProcessorInstructions(-1);
}

}

#endif // META_OCEAN_BASE_PROCESSOR_H
