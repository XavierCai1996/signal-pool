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
    struct BufferInterface {
        SignalType::BufferMap buffer;
        ~BufferInterface()
        {
            for(SignalType::BufferMap::iterator ite = buffer.begin(); ite != buffer.end(); ite++)
            {
                delete ite->second;
            }
            buffer.clear();
        }
    };//struct BufferInterface
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
        SignalHandle(const Identity& id)
            : m_id(id)
        { }
        //identify the handle with its Identity, so that we are able to compare them
        static SignalHandle* Buffer(SignalHandle* handle)
        {
            Identity id = handle->m_id;
            SignalType::BufferMap::iterator find = m_buffer.buffer.find(id);
            if(find != m_buffer.buffer.end())
            {
                delete handle;
                return dynamic_cast<SignalHandle*>(find->second);
            }
            m_buffer.buffer[id] = dynamic_cast<SignalType::Handle*>(handle);
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
                
            public:
                SignalHandleImpl(_FUNC function, _OBJ* obj)
                    : SignalHandle(Identity(function) + Identity(obj)), m_function(function), m_obj(obj)
                { }
                
                virtual _RETURN Invoke(_PARAM param) const
                {
                    _OBJ& ref = *m_obj;
                    return (ref.*m_function)(param);
                }
            } *handle = new SignalHandleImpl(function, obj);
            return Buffer(dynamic_cast<SignalHandle*>(handle));
        }
	
	template <typename _FUNC, typename _OBJ>
        static SignalHandle* Create(_FUNC function, const _OBJ* obj)
        {
            class SignalConstHandleImpl : public SignalHandle {
            private:
                _FUNC m_function;
                const _OBJ* m_obj;
                
            public:
                SignalConstHandleImpl(_FUNC function, const _OBJ* obj)
                    : SignalHandle(Identity(function) + Identity(obj)), m_function(function), m_obj(obj)
                { }
                
                virtual _RETURN Invoke(_PARAM param) const
                {
                    const _OBJ& ref = *m_obj;
                    return (ref.*m_function)(param);
                }
            } *handle = new SignalConstHandleImpl(function, obj);
            return Buffer(dynamic_cast<SignalHandle*>(handle));
        }
        
        static SignalHandle* Create(StaticSignalHandle function)
        {
            class SignalHandleStaticImpl : public SignalHandle {
            private:
                StaticSignalHandle m_function;
		
            public:
                SignalHandleStaticImpl(StaticSignalHandle function)
                    : SignalHandle(Identity(function)), m_function(function)
                { }
                
                virtual _RETURN Invoke(_PARAM param) const
                {
                    return m_function(param);
                }
            } *handle = new SignalHandleStaticImpl(function);
            return Buffer(dynamic_cast<SignalHandle*>(handle));
        }
        
    private:
        static SignalType::BufferInterface m_buffer;
        Identity m_id;
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
    SignalSinkTemplate<_RETURN, _PARAM>(_RETURN (_T::*function)(_PARAM) const, const _OBJ* obj)
        : m_handle(SignalHandle::Create(function, obj))
    { }
    
    SignalSinkTemplate<_RETURN, _PARAM>(StaticSignalHandle function)
        : m_handle(SignalHandle::Create(function))
    { }
    
};//class SignalSink

template <typename _RETURN, typename _PARAM>
SignalType::BufferInterface SignalSinkTemplate<_RETURN, _PARAM>::SignalHandle::m_buffer;


#endif
