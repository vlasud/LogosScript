#pragma once

// ������ ����������
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// ������ ������
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for", "fun", "return", "global", "continue", "break", "const", "static"};
// ������ �������� ����
const std::vector<std::string> KEY_WORDS = { "true", "false", "null" };

// ������������� ����� ����������
enum TYPE_OF_INSTRUCTION { DATA, OPERATOR, COMMAND, TAB};
// ������������� ����� ���������� ��� �������
enum TYPE_OF_INSTRUCTION_FOR_PARSER {_WORD, _OPERATOR, SPACE};
// ������������ ����� ������
enum TYPE_OF_DATA {_INT, _STRING, _DOUBLE, _BOOLEAN, _NONE};
//
//struct MySQL
//{
//	 //�����������
//	MYSQL *connection;
//	 //�������������
//	MYSQL mysql_init;	
//};


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
	// ���� ����������� ����������
	bool isStatic = false;

	Instruction() {}
	Instruction(const std::string body);

	// ���������� ���������� ��������� ��������
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


