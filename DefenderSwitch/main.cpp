#include "common.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		PrintUsage();
		return 0;
	}
	std::string inputSwitch = argv[1];

	auto success = GetSystem();
	if (!success)
	{
		std::cout << "[-] Failed to kill Defender...\n";
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