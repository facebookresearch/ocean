/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_MEDIAN_H
#define META_OCEAN_BASE_MEDIAN_H

#include "ocean/base/Base.h"
#include "ocean/base/Memory.h"

namespace Ocean
{

/**
 * This class implements a median determination value.
 * @ingroup base
 */
class Median
{
	public:

		/**
		 * Returns the median of two given values (the lower of both).
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param v0 First value
		 * @param v1 Second value
		 * @return Resulting median
		 * @tparam T Data type of the data elements
		 */
		template <typename T>
		static inline T median2(const T& v0, const T& v1);

		/**
		 * Returns the median of three given values.
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param v0 First value
		 * @param v1 Second value
		 * @param v2 Third value
		 * @return Resulting median
		 * @tparam T Data type of the data elements
		 */
		template <typename T>
		static inline T median3(const T& v0, const T& v1, const T& v2);

		/**
		 * Returns the median of four given values (the second smallest value).
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param v0 First value
		 * @param v1 Second value
		 * @param v2 Third value
		 * @param v3 Fourth value
		 * @return Resulting median
		 * @tparam T Data type of the data elements
		 */
		template <typename T>
		static inline T median4(const T& v0, const T& v1, const T& v2, const T& v3);

		/**
		 * Returns the median of five given values.
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param v0 First value
		 * @param v1 Second value
		 * @param v2 Third value
		 * @param v3 Fourth value
		 * @param v4 Fifth value
		 * @return Resulting median
		 * @tparam T Data type of the data elements
		 */
		template <typename T>
		static inline T median5(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4);

		/**
		 * Returns the median value in a given data array.
		 * If the given number is even, the median value is selected left from the exact center.<br>
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param values Array with values that will be reordered during the execution, must be valid
		 * @param number Number of values, with range [1, infinity)
		 * @return The median value out of 'values'
		 * @tparam T Type of the data for that the median has to be determined
		 */
		template <typename T>
		static T median(T* values, const size_t number);

