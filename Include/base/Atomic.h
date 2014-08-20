#pragma once

#include <assert.h>

#ifndef __GNUC__
	#if _MSC_VER>=1400
		#include <Windows.h>
		#include <WinBase.h>
		/*
		#include <intrin.h>
		#pragma intrinsic(_InterlockedCompareExchange, _InterlockedCompareExchange64)
		#pragma warning(disable: 4311) // pointer truncation
		*/
	#else
		#define _NO_LOCK_FREE_
	#endif
#endif

#ifdef _NO_LOCK_FREE_
	#include "Lock.h"
#endif

namespace AGBase
{
	
	/************************************************************************/
	/*          LockFree 的基本节点                                         */
	/************************************************************************/
	template<typename T>
	struct LockFree_Node
	{
	public:
		T							value;
	#ifdef _NO_LOCK_FREE_
		LockFree_Node<T>*			pnext;
	#else
		LockFree_Node<T>* volatile	pnext;
	#endif

		LockFree_Node( void ) : value( ), pnext( 0 ) { }

	private:
		LockFree_Node( const LockFree_Node& );
		LockFree_Node &operator=( const LockFree_Node& );
	};

	/************************************************************************/
	/*		if(*ptr==oldv)													*/
	/*			*ptr=newv; return true										*/
	/*		else															*/
	/*			return false;												*/
	/************************************************************************/
	template<typename T>
	bool CAS( LockFree_Node<T>* volatile* _ptr, LockFree_Node<T>* oldv, LockFree_Node<T>* newv )
	{ 
#ifdef __GNUC__

		register bool f;

		__asm__ __volatile__("lock cmpxchgl %1,%2 \n\t sete %%al"
			: "=a"(f)
			: "r"(newv), "m"(*_ptr), "0"(oldv)
			: "memory");

		return f;

#else

#if _MSC_VER>=1400

		long Comperand = reinterpret_cast<long>(oldv); 
		long Exchange  = reinterpret_cast<long>(newv); 

		return InterlockedCompareExchange(reinterpret_cast<long volatile*>(_ptr), Exchange, Comperand)==Comperand; 

#else

		register bool f;

		_asm
		{
			mov ecx, _ptr
				mov eax, oldv
				mov ebx, newv
				lock cmpxchg [ecx], ebx
				setz f
		}

		return f;

#endif

#endif
	}

	/************************************************************************/
	/*		long long Comperand = ( (old1)|(static_cast<long long>(old2)<<32) ); */
	/*		long long Exchange  = ( (new1)|(static_cast<long long>(new2)<<32) ); */
	/*		if(*ptr==Comperand)												*/
	/*			*ptr=Exchange;return true;									*/
	/*		else															*/
	/*			return false;												*/
	/************************************************************************/
	template<typename T>
	bool CAS2( LockFree_Node<T>* volatile* _ptr, LockFree_Node<T>* old1, long old2, LockFree_Node<T>* new1, long new2 )
	{
#ifdef __GNUC__

		register bool f;

		__asm__ __volatile__ ("lock cmpxchg8b (%1) \n\t sete %%al"
			: "=a"(f)
			: "D"(_ptr), "a"(old1), "d"(old2), "b"(new1), "c"(new2)
			: "memory");

		return f;

#else	

#if _MSC_VER>=1400

		long long Comperand = ( reinterpret_cast<long long>(old1)|(static_cast<long long>(old2)<<32) ); 
		long long Exchange  = ( reinterpret_cast<long long>(new1)|(static_cast<long long>(new2)<<32) ); 

		return ::_InterlockedCompareExchange64( reinterpret_cast<long long volatile*>(_ptr), Exchange, Comperand) == Comperand; 

#else

		register bool f;

		_asm
		{
			mov esi, _ptr
				mov eax, old1
				mov edx, old2
				mov ebx, new1
				mov ecx, new2
				lock cmpxchg8b [esi]
			setz f
		}

		return f;

#endif

#endif
	}


	/************************************************************************/
	/*          long r = *_ptr;                                              */
	/*		    *_ptr += dv;                                                */
	/*			return r;													*/
	/************************************************************************/
	inline long EAA( long volatile*_ptr, long dv )
	{
#ifdef __GNUC__

		int r;

		__asm__ __volatile__
			(
			"lock\n\t"
			"xadd %1, %0":
		"=m"( *_ptr ), "=r"( r ): // outputs (%0, %1)
		"m"( *_ptr ), "1"( dv ): // inputs (%2, %3 == %1)
		"memory", "cc" // clobbers
			);

		return r;

#else

		//long oldv = 0;
		//while ( 1 )
		//{
		//	oldv = *_ptr;

		//	if ( _InterlockedCompareExchange(_ptr, oldv+dv, oldv)==oldv )
		//	{
		//		break;
		//	}
		//}

		//_asm
		//{
		//	mov ecx, _ptr
		//		mov eax, dv
		//		lock xadd [ecx], eax
		//}

		return InterlockedExchangeAdd(_ptr, dv);

#endif

	}

	/************************************************************************/
	/*       堆栈                                                           */
	/************************************************************************/
	template<class T>
	class CLockFree_Stack
	{
	public:
		typedef LockFree_Node<T> node;

		CLockFree_Stack( void )
#ifdef _NO_LOCK_FREE_
			: m_phead( 0 ), m_nCount( 0 )
#else
			: m_phead( 0 ), m_cpops( 0 ), m_nCount( 0 )
#endif
		{
		}

		~CLockFree_Stack( void ) 
		{
		}

