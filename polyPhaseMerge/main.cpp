#include <iostream>
#include "polyPhaseMerge.h"
#include "testsHelper.h"
//#include "FileManager.h"
//#include "Fibonacci.h"

#define N 50000	//count of elements to sort
#define INPUT "input.txt"
#define OUTPUT "input(sorted).txt"

using namespace std;


int main()
{
	setlocale(LC_ALL, "Russian");
	generator(INPUT, N);
	polyPhaseMerge(INPUT);
	check(OUTPUT);
	/*for (int i = 0; i < 5; i++)
	{
		cout << endl;
		bin2stream("__tmp1_" + std::to_string(i) + ".bin");
	}*/
}

/* Проверка класса Фибоначчи
Fibonacci a(5);
cout << a.get() << endl;
for (int i = 0; i < 15; i++)
{
unsigned int* tmp = a.sum();
for (int j = 0; j < 5; j++)
{
cout << tmp[j] << " ";

}
cout << endl << endl << a.next() << endl;
}
*/
