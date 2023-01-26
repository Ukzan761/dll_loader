// dll_loader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <Psapi.h>

int wmain(int argc, wchar_t* argv[])
{
    std::cout << "Hello World!\n";

    if (argc == 3)
    {
	    const wchar_t* dll_file_path = argv[1];
	    const wchar_t* target_process_name = argv[2];

	    DWORD process_ids[1024]{};
    	DWORD cb{};
    	if (EnumProcesses(process_ids, sizeof process_ids, &cb))
    	{
    		for (DWORD i = 0; i < cb / sizeof(DWORD); i++)
    		{
    			if (const HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, 0, process_ids[i]))
    			{
    				wchar_t exe_name[MAX_PATH]{};
    				if (GetProcessImageFileName(process, exe_name, sizeof exe_name / sizeof(wchar_t)))
    				{
                        if (!wcsncmp(wcsrchr(exe_name, '\\') + 1, target_process_name, wcslen(target_process_name)))
                        {
                            if (void* dll_path_str = VirtualAllocEx(process, nullptr, MAX_PATH * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))
                            {
                                FlushInstructionCache(process, nullptr, 0);

                                wchar_t full_dll_path[MAX_PATH]{};
                                if (GetFullPathName(dll_file_path, sizeof full_dll_path / sizeof(wchar_t), full_dll_path, nullptr))
                                {
                                    if (WriteProcessMemory(process, dll_path_str, full_dll_path, MAX_PATH * sizeof(wchar_t), nullptr))
                                    {
	                                    if (const HANDLE thread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibrary), dll_path_str, 0, nullptr))
	                                    {
                                            std::cout << "Loaded" << std::endl;
                                            WaitForSingleObject(thread, INFINITE);
                                            CloseHandle(thread);
	                                    }
                                    }
                                }

                                VirtualFreeEx(process, dll_path_str, 0, MEM_RELEASE);
                            }
                        }
    				}

    				CloseHandle(process);
    			}
    		}
    	}
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
