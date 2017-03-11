#include "ParallelOps.h"
#include "Vertex.h"
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
					using com::google::common::base::Strings;
					using mpi::Intracomm;
					using mpi::MPI;
					using mpi::MPIException;
					using mpi::Request;
std::wstring ParallelOps::machineName;
int ParallelOps::nodeCount = 0;
int ParallelOps::threadCount = 0;
std::wstring ParallelOps::mmapScratchDir;
mpi::Intracomm *ParallelOps::worldProcsComm;
int ParallelOps::worldProcRank = 0;
int ParallelOps::worldProcsCount = 0;
java::nio::IntBuffer *ParallelOps::oneIntBuffer;
java::nio::LongBuffer *ParallelOps::oneLongBuffer;
java::nio::DoubleBuffer *ParallelOps::oneDoubleBuffer;
java::nio::IntBuffer *ParallelOps::worldIntBuffer;
java::nio::IntBuffer *ParallelOps::vertexIntBuffer;
java::nio::LongBuffer *ParallelOps::vertexLongBuffer;
java::nio::DoubleBuffer *ParallelOps::vertexDoubleBuffer;
std::map<int, std::vector<int>> ParallelOps::recvfromRankToMsgCountAndforvertexLabels;
std::map<int, std::vector<int>> ParallelOps::sendtoRankToMsgCountAndDestinedVertexLabels;
int ParallelOps::MAX_MSG_SIZE = 500;
std::map<int, java::nio::ShortBuffer*> ParallelOps::recvfromRankToRecvBuffer;
std::map<int, java::nio::ShortBuffer*> ParallelOps::sendtoRankToSendBuffer;
int ParallelOps::msgSizeToReceive = 0;
std::map<int, mpi::Request*> ParallelOps::requests;
bool ParallelOps::debug = false;
std::vector<int> ParallelOps::localVertexCounts;
std::vector<int> ParallelOps::localVertexDisplas;
std::unordered_map<int, int> ParallelOps::vertexLabelToWorldRank;

					void ParallelOps::setupParallelism(std::vector<std::wstring> &args) throw(MPIException)
					{
						MPI::Init(args);
						machineName = MPI::getProcessorName();
						worldProcsComm = MPI::COMM_WORLD; //initializing MPI world communicator
						worldProcRank = worldProcsComm->getRank();
						worldProcsCount = worldProcsComm->getSize();

						oneIntBuffer = MPI::newIntBuffer(1);
						oneLongBuffer = MPI::newLongBuffer(1);
						oneDoubleBuffer = MPI::newDoubleBuffer(1);
						worldIntBuffer = MPI::newIntBuffer(worldProcsCount);

					}

					void ParallelOps::tearDownParallelism() throw(MPIException)
					{
						MPI::Finalize();

					}

					std::vector<Vertex*> ParallelOps::setParallelDecomposition(const std::wstring &file, int vertexCount) throw(MPIException)
					{
						/* Decompose input graph into processes */
						vertexIntBuffer = MPI::newIntBuffer(vertexCount);
						vertexLongBuffer = MPI::newLongBuffer(vertexCount);
						vertexDoubleBuffer = MPI::newDoubleBuffer(vertexCount);
						return simpleGraphPartition(file, vertexCount);
					}

					std::vector<Vertex*> ParallelOps::simpleGraphPartition(const std::wstring &file, int globalVertexCount) throw(MPIException)
					{
						/* Will assume vertex IDs are continuous and starts with zero
						* Then partition these vertices uniformly across all processes
						* Also, this assumes all vertices have out edges, otherwise we can't skip
						* lines like here.*/

						int q = globalVertexCount / worldProcsCount;
						int r = globalVertexCount % worldProcsCount;
						int myVertexCount = (worldProcRank < r) ? q + 1: q;
						std::vector<Vertex*> vertices(myVertexCount);
						int skipVertexCount = q*worldProcRank + (worldProcRank < r ? worldProcRank : r);
						int readCount = 0;
						int i = 0;

						// DEBUG
						//System.out.println("Rank: " + worldProcRank + " q: " + q + " r: " + r + " myVertexCount: " + myVertexCount);
//JAVA TO C++ CONVERTER NOTE: The following 'try with resources' block is replaced by its C++ equivalent:
//ORIGINAL LINE: try (java.io.BufferedReader reader = java.nio.file.Files.newBufferedReader(java.nio.file.Paths.get(file)))
						java::io::BufferedReader *reader = java::nio::file::Files::newBufferedReader(java::nio::file::Paths->get(file));
						try
						{
							std::wstring line;
							while ((line = reader->readLine()) != L"")
							{
								if (Strings::isNullOrEmpty(line))
								{
									continue;
								}
								if (readCount < skipVertexCount)
								{
									++readCount;
									continue;
								}
								vertices[i] = new Vertex(readCount, line);
								++i;
								++readCount;
								if (i == myVertexCount)
								{
									break;
								}
							}

						}
						catch (IOException e)
						{
							e->printStackTrace();
						}
						findNeighbors(globalVertexCount, vertices);
						return vertices;
					}

					void ParallelOps::findNeighbors(int globalVertexCount, std::vector<Vertex*> &vertices) throw(MPIException)
					{
						std::map<int, Vertex*> vertexLabelToVertex;
						for (auto vertex : vertices)
						{
							vertexLabelToVertex[vertex->vertexLabel] = vertex;
						}
						oneIntBuffer->put(0,vertices.size());
						worldProcsComm->allGather(oneIntBuffer, 1, MPI::INT, worldIntBuffer, 1, MPI::INT);
						localVertexCounts = std::vector<int>(worldProcsCount);
						worldIntBuffer->position(0);
						worldIntBuffer->get(localVertexCounts, 0, worldProcsCount);

						localVertexDisplas = std::vector<int>(worldProcsCount);
						System::arraycopy(localVertexCounts, 0, localVertexDisplas, 1, worldProcsCount - 1);
						Arrays::parallelPrefix(localVertexDisplas, [&] (m, n)
						{
								return m + n;
						});

						// DEBUG - check localVertexCounts
						//if (worldProcRank == 1){
						//    for (int i = 0; i < worldProcsCount; ++i){
						//        System.out.println("Rank: " + i + " has " + worldIntBuffer.get(i) + " vertices");
						//    }
						//}


						// DEBUG - check localVertexDisplas
						//if (worldProcRank == 1) {
						//    System.out.println("Rank: " + worldProcRank + " displacements");
						//    for (int i = 0; i < worldProcsCount; ++i) {
						//        System.out.print(localVertexDisplas[i] + " ");
						//    }
						//}

						int displacement = localVertexDisplas[worldProcRank];
						for (int i = 0; i < vertices.size(); ++i)
						{
							vertexIntBuffer->put(i + displacement, vertices[i]->vertexLabel);
						}
						worldProcsComm->allGatherv(vertexIntBuffer, localVertexCounts, localVertexDisplas, MPI::INT);


						// DEBUG - see what ranks have what vertices
						if (debug && worldProcRank == 0)
						{
							int rank = 0;
							do
							{
								std::wcout << std::wstring(L"\n\nRank: ") << rank << std::wstring(L" has ");
								int length = localVertexCounts[rank];
								displacement = localVertexDisplas[rank];
								for (int i = 0; i < length; ++i)
								{
									std::wcout << vertexIntBuffer->get(i << displacement) << std::wstring(L" ");
								}
								++rank;

							} while (rank < worldProcsCount);
						}


						/* Just keep in mind this table and the vertexIntBuffer can be really large
						* Think of optimizations if this becomes a bottleneck */
						vertexLabelToWorldRank = std::unordered_map<*>();
						{
							int rank = 0;
							do
							{
								int length = localVertexCounts[rank];
								displacement = localVertexDisplas[rank];
								for (int i = 0; i < length; ++i)
								{
									vertexLabelToWorldRank[vertexIntBuffer->get(i + displacement)] = rank;
								}
								++rank;
							} while (rank < worldProcsCount);
						}

						// Set where out-neighbors of vertices live
						for (auto v : vertices)
						{
							std::map<int, int> &outNeighborLabelToWorldRank = v->outNeighborLabelToWorldRank;
							for (auto label : outNeighborLabelToWorldRank)
							{
								boost::optional<int> rank = vertexLabelToWorldRank[label.first];
								outNeighborLabelToWorldRank[label.first] = rank;
								VertexBuffer tempVar();
								v->outrankToSendBuffer.put(rank, &tempVar);
							}
						}

						// ----------------
						sendtoRankToMsgCountAndDestinedVertexLabels = std::map<*>();
						const std::vector<int> msgSize = {0};
						// The vertex order is important as it's implicitly assumed to reduce communication cost
						for (auto vertex : vertices)
						{
							std::map<int, std::vector<int>> inverseHT;
							vertex->outNeighborLabelToWorldRank.entrySet().forEach([&] (void *kv)
							{
									int rank = kv::getValue();
									int destinedVertexLabel = kv::getKey();
									if (!inverseHT.containsKey(rank))
									{
										inverseHT.put(rank, std::vector<*>());
									}
									inverseHT.get(rank).add(destinedVertexLabel);
							});
							inverseHT.entrySet().forEach([&] (void *kv)
							{
									int rank = kv::getKey();
									std::vector<int> list = kv::getValue();
									if (sendtoRankToMsgCountAndDestinedVertexLabels.find(rank) == sendtoRankToMsgCountAndDestinedVertexLabels.end())
									{
										sendtoRankToMsgCountAndDestinedVertexLabels[rank] = std::vector<*>();
										msgSize[0] += 2;
									}
									std::vector<int> &countAndDestinedVertexLabels = sendtoRankToMsgCountAndDestinedVertexLabels[rank];
									if (countAndDestinedVertexLabels.size() > 0)
									{
										countAndDestinedVertexLabels.set(0, countAndDestinedVertexLabels.get(0) + 1);
									}
									else
									{
										countAndDestinedVertexLabels.add(0, 1);
									}
									if (list.size() > 1)
									{
										countAndDestinedVertexLabels.add(-1*list.size());
										countAndDestinedVertexLabels.addAll(list);
										msgSize[0] += list.size() + 1;
									}
									else
									{
										countAndDestinedVertexLabels.add(list.get(0));
										msgSize[0] += 1;
									}
							});
						}
						// DEBUG - print how many message counts and what are destined vertex labels (in order) for each rank from me
						if (debug)
						{
							StringBuilder *sb = new StringBuilder();
							sb->append(L"\n--Rank: ")->append(worldProcRank)->append(L'\n');
							for (auto kv : sendtoRankToMsgCountAndDestinedVertexLabels)
							{
								std::vector<int> list = kv->getValue();
								sb->append(L" sends ")->append(list[0])->append(L" msgs to rank ")->append(kv->getKey())->append(std::wstring(L" destined ") + std::wstring(L"to vertices "));
								for (int i = 1; i < list.size(); ++i)
								{
									sb->append(list[i])->append(L" ");
								}
								sb->append(L" in order \n");
							}
							std::wstring msg = allReduce(sb->toString(), worldProcsComm);
							if (worldProcRank == 0)
							{
								std::wcout << msg << std::endl;
							}
						}
						// END ----------------

						// ################
						recvfromRankToMsgCountAndforvertexLabels = std::map<*>();
						oneIntBuffer->put(0, msgSize[0]);
						worldProcsComm->allReduce(oneIntBuffer, 1, MPI::INT, MPI::MAX);
						int maxBufferSize = oneIntBuffer->get(0) + 1; // +1 to send msgSize
						{
							IntBuffer *buffer = MPI::newIntBuffer(maxBufferSize);
							for (int rank = 0; rank < worldProcsCount; ++rank)
							{
								if (rank == worldProcRank)
								{
									buffer->position(0);
									buffer->put(msgSize[0]);
									sendtoRankToMsgCountAndDestinedVertexLabels.entrySet().forEach([&] (void *kv)
									{
					// numbers < -(globalVertexCount+1) will indicate ranks
					buffer->put(-1 * (kv::getKey() + globalVertexCount + 1));
					kv::getValue().forEach(buffer::put);
									});
								}
								worldProcsComm->bcast(buffer, maxBufferSize, MPI::INT, rank);

								buffer->position(0);
								int recvMsgSize = buffer->get();

								for (int i = 1; i <= recvMsgSize; ++i)
								{
									int val = buffer->get(i);
									if (val < 0 && val < -1 * (globalVertexCount))
									{
										// It's the rank information
										int senttoRank = (-1 * val) - (globalVertexCount + 1);
										if (senttoRank == worldProcRank)
										{
											// It'll always be a unique rank, so no need to check if exists
											std::vector<int> list;
											recvfromRankToMsgCountAndforvertexLabels[rank] = list;
											for (int j = i + 1; j <= recvMsgSize; ++j)
											{
												val = buffer->get(j);
												if (val >= 0 || (val < 0 && val >= -1 * globalVertexCount))
												{
													list.push_back(val);
												}
												else
												{
													break;
												}
											}
											break;
										}
									}
								}
							}
						}

						// DEBUG - print how many message counts and what are destined vertex labels (in order) for each rank from me
						if (debug)
						{
							StringBuilder *sb = new StringBuilder();
							sb->append(L"\n##Rank: ")->append(worldProcRank)->append(L'\n');
							for (auto kv : recvfromRankToMsgCountAndforvertexLabels)
							{
								std::vector<int> list = kv->getValue();
								sb->append(L" recvs ")->append(list[0])->append(L" msgs from rank ")->append(kv->getKey())->append(std::wstring(L" ") + std::wstring(L"for vertices "));
								for (int i = 1; i < list.size(); ++i)
								{
									sb->append(list[i])->append(L" ");
								}
								sb->append(L" in order \n");
							}
							std::wstring msg = allReduce(sb->toString(), worldProcsComm);
							if (worldProcRank == 0)
							{
								std::wcout << msg << std::endl;
							}
						}
						// END ################

						// ~~~~~~~~~~~~~~~~
						requests = std::map<*>();
						recvfromRankToRecvBuffer = std::map<*>();
						recvfromRankToMsgCountAndforvertexLabels.entrySet().forEach([&] (void *kv)
						{
								int recvfromRank = kv::getKey();
								std::vector<int> list = kv::getValue();
								int msgCount = list.get(0);
								ShortBuffer *b = MPI::newShortBuffer(BUFFER_OFFSET + msgCount * MAX_MSG_SIZE);
								recvfromRankToRecvBuffer[recvfromRank] = b;
								int currentMsg = 0;
								for (int i = 1; i < list.size();)
								{
									int val = list.get(i);
									if (val >= 0)
									{
										Vertex *vertex = vertexLabelToVertex.get(val);
										RecvVertexBuffer tempVar2(currentMsg, b, recvfromRank, MSG_SIZE_OFFSET);
										vertex->recvBuffers.push_back(&tempVar2);
										Message tempVar3();
										vertex->recvdMessages.push_back(&tempVar3);
										currentMsg++;
										++i;
									}
									else if (val < 0)
									{
										int intendedVertexCount = -1*val;
										for (int j = i + 1; j <= intendedVertexCount + i; ++j)
										{
					val = list.get(j);
					Vertex *vertex = vertexLabelToVertex.get(val);
					RecvVertexBuffer tempVar4(currentMsg, b, recvfromRank, MSG_SIZE_OFFSET);
					vertex->recvBuffers.push_back(&tempVar4);
					Message tempVar5();
					vertex->recvdMessages.push_back(&tempVar5);
										}
										i += intendedVertexCount + 1;
										currentMsg++;
									}
								}
						});

						// DEBUG
						if (debug)
						{
							StringBuilder *sb = new StringBuilder();
							sb->append(L"\n##Rank: ")->append(worldProcRank)->append(L'\n');
							for (auto vertex : vertices)
							{
								sb->append(L"  vertexLabel ")->append(vertex->vertexLabel)->append(L" recvs \n");
								vertex->recvBuffers.forEach([&] (void *recvVertexBuffer)
								{
										sb->append(L"    from rank ")->append(recvVertexBuffer::recvfromRank)->append(L" offsetFactor ")->append(recvVertexBuffer::getOffsetFactor())->append(L'\n');
								});
							}
							std::wstring msg = allReduce(sb->toString(), worldProcsComm);
							if (worldProcRank == 0)
							{
								std::wcout << msg << std::endl;
							}
						}
						// END ~~~~~~~~~~~~~~~~

						sendtoRankToSendBuffer = std::map<*>();
						sendtoRankToMsgCountAndDestinedVertexLabels.entrySet().forEach([&] (void *kv)
						{
								// +2 to store msgCount and msgSize
								int sendtoRank = kv::getKey();
								int msgCount = kv::getValue()->get(0);
								ShortBuffer *b = MPI::newShortBuffer(BUFFER_OFFSET + msgCount*MAX_MSG_SIZE);
								b->put(0, static_cast<short>(msgCount));
								sendtoRankToSendBuffer[sendtoRank] = b;
						});


						std::map<int, int> outrankToOffsetFactor;
						for (auto vertex : vertices)
						{
							Set<int> *vertexOutRanks = vertex->outrankToSendBuffer.keySet();
							vertexOutRanks->forEach([&] (void *outRank)
							{
									if (!outrankToOffsetFactor.containsKey(outRank))
									{
										outrankToOffsetFactor.put(outRank, 0);
									}
									else
									{
										outrankToOffsetFactor.put(outRank, outrankToOffsetFactor.get(outRank) + 1);
									}
									VertexBuffer *vertexSendBuffer = vertex->outrankToSendBuffer.get(outRank);
									vertexSendBuffer->setOffsetFactor(outrankToOffsetFactor.get(outRank));
									vertexSendBuffer->setBuffer(sendtoRankToSendBuffer[outRank]);
							});

						}
					}

					std::wstring ParallelOps::allReduce(const std::wstring &value, Intracomm *comm) throw(MPIException)
					{
						std::vector<int> lengths(worldProcsCount);
						int length = value.length();
						lengths[worldProcRank] = length;
						comm->allGather(lengths, 1, MPI::INT);
						std::vector<int> displas(worldProcsCount);
						displas[0] = 0;
						System::arraycopy(lengths, 0, displas, 1, worldProcsCount - 1);
						Arrays::parallelPrefix(displas, [&] (m, n)
						{
								return m + n;
						});
						int count = IntStream::of(lengths).sum(); // performs very similar to usual for loop, so no harm done
						std::vector<wchar_t> recv(count);
						System::arraycopy(value.toCharArray(), 0,recv, displas[worldProcRank], length);
						comm->allGatherv(recv, lengths, displas, MPI::CHAR);
						return std::wstring(recv);
					}

					void ParallelOps::sendMessages(int msgSize)
					{
						msgSizeToReceive = msgSize;
						sendtoRankToSendBuffer.entrySet().forEach([&] (void *kv)
						{
										// local copy
								int sendtoRank = kv::getKey();
								ShortBuffer *buffer = kv::getValue();
								buffer->put(MSG_SIZE_OFFSET, static_cast<short>(msgSize));
								try
								{
									if (sendtoRank == worldProcRank)
									{
										ShortBuffer *b = recvfromRankToRecvBuffer[worldProcRank];
										b->position(0);
										buffer->position(0);
										b->put(buffer);
									}
									else
									{
										worldProcsComm->iSend(buffer, BUFFER_OFFSET + buffer->get(MSG_COUNT_OFFSET) * msgSize, MPI::INT, sendtoRank, worldProcRank);
									}
								}
								catch (MPIException e)
								{
									e->printStackTrace();
								}
						});
					}

					void ParallelOps::recvMessages() throw(MPIException)
					{
						recvfromRankToRecvBuffer.entrySet().forEach([&] (void *kv)
						{
								int recvfromRank = kv::getKey();
								ShortBuffer *buffer = kv::getValue();
								int msgCount = recvfromRankToMsgCountAndforvertexLabels[recvfromRank][0];
								try
								{
									if (recvfromRank != worldProcRank)
									{
										requests[recvfromRank] = worldProcsComm->iRecv(buffer, BUFFER_OFFSET + msgCount * msgSizeToReceive, MPI::INT, recvfromRank, recvfromRank);
									}
								}
								catch (MPIException e)
								{
									e->printStackTrace();
								}
						});

						requests.values().forEach([&] (void *request)
						{
								try
								{
									request::waitFor();
								}
								catch (MPIException e)
								{
									e->printStackTrace();
								}
						});

						// DEBUG
						if (debug)
						{
							StringBuilder *sb = new StringBuilder();
							sb->append(L"\n%%Rank: ")->append(worldProcRank);
							recvfromRankToRecvBuffer.entrySet().forEach([&] (void *kv)
							{
									int recvfromRank = kv::getKey();
									ShortBuffer *b = kv::getValue();
									int recvdMsgSize = b->get(MSG_SIZE_OFFSET);
									if (recvdMsgSize != msgSizeToReceive)
									{
										std::exception tempVar(std::wstring(L"recvd msg size ") + recvdMsgSize + std::wstring(L" != ") + msgSizeToReceive + std::wstring(L" msgSize"));
										throw &tempVar;
									}
									int msgCount = b->get(MSG_COUNT_OFFSET);
									sb->append(L"\n recvd ")->append(msgCount)->append(L" msgs from rank ")->append(recvfromRank)->append(std::wstring(L" of ") + std::wstring(L"size "))->append(recvdMsgSize)->append(L" msg list: ");
									IntStream::range(0, msgCount).forEach([&] (void *i)
									{
					sb->append(L"[");
					IntStream::range(0, recvdMsgSize).forEach([&] (void *j)
					{
						sb->append(b->get(BUFFER_OFFSET + (i*recvdMsgSize + j)))->append(L" ");
					});
					sb->append(L"] ");
									});
							});
							sb->append(L'\n');
							std::wstring msg = allReduce(sb->toString(), worldProcsComm);
							if (worldProcRank == 0)
							{
								std::wcout << msg << std::endl;
							}
						}
					}
				}
			}
		}
	}
}
