#include "Global.h"

bool do_line_script_commands(Session& session, unsigned int line, const unsigned int begin, unsigned int end)
// Выполнение часть инструкций в строке. Работа с командами
// ~ Возвращает успех выполнения команды
{
	// Поиск команд
	for (register int i = begin; i <= end; i++)
	{
		if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::COMMAND)
		{
			Instruction temp = session.lines[line].instructions[i];

			if (temp.body == "if" || temp.body == "elif")
			{
				session.last_command = "if";
				if (temp.body == "elif")
					if (session.last_command == "if" && session.last_command_success == true) break;

				for (register int find_data_counter = i + 1; find_data_counter <= end; find_data_counter++)
				{
					if (session.lines[line].instructions[find_data_counter].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
					{
						// Если условие истино
						if (session.lines[line].instructions[find_data_counter].data == "true"
							|| (session.lines[line].instructions[find_data_counter].data != "0"
								&& session.lines[line].instructions[find_data_counter].data != "null"
								&& session.lines[line].instructions[find_data_counter].data != "false"))
						{
							int begin_new = -1;
							int end_new = -1;

							// Поиск вхождения в новый уровень локального пространства
							for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
								if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
								{
									begin_new = find_begin_new_counter;
									break;
								}

							// Поиск конца нового уровеня локального пространства
							end_new = line + 1;
							while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
								|| session.lines[end_new].namespace_level == 0))
								end_new++;

							if (end_new == -1) end_new = begin_new;
							else end_new--;

							if (begin_new != -1)
							{
								session.last_command_success = true;
								do_script(session, begin_new, end_new, false, nullptr, true);
								return false;
							}
						}
						else session.last_command_success = false;
					}
					else if (find_data_counter == end)
					{
						// Синтаксическая ошибка
						session.error = new ErrorCore("this command must have a logical expression", &session);
						return true;
					}
				}
			}
			else if (temp.body == "else")
			{
				if ((session.last_command == "if") && session.last_command_success) break;

				int begin_new = -1;
				int end_new = -1;

				// Поиск вхождения в новый уровень локального пространства
				for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
					if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
					{
						begin_new = find_begin_new_counter;
						break;
					}

				// Поиск конца нового уровеня локального пространства
				end_new = line + 1;
				while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
					|| session.lines[end_new].namespace_level == 0))
					end_new++;

				if (end_new == -1) end_new = begin_new;
				else end_new--;

				if (begin_new != -1)
				{
					session.last_command_success = true;
					do_script(session, begin_new, end_new, false, nullptr, true);
					return false;
				}
			}
			else if (temp.body == "while")
			{
				for (register int find_data_counter = i + 1; find_data_counter <= end; find_data_counter++)
				{
					if (session.lines[line].instructions[find_data_counter].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
					{
						// Пока условие истино
						while (true)
						{
							// Перезапись инструкций из сохраненного места
							session.lines[line].instructions = session.lines[line].copy_instructions;

							do_script(session, line, line, true);

							if (!(session.lines[line].instructions[find_data_counter].data == "true"
								|| (session.lines[line].instructions[find_data_counter].data != "0"
									&& session.lines[line].instructions[find_data_counter].data != "null"
									&& session.lines[line].instructions[find_data_counter].data != "false"))) break;

							int begin_new = -1;
							int end_new = -1;

							// Поиск вхождения в новый уровень локального пространства
							for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
								if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
								{
									begin_new = find_begin_new_counter;
									break;
								}

							// Поиск конца нового уровеня локального пространства
							end_new = line + 1;
							while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
								|| session.lines[end_new].namespace_level == 0))
								end_new++;

							if (end_new == -1) end_new = begin_new;
							else end_new--;
							if (begin_new != -1)
							{
								// Перезапись инструкций из сохраненного места
								for (register unsigned update_command_counter = begin_new; update_command_counter <= end_new; update_command_counter++)
									if (session.lines[update_command_counter].copy_instructions.size() > 0) session.lines[update_command_counter].instructions = session.lines[update_command_counter].copy_instructions;

								session.last_command_success = true;
								do_script(session, begin_new, end_new);

								// Если была вызвана инструкция break
								if (session.isBreak)
								{
									session.isBreak = false;
									return true;
								}
							}
						}
					}
					else if (find_data_counter == end)
					{
						// Синтаксическая ошибка
						session.error = new ErrorCore("this command must have a logical expression", &session);
						return true;
					}
				}
			}
			else if (temp.body == "for")
			{
				// Перезапись инструкций из сохраненного места
				session.lines[line].instructions = session.lines[line].copy_instructions;

				byte divide_operator_counter = 0;

				std::vector<Instruction> second_commands_of_loop;
				std::vector<Instruction> thr_commands_of_loop;

				// Поиск разделенных частей команд цикла
				for (register unsigned int i = 0; i < session.lines[line].instructions.size(); i++)
				{
					if (session.lines[line].instructions[i].body == ";")
						divide_operator_counter++;

					else if (divide_operator_counter == 1)
						second_commands_of_loop.push_back(session.lines[line].instructions[i]);
					else if (divide_operator_counter == 2)
						thr_commands_of_loop.push_back(session.lines[line].instructions[i]);
				}
				if (divide_operator_counter < 2)
				{
					session.error = new ErrorCore("command must contain 3 blocks", &session);
					return true;
				}
				//

				// Пока условие истино
				while (true)
				{
					session.lines[line].instructions = second_commands_of_loop;
					do_script(session, line, line, true);

					if (!(session.lines[line].instructions[0].data == "true"
						|| (session.lines[line].instructions[0].data != "0"
							&& session.lines[line].instructions[0].data != "null"
							&& session.lines[line].instructions[0].data != "false"))) break;

					int begin_new = -1;
					int end_new = -1;

					// Поиск вхождения в новый уровень локального пространства
					for (register int find_begin_new_counter = line + 1; find_begin_new_counter < session.lines.size(); find_begin_new_counter++)
						if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
						{
							begin_new = find_begin_new_counter;
							break;
						}

					// Поиск конца нового уровеня локального пространства
					end_new = line + 1;
					while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
						|| session.lines[end_new].namespace_level == 0))
						end_new++;

					if (end_new == -1) end_new = begin_new;
					else end_new--;
					if (begin_new != -1)
					{
						// Перезапись инструкций из сохраненного места
						for (register unsigned update_command_counter = begin_new; update_command_counter <= end_new; update_command_counter++)
							if (session.lines[update_command_counter].copy_instructions.size() > 0) session.lines[update_command_counter].instructions = session.lines[update_command_counter].copy_instructions;

						session.last_command_success = true;
						do_script(session, begin_new, end_new);

						// Если была вызвана инструкция break
						if (session.isBreak)
						{
							session.isBreak = false;
							return true;
						}
					}
					session.lines[line].instructions = thr_commands_of_loop;
					do_script(session, line, line, true);
				}
				break;
			}
			else if (temp.body == "fun")
			{
				FunctionDefinition new_function;

				// Поиск параметров функции
				for (register unsigned int i = 2; i < session.lines[line].instructions.size(); i++)
				{
					if (session.lines[line].instructions[i].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
						new_function.parametrs.push_back(session.lines[line].instructions[i]);
				}

				// Поиск вхождения в новый уровень локального пространства
				for (register int find_begin_new_counter = line + 1; find_begin_new_counter < session.lines.size(); find_begin_new_counter++)
					if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
					{
						new_function.begin = find_begin_new_counter;
						break;
					}

				// Поиск конца нового уровеня локального пространства
				new_function.end = line + 1;
				while (new_function.end < session.lines.size() && (session.lines[new_function.end].namespace_level > session.lines[line].namespace_level
					|| session.lines[new_function.end].namespace_level == 0))
					new_function.end++;

				if (new_function.end == -1) new_function.end = new_function.begin;
				else new_function.end--;
				if (new_function.end != -1)
				{
					session.definition_functions[session.lines[line].instructions[1].body] = new_function;
				}
				i = new_function.end;
				break;
			}
			else if (temp.body == "continue")
			{
				session.isContinue = true;
				break;
			}
			else if (temp.body == "break")
			{
				session.isBreak= true;
				break;
			}
			else if (temp.body == "return")
			{
				if (i + 1 < session.lines[line].instructions.size())
				{
					session.current_function->result			= session.lines[line].instructions[i + 1];
					session.current_function->result.isVariable = false;
					session.current_function->result.body		= "function_value";
					session.current_function->result.ptr		= nullptr;
				}

				session.current_function->isReturn = true;
				return false;
				break;
			}
		}
	}
	return false;
}
