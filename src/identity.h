#ifndef IDENTITY_H
#define IDENTITY_H

#include <string>
#include <assert.h>

/*
 *  translate variable to a identity
 *  for generating key value which is using for indexing, such like the key required in a map
 */
class Identity {
private:
	static unsigned int m_maxSize;
	std::string m_id;

	static int GetSingleMaxSize();

	template <typename _T>
	static std::string Create(const _T& v)
	{
		unsigned int size = sizeof(_T);
		assert(size <= GetSingleMaxSize());
		std::string str(size, '\0');
		const char* c = (const char*)&v;
		for(unsigned int i = 0; i < size; i++)
			str[i] = c[i];
		return str;
	}

public:
	template <typename _T>
	Identity(const _T& v)
		: m_id(Create(v))
	{ }

	Identity(const std::string& str);

	std::string Print() const;

	Identity operator + (const Identity& o) const;

	bool operator == (const Identity& o) const;
	bool operator != (const Identity& o) const;
	bool operator < (const Identity& o) const;
	bool operator > (const Identity& o) const;
	bool operator <= (const Identity& o) const;
	bool operator >= (const Identity& o) const;
};//class Identity

#endif //#ifndef IDENTITY_H
