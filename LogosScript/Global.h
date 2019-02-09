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
#include "Interpreter.h"

#define EMPTY ""

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;


void interpreter_start(const SOCKET client_socket, const int file_id);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session);
void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData = false);
void do_line_script_operators(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
bool do_line_script_commands(Session &session, unsigned int line, const unsigned int begin, unsigned int end);