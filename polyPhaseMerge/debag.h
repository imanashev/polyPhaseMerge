#pragma once
#include <string>
using namespace std;
typedef unsigned long long ull;


void generator(string filename, ull n);

void show_file(string filename);

void check(string filename);

void len(string filename);

void bin2stream(string filename);

void debag(FileManager& manager, bool flag)
{
	int count = 6;
	if (!flag)
	{
		std::cout << std::endl << "==============================="  << std::endl;

		for (int i = 0; i < count - 1; i++)
		{
			std::cout << "Cונטי ג " << i << "-ל פאיכו: " << manager.output[i].numSeries << "/" << manager.output[i].idleSeries << std::endl;
		}
		std::cout << "Cונטי ג out פאיכו: " << manager.input[0].numSeries << "/" << manager.input[0].idleSeries << std::endl;
		std::cout << "-----------------------------";
		for (int i = 0; i < count - 1; i++)
		{
			cout << endl;
			bin2stream("__tmp1_" + std::to_string(i) + ".bin");
		}
		//bin2stream("__tmp0_0.bin");
		std::cout << "===============================" << std::endl;
	}
	else
	{
		std::cout << std::endl << "===============================" << std::endl;
		for (int i = 0; i < count - 1; i++)
		{
			std::cout << "Cונטי ג " << i << "-ל פאיכו: " << manager.input[i].numSeries << "/" << manager.input[i].idleSeries << std::endl;
		}
		std::cout << "Cונטי ג out פאיכו: " << manager.output[0].numSeries << "/" << manager.output[0].idleSeries << std::endl;
		std::cout << "-----------------------------";
		for (int i = 0; i < count - 1; i++)
		{
			cout << endl;
			bin2stream("__tmp1_" + std::to_string(i) + ".bin");
		}
		cout << endl;
		bin2stream("__tmp0_0.bin");
		std::cout << "===============================" << std::endl;
	}
}
