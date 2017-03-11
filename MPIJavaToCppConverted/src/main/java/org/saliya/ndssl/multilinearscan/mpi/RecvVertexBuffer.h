#pragma once

#include "VertexBuffer.h"

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
					/// Saliya Ekanayake on 3/4/17.
					/// </summary>
					class RecvVertexBuffer : public VertexBuffer
					{
					public:
						int recvfromRank = 0;
						int msgSizeOffset = 0;

						RecvVertexBuffer(int offsetFactor, ShortBuffer *buffer, int recvfromRank, int msgSizeOffset);

						virtual int getMessageSize();
					};

				}
			}
		}
	}
}
