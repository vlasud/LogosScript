#pragma once

// Массив операторов
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!="};
// Массив команд
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "for", "while"};
// Массив ключевых слов
const std::vector<std::string> KEY_WORDS = { "true", "false", "null" };

// Перечислентие типов инструкции
enum TYPE_OF_INSTRUCTION { DATA, OPERATOR, COMMAND, TAB};
// Перечислентие типов инструкции для парсера
enum TYPE_OF_INSTRUCTION_FOR_PARSER {_WORD, _OPERATOR, SPACE};
// Перечисление типов данных
enum TYPE_OF_DATA {_INT, _STRING, _DOUBLE, _BOOLEAN, _NONE};

class Instruction
{
private:

public:
	// Тип инструкции
	TYPE_OF_INSTRUCTION type_of_instruction;
	// Тип данных
	TYPE_OF_DATA type_of_data;
	// Имя инструкции
	std::string body;
	// Значение
	std::string data;
	// Статика данных
	bool isVariable;

	Instruction() {}
	Instruction(const std::string body);
};


class Data : public Instruction
{
private:
public:
};



class Tab : Instruction
{
private:
public:
};

class LineInstructions
{
private:

public:
	std::vector<Instruction> instructions;
	// Уровень локального пространства
	int namespace_level;
};

class Session
{
private:

public:
	std::vector<LineInstructions> lines;

	// Ассоциативный массив всех данных скрипта
	std::map<std::string, Instruction> all_data;

	// Последняя выполненая команда
	std::string last_command;
	bool last_command_success;
	//
};


void interpreter_start(const SOCKET client_socket, const int file_id);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session);
void do_script(Session &session, const unsigned int begin, unsigned int end);
void do_line_script_operators(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
bool do_line_script_commands(Session &session, unsigned int line, const unsigned int begin, unsigned int end);