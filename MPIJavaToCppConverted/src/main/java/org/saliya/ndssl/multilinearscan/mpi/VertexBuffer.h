#pragma once

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
					/// Saliya Ekanayake on 3/3/17.
					/// </summary>
					class VertexBuffer
					{
						// The real offset would be 2+(offsetFactor*msgSize)
					public:
						int offsetFactor = 0;
						ShortBuffer *buffer;


						VertexBuffer();

						VertexBuffer(int offsetFactor, ShortBuffer *buffer);

						virtual int getOffsetFactor();

						virtual void setOffsetFactor(int offset);

						virtual ShortBuffer *getBuffer();

						virtual void setBuffer(ShortBuffer *buffer);
					};

				}
			}
		}
	}
}
