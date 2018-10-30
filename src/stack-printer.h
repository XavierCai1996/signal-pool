#ifndef STACK_PRINTER
#define STACK_PRINTER

#include <execinfo.h> 
#include <signal.h> 
#include <stdlib.h>

#include <string>
#include <iostream>

#if defined(__linux__)
	#include <cxxabi.h>
#else
	#define abi::__cxa_demangle(type, p2, p3, p4) (type)
#endif //#if defined(__linux__)

class StackPrinter {
private:
	class StackInfo {
	private:
		friend class StackPrinter;
		static bool NeedDemangling(std::string src);
		StackInfo(std::string info);

		std::string source;
		std::string path;
		std::string function;
		std::string shift;
		std::string location;
	};//class Info
	
	static void Print(int signal);

public:
	static void InitializeSignalHook();
};

#endif //#ifndef STACK_PRINTER
