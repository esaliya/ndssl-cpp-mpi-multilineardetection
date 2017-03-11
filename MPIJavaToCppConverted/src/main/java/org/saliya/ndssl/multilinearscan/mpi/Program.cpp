#include "Program.h"
#include "../GaloisField.h"
#include "ParallelUtils.h"
#include "Constants.h"
#include "../../Utils.h"
#include "ParallelOps.h"
#include "Vertex.h"
#include "../Polynomial.h"

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
					using com::google::common::base::Optional;
					using mpi::MPI;
					using mpi::MPIException;
					using org::apache::commons::cli::CommandLine;
					using org::apache::commons::cli::HelpFormatter;
					using org::apache::commons::cli::Options;
					using Utils = org::saliya::ndssl::Utils;
					using GaloisField = org::saliya::ndssl::multilinearscan::GaloisField;
					using Polynomial = org::saliya::ndssl::multilinearscan::Polynomial;
org::apache::commons::cli::Options *Program::programOptions = new org::apache::commons::cli::Options();
std::wstring Program::inputFile;
int Program::globalVertexCount = 0;
int Program::k = 0;
double Program::epsilon = 0;
int Program::delta = 0;
double Program::alphaMax = 0;
bool Program::bind = false;
int Program::cps = 0;
int Program::roundingFactor = 0;
int Program::r = 0;
int Program::twoRaisedToK = 0;
org::saliya::ndssl::multilinearscan::GaloisField *Program::gf;
int Program::maxIterations = 0;
std::map<int, int> Program::randomAssignments;
std::vector<int> Program::completionVariables;
ParallelUtils *Program::putils;

					Program::StaticConstructor::StaticConstructor()
					{
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_VC), Constants::CMD_OPTION_LONG_NUM_VC, true, Constants::CMD_OPTION_DESCRIPTION_NUM_VC);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_K), Constants::CMD_OPTION_LONG_K, true, Constants::CMD_OPTION_DESCRIPTION_K);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_DELTA), Constants::CMD_OPTION_LONG_DELTA, true, Constants::CMD_OPTION_DESCRIPTION_DELTA);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_ALPHA), Constants::CMD_OPTION_LONG_ALPHA, true, Constants::CMD_OPTION_DESCRIPTION_ALPHA);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_EPSILON), Constants::CMD_OPTION_LONG_EPSILON, true, Constants::CMD_OPTION_DESCRIPTION_EPSILON);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_INPUT), Constants::CMD_OPTION_LONG_INPUT, true, Constants::CMD_OPTION_DESCRIPTION_INPUT);
                                            
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_NC), Constants::CMD_OPTION_LONG_NC, true, Constants::CMD_OPTION_DESCRIPTION_NC);
													programOptions->addOption(static_cast<std::wstring>(Constants::CMD_OPTION_SHORT_TC), Constants::CMD_OPTION_LONG_TC, true, Constants::CMD_OPTION_DESCRIPTION_TC);
                                            
                                            
													programOptions->addOption(Constants::CMD_OPTION_SHORT_MMAP_SCRATCH_DIR, true, Constants::CMD_OPTION_DESCRIPTION_MMAP_SCRATCH_DIR);
													programOptions->addOption(Constants::CMD_OPTION_SHORT_BIND_THREADS, true, Constants::CMD_OPTION_DESCRIPTION_BIND_THREADS);
													programOptions->addOption(Constants::CMD_OPTION_SHORT_CPS, true, Constants::CMD_OPTION_DESCRIPTION_CPS);
					}

