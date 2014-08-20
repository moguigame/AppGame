#pragma once

// 注意问题：
// 不保证线程安全
// 若要保证 一个线程读，一个线程写 是安全的，需要加上内存壁垒

namespace AGBase
{
	/************************************************************************/
	/*	                                                                    */
	/*					循环队列											*/
	/*			如果T为某个指针，那么不要调用Clear，用Pop来pop掉所有的数据	*/
	/*			队列的总长度为1<<SIZE_BIT									*/
	/*																		*/
	/************************************************************************/
	template<typename T, unsigned long SIZE_BIT=10 >
	class CCircleQueue
	{
	public:
		CCircleQueue( void )
		{
			assert(SIZE_BIT>0);
			_queuemask	= ((1<<SIZE_BIT)-1);
			_queue		= new T[(1<<SIZE_BIT)];
			_queuehead	= 0;
			_queuetail	= 0;
		}

		~CCircleQueue( void )
		{
			delete [] _queue;
		}

		// 清理掉内部所有数据
		void Clear( void )
		{
			_queuehead	= 0;
			_queuetail	= 0;
		}

		// 插入一个数据在队列尾
		bool Push( const T& data )
		{
			unsigned long tmptail = ((_queuetail+1)&_queuemask);
			if ( tmptail==_queuehead )		return false;

			_queuetail = tmptail;
			_queue[tmptail] = data;

			return true;
		}

		// 取出一个数据从队列头
		bool Pop( T& data )
		{
			if (_queuehead == _queuetail)	return false;

			unsigned long tmphead = ((_queuehead+1)&_queuemask);
			_queuehead = tmphead;
			data = _queue[tmphead];

			return true;
		}

		// 队列是否为空
		bool IsEmpty( void ) const
		{
			if (_queuehead == _queuetail)	return true;

			return false;
		}

		// 队列的总长度
		unsigned long TotalSize( void ) const
		{
			return (_queuemask+1);
		}

		// 队列当前长度
		unsigned long Size( void ) const
		{
			if ( _queuehead == _queuetail )		return 0;

			return ((_queuetail-_queuehead+_queuemask+1)&_queuemask);
		}

	private:
		T*						_queue;
		unsigned long			_queuemask;

		volatile unsigned long	_queuehead;
		volatile unsigned long	_queuetail;

	private:
		CCircleQueue( const CCircleQueue& );
		CCircleQueue& operator=( const CCircleQueue& );
	};

}

//#endif // __COMMON_CIRCLEQUEUE_H__
