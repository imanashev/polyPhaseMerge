#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

typedef unsigned long long ull;


class FileManager
{
public:
	struct File
	{
		std::fstream file;
		std::string name;
		int numSeries;
		int idleSeries;
		File(std::string _name);
		void swap(File& rhs);
	};
	std::vector <File> input, output;
	int inCount, outCount;
	FileManager(int inCount, int outCount);
	~FileManager();
	void swap();
	void swap(int inIndex, int outIndex);
	int read(int index);
	void write(int index, int value);
	bool eof(int index);
	int getInCount();
	int getOutCount();
};
