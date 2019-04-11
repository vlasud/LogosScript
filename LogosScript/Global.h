#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <thread>
#include <ctime>
#include <WS2tcpip.h>
#include <WinSock2.h>
//#include <mysql.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Page.h"
#include "Output.h"
#include "ErrorCore.h"
#include "Interpreter.h"
#include "FunctionsCore.h"

#define EMPTY ""


// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;
// Статические данные
extern std::map<std::string, Instruction> static_data;
// Пользовательские сессии
extern std::map<std::string, Session> all_user_sessions;


void interpreter_start(const SOCKET client_socket, const int file_id, const std::string request, Session* _session = nullptr);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session);
void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData = false, FunctionDefinition *func = nullptr, bool isCommand = false);
void do_line_script_operators(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
int parse_array_brackets(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
void write_data_from_local_to_global(Session &session, Instruction &first, Instruction &second);
bool do_line_script_commands(Session &session, unsigned int line, const unsigned int begin, unsigned int end);
