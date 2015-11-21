// Test_allocTrace.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <tchar.h>

void test1()
{
	malloc(100);
}

void test()
{
	void* a = (void*)malloc(100);
	free(a);
	test1();
}

int _tmain(int argc, _TCHAR* argv[])
{
	int* i = (int*)malloc(sizeof(int));
	int* j = new int[1];
	test();
	system("pause");
	return 0;
}

