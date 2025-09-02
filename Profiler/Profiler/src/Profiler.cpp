#include "Profiler.h"
#include "Internal.h"
#include "Profiler.h"
#include <Windows.h>
#include <DbgHelp.h>
#include <intrin.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

#pragma comment(lib, "dbghelp.lib")

#define UNUSED(x) [&x]{}()

Profiler& Profiler::GetInstance()
{
    static Profiler instance;
    return instance;
}

static PSYMBOL_INFO GetSymbol(DWORD64 address, PSYMBOL_INFO buff)
{
    PDWORD64 displacement = 0;
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buff;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;
    SymFromAddr(GetCurrentProcess(), address, displacement, symbol);
    return symbol;
}

void Profiler::Init()
{
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (!SymInitialize(GetCurrentProcess(), NULL, true)) {
        std::cerr << "Profiler: SymInitialize failed!\n";
        return;
    }
    timerStart_ = std::chrono::steady_clock::now();
}

void Profiler::Sample()
{
    auto nextSampleTime = std::chrono::steady_clock::now();

    while (SampleNumber_ < Maxcount && !stopFlag_) {
        nextSampleTime += std::chrono::milliseconds(1);

        auto startTime = std::chrono::steady_clock::now();
        auto timerEnd = std::chrono::steady_clock::now();
        timer_ = std::chrono::duration_cast<std::chrono::seconds>(timerEnd - timerStart_).count();

        MainThread_ = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
            0,
            MainThreadId_
        );

        if (MainThread_ == 0 || SuspendThread(MainThread_) == (DWORD)-1)
            return;

        CONTEXT context = { 0 };
        context.ContextFlags = CONTEXT_FULL;

        if (!GetThreadContext(MainThread_, &context)) return;
        ResumeThread(MainThread_);

        auto endTime = std::chrono::steady_clock::now();

        ULONG64 buff[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
        memset(buff, 0, sizeof(buff));
        const PSYMBOL_INFO symbols = GetSymbol(context.Rip, (PSYMBOL_INFO)buff);

        if (tempTimer_ != timer_)
        {
            tempTimer_ = timer_;
            std::cout << "Profiler: TimeInterval " << tempTimer_ << "s\n";
            SampleVector_.push_back(samples_);
            samples_.clear();
        }

        ++samples_[symbols->Address].HitCount_;
        samples_[symbols->Address].Addr_ = symbols->Address;
        samples_[symbols->Address].SymbolName_ = symbols->Name;
        samples_[symbols->Address].TimeDuration_ += std::chrono::duration<double, std::milli>(endTime - startTime).count();
        samples_[symbols->Address].index_ = symbols->TypeIndex;

        std::this_thread::sleep_until(nextSampleTime);
        ++SampleNumber_;
    }
}

void Profiler::Exit()
{
    std::ofstream logFile("ProfileReport.csv");
    logFile << "Function,HitCount,Percentage,Time(ms)\n";

    if (!SampleVector_.empty()) {
        const std::map<ULONG64, SampleInfo>& v = SampleVector_.front();
        std::map<std::string, SampleInfo> mergedSamples;
        unsigned totalHits = 0;

        for (const auto& it : v) {
            std::string name = it.second.SymbolName_;
            name.erase(std::remove(name.begin(), name.end(), '`'), name.end());
            name.erase(std::remove(name.begin(), name.end(), '\''), name.end());
            name.erase(std::remove(name.begin(), name.end(), ','), name.end());

            SampleInfo& merged = mergedSamples[name];
            merged.HitCount_ += it.second.HitCount_;
            merged.TimeDuration_ += it.second.TimeDuration_;
            merged.SymbolName_ = name;
        }

        for (const auto& it : mergedSamples) {
            totalHits += it.second.HitCount_;
        }

        std::vector<std::pair<std::string, SampleInfo>> sortedSamples(mergedSamples.begin(), mergedSamples.end());

        std::sort(sortedSamples.begin(), sortedSamples.end(),
            [](const std::pair<std::string, SampleInfo>& a, const std::pair<std::string, SampleInfo>& b) {
                return a.second.HitCount_ > b.second.HitCount_;
            });

        for (const auto& it : sortedSamples) {
            const std::string& name = it.first;
            const SampleInfo& info = it.second;

            if (name.empty()) continue;

            unsigned hit = info.HitCount_;
            double time = info.TimeDuration_;
            double percent = (totalHits > 0) ? (static_cast<double>(hit) / totalHits * 100.0) : 0.0;

            logFile << name << ","
                << hit << ","
                << std::fixed << std::setprecision(4) << percent << ","
                << time << "\n";
        }
    }

    logFile.close();
}

void Profiler::InitStart()
{
    std::cout << "Profiler: InitStart()\n";
    Init();
}

void Profiler::InitEnd()
{
    std::cout << "Profiler: InitEnd()\n";
}

void Profiler::ExitBegin()
{
    Stop();
    Exit();
    std::cout << "Profiler: ExitBegin()\n";
}

void Profiler::ExitEnd()
{
    std::cout << "Profiler: ExitEnd()\n";
}

void Profiler::Start()
{
    std::cout << "Profiler: Start()\n";

    MainThreadId_ = GetCurrentThreadId();
    MainThread_ = OpenThread(
        THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
        0,
        MainThreadId_
    );

    stopFlag_ = false;
    WorkerThread_ = new std::thread(&Profiler::Sample, this);
}

void Profiler::Stop()
{
    std::cout << "Profiler: Stop()\n";

    stopFlag_ = true;
    if (WorkerThread_ && WorkerThread_->joinable()) {
        WorkerThread_->join();
        delete WorkerThread_;
        WorkerThread_ = nullptr;
    }
}

void Profiler::FrameBegin(int frameCount)
{
    UNUSED(frameCount);
}

void Profiler::FrameEnd(int frameCount)
{
    UNUSED(frameCount);
}

void Profiler::KeyPressed(unsigned char key)
{
    std::cout << "Profiler: Key Pressed: " << key << "\n";
}

void Profiler::EnterFunc(void* retAddress)
{
    ULONG64 addr = reinterpret_cast<ULONG64>(retAddress);
    addr;
}
void Profiler::ExitFunc(void* retAddress)
{
    ULONG64 addr = reinterpret_cast<ULONG64>(retAddress);
    auto now = std::chrono::steady_clock::now();

    if (entryTimes_.count(addr) && !entryTimes_[addr].empty())
    {
        auto start = entryTimes_[addr].back();
        entryTimes_[addr].pop_back();

        double duration = std::chrono::duration<double, std::milli>(now - start).count();
        samples_[addr].TimeDuration_ += duration;
    }
}
extern "C" __declspec(dllexport) void ProfileEnter(void* retAddress)
{
    Profiler::GetInstance().EnterFunc(retAddress);
}

extern "C" __declspec(dllexport) void ProfileExit(void* retAddress)
{
    Profiler::GetInstance().ExitFunc(retAddress);
}