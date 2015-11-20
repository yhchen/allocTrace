#ifndef __SG2D_OBJECT_H__
#define __SG2D_OBJECT_H__

#include <assert.h>

/**
* ���������࣬����SG2D����Ļ��ࡣ
* ������������ü���������������ü���Ϊ0ʱ�������������
* ���������ָ���ͨ������retain���Ӷ������ü�����ɾ������
* ָ�����ú�ʹ��release���ٶ������ü�����
*/
class Object
{
private:
	//���ü��������λ��ʾɾ����ǣ����������ڱ�����ʱ���λ�ᱻ��1������31λ��ʾʵ�����ü�����
	//ʹ�����ü���������������ڴ���һ����Ҫ������������Σ�������Ϊ���ü���Ϊ0����Ҫ��ɾ������
	//ʱ�ڶ�������������п��ܻ�������˶������Ӻͼ������ã�����һ���������ٵĶ����������ò�
	//��������ʱ���������ü����������������ԭ��release�������жϵݼ����ú����Ӧ�����٣���
	//���ٴη���Զ�������ٲ��������´˶������ٶ�Ρ���ˣ�Ϊ���������ر����������������ܹ�
	//ʶ��������Ƿ��������ٴӶ����ڵݼ����ü���ʱ���м��ʶ�𲢱��������ٶ���Ĳ�������˽���
	//�����ü�����Ա�����λ�������������ġ��������١��ı�ǡ�
	unsigned int m_nRefer;

public:
	Object()
	{
		m_nRefer = 1;
	}
	virtual ~Object()
	{

	}
	//��ȡ���ü���
	inline int getRefer()
	{
		return lock_and(m_nRefer, 0x7FFFFFFF);
	}

	//��������
	inline int retain()
	{
		return lock_inc(m_nRefer) & 0x7FFFFFFF;
	}
	//ȡ�����ã�����������ֵΪ0ʱ����������
	inline int release()
	{
#ifdef _DEBUG
		assert((m_nRefer & 0x7FFFFFFF) > 0);
#endif
		unsigned int ret = lock_dec(m_nRefer);
		if (ret == 0)
		{
			lock_or(m_nRefer, 0x80000000);//����ɾ�����
			delete this;
		}
		return ret & 0x7FFFFFFF;
	}

	//�����Ƿ����������� 
	inline bool destroying()
	{
		return (m_nRefer & 0x80000000) != 0;
	}
};

typedef void (Object::*ObjectFunction)();

//���κ����͵����Ա����ת��Ϊ��ַָ��ı�������ƭ����
template <typename class_fn>
inline ObjectFunction objfnptr(class_fn fn)
{
	union { class_fn fn; ObjectFunction objFn; } u = { 0 };
	u.fn = fn;
	return u.objFn;
}

//���κκ���ת��Ϊ��ַָ��ı�������ƭ����
template <typename any_fn>
inline void* fnptr(any_fn fn)
{
	union { any_fn fn; void* ptr; } u = { 0 };
	u.fn = fn;
	return u.ptr;
}

#endif