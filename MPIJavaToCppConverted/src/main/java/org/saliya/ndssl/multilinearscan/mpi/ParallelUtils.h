#pragma once

#include <string>
#include <iostream>

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			namespace multilinearscan
			{
				namespace mpi
				{

					/// <summary>
					/// Saliya Ekanayake on 2/22/17.
					/// </summary>
					class ParallelUtils
					{
					public:
						int threadId = 0;
						int worldProcRank = 0;

						ParallelUtils(int threadId, int worldProcRank);

						virtual void printMessage(const std::wstring &message);
					};

				}
			}
		}
	}
}
