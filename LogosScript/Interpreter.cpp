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
	else if (ch == ' ')
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

				if (word == "\"")
					do { word += page_object.all_lines[i][++j]; } while (page_object.all_lines[i][j] != '"');
			}
			else
			{				
				if (word == ")" && page_object.all_lines[i][j] == ')' || word == "(" && page_object.all_lines[i][j] == '(')
				{
					session.lines[line_counter].instructions.push_back(Instruction{ word });
					session.lines[line_counter].instructions.push_back(Instruction{ word });
					word = EMPTY;
				}
				else 
					word += page_object.all_lines[i][j];
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
					session.lines[i].instructions[j].body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), session.lines[i].instructions[j].body) != KEY_WORDS.end()))
				{
					session.lines[i].instructions[j].isVariable = true;

					if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
					{
						session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
					}
					else session.lines[i].instructions[j].data = session.all_data.find(session.lines[i].instructions[j].body)->second.data;
				}
				else
				{
					session.lines[i].instructions[j].isVariable = false;
					session.lines[i].instructions[j].data = session.lines[i].instructions[j].body;
				}
			}
		}
		do_line_script_with_operators(session, i, 0, session.lines[i].instructions.size() - 1);
	}
	std::cout << "Переменные: " << std::endl;
	for (auto i = session.all_data.begin(); i != session.all_data.end(); i++)
		std::cout << i->second.body << " = " << i->second.data << std::endl;
}

