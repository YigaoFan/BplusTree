#pragma once
#include <map>
#include <string>
#include <vector>
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
	using ::std::vector;

	class CmdUI
	{
	private:
		bool _terminated = true;
		string _title;
		string _currentCmdLine;
		string _hintLine;
		vector<string> _history;
		int _colOffset = 0; // 相对于行尾来说的
		size_t _startShowLineNum = 0;
		// keyword,          add to history,     current cmd line, hint line
		map<string, function<void(function<void(string)>, string*, string*, int*, size_t*)>> _actions;
	public:
		CmdUI(string title) : _title(move(title))
		{

		}

		void Init()
		{
			initscr();
			// cbreak();
			noecho();
			ShowTitle();
			_terminated = false;
		}

		void RegisterAction(string key, function<void(function<void(string)>, string*, string*, int*, size_t*)> callback)
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

			auto addToHistory = [this](string message)
			{
				_history.push_back(std::to_string(_history.size()) + ": " + move(message));
			};

			if (_actions.contains(key))
			{
				_actions[key](addToHistory, &_currentCmdLine, &_hintLine, &_colOffset, &_startShowLineNum);
			}
			else
			{
				// 有的按键按一次会有多个字符，下面处理这种情况
				// 有些键没有注册 action，由于 push_back 进去了就默认把它打出来
				_currentCmdLine.insert(_currentCmdLine.length() + _colOffset, 1, c);
				for (auto const& x : _actions)
				{
					if (string_view(_currentCmdLine).substr(0, _currentCmdLine.length() + _colOffset).ends_with(x.first))
					{
						auto eraseLen = x.first.length();
						_currentCmdLine.erase(_currentCmdLine.length() + _colOffset - eraseLen, eraseLen);
						x.second(addToHistory, &_currentCmdLine, &_hintLine, &_colOffset, &_startShowLineNum);
						break;
					}
				}
			}

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
			int const row = 1;
			wmove(stdscr, row, 0);
			clrtobot();

			size_t maxRowCount = static_cast<size_t>(getmaxy(stdscr)) + 1;
			size_t showHistoryCount = maxRowCount - 3; // 3 is title, current cmd line, hint line
			if (_startShowLineNum > _history.size())
			{
				_startShowLineNum = _history.size();
			}

			for (size_t i = 0; auto const& h : _history)
			{
				if (i >= _startShowLineNum and showHistoryCount > 0)
				{
					printw("%s\n", h.c_str());
					--showHistoryCount;
				}
				++i;
			}

			// Draw current cmd line
			printw(">> %s", _currentCmdLine.c_str());
			int c = getcurx(stdscr);
			int r = getcury(stdscr);
			printw("\n");
			// Draw hint line
			printw("%s\n", _hintLine.c_str());

			// handle cursor position
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
