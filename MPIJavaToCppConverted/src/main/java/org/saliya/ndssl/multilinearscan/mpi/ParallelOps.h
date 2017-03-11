#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <boost/optional.hpp>
#include "stringbuilder.h"

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { namespace mpi { class Vertex; } } } } }

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

					using mpi::Intracomm;
					using mpi::MPIException;
					using mpi::Request;


					/// <summary>
					/// Saliya Ekanayake on 2/22/17.
					/// </summary>
					class ParallelOps
					{
					public:
						static std::wstring machineName;
						static int nodeCount;
						static int threadCount;
						static std::wstring mmapScratchDir;

						static Intracomm *worldProcsComm;
						static int worldProcRank;
						static int worldProcsCount;

						static IntBuffer *oneIntBuffer;
						static LongBuffer *oneLongBuffer;
						static DoubleBuffer *oneDoubleBuffer;
						static IntBuffer *worldIntBuffer;
						static IntBuffer *vertexIntBuffer;
						static LongBuffer *vertexLongBuffer;
						static DoubleBuffer *vertexDoubleBuffer;

						// Will include same rank as well
						static std::map<int, std::vector<int>> recvfromRankToMsgCountAndforvertexLabels;
						static std::map<int, std::vector<int>> sendtoRankToMsgCountAndDestinedVertexLabels;

						// Maximum message size sent by a vertex. To be set later correctly.
						static int MAX_MSG_SIZE;
						static std::map<int, ShortBuffer*> recvfromRankToRecvBuffer;
						static std::map<int, ShortBuffer*> sendtoRankToSendBuffer;
						// to store msg count and msg size
						static const int BUFFER_OFFSET = 2;
						static const int MSG_COUNT_OFFSET = 0;
						static const int MSG_SIZE_OFFSET = 1;

						static int msgSizeToReceive;

						static std::map<int, Request*> requests;

					private:
						static bool debug;
					public:
						static std::vector<int> localVertexCounts;
						static std::vector<int> localVertexDisplas;

						static std::unordered_map<int, int> vertexLabelToWorldRank;


						static void setupParallelism(std::vector<std::wstring> &args) throw(MPIException);

						static void tearDownParallelism() throw(MPIException);

						static std::vector<Vertex*> setParallelDecomposition(const std::wstring &file, int vertexCount) throw(MPIException);

					private:
						static std::vector<Vertex*> simpleGraphPartition(const std::wstring &file, int globalVertexCount) throw(MPIException);

					public:
						static void findNeighbors(int globalVertexCount, std::vector<Vertex*> &vertices) throw(MPIException);

						static std::wstring allReduce(const std::wstring &value, Intracomm *comm) throw(MPIException);

						static void sendMessages(int msgSize);

						static void recvMessages() throw(MPIException);
					};

				}
			}
		}
	}
}
