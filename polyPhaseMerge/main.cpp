#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <vector>
#include "FileManager.h"
#include "polyPhaseMerge.h"
#include "Fibonacci.h"

#define N 50000	//count of elements to sort
#define INPUT "input.txt"
#define OUTPUT "input(sorted).txt"


using namespace std;
typedef unsigned long long ull;

/* Support functions*/
void generator(string filename, ull n)
{
	srand(time(NULL));
	ofstream file;
	file.open(filename);
	for (ull i = 0; i < n; i++)
	{
		file << " " << rand();
	}
	file.close();
}

void show_file(string filename)
{
	ifstream file;
	file.open(filename);
	int temp;
	while (!file.eof())
	{
		file >> temp;
		cout << temp << " ";
	}
	cout << endl;
}

void check(string filename)
{
	bool flag = 1;
	ull counter = 1;
	ull tmp1, tmp2;
	ifstream f;
	f.open(filename);
	f >> tmp1;
	while (!f.eof())
	{
		f >> tmp2;
		counter++;
		if (tmp1 > tmp2)
		{
			cout << "Error pos: " << counter << "  |  " << tmp1 << " > " << tmp2 << endl;

			flag = 0;
		}
		tmp1 = tmp2;
	}
	if (flag)
	{
		cout << "Sorting status: ok" << endl;
	}
	cout << counter << " elements was sorted" << endl;
}

void len(string filename)
{
	ull counter = 0;
	ifstream f;
	f.open(filename);
	ull tmp;
	while (!f.eof())
	{
		f >> tmp;
		counter++;
	}
	cout << counter << " elements to sort" << endl;
}

void bin2stream(string filename)
{
	fstream file;
	file.open(filename, std::ios::in | std::ios::binary);
	int tmp;
	int len;
	cout << "File \"" << filename << "\" contain:" << endl;
	file.read((char *)&len, sizeof(int));
	while (len)
	{
		cout << "[" << len << "]";
		for (int i = 0; i < len; i++)
		{
			file.read((char *)&tmp, sizeof(int));
			cout << " " << tmp;
		}
		cout << endl;

		if(!file.read((char *)&len, sizeof(int)))
			break;
	}
	file.close();
}

int main()
{
	setlocale(LC_ALL, "Russian");
	int count = 6;
	generator(INPUT, N);
	polyPhaseMerge(INPUT);
	check("input(sorted).txt");
	/*for (int i = 0; i < count - 1; i++)
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
