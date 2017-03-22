#pragma once
//#include <iostream>
#include <vector>


class Fibonacci
{
public:
	std::vector <unsigned int> row;
	std::vector <unsigned int> sum;
	int end = 0;	// Индекс последнего элемента
	int oder;		// Порядок 
	Fibonacci(int _oder);
	unsigned int next();		// Выисляет и возвращает следующее число
	unsigned int get() const;	// Возвращает последнее число
	int getOder() const;
	int operator[](int i) const;
};