#include "polyPhaseMerge.h"
#include "debag.h"


void polyPhaseMerge(std::string filename)
{
	int count = 6;	// Узнаем количество создаваемых файлов
	FileManager manager(1, count - 1);
	std::fstream file;
	file.open(filename, std::ios::in);
	firstDistrib(file, manager);	// Начальное распределение
	file.close();

	/* Убираем лишние пустые серии*/
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

	//debag(manager, 0);	// Вывод информации для проверки

	manager.swap(); // Меняем файлы
	polyPhaseMergeHelper(manager);
	//debag(manager, 1);	// Вывод информации для проверки

	/* Записываем результирующий файл файл*/
	manager.swap(); // Меняем файлы
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
	std::multimap <int, int> buffer;				// Буфер для сравнения <значение, номер файла>
	int *seriesLen = new int[manager.getInCount()]; // Массив с длиной серий
	int seriesSum = 2;								// Общее количетсво серий
	int currentLen;									// Длина записываемой серии
	int count = manager.getInCount();				// Число файлов 
	int k = 0;

	while (seriesSum > 1)
	{
		cout << "step " << k++ << endl;
		/* Начальное заполнение буфера */
		seriesSum = 0;
		currentLen = 0;
		for (int i = 0; i <count; i++)
		{
			if (manager.input[i].idleSeries)	// Есть пустые серии
			{
				seriesLen[i] = 0;
				manager.input[i].idleSeries--;
			}
			else if (manager.input[i].numSeries)	// Есть серии
			{
				seriesLen[i] = manager.read(i);
				manager.input[i].numSeries--;
				buffer.insert(std::pair <int, int>(manager.read(i), i));
			}
			else	// Нет серий
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
		//debag(manager, 1);	// Вывод информации для проверки
		//if (seriesSum == 1)	break;
		
		//for (int i = 0; i < count; i++)
		//	std::cout << "len " << i << " = " << seriesLen[i] << endl;

		if (currentLen)
		{
			manager.write(0, currentLen);	// Записываем длину следующей серии
			manager.output[0].numSeries++;
		}
		else
		{
			manager.output[0].idleSeries++;
		}

		/* Сливание серии*/
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
		//debag(manager, 1);	// Вывод информации для проверки
	}
	return 0;
}

/// Рабочая версия первоначального распределения
void firstDistrib(std::fstream& stream, FileManager&  manager)
{	
	int count = manager.getOutCount();	// Число файлов, куда будем распределять
	Fibonacci fib(count);				// Создаем ряд Фибоначчи
	
	int current = 0;					// Индекс текущего файла
	int len = 0;						// Длина текущей серии

	std::streampos startPos, endPos;
	startPos = manager.output[current].file.tellg(); // Резервируем место для длины следующей серии
	manager.write(current, 0);

	int* diff = new int[count];			// Разница (Сколько осталось дописать серий для полного распределения по Фибоначчи)
	for (int i = 0; i < count; i++)		// Заполнение разницы
	{
		diff[i] = 1;
	}
	diff[0] = 0;

	bool stop = 0;						// Флаг остановки (выход из главного цикла)

	int prev = 0, next;					// Элементы для естественного сравнения
	stream >> next;
	manager.output[current].numSeries++;
	for(;;)
	{
		// Записываем серию 
		while (prev <= next)
		{
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // Читаем следующий элемент
			{
				stream >> next;
			}
			else // Если файл закончился
			{
				stop = 1;
				break;
			}
		}
		if (stop) break;

		// Дописываем длину серии
		{
			endPos = manager.output[current].file.tellg();	// Запоминаем последнюю позицию
			manager.output[current].file.seekg(startPos);	// Ставим позицию на зарезервированное место длины серии
			manager.write(current, len);					// Записываем длину прошлой серии
			manager.output[current].file.seekg(endPos);		// Возвращаем позицию на конец файла

			// Меняем файл согласно распределению по Фибоначчи 
			if ((current < count - 1) && ((diff[current] + 1) == diff[current + 1]))	// Выравниваение оставшихся серий
			{
				current++;
				diff[current]--;
				manager.output[current].numSeries++;
			}
			else
			{
				if (!diff[count - 1]) // Вычисляем новые суммы и разницу
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
			
			// Резервируем место для длины следующей серии
			startPos = manager.output[current].file.tellg();
			manager.write(current, 0);

			// Записываем элемент следующей серии 
			manager.write(current, next);
			len = 1;			// Обнуляем длину следующей серии + увеличиваем на 1
			prev = next;
			if (!stream.eof())	// Читаем следующий элемент
			{
				stream >> next;
			}
			else // Если файл закончился
			{
				break;
			}
		}
	}
	// Дописываем длину серии
	endPos = manager.output[current].file.tellg();	// Запоминаем последнюю позицию
	manager.output[current].file.seekg(startPos);	// Ставим позицию на зарезервированное место длины серии
	manager.write(current, len);					// Записываем длину прошлой серии
	manager.output[current].file.seekg(endPos);		// Возвращаем позицию на конец файла

	// Записываем количество пустых серий
	for (int i = 0; i < count; i++) 
	{
		manager.output[i].idleSeries = diff[i];
	}
}


//Рабочая версия, нет фиктивных серий, плохое распределение по фибаначчи 17.03.17
/*
void firstDistrib(std::fstream& stream, FileManager&  manager)
{
	int count = manager.getOutCount();			// Число файлов
	Fibonacci fib(count);						// Создаем ряд Фибоначчи
												//	int* a = new int [count];					// Разница
	int prev, next;								// Элементы для естественного сравнения
	int current = 0;							// С каким файлом сейчас работаем
	int len = 0;
	std::streampos startPos = manager.output[current].file.tellg();
	std::streampos endPos;
	manager.output[current].numSeries++;
	manager.write(current, 0);
	prev = 0;
	stream >> next;
	while (true)
	{
		// Записываем серию 
		while (prev <= next)
		{
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // Читаем следующий элемент
			{
				stream >> next;
			}
			else // Если файл закончился
			{
				endPos = manager.output[current].file.tellg();	// Запоминаем последнюю позицию
				manager.output[current].file.seekg(startPos);	// Записываем длину прошлой серии
				manager.write(current, len);
				manager.output[current].file.seekg(endPos);		// Возвращаем позицию на конец файла
				return;
			}
		}
		// Меняем файл 
		{
			endPos = manager.output[current].file.tellg();	// Запоминаем последнюю позицию
			manager.output[current].file.seekg(startPos);	// Записываем длину прошлой серии
			manager.write(current, len);
			manager.output[current].file.seekg(endPos);		// Возвращаем позицию на конец файла

															// Распределение по Фибоначчи 
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


			// Запоминаем позицию начала серии (место для длины серии) 

			startPos = manager.output[current].file.tellg();
			manager.write(current, 0);

			len = 0; // Обнуляем длину следующей серии

					 // Записываем элемент следующей серии 
			manager.write(current, next);
			len++;
			prev = next;
			if (!stream.eof()) // Читаем следующий элемент
			{
				stream >> next;
			}
			else // Если файл закончился
			{
				endPos = manager.output[current].file.tellg();	// Запоминаем последнюю позицию
				manager.output[current].file.seekg(startPos);	// Записываем длину прошлой серии
				manager.write(current, len);
				manager.output[current].file.seekg(endPos);		// Возвращаем позицию на конец файла
				break;
			}
		}
	}

}
*/