#pragma once

#include <unordered_map>
#include <vector>

/// <summary>
/// Licensed to the Apache Software Foundation (ASF) under one
/// or more contributor license agreements.  See the NOTICE file
/// distributed with this work for additional information
/// regarding copyright ownership.  The ASF licenses this file
/// to you under the Apache License, Version 2.0 (the
/// "License"); you may not use this file except in compliance
/// with the License.  You may obtain a copy of the License at
/// <para>
/// http://www.apache.org/licenses/LICENSE-2.0
/// </para>
/// <para>
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
/// </para>
/// <para>
/// From https://svn.apache.org/repos/asf/hadoop/common/branches/HDFS-1073/mapreduce/src/contrib/raid/src/java/org/apache/hadoop/raid/GaloisField.java
/// </para>
/// </summary>

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{


				/*
				 * Jose Cadena
				 */

				/// <summary>
				/// Implementation of Galois field arithmetics with 2^p elements.
				/// The input must be unsigned integers.
				/// </summary>
				class GaloisField : public Serializable
				{

				private:
					std::vector<int> const logTable;
					std::vector<int> const powTable;
					std::vector<std::vector<int>> const mulTable;
					std::vector<std::vector<int>> const divTable;
					const int fieldSize;
					const int primitivePeriod;
					const int primitivePolynomial;

					// Field size 256 is good for byte based system
					static const int DEFAULT_FIELD_SIZE = 256;
					// primitive polynomial 1 + X^2 + X^3 + X^4 + X^8
					static const int DEFAULT_PRIMITIVE_POLYNOMIAL = 285;

					static const std::unordered_map<int, GaloisField*> instances;

					/// <summary>
					/// Get the object performs Galois field arithmetics </summary>
					/// <param name="fieldSize"> size of the field </param>
					/// <param name="primitivePolynomial"> a primitive polynomial corresponds to the size </param>
				public:
					static GaloisField *getInstance(int fieldSize, int primitivePolynomial);

					/// <summary>
					/// Get the object performs Galois field arithmetics with default setting
					/// </summary>
					static GaloisField *getInstance();

				private:
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: private GaloisField(int fieldSize, int primitivePolynomial)
					GaloisField(int fieldSize, int primitivePolynomial);

					/// <summary>
					/// Return number of elements in the field </summary>
					/// <returns> number of elements in the field </returns>
				public:
					virtual int getFieldSize();

					/// <summary>
					/// Return the primitive polynomial in GF(2) </summary>
					/// <returns> primitive polynomial as a integer </returns>
					virtual int getPrimitivePolynomial();

					/// <summary>
					/// Compute the sum of two fields </summary>
					/// <param name="x"> input field </param>
					/// <param name="y"> input field </param>
					/// <returns> result of addition </returns>
					virtual int add(int x, int y);

					/// <summary>
					/// Compute the multiplication of two fields </summary>
					/// <param name="x"> input field </param>
					/// <param name="y"> input field </param>
					/// <returns> result of multiplication </returns>
					virtual int multiply(int x, int y);

					virtual int ffMultiply(int x, int y);

					/// <summary>
					/// Compute the division of two fields </summary>
					/// <param name="x"> input field </param>
					/// <param name="y"> input field </param>
					/// <returns> x/y </returns>
					virtual int divide(int x, int y);

					/// <summary>
					/// Compute power n of a field </summary>
					/// <param name="x"> input field </param>
					/// <param name="n"> power </param>
					/// <returns> x^n </returns>
					virtual int power(int x, int n);

					/// <summary>
					/// Given a Vandermonde matrix V[i][j]=x[j]^i and vector y, solve for z such
					/// that Vz=y. The output z will be placed in y. </summary>
					/// <param name="x"> the vector which describe the Vandermonde matrix </param>
					/// <param name="y"> right-hand side of the Vandermonde system equation.
					///          will be replaced the output in this vector </param>
					virtual void solveVandermondeSystem(std::vector<int> &x, std::vector<int> &y);

					/// <summary>
					/// Given a Vandermonde matrix V[i][j]=x[j]^i and vector y, solve for z such
					/// that Vz=y. The output z will be placed in y. </summary>
					/// <param name="x"> the vector which describe the Vandermonde matrix </param>
					/// <param name="y"> right-hand side of the Vandermonde system equation.
					///          will be replaced the output in this vector </param>
					/// <param name="len"> consider x and y only from 0...len-1 </param>
					virtual void solveVandermondeSystem(std::vector<int> &x, std::vector<int> &y, int len);

					/// <summary>
					/// Compute the multiplication of two polynomials. The index in the
					/// array corresponds to the power of the entry. For example p[0] is the
					/// constant term of the polynomial p. </summary>
					/// <param name="p"> input polynomial </param>
					/// <param name="q"> input polynomial </param>
					/// <returns> polynomial represents p*q </returns>
					virtual std::vector<int> multiply(std::vector<int> &p, std::vector<int> &q);

					/// <summary>
					/// Compute the remainder of a dividend and divisor pair. The index in the
					/// array corresponds to the power of the entry. For example p[0] is the
					/// constant term of the polynomial p. </summary>
					/// <param name="dividend"> dividend polynomial, the remainder will be placed here when return </param>
					/// <param name="divisor"> divisor polynomial </param>
					virtual void remainder(std::vector<int> &dividend, std::vector<int> &divisor);

					/// <summary>
					/// Compute the sum of two polynomials. The index in the
					/// array corresponds to the power of the entry. For example p[0] is the
					/// constant term of the polynomial p. </summary>
					/// <param name="p"> input polynomial </param>
					/// <param name="q"> input polynomial </param>
					/// <returns> polynomial represents p+q </returns>
					virtual std::vector<int> add(std::vector<int> &p, std::vector<int> &q);

					/// <summary>
					/// Substitute x into polynomial p(x). </summary>
					/// <param name="p"> input polynomial </param>
					/// <param name="x"> input field </param>
					/// <returns> p(x) </returns>
					virtual int substitute(std::vector<int> &p, int x);
				};
			}
		}
	}
}
