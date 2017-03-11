#pragma once

#include "Arithmetic.h"
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include "stringbuilder.h"

/// <summary>
/// Credit to Jakub Lipčák jakub.lipcak@gmail.com
/// https://github.com/XLipcak/Computation-with-finite-fields
/// </summary>
namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{


				/// <summary>
				/// An immutable polynomial in the finite field GF(2^k)
				/// 
				/// Supports standard arithmetic in the field, as well as reducibility tests.
				/// </summary>
				class Polynomial : public Arithmetic<Polynomial*>, public Comparable<Polynomial*>
				{

					/// <summary>
					/// number of elements in the finite field GF(2^k) </summary>
				public:
					static BigInteger *const Q;

					/// <summary>
					/// the polynomial "x" </summary>
					static Polynomial *const X;

					/// <summary>
					/// the polynomial "1" </summary>
					static Polynomial *const ONE;

					/// <summary>
					/// a reverse comparator so that polynomials are printed out correctly </summary>
				private:
					class ReverseComparator final : public Comparator<BigInteger*>
					{
					public:
						int compare(BigInteger *o1, BigInteger *o2);
					};

					/// <summary>
					/// Constructs a polynomial using the bits from a long. Note that Java does
					/// not support unsigned longs.
					/// </summary>
				public:
					static Polynomial *createFromLong(long long l);

					static Polynomial *createFromBytes(std::vector<char> &bytes);

					/// <summary>
					/// Constructs a polynomial using the bits from an array of bytes, limiting
					/// the degree to the specified size.
					/// 
					/// We set the final degree to ensure a monic polynomial of the correct
					/// degree.
					/// </summary>
					static Polynomial *createFromBytes(std::vector<char> &bytes, int degree);

					/// <summary>
					/// Constructs a random polynomial of degree "degree"
					/// </summary>
					static Polynomial *createRandom(int degree);

					/// <summary>
					/// Constructs a random polynomial of degree "degree"
					/// </summary>
					static Polynomial *createRandom(int degree, Random *random);

					/// <summary>
					/// Finds a random irreducible polynomial of degree "degree"
					/// </summary>
					static Polynomial *createIrreducible(int degree);

					/// <summary>
					/// Finds a random irreducible polynomial of degree "degree"
					/// </summary>
					static Polynomial *createIrreducible(int degree, Random *random);

					/// <summary>
					/// An enumeration representing the reducibility of the polynomial
					/// 
					/// A polynomial p(x) in GF(2^k) is called irreducible over GF[2^k] if it is
					/// non-constant and cannot be represented as the product of two or more
					/// non-constant polynomials from GF(2^k).
					/// 
					/// http://en.wikipedia.org/wiki/Irreducible_element
					/// </summary>
				public:
					enum class Reducibility
					{
						REDUCIBLE,
						IRREDUCIBLE
					};

					/// <summary>
					/// A (sorted) set of the degrees of the terms of the polynomial. The
					/// sortedness helps quickly compute the degree as well as print out the
					/// terms in order. The O(nlogn) performance of insertions and deletions
					/// might actually hurt us, though, so we might consider moving to a HashSet
					/// </summary>
				private:
					const std::set<BigInteger*> degrees;

					/// <summary>
					/// Construct a new, empty polynomial
					/// </summary>
				public:
					Polynomial();

					/// <summary>
					/// Construct a new polynomial copy of the input argument
					/// </summary>
					Polynomial(Polynomial *p);

					/// <summary>
					/// Construct a new polynomial from a collection of degrees
					/// </summary>
				protected:
//JAVA TO C++ CONVERTER TODO TASK: Most Java annotations will not have direct C++ equivalents:
//ORIGINAL LINE: @SuppressWarnings("unchecked") protected Polynomial(java.util.TreeSet<java.math.BigInteger> degrees)
					Polynomial(std::set<BigInteger*> &degrees);

					/// <summary>
					/// Factory for create the degrees collection.
					/// </summary>
					static std::set<BigInteger*> createDegreesCollection();

					/// <summary>
					/// Factory for create the copy of current degrees collection.
					/// </summary>
//JAVA TO C++ CONVERTER TODO TASK: Most Java annotations will not have direct C++ equivalents:
//ORIGINAL LINE: @SuppressWarnings("unchecked") protected java.util.TreeSet<java.math.BigInteger> createDegreesCollectionCopy()
					virtual std::set<BigInteger*> createDegreesCollectionCopy();

					/// <summary>
					/// Returns the degree of the highest term or -1 otherwise.
					/// </summary>
				public:
					virtual BigInteger *degree();

					/// <summary>
					/// Tests if the polynomial is empty, i.e. it has no terms
					/// </summary>
					virtual bool isEmpty();

					/// <summary>
					/// Computes (this + that) in GF(2^k)
					/// </summary>
					virtual Polynomial *add(Polynomial *that);

					/// <summary>
					/// Computes (this - that) in GF(2^k)
					/// </summary>
					virtual Polynomial *subtract(Polynomial *that);

					/// <summary>
					/// Computes (this * that) in GF(2^k)
					/// </summary>
					virtual Polynomial *multiply(Polynomial *that);

					/// <summary>
					/// Computes (this & that) in GF(2^k)
					/// </summary>
					virtual Polynomial *and(Polynomial *that);

					/// <summary>
					/// Computes (this | that) in GF(2^k)
					/// </summary>
					virtual Polynomial *or(Polynomial *that);

					/// <summary>
					/// Computes (this ^ that) in GF(2^k)
					/// </summary>
					virtual Polynomial *xor(Polynomial *that);

					/// <summary>
					/// Computes (this mod that) in GF(2^k) using synthetic division
					/// </summary>
					virtual Polynomial *mod(Polynomial *that);

					/// <summary>
					/// Computes (this << shift) in GF(2^k)
					/// </summary>
					virtual Polynomial *shiftLeft(BigInteger *shift);

					/// <summary>
					/// Computes (this >> shift) in GF(2^k)
					/// </summary>
					virtual Polynomial *shiftRight(BigInteger *shift);

					/// <summary>
					/// Tests if there exists a term with degree k
					/// </summary>
					virtual bool hasDegree(BigInteger *k);

					/// <summary>
					/// Sets the coefficient of the term with degree k to 1
					/// </summary>
					virtual Polynomial *setDegree(BigInteger *k);

					/// <summary>
					/// Sets the coefficient of the term with degree k to 0
					/// </summary>
					virtual Polynomial *clearDegree(BigInteger *k);

					/// <summary>
					/// Toggles the coefficient of the term with degree k
					/// </summary>
					virtual Polynomial *toggleDegree(BigInteger *k);

					/// <summary>
					/// Computes (this^e mod m).
					/// 
					/// This algorithm requires at most this.degree() + m.degree() space.
					/// 
					/// http://en.wikipedia.org/wiki/Modular_exponentiation
					/// </summary>
					virtual Polynomial *modPow(BigInteger *e, Polynomial *m);

					/// <summary>
					/// Computes the greatest common divisor between polynomials using Euclid's
					/// algorithm
					/// 
					/// http://en.wikipedia.org/wiki/Euclids_algorithm
					/// </summary>
					virtual Polynomial *gcd(Polynomial *that);

					/// <summary>
					/// Construct a BigInteger whose value represents this polynomial. This can
					/// lose information if the degrees of the terms are larger than
					/// Integer.MAX_VALUE;
					/// </summary>
					virtual BigInteger *toBigInteger();

					/// <summary>
					/// Technically accurate but slow as hell.
					/// </summary>
					virtual BigInteger *toBigIntegerAccurate();

					/// <summary>
					/// Returns a string of hex characters representing this polynomial
					/// </summary>
					virtual std::wstring toHexString();

					/// <summary>
					/// Returns a string of digits presenting this polynomial
					/// </summary>
					virtual std::wstring toDecimalString();

					/// <summary>
					/// Returns a string of binary digits presenting this polynomial
					/// </summary>
					virtual std::wstring toBinaryString();

					/// <summary>
					/// Returns standard ascii representation of this polynomial in the form:
					/// 
					/// e.g.: x^8 + x^4 + x^3 + x + 1
					/// </summary>
					virtual std::wstring toPolynomialString();

					/// <summary>
					/// Default toString override uses the ascii representation
					/// </summary>
					virtual std::wstring toString() override;

					/// <summary>
					/// Tests the reducibility of the polynomial
					/// </summary>
					virtual bool isReducible();

					/// <summary>
					/// Tests the reducibility of the polynomial
					/// </summary>
					virtual Reducibility getReducibility();

					/// <summary>
					/// BenOr Reducibility Test
					/// 
					/// Tests and Constructions of Irreducible Polynomials over Finite Fields
					/// (1997) Shuhong Gao, Daniel Panario
					/// 
					/// http://citeseer.ist.psu.edu/cache/papers/cs/27167/http:zSzzSzwww.math.clemson.eduzSzfacultyzSzGaozSzpaperszSzGP97a.pdf/gao97tests.pdf
					/// </summary>
				protected:
					virtual Reducibility getReducibilityBenOr();

					/// <summary>
					/// Rabin's Reducibility Test
					/// 
					/// This requires the distinct prime factors of the degree, so we don't use
					/// it. But this could be faster for prime degree polynomials
					/// </summary>
					virtual Reducibility getReducibilityRabin(std::vector<int> &factors);

					/// <summary>
					/// Computes ( x^(2^p) - x ) mod f
					/// 
					/// This function is useful for computing the reducibility of the polynomial
					/// </summary>
				private:
					Polynomial *reduceExponent(int const p);

					/// <summary>
					/// Compares this polynomial to the other
					/// </summary>
				public:
					virtual int compareTo(Polynomial *o);
				};
			}
		}
	}
}
