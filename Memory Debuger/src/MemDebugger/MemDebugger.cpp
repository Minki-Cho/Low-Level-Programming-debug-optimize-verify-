#include "MemDebugger.h"

#if defined(_WIN32) || defined(_WIN64)
// Works in Window

#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#include <iostream>

#pragma comment(lib, "dbghelp.lib")

AllocationRecords _allocationRecords;

//singleton
static MemoryDebugger& globalMemoryDebugger = MemoryDebugger::GetInstance();
MemoryDebugger* MemoryDebugger::instance_ = nullptr;

//Nifty-Counter
static TrackerInitializer g_memoryDebuggerInitializer;
static int _initCounter;
MemoryDebugger::MemoryDebugger()
{
    SymInitialize(GetCurrentProcess(), nullptr, true);
    SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES);
}

MemoryDebugger::~MemoryDebugger()
{
    if (_allocationRecords.empty())
        return;

    Logging_csv();

    while (!_allocationRecords.empty())
    {
        DeallocateMemory(_allocationRecords[0].userPtr, _allocationRecords[0].allocCategory);
    }
}

MemoryDebugger& MemoryDebugger::GetInstance()
{
    return *instance_;
}

void RetrieveLogInfo(void* address, const char*& fileName, DWORD& lineNumber, std::string& functionName)
{
    DWORD displacement;
    IMAGEHLP_LINE64 lineData = { 0 };
    lineData.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    if (SymGetLineFromAddr64(GetCurrentProcess(), reinterpret_cast<DWORD64>(address), &displacement, &lineData))
    {
        lineNumber = lineData.LineNumber;
        fileName = lineData.FileName;
    }
    else
    {
        lineNumber = 0;
        fileName = "";
    }

    char buffer[sizeof(SYMBOL_INFO) + MAX_PATH * sizeof(char)] = { 0 };
    PSYMBOL_INFO pSymbolInfo = reinterpret_cast<PSYMBOL_INFO>(buffer);
    pSymbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbolInfo->MaxNameLen = MAX_PATH;
    DWORD64 symbolOffset = 0;
    if (SymFromAddr(GetCurrentProcess(), reinterpret_cast<DWORD64>(address), &symbolOffset, pSymbolInfo))
    {
        functionName = pSymbolInfo->Name;
    }
    else
    {
        functionName = "";
    }
}

void MemoryDebugger::Logging_csv()
{
    FILE* logFile = nullptr;
    // append
    if (fopen_s(&logFile, "Debug_log.csv", "a") != 0)
    {
        std::cerr << "Failed to open the CSV log file." << std::endl;
        return;
    }
    fseek(logFile, 0, SEEK_END);
    bool isFileEmpty = ftell(logFile) == 0;

    if (isFileEmpty)
    {
        fprintf(logFile, "Message,File path,Line,Bytes,Address,Additional Infomation\n");
    }

    for (const AllocationRecord& allocation : _allocationRecords)
    {
        const char* fileName = "";
        DWORD lineNumber = 0;
        std::string functionName;
        RetrieveLogInfo(allocation.callerPtr, fileName, lineNumber, functionName);
        fprintf(logFile, "Memory Leak,%s,%d,%zu,0x%p,%s\n",
            fileName, lineNumber, allocation.byteSize, allocation.userPtr, functionName.c_str());
    }

    fclose(logFile);
}

TrackerInitializer::TrackerInitializer()
{
    if (++_initCounter == 1)
    {
        MemoryDebugger::instance_ = static_cast<MemoryDebugger*>(malloc(sizeof(MemoryDebugger)));
        MemoryDebugger::instance_ = new (MemoryDebugger::instance_) MemoryDebugger;
    }
}

TrackerInitializer::~TrackerInitializer()
{
    if (--_initCounter == 0)
    {
        MemoryDebugger::instance_->~MemoryDebugger();
        free(MemoryDebugger::instance_);
    }
}

