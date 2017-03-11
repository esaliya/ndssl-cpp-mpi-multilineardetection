#include "Utils.h"

namespace org
{
	namespace saliya
	{
		namespace ndssl
		{
			using com::google::common::base::Optional;
			using namespace org::apache::commons::cli;

			Optional<CommandLine*> *Utils::parseCommandLineArguments(std::vector<std::wstring> &args, Options *opts)
			{

				CommandLineParser *optParser = new GnuParser();

				try
				{
					return Optional::fromNullable(optParser->parse(opts, args));
				}
				catch (ParseException e)
				{
					e->printStackTrace();
				}
				return Optional::fromNullable(nullptr);
			}

			double Utils::BJ(double alpha, int anomalousCount, int setSize)
			{
				return setSize * KL((static_cast<double>(anomalousCount)) / setSize, alpha);
			}

			double Utils::KL(double a, double b)
			{
				assert(a >= 0 && a <= 1);
				assert(b > 0 && b < 1);

				// corner cases
				if (a == 0)
				{
					return std::log(1 / (1 - b));
				}
				if (a == 1)
				{
					return std::log(1 / b);
				}

				return a * std::log(a / b) + (1 - a) * std::log((1 - a) / (1 - b));
			}

			double Utils::logb(double n, double b)
			{
				return std::log(n) / std::log(b);
			}

			int Utils::log2(int x)
			{
				if (x <= 0)
				{
					IllegalArgumentException tempVar(std::wstring(L"Error. Argument must be greater than 0. Found ") + std::to_wstring(x));
					throw &tempVar;
				}
				int result = 0;
				x >>= 1;
				while (x > 0)
				{
					result++;
					x >>= 1;
				}
				return result;
			}

			std::wstring Utils::formatElapsedMillis(long long elapsed)
			{
				std::wstring format = L"%dd:%02dH:%02dM:%02dS:%03dmS";
				short millis = static_cast<short>(elapsed % (1000.0));
				elapsed = (elapsed - millis) / 1000; // remaining elapsed in seconds
				char seconds = static_cast<char>(elapsed % 60.0);
				elapsed = (elapsed - seconds) / 60; // remaining elapsed in minutes
				char minutes = static_cast<char>(elapsed % 60.0);
				elapsed = (elapsed - minutes) / 60; // remaining elapsed in hours
				char hours = static_cast<char>(elapsed % 24.0);
				long long days = (elapsed - hours) / 24; // remaining elapsed in days
				return std::wstring::format(format, days, hours, minutes, seconds, millis);
			}
		}
	}
}
