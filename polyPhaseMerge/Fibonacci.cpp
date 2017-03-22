#include "Fibonacci.h"

Fibonacci::Fibonacci(int _oder)
{
	oder = _oder;
	row.reserve(oder);
	sum.reserve(oder);
	for (int i = 0; i < oder; i++)
	{
		row.push_back(0);
		sum.push_back(1);
	}
	row[end] = 1;

}

unsigned int Fibonacci::next()
{
	/* Вычисление следующего числа*/
	end = (end + 1) % oder;
	int i = (end + 1) % oder;
	while (i != end)
	{
		row[end] += row[i];
		i = (i + 1) % oder;
	}

	/* Вычисление суммы */
	for (int i = 0; i < oder; i++)
	{
		sum[i] = row[end];
		int j = (end + 1 + i) % oder;
		while (j != end)
		{
			sum[i] += row[j];
			j = (j + 1) % oder;
		}
	}
	return row[end];
}

unsigned int Fibonacci::get() const
{
	return row[end];
}

int Fibonacci::getOder() const
{
	return oder;
}

int Fibonacci::operator[](int i) const
{
	return row[(end + 1 + i) % oder];
}

/*
unsigned int* Fibonacci::sum() const
{
	unsigned int* array = new unsigned int[oder];
	for (int i = 0; i < oder; i++)
	{
		array[i] = row[end];
		int j = (end + 1 + i) % oder;
		while (j != end)
		{
			array[i] += row[j];
			j = (j + 1) % oder;
		}
	}
	return array;
}
*/