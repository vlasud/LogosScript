#include "Global.h"

Server::Server(const Page &page, const std::vector<char> &request, const SOCKET &socket) {
	this->request	= request;
	this->page		= page;
	this->socket	= socket;

	response();
}

void Server::response(void) {

	parse_request();

	std::string header	= EMPTY;
	std::string body	= EMPTY;

	header += "HTTP/1.1 200 OK\r\n";
	header += "Server: LogosServer / " + LOGOS_VERSION + "(Win64)\r\n";
	header += "X-Powered-By: Logos / " + LOGOS_VERSION + "\r\n";
	header += "Keep-Alive: timeout=5, max=86\r\n";
	header += "Cache-Control: no-cache; no-store; must-revalidate\r\n";
	header += "Connection: Keep-Alive\r\n";
	header += "Content-Language: ru-RU\r\n";

	if (this->page.getType() == "html") {
		body	= page.get_body_as_text();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: text/html; charset=UTF-8\r\n";
	}
	else if (this->page.getType() == "png") {
		body	= page.get_body_as_bytes();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: image/png";
	}
	else if (this->page.getType() == "jpeg" || this->page.getType() == "jpg") {
		body	= page.get_body_as_bytes();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: image/jpg";
	}
	else if (this->page.getType() == "css") {
		body	= page.get_body_as_text();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: text/css; charset=UTF-8\r\n";
	}
	else if (this->page.getType() == "js") {
		body	= page.get_body_as_bytes();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: application/javascript";
	}
	else if (this->page.getType() == "ttf") {
		body	= page.get_body_as_bytes();
		header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
		header	+= "Content-Type: text/ttf";
	}
	else if (this->page.getType() == "lgs") {
		std::string content_type = get_content_type();

		// Если значение хедера Content-Type в первых трех символах содержит "mul"
		if (content_type != "none" && content_type[0] == 'm' && content_type[1] == 'u' && content_type[2] == 'l') {
			parse_global_data_as_multipart();
		}
		else {
			parse_global_data();
		}
		
		// Парсинг куки

		parse_cookie();
		// Парсинг LOGOSID
		this->logosid = parse_logosid();

		// Вызов интерпретатора
		// Возвращает [\x32\x32] в случае если интерпретатор выполнил работу и сервер должен вернуть результат его работы
		// Иначе интерпретатор запросил переадресацию и вернул имя документа на который нужно перейти 
		std::string logos_result = interpreter_start(this->socket, this->page, this);

		if ("[\x32\x32]" == logos_result){
			body	= page.get_body_as_text();
			header	+= "Content-Length: " + std::to_string(body.length()) + "\r\n";
			header	+= "Content-Type: text/html; charset=UTF-8\r\n";
		}
		else {
			header = "HTTP/1.1 302 OK\r\n";
			header += "Server: LogosServer / " + LOGOS_VERSION + "(Win64)\r\n";
			header += "X-Powered-By: Logos / " + LOGOS_VERSION + "\r\n";
			header += "Location: " + logos_result + "\r\n";
		}
		header += "Set-Cookie: LOGOSID=" + this->logosid + "; expires=Fri, 31 Dec 2050 23:59:59 GMT;";
	}

	header += "\r\n\r\n";

	header += body;

	send(socket, header.c_str(), header.length(), 0);
}

void Server::parse_request(void) {

	std::string key, value = EMPTY;
	bool switcher = false;
	this->header_end_line = 0;

	// Парсинг строк
	for (register u_int i = 0; i < this->request.size(); i++) {
		if (this->request[i] == '\n' || i == this->request.size() - 1) {
			if (this->request[i] == '\n' && i >= this->header_end_line && this->header_end_line != 0) {
				value += '\n';
			}
			if ((value == EMPTY || value == "\r") && this->header_end_line == 0) {
				this->header_end_line = this->headers_lines.size();
			}
			this->headers_lines.push_back(value);
			value = EMPTY;
		}
		else value += this->request[i];
	}
	value = EMPTY;

	// Парсинг хедеров и их значение
	for (register u_int i = 1; i < this->header_end_line; i++) {
		for (register u_int j = 0; j < this->headers_lines[i].size(); j++) {
			if (this->headers_lines[i][j] == ':') {
				switcher = true;
			}
			else if(this->headers_lines[i][j] != '\r' && this->headers_lines[i][j] != ' ') {
				if (!switcher) key += this->headers_lines[i][j];
				else value += this->headers_lines[i][j];
			}
			if (j == this->headers_lines[i].size() - 1) {
				this->headers_map[key] = value;
				key = EMPTY;
				value = EMPTY;
				switcher = false;
			}
		}
	}
}

