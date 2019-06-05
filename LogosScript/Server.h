#pragma once


class Server
// Абстракция работы сервера в момент обработки клиенсткого запроса
{
	// Страница, которую запрашивает клиент
	Page page;

	// Дискриптор сокета клиента
	SOCKET socket;

	// Запрос клиента
	std::vector<char> request;
	// Хедеры и их значение в клиенстком запросе
	std::map<std::string, std::string> headers_map;
	// Строки хедеров в клиенстком запросе (для более удобного парсинга)
	std::vector<std::string> headers_lines;
	// Куки
	std::map<std::string, std::string> cookies;

	// Глобальные данные GET/POST
	Instruction global_data;
	// LOGOSID
	std::string logosid;

	// Номер первой строки после хедеров (начало данных, для удобного парсинга)
	u_int header_end_line;

	// Метод обработки ответа на запрос
	void response(void);
	// Парсинг клиенсткого запроса
	void parse_request(void);
	// Парсинг куки
	void parse_cookie(void);
	// Определение Content-Type клиенсткого запроса
	std::string get_content_type(void);
	// Определение куки с сессией
	std::string parse_logosid(void);
	// Парсинг глобальных данных GET/POST если Content-Type - multipart/form-data
	void parse_global_data_as_multipart(void);
	// Парсинг глобальных данных GET/POST если Content-Type - application
	void parse_global_data(void);
	
	// Дружественная функция. Проверяет содержит ли строка хедер для данных multipart/form-data
	friend bool is_this_line_is_header(Server&, const u_int);
	// Дружественная функция. Парсит из строки имена форм и имена файлов для данных multipart/form-data
	friend std::map<std::string, std::string> parse_names_from_line(Server&, const u_int);

public:

	// Конструктор
	Server(const Page &page, const std::vector<char> &request, const SOCKET &socket);
	// Получение глобальных данных сервера
	Instruction get_global_data(void) const;
	// Получение LOGOSID
	std::string get_logosid(void) const;
	// Установить новый LOGOSID
	void set_logosid(const std::string logosid);
};