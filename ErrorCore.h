class Session;

class ErrorCore
{
private:
	std::string error_text;
	Session *session;
public:

	ErrorCore(std::string error_text, Session *session);
	std::string get_error_text(void);
};
