#include "../src/signal-pool.h"
#include "../src-parameter-list/parameter-list.h"
#include <iostream>

//using ParameterList as argument type and return type
typedef const ParameterList& SignalParameterType;
typedef ParameterList SignalReturnType;
typedef SignalSinkTemplate<SignalReturnType, SignalParameterType> SignalSink;
typedef SignalSourceTemplate<SignalReturnType, SignalParameterType> SignalSource; 


#define LOOP_TIME 6

class Sender {
private:
	SignalSource m_src;
	SignalSink m_sink;
    
public:
	Sender()
		: m_sink(&Sender::Function, this) //make a callback
	{ }

	SignalSource& GetSignalSource()
	{
		return m_src;
	}
    
	ParameterList Function(const ParameterList& in = ParameterList())
	{
		ParameterList list;
		list.Add("number", 1); //start from 1
		//using merge mode : replace & conservative
		//  it means no matter what parameters are contained in the in list, we just care about "number"
		list.Merge(in, ParameterList::MergeMode::replace | ParameterList::MergeMode::conservative);

		int number = list.Get("number"); //get the number
		std::cout << "Sender (" << this << ") " << number << std::endl;
		//send a signal if needed
		if(number < LOOP_TIME)
		{
			ParameterList out;
			out.Add("number", number + 1)
			   .Add("callback", m_sink); //add a callback to out list for the signal handlers can find a way back
			m_src.Invoke(out); //send signal
		}

		return ParameterList();
	}
};

class Handler {
public:
	ParameterList Function(const ParameterList& in)
	{
		ParameterList list;
		list.Add("number", 1); //start from 1
		//requiring a callback
		list.Merge(in, ParameterList::RequireList().Add<SignalSink>("callback"));

		//get the number
		int number = list.Get("number");
		std::cout << "Handler(" << this << ") " << number << std::endl;
		//callback if needed
		if(number < LOOP_TIME)
		{
			list.Get("number") = number + 1; //number++
			SignalSink sink = list.Get("callback"); //get the callback
			list.Get("callback") = SignalSink(&Handler::Function, this); //it's necessary to insert this Function to the list if we want to perform a loop
			sink.Invoke(list); //invoke the callback and pass the parameter list
		}
		return ParameterList();
	}
};

void TestFlow()
{
	/*
	 *  Test 1:
	 *    sender send a signal with a callback
	 *    handler handle the signal, and invoke the callback
	*/
	Sender sender;
	Handler handler;
	//link handler to sender's signal source
	sender.GetSignalSource().HookOn(SignalSink(&Handler::Function, &handler));
	sender.Function();//start from sender
	
	std::cout << std::endl;
	
	/*
	 *  Test 2:
	 *    handler handle the signal, and invoke the callback, pass the handle function as a argument
	 *    but callback is the handle function of itself, so it's a loop!
	*/
	Handler a;
	//single object loop
	a.Function(ParameterList().Add("callback", SignalSink(&Handler::Function, &a)));

	std::cout << std::endl;
	
	/*
	 *  Test 3:
	 *    handler handle the signal, and invoke the callback, pass the handle function as a argument
	 *    but callback is the handle function of another handler, so it's a double loop!
	*/
	Handler b;
	//double objects loop
	a.Function(ParameterList().Add("callback", SignalSink(&Handler::Function, &b)));
}


int main(int argc, char* argv[]) {
	std::cout << "\n------------------------\n";
	std::cout << "[Example]::Start";
	std::cout << "\n------------------------\n";
	TestFlow();
	std::cout << "\n------------------------\n";
	std::cout << "[Example]::End";
	std::cout << "\n------------------------\n";
	return 0;
}
