#pragma once
#include <windows.h>
#include <vector>

struct MarkerData {
    int id;
    std::vector<int> marked;
    HANDLE startEvent;
    HANDLE resumeEvent;
    HANDLE stopEvent;
    HANDLE doneEvent;
    bool blocked = false;
    CRITICAL_SECTION cs;
};

extern std::vector<int> arr;
extern HANDLE blockedMutex;
extern int blockedCount;
extern CRITICAL_SECTION arrCs;
extern CRITICAL_SECTION logCs;

DWORD WINAPI MarkerThread(LPVOID param);
