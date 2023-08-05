#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include <cstdint>

class Memory
{
private:
	uintptr_t processId = 0;
	void* processHandle = nullptr;

public:
	Memory(const char *processName)
	{
		PROCESSENTRY32 entry = {};
		entry.dwSize = sizeof(PROCESSENTRY32);
		const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		while (Process32Next(snapshot, &entry))
		{
			if (strcmp(processName, entry.szExeFile) == 0)
			{
				processId = entry.th32ProcessID;
				processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
				break;
			}
		}
	}

	uintptr_t GetModuleAddressByName(const char *moduleName)
	{
		MODULEENTRY32 entry = {};
		entry.dwSize = sizeof(MODULEENTRY32);
		const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

		uintptr_t address = 0;

		while (Module32Next(snapshot, &entry))
		{
			if (strcmp(moduleName, entry.szModule) == 0)
			{
				address = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		return address;
	}

	template <class T>
	const T read(const uintptr_t& address) const noexcept
	{
		T value = {};
		ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}

	template <class T>
	const void write(uintptr_t address, T value) const noexcept
	{
		WriteProcessMemory(processHandle, (LPVOID)address, &value, sizeof(T), NULL);
	}
};
