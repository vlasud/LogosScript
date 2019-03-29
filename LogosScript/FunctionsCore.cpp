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

void get_type(SystemFunction *object)
// Получить тип данных объекта
// 1 параметр
// [данные]
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	Instruction result = instructions[0];

	std::string tmp_array[] = {"int", "string", "double", "boolean", "null"};

	result.data = (result.array.size() > 0) ? "array" : tmp_array[result.type_of_data];
	result.type_of_data = TYPE_OF_DATA::_STRING;
	*res = result;
}