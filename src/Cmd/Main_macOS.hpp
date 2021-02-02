#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <string_view>
#include <asio.hpp>
#include "CmdUI.hpp"
#include "Cmder.hpp"

int UI_Main()
{
	using ::asio::ip::tcp;
	using Cmd::Cmder;
	using Cmd::CmdUI;
	using ::std::back_inserter;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;
	using ::std::vector;

	asio::io_context io;
	// tcp::resolver resolver(io);
	// auto endPoints = resolver.resolve("localhost", "daytime");

	tcp::socket socket(io);
	// asio::connect(socket, endPoints);

	auto cmder = Cmder::NewFrom(move(socket));

	auto title = "Welcome to use Fan's cmd to control server(Press Up key can view history)";
	auto ui = CmdUI(title);
	ui.Init();
	ui.RegisterAction("\t", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		// 只需要看光标之前的部分
		auto offset = *colOffsetPtr;
		auto searchRange = string_view(*currentCmdLine).substr(0, currentCmdLine->length() + offset);
		auto lastWordPos = searchRange.find_last_of(' ');
		string_view lastWord;
		if (lastWordPos == string::npos)
		{
			lastWord = move(searchRange);
		}
		else
		{
			lastWord = searchRange.substr(lastWordPos + 1);
		}

		hintLine->clear();
		auto opts = cmder.Complete(lastWord);
		if (opts.size() == 1)
		{
			// 这些字符串操作还是太麻烦了，看怎么简化下 TODO
			auto eraseLen = lastWord.length();
			// 下面这个怎么运作的，因为长度发生了变化
			// 因为光标相对行尾的 offset 是对的，所以插入进去也对了
			currentCmdLine->erase(currentCmdLine->length() + offset - eraseLen, eraseLen);
			currentCmdLine->insert(currentCmdLine->length() + offset, opts[0]);
		}
		else
		{
			for (auto& op : opts)
			{
				hintLine->append(move(op));
				hintLine->push_back(' ');
			}
		}
	});

	// Enter key on Mac
	// lambda capture 里边可以指定类型吗？比如下面的 n
	ui.RegisterAction("\x0a", [&, n = 0](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr) mutable
	{
		auto& cmd = *currentCmdLine;

		if (cmd.empty())
		{
			return;
		}

		if (cmd == "exit")
		{
			ui.TerminateOnNextRun();
		}
		else
		{
			history->push_back(std::to_string(n++) + ": " + string(">> ") + cmd);
			currentCmdLine->clear();
			hintLine->clear();
			auto& showStart = *startShowLineNumPtr;
			if ((history->size() - showStart) > ui.MaxUsableHeight())
			{
				showStart = history->size() - ui.MaxUsableHeight();
			}

			currentCmdLine->clear();
			hintLine->clear();
			
			try
			{
				auto result = cmder.Run(*currentCmdLine);
				move(result.begin(), result.end(), back_inserter(*history));
			}
			catch (std::exception const& e)
			{
				history->push_back(string("operation failed: ") + e.what());
			}
		}
	});

	// Delete key on Mac
	ui.RegisterAction("\x7f", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		if (not currentCmdLine->empty())
		{
			int dc = *colOffsetPtr;
			if (auto p = dc + static_cast<int>(currentCmdLine->length()); p > 0)
			{
				currentCmdLine->erase(p - 1, 1);
			}
		}
	});
	// Left arrow
	ui.RegisterAction("\x1b\x5b\x44", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		--*colOffsetPtr;
	});
	// Right arrow
	ui.RegisterAction("\x1b\x5b\x43", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		++*colOffsetPtr;
	});
	// Up arrow
	ui.RegisterAction("\x1b\x5b\x41", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		auto& n = *startShowLineNumPtr;
		if (n > 0)
		{
			--n;
		}
	});
	// Down arrow
	ui.RegisterAction("\x1b\x5b\x42", [&](vector<string>* history, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		++*startShowLineNumPtr;
	});
	ui.Runloop();
	return 0;
}