		/**
		 * Returns the median value in a given data array.
		 * If the given number is even, the median value is selected left from the exact center.<br>
		 * Beware: In the case one of the given values is not a number (nan) the result may be arbitrary as the comparison operator is not defined for such values.
		 * @param values Array with values from which the median will be determined, must be valid
		 * @param number Number of values, with range [1, infinity)
		 * @return The median value out of 'values'
		 * @tparam T Type of the data for that the median has to be determined
		 */
		template <typename T>
		static T constMedian(const T* values, const size_t number);
};

template <typename T>
inline T Median::median2(const T& v0, const T& v1)
{
	return v0 < v1 ? v0 : v1;
}

template <typename T>
inline T Median::median3(const T& v0, const T& v1, const T& v2)
{
	if (v0 < v1)
	{
		// [ v0 < v1 ]

		if (v2 < v0)
		{
			// [ v2 < v0 < v1]
			return v0;
		}
		else // v2 >= v0
		{
			// [ v0 < v1,v2 ]

			if (v1 < v2)
				return v1;
			else
				return v2;
		}
	}
	else // v0 >= v1
	{
		// [ v1 < v0 ]

		if (v2 < v1)
		{
			// [ v2 < v1 < v0 ]

			return v1;
		}
		else // v2 >= v1
		{
			// [ v1 < v0,v2 ]

			if (v0 < v2)
				return v0;
			else
				return v2;
		}
	}
}

template <typename T>
inline T Median::median4(const T& v0, const T& v1, const T& v2, const T& v3)
{
	// v0, v1, v2, v3
	if (v0 < v1)
	{
		// v0 < v1 | v2, v3

		if (v0 < v2)
		{
			// v0 < v1,v2 | v3

			if (v0 < v3)
			{
				// v0 < v1,v2,v3

				if (v1 < v2)
				{
					// v0 < v1 < v2 | v0 < v3

					if (v2 < v3)
					{
						// v0 < v1 < v2 < v3
						return v1;
					}
					else // v3 < v2
					{
						// v0 < v1 < v2 | v0 < v3 | v3 < v2

						if (v3 < v1)
						{
							// v0 < v3 < v1 < v2
							return v3;
						}
						else // v1 < v3
						{
							// v0 < v1 < v3 < v2
							return v1;
						}
					}
				}
				else // v2 < v1
				{
					// v0 < v1,v2,v3 | v2 < v1

					if (v2 < v3)
					{
						// v0 < v1,v2,v3 | v2 < v1,v3
						return v2;
					}
					else // v3 < v2
					{
						// v0 < v1,v2,v3 | v3 < v2 < v1
						return v3;
					}
				}
			}
			else // v3 < v0
			{
				// v3 < v0 < v1,v2
				return v0;
			}
		}
		else // v2 < v0
		{
			// v2 < v0 < v1 | v3

			if (v3 < v2)
			{
				// v3 < v2 < v0 < v1
				return v2;
			}
			else // v2 < v3
			{
				// v2 < v0 < v1 | v2 < v3

				if (v0 < v3)
				{
					// v2 < v0 < v1, v3
					return v0;
				}
				else // v3 < v0
				{
					// v2 < v3 < v0 < v1
					return v3;
				}
			}
		}
	}
	else // v1 < v0
	{
		// v1 < v0 | v2, v3

		if (v2 < v1)
		{
			// v2 < v1 < v0 | v3

			if (v3 < v2)
			{
				// v3 < v2 < v1 < v0
				return v2;
			}
			else // v2 < v3
			{
				// v2 < v1 < v0 | v2 < v3

				if (v1 < v3)
				{
					// v2 < v1 < v0,v3
					return v1;
				}
				else // v3 < v1
				{
					// v2 < v3 < v1 < v0
					return v3;
				}
			}
		}
		else // v1 < v2
		{
			// v1 < v0,v2 | v3

			if (v0 < v2)
			{
				// v1 < v0 < v2 | v3

				if (v3 < v1)
				{
					// v3 < v1 < v0 < v2
					return v1;
				}
				else // v1 < v3
				{
					// v1 < v0 < v2 | v1 < v3

					if (v0 < v3)
					{
						// v1 < v0 < v2,v3
						return v0;
					}
					else // v3 < v0
					{
						// v1 < v3 < v0 < v2
						return v3;
					}
				}
			}
			else // v2 < v0
			{
				// v1 < v2 < v0 | v3

				if (v3 < v1)
				{
					// v3 < v1 < v2 < v0
					return v1;
				}
				else // v1 < v3
				{
					// v1 < v2 < v0 | v1 < v3

					if (v2 < v3)
					{
						// v1 < v2 < v0, v3
						return v2;
					}
					else // v3 < v2
					{
						// v1 < v3 < v2 < v0
						return v3;
					}
				}
			}
		}
	}
}

template <typename T>
inline T Median::median5(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4)
{
	// v0, v1, v2, v3, v4
	if (v0 < v1)
	{
		// v0 < v1 | v2, v3, v4

		if (v0 < v2)
		{
			// v0 < v1,v2 | v3, v4

			if (v0 < v3)
			{
				// v0 < v1,v2,v3 | v4

				if (v0 < v4)
				{
					// v0 < v1,v2,v3,v4

					if (v1 < v2)
					{
						// v0 < v1 < v2 | v0 < v3,v4

						if (v2 < v3)
						{
							// v0 < v1 < v2 < v3 | v0 < v4

							if (v3 < v4)
							{
								// v0 < v1 < v2 < v3 < v4
								return v2;
							}
							else // v4 < v3
							{
								// v0 < v1 < v2 < v3 | v0 < v4 < v3

								if (v4 < v2)
								{
									// v0 < v1 < v2 < v3 | v0 < v4 < v2

									if (v4 < v1)
									{
										// v0 < v4 < v1 < v2 < v3
										return v1;
									}
									else // v1 < v4
									{
										// v0 < v1 < v4 < v2 < v3
										return v4;
									}
								}
								else // v2 < v4
								{
									// v0 < v1 < v2 < v4 < v3
									return v2;
								}
							}
						}
						else // v3 < v2
						{
							// v0 < v1 < v2 | v0 < v3,v4 | v3 < v2

							if (v3 < v1)
							{
								// v0 < v3 < v1 < v2 | v0 < v4

								if (v4 < v3)
								{
									// v0 < v4 < v3 < v1 < v2
									return v3;
								}
								else // v3 < v4
								{
									// v0 < v3 < v1 < v2 | v3 < v4

									if (v4 < v1)
									{
										// v0 < v3 < v4 < v1 < v2
										return v4;
									}
									else // v1 < v4
									{
										// v0 < v3 < v1 < v2,v4
										return v1;
									}
								}
							}
							else // v1 < v3
							{
								// v0 < v1 < v3 < v2 | v0 < v4

								if (v1 < v4)
								{
									// v0 < v1 < v3 < v2 | v1 < v4

									if (v3 < v4)
									{
										// v0 < v1 < v3 < v2, v4
										return v3;
									}
									else // v4 < v3
									{
										// v0 < v1 < v4 < v3 < v2
										return v4;
									}
								}
								else // v4 < v1
								{
									// v0 < v4 < v1 < v3 < v2
									return v1;
								}
							}
						}
					}
					else // v2 < v1
					{
						// v0 < v1,v2,v3,v4 | v2 < v1

						if (v2 < v3)
						{
							// v0 < v1,v2,v3,v4 | v2 < v1,v3

							if (v2 < v4)
							{
								// v0 < v2 < v1,v3,v4

								if (v1 < v3)
								{
									// v0 < v2 < v1,v3,v4 | v1 < v3

									if (v1 < v4)
									{
										// v0 < v2 < v1 < v3,v4
										return v1;
									}
									else // v4 < v1
									{
										// v0 < v2 < v4 < v1 < v3
										return v4;
									}
								}
								else // v3 < v1
								{
									// v0 < v2 < v1,v3,v4 | v3 < v1

									if (v3 < v4)
									{
										// v0 < v2 < v3 < v1,v4
										return v3;
									}
									else // v4 < v3
									{
										// v0 < v2 < v4 < v3 < v1
										return v4;
									}
								}
							}
							else // v4 < v2
							{
								// v0 < v4 < v2 < v1,v3
								return v2;
							}
						}
						else // v3 < v2
						{
							// v0 < v1,v2,v3,v4 | v3 < v2 < v1

							if (v3 < v4)
							{
								// v0 < v3 < v2 < v1 | v3 < v4

								if (v2 < v4)
								{
									// v0 < v3 < v2 < v1,v4
									return v2;
								}
								else // v4 < v2
								{
									// v0 < v3 < v4 < v2 < v1
									return v4;
								}
							}
							else // v4 < v3
							{
								// v0 < v4 < v3 < v2 < v1
								return v3;
							}
						}
					}
				}
				else // v4 < v0
				{
					// v4 < v0 < v1,v2,v3

					if (v1 < v2)
					{
						// v4 < v0 < v1,v2,v3 | v1 < v2

						if (v1 < v3)
						{
							// v4 < v0 < v1 < v2,v3
							return v1;
						}
						else // v3 < v1
						{
							// v4 < v0 < v3 < v1 < v2
							return v3;
						}
					}
					else // v2 < v1
					{
						// v4 < v0 < v1,v2,v3 | v2 < v1

						if (v2 < v3)
						{
							// v4 < v0 < v2 < v1,v3
							return v2;
						}
						else // v3 < v2
						{
							// v4 < v0 < v3 < v2 < v1
							return v3;
						}
					}
				}
			}
			else // v3 < v0
			{
				// v3 < v0 < v1,v2 | v4

				if (v0 < v4)
				{
					// v3 < v0 < v1,v2,v4

					if (v1 < v2)
					{
						// v3 < v0 < v1,v2,v4 | v1 < v2

						if (v1 < v4)
						{
							// v3 < v0 < v1 < v2,v4
							return v1;
						}
						else // v4 < v1
						{
							// v3 < v0 < v4 < v1 < v2
							return v4;
						}
					}
					else // v2 < v1
					{
						// v3 < v0 < v1,v2,v4 | v2 < v1

						if (v2 < v4)
						{
							// v3 < v0 < v2 < v1,v4
							return v2;
						}
						else // v4 < v2
						{
							// v3 < v0 < v4 < v2 < v1
							return v4;
						}
					}
				}
				else // v4 < v0
				{
					// v3,v4 < v0 < v1,v2
					return v0;
				}
			}
		}
		else // v2 < v0
		{
			// v2 < v0 < v1 | v3, v4

			if (v3 < v2)
			{
				// v3 < v2 < v0 < v1 | v4

				if (v0 < v4)
				{
					// v3 < v2 < v0 < v1,v4
					return v0;
				}
				else // v4 < v0
				{
					// v3 < v2 < v0 < v1 | v4 < v0

					if (v4 < v2)
					{
						// v3,v4 < v2 < v0 < v1
						return v2;

					}
					else // v2 < v4
					{
						// v3 < v2 < v4 < v0 < v1
						return v4;
					}
				}
			}
			else // v2 < v3
			{
				// v2 < v0 < v1 | v2 < v3 | v4

				if (v4 < v2)
				{
					// v4 < v2 < v0 < v1 | v2 < v3

					if (v0 < v3)
					{
						// v4 < v2 < v0 < v1, v3
						return v0;
					}
					else // v3 < v0
					{
						// v4 < v2 < v3 < v0 < v1
						return v3;
					}
				}
				else // v2 < v4
				{
					// v2 < v0 < v1 | v2 < v3, v4

					if (v4 < v0)
					{
						// v2 < v4 < v0 < v1 | v2 < v3

						if (v4 < v3)
						{
							// v2 < v4 < v0 < v1 | v4 < v3

							if (v0 < v3)
							{
								// v2 < v4 < v0 < v1,v3
								return v0;
							}
							else // v3 < v0
							{
								// v2 < v4 < v3 < v0 < v1
								return v3;
							}
						}
						else // v3 < v4
						{
							// v2 < v3 < v4 < v0 < v1
							return v4;
						}
					}
					else // v0 < v4
					{
						// v2 < v0 < v1,v4 | v2 < v3

						if (v0 < v3)
						{
							// v2 < v0 < v1,v4,v3

							if (v1 < v4)
							{
								// v2 < v0 < v1,v4,v3 | v1 < v4

								if (v1 < v3)
								{
									// v2 < v0 < v1 < v3,v4
									return v1;
								}
								else // v3 < v1
								{
									// v2 < v0 < v3 < v1 < v4
									return v3;
								}
							}
							else // v4 < v1
							{
								// v2 < v0 < v1,v4,v3 | v4 < v1

								if (v4 < v3)
								{
									// v2 < v0 < v4 < v1,v3
									return v4;
								}
								else // v3 < v4
								{
									// v2 < v0 < v3 < v4 < v1
									return v3;
								}
							}
						}
						else // v3 < v0
						{
							// v2 < v3 < v0 < v1,v4
							return v0;
						}
					}
				}
			}
		}
	}
	else // v1 < v0
	{
		// v1 < v0 | v2, v3, v4

		if (v2 < v1)
		{
			// v2 < v1 < v0 | v3, v4

			if (v3 < v2)
			{
				// v3 < v2 < v1 < v0 | v4

				if (v4 < v3)
				{
					// v4 < v3 < v2 < v1 < v0
					return v2;
				}
				else // v3 < v4
				{
					// v3 < v2 < v1 < v0 | v3 < v4

					if (v2 < v4)
					{
						// v3 < v2 < v1 < v0 | v2 < v4

						if (v1 < v4)
						{
							// v3 < v2 < v1 < v0,v4
							return v1;
						}
						else // v4 < v1
						{
							// v3 < v2 < v4 < v1 < v0
							return v4;
						}
					}
					else // v4 < v2
					{
						// v3 < v4 < v2 < v1 < v0
						return v2;
					}
				}
			}
			else // v2 < v3
			{
				// v2 < v1 < v0 | v2 < v3 | v4

				if (v4 < v2)
				{
					// v4 < v2 < v1 < v0 | v2 < v3

					if (v1 < v3)
					{
						// v4 < v2 < v1 < v0,v3
						return v1;
					}
					else // v3 < v1
					{
						// v4 < v2 < v3 < v1 < v0
						return v3;
					}
				}
				else // v2 < v4
				{
					// v2 < v1 < v0 | v2 < v3,v4

					if (v1 < v3)
					{
						// v2 < v1 < v0,v3 | v2 < v4

						if (v1 < v4)
						{
							// v2 < v1 < v0,v3,v4

							if (v0 < v3)
							{
								// v2 < v1 < v0,v3,v4 | v0 < v3

								if (v0 < v4)
								{
									// v2 < v1 < v0 < v3,v4
									return v0;
								}
								else // v4 < v0
								{
									// v2 < v1 < v4 < v0 < v3
									return v4;
								}
							}
							else // v3 < v0
							{
								// v2 < v1 < v0,v3,v4 | v3 < v0

								if (v3 < v4)
								{
									// v2 < v1 < v3 < v0,v4
									return v3;
								}
								else // v4 < v3
								{
									// v2 < v1 < v4 < v3 < v0
									return v4;
								}
							}
						}
						else // v4 < v1
						{
							// v2 < v4 < v1 < v0,v3
							return v1;
						}
					}
					else // v3 < v1
					{
						// v2 < v3 < v1 < v0 | v2 < v4

						if (v3 < v4)
						{
							// v2 < v3 < v1 < v0 | v3 < v4

							if (v1 < v4)
							{
								// v2 < v3 < v1 < v0,v4
								return v1;
							}
							else // v4 < v1
							{
								// v2 < v3 < v4 < v1 < v0
								return v4;
							}
						}
						else // v4 < v3
						{
							// v2 < v4 < v3 < v1 < v0
							return v3;
						}
					}
				}
			}
		}
		else // v1 < v2
		{
			// v1 < v0,v2 | v3, v4

			if (v0 < v2)
			{
				// v1 < v0 < v2 | v3, v4

				if (v3 < v1)
				{
					// v3 < v1 < v0 < v2 | v4

					if (v4 < v3)
					{
						// v4 < v3 < v1 < v0 < v2
						return v1;
					}
					else // v3 < v4
					{
						// v3 < v1 < v0 < v2 | v3 < v4

						if (v1 < v4)
						{
							// v3 < v1 < v0 < v2 | v1 < v4

							if (v0 < v4)
							{
								// v3 < v1 < v0 < v2,v4
								return v0;
							}
							else // v4 < v0
							{
								// v3 < v1 < v4 < v0 < v2
								return v4;
							}
						}
						else // v4 < v1
						{
							// v3 < v4 < v1 < v0 < v2
							return v1;
						}
					}
				}
				else // v1 < v3
				{
					// v1 < v0 < v2 | v1 < v3 | v4

					if (v4 < v1)
					{
						// v4 < v1 < v0 < v2 | v1 < v3

						if (v0 < v3)
						{
							// v4 < v1 < v0 < v2,v3
							return v0;
						}
						else // v3 < v0
						{
							// v4 < v1 < v3 < v0 < v2
							return v3;
						}
					}
					else // v1 < v4
					{
						// v1 < v0 < v2 | v1 < v3,v4

						if (v0 < v3)
						{
							// v1 < v0 < v2,v3 | v1 < v4

							if (v0 < v4)
							{
								// v1 < v0 < v2,v3,v4

								if (v2 < v3)
								{
									// v1 < v0 < v2,v3,v4 | v2 < v3

									if (v2 < v4)
									{
										// v1 < v0 < v2 < v3,v4
										return v2;
									}
									else // v4 < v2
									{
										// v1 < v0 < v4 < v2 < v3
										return v4;
									}
								}
								else // v3 < v2
								{
									// v1 < v0 < v2,v3,v4 | v3 < v2

									if (v3 < v4)
									{
										// v1 < v0 < v3 < v2,v4
										return v3;
									}
									else // v4 < v3
									{
										// v1 < v0 < v4 < v3 < v2
										return v4;
									}
								}
							}
							else // v4 < v0
							{
								// v1 < v4 < v0 < v2,v3
								return v0;
							}
						}
						else // v3 < v0
						{
							// v1 < v3 < v0 < v2 | v1 < v4

							if (v3 < v4)
							{
								// v1 < v3 < v0 < v2 | v3 < v4

								if (v0 < v4)
								{
									// v1 < v3 < v0 < v2, v4
									return v0;
								}
								else // v4 < v0
								{
									// v1 < v3 < v4 < v0 < v2
									return v4;
								}
							}
							else // v4 < v3
							{
								// v1 < v4 < v3 < v0 < v2
								return v3;
							}
						}
					}
				}
			}
			else // v2 < v0
			{
				// v1 < v2 < v0 | v3, v4

				if (v3 < v1)
				{
					// v3 < v1 < v2 < v0 | v4

					if (v4 < v3)
					{
						// v4 < v3 < v1 < v2 < v0
						return v1;
					}
					else // v3 < v4
					{
						// v3 < v1 < v2 < v0 | v3 < v4

						if (v1 < v4)
						{
							// v3 < v1 < v2 < v0 | v1 < v4

							if (v2 < v4)
							{
								// v3 < v1 < v2 < v0,v4
								return v2;
							}
							else // v4 < v2
							{
								// v3 < v1 < v4 < v2 < v0
								return v4;
							}
						}
						else // v4 < v1
						{
							// v3 < v4 < v1 < v2 < v0
							return v1;
						}
					}
				}
				else // v1 < v3
				{
					// v1 < v2 < v0 | v1 < v3 | v4

					if (v4 < v1)
					{
						// v4 < v1 < v2 < v0 | v1 < v3

						if (v2 < v3)
						{
							// v4 < v1 < v2 < v0, v3
							return v2;
						}
						else // v3 < v2
						{
							// v4 < v1 < v3 < v2 < v0
							return v3;
						}
					}
					else // v1 < v4
					{
						// v1 < v2 < v0 | v1 < v3, v4

						if (v2 < v3)
						{
							// v1 < v2 < v0,v3 | v1 < v4

							if (v2 < v4)
							{
								// v1 < v2 < v0,v3,v4

								if (v0 < v3)
								{
									// v1 < v2 < v0,v3,v4 | v0 < v3

									if (v0 < v4)
									{
										// v1 < v2 < v0 < v3,v4
										return v0;
									}
									else // v4 < v0
									{
										// v1 < v2 < v4 < v0 < v3
										return v4;
									}
								}
								else // v3 < v0
								{
									// v1 < v2 < v0,v3,v4 | v3 < v0

									if (v3 < v4)
									{
										// v1 < v2 < v3 < v0,v4
										return v3;
									}
									else // v4 < v3
									{
										// v1 < v2 < v4 < v3 < v0
										return v4;
									}
								}
							}
							else // v4 < v2
							{
								// v1 < v4 < v2 < v0,v3
								return v2;
							}
						}
						else // v3 < v2
						{
							// v1 < v3 < v2 < v0 | v1 < v4

							if (v3 < v4)
							{
								// v1 < v3 < v2 < v0 | v3 < v4

								if (v2 < v4)
								{
									// v1 < v3 < v2 < v0, v4
									return v2;
								}
								else // v4 < v2
								{
									// v1 < v3 < v4 < v2 < v0
									return v4;
								}
							}
							else // v4 < v3
							{
								// v1 < v4 < v3 < v2 < v0
								return v3;
							}
						}
					}
				}
			}
		}
	}
}

template <typename T>
T Median::median(T* values, const size_t number)
{
	ocean_assert(values && number > 0);

	size_t low = 0;
	size_t high = number - 1;
	size_t median = (low + high) >> 1;

	size_t ll, hh, middle;

	while (true)
	{
		// only one element
		if (high <= low)
			return values[median];

		// only two elements
		if (high == low + 1)
		{
			if (values[low] > values[high])
				std::swap(values[low], values[high]);

			return values[median];
		}

		middle = (low + high) / 2;

		if (values[middle] > values[high])
			std::swap(values[middle], values[high]);

		if (values[low] > values[high])
			std::swap(values[low], values[high]);

		if (values[middle] > values[low])
			std::swap(values[middle], values[low]);

		std::swap(values[middle], values[low + 1]);

		ll = low + 1;
		hh = high;

		while (true)
		{
			while (ll < number && values[low] > values[++ll]); // we need the explicit out-of-range check for not-a-number (nan) elements
			
			while (hh > 0 && values[--hh] > values[low]);

			if (hh < ll)
				break;

			std::swap(values[ll], values[hh]);
		}

		std::swap(values[low], values[hh]);

		if (hh <= median)
			low = ll;

		if (hh >= median)
			high = hh - 1;
	}
}

template <typename T>
T Median::constMedian(const T* values, const size_t number)
{
	ocean_assert(values != nullptr && number > 0);

	if (number == 0)
		return 1;

	Memory memory = Memory::create<T>(number);

	if (memory.data() == nullptr)
	{
		ocean_assert(false && "Out of memory");
		return 1;
	}

	memcpy(memory.data<T>(), values, sizeof(T) * number);
	const T result = median(memory.data<T>(), number);

	return result;
}

}

#endif // META_OCEAN_BASE_MEDIAN_H
