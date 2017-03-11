#pragma once

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{

				template<typename T>
				class Arithmetic
				{

				public:
					virtual public T add(T other) = 0;

					virtual public T subtract(T other) = 0;

					virtual public T multiply(T other) = 0;

					virtual public T and(T other) = 0;

					virtual public T or(T other) = 0;

					virtual public T xor(T other) = 0;

					virtual public T mod(T other) = 0;

					virtual public T gcd(T other) = 0;
				};
			}
		}
	}
}
