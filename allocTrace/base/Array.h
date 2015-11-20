#ifndef __SG2D_ARRAY_H__
#define __SG2D_ARRAY_H__

namespace SG2D
{
	/**
	 * SG2D Array ��̬����ģ���ࡣ
	 * 
	 * Array��std::vector��std::array��ͬ��Array�洢�����Ķ��󣬲����ڶ����ڴ�ǰ����һ������
	 * ��������������������Array�洢�����������Ƕ�̬�ģ�����ͨ��setLength������ͬʱ����ʹ��+��
	 * +=�������������׷�����ݡ��������ʵ������ʹ��ͬһ���������ݣ����Ҳ����ظ��������ݣ����Ǹ���
	 * ��������Ҫ�������Լ���Ҫ�漰Ŀ�ģ�������һ��Array������һ���µ�Array������߽�����ֵ����
	 * ��Arrayʱ��������µ��ڴ�����Ϳ��������Array������ָ����ͬ���ڴ���������ͨ�����ü�������
	 * �ù�ͬ���ڴ�����������ָ����ڴ�����Array���󶼱����ٺ󣬸��ڴ�ᱻ�ͷš���һ���յ�Array׷
	 * ������ʱ�������Զ������µ��ڴ������ڴ洢�����ݡ����Ѿ��������ݵ�Array׷�ӻ��д��ͨ��[]�±�
	 * ���㣩ʱ���������������������������������ã�����Զ������ԭ���ڴ��������ò������µ��ڴ���
	 * �洢�µ����ݡ�
	 *
	 * ע�⣺Ƶ���Ķ����ݽ���׷�Ӳ�����ʮ�ֵ�Ч�ģ����Ѿ�ȷ��Ҫ׷��������������ȵ���setLength����
	 * setCapacity�������ճ��Ȼ������ٽ���׷�ӽ�������ܡ�
	 * ע�⣺Array���ֻ�����ṩ���ݴ�ȡ����������д�ŵ����ݲ��ᱻ���ù��캯��������������
	 */
	template <typename T>
	class Array
	{
	public:
		typedef T TYPE;

		struct ArrayDesc
		{
			volatile int refer;
			volatile size_t capacity;
			volatile size_t length;
		};

	protected:
		T* m_ptr;

