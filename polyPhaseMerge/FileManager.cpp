#include "FileManager.h"


FileManager::FileManager(int _inCount, int _outCount) :inCount(_inCount), outCount(_outCount)
{
	input.reserve(inCount);
	output.reserve(outCount);
	for (int i = 0; i < inCount; i++)
	{
		std::string name = "__tmp0_" + std::to_string(i) + ".bin";
		input.push_back(File(name));
	}
	for (int i = 0; i < outCount; i++)
	{
		std::string name = "__tmp1_" + std::to_string(i) + ".bin";
		output.push_back(File(name));
	}
}

FileManager::~FileManager()
{
	for (int i = 0; i < inCount; i++)
	{
		input[i].file.close();
		//remove(input[i].name.c_str());
	}
	for (int i = 0; i < outCount; i++)
	{
		output[i].file.close();
		//remove(output[i].name.c_str());
	}
}

void FileManager::swap()
{
	for (int i = 0; i < inCount; i++)
	{
		input[i].file.close();
		input[i].file.open(input[i].name, std::ios::out | std::ios::binary);
	}
	for(int i = 0; i < outCount; i++)
	{
		output[i].file.close();
		output[i].file.open(output[i].name, std::ios::in | std::ios::binary);
	}
	input.swap(output);
	int tmp = inCount;
	inCount=outCount;
	outCount = tmp;
}

void FileManager::swap(int inIndex, int outIndex)
{
	input[inIndex].file.close();
	output[outIndex].file.close();
	input[inIndex].file.open(input[inIndex].name, std::ios::out | std::ios::binary);
	output[outIndex].file.open(output[outIndex].name, std::ios::in | std::ios::binary);

	input[inIndex].swap(output[outIndex]);

	//input[inIndex].numSeries = output[outIndex].numSeries;
	//input[inIndex].idleSeries = output[outIndex].idleSeries;
	//output[outIndex].numSeries = 0;
	//output[outIndex].idleSeries = 0;
	//std::cout << std::endl << "SWAP " << inIndex << std::endl;
}

int FileManager::read(int index)
{
	int tmp;
	input[index].file.read((char *)&tmp, sizeof(int));
	//input[index].file >> tmp;
	return tmp;
}

void FileManager::write(int index, int value)
{
	//output[index].file << " " << value;
	output[index].file.write((char *)&value, sizeof(value));
	//std::cout << value << " ";
}

bool FileManager::eof(int index)
{
	return input[index].file.eof();
}

int FileManager::getInCount()
{
	return inCount;
}

int FileManager::getOutCount()
{
	return outCount;
}


FileManager::File::File(std::string _name)
{
	name = _name;
	numSeries = 0;
	idleSeries = 0;
	file = std::fstream(name, std::ios::out | std::ios::binary);
}

void FileManager::File::swap(File& rhs)
{
	file.swap(rhs.file);
	name.swap(rhs.name);
	std::swap(numSeries, rhs.numSeries);
	std::swap(idleSeries, rhs.idleSeries);
}
