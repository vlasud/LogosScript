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
	const std::vector<Instruction> get_instructions(void);
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
// Обрезать строку
void substr(SystemFunction *object);
// Удалить из массива/строки
void removeAt(SystemFunction *object);
// Добавить в массив/строку
void add(SystemFunction *object);
// Внедрить в массив/строку
void insert(SystemFunction *object);
// Заменить в массиве/строке
void replace(SystemFunction *object);
// Разбить строку на массив
void split(SystemFunction *object);
// Возвращает индекс первого вхождения объекта
void find(SystemFunction *object);
// Преобразовать к типу int
void __int(SystemFunction *object);
// Преобразовать к типу double
void __double(SystemFunction *object);
// Преобразовать к типу bool
void __bool(SystemFunction *object);
// Преобразовать к типу string
void __str(SystemFunction *object);
// Получить тип данных объекта
void typeof(SystemFunction *object);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const std::vector<SystemFunction> system_functions = 
{
	SystemFunction {"write", 1, write},
	SystemFunction {"len", 1, len},
	SystemFunction {"substr", 3, substr},
	SystemFunction {"removeAt", 2, removeAt},
	SystemFunction {"add", 2, add},
	SystemFunction {"insert", 3, insert},
	SystemFunction {"replace", 3, replace},
	SystemFunction {"split", 2, split},
	SystemFunction {"find", 2, find},
	SystemFunction {"int", 1, __int},
	SystemFunction {"double", 1, __double },
	SystemFunction {"bool", 1, __bool},
	SystemFunction {"str", 1, __str},
	SystemFunction {"typeof", 1, typeof},
};