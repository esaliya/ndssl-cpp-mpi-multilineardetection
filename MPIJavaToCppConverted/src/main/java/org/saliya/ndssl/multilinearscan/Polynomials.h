#pragma once

#include <vector>
#include <iostream>

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{

				class Polynomials
				{
				public:
					static const long long DEFAULT_POLYNOMIAL_LONG = 0x375AD14A67FC7BLL;

					/// <summary>
					/// Generates a handful of irreducible polynomials of the specified degree.
					/// </summary>
					static void printIrreducibles(int const degree);

					/// <summary>
					/// Generates a large irreducible polynomial and prints out its
					/// representation in ascii and hex.
					/// </summary>
					static void printLargeIrreducible();

					/// <summary>
					/// Computes (a mod b) using synthetic division where a and b represent
					/// polynomials in GF(2^k).
					/// </summary>
					static long long mod(long long a, long long b);

					/// <summary>
					/// Returns the index of the maximum set bit. If no bits are set, returns -1.
					/// </summary>
					static int getMaxBit(long long l);

					/// <summary>
					/// Returns the value of the bit at index of the long. The right most bit is
					/// at index 0.
					/// </summary>
					static bool getBit(long long l, int index);

					/// <summary>
					/// Returns the value of the bit at index of the byte. The right most bit is
					/// at index 0.
					/// </summary>
					static bool getBit(char b, int index);

					/// <summary>
					/// Returns the value of the bit at index of the byte. The right most bit is
					/// at index 0 of the last byte in the array.
					/// </summary>
					static bool getBit(std::vector<char> &bytes, int index);

				};
			}
		}
	}
}
