#pragma once

#include <string>
#include <map>
#include <vector>
#include <cmath>

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { namespace mpi { class VertexBuffer; } } } } }
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { namespace mpi { class RecvVertexBuffer; } } } } }
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { namespace mpi { class Message; } } } } }
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { class GaloisField; } } } }

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

					using GaloisField = org::saliya::ndssl::multilinearscan::GaloisField;


					/// <summary>
					/// Saliya Ekanayake on 2/22/17.
					/// </summary>
					class Vertex
					{
					public:
						Pattern *pat = Pattern::compile(L" ");
						int vertexLabel = 0;
						int vertexId = 0;

						std::map<int, int> outNeighborLabelToWorldRank;
						std::map<int, VertexBuffer*> outrankToSendBuffer;
						std::vector<RecvVertexBuffer*> recvBuffers;

						Message *message;
						std::vector<Message*> recvdMessages;


						double weight = 0;
						long long uniqueRandomSeed = 0;
					private:
						int k = 0;
						int r = 0;
						GaloisField *gf;
						std::vector<int> cumulativeCompletionVariables;
						std::vector<std::vector<int>> totalSumTable;
						std::vector<std::vector<short>> optTable;
						std::vector<std::vector<short>> extTable;
						// Vertex unique random generator
						Random *random;

					public:
						Vertex();
						Vertex(int vertexId, const std::wstring &vertexLine);

						virtual void compute(int superStep, int iter, std::vector<int> &completionVariables, std::map<int, int> &randomAssignments);

						virtual int prepareSend(int superStep, int shift);

						virtual void processRecvd(int superStep, int shift);

						virtual void init(int k, int r, GaloisField *gf);

						virtual void reset(int iter, std::vector<int> &completionVariables, std::map<int, int> &randomAssignments);

						virtual void finalizeIteration();

						virtual double finalizeIterations(double alphaMax, int roundingFactor);

						// This is node local best score, not the global best
						// to get the global best we have to find the max of these local best scores,
						// which we'll do in the master using aggregation
					private:
						double getScoreFromTablePower(std::vector<std::vector<int>> &existenceForNode, double alpha, int roundingFactor);
					};

				}
			}
		}
	}
}
