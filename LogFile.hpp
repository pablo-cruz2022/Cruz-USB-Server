#ifndef _LOGFILE_HPP_
#define _LOGFILE_HPP_

/*
  File: LogFile.hpp
  Header file for the LogFile class that manages logging for the USB web server application.
 */

#include <fstream>
#include <Windows.h>

#define MAX_LOG_SIZE 1048576						// 1 MB
#define LOG_FILE_PATH "server/logs/server.log"		// Main log file path.
#define ERROR_LOG_FILE_PATH "server/logs/error.log"	// Separate error log file.

/*
  Class: LogFile
  Manages logging for the USB web server application, including log rotation when the log file
  runs too large.

  Methods:
  - LogFile(const std::string& filePath): Constructor that initializes the log file streams.
  - ~LogFile(): Destructor that closes the log file streams.
  - void write(const std::string& message): Writes a message to the main log file.
  - void writeError(const std::string& message): Writes a message to the error log file.
  - bool isFileTooLarge(const std::string& filePath): Checks if the log file exceeds MAX_LOG_SIZE.
  - void rotateLog(std::ofstream& stream, const std::string& filePath): Rotates the log file.

  Attributes:
  - std::ofstream logStream: Stream for writing to the main log file.
  - std::ofstream errorLogStream: Stream for writing to the error log file.
 */
class LogFile {
private:
	std::ofstream logStream;
	std::ofstream errorLogStream;
	bool isFileTooLarge(const std::string& filePath);
	void rotateLog(std::ofstream& stream, const std::string& filePath);
public:
	LogFile(const std::string& filePath);
	~LogFile();
	void write(SYSTEMTIME localTime, const std::string& message);
	void writeError(SYSTEMTIME localTime, const std::string& message);
};

#endif