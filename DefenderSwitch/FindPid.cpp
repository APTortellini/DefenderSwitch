#include "common.hpp"

DWORD FindPid(_In_ std::wstring imageName)
{
	// create snapshot of processes using RAII classes
	wil::unique_handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));

	if (!snapshot.get())
	{
		Error(::GetLastError());
		return ERROR_FILE_NOT_FOUND;
	}

	PROCESSENTRY32W processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32W);
	auto status = Process32FirstW(snapshot.get(), &processEntry); // start enumerating from the first process
	if (!status)
	{
		Error(::GetLastError());
		return ERROR_FILE_NOT_FOUND;
	}

	std::transform(imageName.begin(), imageName.end(), imageName.begin(), towlower);
	do
	{
		std::wstring processImage = processEntry.szExeFile;
		std::transform(processImage.begin(), processImage.end(), processImage.begin(), towlower);

		if (processImage == imageName)
		{
			std::wcout << L"[+] Found process " << processEntry.szExeFile << " with PID " << processEntry.th32ProcessID << std::endl; // when input process is found return its PID to the caller
			return processEntry.th32ProcessID;
		}
	} while (Process32NextW(snapshot.get(), &processEntry));

	return ERROR_FILE_NOT_FOUND;
}