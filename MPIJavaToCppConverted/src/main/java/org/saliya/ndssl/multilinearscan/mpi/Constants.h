#pragma once

#include <string>

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


					/// <summary>
					/// Saliya Ekanayake on 2/22/17.
					/// </summary>
					class Constants
					{

					public:
						static std::wstring PROGRAM_NAME;

						static const std::wstring CMD_OPTION_SHORT_VC;
						static const std::wstring CMD_OPTION_LONG_NUM_VC;
						static const std::wstring CMD_OPTION_DESCRIPTION_NUM_VC;

						static const std::wstring CMD_OPTION_SHORT_K;
						static const std::wstring CMD_OPTION_LONG_K;
						static const std::wstring CMD_OPTION_DESCRIPTION_K;

						static const std::wstring CMD_OPTION_SHORT_DELTA;
						static const std::wstring CMD_OPTION_LONG_DELTA;
						static const std::wstring CMD_OPTION_DESCRIPTION_DELTA;

						static const std::wstring CMD_OPTION_SHORT_ALPHA;
						static const std::wstring CMD_OPTION_LONG_ALPHA;
						static const std::wstring CMD_OPTION_DESCRIPTION_ALPHA;

						static const std::wstring CMD_OPTION_SHORT_EPSILON;
						static const std::wstring CMD_OPTION_LONG_EPSILON;
						static const std::wstring CMD_OPTION_DESCRIPTION_EPSILON;

						static const std::wstring CMD_OPTION_SHORT_INPUT;
						static const std::wstring CMD_OPTION_LONG_INPUT;
						static const std::wstring CMD_OPTION_DESCRIPTION_INPUT;

						static const std::wstring CMD_OPTION_SHORT_NC;
						static const std::wstring CMD_OPTION_LONG_NC;
						static const std::wstring CMD_OPTION_DESCRIPTION_NC;

						static const std::wstring CMD_OPTION_SHORT_TC;
						static const std::wstring CMD_OPTION_LONG_TC;
						static const std::wstring CMD_OPTION_DESCRIPTION_TC;

						static const std::wstring CMD_OPTION_SHORT_MMS;
						static const std::wstring CMD_OPTION_LONG_MMS;
						static const std::wstring CMD_OPTION_DESCRIPTION_MMS;

						static const std::wstring CMD_OPTION_SHORT_MMAP_SCRATCH_DIR;
						static const std::wstring CMD_OPTION_DESCRIPTION_MMAP_SCRATCH_DIR;

						static const std::wstring CMD_OPTION_SHORT_BIND_THREADS;
						static const std::wstring CMD_OPTION_DESCRIPTION_BIND_THREADS;

						static const std::wstring CMD_OPTION_SHORT_CPS;
						static const std::wstring CMD_OPTION_DESCRIPTION_CPS;



						static const std::wstring ERR_PROGRAM_ARGUMENTS_PARSING_FAILED;
						static const std::wstring ERR_INVALID_PROGRAM_ARGUMENTS;
						static const std::wstring ERR_EMPTY_FILE_NAME;

					};

				}
			}
		}
	}
}
