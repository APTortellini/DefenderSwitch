#include "common.hpp"

NTSTATUS ImpersonateAndStart()
{
	std::cout << "[*] Trying to start Windows Defender...\n";
	// load NtDll and resolve the addresses of NtImpersonateThread()
	wil::unique_hmodule ntdllModule(LoadLibraryW(L"ntdll.dll"));
	if (!ntdllModule.get())
	{
		std::cout << "[-] Couldn't open a handle to ntdll! Error: 0x" << std::hex << GetLastError() << std::endl;
		return 1;
	}

	pNtImpersonateThread NtImpersonateThread = (pNtImpersonateThread)GetProcAddress(ntdllModule.get(), "NtImpersonateThread");
	if (!NtImpersonateThread)
	{
		std::cout << "[-] Couldn't resolve NtImpersonateThread address! Error: 0x" << std::hex << GetLastError() << std::endl;
		return 1;
	}
		// steps:
		// 1. start the trustedinstaller service & process
		// 2. steal the trustedinstaller process token
		// 3. impersonate TrustedInstaller's token
		// 4. start defender

	// step 1 - open the service manager, then start TrustedInstaller
	wil::unique_schandle svcManager(OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS));
	if (!svcManager.get())
	{
		Error(GetLastError());
		return 1;
	}
	else std::cout << "[+] Opened handle to the SCM!\n";

	wil::unique_schandle trustedInstSvc(OpenServiceW(svcManager.get(), L"TrustedInstaller", SERVICE_START));
	if (!trustedInstSvc.get())
	{
		Error(GetLastError());
		std::cout << "[-] Couldn't get a handle to the TrustedInstaller service...\n";
		return 1;
	}
	else std::cout << "[+] Opened handle to the TrustedInstaller service!\n";

	auto success = StartServiceW(trustedInstSvc.get(), 0, nullptr);
	if (!success && GetLastError() != 0x420) // 0x420 is the error code returned when the service is already running
	{
		Error(GetLastError());
		std::cout << "[-] Couldn't start TrustedInstaller service...\n";
		return 1;
	}
	else std::cout << "[+] Successfully started the TrustedInstaller service!\n";

	auto trustedInstPid = FindPid(L"TrustedInstaller.exe");
	if (trustedInstPid == ERROR_FILE_NOT_FOUND)
	{
		std::cout << "[-] Couldn't find the TrustedInstaller process...\n";
		return 1;
	}

	auto trustedInstThreadId = GetFirstThreadID(trustedInstPid);
	if (trustedInstThreadId == ERROR_FILE_NOT_FOUND || trustedInstThreadId == 0)
	{
		std::cout << "[-] Couldn't find TrustedInstaller process' first thread...\n";
		return 1;
	}

	wil::unique_handle hTrustedInstThread(OpenThread(THREAD_DIRECT_IMPERSONATION, false, trustedInstThreadId));
	if (!hTrustedInstThread.get())
	{
		std::cout << "[-] Couldn't open a handle to the TrustedInstaller process' first thread...\n";
		return 1;
	}
	else std::cout << "[+] Opened a THREAD_DIRECT_IMPERSONATION handle to the TrustedInstaller process' first thread!\n";

	// step 3 - impersonate the thread to get TrustedInstaller privilege for the current thread
	SECURITY_QUALITY_OF_SERVICE sqos = {};
	sqos.Length = sizeof(sqos);
	sqos.ImpersonationLevel = SecurityImpersonation;
	auto status = NtImpersonateThread(GetCurrentThread(), hTrustedInstThread.get(), &sqos);
	if (status == STATUS_SUCCESS) std::cout << "[+] Successfully impersonated TrustedInstaller token!\n";
	else
	{
		Error(RtlNtStatusToDosError(status));
		std::cout << "[-] Failed to impersonate TrustedInstaller...\n";
		return 1;
	}

	// step 4 - start Defender :)
	wil::unique_schandle winDefendSvc(OpenServiceW(svcManager.get(), L"WinDefend", SERVICE_START));
	if (!winDefendSvc.get())
	{
		Error(GetLastError());
		std::cout << "[-] Couldn't get a handle to the WinDefend service...\n";
		return 1;
	}
	else std::cout << "[+] Opened handle to the WinDefend service!\n";

	success = StartServiceW(winDefendSvc.get(), 0, nullptr);
	if (!success)
	{
		Error(GetLastError());
		std::cout << "[-] Couldn't start WinDefend service...\n";
		return 1;
	}
	else std::cout << "[+] Successfully started the WinDefend service!\n";
	return status;
}