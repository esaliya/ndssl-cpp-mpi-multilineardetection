#pragma once

#include <string>
#include <vector>
#include <cmath>

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{

			using com::google::common::base::Optional;
			using namespace org::apache::commons::cli;

			/// <summary>
			/// Saliya Ekanayake on 2/22/17.
			/// </summary>
			class Utils
			{
				/// <summary>
				/// Parse command line arguments
				/// </summary>
				/// <param name="args"> Command line arguments </param>
				/// <param name="opts"> Command line options </param>
				/// <returns> An <code>Optional&lt;CommandLine&gt;</code> object </returns>
			public:
				static Optional<CommandLine*> *parseCommandLineArguments(std::vector<std::wstring> &args, Options *opts);

				static double BJ(double alpha, int anomalousCount, int setSize);

				static double KL(double a, double b);

				static double logb(double n, double b);

				static int log2(int x);

				static std::wstring formatElapsedMillis(long long elapsed);


			};

		}
	}
}
