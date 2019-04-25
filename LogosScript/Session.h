#pragma once

class Session
{
	u_int start_line;
	u_int current_line;
	std::string file_name;
	SOCKET client_socket;
public:
	std::vector<LineInstructions> lines;
	// Хэш обьявленных функций
	std::map<std::string, FunctionDefinition> definition_functions;

	// Ассоциативный массив всех данных скрипта
	std::map<std::string, Instruction> all_data;
	// Ассоциативный массив всех данных скрипта (буффер)
	std::map<std::string, Instruction> all_data_buffer;

	// Вектор активных MySQL соединений
	//std::vector<MySQL*> mysql_connections;

	// Последняя выполненая команда
	std::string last_command;
	bool last_command_success;
	//

	// Была ли вызвана команда continue
	bool isContinue = false;
	// Была ли вызвана команда break
	bool isBreak = false;
	// Была ли вызвана команда удаления сессии
	bool isSessionDelete = false;
	// Перенаправление на другую страницу (команда redirect)
	std::string redirect_page = "";
	// ID сессии
	std::string session_key = "";

	ErrorCore *error = nullptr;
	Output output;

	// Начальный уровень локального пространства скрипта
	unsigned int start_level;

	// Текущая выполняемая функция
	FunctionDefinition *current_function = nullptr;

	Session(void);
	Session(const u_int start_line, const SOCKET client_socket);
	~Session();

	u_int get_start_line(void);
	u_int get_current_line(void);
	void update_current_line(u_int new_line);
	void set_file_name(const std::string file_name);
	std::string get_file_name(void);
	SOCKET get_client_socket(void);
};