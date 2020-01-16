#include "Global.h"

ErrorCore::ErrorCore(std::string error_text, Session *session)
{
	this->error_text = "Syntax error: " + error_text + " in file " + session->get_file_name() + " on line " 
		+ std::to_string((int)session->get_current_line() + 1) + "\0";
}

std::string ErrorCore::get_error_text(void)
{
	return this->error_text;
}