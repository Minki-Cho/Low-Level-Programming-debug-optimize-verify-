#pragma once

#include "Profiler_API.h"
#include <string>
#include <windows.h>
#include <thread>
#include <atomic>
#include <map>
#include <vector>
#include <chrono>
class Profiler {
public:
    struct SampleInfo {
        unsigned HitCount_ = 0;
        ULONG64 Addr_ = 0;
        std::string SymbolName_;
        double TimeDuration_ = 0.0;
        DWORD index_ = 0;
    };

    static Profiler& GetInstance();

    void InitStart();
    void InitEnd();
    void ExitBegin();
    void ExitEnd();
    void Start();
    void Stop();
    void FrameBegin(int frameCount);
    void FrameEnd(int frameCount);
    void KeyPressed(unsigned char key);
    void EnterFunc(void* retAddress);
    void ExitFunc(void* retAddress);
private:
    Profiler() = default;
    ~Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    void Init();
    void Exit();
    void Sample();

private:
    HANDLE MainThread_ = nullptr;
    DWORD MainThreadId_ = 0;
    std::thread* WorkerThread_ = nullptr;
    std::atomic<bool> stopFlag_ = false;
    unsigned SampleNumber_ = 0;

    std::map<ULONG64, SampleInfo> samples_;
    std::vector<std::map<ULONG64, SampleInfo>> SampleVector_;
    std::chrono::time_point<std::chrono::steady_clock> timerStart_;
    long long timer_ = 0;
    long long tempTimer_ = 0;
    unsigned Maxcount = 10000;
    std::map<ULONG64, std::vector<std::chrono::high_resolution_clock::time_point>> entryTimes_;
};
