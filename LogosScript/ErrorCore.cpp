#include "Global.h"

ErrorCore::ErrorCore(std::string error_text, Session *session)
{
	this->error_text = "Syntax error: " + error_text + " on line " + std::to_string(session->get_current_line());
}

std::string ErrorCore::get_error_text(void)
{
	return this->error_text;
}