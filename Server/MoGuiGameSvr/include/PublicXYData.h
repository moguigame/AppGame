#pragma once

#include <string>
#include <cassert>

#include <stdint.h>

#include "biostream.h"
#include "publicxydef.h"

namespace MoGui
{
namespace MoGuiXY
{
namespace PublicXYData
{
	using namespace MoGui::MoGuiXY;

	enum DATA_FALG
	{
		DATA_FLAG_0,
		DATA_FLAG_CHAR,
		DATA_FLAG_SHORT,
		DATA_FLAG_INT,
		DATA_FLAG_LONGLONG
	};

	struct SBigNumber
	{
		long long m_nBigNumber;

		SBigNumber() { reset(); }
		void reset() { m_nBigNumber = 0; }
		friend bostream& operator<<(bostream& bos, const SBigNumber& bn)
		{
			if ( bn.m_nBigNumber == 0 )
			{
				bos << static_cast<char>(DATA_FLAG_0);
			}
			else if( bn.m_nBigNumber <= INT8_MAX && bn.m_nBigNumber >= INT8_MIN )
			{
				bos << static_cast<char>(DATA_FLAG_CHAR);
				bos << static_cast<char>(bn.m_nBigNumber);
			}
			else if( bn.m_nBigNumber <= INT16_MAX && bn.m_nBigNumber >= INT16_MIN )
			{
				bos << static_cast<char>(DATA_FLAG_SHORT);
				bos << static_cast<short>(bn.m_nBigNumber);
			}
			else if( bn.m_nBigNumber <= INT32_MAX && bn.m_nBigNumber >= INT32_MIN )
			{
				bos << static_cast<char>(DATA_FLAG_INT);
				bos << static_cast<int>(bn.m_nBigNumber);
			}
			else
			{
				bos << static_cast<char>(DATA_FLAG_LONGLONG);
				bos << bn.m_nBigNumber;
			}

			return bos;
		}

		friend bistream& operator>>(bistream& bis, SBigNumber& bn)
		{
			bn.reset();

			unsigned char DataFlag;
			bis >> DataFlag;

			switch(DataFlag)
			{
			case DATA_FLAG_0:
				{
					bn.m_nBigNumber = 0;
				}
				break;
			case DATA_FLAG_CHAR:
				{
					char TempChar;
					bis >> TempChar;
					bn.m_nBigNumber = TempChar;					
				}
				break;
			case DATA_FLAG_SHORT:
				{
					short TempShort;
					bis >> TempShort;
					bn.m_nBigNumber = TempShort;
				}
				break;
			case DATA_FLAG_INT:
				{
					int TempInt;
					bis >> TempInt;
					bn.m_nBigNumber = TempInt;
				}
				break;
			default:
				{
					bis >> bn.m_nBigNumber;
				}
			}

			return bis;
		}

		SBigNumber( const SBigNumber& bn)
		{
			this->m_nBigNumber = bn.m_nBigNumber;
		}
		SBigNumber( const unsigned long long unum)
		{
			assert(unum<=INT64_MAX);
			this->m_nBigNumber = unum;
		}
		SBigNumber( const long long num)
		{
			this->m_nBigNumber = num;
		}
		SBigNumber( const int num )
		{
			this->m_nBigNumber = num;
		}
		SBigNumber ( const unsigned int num)
		{
			this->m_nBigNumber = num;
		}
		SBigNumber ( const unsigned long int num )
		{
			this->m_nBigNumber = num;
		}
		SBigNumber ( const long int num )
		{
			this->m_nBigNumber = num;
		}
		SBigNumber& operator=( const SBigNumber& bn)
		{
			if(this == &bn)
			{
				return *this;
			}

			this->m_nBigNumber = bn.m_nBigNumber;

			return *this;
		}
		operator long long() const
		{
			return m_nBigNumber;
		}
		long long get() const
		{
			return m_nBigNumber;
		}
		SBigNumber& operator+=( const long long& bn)
		{
			this->m_nBigNumber += bn;
			return *this;
		}
		SBigNumber& operator-=( const long long& bn)
		{
			this->m_nBigNumber -= bn;
			return *this;
		}
	};

	struct UBigNumber
	{
		unsigned long long           m_nBigNumber;

