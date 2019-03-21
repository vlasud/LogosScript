#include "Global.h"

ErrorCore::ErrorCore(std::string error_text, unsigned int error_line)
{
	this->error_line = error_line;
	this->error_text = "Syntax error: " + error_text + " on line " + std::to_string(error_line);
}

std::string ErrorCore::get_error_text(void)
{
	return this->error_text;
}