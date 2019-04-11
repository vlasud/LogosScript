#include "Global.h"

SystemFunction::SystemFunction(std::string name_of_function, u_int count_of_param, void(*ptr_on_function)(SystemFunction *))
{
	this->name_of_function = name_of_function;
	this->count_of_param = count_of_param;
	this->ptr_on_function = ptr_on_function;

	this->result_of_function.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
}

std::string SystemFunction::get_name(void) const
{
	return this->name_of_function;
}

void SystemFunction::start_function(Session *session)
{
	// Если кол-во аргументов не совпадает - ошибка
	if (this->count_of_param != this->instructions_of_function.size())
	{
		session->error = new ErrorCore("the function <" + this->get_name() + "> must take " + std::to_string(this->count_of_param) + " parameter", session);
		return;
	}
	this->session = session;

	this->ptr_on_function(this);
}

Instruction SystemFunction::get_result(void) const
{
	return this->result_of_function;
}

Instruction& SystemFunction::get_result_instruction(void)
{
	return this->result_of_function;
}

const std::vector<Instruction> SystemFunction::get_instructions(void)
{
	return this->instructions_of_function;
}

void SystemFunction::set_params(Instruction &parametr)
{
	this->instructions_of_function.push_back(parametr);
}

Session * SystemFunction::get_session(void)
{
	return this->session;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void len(SystemFunction *object)
// Возвращает длину строки
// 1 параметр
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	
	res->type_of_data = TYPE_OF_DATA::_INT;
	res->data = (instructions[0].array.size() > 0) ?
		std::to_string(instructions[0].array.size()) : std::to_string(instructions[0].data.length());
}

void write(SystemFunction *object)
// Вывод информации в документ
// 1 параметр
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();

	object->get_session()->output.output_data.push_back(instructions[0].data);
}

void writeln(SystemFunction *object)
// Вывод информации в документ с переходом на новую строку в конце
// 1 параметр
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();

	object->get_session()->output.output_data.push_back(instructions[0].data + "<br>");
}

void substr(SystemFunction *object)
// Обрезает строку
// 3 параметра
// [строка, начало, кол-во символов]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();

	res->data = instructions[0].data.substr(atoi(instructions[1].data.c_str()), atoi(instructions[2].data.c_str()));
}

void removeAt(SystemFunction *object)
// Удаляет данный объект из массива/строки
// 2 параметра
// [строка, индекс]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.array.size() > 0)
	{
		if (result.array.size() <= atoi(instructions[1].data.c_str()))
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("out of bounds array", object->get_session());
			return;
		}
		result.array.erase(result.array.begin() + atoi(instructions[1].data.c_str()));
	}
	else if (result.type_of_data == TYPE_OF_DATA::_STRING)
	{
		if (result.data.length() <= atoi(instructions[1].data.c_str()))
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("out of bounds array", object->get_session());
			return;
		}
		result.data.erase(result.data.begin() + atoi(instructions[1].data.c_str()));
	}
	*res = result;
	write_data_from_local_to_global(*object->get_session(), result, result);
}

void add(SystemFunction *object)
// Добавляет в конце массива/строки
// 2 параметра
// [строка/массив, индекс]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.array.size() > 0 || result.type_of_data != TYPE_OF_DATA::_STRING) result.array.push_back(instructions[1]);
	else if (result.type_of_data == TYPE_OF_DATA::_STRING)
	{
		if (instructions[1].type_of_data == TYPE_OF_DATA::_STRING) result.data += instructions[1].data;
		else
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("if the data is a string, the function accepts only the string", object->get_session());
			return;
		}
	}
	*res = result;
	write_data_from_local_to_global(*object->get_session(), result, result);
}

void insert(SystemFunction *object)
// Внедряет в массив/строку
// 3 параметра
// [строка/массив, индекс, данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.array.size() > 0 || result.type_of_data != TYPE_OF_DATA::_STRING)
	{
		if (atoi(instructions[1].data.c_str()) >= result.array.size())
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("out of bounds array", object->get_session());
			return;
		}
		result.array.insert(result.array.begin() + atoi(instructions[1].data.c_str()), instructions[2]);
	}
	else if (result.type_of_data == TYPE_OF_DATA::_STRING)
	{
		if (instructions[2].type_of_data == TYPE_OF_DATA::_STRING)
		{
			if (atoi(instructions[1].data.c_str()) >= result.data.length())
			{
				// Синтаксическая ошибка
				object->get_session()->error = new ErrorCore("out of bounds array", object->get_session());
				return;
			}
			result.data.insert(result.data.begin() + atoi(instructions[1].data.c_str()), instructions[2].data[0]);
		}
		else
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("if the data is a string, the function accepts only the string", object->get_session());
			return;
		}
	}
	*res = result;
	write_data_from_local_to_global(*object->get_session(), result, result);
}

