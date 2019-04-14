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

	std::string getName(void)
		// Получение имени файла
	{
		return name;
	}

	std::string getType(void)
		// Получение расширение файла
	{
		return type_of_file;
	}
	
};

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;