#ifndef OBJECT_RENDER_READER_H
#define OBJECT_RENDER_READER_H
#define ADDR_OBJECT_RENDER 0x023A4538
#define ADDRS_PLAYERDATA 0x0236E900


#include "includes.h"
#include "entityReader.h"
#include "playerReader.h"

 
 

// Forward declarations for undefined classes
class EntityReader;
class PlayerReader;
struct Entity;
struct Obstacle;

// Process reader class for handling process and memory operations
class ProcessReader {
public:
    ProcessReader() {
        setGameProcessId();
        setProcessHandle(OpenProcessHandle(getGameProcessId()));
    }

    HANDLE OpenProcessHandle(DWORD pid);
    uintptr_t GetModuleBaseAddress(HANDLE hProcess, const std::string& moduleName); //this is a bug because i used utf
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
        ptrProcessReader = std::make_unique<ProcessReader>();
        ptrMemoryReader = std::make_unique<MemoryReader>();
        ptrEntityReader = std::make_unique<EntityReader>(this);
        ptrPlayerReader = std::make_unique<PlayerReader>(this);
    }

    ~ObjectRenderReader() = default;

    bool isReadAllRootsAddrs(HANDLE processHandle);
    bool readRootEntityAddress(HANDLE processHandle); //Você lidou com o jogo de 2010 retornar bool sem is é normal '-'
    bool readRootPlayerAddress(HANDLE processHandle);  
 
    ProcessReader* getPtrProcessReader() const { return ptrProcessReader.get(); }
    MemoryReader* getPtrMemoryReader() const { return ptrMemoryReader.get(); }
    EntityReader* getPtrEntityReader() const { return ptrEntityReader.get(); }
    PlayerReader* getPtrPlayerReader() const { return ptrPlayerReader.get(); }
    uintptr_t getRootEntityAddress() const { return rootEntityAddress; }
    uintptr_t getRootPlayerAddress() const { return rootPlayerAddress; }

private:
    std::unique_ptr<EntityReader> ptrEntityReader;
    std::unique_ptr<PlayerReader> ptrPlayerReader;
    std::unique_ptr<ProcessReader> ptrProcessReader;
    std::unique_ptr<MemoryReader> ptrMemoryReader;
    uintptr_t rootEntityAddress = 0;
    uintptr_t rootPlayerAddress = 0;
};

#endif
