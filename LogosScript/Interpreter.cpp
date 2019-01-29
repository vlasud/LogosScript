#include "Global.h"

void interpreter_start(const SOCKET client_socket, const int file_id)
// client_socket - сокет клиента
// file_id - смещение вектора файлов сайта
{
	Page page_object = all_pages[file_id];
	Session session;

	unsigned int count_lines_of_file = all_pages[file_id].all_lines.size();

	for (unsigned register int i = 0; i < count_lines_of_file; i++)
	{
		if (page_object.all_lines[i][0] == '<' && page_object.all_lines[i][1] == '#')
		{
			for (unsigned register int j = i; j < count_lines_of_file; j++)
				if (page_object.all_lines[j][0] == '#' && page_object.all_lines[j][1] == '>')
				{
					read_script(session, page_object, i + 1, j + 1);
					count_lines_of_file = page_object.all_lines.size();
					break;
				}
		}
	}

	std::string body = EMPTY;

	unsigned int count_line_of_page = page_object.all_lines.size();
	for (unsigned register int i = 0; i < count_line_of_page; i++)
		body += page_object.all_lines[i];

	std::string header = "HTTP / 1.1 200 OK\r\nVersion: HTTP/1.1\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n";
	std::string response = header + body;

	send(client_socket, response.c_str(), response.length(), 0);
	closesocket(client_socket);
}

TYPE_OF_INSTRUCTION_FOR_PARSER get_type_of_instruction(char ch)
// ch - символ
{
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' ||
		ch >= '0' && ch <= '9' || ch == '_')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::_WORD;
	else if (ch == ' ')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;
	else return TYPE_OF_INSTRUCTION_FOR_PARSER::_OPERATOR;
}

void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end)
// client_socket - сокет клиента
// start - начало строки
// end - конец строки
{
	unsigned int all_lines_length = 0;
	std::string word = EMPTY;
	unsigned int line_counter = 0;
	TYPE_OF_INSTRUCTION_FOR_PARSER last_type_of_instruction = TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;

	// Определение команд
	for (unsigned register int i = start; i < end - 1; i++)
	{
		all_lines_length = page_object.all_lines[i].length();
		session.lines.push_back(LineInstructions { } );

		for (unsigned register int j = 0; j < all_lines_length; j++)
		{
			if (last_type_of_instruction != get_type_of_instruction(page_object.all_lines[i][j]))
			{
				if (word != EMPTY)
				{
					if(last_type_of_instruction != TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE)
						session.lines[line_counter].instructions.push_back(Instruction{ word });
					word = EMPTY;
				}

				last_type_of_instruction = get_type_of_instruction(page_object.all_lines[i][j]);
				word += page_object.all_lines[i][j];
			}
			else word += page_object.all_lines[i][j];
		}
		
		if (word != EMPTY && last_type_of_instruction != TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE)
		{
			session.lines[line_counter].instructions.push_back(Instruction{ word });
			word = EMPTY;
		}
		last_type_of_instruction = TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;
		line_counter++;
	}

	// Вывод в консоль для отладки
	for (int i = 0; i < session.lines.size(); i++)
	{
		for (int j = 0; j < session.lines[i].instructions.size(); j++)
		{
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::COMMAND) std::cout << "Команда ";
			else if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR) std::cout << "Оператор ";
			else if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA) std::cout << "Данные ";
			if(session.lines[i].instructions[j].body == "\t") std::cout << "табуляция" << std::endl;
			else if (session.lines[i].instructions[j].body == " ") std::cout << "пусто" << std::endl;
			else std::cout << session.lines[i].instructions[j].body << std::endl;
		}
	}
	std::cout << std::endl;
	//

	do_script(session);
}


void do_script(Session &session)
// Выполнение скрипта
{
	// Размер массива строк
	unsigned int session_lines_size = session.lines.size();

	for (register unsigned int i = 0; i < session_lines_size; i++)
	{
		for (register int j = session.lines[i].instructions.size() - 1; j >= 0; j--)
		{
			// Если инструкция - данные
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
			{
				if (!(session.lines[i].instructions[j].body[0] >= '0' && session.lines[i].instructions[j].body[0] <= '9' ||
					session.lines[i].instructions[j].body[0] == '"'))
				{
					if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
					{
						session.lines[i].instructions[j].isVariable = true;
						session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
					}
				}
			}
			// Если инструкция - оператор
			else if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
			{
				do_line_script_with_operators(session, i, 0, session.lines[i].instructions.size() - 1);
			}
		}
	}
	std::cout << "Переменные: " << std::endl;
	for (auto i = session.all_data.begin(); i != session.all_data.end(); i++)
		std::cout << i->second.body << std::endl;
}

void do_line_script_with_operators(Session& session, const unsigned int line, const unsigned int begin, const unsigned int end)
// Выполнение часть инструкций в строке если там есть оператор
{
	std::cout << "Операторы: " << std::endl;
	for (register int i = end; i >= 0; i--)
	{
		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

		}
	}
}

Instruction::Instruction(const std::string body)
{
	this->body = body;


	if (std::find(OPERATORS.begin(), OPERATORS.end(), body) != OPERATORS.end())
		this->type_of_instruction = TYPE_OF_INSTRUCTION::OPERATOR;
	else if (std::find(COMMANDS.begin(), COMMANDS.end(), body) != COMMANDS.end())
		this->type_of_instruction = TYPE_OF_INSTRUCTION::COMMAND;
	else 
		this->type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
}