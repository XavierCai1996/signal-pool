#include "parameter-list.h"
#include "edit-distance.h"
#include "simple-assert.h"

#include <sstream>


template <typename _T>
std::string ParameterList::ItemTemplate<_T>::Print()
{
	std::ostringstream oss;
	oss << key << "[" << param.GetName() << "] : " << description;
	return oss.str();
}


ParameterList::ParameterList()
	: m_isRequireHelp(false)
{ }

ParameterList::ParameterList(const ParameterList& o)
	: m_isRequireHelp(false)
{
	if(o.m_isRequireHelp)
	{
		ASSERT_WITH_MSG(false, "can not copy the ParameterList::Help");
	}

	for(ListMap::const_iterator ite = o.m_list.begin(); ite != o.m_list.end(); ite++)
	{
		m_list.insert(std::make_pair(ite->first, new Item(*(ite->second))));
	}
}

ParameterList::~ParameterList()
{
	for(ListMap::iterator ite = m_list.begin(); ite != m_list.end(); ite++)
	{
		delete ite->second;
		ite->second = NULL;
	}
}

ParameterList& ParameterList::Remove(const std::string key)
{
	ListMap::iterator find = m_list.find(key);
	bool result = find != m_list.end();
	ASSERT_WITH_MSG(result, "can not find key : " << key << "\n" << ListRelated(key));
	delete find->second;
	m_list.erase(find);
	return *this;
}

bool ParameterList::Have(const std::string key) const
{
	return m_list.find(key) != m_list.end();
}

Parameter& ParameterList::Get(const std::string key)
{
	ListMap::iterator find = m_list.find(key);
	bool result = find != m_list.end();
	ASSERT_WITH_MSG(result, "can not find key : " << key << "\n" << ListRelated(key));
	return find->second->param;
}

Parameter ParameterList::GetConst(const std::string key) const
{
	ListMap::const_iterator find = m_list.find(key);
	bool result = find != m_list.end();
	ASSERT_WITH_MSG(result, "can not find key : " << key << "\n" << ListRelated(key));
	return find->second->param;
}

std::string ParameterList::ListRelated(const std::string key, unsigned int maxDistance) const
{
	std::vector<std::string> strs;
	for(ListMap::const_iterator ite = m_list.begin(); ite != m_list.end(); ite++)
	{
		strs.push_back(ite->first);
	}
	strs = EditDistance::Related(key, strs);
	
	std::ostringstream oss;
	if(strs.size() > 0)
	{
		oss << "related parameters (with max edit distance = " << maxDistance << ") : \n";
		for(unsigned int i = 0; i < strs.size(); i++)
		{
			oss << " " << m_list.find(strs[i])->second->Print() << "\n";
		}
	}
	return oss.str();
}

std::string ParameterList::List() const
{
	std::ostringstream oss;
	unsigned int index = 1;
	if(m_list.size() <= 0)
	{
		oss << "no parameters in list";
	}
	else
	{
		for(ListMap::const_iterator ite = m_list.begin(); ite != m_list.end(); ite++, index++)
		{
			oss << "list(" << index << ") " << ite->second->Print() << "\n";
		}
	}
	return oss.str();
}

ParameterList::MergeMode::MergeMode(char mode)
	: m_mode(mode)
{ }

bool ParameterList::MergeMode::IsOne(unsigned int index) const
{
	ASSERT_WITH_MSG(index >= 0 && index < sizeof(char) * 8, "invalid index");
	return (m_mode >> index) & 0x01;
}

char ParameterList::MergeMode::GetOne(unsigned int index)
{
	ASSERT_WITH_MSG(index >= 0 && index < sizeof(char) * 8, "invalid index");
	char c = '\0' | (0x01 << index);
	return c;
}

bool ParameterList::MergeMode::IsReplace() const
{
	return !IsOne(0);
}

bool ParameterList::MergeMode::IsGreedy() const
{
	return !IsOne(1);
}