void replace(SystemFunction *object)
// Заменяет в массиве/строке
// 3 параметра
// [строка/массив, что заменить, на что заменить]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.array.size() > 0 || result.type_of_data != TYPE_OF_DATA::_STRING)
	{
		for (register u_int i = 0; i < result.array.size(); i++)
			if (result.array[i].data == instructions[1].data && result.array[i].type_of_data == instructions[1].type_of_data)
				result.array[i] = instructions[2];
	}
	else if (result.type_of_data == TYPE_OF_DATA::_STRING)
	{
		if (instructions[2].type_of_data == TYPE_OF_DATA::_STRING)
		{
			for (register u_int i = 0; i < result.data.length(); i++)
				if (result.data[i] == instructions[1].data[0])
					result.data[i] = instructions[2].data[0];
		}
		else
		{
			// Синтаксическая ошибка
			object->get_session()->error = new ErrorCore("if the data is a string, the function accepts only the string", object->get_session());
			return;
		}
	}
	*res = result;
	write_data_from_local_to_global(*object->get_session(), result, result);
}

void split(SystemFunction *object)
// Разбивает строку на массив
// 2 параметра
// [строка/массив, символ разделитель]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.type_of_data != TYPE_OF_DATA::_STRING)
	{
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("function accepts string only", object->get_session());
		return;
	}

	std::string word = EMPTY;
	result.array.clear();

	for (register u_int i = 0; i < result.data.length(); i++)
	{
		if (result.data[i] == instructions[1].data[0] || i == result.data.length() - 1)
		{
			if (i == result.data.length() - 1) word += result.data[i];

			Instruction tmp;
			tmp.data				= word;
			tmp.type_of_data		= TYPE_OF_DATA::_STRING;
			tmp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;

			result.array.push_back(tmp);

			word = EMPTY;
		}
		else word += result.data[i];
	}

	*res = result;
}

void down(SystemFunction *object)
// Переводит все символы строки в нижний регистр
// 1 параметр
// [строка]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.type_of_data != TYPE_OF_DATA::_STRING)
	{
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("function accepts string only", object->get_session());
		return;
	}

	for (register u_int i = 0; i < result.data.length(); i++)
	{
		if (result.data[i] >= 'A' && result.data[i] <= 'Z')
			result.data[i] = (char)((int)'a' + ((int)result.data[i] - (int)'A'));
		else if (result.data[i] >= 'А' && result.data[i] <= 'Я')
			result.data[i] = (char)((int)'а' + ((int)result.data[i] - (int)'А'));
	}

	*res = result;
}

void top(SystemFunction *object)
// Переводит все символы строки в нижний регистр
// 1 параметр
// [строка]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (result.type_of_data != TYPE_OF_DATA::_STRING)
	{
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("function accepts string only", object->get_session());
		return;
	}


	for (register u_int i = 0; i < result.data.length(); i++)
	{
		if (result.data[i] >= 'a' && result.data[i] <= 'z')
			result.data[i] = (char)((int)'A' + ((int)result.data[i] - (int)'a'));
		else if (result.data[i] >= 'а' && result.data[i] <= 'я')
			result.data[i] = (char)((int)'А' + ((int)result.data[i] - (int)'я'));
	}

	*res = result;
}

void find(SystemFunction *object)
// Разбивает строку на массив
// 2 параметра
// [строка/массив, символ разделитель]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];
	int index = -1;

	if (result.type_of_data == TYPE_OF_DATA::_STRING && result.array.size() == 0)
	{
		bool isFound = false;

		for (register u_int i = 0; i < result.data.length(); i++)
		{
			if (result.data[i] == instructions[1].data[0])
			{
				for (register u_int j = 0; j < instructions[1].data.length(); j++)
				{
					if (result.data[i + j] != instructions[1].data[j]) break;
					else if (j == instructions[1].data.length() - 1)
					{
						index = i;
						isFound = true;
					}
				}
				if (isFound)
					break;
			}
		}
	}
	else if (result.array.size() > 0)
	{
		for (register u_int i = 0; i < result.array.size(); i++)
		{
			if(result.array[i] == instructions[1])
			{
				index = i;
				break;
			}
		}
	}
	else
	{
		// Синтаксическая ошибка. Принимает только массив или строку
		object->get_session()->error = new ErrorCore("function accepts string/array only", object->get_session());
		return;
	}
	result.type_of_data = TYPE_OF_DATA::_INT;
	result.data = std::to_string(index);
	*res = result;
}

