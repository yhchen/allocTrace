#ifndef __SG2D_ANSISTRING_HASH_MAP_H__
#define __SG2D_ANSISTRING_HASH_MAP_H__

#include "hash_elements.h"
#if defined(__GNUC__) || (_MSC_VER >= 11)
#include "__gnu_hash_imp.h"
#endif

namespace SG2D
{
	/**
	 * SG2D AnsiStringHashMap 基于AnsiString的对象哈希表
	 *
	 */
	class AnsiStringHash : public std::hash<AnsiString>
	{
	public:
		inline size_t operator() (const AnsiString &str) const
		{ return hash_elements(str.ptr(), str.length()); }
	};

	template<class T>
	class AnsiStringHashMap : public std::unordered_map<const AnsiString, T, AnsiStringHash>
	{
	public:
		typedef AnsiString HashKeyType;
		typedef std::pair<AnsiString, T> HashMapPair;
	};

	/**
	 * SG2D UTF8StringHashMap 基于UTF8String的对象哈希表
	 *
	 */
	template<class T>
	class UTF8StringHashMap : public unordered_map<const UTF8String, T, AnsiStringHash>
	{
	public:
		typedef UTF8String HashKeyType;
		typedef pair<UTF8String, T> HashMapPair;
	};

	/**
	 * SG2D MultiByteStringHashMap 基于MultiByteString的对象哈希表
	 *
	 */
	template<class T>
	class MultiByteStringHashMap : public unordered_map<const MultiByteString, T, AnsiStringHash>
	{
	public:
		typedef MultiByteString HashKeyType;
		typedef pair<MultiByteString, T> HashMapPair;
	};

	
	/**
	 * SG2D UnicodeStringHashMap 基于UnicodeString的对象哈希表
	 *
	 */
	class UnicodeStringHash : public hash<UnicodeStringBase>
	{
	public:
		inline size_t operator() (const UnicodeStringBase &str) const
		{ return hash_elements(str.ptr(), str.length()); }
	};

	template<class T>
	class UnicodeStringHashMap : public unordered_map<const UnicodeStringBase, T, UnicodeStringHash>
	{
	public:
		typedef UnicodeStringBase HashKeyType;
		typedef pair<UnicodeStringBase, T> HashMapPair;
        
	};

#if defined(_UNICODESTR)
	template<class T>
	class StringHashMap : public UnicodeStringHashMap<T>{};
#elif defined(_MBSTR)
	template<class T>
	class StringHashMap : public MultiByteStringHashMap<T>{};
#else
	template<class T>
	class StringHashMap : public UTF8StringHashMap<T>{};
#endif
}

#endif