#include "polyPhaseMerge.h"
#include "debag.h"


void polyPhaseMerge(std::string filename)
{
	int count = 6;	// ������ ���������� ����������� ������
	FileManager manager(1, count - 1);
	std::fstream file;
	file.open(filename, std::ios::in);
	firstDistrib(file, manager);	// ��������� �������������
	file.close();

	/* ������� ������ ������ �����*/
	int min = manager.output[0].idleSeries;
	for (int i = 1; i < count -1; i++)
	{
		if (min > manager.output[i].idleSeries)
		{
			min = manager.output[i].idleSeries;
		}
	}
	for (int i = 0; i < count - 1; i++)
	{
		manager.output[i].idleSeries -= min;
	}
	manager.input[0].idleSeries += min;

	//debag(manager, 0);	// ����� ���������� ��� ��������

	manager.swap(); // ������ �����
	polyPhaseMergeHelper(manager);
	//debag(manager, 1);	// ����� ���������� ��� ��������

	/* ���������� �������������� ���� ����*/
	manager.swap(); // ������ �����
	std::fstream result;
	result.open(filename.insert(filename.length() - 4, "(sorted)"), std::ios::out); //create result file
	int len = manager.read(0);
	std::cout << endl << "RESULT: " << len << endl;
	for (int i = 0; i < len; i++)
	{
		result << " " << manager.read(0);
	}
	result.close();
}

int polyPhaseMergeHelper(FileManager& manager)
{
	std::multimap <int, int> buffer;				// ����� ��� ��������� <��������, ����� �����>
	int *seriesLen = new int[manager.getInCount()]; // ������ � ������ �����
	int seriesSum = 2;								// ����� ���������� �����
	int currentLen;									// ����� ������������ �����
	int count = manager.getInCount();				// ����� ������ 
	int k = 0;

	while (seriesSum > 1)
	{
		cout << "step " << k++ << endl;
		/* ��������� ���������� ������ */
		seriesSum = 0;
		currentLen = 0;
		for (int i = 0; i <count; i++)
		{
			if (manager.input[i].idleSeries)	// ���� ������ �����
			{
				seriesLen[i] = 0;
				manager.input[i].idleSeries--;
			}
			else if (manager.input[i].numSeries)	// ���� �����
			{
				seriesLen[i] = manager.read(i);
				manager.input[i].numSeries--;
				buffer.insert(std::pair <int, int>(manager.read(i), i));
			}
			else	// ��� �����
			{
				manager.swap(i, 0);
				if (manager.input[i].idleSeries)
				{
					i--;
					continue;
				}			
				seriesLen[i] = manager.read(i);
				manager.input[i].numSeries--;
				buffer.insert(std::pair <int, int>(manager.read(i), i));
				//currentLen += seriesLen[i]--;
				//seriesSum += manager.input[i].numSeries + manager.input[i].idleSeries + 1;
			}
			currentLen += seriesLen[i]--;
			seriesSum += manager.input[i].numSeries + manager.input[i].idleSeries + 1;
		}


		//seriesSum += manager.output[0].idleSeries;
		//std::cout << std::endl << "Series SUM = " << seriesSum << std::endl << std::endl;
		//debag(manager, 1);	// ����� ���������� ��� ��������
		//if (seriesSum == 1)	break;
		
		//for (int i = 0; i < count; i++)
		//	std::cout << "len " << i << " = " << seriesLen[i] << endl;

		if (currentLen)
		{
			manager.write(0, currentLen);	// ���������� ����� ��������� �����
			manager.output[0].numSeries++;
		}
		else
		{
			manager.output[0].idleSeries++;
		}

		/* �������� �����*/
		auto it = buffer.begin();
		while (!buffer.empty())
		{
			it = buffer.begin();
			manager.write(0, it->first);
			if (seriesLen[it->second]--)
			{
				buffer.insert(std::pair <int, int>(manager.read(it->second), it->second));
			}
			buffer.erase(it);
		}
		seriesSum = 0;
		for (int i = 0; i < count; i++)
		{
			seriesSum += manager.input[i].numSeries + manager.input[i].idleSeries;
		}
		seriesSum += manager.output[0].numSeries + manager.output[0].idleSeries;
		//debag(manager, 1);	// ����� ���������� ��� ��������
	}
	return 0;
}

