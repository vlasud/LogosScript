#pragma once


class Page
{
	// Имя файла
	std::string name;
	// Тип файла
	bool isScriptFile;

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

		// Определение типа файла (простой html или скриптовый)
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		register unsigned int size = path.length() - 1;
		isScriptFile = path[size] == 's' && path[size - 1] == 'g' && path[size - 2] == 'l';
	}

	std::string getName(void)
		// Получение имени файла
	{
		return name;
	}

	bool getType(void)
		// Получение типа файла
	{
		return isScriptFile;
	}
};

// Вектор названий загруженных файлов сайта
extern std::vector<Page> all_pages;