#include "Global.h"

void do_line_script_operators(Session& session, const unsigned int line, const unsigned int begin, unsigned int end)
// Выполнение часть инструкций в строке. Работа с операторами
{
	// Парсинг фигурных скобочек (для массива)
	end -= parse_array_brackets(session, line, begin, end);

	for (register int i = begin; i <= end; i++)
	{
		if (i >= session.lines[line].instructions.size()) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction &temp = session.lines[line].instructions[i];

			if (temp.body == "]")
			{
				for (register unsigned int f = i - 1; f >= begin; f--)
				{
					if (session.lines[line].instructions[f].body == "[")
					{
						// Выполнить операции внутри скобочек
						do_line_script_operators(session, line, f + 1, i - 1);

						Instruction * first_node	= &session.lines[line].instructions[f - 1];
						Instruction * temp			= &session.lines[line].instructions[f - 1];
						temp->ptr					= nullptr;

						for (register unsigned int s = f + 1; s < i; s++)
						{
							if (s >= session.lines[line].instructions.size()) // <------ Костыль
							{
								i = session.lines[line].instructions.size() - 1;
								break;
							}

							if (session.lines[line].instructions[s].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
							{
								if (temp->ptr != nullptr)
									temp = temp->ptr;

								// Если смещение - по числу
								if (session.lines[line].instructions[s].type_of_data == TYPE_OF_DATA::_INT)
								{
									// Смещение
									size_t bias = atoi(session.lines[line].instructions[s].data.c_str());

									// Если число выходит за пределы массива
									if (bias >= temp->array.size())
									{
										// Если данные - строка и массив отсутствует
										if (temp->type_of_data == TYPE_OF_DATA::_STRING && temp->array.size() == 0)
										{
											// Если смещение входит в рамки размера строки
											if (bias < temp->data.length())
											{
												temp->data = temp->data[bias];
												temp->selected_char = bias;
											}
											// Иначе если выходит за пределы массива строки
											else
											{
												// Синтаксическая ошибка...
												session.error = new ErrorCore("out of bounds array", &session);
												return;
											}
										}
										// Иначе если массив существует
										else
										{
											// Новая пустая инструкция
											Instruction new_null_instruction;
											new_null_instruction.data = "null";
											new_null_instruction.type_of_data = TYPE_OF_DATA::_NONE;
											new_null_instruction.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;

											// Заполнение массива новыми инструкциями
											for (register unsigned counter = temp->array.size(); counter <= bias; counter++)
												temp->array.push_back(new_null_instruction);

											// Установка указателя на последний элемент массива
											temp->why_array_is_used = true;
											temp->ptr = &temp->array[temp->array.size() - 1];
											temp->data = "null";
											temp->type_of_data = TYPE_OF_DATA::_NONE;
										}
									}
									// Иначе если массив существует и смещение входит в рамки его размера
									else
									{
										temp->why_array_is_used = true;
										temp->ptr = &temp->array[bias];
										temp->data = temp->array[bias].data;
										temp->type_of_data = temp->array[bias].type_of_data;
									}
								}
								// Если смещение по строке
								else if (session.lines[line].instructions[s].type_of_data == TYPE_OF_DATA::_STRING)
								{
									std::string & bias = session.lines[line].instructions[s].data;

									// Если индекс с таким ключем не существует
									if (temp->array_map.find(bias) == temp->array_map.end())
									{
										Instruction new_null_instruction;
										new_null_instruction.data = "null";
										new_null_instruction.type_of_data = TYPE_OF_DATA::_NONE;
										new_null_instruction.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;

										temp->array_map[bias] = new_null_instruction;
									}

									temp->why_array_is_used = false;
									temp->ptr = &temp->array_map[bias];
									temp->data = temp->array_map[bias].data;
									temp->type_of_data = temp->array_map[bias].type_of_data;
								}
							}
						}

						if (first_node != temp)
						{
							first_node->why_array_is_used = temp->why_array_is_used;
							first_node->data = temp->data;
							first_node->type_of_data = temp->type_of_data;
						}

						// Удаление использованных инструкций
						bool flag = true;
						do
						{
							if (session.lines[line].instructions[f].body == "]") flag = false;

							session.lines[line].instructions.erase(session.lines[line].instructions.begin() + f);
							end--;
							i--;
						} while ((session.lines[line].instructions[f].body != "]" || flag) && f < session.lines[line].instructions.size());
						break;
					}
				}
			}
		}
	}

	// Операторы первого уровня [(, ), ++, --, -]
	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0 || i >= session.lines[line].instructions.size()) break; // <------ Костыль

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
					if (session.lines[line].instructions[i + 1].isConst)
					{
						// Ошибка. Константу нельзя изменять
						session.error = new ErrorCore("constant not subject to change", &session);
						return;
					}

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
						session.lines[line].instructions[i + 1].type_of_data = TYPE_OF_DATA::_INT;
						break;
					}
					}
					session.all_data.find(session.lines[line].instructions[i + 1].body)->second.data = session.lines[line].instructions[i + 1].data;
					session.all_data.find(session.lines[line].instructions[i + 1].body)->second.type_of_data = session.lines[line].instructions[i + 1].type_of_data;

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
					if (session.lines[line].instructions[i - 1].isConst)
					{
						// Ошибка. Константу нельзя изменять
						session.error = new ErrorCore("constant not subject to change", &session);
						return;
					}

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
						session.all_data.find(session.lines[line].instructions[i - 1].body)->second.type_of_data = TYPE_OF_DATA::_INT;
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
					if (session.lines[line].instructions[i + 1].isConst)
					{
						// Ошибка. Константу нельзя изменять
						session.error = new ErrorCore("constant not subject to change", &session);
						return;
					}

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
					if (session.lines[line].instructions[i - 1].isConst)
					{
						// Ошибка. Константу нельзя изменять
						session.error = new ErrorCore("constant not subject to change", &session);
						return;
					}

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
		if (i < 0 || i >= session.lines[line].instructions.size()) break; // <------ Костыль

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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
				}
			}
			else continue;
		}
	}

	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
	// Операторы третьего уровня [+, -, ==]
	for (register int i = begin; i <= end; i++)
	{
		if (i < 0) break; // <------ Костыль

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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
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
	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
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
					session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1] == session.lines[line].instructions[i + 1]) ? "true" : "false";
					session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

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
					session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1] == session.lines[line].instructions[i + 1]) ? "false" : "true";
					session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

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
					if (session.lines[line].instructions[i + 1].data == session.lines[line].instructions[i - 1].data
						|| (session.lines[line].instructions[i + 1].type_of_data == TYPE_OF_DATA::_INT && session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_STRING &&
							session.lines[line].instructions[i - 1].data.length() == atoi(session.lines[line].instructions[i + 1].data.c_str())))
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "false" && session.lines[line].instructions[i + 1].data == "true") ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) < session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) < session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i - 1].data.length() == 1 && session.lines[line].instructions[i + 1].data.length() == 1)
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data[0] < session.lines[line].instructions[i + 1].data[0]
									|| session.lines[line].instructions[i - 1].data[0] == session.lines[line].instructions[i + 1].data[0]) ? "false" : "true";
							else
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() < session.lines[line].instructions[i + 1].data.length()
									|| session.lines[line].instructions[i - 1].data.length() == session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
					if (session.lines[line].instructions[i + 1].data == session.lines[line].instructions[i - 1].data
						|| (session.lines[line].instructions[i - 1].type_of_data == TYPE_OF_DATA::_INT && session.lines[line].instructions[i + 1].type_of_data == TYPE_OF_DATA::_STRING &&
							session.lines[line].instructions[i + 1].data.length() == atoi(session.lines[line].instructions[i - 1].data.c_str())))
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > atof(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true" && session.lines[line].instructions[i + 1].data == "false") ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) > session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) > session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i - 1].data.length() == 1 && session.lines[line].instructions[i + 1].data.length() == 1)
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data[0] > session.lines[line].instructions[i + 1].data[0]
									|| session.lines[line].instructions[i - 1].data[0] == session.lines[line].instructions[i + 1].data[0]) ? "false" : "true";
							else
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() > session.lines[line].instructions[i + 1].data.length()
									|| session.lines[line].instructions[i - 1].data.length() == session.lines[line].instructions[i + 1].data.length()) ? "false" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "true";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "false" && session.lines[line].instructions[i + 1].data == "true"
								|| session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i - 1].data.length() == 1 && session.lines[line].instructions[i + 1].data.length() == 1)
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data[0] <= session.lines[line].instructions[i + 1].data[0]) ? "true" : "false";
							else
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() <= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "true";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= atof(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true" && session.lines[line].instructions[i + 1].data == "false"
								|| session.lines[line].instructions[i - 1].data == session.lines[line].instructions[i + 1].data) ? "true" : "true";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i + 1].data = (session.lines[line].instructions[i + 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= atoi(session.lines[line].instructions[i + 1].data.c_str())) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = (atof(session.lines[line].instructions[i - 1].data.c_str()) >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data == "true") ? "1" : "0";
							session.lines[line].instructions[i - 1].data = (atoi(session.lines[line].instructions[i - 1].data.c_str()) >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i - 1].data.length() == 1 && session.lines[line].instructions[i + 1].data.length() == 1)
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data[0] >= session.lines[line].instructions[i + 1].data[0]) ? "true" : "false";
							else
								session.lines[line].instructions[i - 1].data = (session.lines[line].instructions[i - 1].data.length() >= session.lines[line].instructions[i + 1].data.length()) ? "true" : "false";
							
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						if(session.lines[line].instructions[i + 1].data == "null") 
							session.lines[line].instructions[i - 1].data = "true";
						else 
							session.lines[line].instructions[i - 1].data = "false";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;
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

	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "&&")
			{
				if (session.lines[line].instructions[i + 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA
					&& session.lines[line].instructions[i - 1].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
				{
					session.lines[line].instructions[i - 1].data = ((session.lines[line].instructions[i - 1].data == "true"
						|| session.lines[line].instructions[i - 1].data == "1") && (session.lines[line].instructions[i + 1].data == "true"
							|| session.lines[line].instructions[i + 1].data == "1")) ? "true" : "false";
					session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
		}
	}

	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
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
					session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
					i--;
					end -= 2;
				}
			}
		}
	}


	if (end >= session.lines[line].instructions.size()) end = session.lines[line].instructions.size() - 1;
	for (register int i = end; i >= begin; i--)
	{
		if (i < 0) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			if (session.lines[line].instructions[i - 1].isConst && session.lines[line].instructions[i - 1].type_of_data != TYPE_OF_DATA::_NONE)
			{
				// Ошибка. Константу нельзя изменять
				session.error = new ErrorCore("constant not subject to change", &session);
				return;
			}

			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "=")
			{
				if (i > 0)
				{
					write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i + 1]);

					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i + 1);
					session.lines[line].instructions.erase(session.lines[line].instructions.begin() + i);
				}
				else
				{
					// ... синтаксическая ошибка
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								+ atof(session.lines[line].instructions[i + 1].data.c_str()));

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i - 1].data == "true")
							{
								session.lines[line].instructions[i - 1].data = atoi(session.lines[line].instructions[i - 1].data.c_str()) + 1;
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;

								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_INT;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(std::stof(session.lines[line].instructions[i - 1].data)
								+ std::stof(session.lines[line].instructions[i + 1].data));

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i - 1].data == "true")
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;
								session.lines[line].instructions[i - 1].data = atof(session.lines[line].instructions[i - 1].data.c_str()) + 1;

								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) + 1);

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							if (session.lines[line].instructions[i + 1].data == "true")
								session.lines[line].instructions[i - 1].data = "true";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
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

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
						{
							session.lines[line].instructions[i - 1].data = "false";
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
							atoi(session.lines[line].instructions[i - 1].data.c_str()));
						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
							- atof(session.lines[line].instructions[i + 1].data.c_str()));

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (atoi(session.lines[line].instructions[i - 1].data.c_str()) != 0)
						{
							session.lines[line].instructions[i - 1].data = "false";
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i - 1].data.substr(0,
							atoi(session.lines[line].instructions[i - 1].data.c_str()));

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						}
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						if (session.lines[line].instructions[i + 1].data == "true")
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str()) - 1);
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						}
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						if (session.lines[line].instructions[i + 1].data == "true")
						{
							session.lines[line].instructions[i - 1].data = "true";
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						}
						break;
					}
					case TYPE_OF_DATA::_STRING:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_DOUBLE:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					case TYPE_OF_DATA::_BOOLEAN:
					{
						session.lines[line].instructions[i - 1].data = "null";
						session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
						break;
					}
					}
					break;
				}
				case TYPE_OF_DATA::_NONE:
				{
					session.lines[line].instructions[i - 1].data = "null";

					write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_BOOLEAN;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								/ atof(session.lines[line].instructions[i + 1].data.c_str()));

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "false";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;
								session.lines[line].instructions[i - 1].data = "null";

								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data = "null";
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_NONE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_DOUBLE;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "true";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							std::string temp = session.lines[line].instructions[i - 1].data;
							for (register unsigned int m = 0; m < atoi(session.lines[line].instructions[i + 1].data.c_str()) - 1; m++)
								session.lines[line].instructions[i - 1].data += temp;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);

							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data = std::to_string(atof(session.lines[line].instructions[i - 1].data.c_str())
								* atof(session.lines[line].instructions[i + 1].data.c_str()));

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = "true";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							for (register unsigned int i = 0; i < atoi(session.lines[line].instructions[i + 1].data.c_str()); i++)
								session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "0.0";
								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data = session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							if (session.lines[line].instructions[i + 1].data == "false")
							{
								session.lines[line].instructions[i - 1].data = "";
								write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							}
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							session.lines[line].instructions[i - 1].data = "null";
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_DOUBLE:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_BOOLEAN:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;
							session.lines[line].instructions[i - 1].type_of_data = TYPE_OF_DATA::_STRING;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_STRING:
						{
							session.lines[line].instructions[i - 1].data += session.lines[line].instructions[i + 1].data;

							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						case TYPE_OF_DATA::_NONE:
						{
							write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
							break;
						}
						}
						break;
					}
					case TYPE_OF_DATA::_NONE:
					{
						session.lines[line].instructions[i - 1].data = "null";

						write_data_from_local_to_global(session, session.lines[line].instructions[i - 1], session.lines[line].instructions[i - 1]);
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
					session.error = new ErrorCore("data should be left of the operator", &session);
					return;
				}
			}
		}
	}
}

