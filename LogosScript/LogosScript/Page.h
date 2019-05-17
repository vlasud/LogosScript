#pragma once


class Page
{
	// ��� �����
	std::string name;
	// ���������� �����
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

		get_type_of_file();
	}

	std::string getName(void)
		// ��������� ����� �����
	{
		return name;
	}

	std::string getType(void)
		// ��������� ���������� �����
	{
		return type_of_file;
	}
	
};

// ������ �������� ����������� ������ �����
extern std::vector<Page> all_pages;