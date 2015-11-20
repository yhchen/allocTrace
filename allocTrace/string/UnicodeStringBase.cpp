#include "stdafx.h"
#include "../base/Array.h"
#include "UnicodeStringBase.h"

namespace SG2D
{
	UnicodeStringBase UnicodeStringBase::operator + (const wchar_t* str) const
	{
		if (!str || !str[0])
			return *this;

		size_t myCount = length();
		size_t srcLen = wcslen(str);
		UnicodeStringBase result(myCount + srcLen);
		wchar_t *destPtr = result.own_ptr();
		if (myCount > 0)
		{
			memcpy(destPtr, m_ptr, myCount * sizeof(*m_ptr));
			destPtr += myCount;
		}
		memcpy(destPtr, str, srcLen * sizeof(*m_ptr));
		destPtr += srcLen;
		return result;
	}

	bool UnicodeStringBase::operator == (const UnicodeStringBase& another) const
	{
		const wchar_t* aptr = another.ptr();
		if (m_ptr == aptr)
			return true;
		if (!m_ptr && !aptr[0])
			return true;
		if (!m_ptr[0] && !aptr)
			return true;
		if (length() != another.length())
			return false;
		return compare(another) == 0;
	}

	UnicodeStringBase& UnicodeStringBase::format(const UnicodeStringBase fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(fmt.ptr(), args);
		va_end(args);
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::format(const wchar_t* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(fmt, args);
		va_end(args);
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::format(size_t max, const UnicodeStringBase fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(max, fmt.ptr(), args);
		va_end(args);
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::format(size_t max, const wchar_t* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		formatArgs(max, fmt, args);
		va_end(args);
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::formatArgs(const wchar_t *fmt, va_list args)
	{
		wchar_t buffer[4059];
		const size_t len = formatStringArgs(buffer, sizeof(buffer)/sizeof(buffer[0]), fmt, args);
		exclude(len);
		if (len > 0)
		{
			memcpy(m_ptr, buffer, len * sizeof(*m_ptr));
			m_ptr[len] = 0;
		}
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::formatArgs(size_t max, const wchar_t *fmt, va_list args)
	{
		exclude(max);
		const size_t len = formatStringArgs(m_ptr, max, fmt, args);
		exclude(len);
		if (len > 0)
			m_ptr[len] = 0;
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::catWith(size_t max, const wchar_t *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		catWithArgs(max, fmt, args);
		va_end(args);
		return *this;
	}

	UnicodeStringBase& UnicodeStringBase::catWithArgs(size_t max, const wchar_t *fmt, va_list args)
	{
		wchar_t buffer[4059];
		wchar_t* bufferPtr;
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

	size_t UnicodeStringBase::formatStringArgs(wchar_t *buffer, size_t max, const wchar_t *fmt, va_list args)
	{
		// warning C4996: 'vsnprintf': This function or variable may be unsafe. 
#pragma warning (disable:4996)
        int len = vswprintf(buffer, max, fmt, args);
		if (len == -1)//-1表示填满了缓冲区
			len = (int)max;
		return len;
	}

	double UnicodeStringBase::toNumber() const
	{
		if (!m_ptr)
			return 0;
		wchar_t* e = NULL;
		double d = wcstod(m_ptr, &e);
		if (e && e[0])
			return 0;
		return d;
	}

	long long UnicodeStringBase::toInt64() const
	{
		if (!m_ptr)
			return 0;
		const wchar_t *ptr = m_ptr;
		long long lu = 0;
		while (*ptr)
		{
			lu *= 10;
			lu += *ptr - '0';
			ptr++;
		}
		return lu;
	}
}
