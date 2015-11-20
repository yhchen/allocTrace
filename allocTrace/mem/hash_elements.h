#ifndef __SG2D_HASH_ELEMENTS_H__
#define __SG2D_HASH_ELEMENTS_H__

namespace SG2D
{
	/**
	 * 对象哈希值计算函数
	 */
    template <typename T>
    inline size_t hash_elements(const T* ptr, size_t len)
    {
        const T* end = ptr + len;
        size_t _Val = 2166136261U;
        while (ptr != end)
            _Val = 16777619U * _Val ^ (size_t)*ptr++;
        return (_Val);
    }
}

#endif