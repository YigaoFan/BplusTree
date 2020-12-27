#pragma once
#include <string>
#include <string_view>
#include <asio.hpp>
#include "Cmder.hpp"
#include <ncurses.h> // 这里面有些宏或者其他东西会影响 asio 的代码，所以影响了这里的 include 顺序
// #include <cstdio>

int UI_Main()
{
	using ::asio::ip::tcp;
	using Cmd::Cmder;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;

	asio::io_context io;
	// tcp::resolver resolver(io);
	// auto endPoints = resolver.resolve("localhost", "daytime");

	tcp::socket socket(io);
	// asio::connect(socket, endPoints);

	auto cmd = Cmder::NewFrom(move(socket));
	initscr();

	{
		int row, col;
		getmaxyx(stdscr, row, col);
		string_view weclome = "Welcome to use Fan's cmd to control server";
		mvprintw(0, (col - weclome.length()) / 2, "%s", weclome);
		printw("\n");
	}

	printw(">>");
	noecho();
	// echo();

	string currentCmdLine;
	int currentRow = 1;

	while (true)
	{
		auto c = getch();
		switch (c)
		{
		case '\t':
			{
				auto lastWordPos = currentCmdLine.find_last_of(' ');
				string lastWord;
				if (lastWordPos == string::npos)
				{
					lastWord = currentCmdLine;
				}
				else
				{
					lastWord = currentCmdLine.substr(lastWordPos + 1);
				}

				auto opts = cmd.Complete(lastWord);
				int col = getcurx(stdscr);
				int row = getcury(stdscr);
				addstr("\n");

				for (auto& op : opts)
				{
					printw("%s ", op.c_str());
				}
				addstr("\n");
				wmove(stdscr, row, col);

				break;
			}
		
		case 127: // Delete key on Mac, backspace is '\b'
			currentCmdLine.pop_back();
			wmove(stdscr, currentRow, 0);
			clrtoeol();
			addstr(">>");
			addstr(currentCmdLine.c_str());
			break;

		default:
			currentCmdLine.push_back(c);
			addch(c); // 这个用法是正确的吗
			// printw("%d", c);
		}
		
		// refresh(); 好像不用
	}

	endwin();
}