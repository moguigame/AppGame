#pragma once

// 引用技术类
// 注意问题：
// 1、非线程安全

#include <assert.h>

namespace AGBase
{

class RefCount
{
public:
	RefCount(void)
	{
		m_refs=0;
	}

	virtual ~RefCount(void)
	{

	}

	void UpRefs(void)
	{
		m_refs ++;
	}

	void DownRefs(void)
	{
		if ( --m_refs == 0 )
		{
			m_refs = 0;
			OnDestroy( );
		}
		else if ( m_refs < 0 )
		{
			assert( 0 );
		}
	}

	virtual void OnDestroy(void)
	{
		delete this;
	}

private:
	int	m_refs;	
};

template <class T>
class RefPtr
{
public:
	RefPtr( T* p )
	{
		m_p = p;
		if (m_p)     m_p->UpRefs();
	}

	RefPtr(const RefPtr<T>& p)
	{
		m_p = p.m_p;
		if (m_p)     m_p->UpRefs();
	}

	explicit RefPtr(void) : m_p(0)
	{

	}

	~RefPtr(void)
	{
		if(m_p) m_p->DownRefs();
	}

	operator T*(void) const
	{ 
		return m_p;	
	}

	T* operator->(void)const
	{ 
		return m_p;
	}

	T* get(void) const
	{ 
		return m_p;
	}

	RefPtr<T>& operator=(const RefPtr<T>& p)
	{
		if (this != &p && m_p!=p.m_p)
		{
			T* pSrc = m_p;
			m_p = p.m_p;

			if (m_p)    m_p->UpRefs();
			if (pSrc)	pSrc->DownRefs();
		}

		return *this;
	}

	RefPtr<T>& operator=( T* p)
	{
		if (m_p != p)
		{
			T* pSrc = m_p;
			m_p = p;

			if (m_p)    m_p->UpRefs();
			if (pSrc)	pSrc->DownRefs();			
		}

		return *this;
	}

private:
	T*	m_p;
};

}

//#endif // __AGBASE_REFPTR_H__

