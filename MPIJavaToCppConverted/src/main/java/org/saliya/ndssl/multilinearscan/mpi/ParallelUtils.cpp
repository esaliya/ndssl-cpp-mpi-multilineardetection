#include "ParallelUtils.h"

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

					ParallelUtils::ParallelUtils(int threadId, int worldProcRank)
					{
						this->threadId = threadId;
						this->worldProcRank = worldProcRank;
					}

					void ParallelUtils::printMessage(const std::wstring &message)
					{
						if (worldProcRank == 0 && threadId == 0)
						{
							std::wcout << message << std::endl;
						}
					}
				}
			}
		}
	}
}
