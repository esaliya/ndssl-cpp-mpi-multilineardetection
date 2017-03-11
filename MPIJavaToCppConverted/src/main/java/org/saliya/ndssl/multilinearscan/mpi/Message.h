#pragma once

#include <vector>
#include "stringbuilder.h"

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
					class Message
					{
					private:
						int originalVertexLabel = 0;
						int msgSize = 0;
						std::vector<std::vector<short>> data;
						ShortBuffer *serializedBytes = nullptr;

					public:
						Message();

						Message(int originalVertexLabel);


						virtual void copyTo(ShortBuffer *buffer, int offset);

						virtual void loadFrom(ShortBuffer *buffer, int offset, int recvdMsgSize);

						virtual void pack();

						virtual int getOriginalVertexLabel();

						virtual void setOriginalVertexLabel(int originalVertexLabel);

						virtual int getMsgSize();

						virtual std::vector<std::vector<short>> getData();

						virtual void setDataAndMsgSize(std::vector<std::vector<short>> &data, int msgSize);

						virtual StringBuilder *toString(StringBuilder *sb);
					};

				}
			}
		}
	}
}
