#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <experimental/filesystem>
#include <vector>
#include <stdio.h>
#include <map>
#include <iostream>
#include <string>
#include <thread>
#include <ctime>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <direct.h>
#include "mysql/mysql.h"
#include <fstream>
#include <algorithm>
#include "Page.h"
#include "Output.h"
#include "ErrorCore.h"
#include "Interpreter.h"
#include "FunctionsCore.h"
#include "Session.h"
#include "Server.h"

#define EMPTY ""

namespace fs = std::experimental::filesystem;

const std::string LOGOS_VERSION = "2.2";

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;
// Статические данные
extern std::map<std::string, Instruction> static_data;
// Пользовательские сессии
extern std::map<std::string, Session> all_user_sessions;


std::string interpreter_start(const SOCKET client_socket, Page &page_object, Server *server, Session* _session = nullptr);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData = false, FunctionDefinition *func = nullptr, bool isCommand = false);
void do_line_script_operators(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
int parse_array_brackets(Session &session, const unsigned int line, const unsigned int begin, unsigned int &end);
void write_data_from_local_to_global(Session &session, Instruction &first, Instruction &second);
bool do_line_script_commands(Session &session, unsigned int line, const unsigned int begin, unsigned int end);
TYPE_OF_INSTRUCTION_FOR_PARSER get_type_of_instruction(char ch);
TYPE_OF_DATA get_type_of_data(std::string data);