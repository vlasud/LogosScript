#pragma once

class Session
{
	u_int start_line;
	u_int current_line;
	std::string file_name;
	SOCKET client_socket;
public:
	std::vector<LineInstructions> lines;
	// ��� ����������� �������
	std::map<std::string, FunctionDefinition> definition_functions;

	// ������������� ������ ���� ������ �������
	std::map<std::string, Instruction> all_data;
	// ������������� ������ ���� ������ ������� (������)
	std::map<std::string, Instruction> all_data_buffer;

	// ������ �������� MySQL ����������
	//std::vector<MySQL*> mysql_connections;

	// ��������� ���������� �������
	std::string last_command;
	bool last_command_success;
	//

	// ���� �� ������� ������� continue
	bool isContinue = false;
	// ���� �� ������� ������� break
	bool isBreak = false;
	// ���� �� ������� ������� �������� ������
	bool isSessionDelete = false;
	// ��������������� �� ������ �������� (������� redirect)
	std::string redirect_page = "";
	// ID ������
	std::string session_key = "";

	ErrorCore *error = nullptr;
	Output output;

	// ��������� ������� ���������� ������������ �������
	unsigned int start_level;

	// ������� ����������� �������
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