#pragma once


class Page
{
	// ��� �����
	std::string name;
	// ��� �����
	bool isScriptFile;

public:
	// ������ ����� �����
	std::vector<std::string> all_lines;

	Page(const std::string path)
		// � ������������ ���������� �������� ���� ����� ����� � ���
	{

		std::ifstream stream("source" + path);
		std::string line;

		while (std::getline(stream, line))
			all_lines.push_back(line);

		stream.close();

		name = path;

		// ����������� ���� ����� (������� html ��� ����������)
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		register unsigned int size = path.length() - 1;
		isScriptFile = path[size] == 's' && path[size - 1] == 'g' && path[size - 2] == 'l';
	}

	std::string getName(void)
		// ��������� ����� �����
	{
		return name;
	}

	bool getType(void)
		// ��������� ���� �����
	{
		return isScriptFile;
	}
};

// ������ �������� ����������� ������ �����
extern std::vector<Page> all_pages;