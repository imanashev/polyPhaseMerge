#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <string>
#include "FileManager.h"
using namespace std;
typedef unsigned long long ull;


void generator(string filename, ull n);
void show_file(string filename);
void check(string filename);
void len(string filename);
void bin2stream(string filename);
void debug(FileManager& manager, bool flag);
