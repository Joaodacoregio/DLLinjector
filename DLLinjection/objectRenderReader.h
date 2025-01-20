#ifndef OBJECT_RENDER_READER_H
#define OBJECT_RENDER_READER_H
#define ADDR_OBJECT_RENDER 0x023A4538

#include <windows.h>
#include <memory>
#include <vector>
#include <string>

// Forward declarations for undefined classes
class EntityReader;

// Process reader class for handling process and memory operations
class ProcessReader {
public:
    ProcessReader() {
        setGameProcessId();
        setProcessHandle(OpenProcessHandle(getGameProcessId()));
    }

    HANDLE OpenProcessHandle(DWORD pid);
    uintptr_t GetModuleBaseAddress(HANDLE hProcess, const std::string& moduleName);
    void setProcessHandle(HANDLE hProcess) { processHandle = hProcess; }
    DWORD GetProcessIdByWindowName(const std::string& windowName);
    void setGameProcessId() { gameProcessId = GetProcessIdByWindowName("Lunia"); }
    DWORD getGameProcessId() const { return gameProcessId; }
    HANDLE getProcessHandle() const { return processHandle; }

private:
    HANDLE processHandle = nullptr;
    DWORD gameProcessId = 0;
};

// Memory reader class for reading memory from the game process
class MemoryReader {
public:
    // Reads memory from the specified process at the given address into the buffer
    bool ReadMemory(HANDLE hProcess, uintptr_t address, void* buffer, size_t size);

    // Reads a pointer with offsets from the specified process
    uintptr_t ReadPointerWithOffsets(HANDLE hProcess, uintptr_t base, const std::vector<uintptr_t>& offsets);
};

// Class for reading all rendered objects in the game (entities, NPCs, players, drops, etc.)
class ObjectRenderReader {
public:
    ObjectRenderReader() {
        processReader = std::make_unique<ProcessReader>();
        memoryReader = std::make_unique<MemoryReader>();
        entityReader = std::make_unique<EntityReader>(this);
    }

    ~ObjectRenderReader() = default;

    bool isReadAllRootsAddrs(HANDLE processHandle);
    bool readRootEntityAddress(HANDLE processHandle); //Você lidou com o jogo de 2010 retornar bool sem is é normal '-'
 
    ProcessReader* getProcessReader() const { return processReader.get(); }
    MemoryReader* getMemoryReader() const { return memoryReader.get(); }
    EntityReader* getEntityReader() const { return entityReader.get(); }

    uintptr_t getRootEntityAddress() const { return rootEntityAddress; }

private:
    std::unique_ptr<EntityReader> entityReader;
    std::unique_ptr<ProcessReader> processReader;
    std::unique_ptr<MemoryReader> memoryReader;
    uintptr_t rootEntityAddress = 0;
};

#endif
