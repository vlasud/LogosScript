#pragma once


class Server
// ���������� ������ ������� � ������ ��������� ����������� �������
{
	// ��������, ������� ����������� ������
	Page page;

	// ���������� ������ �������
	SOCKET socket;

	// ������ �������
	std::vector<char> request;
	// ������ � �� �������� � ���������� �������
	std::map<std::string, std::string> headers_map;
	// ������ ������� � ���������� ������� (��� ����� �������� ��������)
	std::vector<std::string> headers_lines;
	// ����
	std::map<std::string, std::string> cookies;

	// ���������� ������ GET/POST
	Instruction global_data;
	// LOGOSID
	std::string logosid;

	// ����� ������ ������ ����� ������� (������ ������, ��� �������� ��������)
	u_int header_end_line;

	// ����� ��������� ������ �� ������
	void response(void);
	// ������� ����������� �������
	void parse_request(void);
	// ������� ����
	void parse_cookie(void);
	// ����������� Content-Type ����������� �������
	std::string get_content_type(void);
	// ����������� ���� � �������
	std::string parse_logosid(void);
	// ������� ���������� ������ GET/POST ���� Content-Type - multipart/form-data
	void parse_global_data_as_multipart(void);
	// ������� ���������� ������ GET/POST ���� Content-Type - application
	void parse_global_data(void);
	
	// ������������� �������. ��������� �������� �� ������ ����� ��� ������ multipart/form-data
	friend bool is_this_line_is_header(Server&, const u_int);
	// ������������� �������. ������ �� ������ ����� ���� � ����� ������ ��� ������ multipart/form-data
	friend std::map<std::string, std::string> parse_names_from_line(Server&, const u_int);

public:

	// �����������
	Server(const Page &page, const std::vector<char> &request, const SOCKET &socket);
	// ��������� ���������� ������ �������
	Instruction get_global_data(void) const;
	// ��������� LOGOSID
	std::string get_logosid(void) const;
	// ���������� ����� LOGOSID
	void set_logosid(const std::string logosid);
};