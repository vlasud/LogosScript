#pragma once

// Массив операторов
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// Массив команд
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for", "fun", "return", "global", "continue", "break", "const", "static"};
// Массив ключевых слов
const std::vector<std::string> KEY_WORDS = { "true", "false", "null" };

// Перечислентие типов инструкции
enum TYPE_OF_INSTRUCTION { DATA, OPERATOR, COMMAND, TAB};
// Перечислентие типов инструкции для парсера
enum TYPE_OF_INSTRUCTION_FOR_PARSER {_WORD, _OPERATOR, SPACE};
// Перечисление типов данных
enum TYPE_OF_DATA {_INT, _STRING, _DOUBLE, _BOOLEAN, _NONE};
//
//struct MySQL
//{
//	 //Подключение
//	MYSQL *connection;
//	 //Инициализация
//	MYSQL mysql_init;	
//};


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
	// Какой массив используется? (true - array; false - array_map)
	bool why_array_is_used;
	// Указатель на массив из списка всех переменных (для массива)
	Instruction *ptr;
	// Выбранный символ в строке (Для обращения к строке как к массиву)
	int selected_char;
	// Если перед данными был указан модификатор global
	bool isUsedHasGlobal = false;
	// Если это константа
	bool isConst = false;
	// Если статическая переменная
	bool isStatic = false;

	Instruction() {}
	Instruction(const std::string body);

	// Перегрузка операторов сравнения объектов
	const bool operator == (const Instruction &i) const
	{
		if (i.isVariable)
			return this->data == i.data && this->ptr == i.ptr && this->selected_char == i.selected_char
			&& this->array == i.array && this->array_map == i.array_map;
		else
			return this->data == i.data;
	}
	const bool operator != (const Instruction &i) const
	{
		if (i.isVariable)
			return !(this->data == i.data && this->ptr == i.ptr && this->selected_char == i.selected_char
				&& this->array == i.array && this->array_map == i.array_map);
		else
			return !(this->data == i.data);
	}
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

// Определение функции
struct FunctionDefinition
{
	// Начало функции
	int begin;
	// Конец функции
	int end;
	// Параметры функции
	std::vector<Instruction> parametrs;
	// Результат (что вернула функция)
	Instruction result;
	// Если была вызвана инструкция return
	bool isReturn;

	FunctionDefinition()
	{
		this->result.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
		this->result.type_of_data = TYPE_OF_DATA::_NONE;
		this->result.data = "null";

		this->isReturn = false;
	}
};


