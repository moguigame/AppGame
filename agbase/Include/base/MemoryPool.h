#pragma once

// 注意问题：
// 1、保证线程安全
// 2、不支持new[] 和 delete[]，只支持new delete 操作
// 3、new之后CMemoryPool_Public不再保存new出来的内存，与delete必须成对出现，且不能对同一内存多次delete
// 4、所有内存都是单次::malloc出的，所以第一的申请内存的速度不快


#include <memory>
#include <Windows.h>
#include "Atomic.h"

namespace AGBase
{
	template<class T, unsigned long ALLOC_BLOCK_SIZE=5>
	class CMemoryPool_Private
	{
	public:
		typedef LockFree_Node<T> node;

		CMemoryPool_Private( void ):m_nTotalCount(0)
		{
		}

		~CMemoryPool_Private( void )
		{	
		}

		void* Alloc_From_Pool( void )
		{
			node *pret = 0;
			while ( !( pret=m_pool.Pop() ) )
			{
				AllocBlock( );
			}
			return pret;
		}

		T* New_Object( void )
		{
			void* object = Alloc_From_Pool( );
			if ( object )	return ( T* )( ::new(object)T() );

			return 0;
		}

		template<class P0>
		T* New_Object( const P0& p0 )
		{
			void* object = Alloc_From_Pool( );
			if ( object )	return ( T* )( ::new(object)T( p0 ) );

			return 0;	
		}

		template<class P0, class P1>
		T* New_Object( const P0& p0, const P1& p1 )
		{
			void* object = Alloc_From_Pool( );
			if ( object )	return ( T* )( ::new(object)T( p0, p1 ) );

			return 0;	
		}

		template<class P0, class P1, class P2>
		T* New_Object( const P0& p0, const P1& p1, const P2& p2 )
		{
			void* object = Alloc_From_Pool( );
			if ( object )	return ( T* )( ::new(object)T( p0, p1, p2 ) );

			return 0;	
		}

		template<class P0, class P1, class P2, class P3>
		T* New_Object( const P0& p0, const P1& p1, const P2& p2, const P3& p3 )
		{
			void* object = Alloc_From_Pool( );
			if ( object )	return ( T* )( ::new(object)T( p0, p1, p2, p3 ) );

			return 0;	
		}

		void Free_To_Pool(void* deletepointer)
		{
			assert( deletepointer );
			m_pool.Push( reinterpret_cast<node*>(deletepointer) );
		}

		void Delete_Object( void* object )
		{
			if ( object )
			{
				((T*)object)->~T( );
				Free_To_Pool( object );
			}
		}

		void Clear_Pool( void )
		{
			node *pret = 0;
			while ( (pret=m_pool.Pop()) )
			{
				::free(pret);
			}
		}

		size_t GetUseCount()  { return m_nTotalCount - m_pool.GetSize(); }
		size_t GetTotalCount(){ return m_nTotalCount;    }

	private:
		void AllocBlock( void )
		{
			assert( ALLOC_BLOCK_SIZE>0 );

			for (unsigned long i=0; i<ALLOC_BLOCK_SIZE; ++i)
			{
				node* ptemp = (node*)(::malloc( sizeof(node) ));
				if ( ptemp )
				{
					m_nTotalCount++;
					ptemp->pnext = 0;
					m_pool.Push( ptemp );
				}
			}
		}

	private:
		CMemoryPool_Private( const CMemoryPool_Private& );
		CMemoryPool_Private& operator=( const CMemoryPool_Private& );

		CLockFree_Stack<T>	m_pool;
		long volatile       m_nTotalCount;
	};


	template<class T, unsigned long ALLOC_BLOCK_SIZE=5>
	class CMemoryPool_Public
	{
	public:
		virtual ~CMemoryPool_Public( void )
		{

		}

		static void* operator new(size_t alloclength)
		{
			assert( sizeof(T)==alloclength );
			InterlockedExchangeAdd(&s_nNewTimes,1);	
			return m_memorypool.Alloc_From_Pool( );
		}

		static void operator delete(void* deletepointer)
		{
			InterlockedExchangeAdd(&s_nDeleteTimes,1);
			m_memorypool.Free_To_Pool( deletepointer );
		}

		static void Clear_Pool( void )
		{
			m_memorypool.Clear_Pool( );
		}

		static size_t GetUseCount(){ return m_memorypool.GetUseCount(); }
		static size_t GetTotalCount(){ return m_memorypool.GetTotalCount(); }
		static long long GetNewTimes(){ return s_nNewTimes; }
		static long long GetDeleteTimes(){ return s_nDeleteTimes; }

	private:
		static void* operator new[](size_t alloclength);
		static void operator delete[](void* deletepointer);

		static CMemoryPool_Private<T, ALLOC_BLOCK_SIZE>	m_memorypool;

		static long volatile s_nNewTimes;
		static long volatile s_nDeleteTimes;
	};

	template<class T, unsigned long ALLOC_BLOCK_SIZE>
	CMemoryPool_Private<T, ALLOC_BLOCK_SIZE> CMemoryPool_Public<T, ALLOC_BLOCK_SIZE>::m_memorypool;

	template<class T, unsigned long ALLOC_BLOCK_SIZE>
	long volatile CMemoryPool_Public<T, ALLOC_BLOCK_SIZE>::s_nNewTimes = 0;

	template<class T, unsigned long ALLOC_BLOCK_SIZE>
	long volatile CMemoryPool_Public<T, ALLOC_BLOCK_SIZE>::s_nDeleteTimes = 0;
}

//#endif // __AGBASE_MEMORYPOOL_H__
