#include "testsHelper.h"

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

		if (!file.read((char *)&len, sizeof(int)))
			break;
	}
	file.close();
}

void debag(FileManager& manager, bool flag)
{
	int count = 6;
	if (!flag)
	{
		std::cout << std::endl << "===============================" << std::endl;

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