std::string Server::get_content_type(void){
	auto content_type = this->headers_map.find("Content-Type");
	if (content_type != this->headers_map.end()) {
		return content_type->second;
	}
	else {
		return "none";
	}
}

std::string Server::parse_logosid(void) {
	auto logosid = this->cookies.find("LOGOSID");
	if (logosid != this->cookies.end() && logosid->second.length() == 20) {
		return logosid->second;
	}
	else {
		return EMPTY;
	}
}

void Server::parse_cookie(void) {
	auto cookie = this->headers_map.find("Cookie");
	if (cookie != this->headers_map.end()) {
		std::string key, value = EMPTY;
		bool switcher = false;
		for (register u_int i = 0; i < cookie->second.length(); i++) {
			if (cookie->second[i] == '=') {
				switcher = true;
			}
			else if (cookie->second[i] == ';') {
				this->cookies[key] = value;
				key = value = EMPTY;
				switcher = false;
			}
			else {
				if (!switcher) key += cookie->second[i];
				else value += cookie->second[i];

				if (i == cookie->second.length() - 1) {
					this->cookies[key] = value;
				}
			}
		}

	}
}

bool is_this_line_is_header(Server &server, const u_int line) {
	const std::string header = "------WebKitFormBoundary";

	for (register u_int j = 0; j < header.length() && j < server.headers_lines[line].length(); j++) {
		if (server.headers_lines[line][j] != header[j]) {
			return false;
		}
		else if (j == header.length() - 1) {
			return true;
		}
	}
}

std::map<std::string, std::string> parse_names_from_line(Server &server, const u_int line) {
	std::map<std::string, std::string> result;
	std::string key, value = EMPTY;
	bool switcher = false;
	for (register u_int i = 0; i < server.headers_lines[line].length(); i++) {
		if (server.headers_lines[line][i] == '=') {
			switcher = true;
		}
		else if (server.headers_lines[line][i] == ';') {
			for (register u_int b = 0; b < value.length(); b++) {
				if (value[b] == '\r' || value[b] == '"') {
					value.erase(value.begin() + b);
					b--;
				}
			}
			while (key[0] == ' ') {
				key.erase(key.begin());
			}  
			result[key] = value;
			key = value = EMPTY;
			switcher = false;
		}
		else {
			if (!switcher) key += server.headers_lines[line][i];
			else value += server.headers_lines[line][i];

			if (i == server.headers_lines[line].length() - 1) {
				for (register u_int b = 0; b < value.length(); b++) {
					if (value[b] == '\r' || value[b] == '"') {
						value.erase(value.begin() + b);
						b--;
					}
				}
				while (key[0] == ' ') {
					key.erase(key.begin());
				}
				result[key] = value;
			}
		}
	}
	return result;
}

