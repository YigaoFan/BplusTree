#pragma once
#include <map>
#include <string>
#include <functional>
#include <string_view>
#include <ncurses.h> // 这里面有些宏或者其他东西会影响 asio 的代码

namespace Cmd
{
	using ::std::function;
	using ::std::map;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;

	class CmdUI
	{
	private:
		bool _terminated = true;
		string _title;
		string _currentCmdLine;
		string _hintLine;
		int _colOffset = 0; // 相对于行尾来说的
		// keyword,          add to history,     current cmd line, hint line
		map<string, function<void(function<void(string)>, string*, string*, int*)>> _actions;
	public:
		CmdUI(string title) : _title(move(title))
		{

		}

		void Init()
		{
			initscr();
			noecho();
			ShowTitle();
			_terminated = false;
		}

		void RegisterAction(string key, function<void(function<void(string)>, string*, string*, int*)> callback)
		{
			_actions.insert({ move(key), move(callback) });
		}

		/// Keydown drive the run
		void Runloop()
		{
			Draw();

			if (_terminated)
			{
				return;
			}

			auto c = getch();
			string key(1, c);

			// TODO addToHistory
			auto addToHistory = [](string message)
			{

			};

			if (_actions.contains(key))
			{
				_actions[key](addToHistory, &_currentCmdLine, &_hintLine, &_colOffset);
			}
			else
			{
				_currentCmdLine.insert(_currentCmdLine.length() + _colOffset, 1, c);
				for (auto& x : _actions)
				{
					if (string_view(_currentCmdLine).substr(0, _currentCmdLine.length() + _colOffset).ends_with(x.first))
					{
						auto eraseLen = x.first.length();
						_currentCmdLine.erase(_currentCmdLine.length() + _colOffset - eraseLen, eraseLen);
						x.second(addToHistory, &_currentCmdLine, &_hintLine, &_colOffset);
						break;
					}
				}
			}

			// 有的按键按一次会有多个字符，下面处理这种情况

			// 有些键没有注册 action，由于 push_back 进去了就默认把它打出来
			Runloop();
		}

		void TerminateOnNextRun()
		{
			_terminated = true;
		}

		~CmdUI()
		{
			endwin();
		}

	private:
		void ShowTitle()
		{
			int row, col;
			getmaxyx(stdscr, row, col);
			mvprintw(0, (col - _title.length()) / 2, "%s", _title.c_str());
			printw("\n");
		}

		void Draw()
		{
			// Draw history

			// Draw current cmd line
			auto row = 1;
			wmove(stdscr, row, 0);
			printw(">> %s", _currentCmdLine.c_str());
			int c = getcurx(stdscr);
			int r = getcury(stdscr);
			printw("\n");
			// Draw hint
			wmove(stdscr, row + 1, 0);
			printw("%s\n", _hintLine.c_str());
			// printw("debug %d\n", _colOffset);

			if (_colOffset < -static_cast<int>(_currentCmdLine.length()))
			{
				_colOffset = -_currentCmdLine.length();
			}
			else if (_colOffset > 0)
			{
				_colOffset = 0;
			}
			
			wmove(stdscr, r, c + _colOffset);
		}
	};
}
