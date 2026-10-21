#pragma once
typedef void* BreakpadRef;

class CrashReporter
{

public:
    static void setupCrashReporter();
    static void shutDownCrashReporter();
    static void addBreakpadLog(const char* logfile);
    static void addBreakpadKeyValue(const char* key, const char* value);
    static void CreateLogDump();



private:
    static void addDebugInfoToBreakpad();
    static void initBreakpadCallback(BreakpadRef breakpad);

    static BreakpadRef breakpad;
    static char* context;
};