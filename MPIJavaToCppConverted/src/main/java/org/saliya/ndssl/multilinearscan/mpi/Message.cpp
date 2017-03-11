#include "Message.h"

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
					using mpi::MPI;

					Message::Message()
					{

					}

					Message::Message(int originalVertexLabel)
					{
						this->originalVertexLabel = originalVertexLabel;
					}

					void Message::copyTo(ShortBuffer *buffer, int offset)
					{
						buffer->position(offset);
						serializedBytes->position(0);
						buffer->put(serializedBytes);
					}

					void Message::loadFrom(ShortBuffer *buffer, int offset, int recvdMsgSize)
					{
						this->msgSize = recvdMsgSize;
						int dimA = buffer->get(offset);
						int dimB = buffer->get(offset + 1);
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: data = new short[dimA][dimB];
						data = RectangularVectors::ReturnRectangularShortVector(dimA, dimB);
						IntStream::range(0, dimA).forEach([&] (void *i)
						{
								java::util::stream::IntStream::range(0,dimB).forEach([&] (void *j)
								{
			data[i][j] = buffer->get(offset + 2 + (i*dimB + j));
								});
						});
					}

					void Message::pack()
					{
						serializedBytes->position(0);
						serializedBytes->put(static_cast<short>(data.size()));
						serializedBytes->put(static_cast<short>((data.size() > 0) ? data[0].length : 0));
						for (auto arr : data)
						{
							serializedBytes->put(arr);
						}
					}

					int Message::getOriginalVertexLabel()
					{
						return originalVertexLabel;
					}

					void Message::setOriginalVertexLabel(int originalVertexLabel)
					{
						this->originalVertexLabel = originalVertexLabel;
					}

					int Message::getMsgSize()
					{
						return msgSize;
					}

					std::vector<std::vector<short>> Message::getData()
					{
						return data;
					}

					void Message::setDataAndMsgSize(std::vector<std::vector<short>> &data, int msgSize)
					{
						this->data = data;
						// +2 to store dimensions
						this->msgSize = msgSize+2;

						if (serializedBytes == nullptr || serializedBytes->capacity() < this->msgSize)
						{
							serializedBytes = MPI::newShortBuffer(this->msgSize);
						}
					}

					StringBuilder *Message::toString(StringBuilder *sb)
					{
						sb->append(L"[");
						for (auto arr : data)
						{
//JAVA TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'toString':
							sb->append(Arrays->toString(arr));
						}
						sb->append(L"]");
						return sb;
					}
				}
			}
		}
	}
}
