#pragma once
#include <string>

//Class to detect custom and standard exceptions, and log crash logs into crash-log text file
class CrashLog {
public:
	static void Initialise();
	static void Cleanup();
	//Log messages into crash-log.txt
	static void LogDebugMessage(const std::string& message, const char* file = nullptr, int line = 0);
	static void SignalHandler(int signum); //Log specific messages into log file according to signal receive
	static void SignalChecks(); //Check for program crash signals
	struct Exception { //Unique exception to record file and line of thrown exceptions
		std::string message;
		const char* file;
		int line;

		Exception(const std::string& msg, const char* f, int l)
			: message(msg), file(f), line(l) {}
	};
private:
	static std::ofstream logFile;

	static std::string GetCurrentTimestamp(); //Record time of logging

};