#ifndef _SERVER_HPP_
#define _SERVER_HPP_

/*
  File: Server.hpp
  Header file for managing the server processes (NGINX, MariaDB, PHP) in the USB web server application.
 */

#include <Windows.h>

extern PROCESS_INFORMATION nginxProcessInfo;	// NGINX process information
extern PROCESS_INFORMATION mariadbProcessInfo;	// MariaDB process information
extern PROCESS_INFORMATION phpProcessInfo;		// PHP process information

namespace ErrorCodes {
	const int SUCCESS = 0;
	const int NGINX_START_FAILURE = 1;
	const int MARIADB_START_FAILURE = 2;
	const int PHP_START_FAILURE = 3;
	const int UNKNOWN_ERROR = 99;
	std::string getErrorMessage(int code);
}

constexpr const wchar_t NGINX_PATH[] = L"server\\nginx\\";			// root NGINX directory
constexpr const wchar_t MARIADB_PATH[] = L"server\\mariadb\\bin\\";	// root MariaDB directory
constexpr const wchar_t PHP_PATH[] = L"server\\php\\";				// root PHP directory

PROCESS_INFORMATION BeginProcess(const wchar_t* path, const wchar_t* exec);
bool StartNginx();
bool StartMariaDB();
bool StartPHP();
int StartServer();
void StopServer(int sigNum);

#endif