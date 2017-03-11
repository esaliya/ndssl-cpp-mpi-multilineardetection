#include "GaloisField.h"

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{

const std::unordered_map<int, GaloisField*> GaloisField::instances = std::unordered_map<int, GaloisField*>();

				GaloisField *GaloisField::getInstance(int fieldSize, int primitivePolynomial)
				{
					int key = ((fieldSize << 16) & 0xFFFF0000) + (primitivePolynomial & 0x0000FFFF);
					GaloisField *gf;
//JAVA TO C++ CONVERTER TODO TASK: Multithread locking is not converted to native C++:
					synchronized(instances)
					{
						gf = instances[key];
						if (gf == nullptr)
						{
							gf = new GaloisField(fieldSize, primitivePolynomial);
							instances[key] = gf;
						}
					}
					return gf;
				}

				GaloisField *GaloisField::getInstance()
				{
					return getInstance(DEFAULT_FIELD_SIZE, DEFAULT_PRIMITIVE_POLYNOMIAL);
				}

//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: private GaloisField(int fieldSize, int primitivePolynomial)
				GaloisField::GaloisField(int fieldSize, int primitivePolynomial) : logTable(std::vector<int>(fieldSize)), powTable(std::vector<int>(fieldSize)), mulTable(RectangularVectors::ReturnRectangularIntVector(fieldSize, fieldSize)), divTable(RectangularVectors::ReturnRectangularIntVector(fieldSize, fieldSize)), fieldSize(fieldSize), primitivePeriod(fieldSize - 1), primitivePolynomial(primitivePolynomial)
				{
					if (fieldSize <= 0)
					{
						IllegalArgumentException tempVar(L"fieldSize must be a positive integer");
						throw &tempVar;
					}
					if (primitivePolynomial <= 0)
					{
						IllegalArgumentException tempVar2(L"primitivePolynomial must be a positive integer");
						throw &tempVar2;
					}

					int value = 1;
					for (int pow = 0; pow < fieldSize - 1; pow++)
					{
						powTable[pow] = value;
						logTable[value] = pow;
						value = value * 2;
						if (value >= fieldSize)
						{
							value = value ^ primitivePolynomial;
						}
					}

					// building multiplication table
					for (int i = 0; i < fieldSize; i++)
					{
						for (int j = 0; j < fieldSize; j++)
						{
							if (i == 0 || j == 0)
							{
								mulTable[i][j] = 0;
								continue;
							}
							int z = logTable[i] + logTable[j];
							z = z >= primitivePeriod ? z - primitivePeriod : z;
							z = powTable[z];
							mulTable[i][j] = z;
						}
					}
					// building division table
					for (int i = 0; i < fieldSize; i++)
					{
						for (int j = 1; j < fieldSize; j++)
						{
							if (i == 0)
							{
								divTable[i][j] = 0;
								continue;
							}
							int z = logTable[i] - logTable[j];
							z = z < 0 ? z + primitivePeriod : z;
							z = powTable[z];
							divTable[i][j] = z;
						}
					}
				}

				int GaloisField::getFieldSize()
				{
					return fieldSize;
				}

				int GaloisField::getPrimitivePolynomial()
				{
					return primitivePolynomial;
				}

				int GaloisField::add(int x, int y)
				{
					if ((x < 0 || x >= getFieldSize()) || (y < 0 || y >= getFieldSize()))
					{
						IllegalArgumentException tempVar(L"x and y must be in the range [0, fieldSize)");
						throw &tempVar;
					}
					return x ^ y;
				}

				int GaloisField::multiply(int x, int y)
				{
					if ((x < 0 || x >= getFieldSize()) || (y < 0 || y >= this->fieldSize))
					{
						IllegalArgumentException tempVar(L"x and y must be in the range [0, fieldSize)");
						throw &tempVar;
					}
					return mulTable[x][y];
				}

				int GaloisField::ffMultiply(int x, int y)
				{
					if ((x < 0 || x >= this->fieldSize) || (y < 0 || y >= this->fieldSize))
					{
						IllegalArgumentException tempVar(L"x and y must be in the range [0, fieldSize)");
						throw &tempVar;
					}
					int xx = x, yy = y, r = 0;
					while (xx != 0)
					{
						if ((xx & 1) != 0)
						{
							r = r ^ yy;
						}
						yy = yy << 1;
						if (yy >= this->fieldSize)
						{
							yy = yy ^ this->primitivePolynomial;
						}
						xx = xx >> 1;
					}
					return r;
				}

				int GaloisField::divide(int x, int y)
				{
					if ((x < 0 || x >= getFieldSize()) || (y < 0 || y >= this->fieldSize))
					{
						IllegalArgumentException tempVar(L"x and y must be in the range [0, fieldSize)");
						throw &tempVar;
					}
					return divTable[x][y];
				}

				int GaloisField::power(int x, int n)
				{
					if ((x < 0 || x >= getFieldSize()))
					{
						IllegalArgumentException tempVar(L"x must be in the range [0, fieldSize)");
						throw &tempVar;
					}
					if (n == 0)
					{
						return 1;
					}
					if (x == 0)
					{
						return 0;
					}
					x = logTable[x] * n;
					if (x < primitivePeriod)
					{
						return powTable[x];
					}
					x = x % primitivePeriod;
					return powTable[x];
				}

				void GaloisField::solveVandermondeSystem(std::vector<int> &x, std::vector<int> &y)
				{
					solveVandermondeSystem(x, y, x.size());
				}

				void GaloisField::solveVandermondeSystem(std::vector<int> &x, std::vector<int> &y, int len)
				{
					//assert(x.length <= len && y.length <= len);
					if (x.size() > len || y.size() > len)
					{
						IllegalArgumentException tempVar(L"x and y must be less than or equal to len");
						throw &tempVar;
					}
					for (int i = 0; i < len - 1; i++)
					{
						for (int j = len - 1; j > i; j--)
						{
							y[j] = y[j] ^ mulTable[x[i]][y[j - 1]];
						}
					}
					for (int i = len - 1; i >= 0; i--)
					{
						for (int j = i + 1; j < len; j++)
						{
							y[j] = divTable[y[j]][x[j] ^ x[j - i - 1]];
						}
						for (int j = i; j < len - 1; j++)
						{
							y[j] = y[j] ^ y[j + 1];
						}
					}
				}

				std::vector<int> GaloisField::multiply(std::vector<int> &p, std::vector<int> &q)
				{
					int len = p.size() + q.size() - 1;
					std::vector<int> result(len);
					for (int i = 0; i < len; i++)
					{
						result[i] = 0;
					}
					for (int i = 0; i < p.size(); i++)
					{
						for (int j = 0; j < q.size(); j++)
						{
							result[i + j] = add(result[i + j], multiply(p[i], q[j]));
						}
					}
					return result;
				}

				void GaloisField::remainder(std::vector<int> &dividend, std::vector<int> &divisor)
				{
					for (int i = dividend.size() - divisor.size(); i >= 0; i--)
					{
						int ratio = divTable[dividend[i + divisor.size() - 1]][divisor[divisor.size() - 1]];
						for (int j = 0; j < divisor.size(); j++)
						{
							int k = j + i;
							dividend[k] = dividend[k] ^ mulTable[ratio][divisor[j]];
						}
					}
				}

				std::vector<int> GaloisField::add(std::vector<int> &p, std::vector<int> &q)
				{
					int len = std::max(p.size(), q.size());
					std::vector<int> result(len);
					for (int i = 0; i < len; i++)
					{
						if (i < p.size() && i < q.size())
						{
							result[i] = add(p[i], q[i]);
						}
						else if (i < p.size())
						{
							result[i] = p[i];
						}
						else
						{
							result[i] = q[i];
						}
					}
					return result;
				}

				int GaloisField::substitute(std::vector<int> &p, int x)
				{
					int result = 0;
					int y = 1;
					for (int i = 0; i < p.size(); i++)
					{
						result = result ^ mulTable[p[i]][y];
						y = mulTable[x][y];
					}
					return result;
				}
			}
		}
	}
}
