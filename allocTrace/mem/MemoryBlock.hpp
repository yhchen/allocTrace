#ifndef __SG2D_MEMORY_BLOCK_H__
#define __SG2D_MEMORY_BLOCK_H__

namespace SG2D
{
	template<typename _Type, int GrowSize>
	class MemoryBlock
	{
	public:
		_Type *start;
		_Type *end;
		_Type *offset;
	public:
		MemoryBlock(){ start = end = offset = NULL; }
		~MemoryBlock() { clear(); }
		inline void clear ()
		{
			if (start)
			{
				free(start);
				start = end = offset = NULL;
			}
		}
		inline _Type* add(const _Type *data, size_t count)
		{
			if ((size_t)(end - offset) < count)
			{
				reserve(offset - start + count);
			}
			memcpy(offset, data, count * sizeof(start[0]));
			offset += count;
			return offset;
		}
		inline _Type* add(const _Type &data)
		{
			if ((size_t)(end - offset) < 1)
			{
				reserve(offset - start + 1);
			}
			*offset = data;
			offset++;
			return offset;
		}
		inline _Type* insert(size_t index, size_t count, bool zero = false)
		{
			__DEBUG_ASSERT__(count > 0);
			const size_t myCount = offset - start;
			if (index > myCount)
			{
				__DEBUG_ASSERT__(false);
				return NULL;
			}
			if ((size_t)(end - offset) < count)
			{
				reserve(myCount + count);
			}
			if (index < myCount)
				memmove(&start[index + count], &start[index], sizeof(_Type) * (size_t)(offset - start - index));
			if (zero)
				memset(&start[index], 0, sizeof(_Type) * count);
			offset += count;
			return &start[index];
		}
		inline _Type pop_back()
		{
			if (offset > start)
			{
				offset--;
				_Type t = *offset;
				return t;
			}
			_Type t = {0};
			return t;
		}
		inline void remove(size_t index, size_t count)
		{
			if (index < (size_t)(offset - start))
			{
				size_t myCount = offset - start;
				if (count > myCount - index)
					count = myCount - index;
				memcpy(&start[index], &start[index + count], sizeof(_Type) * (myCount - index - count));
				offset -= count;
			}
		}
		inline void move(size_t newIndex, size_t oldIndex)
		{
			if (newIndex != oldIndex && newIndex >= 0 && newIndex < (size_t)(offset - start))
			{
				_Type tmp = start[oldIndex];
				remove(oldIndex, 1);
				*insert(newIndex, 1, false) = tmp;
			}
		}
		inline void reserve(size_t newSize)
		{
			if (newSize != end - start)
			{
				if (newSize <= 0)
				{
					clear();
				}
				else if (newSize > size_t(end - start))
				{
					newSize = ((newSize - 1) / GrowSize + 1) * GrowSize;
					size_t pos = offset - start;
					start = (_Type*)realloc(start, sizeof(start[0]) * newSize);
					end = start + newSize;
					offset = start + pos;
				}
			}
		}
		inline void resize(size_t newSize)
		{
			if (newSize > (size_t)(end - start))
				reserve(newSize);
			offset = start + newSize;
		}
		inline size_t capacity() const
		{
			return end - start;
		}
		inline size_t size() const
		{
			return offset - start;
		}
		inline _Type& operator [] (size_t index){ __DEBUG_ASSERT__(index < size()); return start[index]; }
		inline const _Type& operator [] (size_t index) const { __DEBUG_ASSERT__(index < size());  return start[index]; }
	};
}

#endif