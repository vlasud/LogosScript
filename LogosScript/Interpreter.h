#pragma once

// ������ ����������
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "+=", "-=", "*=", "/=", "++", "--", "==", "!=", "<", ">", "<=", ">=", "&&", "||", "!", ";"};
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


void interpreter_start(const SOCKET client_socket, const int file_id);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session);
void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData);
void do_line_script_operators(Session &session, const unsigned int line, const unsigned int begin, unsigned int end);
bool do_line_script_commands(Session &session, unsigned int line, const unsigned int begin, unsigned int end);