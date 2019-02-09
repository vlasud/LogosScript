#include "Global.h"

bool do_line_script_commands(Session& session, unsigned int line, const unsigned int begin, unsigned int end)
// ���������� ����� ���������� � ������. ������ � ���������
// ~ ���������� ����� ���������� �������
{
	// ����� ������
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
						// ���� ������� ������
						if (session.lines[line].instructions[find_data_counter].data == "true"
							|| (session.lines[line].instructions[find_data_counter].data != "0"
								&& session.lines[line].instructions[find_data_counter].data != "null"
								&& session.lines[line].instructions[find_data_counter].data != "false"))
						{
							int begin_new = -1;
							int end_new = -1;

							// ����� ��������� � ����� ������� ���������� ������������
							for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
								if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
								{
									begin_new = find_begin_new_counter;
									break;
								}

							// ����� ����� ������ ������� ���������� ������������
							end_new = line + 1;
							while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
								|| session.lines[end_new].namespace_level == 0))
								end_new++;

							if (end_new == -1) end_new = begin_new;
							else end_new--;

							if (begin_new != -1)
							{
								session.last_command_success = true;
								do_script(session, begin_new, end_new);
								return false;
							}
						}
						else session.last_command_success = false;
					}
					else if (find_data_counter == end)
					{
						// �������������� ������
					}
				}
			}
			else if (temp.body == "else")
			{
				if ((session.last_command == "if") && session.last_command_success == true) break;

				int begin_new = -1;
				int end_new = -1;

				// ����� ��������� � ����� ������� ���������� ������������
				for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
					if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
					{
						begin_new = find_begin_new_counter;
						break;
					}

				// ����� ����� ������ ������� ���������� ������������
				end_new = line + 1;
				while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
					|| session.lines[end_new].namespace_level == 0))
					end_new++;

				if (end_new == -1) end_new = begin_new;
				else end_new--;

				if (begin_new != -1)
				{
					session.last_command_success = true;
					do_script(session, begin_new, end_new);
					return false;
				}
			}
			else if (temp.body == "while")
			{
				for (register int find_data_counter = i + 1; find_data_counter <= end; find_data_counter++)
				{
					if (session.lines[line].instructions[find_data_counter].type_of_instruction == TYPE_OF_INSTRUCTION::DATA)
					{
						// ���� ������� ������
						while (true)
						{
							// ���������� ���������� �� ������������ �����
							session.lines[line].instructions = session.lines[line].copy_instructions;

							do_script(session, line, line, true);

							if (!(session.lines[line].instructions[find_data_counter].data == "true"
								|| (session.lines[line].instructions[find_data_counter].data != "0"
									&& session.lines[line].instructions[find_data_counter].data != "null"
									&& session.lines[line].instructions[find_data_counter].data != "false"))) break;

							int begin_new = -1;
							int end_new = -1;

							// ����� ��������� � ����� ������� ���������� ������������
							for (register int find_begin_new_counter = line + 1; find_begin_new_counter <= session.lines.size(); find_begin_new_counter++)
								if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
								{
									begin_new = find_begin_new_counter;
									break;
								}

							// ����� ����� ������ ������� ���������� ������������
							end_new = line + 1;
							while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
								|| session.lines[end_new].namespace_level == 0))
								end_new++;

							if (end_new == -1) end_new = begin_new;
							else end_new--;
							if (begin_new != -1)
							{
								// ���������� ���������� �� ������������ �����
								for (register unsigned update_command_counter = begin_new; update_command_counter <= end_new; update_command_counter++)
									if (session.lines[update_command_counter].copy_instructions.size() > 0) session.lines[update_command_counter].instructions = session.lines[update_command_counter].copy_instructions;

								session.last_command_success = true;
								do_script(session, begin_new, end_new);
							}
						}
					}
					else if (find_data_counter == end)
					{
						// �������������� ������
					}
				}
			}
			else if (temp.body == "for")
			{
				// ���������� ���������� �� ������������ �����
				session.lines[line].instructions = session.lines[line].copy_instructions;

				byte divide_operator_counter = 0;

				std::vector<Instruction> second_commands_of_loop;
				std::vector<Instruction> thr_commands_of_loop;

				// ����� ����������� ������ ������ �����
				for (register unsigned int i = 0; i < session.lines[line].instructions.size(); i++)
				{
					if (session.lines[line].instructions[i].body == ";")
						divide_operator_counter++;

					else if (divide_operator_counter == 1)
						second_commands_of_loop.push_back(session.lines[line].instructions[i]);
					else if (divide_operator_counter == 2)
						thr_commands_of_loop.push_back(session.lines[line].instructions[i]);
				}
				//

				// ���� ������� ������
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

					// ����� ��������� � ����� ������� ���������� ������������
					for (register int find_begin_new_counter = line + 1; find_begin_new_counter < session.lines.size(); find_begin_new_counter++)
						if (session.lines[find_begin_new_counter].namespace_level == session.lines[line].namespace_level + 1)
						{
							begin_new = find_begin_new_counter;
							break;
						}

					// ����� ����� ������ ������� ���������� ������������
					end_new = line + 1;
					while (end_new < session.lines.size() && (session.lines[end_new].namespace_level > session.lines[line].namespace_level
						|| session.lines[end_new].namespace_level == 0))
						end_new++;

					if (end_new == -1) end_new = begin_new;
					else end_new--;
					if (begin_new != -1)
					{
						// ���������� ���������� �� ������������ �����
						for (register unsigned update_command_counter = begin_new; update_command_counter <= end_new; update_command_counter++)
							if (session.lines[update_command_counter].copy_instructions.size() > 0) session.lines[update_command_counter].instructions = session.lines[update_command_counter].copy_instructions;

						session.last_command_success = true;
						do_script(session, begin_new, end_new);
					}
					session.lines[line].instructions = thr_commands_of_loop;
					do_script(session, line, line, true);
				}
				break;
			}
		}
	}
	return false;
}
