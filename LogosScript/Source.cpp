#include "Global.h"

#define BOOST_ASIO_SEPARATE_COMPILATION

#define MAX_BUFFER_SIZE 1024
#define DEFAULT_SCRIPT_DIRECTORY "\\source"

#pragma comment(lib, "Ws2_32.lib")

std::vector<Page> all_pages;

const std::string ip = "127.0.0.1";
const char* port = "2020";

void client_handler(const SOCKET client_socket)
// ������� ��������� ����������� �������
{
	char buffer[MAX_BUFFER_SIZE];

	// ������ ����������� ������� �� ������
	int res = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);

	if (res != SOCKET_ERROR)
	{
		char* temp = new char[res];
		for (register unsigned short i = 0; i < res; i++)
			temp[i] = buffer[i];
		std::cout << temp << std::endl;
		// ����, ������� �������� ������
		std::string file_name = "/";
		bool isFound = false;

		for (register unsigned short i = 0; i < res && temp[i] != '\n'; i++)
		{
			if (temp[i] == '/')
			{
				for (register unsigned short j = i + 1; j < res && temp[j] != ' '; j++)
					file_name += temp[j];
				if (file_name != "")
					isFound = true;
			}
			if (isFound)
				break;
		}
		
		delete[] temp;

		std::string body = "";

		int all_pages_size = all_pages.size();
		register unsigned int i = 0; // -> ID ����� � �������

		for (; i < all_pages_size; i++)
		{
			if (all_pages[i].getName() == file_name)
			{
				int page_all_lines_size = all_pages[i].all_lines.size();
				for (register unsigned int j = 0; j < page_all_lines_size; j++)
					body += all_pages[i].all_lines[j];
				break;
			}
			else if (i == all_pages_size - 1)
			{
				std::string response = "HTTP/1.1 404 Not Found";

				send(client_socket, response.c_str(), response.length(), 0);
				closesocket(client_socket);
				return;
			}
		}

		// ���� ���� ����� ���������� html
		if (!all_pages[i].getType())
		{
			std::string header = "HTTP / 1.1 200 OK\r\nVersion: HTTP/1.1\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: " + std::to_string(body.length()) + "\r\n\r\n";
			std::string response = header + body;

			send(client_socket, response.c_str(), response.length(), 0);
			closesocket(client_socket);
		}
		// ���� ����� ��������� ������...
		else
		{
			// �������� ���������� ��������������
			interpreter_start(client_socket, i);
		}
	}
}

size_t getSize(const char *text)
// ����������� �������
{
	unsigned int counter = 0;
	while (text[counter] != '\0') counter++;
	return counter;
}

void loadPages(const std::string dir = "")
// ������� ��������� ��� ����� � ���
{
	WIN32_FIND_DATA winFileData;
	HANDLE hFile;
	char szPath[MAX_PATH];
	if (GetCurrentDirectory(sizeof(szPath), szPath))
	{
		lstrcat(szPath, (DEFAULT_SCRIPT_DIRECTORY + dir).c_str());
		lstrcat(szPath, "\\*.*");

		hFile = FindFirstFile(szPath, &winFileData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				size_t size = getSize(winFileData.cFileName);
				for (register unsigned int i = 0; i < size; i++)
				{
					if (winFileData.cFileName[0] == '.') break;
					if (winFileData.cFileName[i] == '.')
					{
						all_pages.push_back(Page{ dir + "/" + winFileData.cFileName });
						std::string s = dir + "/" + winFileData.cFileName;
						break;
					}
					else if (i == size - 1)
						loadPages(dir + "/" + winFileData.cFileName);
				}

			} while (FindNextFile(hFile, &winFileData) != 0);
			FindClose(hFile);
		}
	}
}

int main(int argc, char **argv[])
{
	setlocale(LC_NUMERIC, "C");

	std::cout << "IP: \t" << ip << std::endl;
	std::cout << "PORT: \t" << port << std::endl;

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	std::cout << "������������� ������� \t... ";

	//// ���� ����� ������ ���� - ��������
	//if (argc > 0)
	//	ip = argv[0][0];

	// ������������� ����������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR)
	{
		std::cout << "\t �� ������� ���������������� ����������" << std::endl;
		WSACleanup();
		system("pause >> VOID");
		return 1;
	}

	// ������������� �������� ��������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct addrinfo* addr;
	struct addrinfo hints;

	ZeroMemory(&addr, sizeof(addr));
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// ���� ������������� �� ������� - ������� ��������� �� ������
	if (getaddrinfo(ip.c_str(), port, &hints, &addr) == SOCKET_ERROR)
	{
		std::cout << "\t �� ������� ���������������� �������� ���������" << std::endl;
		WSACleanup();
		system("pause >> VOID");
		return 1;
	}

	// ������������� ��������� ������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	SOCKET server_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (server_socket == SOCKET_ERROR)
	{
		std::cout << "\t �� ������� ���������������� �������� �����" << std::endl;
		WSACleanup();
		freeaddrinfo(addr);
		system("pause >> VOID");
		return 1;
	}

	// �������� ��������� ������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if (bind(server_socket, addr->ai_addr, (int)addr->ai_addrlen) == SOCKET_ERROR)
	{
		std::cout << "\t �� ������� ��������� �������� �����" << std::endl;
		WSACleanup();
		freeaddrinfo(addr);
		system("pause >> VOID");
		return 1;
	}

	std::cout << "\tOK" << std::endl;
	std::cout << "�������� ������ \t... ";

	// ��������� �����
	loadPages();
	std::cout << "\tOK (" << all_pages.size() << ")" << std::endl;

	// ������ ��������� �������� TCP ����������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::cout << "������ ������� \t\t... ";
	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout <<  "\t �� ������� ������ ��������� �������� TCP ����������" << std::endl;
		WSACleanup();
		freeaddrinfo(addr);
		return 1;
	}

	std::cout << "\t������ ������� �������!" << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	// ������ ������ �������
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	while (true)
	{
		SOCKET client_socket = accept(server_socket, NULL, NULL);
		if (client_socket != INVALID_SOCKET)
		{
			// ������ ������ ��������� ����������� �������
			std::thread listen_client_thread(client_handler, client_socket);
			listen_client_thread.detach();
		}
	}

	system("pause >> VOID");
	return 0;
}