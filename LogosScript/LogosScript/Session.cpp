#include "Global.h"

Session::Session(void) {}

Session::Session(const u_int start_line, const SOCKET client_socket)
{
	this->start_line = start_line;
	this->client_socket = client_socket;
}

Session::~Session()
{
	delete this->error;
}

u_int Session::get_start_line(void)
{
	return this->start_line;
}
u_int Session::get_current_line(void)
{
	return this->current_line;
}

void Session::update_current_line(u_int new_line)
{
	this->current_line = new_line;
}

void Session::set_file_name(const std::string file_name)
{
	this->file_name = file_name;
}
std::string Session::get_file_name(void)
{
	return this->file_name;
}

SOCKET Session::get_client_socket(void)
{
	return this->client_socket;
}