void __int(SystemFunction *object)
// Преобразование к типу int
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	switch (result .type_of_data)
	{
	case TYPE_OF_DATA::_BOOLEAN:
		result.data = (result.data == "true") ? "1" : "0";
		break;
	case TYPE_OF_DATA::_DOUBLE:
		result.data = std::to_string((int)atof(result.data.c_str()));
		break;
	case TYPE_OF_DATA::_STRING:
	{
		int res_int = 0;
		for (register u_int i = 0; i < result.data.length() && result.data[i] >= '0' && result.data[i] <= '9'; i++)
			res_int = res_int * 10 + (result.data[i] - (int)'0');

		result.data = std::to_string(res_int);
		break;
	}
	case TYPE_OF_DATA::_NONE:
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("null cannot be converted to int", object->get_session());
		return;
		break;
	}
	result.type_of_data = TYPE_OF_DATA::_INT;
	*res = result;
}

void __double(SystemFunction *object)
// Преобразование к типу double
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	switch (result.type_of_data)
	{
	case TYPE_OF_DATA::_BOOLEAN:
		result.data = (result.data == "true") ? "1.0" : "0.0";
		break;
	case TYPE_OF_DATA::_INT:
		result.data = std::to_string((int)atoi(result.data.c_str()));
		break;
	case TYPE_OF_DATA::_STRING:
	{
		double res_ = 0;
		for (register u_int i = 0; i < result.data.length() && result.data[i] >= '0' && result.data[i] <= '9' 
			|| result.data[i] == '.'; i++)
		{
			if (result.data[i] == '.')
			{
				if (i + 1 < result.data.length() && result.data[i + 1] >= '0' && result.data[i + 1] <= '9')
				{
					u_int counter = 0;
					while (result.data[++i] >= '0' && result.data[i] <= '9')
						res_ += (float)(result.data[i] - (int)'0') / (pow(10.0, ++counter));
				}
				else
				{
					// Синтаксическая ошибка
					object->get_session()->error = new ErrorCore("Invalid data format. No number after point", object->get_session());
					return;
				}
			}
			else res_ = res_ * 10 + (result.data[i] - (int)'0');
		}

		result.data = std::to_string(res_);
		break;
	}
	case TYPE_OF_DATA::_NONE:
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("null cannot be converted to double", object->get_session());
		return;
		break;
	}
	result.type_of_data = TYPE_OF_DATA::_DOUBLE;
	*res = result;
}

void __bool(SystemFunction *object)
// Преобразование к типу bool
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	switch (result.type_of_data)
	{
	case TYPE_OF_DATA::_DOUBLE:
		result.data = (atof(result.data.c_str()) == 0.0) ? "false" : "true";
		break;
	case TYPE_OF_DATA::_INT:
		result.data = (atoi(result.data.c_str()) == 0) ? "false" : "true";
		break;
	case TYPE_OF_DATA::_STRING:
	{
		if (result.data == "true" || atof(result.data.c_str()) == 0.0 || atoi(result.data.c_str()) == 0)
			result.data = "true";
		else
			result.data = "false";
		break;
	}
	case TYPE_OF_DATA::_NONE:
		// Синтаксическая ошибка
		object->get_session()->error = new ErrorCore("null cannot be converted to boolean", object->get_session());
		return;
		break;
	}
	result.type_of_data = TYPE_OF_DATA::_BOOLEAN;
	*res = result;
}

void __str(SystemFunction *object)
// Преобразование к типу string
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	result.type_of_data = TYPE_OF_DATA::_STRING;
	*res = result;
}

void typeof(SystemFunction *object)
// Получить тип данных объекта
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	std::string tmp_array[] = {"int", "string", "double", "boolean", "null"};

	result.data = (result.array.size() > 0 && result.ptr == nullptr) ? "array" : tmp_array[result.type_of_data];
	result.type_of_data = TYPE_OF_DATA::_STRING;
	*res = result;
}