void* CaptureReturnAddress()
{
    CONTEXT context = { 0 };
    // Capture the current thread's context
    RtlCaptureContext(&context);

#if defined(_M_X64) // 64-bit
    STACKFRAME64 frame = { 0 };
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrPC.Offset = context.Rip;   // 64-bit instruction pointer
    frame.AddrStack.Offset = context.Rsp; // 64-bit stack pointer
    frame.AddrFrame.Offset = context.Rbp; // 64-bit frame pointer

    int stackDepth = 4;
    for (int i = 0; i < stackDepth; ++i)
    {
        BOOL stackWalkResult = StackWalk64(
            IMAGE_FILE_MACHINE_AMD64,       // 64-bit architecture
            GetCurrentProcess(),
            GetCurrentThread(),
            &frame,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );
        if (stackWalkResult == FALSE)
        {
            break;
        }
    }
    return reinterpret_cast<void*>(frame.AddrPC.Offset);

#elif defined(_M_IX86) // 32-bit
    STACKFRAME frame = { 0 };
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrStack.Mode = AddrModeFlat;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrPC.Offset = context.Eip;   // 32-bit instruction pointer
    frame.AddrStack.Offset = context.Esp; // 32-bit stack pointer
    frame.AddrFrame.Offset = context.Ebp; // 32-bit frame pointer

    int stackDepth = 4;
    for (int i = 0; i < stackDepth; ++i)
    {
        BOOL stackWalkResult = StackWalk(
            IMAGE_FILE_MACHINE_I386,        // 32-bit architecture
            GetCurrentProcess(),
            GetCurrentThread(),
            &frame,
            &context,
            NULL,
            SymFunctionTableAccess,
            SymGetModuleBase,
            NULL
        );
        if (stackWalkResult == FALSE)
        {
            break;
        }
    }
    return reinterpret_cast<void*>(frame.AddrPC.Offset);

#else
#error "Unsupported architecture. Only x86 and x64 are supported."
#endif
}


void* MemoryDebugger::AllocateMemory(size_t size, int allocCategory)
{
    if (size >= std::numeric_limits<size_t>::max() / 2)
    {
        return nullptr;
    }
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    size_t pageSize = sysInfo.dwPageSize; // 4096
    size_t numberOfPages = size / pageSize + 1;

    void* reservedMemory = VirtualAlloc(nullptr, (numberOfPages + 1) * pageSize, MEM_RESERVE, PAGE_NOACCESS);
    void* committedAddress = (reservedMemory != nullptr) ? VirtualAlloc(reservedMemory, numberOfPages * pageSize, MEM_COMMIT, PAGE_READWRITE) : nullptr;
    std::memset(reservedMemory, 0xfdfdfdfd, pageSize);

    if (!reservedMemory || !committedAddress)
    {
        throw std::bad_alloc();
    }
    reservedMemory = committedAddress;

    void* userAddress = static_cast<char*>(reservedMemory) + ((numberOfPages * pageSize) - size);

    AllocationRecord newAllocation;
    newAllocation.userPtr = userAddress;
    newAllocation.basePtr = reservedMemory;
    newAllocation.byteSize = size;
    newAllocation.allocCategory = allocCategory;
    newAllocation.callerPtr = CaptureReturnAddress();
    _allocationRecords.push_back(newAllocation);

    return userAddress;
}

void MemoryDebugger::DeallocateMemory(void* userPtr, int allocCategory)
{
    if (userPtr == nullptr)
        return;

    auto it = std::find_if(_allocationRecords.begin(), _allocationRecords.end(),
        [userPtr](const AllocationRecord& record) {
            return record.userPtr == userPtr;
        });

    if (it != _allocationRecords.end())
    {
        if (allocCategory != it->allocCategory)
        {
            __debugbreak();
            return;
        }
        VirtualFree(it->userPtr, 0, MEM_RELEASE);
        VirtualFree(it->basePtr, 0, MEM_DECOMMIT);
        _allocationRecords.erase(it);
        return;
    }
}

void* operator new(size_t size)
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size);
    if (!allocatedMemory)
    {
        throw std::bad_alloc();
    }
    return allocatedMemory;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size);
    return allocatedMemory;
}

void* operator new[](size_t size)
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size, 1);
    if (!allocatedMemory)
    {
        throw std::bad_alloc();
    }
    return allocatedMemory;
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size, 1);
    return allocatedMemory;
}

void operator delete(void* ptr) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete[](void* ptr) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}

void operator delete[](void* ptr, size_t) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}
#elif defined(__linux__)

#include <sys/mman.h>
#include <unistd.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <limits>
#include <new>
#include <iostream>

AllocationRecords _allocationRecords;

static MemoryDebugger& globalMemoryDebugger = MemoryDebugger::GetInstance();
MemoryDebugger* MemoryDebugger::instance_ = nullptr;

// Nifty-Counter
static TrackerInitializer g_memoryDebuggerInitializer;
static int _initCounter;

MemoryDebugger::MemoryDebugger()
{
}

MemoryDebugger::~MemoryDebugger()
{
    if (!_allocationRecords.empty())
    {
        Logging_csv();

        while (!_allocationRecords.empty())
        {
            DeallocateMemory(_allocationRecords[0].userPtr, _allocationRecords[0].allocCategory);
        }
    }
}

MemoryDebugger& MemoryDebugger::GetInstance()
{
    return *instance_;
}