		size_t GetSize(){ assert(m_nCount>=0); return (size_t)m_nCount; }

		// 入栈 
		void Push( node* pnode )
		{
#ifdef _NO_LOCK_FREE_
			CSelfLock l( m_lock );

			pnode->pnext = m_phead;
			m_phead		 = pnode;
#else
			while (1)
			{
				pnode->pnext = m_phead;
				if ( CAS( &m_phead, pnode->pnext, pnode ) )
				{
					break;
				}
			}
#endif
			InterlockedExchangeAdd(&m_nCount,1);
		}
		// 出栈
		node* Pop( void )
		{
#ifdef _NO_LOCK_FREE_
			CSelfLock l( m_lock );

			if( m_phead==0 )	return 0;

			node* phead = m_phead;
			m_phead = phead->pnext;

			if ( phead )
			{
				InterlockedExchangeAdd(&m_nCount,-1);
			}

			return phead;
#else
			while (1)
			{
				node*			phead = m_phead;
				unsigned long	cpops = m_cpops;

				if ( 0==phead )		return 0;

				node* pnext = phead->pnext;
				if ( CAS2( &m_phead, phead, cpops, pnext, cpops+1) )
				{
					if ( phead )
					{
						InterlockedExchangeAdd(&m_nCount,-1);
					}

					return phead;
				}
			}
			return 0;
#endif
		}

	private:
	#ifdef _NO_LOCK_FREE_
		node*	m_phead;
		CLock	m_lock;
	#else
		node* volatile			m_phead;
		volatile unsigned long	m_cpops;
	#endif

		long volatile           m_nCount;     //记录数据的数量

	private:
		CLockFree_Stack( const CLockFree_Stack& );
		CLockFree_Stack& operator=( const CLockFree_Stack& );
	};


	/************************************************************************/
	/*       队列                                                           */
	/************************************************************************/
	template<class T>
	class CLockFree_Queue
	{
	public:
		typedef LockFree_Node<T> node;

		CLockFree_Queue( node* head )
#ifndef _NO_LOCK_FREE_
			 : m_cpops( 0 ), m_cpushes( 0 ) 
#endif
		{
			assert( head );
			m_ptail = m_phead = head;
		}

		~CLockFree_Queue( void )
		{
			m_phead = m_ptail = 0;
#ifndef _NO_LOCK_FREE_
			m_cpops = m_cpushes=0;
#endif
		}

		// 入队列
		void Push( node* pnode )
		{
#ifdef _NO_LOCK_FREE_
			CSelfLock l( m_lock );

			pnode->pnext = 0;
			m_ptail->pnext= pnode;
			m_ptail = pnode;
#else
			pnode->pnext = 0;

			unsigned long	cpushes = 0;
			node*			ptail = 0;

			while( 1 )
			{
				cpushes = m_cpushes;
				ptail	= m_ptail;

				// 如果m_ptail指向的节点是最后一个节点
				// 那么，就更新这个节点，使其指向新加入的节点
				if ( CAS( &m_ptail->pnext, reinterpret_cast<node*>(0), pnode ) )
				{
					break;
				}
				else
				{
					// 由于m_ptail并没有指向最后一个节点
					// 那么需要一直更新它，直到它指向队列中最后一个节点
					CAS2( &m_ptail, ptail, cpushes, m_ptail->pnext, cpushes+1);
				}
			}

			// 如果m_ptail指向了我们认为的最后一个节点
			// 那么就更新这个指针，使它指向新增加的节点
			CAS2( &m_ptail, ptail, cpushes, pnode, cpushes+1);
#endif
		}
		// 出队列
		node* Pop( void )
		{
#ifdef _NO_LOCK_FREE_
			CSelfLock l( m_lock );

			if ( m_phead==m_ptail )	return 0;

			node* phead = m_phead;
			m_phead = phead->pnext;
			phead->pnext = 0;

			return phead;
#else
			T value = T();
			node* phead = 0;

			while( 1 )
			{
				unsigned long cpops  = m_cpops;
				unsigned long cpushes= m_cpushes;
				phead = m_phead;
				node* pnext = phead->pnext;

				// 确定我们不是在其他更新操作的过程之中类得到这些指针
				if ( cpops != m_cpops )
				{
					continue;
				}

				// 检查队列是否为空
				if (phead == m_ptail)
				{
					if (NULL == pnext)
					{
						phead = 0;
						break;
					}

					// 特殊情况，如果队列中确实有节点，但是ptail落在后面
					// 那么让ptail离开队列头部
					CAS2( &m_ptail, phead, cpushes, pnext, cpushes+1);
				}
				else if ( 0!=pnext )
				{
					value = pnext->value;

					// 移动队列头指针，有效地移除节点
					if ( CAS2( &m_phead, phead, cpops, pnext, cpops+1) )
					{
						break;
					}
				}
			}

			if ( 0 != phead )
			{
				phead->value = value;
			}

			return phead;
#endif
		}

	private:
	#ifdef _NO_LOCK_FREE_
		node*	m_phead;
		node*	m_ptail;
		CLock	m_lock;
	#else
		// 变量顺序不能修改
		node* volatile			m_phead;
		volatile unsigned long	m_cpops;

		node* volatile			m_ptail;
		volatile unsigned long  m_cpushes;
	#endif

	private:
		CLockFree_Queue( const CLockFree_Queue& );
		CLockFree_Queue& operator=( const CLockFree_Queue& );
	};

}

//#endif // __AGBASE_ATOMIC_H__
