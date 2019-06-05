
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
// ��������� ��������� �������

// ����� ���������� � ��������
void write(SystemFunction *object);
// ����� ���������� � �������� � ��������� �� ����� ������ � �����
void writeln(SystemFunction *object);
// ����� ������ ��� ������
void len(SystemFunction *object);
// �������� ������
void substr(SystemFunction *object);
// ������� �� �������/������ �� �������
void removeAt(SystemFunction *object);
// �������� � ������/������
void add(SystemFunction *object);
// �������� � ������/������
void insert(SystemFunction *object);
// �������� � �������/������
void replace(SystemFunction *object);
// �������� � �������/������
void split(SystemFunction *object);
// ��������� ��� ������� ������ � ������ �������
void down(SystemFunction *object);
// ��������� ��� ������� ������ � �������
void top(SystemFunction *object);
// ���������� ������ ������� ��������� �������
void find(SystemFunction *object);
// ������������� � ���� int
void __int(SystemFunction *object);
// ������������� � ���� double
void __double(SystemFunction *object);
// ������������� � ���� bool
void __bool(SystemFunction *object);
// ������������� � ���� string
void __str(SystemFunction *object);
// �������� ��� ������ �������
void typeof(SystemFunction *object);
 //���������� ����� � ����� ������
//void mysql_connect(SystemFunction *object);
//// ��������� ����� � ����� ������
//void mysql_close(SystemFunction *object);
//// �������� ������ � ���� ������
//void mysql_query(SystemFunction *object);
//������� �� ������ ��������
void include(SystemFunction *object);
// ���� �� ������
void is_null(SystemFunction *object);
// ������� ������� ������
void del_session(SystemFunction *object);
// ������� ������
void del(SystemFunction *object);
// ������� ������
void clear_array(SystemFunction *object);
// ���������� ��� ���������� �� �������
void show_info(SystemFunction *object);
// ��������������� �� ������ ��������
void redirect(SystemFunction *object);
// ���������� �����
void roundup(SystemFunction *object);
// ����������� � ���������� ��������
void abs(SystemFunction *object);
// �������� �����
void create_file(SystemFunction *object);
// �������� �����
void delete_file(SystemFunction *object);
// �������� - ���� �� ����� ������ � �������
void is_stream(SystemFunction *object);
// ������ ������ � ����������
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