		UBigNumber() { reset(); }
		void reset() { m_nBigNumber = 0; }
		friend bostream& operator<<(bostream& bos, const UBigNumber& bn)
		{
			if( bn.m_nBigNumber == 0 )
			{
				bos << static_cast<unsigned char>(DATA_FLAG_0);
			}
			else if( bn.m_nBigNumber <= UINT8_MAX )
			{
				bos << static_cast<unsigned char>(DATA_FLAG_CHAR);
				bos << static_cast<unsigned char>(bn.m_nBigNumber);
			}
			else if(bn.m_nBigNumber <= UINT16_MAX )
			{
				bos << static_cast<unsigned char>(DATA_FLAG_SHORT);
				bos << static_cast<unsigned short>(bn.m_nBigNumber);
			}
			else if(bn.m_nBigNumber <= UINT32_MAX )
			{
				bos << static_cast<unsigned char>(DATA_FLAG_INT);
				bos << static_cast<unsigned int>(bn.m_nBigNumber);
			}
			else
			{
				bos << static_cast<unsigned char>(DATA_FLAG_LONGLONG);
				bos << bn.m_nBigNumber;
			}

			return bos;
		}

		friend bistream& operator>>(bistream& bis, UBigNumber& bn)
		{
			bn.reset();

			unsigned char TempMaxFlag;
			bis >> TempMaxFlag;
			if( TempMaxFlag == DATA_FLAG_0 )
			{
				bn.m_nBigNumber = 0;
			}
			else if( TempMaxFlag == DATA_FLAG_CHAR )
			{
				unsigned char TempChar;
				bis >> TempChar;
				bn.m_nBigNumber = TempChar;
			}
			else if ( TempMaxFlag == DATA_FLAG_SHORT)
			{
				unsigned short TempShort;
				bis >> TempShort;
				bn.m_nBigNumber = TempShort;
			}
			else if ( TempMaxFlag == DATA_FLAG_INT )
			{
				unsigned int TempInt;
				bis >> TempInt;
				bn.m_nBigNumber = TempInt;
			}
			else
			{
				bis >> bn.m_nBigNumber;
			}

			return bis;
		}

		UBigNumber( const UBigNumber& bn)
		{
			this->m_nBigNumber = bn.m_nBigNumber;
		}
		UBigNumber( const unsigned long long unum)
		{
			this->m_nBigNumber = unum;
		}
		UBigNumber( const long long num)
		{
			assert(num>=0);
			this->m_nBigNumber = num;
		}
		UBigNumber( const int num )
		{
			assert(num>=0);
			this->m_nBigNumber = num;
		}
		UBigNumber ( const unsigned int num)
		{
			this->m_nBigNumber = num;
		}
		UBigNumber ( const unsigned long int num )
		{
			this->m_nBigNumber = num;
		}
		UBigNumber ( const long int num )
		{
			assert(num>=0);
			this->m_nBigNumber = num;
		}
		UBigNumber& operator=( const UBigNumber& bn)
		{
			if(this == &bn)
			{
				return *this;
			}
			this->m_nBigNumber = bn.m_nBigNumber;

			return *this;
		}
		operator unsigned long long() const
		{
			return m_nBigNumber;
		}
	};

	struct Session
	{
		char m_Session[MAX_SESSION_SIZE+1];

		Session() { ReSet(); }
		void ReSet(){ memset(this,0,sizeof(*this));}

		friend bostream& operator<<( bostream& bos, const Session& ss )
		{
			for(int i=0;i<MAX_SESSION_SIZE;i++)
			{
				bos << ss.m_Session[i];
			}
			return bos;
		}
		friend bistream& operator>>( bistream& bis, Session& ss )
		{
			ss.ReSet();
		
			for(int i=0;i<MAX_SESSION_SIZE;i++)
			{
				bis >> ss.m_Session[i];
			}

			return bis;
		}
		std::string GetString()
		{
			m_Session[MAX_SESSION_SIZE] = 0;
			return std::string(m_Session);
		}
		Session(const Session& ss)
		{
			Copy(ss);
		}		
		Session(std::string strSS)
		{
			for (int i=0;i<MAX_SESSION_SIZE;i++)
			{
				if (i < int(strSS.length()))
				{
					m_Session[i] = strSS[i];
				}
				else
				{
					m_Session[i] = 0;
				}
			}
			m_Session[MAX_SESSION_SIZE] = 0;
		}
		Session& operator=(const Session& ss)
		{
			if( this != &ss )
			{
				Copy(ss);
			}
			return *this;
		}
		void Copy( const Session& ss )
		{
			for(int i=0;i<MAX_SESSION_SIZE;i++)
			{
				m_Session[i] = ss.m_Session[i];
			}
			m_Session[MAX_SESSION_SIZE] = 0;
		}
		friend bool operator==(const Session& lss,const Session& rss)
		{
			for(int i=0;i<MAX_SESSION_SIZE;i++)
			{
				if ( lss.m_Session[i] != rss.m_Session[i] )
				{
					return false;
				}
			}
			return true;
		}		
	};
}
}
}
