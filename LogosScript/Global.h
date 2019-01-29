#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <thread>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Page.h"
#include "Global.h"
#include "Interpreter.h"

#define EMPTY ""

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;