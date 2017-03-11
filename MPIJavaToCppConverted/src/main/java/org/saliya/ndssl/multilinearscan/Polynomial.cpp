#include "Polynomial.h"
#include "Polynomials.h"

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{

java::math::BigInteger *const Polynomial::Q = static_cast<java::math::BigInteger>(2LL);
Polynomial *const Polynomial::X = Polynomial::createFromLong(2LL);
Polynomial *const Polynomial::ONE = Polynomial::createFromLong(1LL);

				int Polynomial::ReverseComparator::compare(BigInteger *o1, BigInteger *o2)
				{
					return -1 * o1->compare(o2);
				}

				Polynomial *Polynomial::createFromLong(long long l)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					for (int i = 0; i < 64; i++)
					{
						if (((l >> i) & 1) == 1)
						{
							dgrs.insert(static_cast<BigInteger>(i));
						}
					}
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::createFromBytes(std::vector<char> &bytes)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					int degree = 0;
					for (int i = bytes.size() - 1; i >= 0; i--)
					{
						for (int j = 0; j < 8; j++)
						{
							if ((((bytes[i] >> j) & 1) == 1))
							{
								dgrs.insert(static_cast<BigInteger>(degree));
							}
							degree++;
						}
					}
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::createFromBytes(std::vector<char> &bytes, int degree)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					for (int i = 0; i < degree; i++)
					{
						if (Polynomials::getBit(bytes, i))
						{
							dgrs.insert(static_cast<BigInteger>(i));
						}
					}
					dgrs.insert(static_cast<BigInteger>(degree));
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::createRandom(int degree)
				{
					Random *random = new Random();
					std::vector<char> bytes((degree / 8) + 1);
					random->nextBytes(bytes);
					return createFromBytes(bytes, degree);
				}

				Polynomial *Polynomial::createRandom(int degree, Random *random)
				{
					std::vector<char> bytes((degree / 8) + 1);
					random->nextBytes(bytes);
					return createFromBytes(bytes, degree);
				}

				Polynomial *Polynomial::createIrreducible(int degree)
				{
					while (true)
					{
						Polynomial *p = createRandom(degree);
						if (p->getReducibility() == Reducibility::IRREDUCIBLE)
						{
							return p;
						}
					}
				}

				Polynomial *Polynomial::createIrreducible(int degree, Random *random)
				{
					while (true)
					{
						Polynomial *p = createRandom(degree, random);
						if (p->getReducibility() == Reducibility::IRREDUCIBLE)
						{
							return p;
						}
					}
				}

				Polynomial::Polynomial() : degrees(createDegreesCollection())
				{
				}

				Polynomial::Polynomial(Polynomial *p) : Polynomial(p->degrees)
				{
				}

//JAVA TO C++ CONVERTER TODO TASK: Most Java annotations will not have direct C++ equivalents:
//ORIGINAL LINE: @SuppressWarnings("unchecked") protected Polynomial(java.util.TreeSet<java.math.BigInteger> degrees)
				Polynomial::Polynomial(std::set<BigInteger*> &degrees) : degrees(static_cast<std::set<BigInteger*>>(degrees.clone()))
				{
				}

				std::set<BigInteger*> Polynomial::createDegreesCollection()
				{
					ReverseComparator tempVar();
					return std::set<BigInteger*>(&tempVar);
				}

//JAVA TO C++ CONVERTER TODO TASK: Most Java annotations will not have direct C++ equivalents:
//ORIGINAL LINE: @SuppressWarnings("unchecked") protected java.util.TreeSet<java.math.BigInteger> createDegreesCollectionCopy()
				std::set<BigInteger*> Polynomial::createDegreesCollectionCopy()
				{
					return static_cast<std::set<BigInteger*>>(this->degrees.clone());
				}

				BigInteger *Polynomial::degree()
				{
					if (degrees.empty())
					{
						return BigInteger::ONE::negate();
					}
					return degrees.first();
				}

				bool Polynomial::isEmpty()
				{
					return degrees.empty();
				}

				Polynomial *Polynomial::add(Polynomial *that)
				{
					return xor(that);
				}

				Polynomial *Polynomial::subtract(Polynomial *that)
				{
					return xor(that);
				}

				Polynomial *Polynomial::multiply(Polynomial *that)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					for (auto pa : this->degrees)
					{
						for (auto pb : that->degrees)
						{
							BigInteger *sum = pa->add(pb);
							// xor the result
							if (std::find(dgrs.begin(), dgrs.end(), sum) != dgrs.end())
							{
								dgrs.remove(sum);
							}
							else
							{
								dgrs.insert(sum);
							}
						}
					}
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::and(Polynomial *that)
				{
					std::set<BigInteger*> &dgrs = this->createDegreesCollectionCopy();
					dgrs.retainAll(that->degrees);
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::or(Polynomial *that)
				{
					std::set<BigInteger*> &dgrs = this->createDegreesCollectionCopy();
					dgrs.addAll(that->degrees);
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::xor(Polynomial *that)
				{
					std::set<BigInteger*> &dgrs0 = this->createDegreesCollectionCopy();
					dgrs0.removeAll(that->degrees);
					std::set<BigInteger*> &dgrs1 = that->createDegreesCollectionCopy();
					dgrs1.removeAll(this->degrees);
					dgrs1.addAll(dgrs0);
					return new Polynomial(dgrs1);
				}

				Polynomial *Polynomial::mod(Polynomial *that)
				{
					BigInteger *da = this->degree();
					BigInteger *db = that->degree();
					Polynomial *register_Renamed = new Polynomial(this->degrees);
					for (BigInteger *i = da->subtract(db); i->compare(BigInteger::ZERO) >= 0; i = i->subtract(BigInteger::ONE))
					{
						if (register_Renamed->hasDegree(i->add(db)))
						{
							Polynomial *shifted = that->shiftLeft(i);
							register_Renamed = register_Renamed->xor(shifted);
						}
					}
					return register_Renamed;
				}

				Polynomial *Polynomial::shiftLeft(BigInteger *shift)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					for (auto degree : degrees)
					{
						BigInteger *shifted = degree->add(shift);
						dgrs.insert(shifted);
					}
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::shiftRight(BigInteger *shift)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					for (auto degree : degrees)
					{
						BigInteger *shifted = degree->subtract(shift);
						if (shifted->compare(BigInteger::ZERO) < 0)
						{
							continue;
						}
						dgrs.insert(shifted);
					}
					return new Polynomial(dgrs);
				}

				bool Polynomial::hasDegree(BigInteger *k)
				{
					return std::find(degrees.begin(), degrees.end(), k) != degrees.end();
				}

				Polynomial *Polynomial::setDegree(BigInteger *k)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					dgrs.addAll(this->degrees);
					dgrs.insert(k);
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::clearDegree(BigInteger *k)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					dgrs.addAll(this->degrees);
					dgrs.remove(k);
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::toggleDegree(BigInteger *k)
				{
					std::set<BigInteger*> &dgrs = createDegreesCollection();
					dgrs.addAll(this->degrees);
					if (std::find(dgrs.begin(), dgrs.end(), k) != dgrs.end())
					{
						dgrs.remove(k);
					}
					else
					{
						dgrs.insert(k);
					}
					return new Polynomial(dgrs);
				}

				Polynomial *Polynomial::modPow(BigInteger *e, Polynomial *m)
				{
					Polynomial *result = Polynomial::ONE;
					Polynomial *b = new Polynomial(this);
					while (e->bitCount() != 0)
					{
						if (e->testBit(0))
						{
							result = result->multiply(b)->mod(m);
						}
						e = e->shiftRight(1);
						b = b->multiply(b)->mod(m);
					}
					return result;
				}

				Polynomial *Polynomial::gcd(Polynomial *that)
				{
					Polynomial *a = new Polynomial(this);
					while (!that->isEmpty())
					{
						Polynomial *t = new Polynomial(that);
						that = a->mod(that);
						a = t;
					}
					return a;
				}

				BigInteger *Polynomial::toBigInteger()
				{
					BigInteger *b = BigInteger::ZERO;
					for (auto degree : degrees)
					{
						b = b->setBit(static_cast<int>(degree->longValue()));
					}
					return b;
				}

				BigInteger *Polynomial::toBigIntegerAccurate()
				{
					BigInteger *b = BigInteger::ZERO;
					for (auto degree : degrees)
					{
						BigInteger *term = BigInteger::ONE;
						for (BigInteger *i = BigInteger::ONE; i->compare(degree) <= 0; i = i->add(BigInteger::ONE))
						{
							term = term->shiftLeft(1);
						}
						b->add(term);
					}
					return b;
				}

				std::wstring Polynomial::toHexString()
				{
//JAVA TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'toString':
					return toBigInteger()->toString(16)->toUpperCase();
				}

				std::wstring Polynomial::toDecimalString()
				{
//JAVA TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'toString':
					return toBigInteger()->toString();
				}

				std::wstring Polynomial::toBinaryString()
				{
					StringBuilder *str = new StringBuilder();
					for (BigInteger *deg = degree(); deg->compare(BigInteger::ZERO) >= 0; deg = deg->subtract(BigInteger::ONE))
					{
						if (std::find(degrees.begin(), degrees.end(), deg) != degrees.end())
						{
							str->append(L"1");
						}
						else
						{
							str->append(L"0");
						}
					}
					return str->toString();
				}

				std::wstring Polynomial::toPolynomialString()
				{
					StringBuilder *str = new StringBuilder();
					for (auto degree : degrees)
					{
						if (str->length() != 0)
						{
							str->append(L" + ");
						}
						if (degree->compare(BigInteger::ZERO) == 0)
						{
							str->append(L"1");
						}
						else
						{
							str->append(std::wstring(L"x^") + degree);
						}
					}
					return str->toString();
				}

				std::wstring Polynomial::toString()
				{
					return toPolynomialString();
				}

				bool Polynomial::isReducible()
				{
					return getReducibility() == Reducibility::REDUCIBLE;
				}

				Polynomial::Reducibility Polynomial::getReducibility()
				{
					// test trivial cases
					if (this->compare(Polynomial::ONE) == 0)
					{
						return Reducibility::REDUCIBLE;
					}
					if (this->compare(Polynomial::X) == 0)
					{
						return Reducibility::REDUCIBLE;
					}

					// do full-on reducibility test
					return getReducibilityBenOr();
				}

				Polynomial::Reducibility Polynomial::getReducibilityBenOr()
				{
					const long long degree = this->degree()->longValue();
					for (int i = 1; i <= static_cast<int>(degree / 2); i++)
					{
						Polynomial *b = reduceExponent(i);
						Polynomial *g = this->gcd(b);
						if (g->compare(Polynomial::ONE) != 0)
						{
							return Reducibility::REDUCIBLE;
						}
					}

					return Reducibility::IRREDUCIBLE;
				}

				Polynomial::Reducibility Polynomial::getReducibilityRabin(std::vector<int> &factors)
				{
					int degree = static_cast<int>(degree()->longValue());
					for (int i = 0; i < factors.size(); i++)
					{
						int n_i = factors[i];
						Polynomial *b = reduceExponent(n_i);
						Polynomial *g = this->gcd(b);
						if (g->compare(Polynomial::ONE) != 0)
						{
							return Reducibility::REDUCIBLE;
						}
					}

					Polynomial *g = reduceExponent(degree);
					if (!g->isEmpty())
					{
						return Reducibility::REDUCIBLE;
					}

					return Reducibility::IRREDUCIBLE;
				}

				Polynomial *Polynomial::reduceExponent(int const p)
				{
					// compute (x^q^p mod f)
					BigInteger *q_to_p = Q->pow(p);
					Polynomial *x_to_q_to_p = X->modPow(q_to_p, this);

					// subtract (x mod f)
					return x_to_q_to_p->xor(X)->mod(this);
				}

				int Polynomial::compareTo(Polynomial *o)
				{
					int cmp = degree()->compare(o->degree());
					if (cmp != 0)
					{
						return cmp;
					}
					// get first degree difference
					Polynomial *x = this->xor(o);
					if (x->isEmpty())
					{
						return 0;
					}
					return this->hasDegree(x->degree()) ? 1 : -1;
				}
			}
		}
	}
}
