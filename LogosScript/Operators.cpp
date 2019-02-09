#include "Global.h"

void do_line_script_operators(Session& session, const unsigned int line, const unsigned int begin, unsigned int end)
// Выполнение часть инструкций в строке. Работа с операторами
{
	// Операторы первого уровня [(, ), ++, --, -]
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];
			if (temp.body == ")")
			{
				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				end--;
				unsigned int counter = 1;

				for (register int j = i - 1; j >= 0; j--)
				{
					if (session.lines[line].instructions[j].body == ")") counter++;
					else if (session.lines[line].instructions[j].body == "(")
					{
						if (--counter == 0)
						{
							session.lines[line].instructions.erase(session.lines[line].instructions.begin() + j);
							do_line_script_operators(session, line, j, i - 2);
							end -= (i - j) - 1;
							i -= (i - j);
							break;
						}
					}
				}
			}
			else if (temp.body == "-" && (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA &&
				session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA))
			{
				switch (session.lines[line].instructions[i + 1].type_of_data)
				{
				case TYPE_OF_DATA::_INT:
				{
					session.lines[line].instructions[i + 1].data = std::to_string(-1 * atoi(session.lines[line].instructions[i + 1].data.c_str()));

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_DOUBLE:
				{
					session.lines[line].instructions[i + 1].data = std::to_string(-1 * atof(session.lines[line].instructions[i + 1].data.c_str()));

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_BOOLEAN:
				{
					session.lines[line].instructions[i + 1].data = "false";

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_STRING:
				{
					session.lines[line].instructions[i + 1].data = "null";
					session.lines[line].instructions[i + 1].type_of_data = TYPE_OF_DATA::_NONE;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_NONE:
				{
					session.lines[line].instructions[i + 1].data = "null";

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				}
			}
			else if (temp.body == "!" && (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA &&
				session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA))
			{
				switch (session.lines[line].instructions[i + 1].type_of_data)
				{
				case TYPE_OF_DATA::_INT:
				{
					session.lines[line].instructions[i + 1].data = (atoi(session.lines[line].instructions[i + 1].data.c_str()) > 0) ? "true" : "false";
					session.lines[line].instructions[i + 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_DOUBLE:
				{
					session.lines[line].instructions[i + 1].data = (atof(session.lines[line].instructions[i + 1].data.c_str()) > 0) ? "true" : "false";
					session.lines[line].instructions[i + 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_BOOLEAN:
				{
					session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "false" : "true";

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_STRING:
				{
					session.lines[line].instructions[i + 1].data = "null";
					session.lines[line].instructions[i + 1].type_of_data = TYPE_OF_DATA::_NONE;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				case TYPE_OF_DATA::_NONE:
				{
					session.lines[line].instructions[i + 1].data = "null";

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					end--;
					break;
				}
				}
			}
			else if (temp.body == "++")
			{
				if (i < end && session.lines[line].instructions[i + 1].isVariable && session.lines[line].instructions[i + 1].type_of_instruction
					== TYPE_OF_INSTRUCTION::DATA)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
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
					switch (session.lines[line].instructions[i - 1].type_of_data)
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
					switch (session.lines[line].instructions[i + 1].type_of_data)
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
					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) - 1);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) - 1);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = "false";
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data += "0";
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.data = "null";
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
	for (register int i = begin; i <= end; i++)
	{
		if (i >= session.lines[line].instructions.size()) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];


			if (temp.body == "*")
			{
				if (i > 0)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
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

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							std::string temp = session.lines[line].instructions[i + 1].data;
							unsigned int size = atoi(session.lines[line].instructions[i - 1].data.c_str());

							session.lines[line].instructions[i - 1].data = EMPTY;

							for (register unsigned int m = 0; m < size; m++)
								session.lines[line].instructions[i - 1].data += temp;

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							std::string temp = session.lines[line].instructions[i + 1].data;
							unsigned int size = atoi(session.lines[line].instructions[i - 1].data.c_str());

							session.lines[line].instructions[i - 1].data = EMPTY;

							for (register unsigned int m = 0; m < size; m++)
								session.lines[line].instructions[i - 1].data += temp;

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
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
					i--;
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
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "null";
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
								session.lines[line].instructions[i - 1].data = "null";
							}
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
					i--;
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
	// Операторы третьего уровня [+, -, ==]
	for (register int i = begin; i <= end; i++)
	{
		if (i >= session.lines[line].instructions.size()) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "+")
			{
				if (i > 0)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
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
							if (session.lines[line].instructions[i - 1].data == "true")
							{
								session.lines[line].instructions[i - 1].data = atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1;
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;
							}
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
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
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
								session.lines[line].instructions[i - 1].data = atof(session.lines[line].instructions[i - 1].data.c_str()) + 1;
							}
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
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
					i--;
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "-")
			{
				switch (session.lines[line].instructions[i + 1].type_of_data)
				{
				case TYPE_OF_DATA::_INT:
				{
					if (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA)
					{
						session.lines[line].instructions[i + 1].data = std::to_string(-1 * atoi(session.lines[line].instructions[i + 1].data.c_str()));

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						end--;
						continue;
					}

					switch (session.lines[line].instructions[i - 1].type_of_data)
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
						session.lines[line].instructions[i - 1].data = "null";
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_DOUBLE:
				{
					if (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA)
					{
						session.lines[line].instructions[i + 1].data = std::to_string(-1 * atof(session.lines[line].instructions[i + 1].data.c_str()));

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						end--;
						continue;
					}

					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
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
						session.lines[line].instructions[i - 1].data = "null";
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_BOOLEAN:
				{
					if (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA)
					{
						session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "false" : "true";

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						end--;
						continue;
					}

					switch (session.lines[line].instructions[i - 1].type_of_data)
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
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
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
					if (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA)
					{
						session.lines[line].instructions[i + 1].data = "";

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						end--;
						continue;
					}

					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_NONE:
				{
					if (i == 0 || session.lines[line].instructions[i - 1].type_of_instruction != TYPE_OF_INSTRUCTION::DATA)
					{
						session.lines[line].instructions[i + 1].data = "null";

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						end--;
						continue;
					}

					session.lines[line].instructions[i - 1].data = "null";
					break;
				}
				}

				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				i--;
				end -= 2;
			}
			else continue;
		}
	}
	// Сюда
	// Операторы нулевого уровня [=]
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "==")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "true" : "false";
					session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			else if (temp.body == "!=")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "false" : "true";
					session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			else if (temp.body == ">")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					if (session.lines[line].instructions[i + 1].data == session.lines[line].instructions[i - 1].data)
					{
						session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						i--;
						end -= 2;

						continue;
					}

					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "false" && session.lines[line].instructions[i + 1].data == "true") ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < session.lines[line].instructions[i + 1].data.length()
								|| session.lines[line].instructions[i - 1].data.length() == session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
							&& session.lines[line].instructions[i + 1].data != "false")
							session.lines[line].instructions[i - 1].data = "true";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			else if (temp.body == "<")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					if (session.lines[line].instructions[i + 1].data == session.lines[line].instructions[i - 1].data)
					{
						session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
						session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
						i--;
						end -= 2;

						continue;
					}

					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true" && session.lines[line].instructions[i + 1].data == "false") ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > session.lines[line].instructions[i + 1].data.length()
								|| session.lines[line].instructions[i - 1].data.length() == session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						if (session.lines[line].instructions[i + 1].data != "0" && session.lines[line].instructions[i + 1].data != "null"
							&& session.lines[line].instructions[i + 1].data != "false")
							session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			else if (temp.body == "<=")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "false" && session.lines[line].instructions[i + 1].data == "true"
								|| session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						if (session.lines[line].instructions[i + 1].data != "0"
							&& session.lines[line].instructions[i + 1].data != "false")
							session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			else if (temp.body == ">=")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true" && session.lines[line].instructions[i + 1].data == "false"
								|| session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "true" : "true";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							if (session.lines[line].instructions[i + 1].data != "0"
								&& session.lines[line].instructions[i + 1].data != "false")
								session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						if (session.lines[line].instructions[i + 1].data != "0"
							&& session.lines[line].instructions[i + 1].data != "false")
							session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
				else continue;
			}
		}
	}
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "||")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					session.lines[line].instructions[i - 1].data = ((session.lines[line].instructions[i - 1].data == "true"
						|| session.lines[line].instructions[i - 1].data == "1") || (session.lines[line].instructions[i + 1].data == "true"
							|| session.lines[line].instructions[i + 1].data == "1")) ? "true" : "false";
					session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
			if (temp.body == "&&")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					session.lines[line].instructions[i - 1].data = ((session.lines[line].instructions[i - 1].data == "true"
						|| session.lines[line].instructions[i - 1].data == "1") && (session.lines[line].instructions[i + 1].data == "true"
							|| session.lines[line].instructions[i + 1].data == "1")) ? "true" : "false";
					session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
		}
	}
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "=")
			{
				if (i > 0)
				{
					session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
					session.lines[line].instructions[i - 1].type_of_data = session.lines[line].instructions[i + 1].type_of_data;
					session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "+=")
			{
				if (i > 0)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
								+ atoi(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i - 1].data == "true")
							{
								session.lines[line].instructions[i - 1].data = atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1;
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;

								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(std::stof(session.lines[line].instructions[i - 1].data)
								+ std::stof(session.lines[line].instructions[i + 1].data));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i - 1].data == "true")
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
								session.lines[line].instructions[i - 1].data = atof(session.lines[line].instructions[i - 1].data.c_str()) + 1;

								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1);

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) + 1);

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = "true";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "-=")
			{
				switch (session.lines[line].instructions[i + 1].type_of_data)
				{
				case TYPE_OF_DATA::_INT:
				{
					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
							- atoi(session.lines[line].instructions[i + 1].data.c_str()));

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
							atoi(session.lines[line].instructions[i - 1].data.c_str()));
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_DOUBLE:
				{

					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
							atoi(session.lines[line].instructions[i - 1].data.c_str()));

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_BOOLEAN:
				{
					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						if (session.lines[line].instructions[i + 1].data == "true")
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str()) - 1);
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						if (session.lines[line].instructions[i + 1].data == "true")
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) - 1);
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (session.lines[line].instructions[i + 1].data == "true")
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_STRING:
				{
					switch (session.lines[line].instructions[i - 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_NONE:
				{
					session.lines[line].instructions[i - 1].data = "null";

					session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
					break;
				}
				}

				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
				session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				i--;
				end -= 2;
			}
			else if (temp.body == "/=")
			{
				if (i > 0)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
								/ atoi(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "null";
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
								session.lines[line].instructions[i - 1].data = "null";

								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
			else if (temp.body == "*=")
			{
				if (i > 0)
				{
					switch (session.lines[line].instructions[i + 1].type_of_data)
					{
					case TYPE_OF_DATA::_INT:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atoi(session.lines[line].instructions[i - 1].data.c_str())
								* atoi(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "true";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							std::string temp = session.lines[line].instructions[i - 1].data;
							for (register unsigned int m = 0; m < atoi(session.lines[line].instructions[i + 1].data.c_str()) - 1; m++)
								session.lines[line].instructions[i - 1].data += temp;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];

							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "true";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							for (register unsigned int i = 0; i < atoi(session.lines[line].instructions[i + 1].data.c_str()); i++)
								session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "0";
								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "0.0";
								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "";
								session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							}
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						switch (session.lines[line].instructions[i - 1].type_of_data)
						{
						case TYPE_OF_DATA::_INT:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						session.all_data.find(session.lines[line].instructions[i - 1].body)->second = session.lines[line].instructions[i - 1];
						break;
					}
					}

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
				else
				{
					// ... синтаксическая ошибка
				}
			}
		}
	}
}