void Server::parse_global_data_as_multipart(void) { 

	Instruction result;
	result.type_of_data			= TYPE_OF_DATA::_STRING;
	result.type_of_instruction	= TYPE_OF_INSTRUCTION::DATA;
	result.isVariable			= true;
	result.body					= "_post";

	std::vector<char> bytes_data;
	std::string data = EMPTY;
 	std::string key = EMPTY;
	bool switcher = false;
	bool isHeader = false;
	bool isFile = false;

	// Если первая строка заголовок - пропуск
	// Парсинг пока не встретится линия с \r\n
	// Парсинг

	for (register u_int i = this->header_end_line + 1; i < this->headers_lines.size(); ) {
		if (is_this_line_is_header(*this, i)) {
			if (++i >= this->headers_lines.size()) break;

			std::map<std::string, std::string> names = parse_names_from_line(*this, i);
			key = names["name"];

			isFile = (names.find("filename") != names.end());
						
			while (this->headers_lines[i][0] != '\r' && this->headers_lines[i][1] != '\n') {
				i++;
			}
			i++;
			bytes_data.clear();
			while (i < this->headers_lines.size() && !is_this_line_is_header(*this, i)) {
				if (isFile) {
					for (register u_int p = 0; p < this->headers_lines[i].length(); p++) {
						bytes_data.push_back(this->headers_lines[i][p]);
					}
				}
				else {
					data += this->headers_lines[i];
				}
				i++;
			}

			
			if (data.length() > 0 && (data[data.length() - 1] == '\r' || data[data.length() - 1] == '\n')) {
				data.erase(data.end() - 1);
			}
			if (data.length() > 0 && (data[data.length() - 1] == '\r' || data[data.length() - 1] == '\n')) {
				data.erase(data.end() - 1);
			}
			if (key[key.length() - 1] == '\r' || key[key.length() - 1] == '\n') {
				key.erase(key.end() - 1);
			}

			Instruction tmp;
			tmp.selected_char = -1;
			tmp.type_of_data = TYPE_OF_DATA::_STRING;
			tmp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
			if (isFile) {
				tmp.data = names.find("filename")->second;
				if (tmp.data.length() > 0 && (tmp.data[tmp.data.length() - 1] == '\r' || tmp.data[tmp.data.length() - 1] == '\n')) {
					tmp.data.erase(tmp.data.end() - 1);
				}
				if (bytes_data[bytes_data.size() - 1] == '\r') {
					bytes_data.pop_back();
				}
				if (bytes_data[bytes_data.size() - 2] == '\r' && bytes_data[bytes_data.size() - 1] == '\n') {
					bytes_data.pop_back();
					bytes_data.pop_back();
				}
				tmp.stream = bytes_data;
			}
			else{
				tmp.data = data;
			}
			if (result.array_map.find(key) != result.array_map.end()) {
				if (result.array_map[key].array.size() == 0) {
					result.array_map[key].array.push_back(result.array_map[key]);
				}
				result.array_map[key].array.push_back(tmp);
			}
			else {
				result.array_map[key] = tmp;
			}

			key = EMPTY;
			data = EMPTY;
		}
		else {
			i++;
		}
	}
	this->global_data = result;
}

void Server::parse_global_data(void) {

	// TODO: Оптимизировать

	Instruction result;
	result.type_of_data			= TYPE_OF_DATA::_STRING;
	result.type_of_instruction	= TYPE_OF_INSTRUCTION::DATA;
	result.isVariable			= true;

	std::string data			= EMPTY;
	std::string key				= EMPTY;

	for (register u_int i = 0; i < request.size() && request[i] != ' '; i++)
		data += request[i];

	if (data == "GET")
	{
		result.body = "_get";
		data = EMPTY;

		for (register u_int i = 0; i < this->request.size(); i++)
		{
			if (this->request[i] == '?')
			{
				for (register u_int j = i + 1; j < this->request.size(); j++)
				{
					if (this->request[j] == '=')
					{
						key = data;
						data = EMPTY;
					}
					else if (this->request[j] == '&' || this->request[j] == ' ')
					{
						Instruction tmp;
						tmp.selected_char = -1;
						tmp.type_of_data = TYPE_OF_DATA::_STRING;
						tmp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
						tmp.data = data;

						result.array_map[key] = tmp;

						key = EMPTY;
						data = EMPTY;

						if (this->request[j] == ' ') break;
					}
					else data += this->request[j];
				}
				break;
			}
		}
	}
	else if (data == "POST")
	{
		result.body = "_post";
		data = EMPTY;

		for (register u_int j = 0; j < this->headers_lines[this->headers_lines.size() - 1].size(); j++)
		{
			if (this->headers_lines[this->headers_lines.size() - 1][j] == '=')
			{
				key = data;
				data = EMPTY;
			}
			else if (this->headers_lines[this->headers_lines.size() - 1][j] == '&'
				|| this->headers_lines[this->headers_lines.size() - 1][j] == '\0'
				|| j == this->headers_lines[this->headers_lines.size() - 1].length() - 1)
			{
				Instruction tmp;
				tmp.selected_char = -1;
				tmp.type_of_data = TYPE_OF_DATA::_STRING;
				tmp.type_of_instruction = TYPE_OF_INSTRUCTION::DATA;
				tmp.data = data;

				result.array_map[key] = tmp;

				key = EMPTY;
				data = EMPTY;

				if (this->headers_lines[this->headers_lines.size() - 1][j] == '\0') break;
			}
			else data += this->headers_lines[this->headers_lines.size() - 1][j];
		}

	}
	this->global_data = result;

}

Instruction Server::get_global_data(void) const{
	return this->global_data;
}
std::string Server::get_logosid(void) const {
	return this->logosid;
}

void Server::set_logosid(const std::string logosid) {
	this->logosid = logosid;
}