void do_line_script_with_operators(Session& session, const unsigned int line, const unsigned int begin, unsigned int end)
// Выполнение часть инструкций в строке если там есть оператор
{
	std::cout << "Операторы: " << std::endl;
	// Операторы первого уровня [(, ), ++, --]
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];
			if (temp.body == ")")
			{
				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				unsigned int counter = 1;

				for (register int j = i - 1; j >= 0; j--)
				{
					if (session.lines[line].instructions[j].body == ")") counter++;
					else if (session.lines[line].instructions[j].body == "(")
					{
						if (--counter == 0)
						{
							session.lines[line].instructions.erase(session.lines[line].instructions.begin() + j);
							do_line_script_with_operators(session, line, j, i - 2);
							end -= (i - j);
							i -= (i - j);
							break;
						}
					}
				}
			}
			else if (temp.body == "++")
			{
				if (i < end && session.lines[line].instructions[i + 1].isVariable && session.lines[line].instructions[i + 1].type_of_instruction 
					== TYPE_OF_INSTRUCTION::DATA)
				{
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = std::to_string(atoi(session.lines[line].instructions[i + 1].data.c_str()) + 1);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = std::to_string(atof(session.lines[line].instructions[i + 1].data.c_str()) + 1);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i + 1].data = "true";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data += "1";
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i + 1].data = "1";
							break;
						}
					}
					session.all_data.find(session.lines[line].instructions[i + 1].body)->second.data = session.lines[line].instructions[i + 1].data;

					for (register int z = end; z >= begin; z--)
					{
						if (z < 0) break; // <------ Костыль

						if (session.lines[line].instructions[i + 1].body == session.lines[line].instructions[z].body)
							session.lines[line].instructions[z].data = session.lines[line].instructions[i + 1].data;
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
				}
				else if (i > begin && session.lines[line].instructions[i - 1].isVariable && session.lines[line].instructions[i - 1].type_of_instruction
					== TYPE_OF_INSTRUCTION::DATA)
				{
					switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
					{
					case TYPE_OF_DATA::_INT:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) + 1);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = "true";
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data += "1";
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = "1";
						break;
					}
					}
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
				}
			}
			else if (temp.body == "--")
			{
				if (i < end && session.lines[line].instructions[i + 1].isVariable)
				{	
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = std::to_string(atoi(session.lines[line].instructions[i + 1].data.c_str()) - 1);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = std::to_string(atof(session.lines[line].instructions[i + 1].data.c_str()) - 1);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i + 1].data = "false";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data += "0";
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i + 1].data = "null";
							break;
						}
					}

					session.all_data.find(session.lines[line].instructions[i + 1].body)->second.data = session.lines[line].instructions[i + 1].data;

					for (register int z = end; z >= begin; z--)
					{
						if (z < 0) break; // <------ Костыль

						if (session.lines[line].instructions[i + 1].body == session.lines[line].instructions[z].body)
							session.lines[line].instructions[z].data = session.lines[line].instructions[i + 1].data;
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
				}
				else if (i > begin && session.lines[line].instructions[i - 1].isVariable)
				{
					switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) - 1);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) - 1);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.lines[line].instructions[i - 1].data = "false";
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data += "0";
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						break;
					}
					}
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
				}
			}
		}
	}
	// Операторы второго уровня [*, /]
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];
			
			if (temp.body == "*")
			{
				if (i > 0)
				{
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
						case TYPE_OF_DATA::_INT:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
								case TYPE_OF_DATA::_INT:
								{
									session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
										* atoi(session.lines[line].instructions[i + 1].data.c_str()));
									break;
								}
								case TYPE_OF_DATA::_DOUBLE:
								{
									session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
										* atof(session.lines[line].instructions[i + 1].data.c_str()));
									break;
								}
								case TYPE_OF_DATA::_BOOLEAN:
								{
									session.lines[line].instructions[i - 1].data = "true";
									break;
								}
								case TYPE_OF_DATA::_STRING:
								{
									std::string temp = session.lines[line].instructions[i - 1].data;
									for (register unsigned int m = 0; m < atoi(session.lines[line].instructions[i + 1].data.c_str()) - 1; m++)
										session.lines[line].instructions[i - 1].data += temp;
									break;
								}
								case TYPE_OF_DATA::_NONE:
								{
									session.lines[line].instructions[i - 1].data = "null";
									break;
								}
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
								case TYPE_OF_DATA::_INT:
								{
									session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
										* atof(session.lines[line].instructions[i + 1].data.c_str()));
									break;
								}
								case TYPE_OF_DATA::_DOUBLE:
								{
									session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
										* atof(session.lines[line].instructions[i + 1].data.c_str()));
									break;
								}
								case TYPE_OF_DATA::_BOOLEAN:
								{
									session.lines[line].instructions[i - 1].data = "true";
									break;
								}
								case TYPE_OF_DATA::_STRING:
								{
									for (register unsigned int i = 0; i < atoi(session.lines[line].instructions[i + 1].data.c_str()); i++)
										session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_NONE:
								{
									session.lines[line].instructions[i - 1].data = "null";
									break;
								}
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
								case TYPE_OF_DATA::_INT:
								{
									if (session.lines[line].instructions[i + 1].data == "false")
										session.lines[line].instructions[i - 1].data = "0";
									break;
								}
								case TYPE_OF_DATA::_DOUBLE:
								{
									if (session.lines[line].instructions[i + 1].data == "false")
										session.lines[line].instructions[i - 1].data = "0.0";
									break;
								}
								case TYPE_OF_DATA::_BOOLEAN:
								{
									session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_STRING:
								{
									if (session.lines[line].instructions[i + 1].data == "false")
										session.lines[line].instructions[i - 1].data = "";
									break;
								}
								case TYPE_OF_DATA::_NONE:
								{
									session.lines[line].instructions[i - 1].data = "null";
									break;
								}
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
								case TYPE_OF_DATA::_INT:
								{
									session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_DOUBLE:
								{
									session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_BOOLEAN:
								{
									session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_STRING:
								{
									session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
								case TYPE_OF_DATA::_NONE:
								{
									session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
									break;
								}
							}
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "/")
			{
				if (i > 0)
				{
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
							case TYPE_OF_DATA::_INT:
							{
								session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
									/ atoi(session.lines[line].instructions[i + 1].data.c_str()));
								break;
							}
							case TYPE_OF_DATA::_DOUBLE:
							{
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
									/ atof(session.lines[line].instructions[i + 1].data.c_str()));
								break;
							}
							case TYPE_OF_DATA::_BOOLEAN:
							{
								session.lines[line].instructions[i - 1].data = "false";
								break;
							}
							case TYPE_OF_DATA::_STRING:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_NONE:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
							case TYPE_OF_DATA::_INT:
							{
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
									/ atof(session.lines[line].instructions[i + 1].data.c_str()));
								break;
							}
							case TYPE_OF_DATA::_DOUBLE:
							{
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
									/ atof(session.lines[line].instructions[i + 1].data.c_str()));
								break;
							}
							case TYPE_OF_DATA::_BOOLEAN:
							{
								session.lines[line].instructions[i - 1].data = "false";
								break;
							}
							case TYPE_OF_DATA::_STRING:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_NONE:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
							case TYPE_OF_DATA::_INT:
							{
								if (session.lines[line].instructions[i + 1].data == "false")
									session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_DOUBLE:
							{
								if (session.lines[line].instructions[i + 1].data == "false")
									session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_BOOLEAN:
							{
								session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
								break;
							}
							case TYPE_OF_DATA::_STRING:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_NONE:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
							{
							case TYPE_OF_DATA::_INT:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_DOUBLE:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_BOOLEAN:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_STRING:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							case TYPE_OF_DATA::_NONE:
							{
								session.lines[line].instructions[i - 1].data = "null";
								break;
							}
							}
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else continue;
		}
	}
	// Операторы третьего уровня [+, -, =]
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];


			if (temp.body == "+")
			{
				if (i > 0)
				{
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
								+ atoi(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if(session.lines[line].instructions[i - 1].data == "true")
								session.lines[line].instructions[i - 1].data = atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(std::stof(session.lines[line].instructions[i - 1].data)
								+ std::stof(session.lines[line].instructions[i + 1].data));
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i - 1].data == "true")
								session.lines[line].instructions[i - 1].data = atof(session.lines[line].instructions[i - 1].data.c_str()) + 1;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) + 1);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = "true";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "-")
			{
				if (i > 0)
				{
					switch (get_type_of_data(session.lines[line].instructions[i + 1].data))
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
								- atoi(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								- atof(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if(atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
								session.lines[line].instructions[i - 1].data = "false";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
								atoi(session.lines[line].instructions[i - 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								- atof(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								- atof(session.lines[line].instructions[i + 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
								session.lines[line].instructions[i - 1].data = "false";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
								atoi(session.lines[line].instructions[i - 1].data.c_str()));
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) - 1);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) - 1);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = "true";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (get_type_of_data(session.lines[line].instructions[i - 1].data))
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "=")
			{
				if (i > 0)
				{
					session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
					session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = session.lines[line].instructions[i - 1].data;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else continue;
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
			body[0] == '"'))
			data = "null";
		else data = body;
	}
}