#include "identity.h"

#define MAX(a, b) (a > b ? a : b)

unsigned int Identity::m_maxSize = 4 * Identity::GetSingleMaxSize();

int Identity::GetSingleMaxSize()
{
	class SizeClass {
	public:
		virtual ~SizeClass() = 0;
		void Function(const int& param) { }
	};//class SizeClass
	static unsigned int singleMaxSize = MAX(2 * sizeof(&SizeClass::Function), 32);
	return singleMaxSize;
}

Identity::Identity(const std::string& str)
	: m_id(str)
{
	assert(m_id.size() <= m_maxSize);
}

std::string Identity::Print() const
{
	return m_id;
}

Identity Identity::operator + (const Identity& o) const
{
	return Identity(m_id + o.m_id);
}

bool Identity::operator == (const Identity& o) const
{
	return m_id == o.m_id;
}
bool Identity::operator != (const Identity& o) const
{
	return m_id != o.m_id;
}
bool Identity::operator < (const Identity& o) const
{
	return m_id < o.m_id;
}
bool Identity::operator > (const Identity& o) const
{
	return m_id > o.m_id;
}
bool Identity::operator <= (const Identity& o) const
{
	return m_id <= o.m_id;
}
bool Identity::operator >= (const Identity& o) const
{
	return m_id >= o.m_id;
}
