#pragma once

// ������ ����������
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";", ",", "{", "}", "[", "]"};
// ������ ������
const std::vector<std::string> COMMANDS = { "if", "elif", "else", "while", "for"};
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
	// ��������� �� ���������� �� ������ ���� ���������� (��� �������)
	Instruction *ptr;

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

class Session
{
private:

public:
	std::vector<LineInstructions> lines;

	// ������������� ������ ���� ������ �������
	std::map<std::string, Instruction> all_data;

	// ��������� ���������� �������
	std::string last_command;
	bool last_command_success;
	//

	// ��������� ������� ���������� ������������ �������
	unsigned int start_level;
};