#include "objectRenderReader.h"

HANDLE ProcessReader::OpenProcessHandle(DWORD pid) {
    return OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
}

DWORD ProcessReader::GetProcessIdByWindowName(const std::string& windowName) {
    // Converte o nome da janela para o formato necessário (LPCSTR -> LPCTSTR)
    HWND hWnd = FindWindowA(NULL, windowName.c_str());
    if (hWnd == NULL) {
        std::cerr << "Janela não encontrada: " << windowName << std::endl;
        return 0;
    }

    DWORD pid;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid;
}

uintptr_t ProcessReader::GetModuleBaseAddress(HANDLE hProcess, const std::string& moduleName) {
    uintptr_t moduleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &moduleEntry)) {
            do {
#ifdef UNICODE
                // Converte `moduleName` (std::string) para `std::wstring` para a comparação
                if (std::wstring(moduleName.begin(), moduleName.end()) == moduleEntry.szModule) {
#else
                // Comparação direta com `std::string` em modo ANSI
                if (moduleName == std::string(moduleEntry.szModule)) {
#endif
                    moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                    break;
                }
                } while (Module32Next(hSnapshot, &moduleEntry));
            }
        }
    return moduleBaseAddress;
    }



bool MemoryReader::ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, size_t size) {
    return ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, nullptr);
}

uintptr_t MemoryReader::ReadPointerWithOffsets(HANDLE hProcess, uintptr_t base, const std::vector<uintptr_t>&offsets) {
    uintptr_t address = base;
    for (uintptr_t offset : offsets) {
        if (!ReadMemory(hProcess, address, &address, sizeof(address))) {
            return 0;  // Retorna 0 em caso de falha
        }
        address += offset;
    }
    return address;
}



bool ObjectRenderReader::readRootEntityAddress(HANDLE ReaderHProcess) {
    if (ReaderHProcess) {
        uintptr_t baseAddress = ptrProcessReader->GetModuleBaseAddress(ReaderHProcess, "LuniaClient.exe");
        std::vector<uintptr_t> offsets = { 0x88 , 0x3C0 , 0x120 ,0x00 };

        uintptr_t finalAddress = ptrMemoryReader->ReadPointerWithOffsets(ReaderHProcess, baseAddress + ADDR_OBJECT_RENDER, offsets);

        if (finalAddress) {
            int value;
            if (ptrMemoryReader->ReadMemory(ReaderHProcess, finalAddress, &value, sizeof(value))) {
                rootEntityAddress = finalAddress;
                return true;
            }
            else {
                std::cerr << "Falha na leitura de memória!" << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "Falha ao calcular o endereço final!" << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Falha ao abrir o processo!" << std::endl;
        return false;
    }

}


bool ObjectRenderReader::isReadAllRootsAddrs(HANDLE hProcess) {
    //Add more hear
    if (readRootEntityAddress(hProcess))
    {
        return true;
    }

    return false;
}