/// ������� ������ ��������������� �������������
void firstDistrib(std::fstream& stream, FileManager&  manager)
{	
	int count = manager.getOutCount();	// ����� ������, ���� ����� ������������
	Fibonacci fib(count);				// ������� ��� ���������
	
	int current = 0;					// ������ �������� �����
	int len = 0;						// ����� ������� �����

	std::streampos startPos, endPos;
	startPos = manager.output[current].file.tellg(); // ����������� ����� ��� ����� ��������� �����
	manager.write(current, 0);

	int* diff = new int[count];			// ������� (������� �������� �������� ����� ��� ������� ������������� �� ���������)
	for (int i = 0; i < count; i++)		// ���������� �������
	{
		diff[i] = 1;
	}
	diff[0] = 0;

	bool stop = 0;						// ���� ��������� (����� �� �������� �����)

	int prev = 0, next;					// �������� ��� ������������� ���������
	stream >> next;
	manager.output[current].numSeries++;
	for(;;)
	{
		// ���������� ����� 
		while (prev <= next)
		{
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // ������ ��������� �������
			{
				stream >> next;
			}
			else // ���� ���� ����������
			{
				stop = 1;
				break;
			}
		}
		if (stop) break;

		// ���������� ����� �����
		{
			endPos = manager.output[current].file.tellg();	// ���������� ��������� �������
			manager.output[current].file.seekg(startPos);	// ������ ������� �� ����������������� ����� ����� �����
			manager.write(current, len);					// ���������� ����� ������� �����
			manager.output[current].file.seekg(endPos);		// ���������� ������� �� ����� �����

			// ������ ���� �������� ������������� �� ��������� 
			if ((current < count - 1) && ((diff[current] + 1) == diff[current + 1]))	// ������������� ���������� �����
			{
				current++;
				diff[current]--;
				manager.output[current].numSeries++;
			}
			else
			{
				if (!diff[count - 1]) // ��������� ����� ����� � �������
				{
					fib.next();
					for (int i = 0; i < count; i++)
					{
						diff[i] = fib.sum[i] - manager.output[i].numSeries;
					}

				}
				current = 0;
				diff[current]--;
				manager.output[current].numSeries++;
			}
			
			// ����������� ����� ��� ����� ��������� �����
			startPos = manager.output[current].file.tellg();
			manager.write(current, 0);

			// ���������� ������� ��������� ����� 
			manager.write(current, next);
			len = 1;			// �������� ����� ��������� ����� + ����������� �� 1
			prev = next;
			if (!stream.eof())	// ������ ��������� �������
			{
				stream >> next;
			}
			else // ���� ���� ����������
			{
				break;
			}
		}
	}
	// ���������� ����� �����
	endPos = manager.output[current].file.tellg();	// ���������� ��������� �������
	manager.output[current].file.seekg(startPos);	// ������ ������� �� ����������������� ����� ����� �����
	manager.write(current, len);					// ���������� ����� ������� �����
	manager.output[current].file.seekg(endPos);		// ���������� ������� �� ����� �����

	// ���������� ���������� ������ �����
	for (int i = 0; i < count; i++) 
	{
		manager.output[i].idleSeries = diff[i];
	}
}


//������� ������, ��� ��������� �����, ������ ������������� �� ��������� 17.03.17
/*
void firstDistrib(std::fstream& stream, FileManager&  manager)
{
	int count = manager.getOutCount();			// ����� ������
	Fibonacci fib(count);						// ������� ��� ���������
												//	int* a = new int [count];					// �������
	int prev, next;								// �������� ��� ������������� ���������
	int current = 0;							// � ����� ������ ������ ��������
	int len = 0;
	std::streampos startPos = manager.output[current].file.tellg();
	std::streampos endPos;
	manager.output[current].numSeries++;
	manager.write(current, 0);
	prev = 0;
	stream >> next;
	while (true)
	{
		// ���������� ����� 
		while (prev <= next)
		{
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // ������ ��������� �������
			{
				stream >> next;
			}
			else // ���� ���� ����������
			{
				endPos = manager.output[current].file.tellg();	// ���������� ��������� �������
				manager.output[current].file.seekg(startPos);	// ���������� ����� ������� �����
				manager.write(current, len);
				manager.output[current].file.seekg(endPos);		// ���������� ������� �� ����� �����
				return;
			}
		}
		// ������ ���� 
		{
			endPos = manager.output[current].file.tellg();	// ���������� ��������� �������
			manager.output[current].file.seekg(startPos);	// ���������� ����� ������� �����
			manager.write(current, len);
			manager.output[current].file.seekg(endPos);		// ���������� ������� �� ����� �����

															// ������������� �� ��������� 
			int i = 0;
			while (i < count) // <
			{
				current = (current + 1) % count;
				if ((manager.output[current].numSeries) < (fib.sum[current]))
				{
					manager.output[current].numSeries++;
					break;
				}
				i++;
			}
			if (i == count)
			{
				fib.next();
				current = 0;
				manager.output[current].numSeries++;
			}


			// ���������� ������� ������ ����� (����� ��� ����� �����) 

			startPos = manager.output[current].file.tellg();
			manager.write(current, 0);

			len = 0; // �������� ����� ��������� �����

					 // ���������� ������� ��������� ����� 
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // ������ ��������� �������
			{
				stream >> next;
			}
			else // ���� ���� ����������
			{
				endPos = manager.output[current].file.tellg();	// ���������� ��������� �������
				manager.output[current].file.seekg(startPos);	// ���������� ����� ������� �����
				manager.write(current, len);
				manager.output[current].file.seekg(endPos);		// ���������� ������� �� ����� �����
				break;
			}
		}
	}

}
*/