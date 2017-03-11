#include "Vertex.h"
#include "VertexBuffer.h"
#include "RecvVertexBuffer.h"
#include "Message.h"
#include "../GaloisField.h"
#include "../../Utils.h"

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
					using Utils = org::saliya::ndssl::Utils;
					using GaloisField = org::saliya::ndssl::multilinearscan::GaloisField;

					Vertex::Vertex()
					{

					}

					Vertex::Vertex(int vertexId, const std::wstring &vertexLine)
					{
//JAVA TO C++ CONVERTER TODO TASK: There is no direct native C++ equivalent to the Java String 'split' method:
						std::vector<std::wstring> splits = pat->split(vertexLine);
						this->vertexId = vertexId;
						vertexLabel = std::stoi(splits[0]);
						weight = std::stod(splits[1]);
						outrankToSendBuffer = std::map<*>();
						outNeighborLabelToWorldRank = std::map<*>();
						for (int i = 2; i < splits.size(); ++i)
						{
							outNeighborLabelToWorldRank[std::stoi(splits[i])] = -1;
						}
						recvBuffers = std::vector<*>();
						message = new Message(vertexLabel);
						recvdMessages = std::vector<*>();
					}

					void Vertex::compute(int superStep, int iter, std::vector<int> &completionVariables, std::map<int, int> &randomAssignments)
					{

						// In the original code I started from 2 and went till k (including).
						// we start ss from zero, which is for initial msg sending
						// then real work begins with ss=1, which should give I=2, hence I=ss+1
						int I = superStep + 1;

						/* compute logic */
						if (superStep == 0)
						{
							reset(iter, completionVariables, randomAssignments);
						}
						else if (superStep > 0)
						{
							// DEBUG - see received messages
							/*StringBuffer sb = new StringBuffer("\nVertex ");
							sb.append(vertexLabel).append(" recvd ");
							recvdMessages.forEach(m -> {
							    m.toString(sb).append(" ");
							});
							sb.append('\n');
							System.out.println(sb.toString());*/

							// Business logic
							int fieldSize = gf->getFieldSize();
							// for every quota l from 0 to r
							for (int l = 0; l <= r; l++)
							{
								// initialize the polynomial P_{i,u,l}
								int polynomial = 0;
								// recursive step:
								// iterate through all the pairs of polynomials whose sizes add up to i
								for (int iPrime = 1; iPrime < I; iPrime++)
								{
									for (auto msg : recvdMessages)
									{
										std::vector<std::vector<short>> neighborOptTable = msg->getData();
										if (l == 0)
										{
											int weight = random->nextInt(fieldSize);
											int product = gf->multiply(optTable[iPrime][0], neighborOptTable[I - iPrime][0]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);
										}
										else if (l == 1)
										{
											int weight = random->nextInt(fieldSize);
											int product = gf->multiply(optTable[iPrime][1], neighborOptTable[I - iPrime][0]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);

											weight = random->nextInt(fieldSize);
											product = gf->multiply(optTable[iPrime][0], neighborOptTable[I - iPrime][1]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);
										}
										else
										{
											int weight = random->nextInt(fieldSize);
											int product = gf->multiply(optTable[iPrime][l - 1], neighborOptTable[I - iPrime][l - 1]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);

											weight = random->nextInt(fieldSize);
											product = gf->multiply(optTable[iPrime][l], neighborOptTable[I - iPrime][0]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);

											weight = random->nextInt(fieldSize);
											product = gf->multiply(optTable[iPrime][0], neighborOptTable[I - iPrime][l]);
											product = gf->multiply(weight, product);
											polynomial = gf->add(polynomial, product);
										}
									}
								}
								optTable[I][l] = static_cast<short>(polynomial);
								if (cumulativeCompletionVariables[k - I] != 0)
								{
									extTable[I][l] = optTable[I][l];
								}
							}
						}
					}

					int Vertex::prepareSend(int superStep, int shift)
					{
						/* copy msg to outrankToSendBuffer */
						message->pack();
						outrankToSendBuffer.entrySet().forEach([&] (void *kv)
						{
								VertexBuffer *vertexBuffer = kv::getValue();
								int offset = shift + vertexBuffer->offsetFactor * message->getMsgSize();
								message->copyTo(vertexBuffer->buffer, offset);
						});
						return message->getMsgSize();
					}

					void Vertex::processRecvd(int superStep, int shift)
					{
						for (int i = 0; i < recvBuffers.size(); ++i)
						{
							RecvVertexBuffer *recvVertexBuffer = recvBuffers[i];
							Message *recvdMessage = recvdMessages[i];
							int recvdMsgSize = recvVertexBuffer->getMessageSize();
							recvdMessage->loadFrom(recvVertexBuffer->buffer, shift + recvVertexBuffer->offsetFactor*recvdMsgSize, recvdMsgSize);
						}
					}

					void Vertex::init(int k, int r, GaloisField *gf)
					{
						this->k = k;
						this->r = r;
						this->gf = gf;
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: totalSumTable = new int[k+1][r+1];
						totalSumTable = RectangularVectors::ReturnRectangularIntVector(k + 1, r + 1);
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: optTable = new short[k+1][r+1];
						optTable = RectangularVectors::ReturnRectangularShortVector(k + 1, r + 1);
//JAVA TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in Java:
//ORIGINAL LINE: extTable = new short[k+1][r+1];
						extTable = RectangularVectors::ReturnRectangularShortVector(k + 1, r + 1);
						for (int i = 0; i <= k; i++)
						{
							for (int j = 0; j <= r; j++)
							{
								totalSumTable[i][j] = 0;
							}
						}
						cumulativeCompletionVariables = std::vector<int>(k);

					}

					void Vertex::reset(int iter, std::vector<int> &completionVariables, std::map<int, int> &randomAssignments)
					{
						// the ith index of this array contains the polynomial
						// y_1 * y_2 * ... y_i
						// where y_0 is defined as the identity of the finite field, (i.e., 1)
						cumulativeCompletionVariables[0] = 1;
						for (int i = 1; i < k; i++)
						{
							int dotProduct = (completionVariables[i - 1] & iter); // dot product is bitwise and
							cumulativeCompletionVariables[i] = (Integer::bitCount(dotProduct) % 2 == 1) ? 0 : cumulativeCompletionVariables[i - 1];
						}

						// create the vertex unique random variable
						random = new Random(uniqueRandomSeed);
						// set arrays in vertex data
						IntStream::range(0,k + 1).forEach([&] (void *i)
						{
								java::util::stream::IntStream::range(0, r + 1).forEach([&] (void *j)
								{
			optTable[i][j] = 0;
			extTable[i][j] = 0;
								});
						});

						int nodeWeight = static_cast<int>(weight);
						int dotProduct = (randomAssignments[vertexLabel] & iter); // dot product is bitwise and
						int eigenvalue = (Integer::bitCount(dotProduct) % 2 == 1) ? 0 : 1;
						for (int i = 0; i <= r; i++)
						{
							optTable[1][i] = extTable[1][i] = 0;
						}
						optTable[1][nodeWeight] = static_cast<short>(eigenvalue);
						extTable[1][nodeWeight] = static_cast<short>(eigenvalue * cumulativeCompletionVariables[k - 1]);
						message->setDataAndMsgSize(optTable, (k + 1)*(r + 1));
					}

					void Vertex::finalizeIteration()
					{
						// aggregate to master
						// hmm, but we don't need to aggregate, just add to totalSumTable of the vertex
						for (int kPrime = 0; kPrime <= k; kPrime++)
						{
							for (int rPrime = 0; rPrime <= r; rPrime++)
							{
								totalSumTable[kPrime][rPrime] = gf->add(totalSumTable[kPrime][rPrime], extTable[kPrime][rPrime]);
							}
						}
					}

					double Vertex::finalizeIterations(double alphaMax, int roundingFactor)
					{
						// Now, we can change the totalSumTable to the decisionTable
						for (int kPrime = 0; kPrime <= k; kPrime++)
						{
							for (int rPrime = 0; rPrime <= r; rPrime++)
							{
								totalSumTable[kPrime][rPrime] = (totalSumTable[kPrime][rPrime] > 0) ? 1 : -1;
							}
						}
						return getScoreFromTablePower(totalSumTable, alphaMax, roundingFactor);
					}

					double Vertex::getScoreFromTablePower(std::vector<std::vector<int>> &existenceForNode, double alpha, int roundingFactor)
					{
						double nodeLocalBestScore = 0;
						for (int kPrime = 1; kPrime < existenceForNode.size(); kPrime++)
						{
							for (int rPrime = 0; rPrime < existenceForNode[0].length; rPrime++)
							{
								if (existenceForNode[kPrime][rPrime] == 1)
								{
									//System.out.println("Found subgraph with size " + kPrime + " and score " + rPrime);
									// size cannot be smaller than weight
									int unroundedPrize = static_cast<int>(std::pow(roundingFactor, rPrime - 1));
									// adjust for the fact that this is the refined graph
									int unroundedSize = std::max(kPrime, unroundedPrize);
									double score = Utils::BJ(alpha, unroundedPrize, unroundedSize);
									//System.out.println("Score is " + score);
									nodeLocalBestScore = std::max(nodeLocalBestScore, score);
								}
							}
						}
						return nodeLocalBestScore;
					}
				}
			}
		}
	}
}
