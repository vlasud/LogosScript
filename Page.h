#pragma once


class Page
{
	// Имя файла
	std::string name;
	// Расширение файла
	std::string type_of_file;

	void get_type_of_file(void)
	{
		for (register u_int i = name.length() - 1; i >= 0; i--)
		{
			if (name[i] == '.')
				break;
			type_of_file += name[i];
		}
		std::reverse(type_of_file.begin(), type_of_file.end());
	}

public:
	// Вектор строк файла
	std::vector<std::string> all_lines;

	Page(void) {

	}

	Page(const std::string path)
		// В конструкторе происходит загрузка всех строк файла в ОЗУ
	{

		std::ifstream stream("source" + path);
		std::string line;

		while (std::getline(stream, line))
			all_lines.push_back(line);

		stream.close();

		name = path;

		get_type_of_file();
	}

	std::string getName(void) const
		// Получение имени файла
	{
		return name;
	}

	std::string getType(void) const
		// Получение расширение файла
	{
		return type_of_file;
	}

	std::string get_body_as_text(void) const
	// Получение содержимое файла в виде текста
	{
		std::string body = "";
		for (register u_int i = 0; i < all_lines.size(); i++) {
			body += all_lines[i];
		}
		return body;
	}
	std::string get_body_as_bytes(void) const
		// Получение содержимое файла в виде байт
	{
		std::string body;
		std::vector<char> bytes;

		std::ifstream binary_file("Source" + this->name, std::ios_base::binary);
		if (binary_file){
			char ch;
			while (binary_file.get(ch)) {
				bytes.push_back(ch);
			}
		}
		binary_file.close();

		for (int z = 0; z < bytes.size(); z++) {
			body += bytes[z];
		}
		
		return body;
	}
	
};

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;