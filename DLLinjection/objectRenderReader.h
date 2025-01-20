#ifndef OBJECT_RENDER_READER_H
#define OBJECT_RENDER_READER_H
#define ADDR_OBJECT_RENDER 0x023A4538

#include "includes.h"
#include "entityReader.h"


struct Entity {
    struct Entity* PtrPai; //0x0000
    struct Entity* PtrFilho; //0x0008
    struct Entity* PtrEspecie; //0x0010
    char pad_0018[24]; //0x0018
    float X; //0x0030
    char pad_0034[4]; //0x0034
    float Y; //0x0038
    float VectorX; //0x003C
    char pad_0040[4]; //0x0040
    float VectorY; //0x0044
    char pad_0048[64]; //0x0048
    struct EntitySkill* currentSkillPtr; //0x0088
    char pad_0090[232]; //0x0090
    struct EntityName* nameEntityPtr; //0x0178
};

struct StageRange {
    float p1x, p1y, p2x, p2y;
};

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
        ptrProcessReader = std::make_unique<ProcessReader>();
        ptrMemoryReader = std::make_unique<MemoryReader>();
        ptrEntityReader = std::make_unique<EntityReader>(this);
    }

    ~ObjectRenderReader() = default;

    bool isReadAllRootsAddrs(HANDLE processHandle);
    bool readRootEntityAddress(HANDLE processHandle); //Você lidou com o jogo de 2010 retornar bool sem is é normal '-'
 
    ProcessReader* getPtrProcessReader() const { return ptrProcessReader.get(); }
    MemoryReader* getPtrMemoryReader() const { return ptrMemoryReader.get(); }
    EntityReader* getPtrEntityReader() const { return ptrEntityReader.get(); }
    uintptr_t getRootEntityAddress() const { return rootEntityAddress; }

private:
    std::unique_ptr<EntityReader> ptrEntityReader;
    std::unique_ptr<ProcessReader> ptrProcessReader;
    std::unique_ptr<MemoryReader> ptrMemoryReader;
    uintptr_t rootEntityAddress = 0;
};

#endif
