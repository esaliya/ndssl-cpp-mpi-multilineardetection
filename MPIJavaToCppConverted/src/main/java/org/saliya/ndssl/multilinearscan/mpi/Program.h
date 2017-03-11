#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include "stringbuilder.h"

//JAVA TO C++ CONVERTER NOTE: Forward class declarations:
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { class GaloisField; } } } }
namespace org { namespace saliya { namespace ndssl { namespace multilinearscan { namespace mpi { class ParallelUtils; } } } } }
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

					using mpi::MPIException;
					using org::apache::commons::cli::CommandLine;
					using org::apache::commons::cli::Options;
					using GaloisField = org::saliya::ndssl::multilinearscan::GaloisField;


					/// <summary>
					/// Saliya Ekanayake on 2/21/17.
					/// </summary>
					class Program
					{
					private:
						static Options *programOptions;
						static std::wstring inputFile;
						static int globalVertexCount;
						static int k;
						static double epsilon;
						static int delta;
						static double alphaMax;
						static bool bind;
						static int cps;

						static int roundingFactor;
						static int r;
						static int twoRaisedToK;
						static GaloisField *gf;
						static int maxIterations;
						static std::map<int, int> randomAssignments;
						static std::vector<int> completionVariables;

						static ParallelUtils *putils;

										private:
											class StaticConstructor
											{
											public:
												StaticConstructor();
											};

										private:
											static Program::StaticConstructor staticConstructor;


						static void main(std::vector<std::wstring> &args) throw(MPIException);

						static void sendMessages(std::vector<Vertex*> &vertices, int superStep);


						static void receiveMessages(std::vector<Vertex*> &vertices, int superStep) throw(MPIException);

						static void runProgram(std::vector<Vertex*> &vertices) throw(MPIException);

						static double runGraphComp(int loopNumber, std::vector<Vertex*> &vertices) throw(MPIException);

						static double finalizeIterations(std::vector<Vertex*> &vertices);

						static void finalizeIteration(std::vector<Vertex*> &vertices);

						static void compute(int iter, std::vector<Vertex*> &vertices, int ss);

						static void initComp(std::vector<Vertex*> &vertices) throw(MPIException);

						static void initLoop(std::vector<Vertex*> &vertices) throw(MPIException);


						static void readConfiguration(CommandLine *cmd);

					public:
						static std::wstring configPrettyString(bool centerAligned);

					private:
						static std::wstring getPadding(int count, const std::wstring &prefix);
					};

				}
			}
		}
	}
}
