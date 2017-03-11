#include "RecvVertexBuffer.h"

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

					RecvVertexBuffer::RecvVertexBuffer(int offsetFactor, ShortBuffer *buffer, int recvfromRank, int msgSizeOffset) : VertexBuffer(offsetFactor, buffer)
					{
						this->recvfromRank = recvfromRank;
						this->msgSizeOffset = msgSizeOffset;
					}

					int RecvVertexBuffer::getMessageSize()
					{
						return buffer->get(msgSizeOffset);
					}
				}
			}
		}
	}
}
