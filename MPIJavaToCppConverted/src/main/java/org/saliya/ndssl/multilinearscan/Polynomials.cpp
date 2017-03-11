#include "Polynomials.h"
#include "Polynomial.h"

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{

				void Polynomials::printIrreducibles(int const degree)
				{
					for (int i = 0; i < 10; i++)
					{
						Polynomial *p = Polynomial::createIrreducible(degree);
						std::wcout << p->toPolynomialString() << std::endl;
					}
				}

				void Polynomials::printLargeIrreducible()
				{
					Polynomial *p = Polynomial::createIrreducible(127);
					std::wcout << p->toPolynomialString() << std::endl;
					std::wcout << p->toHexString() << std::endl;
				}

				long long Polynomials::mod(long long a, long long b)
				{
					int ma = getMaxBit(a);
					int mb = getMaxBit(b);
					for (int i = ma - mb; i >= 0; i--)
					{
						if (getBit(a, (i + mb)))
						{
							long long shifted = b << i;
							a = a ^ shifted;
						}
					}
					return a;
				}

				int Polynomials::getMaxBit(long long l)
				{
					for (int i = 64 - 1; i >= 0; i--)
					{
						if (getBit(l, i))
						{
							return i;
						}
					}
					return -1;
				}

				bool Polynomials::getBit(long long l, int index)
				{
					return (((l >> index) & 1) == 1);
				}

				bool Polynomials::getBit(char b, int index)
				{
					return (((b >> index) & 1) == 1);
				}

				bool Polynomials::getBit(std::vector<char> &bytes, int index)
				{
					// byte array index
					const int aidx = bytes.size() - 1 - (index / 8);
					// bit index
					const int bidx = index % 8;
					// byte
					const char b = bytes[aidx];
					// bit
					return getBit(b, bidx);
				}
			}
		}
	}
}
