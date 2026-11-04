#include <iostream>
#include <fstream>
#include <string>
#include "log.hpp"

#if defined(__linux__)
static bool IsWSL()
{
	std::ifstream file("/proc/version");
	std::string line;

	if (file.is_open())
	{
		std::getline(file, line);
		return line.find("Microsoft") != std::string::npos ||
			line.find("WSL") != std::string::npos;
	}

	return false;
}
#endif

void LogTargetOperatingSystem(void)
{
	std::cout << "Target Operating System: ";
#if defined(_WIN32) && defined(_WIN64)
	std::cout << "Windows";
#elif defined(_WIN32)
	std::cout << "Windows 32-bit" ;
#elif defined(__APPLE__) || defined(__MACH__)
	std::cout << "macOS";
#elif defined(__linux__)
	std::cout << "Linux ";
	if (IsWSL())
	{
		std::cout << "WSL (Windows Subsystem for Linux)";
	}
#elif defined(__unix__)
	std::cout << "Unix";
#elif defined(_POSIX_VERSION)
	std::cout << "POSIX-compliant OS";
#else
	std::cout << "Unknown OS";
#endif
	std::cout << std::endl;
}

void LogTargetArchitecture(void)
{
	std::cout << "Target Architecture: ";
#if defined(_M_X64)
	std::cout << "x64";
#elif defined(_M_IX86)
	std::cout << "x86";
#elif defined(__x86_64__)
	std::cout << "x64 (GCC)";
#elif defined(__i386__)
	std::cout << "x86 (GCC)";
#else
#error Platform not supported
#endif
	std::cout << std::endl;
}

void LogRunTimeArchitecture(void)
{
	// sizeof(void*) geeft de grootte van een pointer (adres in geheugen) in bytes
	// 32bit: 4 bytes
	// 64bit: 8 bytes
	std::cout << "Runtime Architecture: ";
	if (sizeof(void*) == 8)
	{
		std::cout << "64-bit";
	}
	else if (sizeof(void*) == 4)
	{
		std::cout << "32-bit";
	}
	else if (sizeof(void*) == 2)
	{
		std::cout << "16-bit";
	}
	else if (sizeof(void*) == 1)
	{
		std::cout << "8-bit";
	}
	else
	{
		std::cout << "Unknown, pointer size = "
			<< sizeof(void*) << " bytes";
	}
	std::cout << std::endl;
}

void LogTargetCompiler(void)
{
	std::cout << "Target Compiler: ";
#if defined(__clang__)
	std::cout << "Clang version: " << __clang_version__;
#elif defined(__GNUC__) || defined(__GNUG__)
	std::cout << "GCC version: " << __VERSION__ << " (" << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << ")";
#elif defined(_MSC_FULL_VER)
	std::cout << "MSVC version: " << _MSC_FULL_VER;
#else
	std::cout << "Unknown compiler";
#endif
	std::cout << std::endl;
}

void LogTargetCxxStandard(void)
{
	std::cout << "Target C++ standard: ";

#if __cplusplus > 202302L
	std::cout << "C++26 or later";
#elif __cplusplus == 202302L
	std::cout << "C++23";
#elif __cplusplus == 202002L
	std::cout << "C++20";
#elif __cplusplus == 201703L
	std::cout << "C++17";
#elif __cplusplus == 201402L
	std::cout << "C++14";
#elif __cplusplus == 201103L
	std::cout << "C++11";
#elif __cplusplus == 199711L
	std::cout << "C++98/C++03";
#else
	std::cout << "pre C++11";
#endif
	std::cout << "(" << __cplusplus << ")" << std::endl;
}
