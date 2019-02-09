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

	std::cout << "Variables: " << std::endl;
	for (auto i = session.all_data.begin(); i != session.all_data.end(); i++)
		std::cout << i->second.body << " = " << i->second.data << std::endl;
}

std::string format_data(std::string data)
// data - данные
// ~ Переводит данные в правильный формат
{
	if (data[0] == '"')
	{
		if(data[data.length() - 1] != '"')
			data += "\"";
		return data;
	}
	
	// Перевод в нижний регистр
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);

	int counter = 0;
	bool isDoted = false;
	while (data[counter] >= '0' && data[counter] <= '9' || data[counter] == '.')
	{
		if (data[counter] == '.')
		{
			if (isDoted) break;
			else isDoted = true;
		}
		counter++;
	}

	if (data.length() > 0 && counter == 0 && 
		std::find(KEY_WORDS.begin(), KEY_WORDS.end(), data) == KEY_WORDS.end()) data = "null";

	return data;
}

TYPE_OF_INSTRUCTION_FOR_PARSER get_type_of_instruction(char ch)
// ch - символ
// ~ Определяет тип инструкции
{
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' ||
		ch >= '0' && ch <= '9' || ch == '_' || ch == '"' || ch == '.')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::_WORD;
	else if (ch == '\t' || ch == ' ')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;
	else return TYPE_OF_INSTRUCTION_FOR_PARSER::_OPERATOR;
}

TYPE_OF_DATA get_type_of_data(std::string data)
// data - данные
// ~ Определяет тип данных
{
	// Приведение данных к правильному виду
	data = format_data(data);

	if (data[0] == '"') return TYPE_OF_DATA::_STRING;
	else if (data == "true" || data == "false") return TYPE_OF_DATA::_BOOLEAN;
	else if (data == "null") return TYPE_OF_DATA::_NONE;
	else if (data.find('.') != std::string::npos) return TYPE_OF_DATA::_DOUBLE;
	else return TYPE_OF_DATA::_INT;
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

		// Подсчет уровня локального пространства
		register int space_counter = -1;
		unsigned register int j = 0;

		while (page_object.all_lines[i][++space_counter] == '\t')
		{
			session.lines[line_counter].namespace_level++;
			j++;
		}
		//

		for (; j < all_lines_length; j++)
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

				if (word == "\"")
					do { word += page_object.all_lines[i][++j]; } while (page_object.all_lines[i][j] != '"');
			}
			else
			{				
				word += page_object.all_lines[i][j];

				if (last_type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
				{
					if (std::find(OPERATORS.begin(), OPERATORS.end(), word) != OPERATORS.end())
					{
						session.lines[line_counter].instructions.push_back(Instruction{ word });
						word = EMPTY;
					}
					else
					{
						for (register unsigned int z = 0; z < 2; z++)
						{
							if (std::find(OPERATORS.begin(), OPERATORS.end(), std::string{ word[z] }) != OPERATORS.end())
								session.lines[line_counter].instructions.push_back(Instruction{ std::string { word[z]} });
						}
						word = EMPTY;
					}
				}
			}
		}
		
		if (word != EMPTY && last_type_of_instruction != TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE)
		{
			session.lines[line_counter].instructions.push_back(Instruction{ word });
			word = EMPTY;
		}
		last_type_of_instruction = TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;
		line_counter++;
	}

	do_script(session);
}

void do_script(Session &session)
// Выполнение скрипта
{
	// Размер массива строк
	unsigned int session_lines_size = session.lines.size();

	session.start_level = session.lines[0].namespace_level;
	for (register unsigned int i = 0; i < session_lines_size; i++)
	{
		if (session.lines[i].namespace_level != session.start_level) continue;

		for (register int j = session.lines[i].instructions.size() - 1; j >= 0; j--)
		{
			// Если инструкция - данные
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
			{
				if (!(session.lines[i].instructions[j].body[0] >= '0' && session.lines[i].instructions[j].body[0] <= '9' ||
					session.lines[i].instructions[j].body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), session.lines[i].instructions[j].body) != KEY_WORDS.end()))
				{
					if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
					{
						session.lines[i].instructions[j].isVariable = true;
						session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
					}
					else session.lines[i].instructions[j] = session.all_data.find(session.lines[i].instructions[j].body)->second;
				}
				else
				{
					session.lines[i].instructions[j].isVariable = false;
					//session.lines[i].instructions[j].data = session.lines[i].instructions[j].body;
				}
			}
		}

		if (session.lines[i].instructions.size() > 0)
		{
			// Сохранение инструкций перед их выполнением
			if (session.lines[i].copy_instructions.size() == 0) session.lines[i].copy_instructions = session.lines[i].instructions;

			do_line_script_operators(session, i, 0, session.lines[i].instructions.size() - 1);
			do_line_script_commands(session, i, 0, session.lines[i].instructions.size() - 1);
		}
	}
}

void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData)
// Выполнение скрипта
{
	unsigned int start_level = session.lines[begin].namespace_level;
	for (register unsigned int i = begin; i <= end; i++)
	{
		if (session.lines[i].namespace_level != start_level) continue;

		for (register int j = session.lines[i].instructions.size() - 1; j >= 0; j--)
		{
			// Если инструкция - данные
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
			{
				if (!(session.lines[i].instructions[j].body[0] >= '0' && session.lines[i].instructions[j].body[0] <= '9' ||
					session.lines[i].instructions[j].body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), session.lines[i].instructions[j].body) != KEY_WORDS.end()))
				{
					if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
					{
						session.lines[i].instructions[j].isVariable = true;
						session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
					}
					else session.lines[i].instructions[j] = session.all_data.find(session.lines[i].instructions[j].body)->second;
				}
				else
				{
					session.lines[i].instructions[j].isVariable = false;
					//session.lines[i].instructions[j].data = session.lines[i].instructions[j].body;
				}
			}
		}
		if (session.lines[i].instructions.size() > 0)
		{
			// Сохранение инструкций перед их выполнением
			if(session.lines[i].copy_instructions.size() == 0) session.lines[i].copy_instructions = session.lines[i].instructions;

			do_line_script_operators(session, i, 0, session.lines[i].instructions.size() - 1);

			if (!isOnlyData && do_line_script_commands(session, i, 0, session.lines[i].instructions.size() - 1));
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

	if (type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
	{
		if (!(body[0] >= '0' && body[0] <= '9' ||
			body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), body) != KEY_WORDS.end()))
		{
			this->data = "null";
			this->type_of_data = TYPE_OF_DATA::_NONE;
		}
		else
		{
			this->data = body;
			type_of_data = get_type_of_data(this->data);

			if (this->type_of_data == TYPE_OF_DATA::_STRING)
			{
				data = EMPTY;
				for(register unsigned int i = 1; i < body.length() - 1; i++)
					this->data += body[i];
			}
		}
	}
}