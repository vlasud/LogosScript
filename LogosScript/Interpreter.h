#pragma once

// ������ ����������
const std::vector<std::string> OPERATORS = { "==", "(", ")", "+", "-", "/", "*", "=", "\t"};
// ������ ������
const std::vector<std::string> COMMANDS = { "if", "for", "while"};

// ������������� ����� ����������
enum TYPE_OF_INSTRUCTION { DATA, OPERATOR, COMMAND, TAB};
// ������������� ����� ���������� ��� �������
enum TYPE_OF_INSTRUCTION_FOR_PARSER {_WORD, _OPERATOR, SPACE};

class Instruction
{
private:

public:
	// ��� ����������
	TYPE_OF_INSTRUCTION type_of_instruction;
	// ��� ����������
	std::string body;
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
	std::vector<Instruction> instructions;
};

class Session
{
private:

public:
	std::vector<LineInstructions> lines;

	// ������������� ������ ���� ������ �������
	std::map<std::string, Instruction> all_data;
};


void interpreter_start(const SOCKET client_socket, const int file_id);
void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end);
void do_script(Session &session);
void do_line_script_with_operators(Session &session, const unsigned int line, const unsigned int begin, const unsigned int end);