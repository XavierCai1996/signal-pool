#ifndef SIGNAL_SINK_TEMPLATE_H
#define SIGNAL_SINK_TEMPLATE_H

#include "identity.h"
#include <map>
#include <assert.h>

#ifndef NULL
    #define NULL 0
#endif

//define some types used in class SignalSinkTemplate
class SignalType {
public:
    virtual ~SignalType() = 0;
    class Handle {
    public:
		virtual ~Handle() { };
    };
    typedef std::map<Identity, SignalType::Handle*> BufferMap;
};

//one argument signal sink template
template <typename _RETURN, typename _PARAM>
class SignalSinkTemplate {
public:
    typedef _RETURN (*StaticSignalHandle)(_PARAM);
    
private:
    class SignalHandle : public SignalType::Handle {
    public:
        virtual _RETURN Invoke(_PARAM param) const = 0;
	virtual ~SignalHandle() { };
        
    protected:
        virtual Identity GetIdentity() const = 0;
        //identify the handle with its Identity, so that we are able to compare them
        static SignalHandle* Buffer(SignalHandle* handle)
        {
            Identity id = handle->GetIdentity();
            SignalType::BufferMap::iterator find = m_buffer.find(id);
            if(find != m_buffer.end())
            {
                delete handle;
                return dynamic_cast<SignalHandle*>(find->second);
            }
            m_buffer[id] = dynamic_cast<SignalType::Handle*>(handle);
            return handle;
        }
        
    public:
        template <typename _FUNC, typename _OBJ>
        static SignalHandle* Create(_FUNC function, _OBJ* obj)
        {
            class SignalHandleImpl : public SignalHandle {
            private:
                _FUNC m_function;
                _OBJ* m_obj;
		Identity m_id;
                
            protected:
                virtual Identity GetIdentity() const
                {
			return m_id;
                }
                
            public:
                SignalHandleImpl(_FUNC function, _OBJ* obj)
			: m_function(function), m_obj(obj), m_id(Identity(function) + Identity(obj))
                { }
                
                virtual _RETURN Invoke(_PARAM param) const
                {
                    _OBJ& ref = *m_obj;
                    return (ref.*m_function)(param);
                }
            } *handle = new SignalHandleImpl(function, obj);
            return Buffer(dynamic_cast<SignalHandle*>(handle));
        }
        
        static SignalHandle* Create(StaticSignalHandle function)
        {
            class SignalHandleStaticImpl : public SignalHandle {
            private:
                StaticSignalHandle m_function;
		Identity m_id;
                
            protected:
                virtual Identity GetIdentity() const
                {
			return m_id;
                }
                
            public:
                SignalHandleStaticImpl(StaticSignalHandle function)
			: m_function(function), m_id(Identity(function))
                { }
                
                virtual _RETURN Invoke(_PARAM param) const
                {
                    return m_function(param);
                }
            } *handle = new SignalHandleStaticImpl(function);
            return Buffer(dynamic_cast<SignalHandle*>(handle));
        }
        
    private:
        static SignalType::BufferMap m_buffer;
    };//class SignalHandle
    
    SignalHandle* m_handle;
    
public:
    SignalSinkTemplate<_RETURN, _PARAM>()
		: m_handle(NULL)
	{ }
    
    bool operator == (const SignalSinkTemplate<_RETURN, _PARAM>& o) const
    {
        if(m_handle != NULL && o.m_handle != NULL)
        {
            return m_handle == o.m_handle;
        }
        return false;
    }
    
    bool operator != (const SignalSinkTemplate<_RETURN, _PARAM>& o) const
    {
        return !(*this == o);
    }
    
    bool IsValid() const
    {
        return m_handle != NULL;
    }
    
    _RETURN Invoke(_PARAM param) const
    {
        assert(IsValid());
        return m_handle->Invoke(param);
    }
    
    template <typename _FUNC, typename _OBJ>
    bool Set(_FUNC function, _OBJ* obj)
    {
        if(m_handle != NULL)
        {
            delete m_handle;
        }
        m_handle = SignalHandle::Create(function, obj);
        return true;
    }
    
    bool Set(StaticSignalHandle function)
    {
        if(m_handle != NULL)
        {
            delete m_handle;
        }
        m_handle = SignalHandle::Create(function);
        return true;
    }

    template <typename _T, typename _OBJ>
    SignalSinkTemplate<_RETURN, _PARAM>(_RETURN (_T::*function)(_PARAM), _OBJ* obj)
        : m_handle(SignalHandle::Create(function, obj))
    { }
    
    template <typename _T, typename _OBJ>
    SignalSinkTemplate<_RETURN, _PARAM>(_RETURN (_T::*function)(_PARAM) const, _OBJ* obj)
        : m_handle(SignalHandle::Create(function, obj))
    { }
    
    SignalSinkTemplate<_RETURN, _PARAM>(StaticSignalHandle function)
        : m_handle(SignalHandle::Create(function))
    { }
    
};//class SignalSink

template <typename _RETURN, typename _PARAM>
SignalType::BufferMap SignalSinkTemplate<_RETURN, _PARAM>::SignalHandle::m_buffer;


#endif
