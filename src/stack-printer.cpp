#include "stack-printer.h"

bool StackPrinter::StackInfo::NeedDemangling(std::string src)
{
	if(src.length() < 0)
	{
		return false;
	}
	bool hasNumber = false;
	bool hasCap = false;
	unsigned int index = 0;

	for(; index < src.length(); index++)
	{
		char c = src[index];
		if(!hasNumber && (c >= '0' && c <= '9'))
		{
			hasNumber = true;
		}
		if(!hasCap && (c >= 'A' && c <= 'Z'))
		{
			hasCap = true;
		}
		if(hasNumber && hasCap)
		{
			break;
		}
	}
	return hasNumber && hasCap;
}
		
StackPrinter::StackInfo::StackInfo(std::string info)
{
	source = info;
	
	//location
	std::size_t end, start;
	end = info.rfind(']');
	start = info.rfind('[');
	if(start != std::string::npos && end != std::string::npos && start < end)
	{
		location = info.substr(start + 1, end - start - 1);
		info = info.substr(0, start);
	}

	//function & shift
	start = info.rfind('(');
	end = info.rfind(')');
	if(start != std::string::npos && end != std::string::npos && start < end)
	{
		std::string str = info.substr(start + 1, end - start - 1);
		std::size_t plus = str.find('+');
		if(plus != std::string::npos)
		{
			function = str.substr(0, plus);
			shift = str.substr(plus + 1);
			info = info.substr(0, start);
		}
	}

	//path
	path = info;

	//demangling
	if(NeedDemangling(function))
	{
		function = abi::__cxa_demangle(function.c_str(), NULL, NULL, NULL);
	}
}
	
void StackPrinter::Print(int signal)
{
	int maxSize = 100;
	void** buffer = new void*[maxSize];
	int nptrs = backtrace(buffer, maxSize);
    	char **strings = backtrace_symbols(buffer, nptrs);

	if (strings == NULL)
	{
		std::cerr << "backtrace symbols" << std::endl;
		exit(EXIT_FAILURE);
	}
 
	for (int i = 1; i < nptrs; i++)
	{
		StackInfo si(strings[i]);
		std::cerr << "\nstack(" << i << ")\n";
		std::cerr << "from : " << si.path << " @[" << si.location << "]\n";
		std::cerr << "function : [ " << si.function << " ] @shift=[" << si.shift << "]";
		//std::cerr << "\nsrc=" << si.source;
		std::cerr << std::endl;
	}
 
    	delete buffer;
    	exit(signal);
}

void StackPrinter::InitializeSignalHook()
{
	signal(SIGABRT, &Print);
	signal(SIGFPE, &Print);
	signal(SIGILL, &Print);
	signal(SIGINT, &Print);
	signal(SIGSEGV, &Print);
	signal(SIGTERM, &Print);
	std::cout << "Hook Success" << std::endl;
}

struct InitializeSignalHook {
	InitializeSignalHook()
	{
		StackPrinter::InitializeSignalHook();
	}
} InitializeSignalHookInstance;

