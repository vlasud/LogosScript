#include "Global.h"

class SystemFunction
{
private:
	std::vector<Instruction> instructions_of_function;
	Instruction result_of_function;
	u_int count_of_param;
	std::string name_of_function;
	void(*ptr_on_function)(SystemFunction *);
	Session *session;
public:
	SystemFunction(std::string name_of_function, u_int count_of_param, void(*ptr_on_function)(SystemFunction *));
	std::string get_name(void) const;
	Instruction get_result(void) const;
	Instruction& get_result_instruction(void);
	std::vector<Instruction> get_instructions(void) const;
	Session *get_session(void);
	void start_function(Session *session);
	void set_params(Instruction &parametr);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Прототипы системных функций

// Вывод информации в документ
void write(SystemFunction *object);
// Длина строки или масива
void len(SystemFunction *object);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const std::vector<SystemFunction> system_functions = 
{
	SystemFunction {"write", 1, write},
	SystemFunction {"len", 1, len}
};