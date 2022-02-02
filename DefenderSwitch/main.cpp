#include "common.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		PrintUsage();
		return 0;
	}
	std::string inputSwitch = argv[1];

	wil::unique_handle tokenHandle;
	auto success = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &tokenHandle);
	if (!success)
	{
		std::cout << "[-] Failed to open current process token, exiting...\n";
		return 0;
	}

	success = SetPrivilege(tokenHandle.get(), L"SeDebugPrivilege", true);
	if (!success)
	{
		std::cout << "[-] Failed to enable SeDebugPrivilege, exiting...\n";
		return 0;
	}

	if (inputSwitch.find("off") != -1)
	{
		std::thread stopThread(ImpersonateAndStop);
		stopThread.join();
	}

	else if (inputSwitch.find("on") != -1)
	{
		std::thread startThread(ImpersonateAndStart);
		startThread.join();
	}
	else PrintUsage();

	return 0;
}