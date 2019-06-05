
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
	u_int get_count_of_params(void) const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Прототипы системных функций

// Вывод информации в документ
void write(SystemFunction *object);
// Вывод информации в документ с переходом на новую строку в конце
void writeln(SystemFunction *object);
// Длина строки или масива
void len(SystemFunction *object);
// Обрезать строку
void substr(SystemFunction *object);
// Удалить из массива/строки по индексу
void removeAt(SystemFunction *object);
// Добавить в массив/строку
void add(SystemFunction *object);
// Внедрить в массив/строку
void insert(SystemFunction *object);
// Заменить в массиве/строке
void replace(SystemFunction *object);
// Заменить в массиве/строке
void split(SystemFunction *object);
// Перевести все символы строки в нижний регистр
void down(SystemFunction *object);
// Перевести все символы строки в верхний
void top(SystemFunction *object);
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
 //Установить связь с базой данных
//void mysql_connect(SystemFunction *object);
//// Прерывает связь с базой данных
//void mysql_close(SystemFunction *object);
//// Посылает запрос к базе данных
//void mysql_query(SystemFunction *object);
//Переход на другой документ
void include(SystemFunction *object);
// Есть ли данные
void is_null(SystemFunction *object);
// Удаляет текущую сессию
void del_session(SystemFunction *object);
// Удаляет объект
void del(SystemFunction *object);
// Очищает массив
void clear_array(SystemFunction *object);
// Показывает всю информацию об объекте
void show_info(SystemFunction *object);
// Перенаправление на другую страницу
void redirect(SystemFunction *object);
// Округление вверх
void roundup(SystemFunction *object);
// Возвдедение в абсолютную велечину
void abs(SystemFunction *object);
// Создание файла
void create_file(SystemFunction *object);
// Удаление файла
void delete_file(SystemFunction *object);
// Проверка - есть ли поток данных у объекта
void is_stream(SystemFunction *object);
// Список файлов в директории
void files_list(SystemFunction *object);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const std::vector<SystemFunction> system_functions = 
{
	SystemFunction {"write", 1, write},
	SystemFunction {"writeln", 1, writeln},
	SystemFunction {"len", 1, len},
	SystemFunction {"substr", 3, substr},
	SystemFunction {"removeAt", 2, removeAt},
	SystemFunction {"add", 2, add},
	SystemFunction {"insert", 3, insert},
	SystemFunction {"replace", 3, replace},
	SystemFunction {"down", 1, down},
	SystemFunction {"top", 1, top},
	SystemFunction {"split", 2, split},
	SystemFunction {"find", 2, find},
	SystemFunction {"int", 1, __int},
	SystemFunction {"double", 1, __double },
	SystemFunction {"bool", 1, __bool},
	SystemFunction {"str", 1, __str},
	SystemFunction {"typeof", 1, typeof},
	/*SystemFunction {"connect", 4, mysql_connect},
	SystemFunction {"close", 1, mysql_close},
	SystemFunction {"query", 2, mysql_query},*/
	SystemFunction {"include", 1, include},
	SystemFunction {"is_null", 1, is_null},
	SystemFunction {"del_session", 0, del_session},
	SystemFunction {"del", 1, del},
	SystemFunction {"clear_array", 1, clear_array},
	SystemFunction {"show_info", 1, show_info},
	SystemFunction {"redirect", 1, redirect},
	SystemFunction {"roundup", 1, roundup},
	SystemFunction {"abs", 1, abs},
	SystemFunction {"create_file", 1, create_file},
	SystemFunction {"delete_file", 1, delete_file},
	SystemFunction {"is_stream", 1, is_stream},
	SystemFunction {"files_list", 1, files_list},
};