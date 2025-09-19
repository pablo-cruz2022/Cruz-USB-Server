#include <iostream>
#include <cstring>
#include <string>
#include <csignal>
#include <Windows.h>
#include "Server.hpp"
#include "LogFile.hpp"

/*
  File: Server.cpp
  Implementation file for managing the server processes (NGINX, MariaDB, PHP) in the USB web server. 
 */

PROCESS_INFORMATION nginxProcessInfo;
PROCESS_INFORMATION mariadbProcessInfo;
PROCESS_INFORMATION phpProcessInfo;

extern LogFile logger;
extern LogFile errorLogger;

/*
 Function: getErrorMessage
 Returns a human-readable error message corresponding to the given error code.

 Parameters:
 - code: The error code for which to retrieve the message.

 Returns: A string containing the error message.
 */
std::string ErrorCodes::getErrorMessage(int code) {
	switch (code) {
	case SUCCESS:
		return "Success";
	case NGINX_START_FAILURE:
		return "Failed to start NGINX server.";
	case MARIADB_START_FAILURE:
		return "Failed to start MariaDB server.";
	case PHP_START_FAILURE:
		return "Failed to start PHP service.";
	default:
		return "Unknown error.";
	}
}

/*
 Function: BeginProcess
 Wrapper for CreateProcess Windows API function. Creates a new process for the given executable
 and its path.
  
 Parameters:
 - path: The directory path where the executable is located.
 - exec: The name of the executable to run.
 
 Returns: A PROCESS_INFORMATION structure containing information about the newly created process.
 */
PROCESS_INFORMATION BeginProcess(const wchar_t* path, const wchar_t* exec) {
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	size_t bufferSize = wcslen(path) + wcslen(exec) + 1;
	wchar_t* cmd = new wchar_t[bufferSize];

	wcscpy_s(cmd, bufferSize, path);
	wcscat_s(cmd, bufferSize, exec);

	if (!CreateProcess(
		NULL,
		cmd,
		NULL, NULL, FALSE, NULL, NULL,
		path,
		&si, &pi)
	) {
		std::wcerr << L"Failed to start process " << exec << L". Error: " << GetLastError() << std::endl;
		ZeroMemory(&pi, sizeof(pi));
	}

	delete[] cmd;
	return pi;
}

/*
 Function: StartNginx
 Wrapper function to start the NGINX web server.
 
 Parameters: None
 
 Returns: TRUE on success, FALSE on failure.
 */
bool StartNginx() {
	std::cout << "Starting NGINX..." << std::endl;
	nginxProcessInfo = BeginProcess(NGINX_PATH, L"nginx.exe");
	if (nginxProcessInfo.hProcess != NULL) {
		return TRUE;
	}

	return FALSE;
}

/*
 Function: StartMariaDB
 Wrapper function to start the MariaDB database server.
  
 Parameters: None
  
 Returns: TRUE on success, FALSE on failure.
 */
