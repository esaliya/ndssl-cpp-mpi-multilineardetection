#include "Constants.h"

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
					using org::apache::commons::cli::Options;
std::wstring Constants::PROGRAM_NAME = L"MultilinearScan";
const std::wstring Constants::CMD_OPTION_SHORT_VC = L"v";
const std::wstring Constants::CMD_OPTION_LONG_NUM_VC = L"vertexCount";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_NUM_VC = L"Vertex count";
const std::wstring Constants::CMD_OPTION_SHORT_K = L"k";
const std::wstring Constants::CMD_OPTION_LONG_K = L"k";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_K = L"Motif size";
const std::wstring Constants::CMD_OPTION_SHORT_DELTA = L"d";
const std::wstring Constants::CMD_OPTION_LONG_DELTA = L"delta";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_DELTA = L"Delta";
const std::wstring Constants::CMD_OPTION_SHORT_ALPHA = L"a";
const std::wstring Constants::CMD_OPTION_LONG_ALPHA = L"alpha";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_ALPHA = L"Alpha max";
const std::wstring Constants::CMD_OPTION_SHORT_EPSILON = L"e";
const std::wstring Constants::CMD_OPTION_LONG_EPSILON = L"epsilon";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_EPSILON = L"Epsilon";
const std::wstring Constants::CMD_OPTION_SHORT_INPUT = L"i";
const std::wstring Constants::CMD_OPTION_LONG_INPUT = L"input";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_INPUT = L"Input file";
const std::wstring Constants::CMD_OPTION_SHORT_NC = L"nc";
const std::wstring Constants::CMD_OPTION_LONG_NC = L"nodeCount";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_NC = L"Node count";
const std::wstring Constants::CMD_OPTION_SHORT_TC = L"tc";
const std::wstring Constants::CMD_OPTION_LONG_TC = L"threadCount";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_TC = L"Thread count";
const std::wstring Constants::CMD_OPTION_SHORT_MMS = L"mms";
const std::wstring Constants::CMD_OPTION_LONG_MMS = L"maxMsgSize";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_MMS = L"Maximum message size (#shorts)";
const std::wstring Constants::CMD_OPTION_SHORT_MMAP_SCRATCH_DIR = L"mmapdir";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_MMAP_SCRATCH_DIR = std::wstring(L"Scratch directory to store memmory mapped files. A node local ") + std::wstring(L"volatile storage like tmpfs is advised for this");
const std::wstring Constants::CMD_OPTION_SHORT_BIND_THREADS = L"bind";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_BIND_THREADS = L"Bind threads to cores";
const std::wstring Constants::CMD_OPTION_SHORT_CPS = L"cps";
const std::wstring Constants::CMD_OPTION_DESCRIPTION_CPS = L"Cores per socket";
const std::wstring Constants::ERR_PROGRAM_ARGUMENTS_PARSING_FAILED = L"Argument parsing failed!";
const std::wstring Constants::ERR_INVALID_PROGRAM_ARGUMENTS = L"Invalid program arguments!";
const std::wstring Constants::ERR_EMPTY_FILE_NAME = L"File name is null or empty!";
				}
			}
		}
	}
}
