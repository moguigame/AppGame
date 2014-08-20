#pragma once

#include <string>

namespace AGBase
{
	// ͨ�ù�����ͺ���
	// ��ó���Ŀ¼
	extern std::string	GetModulePath( void );
	// ������Ŀ¼
	extern bool			CreateDir(const char * path);

	// ���IP��ַ���ַ�����port
	extern short		GetPeerPort(int socketid);
	extern std::string	GetPeerStringIP(int socketid);
	extern long			GetPeerLongIP(int socketid);
	extern short		GetPort(const struct sockaddr_in &addr);
	extern std::string	GetIP(const struct sockaddr_in &addr);
	extern bool			PingTest(const char *ips, int timeout);

	// sleep, ����
	extern void			SleepMillisecond(int millsec);
	// ���ϵͳʱ�䣬�Ժ����ʾ
	extern unsigned long long GetTimeMs( void );
	// ���cpu����������ʱ���������, �뼶��
	extern unsigned long GetSecondCount( void );

	//time_t -> tm ���̰߳�ȫ
	//time_t *time_input:�����time_t���͵�ʱ��
	//struct tm* tm_result:�����tm�ṹ��ʱ��
	extern int			time_to_tm(time_t *time_input,struct tm* tm_result);
	//tm -> time_t ���̰߳�ȫ
	extern int			tm_to_time(struct tm* tm_input, time_t *time_result);
	// ʱ���ַ���
	extern std::string	GetTimeString( void );
	extern std::string  GetDataTimeString( void );

	// ���ֽ�Ϊ�ǿ���ʾ�ַ�, ��ת����16�������
	extern std::string MemoryToString(const void *memory, int size);

	// Converts all the characters in this string to lowercase characters
	extern std::string Lower(const std::string &str);
	// Converts all the characters in this string to uppercase characters
	extern std::string Upper(const std::string &str);
	// Trims all leading and trailing whitespace characters from the string
	extern std::string Trim(const std::string &str);
	// Trims leading whitespace characters from the string
	extern std::string TrimLeft(const std::string &str);
	// Trims trailing whitespace characters from the string
	extern std::string TrimRight(const std::string &str);
}

//#endif // __AGBASE_UTILS_H__

