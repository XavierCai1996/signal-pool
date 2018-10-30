#ifndef SIGNAL_SOURCE_TEMPLATE_H
#define SIGNAL_SOURCE_TEMPLATE_H

#include "signal-sink-template.h"
#include <vector>

//one agrgument signal source template
template <typename _RETURN, typename _PARAM>
class SignalSourceTemplate {
protected:
    typedef SignalSinkTemplate<_RETURN, _PARAM> SignalSinkType; 
    std::vector<SignalSinkType> m_sinks;
    
public:
    SignalSourceTemplate() { }
    virtual ~SignalSourceTemplate() { }

    //virtual is for some particular derived classes can return some values from sink
    virtual _RETURN Invoke(_PARAM param) const 
    {
        for(unsigned int i = 0; i < m_sinks.size(); i++)
        {
            m_sinks[i].Invoke(param);
        }
        return _RETURN();
    }
    
    bool HookOn(const SignalSinkType &sink)
    {
        if(!sink.IsValid())
        {
            return false;
        }
        
        bool exsit = false;
        for(typename std::vector<SignalSinkType>::const_iterator ite = m_sinks.begin(); ite != m_sinks.end(); ite++)
        {
            if(*ite == sink)
            {
                exsit = true;
                break;
            }
        }
        
        if(exsit)
        {
            return false;
        }
        
        m_sinks.push_back(sink);
        return true;
    }
    
    bool HookOff(SignalSinkType sink)
    {
        for(typename std::vector<SignalSinkType>::iterator ite = m_sinks.begin(); ite != m_sinks.end(); ite++)
        {
            if(*ite == sink)
            {
                m_sinks.erase(ite);
                return true;
            }
        }
	return false;
    }
};//class SignalSource

#endif