static void* CaptureReturnAddress()
{

    void* callstack[100];
    int depth = 4;

    if (backtrace(callstack, depth) >= 3)
    {
        return callstack[3];
    }
    else
    {
        return nullptr;
    }
}

void MemoryDebugger::Logging_csv()
{
    FILE* logFile = std::fopen("DebugLog.csv", "a");
    if (!logFile)
    {
        std::cerr << "Failed to open the CSV log file." << std::endl;
        return;
    }

    std::fseek(logFile, 0, SEEK_END);
    bool isFileEmpty = (std::ftell(logFile) == 0);

    if (isFileEmpty)
    {
        std::fprintf(logFile, "Message,File path,Line,Bytes,Address,Additional Info\n");
    }

    for (const AllocationRecord& allocation : _allocationRecords)
    {
        Dl_info dlinfo;
        std::memset(&dlinfo, 0, sizeof(dlinfo));
        if (dladdr(allocation.callerPtr, &dlinfo) && dlinfo.dli_sname)
        {
            std::fprintf(
                logFile,
                "Memory Leak,%s,%d,%zu,0x%p,%s\n",
                dlinfo.dli_fname,
                0,
                allocation.byteSize,
                allocation.userPtr,
                dlinfo.dli_sname
            );
        }
        else
        {
            std::fprintf(
                logFile,
                "Memory Leak,Unknown,%d,%zu,0x%p,Unknown\n",
                0,
                allocation.byteSize,
                allocation.userPtr
            );
        }
    }

    std::fclose(logFile);
}

void* MemoryDebugger::AllocateMemory(size_t size, int allocCategory)
{
    if (size >= std::numeric_limits<size_t>::max() / 2)
    {
        return nullptr;
    }
    const size_t pageSize = 4096;
    size_t pageCount = size / pageSize + 1;
    void* basePtr = mmap(nullptr, (pageCount + 1) * pageSize, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (!basePtr)
    {
        throw std::bad_alloc();
    }
    int result = mprotect(basePtr, pageCount * pageSize, PROT_READ | PROT_WRITE);
    if (result == -1)
    {
        throw std::bad_alloc();
    }
    void* ptr = static_cast<char*>(basePtr) + ((pageCount * pageSize) - size);

    AllocationRecord newAllocation;
    newAllocation.userPtr = ptr;
    newAllocation.basePtr = basePtr;
    newAllocation.byteSize = size;
    newAllocation.allocCategory = allocCategory;
    newAllocation.callerPtr = CaptureReturnAddress();
    _allocationRecords.push_back(newAllocation);

    return ptr;
}

void MemoryDebugger::DeallocateMemory(void* userPtr, int allocCategory)
{
    if (userPtr == nullptr)
        return;

    for (size_t i = 0; i < _allocationRecords.size(); ++i)
    {
        if (_allocationRecords[i].userPtr == userPtr)
        {
            if (allocCategory != _allocationRecords[i].allocCategory)
            {
                DEBUG_BREAKPOINT();
                return;
            }
            munmap(_allocationRecords[i].basePtr, _allocationRecords[i].byteSize);
            _allocationRecords.erase(_allocationRecords.begin() + i);
            return;
        }
    }
    DEBUG_BREAKPOINT();
    return;
}

TrackerInitializer::TrackerInitializer()
{
    if (++_initCounter == 1)
    {
        MemoryDebugger::instance_ = static_cast<MemoryDebugger*>(std::malloc(sizeof(MemoryDebugger)));
        MemoryDebugger::instance_ = new (MemoryDebugger::instance_) MemoryDebugger;
    }
}

TrackerInitializer::~TrackerInitializer()
{
    if (--_initCounter == 0)
    {
        MemoryDebugger::instance_->~MemoryDebugger();
        std::free(MemoryDebugger::instance_);
    }
}


void* operator new(size_t size)
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size);
    if (!allocatedMemory)
    {
        throw std::bad_alloc();
    }
    return allocatedMemory;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size);
    return allocatedMemory;
}

void* operator new[](size_t size)
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size, 1);
    if (!allocatedMemory)
    {
        throw std::bad_alloc();
    }
    return allocatedMemory;
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    void* allocatedMemory = globalMemoryDebugger.AllocateMemory(size, 1);
    return allocatedMemory;
}

void operator delete(void* ptr) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr);
}

void operator delete[](void* ptr) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}

void operator delete[](void* ptr, size_t) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    if (ptr == nullptr) { return; }
    globalMemoryDebugger.DeallocateMemory(ptr, 1);
}

#else
#error "Works only on Windows or Linux."
#endif