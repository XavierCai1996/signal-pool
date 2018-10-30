#include "../src/signal-pool.h"
#include <iostream>

//for specialization the types
typedef SignalSinkTemplate<void, const int&> SignalSinkInt;
typedef SignalSourceTemplate<void, const int&> SignalSourceInt; 

//functions in class for handle signal
class Handler {
public:
	void HandleFunction(const int& param)
	{
		std::cout << "Handler::" << __FUNCTION__ << " " << param << std::endl;
	}

	static void StaticHandleFunction(const int& param)
	{
		std::cout << "Handler::" << __FUNCTION__ << " " << param << std::endl;
	}

	void ConstHandleFunction(const int& param) const
	{
		std::cout << "Handler::" << __FUNCTION__ << " " << param << std::endl;
	}
};//class Sink

//outside functions for handle signal
void OutsideHandleFunction(const int& param)
{
	std::cout << "Outside::" << __FUNCTION__ << " " << param << std::endl;
}

static void OutsideStaticHandleFunction(const int& param)
{
	std::cout << "Outside::" << __FUNCTION__ << " " << param << std::endl;
}

/*************** The example of using Signal Sink&Source Template [start] ***************/
class Sender {
private:
	SignalSourceInt m_src; //source as a member
    
public:
	SignalSourceInt& GetSignalSourceInt()
	{
		return m_src;
	}

	void DoSomething()
	{
		m_src.Invoke(0); //sende signal to source, all sinks hooked on this source will be invoked
	}
};//class Source


//test flow
void SinkSourceTestFlow()
{
	Handler handler;
	SignalSinkInt sink1(&Handler::HandleFunction, &handler); //with pointer
	SignalSinkInt sink2(&Handler::StaticHandleFunction); //without pointer

	sink1.Invoke(1); //use it just like a callback
	sink2.Invoke(2);


	Sender sender;
	sender.GetSignalSourceInt().HookOn(sink1); //hook the sinks on the source in sender
	sender.GetSignalSourceInt().HookOn(sink2);
	sender.GetSignalSourceInt().HookOn(SignalSinkInt(&OutsideHandleFunction));
	sender.GetSignalSourceInt().HookOn(SignalSinkInt(&OutsideStaticHandleFunction));

	sender.DoSomething(); //invoke
}
/*************** The example of using Signal Sink&Source Template [ end ] ***************/



/*************** The example of using calss template with signal poor [start] ***************/
//SignalClassBaseTemplate has same interfaces to SignalPool
typedef SignalPool<void, const int&> SignalPoolInt;
typedef SignalClassBaseTemplate<void, const int&> SignalClassBase;

//signal sender, inherit interfaces form base class
class SignalClassTest : public SignalClassBase {
public:
	void Function1() const
	{
		//use the invoke interface from base class
		this->Invoke(Identity(this) + "signal 1", 11);
	}

	void Function2() const
	{
		//use the invoke interface from base class
		this->Invoke(Identity(this) + "signal 2", 22);
	}
};


//test flow
void SignalPoolTestFlow()
{
	/*
	 * codes for testing signal pool
	*/
	Handler handler;
	SignalPoolInt pool;
	//hook handlers on two signals
	//  argument 1 : "signal 1" : the name/identity of the signal
	//  argument 2 : SignalPoolInt::SignalSink : the signal sink
	pool.HookOn("signal 1", SignalPoolInt::SignalSink(&Handler::HandleFunction, &handler));
	pool.HookOn("signal 2", SignalPoolInt::SignalSink(&Handler::ConstHandleFunction, &handler));
	pool.HookOn("signal 1", SignalPoolInt::SignalSink(&Handler::StaticHandleFunction));
	pool.HookOn("signal 2", SignalPoolInt::SignalSink(&OutsideHandleFunction));
	pool.HookOn("signal 1", SignalPoolInt::SignalSink(&OutsideStaticHandleFunction));

	//invoke signal 1
	pool.Invoke("signal 1", 1);
	//invoke signal 2
	pool.Invoke("signal 2", 2);

	std::cout << std::endl;
	
	/*
	 * codes for testing signal class
	*/
	SignalClassTest signalClass;
	//p.s. argument 1 : Identity(&signalClass) + "signal 1", just a name/identity for specifying a signal
	signalClass.HookOn(Identity(&signalClass) + "signal 1", SignalClassTest::SignalSink(&Handler::HandleFunction, &handler));
	signalClass.HookOn(Identity(&signalClass) + "signal 2", SignalClassTest::SignalSink(&Handler::ConstHandleFunction, &handler));
	signalClass.HookOn(Identity(&signalClass) + "signal 1", SignalClassTest::SignalSink(&Handler::StaticHandleFunction));
	signalClass.HookOn(Identity(&signalClass) + "signal 2", SignalClassTest::SignalSink(&OutsideHandleFunction));
	signalClass.HookOn(Identity(&signalClass) + "signal 1", SignalClassTest::SignalSink(&OutsideStaticHandleFunction));
	//signal will be send in functions
	signalClass.Function1();
	signalClass.Function2();
}
/*************** The example of using calss template with signal poor [ end ] ***************/


int main(int argc, char* argv[]) {
	std::cout << "\n------------------------\n";
	std::cout << "[Example]::Sink&Source";
	std::cout << "\n------------------------\n";
	SinkSourceTestFlow();
	std::cout << "\n------------------------\n";
	std::cout << "[Example]::SignalPool";
	std::cout << "\n------------------------\n";
	SignalPoolTestFlow();
	std::cout << "\n------------------------\n";
	std::cout << "[Example]::End";
	std::cout << "\n------------------------\n";
	return 0;
}