bool StartMariaDB() {
	std::cout << "Starting MariaDB..." << std::endl;
	mariadbProcessInfo = BeginProcess(MARIADB_PATH, L"mariadbd.exe");
	if (mariadbProcessInfo.hProcess != NULL) {
		return TRUE;
	}

	// Sometimes, MariaDB will fail to start on first try, so call the installer instead of the
	// daemon directly, then try starting the daemon again.
	mariadbProcessInfo = BeginProcess(MARIADB_PATH, L"mariadb-install-db.exe");
	if (mariadbProcessInfo.hProcess != NULL) {
		// Wait for the installation process to complete before starting the daemon.
		WaitForSingleObject(mariadbProcessInfo.hProcess, INFINITE);
		mariadbProcessInfo = BeginProcess(MARIADB_PATH, L"mariadbd.exe");
		if (mariadbProcessInfo.hProcess != NULL) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 Function: StartPHP
 Wrapper function to start the PHP service.
  
 Parameters: None
  
 Returns: TRUE on success, FALSE on failure.
 */
bool StartPHP() {
	std::cout << "Starting PHP..." << std::endl;
	phpProcessInfo = BeginProcess(PHP_PATH, L"php-cgi.exe -b 127.0.0.1:9000");
	if (phpProcessInfo.hProcess != NULL) {
		return TRUE;
	}

	return FALSE;
}

/*
 Function: StartServer
 Entry point to the web server. Starts NGINX, MariaDB, and PHP services and checks for errors
 with each service.
 
 Parameters: None
  
 Returns: 0 on success, non-zero error code on failure.
 */
int StartServer() {
	std::cout << "Starting USB Web Server..." << std::endl;

	bool StatusNginx = StartNginx();
	bool StatusMariaDB = StartMariaDB();
	bool StatusPHP = StartPHP();

	if (!StatusNginx) {
		std::cerr << "Error starting NGINX server." << std::endl;
		return ErrorCodes::NGINX_START_FAILURE;
	}
	else if (!StatusMariaDB) {
		std::cerr << "Error starting MariaDB server." << std::endl;
		return ErrorCodes::MARIADB_START_FAILURE;
	}
	else if (!StatusPHP) {
		std::cerr << "Error starting PHP service." << std::endl;
		return ErrorCodes::PHP_START_FAILURE;
	}
	std::cout << "All services started successfully." << std::endl;

	return ErrorCodes::SUCCESS;
}

/*
  Function: StopServer
  Signal handler to stop all running services and clean up resources.

  Parameters:
  - sigNum: The signal number received (e.g., SIGINT, SIGTERM).

  Returns: None
 */
void StopServer(int sigNum) {
	SYSTEMTIME localTime;

	GetLocalTime(&localTime);
	logger.write(localTime, "Received signal to stop the server (Signal number: " + std::to_string(sigNum) + ").");

	// Terminate all running processes and clean up resources.
	GetLocalTime(&localTime);
	logger.write(localTime, "Stopping NGINX...");
	if (nginxProcessInfo.hProcess != NULL) {
		// NGINX has to be terminated with the -s quit signal for the cleanest and most graceful exit.
		CloseHandle(nginxProcessInfo.hProcess);
		CloseHandle(nginxProcessInfo.hThread);
		nginxProcessInfo = BeginProcess(NGINX_PATH, L"nginx.exe -s quit");
		CloseHandle(nginxProcessInfo.hProcess);
		CloseHandle(nginxProcessInfo.hThread);
		GetLocalTime(&localTime);
		logger.write(localTime, "NGINX server stopped.");
	}
	GetLocalTime(&localTime);
	logger.write(localTime, "Stopping MariaDB...");
	if (mariadbProcessInfo.hProcess != NULL) {
		TerminateProcess(mariadbProcessInfo.hProcess, 0);
		CloseHandle(mariadbProcessInfo.hProcess);
		CloseHandle(mariadbProcessInfo.hThread);
		GetLocalTime(&localTime);
		logger.write(localTime, "MariaDB server stopped.");
	}
	GetLocalTime(&localTime);
	logger.write(localTime, "Stopping PHP service...");
	if (phpProcessInfo.hProcess != NULL) {
		TerminateProcess(phpProcessInfo.hProcess, 0);
		CloseHandle(phpProcessInfo.hProcess);
		CloseHandle(phpProcessInfo.hThread);
		GetLocalTime(&localTime);
		logger.write(localTime, "PHP service stopped.");
	}

	// Provide feedback based on the signal received.
	GetLocalTime(&localTime);
	if (sigNum == SIGINT) {
		logger.write(localTime, "USB Web Server stopped successfully.");
	}
	else if (sigNum == SIGTERM) {
		logger.write(localTime, "USB Web Server terminated.");
		logger.writeError(localTime, "USB Web Server terminated unexpectedly.");
	}
	else if (sigNum == SIGABRT) {
		logger.write(localTime, "USB Web Server aborted.");
		logger.writeError(localTime, "USB Web Server aborted unexpectedly.");
	}
	else {
		logger.write(localTime, "USB Web Server received unknown signal (" + std::to_string(sigNum) + "). Exiting...");
		logger.writeError(localTime, "USB Web Server received unknown signal (" + std::to_string(sigNum) + "). Exiting...");
	}
}