Program::StaticConstructor Program::staticConstructor;

					void Program::main(std::vector<std::wstring> &args) throw(MPIException)
					{
						Optional<CommandLine*> *parserResult = Utils::parseCommandLineArguments(args, programOptions);

						if (!parserResult->isPresent())
						{
							std::wcout << Constants::ERR_PROGRAM_ARGUMENTS_PARSING_FAILED << std::endl;
							HelpFormatter tempVar();
							(&tempVar)->printHelp(Constants::PROGRAM_NAME, programOptions);
							return;
						}

						CommandLine *cmd = parserResult->get();
						if (!((cmd->hasOption(Constants::CMD_OPTION_SHORT_VC) || cmd->hasOption(Constants::CMD_OPTION_LONG_NUM_VC)) && (cmd->hasOption(Constants::CMD_OPTION_SHORT_K) || cmd->hasOption(Constants::CMD_OPTION_LONG_K)) && (cmd->hasOption(Constants::CMD_OPTION_SHORT_INPUT) || cmd->hasOption(Constants::CMD_OPTION_LONG_INPUT)) && (cmd->hasOption(Constants::CMD_OPTION_SHORT_NC) || cmd->hasOption(Constants::CMD_OPTION_LONG_NC))))
						{
							std::wcout << Constants::ERR_INVALID_PROGRAM_ARGUMENTS << std::endl;
							HelpFormatter tempVar2();
							(&tempVar2)->printHelp(Constants::PROGRAM_NAME, programOptions);
							return;
						}

						readConfiguration(cmd);

						ParallelOps::setupParallelism(args);
						std::vector<Vertex*> vertices = ParallelOps::setParallelDecomposition(inputFile, globalVertexCount);

						runProgram(vertices);

						ParallelOps::tearDownParallelism();
					}

					void Program::sendMessages(std::vector<Vertex*> &vertices, int superStep)
					{
						int msgSize = -1;
						for (auto vertex : vertices)
						{
							msgSize = vertex->prepareSend(superStep, ParallelOps::BUFFER_OFFSET);
						}
						ParallelOps::sendMessages(msgSize);
					}

					void Program::receiveMessages(std::vector<Vertex*> &vertices, int superStep) throw(MPIException)
					{
						ParallelOps::recvMessages();
						for (auto vertex : vertices)
						{
							vertex->processRecvd(superStep, ParallelOps::BUFFER_OFFSET);
						}
					}

					void Program::runProgram(std::vector<Vertex*> &vertices) throw(MPIException)
					{
						putils = new ParallelUtils(0, ParallelOps::worldProcRank);

						putils->printMessage(std::wstring(L"\n== ") + Constants::PROGRAM_NAME + std::wstring(L" run started on ") + Date() + std::wstring(L" ==\n"));
						putils->printMessage(configPrettyString(true));
						// get number of iterations for a target error bound (epsilon)
						double probSuccess = 0.2;
						int iter = static_cast<int>(std::round(std::log(epsilon) / std::log(1 - probSuccess)));
						putils->printMessage(std::wstring(L"  ") + iter + std::wstring(L" assignments will be evaluated for epsilon = ") + std::to_wstring(epsilon));
						double roundingFactor = 1 + delta;
						putils->printMessage(std::wstring(L"  approximation factor is ") + std::to_wstring(roundingFactor));


						double bestScore = Double::MIN_VALUE;
						long long startTime = System::currentTimeMillis();
						initComp(vertices);
						for (int i = 0; i < iter; ++i)
						{
							putils->printMessage(std::wstring(L"  Start of Loop: ") + std::to_wstring(i));
							long long loopStartTime = System::currentTimeMillis();
							bestScore = std::max(bestScore, runGraphComp(i, vertices));
							putils->printMessage(std::wstring(L"  End of Loop: ") + i + std::wstring(L" took ") + (System::currentTimeMillis() - loopStartTime) + std::wstring(L" ") + std::wstring(L"ms"));
						}
						long long endTime = System::currentTimeMillis();
						putils->printMessage(std::wstring(L"Best score: ") + std::to_wstring(bestScore));
						putils->printMessage(std::wstring(L"\n== ") + Constants::PROGRAM_NAME + std::wstring(L" run ended on ") + Date() + std::wstring(L" took ") + std::to_wstring(endTime - startTime) + std::wstring(L" ms ==\n"));
					}

					double Program::runGraphComp(int loopNumber, std::vector<Vertex*> &vertices) throw(MPIException)
					{
						initLoop(vertices);

						long long startTime = System::currentTimeMillis();
						for (int iter = 0; iter < twoRaisedToK; ++iter)
						{
							/* Super step loop*/
							int workerSteps = maxIterations + 1; // +1 to send initial values
							for (int ss = 0; ss < workerSteps; ++ss)
							{
								if (ss > 0)
								{
									receiveMessages(vertices, ss);
								}

								compute(iter, vertices, ss);

								if (ss < workerSteps - 1)
								{
									sendMessages(vertices, ss);
								}
							}
							finalizeIteration(vertices);
							if (iter % 10 == 0 || iter == twoRaisedToK - 1)
							{
								putils->printMessage(std::wstring(L"      Iteration ") + std::to_wstring(iter + 1) + std::wstring(L" of ") + twoRaisedToK + std::wstring(L" elapsed ") + (System::currentTimeMillis() - startTime) + std::wstring(L" ms"));
							}
						}
						double bestScore = finalizeIterations(vertices);
						ParallelOps::oneDoubleBuffer->put(0, bestScore);
						ParallelOps::worldProcsComm->allReduce(ParallelOps::oneDoubleBuffer, 1, MPI::DOUBLE, MPI::MAX);
						bestScore = ParallelOps::oneDoubleBuffer->get(0);
						putils->printMessage(std::wstring(L"    Loop ") + loopNumber + std::wstring(L" best score: ") + std::to_wstring(bestScore));
						return bestScore;
					}

					double Program::finalizeIterations(std::vector<Vertex*> &vertices)
					{
						double bestScore = Double::MIN_VALUE;
						for (auto vertex : vertices)
						{
							bestScore = std::max(bestScore, vertex->finalizeIterations(alphaMax, roundingFactor));
						}
						return bestScore;
					}

					void Program::finalizeIteration(std::vector<Vertex*> &vertices)
					{
						for (auto vertex : vertices)
						{
							vertex->finalizeIteration();
						}
					}

					void Program::compute(int iter, std::vector<Vertex*> &vertices, int ss)
					{
						for (auto vertex : vertices)
						{
							vertex->compute(ss, iter, completionVariables, randomAssignments);
						}
					}

					void Program::initComp(std::vector<Vertex*> &vertices) throw(MPIException)
					{
						roundingFactor = 1 + delta;
						// (1 << k) is 2 raised to the kth power
						twoRaisedToK = (1 << k);
						maxIterations = k - 1; // the original pregel loop was from 2 to k (including k), so that's (k-2)+1 times


						int myDisplas = ParallelOps::localVertexDisplas[ParallelOps::worldProcRank];
						// same as vertices.length
						int myLength = ParallelOps::localVertexCounts[ParallelOps::worldProcRank];
						for (int i = 0; i < myLength; ++i)
						{
							ParallelOps::vertexDoubleBuffer->put(myDisplas + i, vertices[i]->weight);
							vertices[i]->weight = static_cast<int>(std::ceil(Utils::logb(static_cast<int>(vertices[i]->weight) + 1, roundingFactor)));
						}

						ParallelOps::worldProcsComm->allGatherv(ParallelOps::vertexDoubleBuffer, ParallelOps::localVertexCounts, ParallelOps::localVertexDisplas, MPI::DOUBLE);

						PriorityQueue<double> *pq = new PriorityQueue<double>();
						for (int i = 0; i < globalVertexCount; ++i)
						{
							double val = ParallelOps::vertexDoubleBuffer->get(i);
							if (i < k)
							{
								pq->add(val);
							}
							else
							{
								if (pq->peek() >= val)
								{
									continue;
								}
								if (pq->size() == k)
								{
									pq->poll();
								}
								pq->offer(val);
							}
						}
						double maxWeight = 0;
						for (auto d : pq)
						{
							maxWeight += d;
						}

						r = static_cast<int>(std::ceil(Utils::logb(static_cast<int>(maxWeight) + 1, roundingFactor)));
						putils->printMessage(std::wstring(L"  Max Weight: ") + maxWeight + std::wstring(L" r: ") + r + std::wstring(L"\n"));
						// invalid input: r is negative
						if (r < 0)
						{
							IllegalArgumentException tempVar(L"  r must be a positive integer or 0");
							throw &tempVar;
						}
					}

					void Program::initLoop(std::vector<Vertex*> &vertices) throw(MPIException)
					{
						long long perLoopRandomSeed = System::currentTimeMillis();
						if (ParallelOps::worldProcRank == 0)
						{
							ParallelOps::oneLongBuffer->put(0, perLoopRandomSeed);
						}
						ParallelOps::worldProcsComm->bcast(ParallelOps::oneLongBuffer, 1, MPI::LONG, 0);
						perLoopRandomSeed = ParallelOps::oneLongBuffer->get(0);

						Random *random = new Random(perLoopRandomSeed);
						int degree = (3 + Utils::log2(k));
						gf = GaloisField::getInstance(1 << degree, Polynomial::createIrreducible(degree, random)->toBigInteger()->intValue());

						int myDisplas = ParallelOps::localVertexDisplas[ParallelOps::worldProcRank];
						// same as vertices.length
						int myLength = ParallelOps::localVertexCounts[ParallelOps::worldProcRank];
						for (int i = 0; i < globalVertexCount; ++i)
						{
							long long uniqRandomVal = random->nextLong();
							if (i >= myDisplas && i < myDisplas + myLength)
							{
								vertices[i - myDisplas]->uniqueRandomSeed = uniqRandomVal;
								// put vertex weights to be collected to compute maxweight - the sum of largest k weights
							}
						}

						// DEBUG - check each vertex has a unique random seed
						/*StringBuffer sb = new StringBuffer("Rank ");
						sb.append(ParallelOps.worldProcRank).append('\n');
						for (Vertex vertex : vertices){
						    sb.append(vertex.uniqueRandomSeed).append(" ");
						}
						System.out.println(sb.toString());*/

						randomAssignments = std::map<*>();
						ParallelOps::vertexLabelToWorldRank.keySet().forEach([&] (void *vertexLabel)
						{
								randomAssignments[vertexLabel] = random->nextInt(twoRaisedToK);
						});
						completionVariables = std::vector<int>(k - 1);
						IntStream::range(0,k - 1).forEach([&] (void *i)
						{
								completionVariables[i] = random->nextInt(twoRaisedToK);
						});
						for (auto vertex : vertices)
						{
							vertex->init(k, r, gf);
						}
					}

					void Program::readConfiguration(CommandLine *cmd)
					{
						inputFile = cmd->hasOption(Constants::CMD_OPTION_SHORT_INPUT) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_INPUT) : cmd->getOptionValue(Constants::CMD_OPTION_LONG_INPUT);
						globalVertexCount = static_cast<Integer>(cmd->hasOption(Constants::CMD_OPTION_SHORT_VC) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_VC) : cmd->getOptionValue(Constants::CMD_OPTION_LONG_NUM_VC));
						k = static_cast<Integer>(cmd->hasOption(Constants::CMD_OPTION_SHORT_K) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_K) : cmd->getOptionValue(Constants::CMD_OPTION_LONG_K));
						epsilon = static_cast<Double>(cmd->hasOption(Constants::CMD_OPTION_SHORT_EPSILON) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_EPSILON) : (cmd->hasOption(Constants::CMD_OPTION_LONG_EPSILON) ? cmd->getOptionValue(Constants::CMD_OPTION_LONG_EPSILON) : L"1.0"));
						delta = static_cast<Integer>(cmd->hasOption(Constants::CMD_OPTION_SHORT_DELTA) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_DELTA) : (cmd->hasOption(Constants::CMD_OPTION_LONG_DELTA) ? cmd->getOptionValue(Constants::CMD_OPTION_LONG_DELTA) : L"1"));
						alphaMax = static_cast<Double>(cmd->hasOption(Constants::CMD_OPTION_SHORT_ALPHA) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_ALPHA) : (cmd->hasOption(Constants::CMD_OPTION_LONG_ALPHA) ? cmd->getOptionValue(Constants::CMD_OPTION_LONG_ALPHA) : L"0.15"));

						ParallelOps::nodeCount = static_cast<Integer>(cmd->hasOption(Constants::CMD_OPTION_SHORT_NC) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_NC) : cmd->getOptionValue(Constants::CMD_OPTION_LONG_NC));
						ParallelOps::threadCount = static_cast<Integer>(cmd->hasOption(Constants::CMD_OPTION_SHORT_TC) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_TC) : (cmd->hasOption(Constants::CMD_OPTION_LONG_TC) ? cmd->getOptionValue(Constants::CMD_OPTION_LONG_TC) : L"1"));
						ParallelOps::mmapScratchDir = cmd->hasOption(Constants::CMD_OPTION_SHORT_MMAP_SCRATCH_DIR) ? cmd->getOptionValue(Constants::CMD_OPTION_SHORT_MMAP_SCRATCH_DIR) : L"/tmp";

						ParallelOps::MAX_MSG_SIZE = cmd->hasOption(Constants::CMD_OPTION_SHORT_MMS) ? static_cast<Integer>(cmd->getOptionValue(Constants::CMD_OPTION_SHORT_MMS)) : cmd->hasOption(Constants::CMD_OPTION_LONG_MMS) ? static_cast<Integer>(cmd->getOptionValue(Constants::CMD_OPTION_LONG_MMS)) : 500;

						bind = !cmd->hasOption(Constants::CMD_OPTION_SHORT_BIND_THREADS) || static_cast<Boolean>(cmd->getOptionValue(Constants::CMD_OPTION_SHORT_BIND_THREADS));
						cps = (cmd->hasOption(Constants::CMD_OPTION_SHORT_CPS)) ? static_cast<Integer>(cmd->getOptionValue(Constants::CMD_OPTION_SHORT_CPS)) : -1;
					}

					std::wstring Program::configPrettyString(bool centerAligned)
					{
						std::vector<std::wstring> params = {L"Input File", L"Global Vertex Count", L"K", L"Epsilon", L"Delta", L"Alpha Max", L"Parallel Pattern"};
						std::vector<void*> args = {inputFile, globalVertexCount, k, epsilon, delta, alphaMax, std::wstring(L"1x") + std::to_wstring(ParallelOps::worldProcsCount / ParallelOps::nodeCount) + std::wstring(L"x") + std::to_wstring(ParallelOps::nodeCount)};

						java::util::Optional<int> *maxLength = Arrays::stream(params).map(std::wstring::length).reduce(Math::max);
						if (!maxLength->isPresent())
						{
							return L"";
						}
						const int max = maxLength->get();
						const std::wstring prefix = L"  ";
						StringBuilder *sb = new StringBuilder(L"Parameters...\n");
						if (centerAligned)
						{
							IntStream::range(0, params.size()).forEach([&] (void *i)
							{
									std::wstring param = params[i];
									sb->append(getPadding(max - param.length(), prefix))->append(param)->append(L": ")->append(args[i])->append(L'\n');
							});
						}
						else
						{
							IntStream::range(0, params.size()).forEach([&] (void *i)
							{
									std::wstring param = params[i];
									sb->append(prefix)->append(param)->append(L':')->append(getPadding(max - param.length(), L""))->append(args[i])->append(L'\n');
							});
						}
						return sb->toString();
					}

					std::wstring Program::getPadding(int count, const std::wstring &prefix)
					{
						StringBuilder *sb = new StringBuilder(prefix);
						IntStream::range(0,count).forEach([&] (void *i)
						{
								sb->append(L' ');
						});
						return sb->toString();
					}
				}
			}
		}
	}
}
