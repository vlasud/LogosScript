
// ������ ����������
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// ������ ������
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for", "fun", "return", "global", "continue", "break", "const"};
// ������ �������� ����
const std::vector<std::string> KEY_WORDS = { "true", "false", "null" };

// ������������� ����� ����������
enum TYPE_OF_INSTRUCTION { DATA, OPERATOR, COMMAND, TAB};
// ������������� ����� ���������� ��� �������
enum TYPE_OF_INSTRUCTION_FOR_PARSER {_WORD, _OPERATOR, SPACE};
// ������������ ����� ������
enum TYPE_OF_DATA {_INT, _STRING, _DOUBLE, _BOOLEAN, _NONE};

class Instruction
{
private:

public:
	// ��� ����������
	TYPE_OF_INSTRUCTION type_of_instruction;
	// ��� ������
	TYPE_OF_DATA type_of_data;
	// ��� ����������
	std::string body;
	// ��������
	std::string data;
	// ������� ������
	bool isVariable;
	// ������ ���������� (���� ��� ������)
	std::vector<Instruction> array;
	// ��� ���������� (���� ��� ������)
	std::map<std::string, Instruction> array_map;
	// ����� ������ ������������? (true - array; false - array_map)
	bool why_array_is_used;
	// ��������� �� ������ �� ������ ���� ���������� (��� �������)
	Instruction *ptr;
	// ��������� ������ � ������ (��� ��������� � ������ ��� � �������)
	int selected_char;
	// ���� ����� ������� ��� ������ ����������� global
	bool isUsedHasGlobal = false;
	// ���� ��� ���������
	bool isConst = false;

	Instruction() {}
	Instruction(const std::string body);
};

class LineInstructions
{
private:

public:
	// ������ ����������
	std::vector<Instruction> instructions;
	// ����� ������� ����������
	std::vector<Instruction> copy_instructions;
	// ������� ���������� ������������
	int namespace_level;
};

// ����������� �������
struct FunctionDefinition
{
	// ������ �������
	int begin;
	// ����� �������
	int end;
	// ��������� �������
	std::vector<Instruction> parametrs;
	// ��������� (��� ������� �������)
	Instruction result;
	// ���� ���� ������� ���������� return
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
	// ��� ����������� �������
	std::map<std::string, FunctionDefinition> definition_functions;

	// ������������� ������ ���� ������ �������
	std::map<std::string, Instruction> all_data;
	// ������������� ������ ���� ������ ������� (������)
	std::map<std::string, Instruction> all_data_buffer;

	// ��������� ���������� �������
	std::string last_command;
	bool last_command_success;
	//

	// ���� �� ������� ������� continue
	bool isContinue = false;
	// ���� �� ������� ������� break
	bool isBreak = false;

	ErrorCore *error = nullptr;
	Output output;

	// ��������� ������� ���������� ������������ �������
	unsigned int start_level;

	// ������� ����������� �������
	FunctionDefinition *current_function = nullptr;

	Session(u_int start_line);
	~Session();

	u_int get_start_line(void);
	u_int get_current_line(void);
	void update_current_line(u_int new_line);
};