#include "stdafx.h"
#include "base/Array.h"
#include "AnsiString.h"

namespace SG2D
{

	AnsiString AnsiString::operator + (const char* str) const
	{
		if (!str || !str[0])
			return *this;

		size_t myCount = length();
		size_t srcLen = strlen(str);
		AnsiString result(myCount + srcLen);
		char *destPtr = result.own_ptr();
		if (myCount > 0)
		{
			memcpy(destPtr, m_ptr, myCount * sizeof(*m_ptr));
			destPtr += myCount;
		}
		memcpy(destPtr, str, srcLen * sizeof(*m_ptr));
		destPtr += srcLen;
		return result;
	}


	int AnsiString::compare(const AnsiString& another) const
	{
		if (another.ptr() == m_ptr)
			return 0;
		int cmp = (int)(length() - another.length());
		if (cmp != 0)
			return cmp;
		return strcmp(m_ptr, another.m_ptr);
	}


	AnsiString& AnsiString::format(const AnsiString fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(fmt.ptr(), args);
		va_end(args);
		return *this;
	}

	AnsiString& AnsiString::format(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(fmt, args);
		va_end(args);
		return *this;
	}

	AnsiString& AnsiString::format(size_t max, const AnsiString fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(max, fmt.ptr(), args);
		va_end(args);
		return *this;
	}

	AnsiString& AnsiString::format(size_t max, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(max, fmt, args);
		va_end(args);
		return *this;
	}

	AnsiString& AnsiString::formatArgs(const char *fmt, va_list args)
	{
		char buffer[4059];
		const size_t len = formatStringArgs(buffer, sizeof(buffer)/sizeof(buffer[0]), fmt, args);
		exclude(len);
		if (len > 0)
		{
			memcpy(m_ptr, buffer, len * sizeof(*m_ptr));
			m_ptr[len] = 0;
		}
		return *this;
	}

	AnsiString& AnsiString::formatArgs(size_t max, const char *fmt, va_list args)
	{
		exclude(max);
		// warning C4996: 'vsnprintf': This function or variable may be unsafe. 
#pragma warning (disable:4996)
		const size_t len = formatStringArgs(m_ptr, max, fmt, args);
		exclude(len);
		if (len > 0)
				m_ptr[len] = 0;
		return *this;
	}

	AnsiString& AnsiString::catWith(size_t max, const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		catWithArgs(max, fmt, args);
		va_end(args);
		return *this;
	}

	AnsiString& AnsiString::catWithArgs(size_t max, const char *fmt, va_list args)
	{
		char buffer[4059];
		char* bufferPtr;
		size_t myLen = length();
		if (max != 0)
		{
			setLength(myLen + max);
			bufferPtr = m_ptr + myLen;
		}
		else
		{
			bufferPtr = buffer;
			max = sizeof(buffer)/sizeof(buffer[0]);
		}
		const size_t len = formatStringArgs(bufferPtr, max, fmt, args);
		if (len > 0)
		{
			exclude(myLen + len);
			if (bufferPtr == buffer)
				memcpy(m_ptr + myLen, buffer, len * sizeof(*m_ptr));
			m_ptr[myLen + len] = 0;
		}
		return *this;
	}

	size_t AnsiString::formatStringArgs(char *buffer, size_t max, const char *fmt, va_list args)
	{
		// warning C4996: 'vsnprintf': This function or variable may be unsafe. 
#pragma warning (disable:4996)
		size_t len = vsnprintf(buffer, max, fmt, args);
		if (len == -1)//-1表示填满了缓冲区
			len = max;
		return len;
	}

	double AnsiString::toNumber() const
	{
		if (!m_ptr)
			return 0;
		char* e = NULL;
		double d = strtod(m_ptr, &e);
		if (e && e[0])
			return 0;
		return d;
	}

	long long AnsiString::toInt64() const
	{
		if (!m_ptr)
			return 0;
		const char *ptr = m_ptr;
		bool neg = false;
		if (*ptr == '-')
		{
			neg = true;
			ptr++;
		}
		long long lu = 0;
		while (*ptr)
		{
			lu *= 10;
			lu += *ptr - '0';
			ptr++;
		}
		return neg ? -lu : lu;
	}
}
