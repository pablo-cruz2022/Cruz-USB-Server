#include <iostream>
#include <csignal>
#include <Windows.h>
#include "Server.hpp"
#include "LogFile.hpp"

/*
 File: Cruz USB Server.cpp
 Entry point for the portable and lightweight USB web server application with WEMP(Windows,
 NGINX, MariaDB, PHP) stack.
 */

extern PROCESS_INFORMATION nginxProcessInfo;
extern PROCESS_INFORMATION mariadbProcessInfo;
extern PROCESS_INFORMATION phpProcessInfo;

LogFile logger(LOG_FILE_PATH);				// General logger
LogFile errorLogger(ERROR_LOG_FILE_PATH);	// Error logger

int main() {
	SYSTEMTIME localTime;
	int ret = StartServer();

	// Inform the user to allow any firewall prompts.
	std::cout << std::endl << "**************************************************************************************" << std::endl;
	std::cout <<			  "Please click \"Allow\" on any pop-ups that come up or the server will not work properly.";
	std::cout << std::endl << "**************************************************************************************" << std::endl;
	std::cout << std::endl;

	std::signal(SIGTERM, StopServer);
	std::signal(SIGABRT, StopServer);
	std::signal(SIGINT, StopServer);

	// Check if the server started successfully.
	if (ret != 0) {
		GetLocalTime(&localTime);
		errorLogger.write(localTime, "Server failed to start: " + ErrorCodes::getErrorMessage(ret));

		// Terminate this process if server fails to start.
		raise(SIGTERM);
		return ret;
	}

	// Inform the user that the server is running.
	std::cout << "USB Web Server is running!" << std::endl;
	std::cout << "Press Ctrl+C to stop the server." << std::endl;
	GetLocalTime(&localTime);
	logger.write(localTime, "USB Web Server started successfully.");

	// Wait for all processes to finish before exiting the application.
	WaitForSingleObject(nginxProcessInfo.hProcess, INFINITE);
	WaitForSingleObject(mariadbProcessInfo.hProcess, INFINITE);
	WaitForSingleObject(phpProcessInfo.hProcess, INFINITE);

	return 0;
}