	public:
		Array()
		{
			m_ptr = NULL;
		}
		Array(const Array<T> &another)
		{
			m_ptr = NULL;
			operator = (another);
		}
		Array(const T* ptr, size_t length)
		{
			m_ptr = NULL;
			setLength(length);
			memcpy(m_ptr, ptr, length * sizeof(*ptr));
		}
		Array(size_t length)
		{
			m_ptr = NULL;
			setLength(length);
		}
		~Array()
		{
			setLength(0);
		}
		inline Array<T>& operator = (const Array<T> &another)
		{
			if (m_ptr != another.m_ptr)
			{
				setLength(0);
				if (another.m_ptr)
				{
					ArrayDesc *pDesc = ((ArrayDesc*)another.m_ptr) - 1;
					lock_inc(pDesc->refer);
					m_ptr = (T*)(pDesc + 1);
				}
			}
			return *this;
		}
		inline Array<T> operator + (const Array<T> &another)
		{
			Array<T> result;
			if (m_ptr || another.m_ptr)
			{
				size_t myCount = length();
				size_t anotherCount = another.length();
				result.setLength(myCount + anotherCount);
				T* destPtr = result.m_ptr;
				if (myCount > 0)
				{
					memcpy(destPtr, m_ptr, myCount * sizeof(*m_ptr));
					destPtr += myCount;
				}
				if (anotherCount > 0)
				{
					memcpy(destPtr, another.m_ptr, anotherCount * sizeof(*m_ptr));
					destPtr += anotherCount;
				}
			}
			return result;
		}
		inline Array<T> operator + (const T &another)
		{
			size_t myCount = length();
			Array<T> result(myCount + 1);
			if (myCount > 0)
				memcpy(result.m_ptr, m_ptr, myCount * sizeof(*m_ptr));
			result.m_ptr[myCount] = another;
			return result;
		}
		inline Array<T>& operator += (const Array<T> &another)
		{
			size_t len = another.length();
			if (len > 0 )
				cat(another.m_ptr, another.length());
			return *this;
		}
		inline Array<T>& operator += (const T &another)
		{
			size_t myCount = length();
			setLength(myCount + 1);
			m_ptr[myCount] = another;
			return *this;
		}
		inline T& operator [] (size_t index)
		{
			if (!m_ptr)
				return *(T*)NULL;
			ArrayDesc *pDesc = ((ArrayDesc*)m_ptr) - 1;
			__DEBUG_ASSERT__(index < pDesc->length);
			duplicate();
			return m_ptr[index];
		}
		inline const T& operator [] (size_t index) const 
		{
			if (!m_ptr)
				return *(T*)NULL;
			ArrayDesc * const pDesc = ((ArrayDesc* const)m_ptr) - 1;
			__DEBUG_ASSERT__(index < pDesc->length);
			return m_ptr[index];
		}
		inline bool operator == (const Array<T> &another) const
		{
			return m_ptr == another.m_ptr;
		}
		inline bool operator != (const Array<T> &another) const
		{
			return m_ptr != another.m_ptr;
		}
		//����ֻ������������ָ��
		inline const T* ptr() const
		{
			return m_ptr;
		}
		//���ؿ�д����������ָ��
		inline T* own_ptr()
		{
			duplicate();
			return m_ptr;
		}
		inline size_t length() const
		{
			if (!m_ptr)
				return 0;
			ArrayDesc * const pDesc = ((ArrayDesc* const)m_ptr) - 1;
			return pDesc->length;
		}
		inline Array<T>& cat(const T* ptr, size_t count)
		{
			return insert(length(), ptr, count);
		}
		inline Array<T>& insert(size_t index, const T* ptr, size_t count)
		{
			if (count > 0)
			{
				ArrayDesc *pMyDesc = m_ptr ? ((ArrayDesc*)m_ptr) - 1 : NULL;
				size_t myCount = pMyDesc ? pMyDesc->length : 0;

				__DEBUG_ASSERT__(index <= myCount);

				//�ж�Ŀ���Ƿ��������Լ��ڴ���������ݣ�����������Լ��ڴ�����ģ���ô����
				//��setLength��ptrָ��ʧЧ����ʱ��Ҫ����ȷ��ptr��λ�á�
				bool isMySelf = false;
				size_t inMyPos = 0;

				if (pMyDesc && (ptr >= m_ptr) && (ptr <= (m_ptr + pMyDesc->capacity)))
				{
					isMySelf = true;
					inMyPos = ptr - m_ptr;
					if (inMyPos >= index)
						inMyPos += count;
				}

				setLength(myCount + count);
				if (index < myCount)
					memmove(&m_ptr[index + count], &m_ptr[index], sizeof(*ptr) * (myCount - index));
				if (isMySelf)
					memcpy(&m_ptr[index], &m_ptr[inMyPos], count * sizeof(*m_ptr));
				else memcpy(&m_ptr[index], ptr, count * sizeof(*m_ptr));
			}
			return *this;
		}
		inline Array<T>& remove(size_t index, size_t count)
		{
			if (count > 0)
			{
				duplicate();

				ArrayDesc *pMyDesc = m_ptr ? ((ArrayDesc*)m_ptr) - 1 : NULL;
				size_t myCount = pMyDesc ? pMyDesc->length : 0;

				__DEBUG_ASSERT__(index < myCount);
				if (count > myCount - index) 
					count = myCount - index;
				size_t copyCount = myCount - index - count;
				if (copyCount > 0)
					memcpy(&m_ptr[index], &m_ptr[index + count], sizeof(*m_ptr) * copyCount);
				setLength(myCount - count);
			}
			return *this;
		}
		inline Array<T>& setLength(const size_t length)
		{
			__DEBUG_ASSERT__(length >= 0);

			ArrayDesc *pDesc = NULL;
			if (m_ptr) pDesc = ((ArrayDesc*)m_ptr) - 1;

			if (length == 0)
			{
				if (pDesc)
				{
					if (lock_dec(pDesc->refer) <= 0)
						free(pDesc);
					m_ptr = NULL;
				}
			}
			else if (pDesc)
			{
				if (length != pDesc->length)
				{
					if (pDesc->refer > 1)
					{
						lock_dec(pDesc->refer);

						ArrayDesc *pNewDesc = (ArrayDesc*)calloc(1, sizeof(*pDesc) + sizeof(*m_ptr) * (length + 1));
						pNewDesc->capacity = length;
						pNewDesc->length = length;
						pNewDesc->refer = 1;
						m_ptr = (T*)(pNewDesc + 1);
						size_t lengthMin = pDesc->length < length ? pDesc->length : length;
						memcpy(m_ptr, pDesc + 1, lengthMin * sizeof(*m_ptr));
					}
					else 
					{
						if (length >= pDesc->capacity)
						{
							pDesc = (ArrayDesc*)realloc(pDesc, sizeof(*pDesc) + sizeof(*m_ptr) * (length + 1));
							m_ptr = (T*)(pDesc + 1);
							memset(&m_ptr[pDesc->length], 0, sizeof(*m_ptr) * (length - pDesc->capacity + 1));
							pDesc->capacity = length;
							pDesc->length = length;
						}
						else pDesc->length = length;
					}
					memset(&m_ptr[length], 0, sizeof(*m_ptr));
				}

			}
			else //no ptr
			{
				ArrayDesc *pNewDesc = (ArrayDesc*)calloc(1, sizeof(*pDesc) + sizeof(*m_ptr) * (length + 1));
				pNewDesc->capacity = length;
				pNewDesc->length = length;
				pNewDesc->refer = 1;
				m_ptr = (T*)(pNewDesc + 1);
			}
			return *this;
		}
		inline Array<T>& exclude(size_t newLength)
		{
			__DEBUG_ASSERT__(newLength >= 0);

			ArrayDesc *pDesc = NULL;
			if (m_ptr) pDesc = ((ArrayDesc*)m_ptr) - 1;

			if (pDesc && pDesc->capacity >= newLength)
			{
				if (lock_dec(pDesc->refer) == 0)
				{
					lock_inc(pDesc->refer);
					pDesc->length = newLength;
					return *this;
				}
				else m_ptr = NULL;
			}
			setLength(newLength);
			return *this;
		}
		inline size_t capacity() const
		{
			if (m_ptr)
			{
				ArrayDesc *pDesc = ((ArrayDesc*)m_ptr) - 1;
				return pDesc->capacity;
			}
			return 0;
		}
		inline Array<T>& setCapacity(size_t capacity)
		{
			size_t myCount = length();
			//��������������������Ҫ������ղ������������setLength(0)
			if (capacity > myCount)
			{
				setLength(capacity);
				ArrayDesc *pDesc = ((ArrayDesc*)m_ptr) - 1;
				pDesc->length = myCount;
			}
			return *this;
		}
	protected:
		inline void duplicate()
		{
			if (m_ptr)
			{
				ArrayDesc *pDesc = ((ArrayDesc*)m_ptr) - 1;
				if (pDesc->refer > 1)
				{
					ArrayDesc *pNewDesc = (ArrayDesc*)malloc(sizeof(*pDesc) + sizeof(*m_ptr) * (pDesc->capacity + 1));
					pNewDesc->capacity = pDesc->capacity;
					pNewDesc->length = pDesc->length;
					pNewDesc->refer = 1;
					m_ptr = (T*)(pNewDesc + 1);
					memcpy(m_ptr, pDesc + 1, (pDesc->length + 1) * sizeof(*m_ptr));

					lock_dec(pDesc->refer);
				}
			}
		}
	};
}

#endif