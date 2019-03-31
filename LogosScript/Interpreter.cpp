#include "Global.h"

void interpreter_start(const SOCKET client_socket, const int file_id)
// client_socket - ����� �������
// file_id - �������� ������� ������ �����
{
	Page page_object = all_pages[file_id];
	bool isError = false;

	unsigned int count_lines_of_file = all_pages[file_id].all_lines.size();

	for (unsigned register int i = 0; i < count_lines_of_file; i++)
	{
		if (page_object.all_lines[i][0] == '<' && page_object.all_lines[i][1] == '#')
		{
			for (unsigned register int j = i; j < count_lines_of_file; j++)
				if (page_object.all_lines[j][0] == '#' && page_object.all_lines[j][1] == '>')
				{
					Session session(i + 2);
					read_script(session, page_object, i + 1, j + 1);

					// ���� ���� �������������� ������ - �������� ��������� ������
					if (session.error != nullptr)
					{
						page_object.all_lines.clear();
						page_object.all_lines.push_back(session.error->get_error_text());
						isError = true;
						break;
					}
					// ����� �������� ����������� ������ �� ��������� ������ �������
					else
					{
						page_object.all_lines.erase(page_object.all_lines.begin() + i, page_object.all_lines.begin() + j + 1);

						// ����������� ������
						std::reverse(session.output.output_data.begin(), session.output.output_data.end());

						for (register unsigned int mn = 0; mn < session.output.output_data.size(); mn++)
							page_object.all_lines.insert(page_object.all_lines.begin() + i, session.output.output_data[mn]);
					}

					count_lines_of_file = page_object.all_lines.size();
					break;
				}
			if (isError)
				break;
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
// data - ������
// ~ ��������� ������ � ���������� ������
{
	if (data[0] == '"')
	{
		if(data[data.length() - 1] != '"')
			data += "\"";
		return data;
	}
	
	// ������� � ������ �������
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
// ch - ������
// ~ ���������� ��� ����������
{
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' ||
		ch >= '0' && ch <= '9' || ch == '_' || ch == '"' || ch == '.')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::_WORD;
	else if (ch == '\t' || ch == ' ')
		return TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;
	else return TYPE_OF_INSTRUCTION_FOR_PARSER::_OPERATOR;
}

TYPE_OF_DATA get_type_of_data(std::string data)
// data - ������
// ~ ���������� ��� ������
{
	// ���������� ������ � ����������� ����
	data = format_data(data);

	if (data[0] == '"') return TYPE_OF_DATA::_STRING;
	else if (data == "true" || data == "false") return TYPE_OF_DATA::_BOOLEAN;
	else if (data == "null") return TYPE_OF_DATA::_NONE;
	else if (data.find('.') != std::string::npos) return TYPE_OF_DATA::_DOUBLE;
	else return TYPE_OF_DATA::_INT;
}

void read_script(Session &session, Page &page_object, const unsigned int start, const unsigned int end)
// client_socket - ����� �������
// start - ������ ������
// end - ����� ������
{
	unsigned int all_lines_length = 0;
	std::string word = EMPTY;
	unsigned int line_counter = 0;
	TYPE_OF_INSTRUCTION_FOR_PARSER last_type_of_instruction = TYPE_OF_INSTRUCTION_FOR_PARSER::SPACE;

	// ����������� ������
	for (unsigned register int i = start; i < end - 1; i++)
	{
		all_lines_length = page_object.all_lines[i].length();
		session.lines.push_back(LineInstructions { } );

		// ������� ������ ���������� ������������
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
						if (std::find(OPERATORS.begin(), OPERATORS.end(), std::string{ word[0] }) != OPERATORS.end())
							session.lines[line_counter].instructions.push_back(Instruction{ std::string { word[0]} });

						word = word[1];
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
// ���������� �������
{
	// ������ ������� �����
	unsigned int session_lines_size = session.lines.size();

	// ����������� ���������� ������ ���������� ������������
	for (register unsigned int i = 0; i < session_lines_size; i++)
	{
		if (session.lines[i].instructions.size() > 0)
		{
			session.start_level = session.lines[i].namespace_level;
			break;
		}
	}

	for (register unsigned int i = 0; i < session_lines_size; i++)
	{
		// ���������� ������ ������� ������ � �����
		session.update_current_line(session.get_start_line() + i);

		if (session.lines[i].namespace_level != session.start_level) continue;

		// ���������� ���������� ����� �� �����������
		if (session.lines[i].copy_instructions.size() == 0) session.lines[i].copy_instructions = session.lines[i].instructions;

		for (register int j = session.lines[i].instructions.size() - 1; j >= 0; j--)
		{
			// ���� ���������� - ������
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
			{
				if (!(session.lines[i].instructions[j].body[0] >= '0' && session.lines[i].instructions[j].body[0] <= '9' ||
					session.lines[i].instructions[j].body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), session.lines[i].instructions[j].body) != KEY_WORDS.end()))
				{
					// ���� ��� �������
					if (j < session.lines[i].instructions.size() - 1 && session.lines[i].instructions[j + 1].body == "(")
					{
						if (session.definition_functions.find(session.lines[i].instructions[j].body) != session.definition_functions.end())
						{
							int param_counter = -1;
							FunctionDefinition *tmp = &session.definition_functions.find(session.lines[i].instructions[j].body)->second;

							for (register u_int z = j + 1; z < session.lines[i].instructions.size(); z++)
							{
								if (session.lines[i].instructions[z].body == ")")
								{
									do_line_script_operators(session, i, j + 1, z - 1);
									break;
								}
							}

							// ����������� ����������
							for (register unsigned int z = j + 1; z < session.lines[i].instructions.size() && session.lines[i].instructions[z].body != ")"; z++)
								if (session.lines[i].instructions[z].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
								{
									tmp->parametrs[++param_counter].data = session.lines[i].instructions[z].data;
									tmp->parametrs[param_counter].type_of_data = session.lines[i].instructions[z].type_of_data;
									tmp->parametrs[param_counter].array = session.lines[i].instructions[z].array;
									tmp->parametrs[param_counter].array_map = session.lines[i].instructions[z].array_map;
								}

							do_script(session, tmp->begin, tmp->end, false, tmp);

							session.lines[i].instructions[j] = tmp->result;
							
							// �������� ������ ���������� ����� ������ �������
							for (register unsigned p = j + 1; p < session.lines[i].instructions.size()
								&& session.lines[i].instructions[p].body != ")"; p++)
							{
								session.lines[i].instructions.erase(session.lines[i].instructions.begin() + p);
								p--;
							}
							session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
							continue;
						}
						else if (j > 0 && session.lines[i].instructions[j - 1].body != "fun" || j == 0)
						{
							for (register u_int b = 0; b < system_functions.size(); b++)
							{
								if (system_functions[b].get_name() == session.lines[i].instructions[j].body)
								{
									SystemFunction tmp = system_functions[b];
									byte param_counter = 1;

									for (register u_int z = j + 1; z < session.lines[i].instructions.size(); z++)
									{
										if (session.lines[i].instructions[z].body == ")")
										{
											do_line_script_operators(session, i, j + 2, z - 1);
											break;
										}
									}
									for (register u_int z = j; z < session.lines[i].instructions.size(); z++)
									{
										if (session.lines[i].instructions[z].body == ",") param_counter++;
										else if (session.lines[i].instructions[z].body == ")")
										{
											session.lines[i].instructions.erase(session.lines[i].instructions.begin() + z);
											break;
										}
									}
									while(session.lines[i].instructions.size() > 1 && param_counter > 0)
									{
										if (session.lines[i].instructions[j + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
										{
											tmp.set_params(session.lines[i].instructions[j + 1]);
											session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
											param_counter--;
										}
										else session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
									}

									tmp.start_function(&session);

									session.lines[i].instructions[j] = tmp.get_result();
									break;
								}
								else if (b == system_functions.size() - 1)
								{
									// ������. ����������� �������
									session.error = new ErrorCore("unknow function {" + session.lines[i].instructions[j].body + "} ", &session);
									return;
								}
							}
						}
					}
					// ����� ���� ����������
					else
					{
						if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
						{
							session.lines[i].instructions[j].isVariable = true;

							// ���� ����� ���������� ����� ����������� ���������
							if (j > 0 && session.lines[i].instructions[j - 1].body == "const")
								session.lines[i].instructions[j].isConst = true;

							session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
						}
						else session.lines[i].instructions[j] = session.all_data.find(session.lines[i].instructions[j].body)->second;
					}
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
			do_line_script_operators(session, i, 0, session.lines[i].instructions.size() - 1);
			// ���� ��������� �������������� ������ - ���������� ���������� ���������� ���������
			if (session.error != nullptr) return;
			do_line_script_commands(session, i, 0, session.lines[i].instructions.size() - 1);
			// ���� ��������� �������������� ������ - ���������� ���������� ���������� ���������
			if (session.error != nullptr) return;
		}
	}
}

void do_script(Session &session, const unsigned int begin, unsigned int end, bool isOnlyData, FunctionDefinition *func, bool isCommand)
// ���������� �������
{

	// ���� ���������� ���������� �������, �� ����������� ��� ������ � ������
	// � ������ �������� ������ ��� ��������� ����������
	// ����� ���������� ������� ������� ������ �� ������
	if (func != nullptr)
	{
		session.all_data_buffer = session.all_data;
		session.all_data.clear();
		
		session.current_function = func;

		// �������� ���������� � ������ ��������� ����������
		for (register unsigned int i = 0; i < func->parametrs.size(); i++)
			session.all_data[func->parametrs[i].body] = func->parametrs[i];
	}

	unsigned int start_level = session.lines[begin].namespace_level;
	for (register unsigned int i = begin; i <= end; i++)
	{
		// ���������� ������ ������� ������ � �����
		session.update_current_line(session.get_start_line() + i);

		// ���� ���� ������� ������� ���������� continue
		if (session.isContinue)
		{
			session.isContinue = false;
			return;
		}
		// ���� ���� ������� ������� ���������� break
		if (session.isContinue) return;

		if (session.lines[i].namespace_level != start_level) continue;

		// �������������� �������������� ����������
		if ((func != nullptr || isCommand) && session.lines[i].copy_instructions.size() != 0 
			&& session.lines[i].copy_instructions.size() != session.lines[i].instructions.size())
			session.lines[i].instructions = session.lines[i].copy_instructions;

		// ���������� ���������� ����� �� �����������
		if (session.lines[i].copy_instructions.size() == 0) session.lines[i].copy_instructions = session.lines[i].instructions;

		for (register int j = session.lines[i].instructions.size() - 1; j >= 0; j--)
		{

			// ���� ���������� - ������
			if (session.lines[i].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
			{
				if (!(session.lines[i].instructions[j].body[0] >= '0' && session.lines[i].instructions[j].body[0] <= '9' ||
					session.lines[i].instructions[j].body[0] == '"' || std::find(KEY_WORDS.begin(), KEY_WORDS.end(), session.lines[i].instructions[j].body) != KEY_WORDS.end()))
				{
					// ���� ��� �������
					if (j < session.lines[i].instructions.size() - 1 && session.lines[i].instructions[j + 1].body == "(")
					{
						if (session.definition_functions.find(session.lines[i].instructions[j].body) != session.definition_functions.end())
						{
							int param_counter = -1;
							FunctionDefinition *tmp = &session.definition_functions.find(session.lines[i].instructions[j].body)->second;

							for (register u_int z = j + 1; z < session.lines[i].instructions.size(); z++)
							{
								if (session.lines[i].instructions[z].body == ")")
								{
									do_line_script_operators(session, i, j + 1, z - 1);
									break;
								}
							}
							// ����������� ����������
							for (register unsigned int z = j + 1; z < session.lines[i].instructions.size() && session.lines[i].instructions[z].body != ")"; z++)
								if (session.lines[i].instructions[z].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
								{
									tmp->parametrs[++param_counter].data = session.lines[i].instructions[z].data;
									tmp->parametrs[param_counter].type_of_data = session.lines[i].instructions[z].type_of_data;
									tmp->parametrs[param_counter].array = session.lines[i].instructions[z].array;
									tmp->parametrs[param_counter].array_map = session.lines[i].instructions[z].array_map;
								}

							do_script(session, tmp->begin, tmp->end, false, tmp);

							session.lines[i].instructions[j] = tmp->result;

							// �������� ������ ���������� ����� ������ �������
							for (register unsigned p = j + 1; p < session.lines[i].instructions.size()
								&& session.lines[i].instructions[p].body != ")"; p++)
							{
								session.lines[i].instructions.erase(session.lines[i].instructions.begin() + p);
								p--;
							}
							if(session.lines[i].instructions.size() > j + 1)
								session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
							continue;
						}
						else if (j > 0 && session.lines[i].instructions[j - 1].body != "fun" || j == 0)
						{
							for (register u_int b = 0; b < system_functions.size(); b++)
							{
								if (system_functions[b].get_name() == session.lines[i].instructions[j].body)
								{
									SystemFunction tmp = system_functions[b];

									u_int param_counter = 1;

									for (register u_int z = j + 1; z < session.lines[i].instructions.size(); z++)
									{
										if (session.lines[i].instructions[z].body == ")")
										{
											do_line_script_operators(session, i, j + 2, z - 1);
											break;
										}
									}
									for (register u_int z = j; z < session.lines[i].instructions.size(); z++)
									{
										if (session.lines[i].instructions[z].body == ",") param_counter++;
										else if (session.lines[i].instructions[z].body == ")")
										{
											session.lines[i].instructions.erase(session.lines[i].instructions.begin() + z);
											break;
										}
									}
									while (session.lines[i].instructions.size() > 1 && param_counter > 0)
									{
										if (session.lines[i].instructions[j + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
										{
											tmp.set_params(session.lines[i].instructions[j + 1]);
											session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
											param_counter--;
										}
										else session.lines[i].instructions.erase(session.lines[i].instructions.begin() + j + 1);
									}

									tmp.start_function(&session);

									session.lines[i].instructions[j] = tmp.get_result();
									break;
								}
								else if (b == system_functions.size() - 1)
								{
									// ������. ����������� �������
									session.error = new ErrorCore("unknow function {" + session.lines[i].instructions[j].body + "} ", &session);
									return;
								}
							}
						}
					}
					// ����� ���� ����������
					else if (session.all_data.find(session.lines[i].instructions[j].body) == session.all_data.end())
					{
						// ���� ���������� �� ������� � ��������� ���������, �� ���� �� � ����������
						if (j > 0 && session.lines[i].instructions[j - 1].body == "global" && session.all_data_buffer.find(session.lines[i].instructions[j].body) != session.all_data_buffer.end())
						{
							session.all_data[session.all_data_buffer.find(session.lines[i].instructions[j].body)->second.body] = session.all_data_buffer.find(session.lines[i].instructions[j].body)->second;
							session.all_data[session.all_data_buffer.find(session.lines[i].instructions[j].body)->second.body].isUsedHasGlobal = true;
						}
						// ����� ���� ���������� � ����� ������ ����� �� ����������
						else
						{
							session.lines[i].instructions[j].isVariable = true;

							// ���� ����� ���������� ����� ����������� ���������
							if (j > 0 && session.lines[i].instructions[j - 1].body == "const")
								session.lines[i].instructions[j].isConst = true;

							session.all_data[session.lines[i].instructions[j].body] = session.lines[i].instructions[j];
						}
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
			do_line_script_operators(session, i, 0, session.lines[i].instructions.size() - 1);
			// ���� ��������� �������������� ������ - ���������� ���������� ���������� ���������
			if (session.error != nullptr) return;

			if (!isOnlyData && do_line_script_commands(session, i, 0, session.lines[i].instructions.size() - 1));
			// ���� ��������� �������������� ������ - ���������� ���������� ���������� ���������
			if (session.error != nullptr) return;

			if (func != nullptr && func->isReturn)
			{
				func->isReturn = false;
				break;
			}
		}
	}
	if (func != nullptr)
	{
		// �������� ������ � ������������� global
		for (auto b = session.all_data.begin(); b != session.all_data.end(); b++)
			if (b->second.isUsedHasGlobal)
			{
				b->second.isUsedHasGlobal = false;
				session.all_data_buffer[b->second.body] = b->second;
			}

		session.all_data = session.all_data_buffer;
		session.all_data_buffer.clear();
	}
}

Session::Session(u_int start_line)
{
	this->start_line = start_line;
}

Session::~Session()
{
	delete this->error;
}

u_int Session::get_start_line(void)
{
	return this->start_line;
}
u_int Session::get_current_line(void)
{
	return this->current_line;
}

void Session::update_current_line(u_int new_line)
{
	this->current_line = new_line;
}

Instruction::Instruction(const std::string body)
{
	this->body = body;
	this->ptr = nullptr;
	this->selected_char = -1;

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