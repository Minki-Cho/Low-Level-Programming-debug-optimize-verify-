#ifndef MEMORY_DEBUGGER_H
#define MEMORY_DEBUGGER_H

#include <vector>
#include "Common.h"

struct AllocationRecord
{
    void* userPtr = nullptr;
    void* basePtr = nullptr;
    void* callerPtr = nullptr;
    size_t byteSize = 0;
    int allocCategory = 0;
};

using AllocationRecords = std::vector<AllocationRecord, Mallocator<AllocationRecord>>;

class MemoryDebugger
{
public:
    MemoryDebugger();
    ~MemoryDebugger();

    static MemoryDebugger& GetInstance();
    void Logging_csv();
    void* AllocateMemory(size_t size, int allocCategory = 0);
    void DeallocateMemory(void* userPtr, int allocCategory = 0);
private:
    static MemoryDebugger* instance_;
    friend struct TrackerInitializer;
};

struct TrackerInitializer
{
    TrackerInitializer();
    ~TrackerInitializer();
};

void* operator new(size_t size);
void* operator new(size_t size, const std::nothrow_t&) noexcept;
void* operator new[](size_t size);
void* operator new[](size_t size, const std::nothrow_t&) noexcept;
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, size_t) noexcept;
void operator delete(void* ptr, const std::nothrow_t&) noexcept;
void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, size_t) noexcept;
void operator delete[](void* ptr, const std::nothrow_t&) noexcept;

#endif