//void mysql_connect(SystemFunction *object)
//// Соединение с базой данных MySQL
//// 4 параметра
//// [хост, имя базы, пользователь, пароль]
//{
//	Instruction *res = &object->get_result_instruction();
//	const std::vector<Instruction> &instructions = object->get_instructions();
//	Instruction result = instructions[0];
//
//	MySQL *new_mysql_connection = new MySQL();
//
//	mysql_init(&new_mysql_connection->mysql_init);
//	new_mysql_connection->connection = mysql_real_connect(&new_mysql_connection->mysql_init, instructions[0].data.c_str(), instructions[2].data.c_str(),
//		instructions[3].data.c_str(), instructions[1].data.c_str(), 3306, 0, 0);
//
//	// Если не удалось соединиться с базой данных
//	if (new_mysql_connection->connection == NULL)
//	{
//		object->get_session()->error = new ErrorCore(mysql_error(new_mysql_connection->connection), object->get_session());
//		return;
//	}
//
//	object->get_session()->mysql_connections.push_back(new_mysql_connection);
//
//	result.type_of_data = TYPE_OF_DATA::_INT;
//	result.data = std::to_string(object->get_session()->mysql_connections.size() - 1);
//
//	*res = result;
//}
//
//void mysql_close(SystemFunction *object)
//// Прерывает соединение с базой данных MySQL
//// 1 параметр
//// [id соединения]
//{
//	Instruction *res = &object->get_result_instruction();
//	const std::vector<Instruction> &instructions = object->get_instructions();
//	Instruction result = instructions[0];
//
//	MySQL *connection = object->get_session()->mysql_connections[atoi(instructions[0].data.c_str())];
//	mysql_close(connection->connection);
//
//	delete(object->get_session()->mysql_connections[atoi(instructions[0].data.c_str())]);
//
//	*res = result;
//}
//
//void mysql_query(SystemFunction *object)
//// Посылает запрос к базе данных
//// 2 параметра
//// [id соединения, запрос]
//{
//	Instruction *res = &object->get_result_instruction();
//	const std::vector<Instruction> &instructions = object->get_instructions();
//	Instruction result = instructions[0];
//
//	MySQL *connection = object->get_session()->mysql_connections[atoi(instructions[0].data.c_str())];
//	
//	int state_of_query = mysql_query(connection->connection, instructions[1].data.c_str());
//
//	// Если произошла ошибка в заросе
//	if (state_of_query != 0)
//	{
//		object->get_session()->error = new ErrorCore(mysql_error(connection->connection), object->get_session());
//		return;
//	}
//
//	MYSQL_RES *result_of_query = mysql_store_result(connection->connection);
//	if (result_of_query != NULL)
//	{
//		result.array.clear();
//
//		MYSQL_ROW row;
//		MYSQL_FIELD *fields = mysql_fetch_fields(result_of_query);
//		int num_of_index = -1;
//
//		while ((row = mysql_fetch_row(result_of_query)) != NULL)
//		{
//			num_of_index = -1;
//			Instruction inst;
//			inst.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
//			inst.type_of_data = TYPE_OF_DATA::_STRING;
//
//			for (register u_int i = 0; i < mysql_num_fields(result_of_query); i++)
//			{
//				Instruction new_instruction;
//				new_instruction.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
//
//				if (row[i] != NULL)
//				{
//					new_instruction.type_of_data = TYPE_OF_DATA::_STRING;
//					new_instruction.data = row[i];
//
//					inst.array_map[fields[++num_of_index].name].data = row[i];
//					inst.array_map[fields[num_of_index].name].type_of_data = TYPE_OF_DATA::_STRING;
//				}
//				else
//				{
//					new_instruction.type_of_data = TYPE_OF_DATA::_NONE;
//					new_instruction.data = "null";
//
//					inst.array_map[fields[++num_of_index].name].data = "null";
//					inst.array_map[fields[num_of_index].name].type_of_data = TYPE_OF_DATA::_STRING;
//				}
//
//				inst.array.push_back(new_instruction);
//			}
//			result.array.push_back(inst);
//		}
//	}
//	mysql_free_result(result_of_query);
//
//	*res = result;
//}

void include(SystemFunction *object)
// Переход на другой документ
// 1 параметр
// [id соединения]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];
	std::string file_name = result.data;

	int all_pages_size = all_pages.size();
	register unsigned int i = 0; // -> ID файла в векторе

	for (; i < all_pages_size; i++)
	{
		if (all_pages[i].getName() == file_name)
		{
			interpreter_start(object->get_session()->get_client_socket(), i, EMPTY, object->get_session());
			break;
		}
		else if (i == all_pages_size - 1)
		{
			object->get_session()->error = new ErrorCore("could not find this file", object->get_session());
			return;
		}
	}

	*res = result;
}

void isset(SystemFunction *object)
// Есть ли данные
// 1 параметр
// [даныне]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];
	
	result.type_of_data = TYPE_OF_DATA::_BOOLEAN;
	result.data = (result.data == "null" || result.data == "" || result.data.length() == 0) ? "false" : "true";

	*res = result;
}

void del_session(SystemFunction *object)
// Удаляет текущую сессию
// 0 параметров
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();

	object->get_session()->isSessionDelete = true;
	object->get_session()->all_data.clear();
	object->get_session()->definition_functions.clear();
}

void del(SystemFunction *object)
// Удаляет объект
// 1 параметр
// [объект]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	if (object->get_session()->all_data.find(result.body) == object->get_session()->all_data.end())
	{
		object->get_session()->error = new ErrorCore("the object was not found", object->get_session());
		return;
	}
	object->get_session()->all_data.erase(result.body);
	if(object->get_session()->all_data_buffer.find(result.body) != object->get_session()->all_data_buffer.end())
		object->get_session()->all_data_buffer.erase(result.body);

	*res = result;
}