/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  Crashlog.cpp
@brief  :  This file contains the function definition of the crashlog system

*Lew Zong Han Owen (z.lew) :
		- Integrated crash log system to capture data timestamp and code line location during an error

File Contributions: Lew Zong Han Owen (100%)

/*_______________________________________________________________________________________________________________*/
#include "Crashlog.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

//Variables for CrashLog
std::ofstream CrashLog::logFile;

void CrashLog::Initialise() {
	logFile.open("crash-log.txt", std::ios::out | std::ios::trunc); //Create crash log file
	if (logFile.is_open()) {
		//Log start of crash logging
		logFile << "[" << GetCurrentTimestamp() << "] " << "Log started" << std::endl;
		logFile.flush();
	}
	else {
		std::cerr << "Failed to open crash-log.txt" << std::endl;
	}
}

void CrashLog::Cleanup() {
	if (logFile.is_open()) {
		LogDebugMessage("Log end");
		logFile.close();
	}
}

//Function to log crash messages to crash-log file based on custom exception
void CrashLog::LogDebugMessage(const std::string& message, const char* file, int line) {
	if (logFile.is_open()) {
		if (file && line) { //Log exceptions with file and line location
			logFile << "[" << GetCurrentTimestamp() << "] " << message << " at " << file << " line " << line << std::endl;
			logFile.flush();
		}
		else logFile << "[" << GetCurrentTimestamp() << "] " << message << std::endl;
	}
}

//Log time and date of crash log messages
std::string CrashLog::GetCurrentTimestamp() {
	std::time_t now = std::time(nullptr); //Capture PC date and time
	std::tm timeinfo;
	localtime_s(&timeinfo, &now);
	std::ostringstream oss;
	oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

//Handle standard c++ critical signals
void CrashLog::SignalHandler(int signum) {
	switch (signum) {
	case SIGSEGV:
		CrashLog::LogDebugMessage("Signal: SIGSEV (Segmentation Fault)");
		CrashLog::LogDebugMessage("Program accessed memory it shouldn't (e.g. dereferencing a null pointer)");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGABRT:
		CrashLog::LogDebugMessage("Signal: SIGABRT (Abort())");
		CrashLog::LogDebugMessage("Program detected a serious error and called abort()");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGFPE:
		CrashLog::LogDebugMessage("Signal: SIGFPE (Floating-point exception)");
		CrashLog::LogDebugMessage("Program detected mathematical error (e.g. division by zero)");
		CrashLog::LogDebugMessage("Log end");
		break;
	case SIGILL:
		CrashLog::LogDebugMessage("SIGILL (Illegal instruction)");
		CrashLog::LogDebugMessage("Program tried to execute an invalid machine instruction");
		CrashLog::LogDebugMessage("Log end");
		break;
	default:
		CrashLog::LogDebugMessage("Unknown signal: " + std::to_string(signum));
	}
	std::exit(signum);
}

//Checks program for standard c++ critical signals
void CrashLog::SignalChecks() {
	std::signal(SIGSEGV, SignalHandler); //Segmentation signal
	std::signal(SIGABRT, SignalHandler); //Abort signal
	std::signal(SIGFPE, SignalHandler);  //Floating-point signal
	std::signal(SIGILL, SignalHandler);  //Illegal signal
}