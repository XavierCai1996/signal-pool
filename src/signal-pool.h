#ifndef SIGNAL_POOL_H
#define SIGNAL_POOL_H

#include "signal-sink-template.h"
#include "signal-source-template.h"
#include "identity.h"
#include <map>
#include "edit-distance.h"

/*
 *  a pool to store signal sources with signal sinks
 *  as you calling interfaces to Invoke, HookOn or HookOff a specific signal, using Idetity for indexing
 *
 *  the signal source with a specific Identity will generate automaticlly when you calling the inferfaces
 *  you also can utilize List to find signal source exited
 */
template <typename _RETURN, typename _PARAM>
class SignalPool {
public:
	typedef SignalSinkTemplate<_RETURN, _PARAM> SignalSink;
	typedef SignalSourceTemplate<_RETURN, _PARAM> SignalSource;
	
protected:
	typedef std::map<Identity, SignalSource*> SignalMap;
	typedef SignalPool<_RETURN, _PARAM>* Relay;
	typedef std::map<Relay, bool> RelayMap;
	SignalMap m_pool;
	RelayMap m_relays;

	SignalSource* Get(const Identity& identity)
	{
		typename SignalMap::iterator find = m_pool.find(identity);
		if(find == m_pool.end())
		{
			typename SignalPool<_RETURN, _PARAM>::SignalSource* ss = CreateSignalSource();
			m_pool[identity] = ss;
			return ss;
		}
		return find->second;
	}
	
	bool Have(const Identity& identity) const
	{
		return m_pool.find(identity) != m_pool.end();
	}
	
	virtual SignalSource* CreateSignalSource() const
	{
		return new SignalPool<_RETURN, _PARAM>::SignalSource();
	}
	
public:
	~SignalPool()
	{
		for(typename SignalMap::iterator ite = m_pool.begin(); ite != m_pool.end(); ite++)
		{
			delete ite->second;
		}
	}

	//generate a signal source without any other operations
	//usually for making sure this signal source will be list when the function List is called
	void Regist(const Identity& identity)
	{
		Get(identity);
	}
	
	_RETURN Invoke(const Identity& identity, _PARAM param) const
	{
		//relay
		for(typename RelayMap::const_iterator ite = m_relays.begin(); ite != m_relays.end(); ite++)
		{
			if(!ite->second || ite->first->Have(identity))
			{
				ite->first->Invoke(identity, param);
			}
		}

		typename SignalMap::const_iterator find = m_pool.find(identity);
		if(find == m_pool.end())
		{
			return _RETURN();
		}
		return find->second->Invoke(param);
	}
	bool HookOn(const Identity& identity, const SignalSink& sink)
	{
		return Get(identity)->HookOn(sink);
	}
	bool HookOff(const Identity& identity, const SignalSink& sink)
	{
		return Get(identity)->HookOff(sink);
	}
	void HookOn(const Relay& relay, bool isTerminate = false) //if is terminate, only relay signal existed in pool
	{
		m_relays[relay] = isTerminate;
	}
	void HookOff(const Relay& relay)
	{
		typename RelayMap::iterator find = m_relays.find(relay);
		if(find != m_relays.end())
		{
			m_relays.erase(find);
		}
	}

	//list signal sources as string array
	std::vector<std::string> List(const std::string& key = "") const
	{
		std::vector<std::string> strs;
		for(typename SignalMap::const_iterator ite = m_pool.begin(); ite != m_pool.end(); ite++)
		{
			strs.push_back(ite->first.Print());
		}
		if(key.length() > 0)
		{
			strs = EditDistance::Related(key, strs);
		}
		return strs;
	}

};





template <typename _RETURN, typename _PARAM>
class SignalClassBaseTemplate {
private:
	SignalPool<_RETURN, _PARAM> m_pool;
	
public:
	typedef SignalSinkTemplate<_RETURN, _PARAM> SignalSink;
	typedef SignalSourceTemplate<_RETURN, _PARAM> SignalSource;
	typedef SignalPool<_RETURN, _PARAM>* Relay;
	
	_RETURN Invoke(const Identity& identity, _PARAM param) const
	{
		return m_pool.Invoke(identity, param);
	}
	bool HookOn(const Identity& identity, const SignalSink& sink)
	{
		return m_pool.HookOn(identity, sink);
	}
	bool HookOff(const Identity& identity, const SignalSink& sink)
	{
		return m_pool.HookOff(identity, sink);
	}
	void HookOn(const Relay& relay, bool isTerminate = false)
	{
		m_pool.HookOn(relay, isTerminate);
	}
	void HookOff(const Relay& relay)
	{
		m_pool.HookOff(relay);
	}
	void HookOn(SignalClassBaseTemplate<_RETURN, _PARAM>* obj, bool isTerminate = false)
	{
		m_pool.HookOn(&(obj->m_pool), isTerminate);
	}
	void HookOff(SignalClassBaseTemplate<_RETURN, _PARAM>* obj)
	{
		m_pool.HookOff(&(obj->m_pool));
	}
	std::vector<std::string> List(const std::string& key = "") const
	{
		return m_pool.List(key);
	}
};


#endif
