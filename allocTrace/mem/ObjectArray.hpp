#ifndef __SG2D_OBJECT_ARRAY_H__
#define __SG2D_OBJECT_ARRAY_H__

namespace SG2D
{
	template<class T = Object>
	class ObjectArray : public Object, protected MemoryBlock<T*, 8>
	{
	protected:
		typedef MemoryBlock<T*, 8> super;
		class ArrayOperation;

	public:
		ObjectArray():Object(),super()
		{
		}
		virtual ~ObjectArray()
		{
			clear();
		}
		inline size_t size() const { return super::size(); }
		inline T** const data(){ return super::start; }
		inline const T* const * data() const { return super::start; }
		inline T*& operator [] (size_t index){ __DEBUG_ASSERT__(index < super::size()); return super::start[index]; }
		inline const T* operator [] (size_t index) const { __DEBUG_ASSERT__(index < super::size());  return super::start[index]; }
		
		//获取对象.
		inline T* get(size_t index){ __DEBUG_ASSERT__(index < super::size()); return super::start[index]; }

		//设置对象
		inline void set(size_t index, T* object){
			__DEBUG_ASSERT__(index < super::size());

			T* pOldObject = super::start[index];

			if (pOldObject == object)
			{
				return;
			}

			super::start[index] = object;

			if (pOldObject)
			{
				pOldObject->release();
			}

			if (object)
			{
				object->retain();
			}
		}


		//清空容器
		inline void clear()
		{
			T** list = super::start;
			for (int i=(int)super::size()-1; i>-1; --i)
			{
				if (list[i]) list[i]->release();
			}
			super::clear();
		}
		//添加对象
		ObjectArray& add(T* object)
		{
			super::add(object);
			if (object)
				object->retain();
			return *this;
		}
		//添加对象数组
		ObjectArray& add(T** objects, size_t count)
		{
			super::add(objects, count);
			for (size_t i=0; i<count; ++i)
			{
				if (objects[i])
					objects[i]->retain();
			}
			return *this;
		}
		//插入对象
		ObjectArray& insert(size_t index, T* object)
		{
			*super::insert(index, 1, false) = object;
			if (object)
				object->retain();
			return *this;
		}
		//插入对象
		ObjectArray& insert(size_t index, T** objects, size_t count)
		{
			T** news = super::insert(index, count, false);
			for (int i=count-1; i>-1; --i)
			{
				news[i] = objects[i];
				if (news[i]) news[i]->retain();
			}
			return *this;
		}
		//删除对象
		inline ObjectArray& remove(const T* object)
		{
			int index = indexOf(object);
			if (index > -1)
			{
				Object *obj = super::start[index];
				super::remove(index, 1);
				if (obj) obj->release();
			}
			return *this;
		}

		//按索引移除对象
		ObjectArray& remove(size_t index, size_t count = 1)
		{
			if (index < size())
			{
				count = min(count, size() - index);
				T** list = super::start;
				for (int i=(int)(index + count - 1); i >= (int)index; --i)
				{
					if (list[i]) list[i]->release();
				}
				super::remove(index, count);
			}
			return *this;
		}

		//改变索引
		inline void move(size_t newIndex, size_t oldIndex)
		{
			super::move(newIndex, oldIndex);
		}

		//弹出并返回最后一个对象
		inline SmartObject<T> pop()
		{
			size_t myLen = super::size();
			if (myLen > 0)
			{
				SmartObject<T> result(super::start[myLen-1]);
				remove(myLen-1, 1);
				return result;
			}
			SmartObject<T> nl;
            return nl;
		}
		int indexOf(const T* object) const
		{
			const T*const * list = super::start;
			for (int i=(int)size()-1; i>-1; --i)
			{
				if (list[i] == object)
					return i;
			}
			return -1;
		}

		void reserve(size_t newSize)
		{
			super::reserve(newSize);
		}

		inline ObjectArray<T>& push_back(T* object)
		{
			return add(object);
		}
		inline SmartObject<T> pop_back()
		{
			return pop();
		}
		inline T* back()
		{
			return (super::offset > super::start) ? *(super::offset - 1) : NULL;
		}

		//删除并销毁所有仅存在于此列表中的对象
		inline int removeAloneObjects()
		{
			int result = 0;
			size_t myLen = super::size();
			if (myLen > 0)
			{
				T** list = super::start;
				for (int i=myLen-1; i>-1; --i)
				{
					T* object = list[i];
					if (!object || object->getRefer() == 1)
					{
						remove(i, 1);
						result++;
					}
				}
			}
			return result;
		}
	};
}

#endif