int parse_array_brackets(Session &session, const unsigned int line, const unsigned int begin, unsigned int end)
// Парсинг фигурных собочек
{
	int sub = 0;
	for (register int i = begin; i <= end; i++)
	{
		if (i >= session.lines[line].instructions.size()) break; // <------ Костыль

		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::OPERATOR)
		{
			Instruction &temp = session.lines[line].instructions[i];

			if (temp.body == "{")
			{
				temp.array.clear();

				for (register int j = i + 1; j <= end; j++)
				{
					if (session.lines[line].instructions[j].body == "{")
					{
						parse_array_brackets(session, line, j, end);
						temp.array.push_back(session.lines[line].instructions[j]);
					}
					else if (session.lines[line].instructions[j].body == "}")
					{
						for (register unsigned int z = i + 1; z <= j; z++)
						{
							session.lines[line].instructions.erase(session.lines[line].instructions.begin() + z);
							sub++;
							end--;
							z--;
							j--;
							i++;
						}

						if (temp.array.size() != 0)
						{
							temp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
							temp.type_of_data = temp.array[0].type_of_data;
							temp.body = "array";
							temp.data = temp.array[0].data;
						}
						else
						{
							temp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
							temp.type_of_data = TYPE_OF_DATA::_NONE;
							temp.body = "array";
							temp.data = "null";
						}

						break;
					}
					else if (session.lines[line].instructions[j].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
						temp.array.push_back(session.lines[line].instructions[j]);
				}
				break;
			}
		}
	}
	return sub;
}

void write_data_from_local_to_global(Session &session, Instruction &first, Instruction &second)
// Запись данных из кэша в глабольную переменную
{
	if (first.selected_char > -1)
		session.all_data.find(first.body)->second.data[first.selected_char] = second.data[0];
	// Если это не массив
	else if (first.ptr == nullptr)
	{
		session.all_data.find(first.body)->second.data = second.data;
		session.all_data.find(first.body)->second.type_of_data = second.type_of_data;
	}
	else
	{
		first.ptr->data = second.data;
		first.ptr->type_of_data = second.type_of_data;

		session.all_data.find(first.body)->second = first;
	}

	if(second.array.size() > 0 && second.ptr == nullptr)
		session.all_data.find(first.body)->second.array = second.array;
	if(second.array_map.size() > 0 && second.ptr == nullptr)
		session.all_data.find(first.body)->second.array_map = second.array_map;

	first.data = second.data;
	first.type_of_data = second.type_of_data;
}