
// Массив операторов
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// Массив команд
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for", "fun", "return", "global", "continue", "break", "const"};
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

class Session
{
private:
	u_int start_line;
	u_int current_line;
public:
	std::vector<LineInstructions> lines;
	// Хэш обьявленных функций
	std::map<std::string, FunctionDefinition> definition_functions;

	// Ассоциативный массив всех данных скрипта
	std::map<std::string, Instruction> all_data;
	// Ассоциативный массив всех данных скрипта (буффер)
	std::map<std::string, Instruction> all_data_buffer;

	// Последняя выполненая команда
	std::string last_command;
	bool last_command_success;
	//

	// Была ли вызвана команда continue
	bool isContinue = false;
	// Была ли вызвана команда break
	bool isBreak = false;

	ErrorCore *error = nullptr;
	Output output;

	// Начальный уровень локального пространства скрипта
	unsigned int start_level;

	// Текущая выполняемая функция
	FunctionDefinition *current_function = nullptr;

	Session(u_int start_line);
	~Session();

	u_int get_start_line(void);
	u_int get_current_line(void);
	void update_current_line(u_int new_line);
};