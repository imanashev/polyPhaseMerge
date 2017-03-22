#include "polyPhaseMerge.h"
#include "testsHelper.h"


void polyPhaseMerge(std::string filename)
{
	int count = 6;	// Количество создаваемых файлов
	FileManager manager(1, count - 1);
	std::fstream file;
	file.open(filename, std::ios::in);
	firstDistrib(file, manager);	// Начальное распределение
	file.close();

	/* Сливаем пустые серии*/
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

	manager.swap(); // Меняем файлы
	merge(manager); // Сортируем

	/* Записываем результат в файл*/
	manager.swap(); 
	std::fstream result;
	result.open(filename.insert(filename.length() - 4, "(sorted)"), std::ios::out); //create result file
	int len = manager.read(0);
	for (int i = 0; i < len; i++)
	{
		result << " " << manager.read(0);
	}
	result.close();
}

void merge(FileManager& manager)
{
	std::multimap <int, int> buffer;				// Буфер для сравнения <значение, номер файла>
	int *seriesLen = new int[manager.getInCount()]; // Массив с длиной серий
	int seriesSum = 2;								// Общее количетсво серий
	int currentLen;									// Длина записываемой серии
	int count = manager.getInCount();				// Число файлов 

	while (seriesSum > 1)
	{
		/* Начальное заполнение буфера */
		currentLen = 0;
		for (int i = 0; i <count; i++)
		{
			if (manager.input[i].idleSeries) // Есть пустые серии
			{
				seriesLen[i] = 0;
				manager.input[i].idleSeries--;
			}
			else
			{
				if (!manager.input[i].numSeries)  //Cерий нет
				{
					manager.swap(i, 0);
					if (manager.input[i].idleSeries) // В новом файле есть пустые серии
					{
						i--;
						continue;
					}
				}
				seriesLen[i] = manager.read(i);
				manager.input[i].numSeries--;
				buffer.insert(std::pair <int, int>(manager.read(i), i));			
			}
			currentLen += seriesLen[i]--;
		}

		manager.write(0, currentLen);	// Записываем длину следующей серии
		manager.output[0].numSeries++;

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

		/* Считаем сумму серий */
		seriesSum = 0;
		for (int i = 0; i < count; i++)
		{
			seriesSum += manager.input[i].numSeries + manager.input[i].idleSeries;
		}
		seriesSum += manager.output[0].numSeries + manager.output[0].idleSeries;

		cout << "Series count: " << seriesSum << endl;
	}
}

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
