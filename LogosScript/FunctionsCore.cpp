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
		session->error = new ErrorCore("the function <" + this->get_name() + "> must take " + std::to_string(this->count_of_param) + " parameter", 0);
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

std::vector<Instruction> SystemFunction::get_instructions(void) const
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
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();
	
	res->type_of_data = TYPE_OF_DATA::_INT;
	res->data = (instructions[0].type_of_data == TYPE_OF_DATA::_STRING) ? std::to_string(instructions[0].data.length()) :
		std::to_string(instructions[0].array.size());
}

void write(SystemFunction *object)
// Вывод информации в документ
{
	Instruction *res = &object->get_result_instruction();
	const std::vector<Instruction> &instructions = object->get_instructions();

	object->get_session()->output.output_data.push_back(instructions[0].data);
}