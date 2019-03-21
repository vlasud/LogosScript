

class ErrorCore
{
private:
	std::string error_text;
	unsigned int error_line;
public:

	ErrorCore(std::string error_text, unsigned int error_line);
	std::string get_error_text(void);
};