#pragma once

// Массив операторов
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// Массив команд
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for"};
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
	// Вектор инструкицй (если это массив)
	std::vector<Instruction> array;
	// Мап инструкицй (если это массив)
	std::map<std::string, Instruction> array_map;
	// Указатель на инструкцию из списка всех переменных (для массива)
	Instruction *ptr;

	Instruction() {}
	Instruction(const std::string body);
};

class LineInstructions
{
private:

public:
	// Вектор инструкций
	std::vector<Instruction> instructions;
	// Копия вектора инструкций
	std::vector<Instruction> copy_instructions;
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

	// Начальный уровень локального пространства скрипта
	unsigned int start_level;
};