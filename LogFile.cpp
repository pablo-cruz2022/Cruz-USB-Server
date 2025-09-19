#include <iostream>
#include <fstream>
#include <sstream>
#include "LogFile.hpp"

/* 
  File: LogFile.cpp
  Implementation file for the LogFile class that manages logging for the USB web server application.
 */

/*
  Constructor: LogFile
  Initializes the log file streams for general logs and error logs.
 */
LogFile::LogFile(const std::string& filePath) {
	logStream.open(filePath, std::ios::app);
	errorLogStream.open(ERROR_LOG_FILE_PATH, std::ios::app);
}

/*
  Destructor: ~LogFile
  Closes the log file streams if they are open.
 */
LogFile::~LogFile() {
	if (logStream.is_open()) {
		logStream.close();
	}
	if (errorLogStream.is_open()) {
		errorLogStream.close();
	}
}

/*
  Method: isFileTooLarge
  Checks if the specified log file exceeds the maximum allowed size.
  Parameters:
  - filePath: The path to the log file.
  Returns: true if the file size exceeds MAX_LOG_SIZE, false otherwise.
 */
bool LogFile::isFileTooLarge(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		auto fileSize = file.tellg();
		file.close();
		return fileSize > MAX_LOG_SIZE;
	}
	return false;
}

/*
  Method: rotateLog
  Rotates the log file by renaming the current log file to a backup and creating a new log file.

  Parameters:
  - stream: The ofstream object representing the log file stream.
  - filePath: The path to the log file.

  Returns: None
 */
void LogFile::rotateLog(std::ofstream& stream, const std::string& filePath) {
	int renameResult;

	stream.close();
	std::remove((filePath + ".old").c_str());
	renameResult = std::rename(filePath.c_str(), (filePath + ".old").c_str());
	if (renameResult != 0) {
		char errorMsg[128];

		std::cerr << "Failed to rotate log file: " << filePath << std::endl;
		strerror_s(errorMsg, sizeof(errorMsg), errno);
		std::perror(errorMsg);
		return;
	}
	stream.open(filePath, std::ios::app);
}

/*
  Method: write
  Writes a message to the general log file.

  Parameters:
  - localTime: The current local time.
  - message: The message to log.

  Returns: None
 */
void LogFile::write(SYSTEMTIME localTime, const std::string& message) {
	std::ostringstream logMessage;

	logMessage << "[" << localTime.wYear << "-"
		<< (localTime.wMonth < 10 ? "0" : "") << localTime.wMonth << "-"
		<< (localTime.wDay < 10 ? "0" : "") << localTime.wDay << " "
		<< (localTime.wHour < 10 ? "0" : "") << localTime.wHour << ":"
		<< (localTime.wMinute < 10 ? "0" : "") << localTime.wMinute << ":"
		<< (localTime.wSecond < 10 ? "0" : "") << localTime.wSecond << "] ";
	logMessage << message;
	if (isFileTooLarge(LOG_FILE_PATH)) {
		rotateLog(logStream, LOG_FILE_PATH);
	}
	if (logStream.is_open()) {
		logStream << logMessage.str() << std::endl;
	}
}

/*
  Method: writeError
  Writes a message to the error log file.

  Parameters:
  - localTime: The current local time.
  - message: The error message to log.

  Returns: None
 */
void LogFile::writeError(SYSTEMTIME localTime, const std::string& message) {
	std::ostringstream logMessage;

	logMessage << "[" << localTime.wYear << "-"
		<< (localTime.wMonth < 10 ? "0" : "") << localTime.wMonth << "-"
		<< (localTime.wDay < 10 ? "0" : "") << localTime.wDay << " "
		<< (localTime.wHour < 10 ? "0" : "") << localTime.wHour << ":"
		<< (localTime.wMinute < 10 ? "0" : "") << localTime.wMinute << ":"
		<< (localTime.wSecond < 10 ? "0" : "") << localTime.wSecond << "] ";
	logMessage << message;
	if (isFileTooLarge(LOG_FILE_PATH)) {
		rotateLog(errorLogStream, LOG_FILE_PATH);
	}
	if (errorLogStream.is_open()) {
		errorLogStream << logMessage.str() << std::endl;
	}
}