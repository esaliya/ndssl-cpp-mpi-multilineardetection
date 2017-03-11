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

					VertexBuffer::VertexBuffer()
					{
					}

					VertexBuffer::VertexBuffer(int offsetFactor, ShortBuffer *buffer)
					{
						this->offsetFactor = offsetFactor;
						this->buffer = buffer;
					}

					int VertexBuffer::getOffsetFactor()
					{
						return offsetFactor;
					}

					void VertexBuffer::setOffsetFactor(int offset)
					{
						this->offsetFactor = offset;
					}

					ShortBuffer *VertexBuffer::getBuffer()
					{
						return buffer;
					}

					void VertexBuffer::setBuffer(ShortBuffer *buffer)
					{
						this->buffer = buffer;
					}
				}
			}
		}
	}
}