char ParameterList::MergeMode::replace = '\0'; //default value should be 0
char ParameterList::MergeMode::retain = GetOne(0);

char ParameterList::MergeMode::greedy = '\0'; //default value should be 0
char ParameterList::MergeMode::conservative = GetOne(1);

ParameterList& ParameterList::Merge(const ParameterList& o, const MergeMode& mode)
{
	return Merge(o, RequireList(), mode);
}

ParameterList& ParameterList::Merge(const ParameterList& o, const ParameterList::RequireList& requires, const MergeMode& mode)
{
	//print help information if need
	if(o.m_isRequireHelp)
	{
		std::size_t defaultN = m_list.size();
		std::size_t requiresN = requires.Size();
		if(defaultN <= 0 && requiresN <= 0)
		{
			std::cout << "\nno arguments in this function" << std::endl;
		}
		else
		{
			std::cout << "\n(format: key[type] : description)\n";
			if(requiresN > 0)
			{
				std::cout << "arguments required : \n";
				for(std::size_t i = 0; i < requiresN; i++)
				{
					std::cout << " " << requires.Get(i).Print() << std::endl;
				}
			}
			if(defaultN > 0)
			{
				std::cout << "arguments with default value : \n";
				for(ListMap::const_iterator ite = m_list.begin(); ite != m_list.end(); ite++)
				{
					std::cout << " " << ite->second->Print() << std::endl;
				}
			}
		}
		ASSERT_WITH_MSG(false, "print help information in function ParameterList::" << __FUNCTION__);
	}

	bool isReplace = mode.IsReplace();
	bool isGreedy = mode.IsGreedy();
	std::map<std::string, bool> keys;//for mode conservative
	
	//requires check
	bool result = true;
	for(std::size_t i = 0; i < requires.Size(); i++)
	{
		std::string key = requires.Get(i).key;
		if(!o.Have(key))
		{
			result = false;
			ASSERT_WITH_MSG(false, "parameter key [" << requires.Get(i).key << "] is required!");
		}
		else if(o.GetConst(key).GetTypeVerify() != requires.Get(i).param)
		{
			result = false;
			ASSERT_WITH_MSG(false, "the type of parameter with key [" << requires.Get(i).key << "] should be [" << requires.Get(i).param.GetName() << "]");
		}
		if(!isGreedy)
		{
			keys[key] = true;
		}
	}

	//merge
	if(result)
	{
		for(ListMap::const_iterator ite = o.m_list.begin(); ite != o.m_list.end(); ite++)
		{
			if(Have(ite->first) && isReplace) //replace
			{
				bool typeCheck = Get(ite->first).CompareType(ite->second->param);
				ASSERT_WITH_MSG(typeCheck, "the type of parameter with key[" << ite->first << "] should be [" << Get(ite->first).GetName() << "]");
				Get(ite->first) = ite->second->param;
			}
			else //add
			{
				if(isGreedy || (keys.find(ite->first) != keys.end()))
				{
					m_list.insert(std::make_pair(ite->first, new Item(*(ite->second))));
				}
			}
		}
	}
	
	return *this;
}


/* //use for ParameterList::RequireList("str1", "str2", "strN", NULL)
   //example: list.Merge(inList, ParameterList::RequireList("a", "b", NULL));
ParameterList::RequireList::RequireList(const char* str)
{
	m_strings.push_back(str);
}

ParameterList::RequireList::RequireList(const char* str1, const char* str2, ...)
{
	m_strings.push_back(str1);
	va_list argv;
	va_start(argv, str2);
	while(str2 != NULL)
	{
		m_strings.push_back(str2);
		str2 = va_arg(argv, const char*);
	}
	va_end(argv);
}
*/

ParameterList::RequireItem ParameterList::RequireList::Get(unsigned int i) const
{
	return m_requires[i];
}

std::size_t ParameterList::RequireList::Size() const
{
	return m_requires.size();
}

const ParameterList& ParameterList::Help()
{
	static ParameterList help;
	help.m_isRequireHelp = true;
	return help;
}
