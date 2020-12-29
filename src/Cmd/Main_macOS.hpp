#pragma once
#include <string>
#include <string_view>
#include <asio.hpp>
#include "CmdUI.hpp"
#include "Cmder.hpp"

int UI_Main()
{
	using ::asio::ip::tcp;
	using Cmd::Cmder;
	using Cmd::CmdUI;
	using ::std::move;
	using ::std::string;
	using ::std::string_view;

	// printf("r: %d", '\r');
	// return 0;
	asio::io_context io;
	// tcp::resolver resolver(io);
	// auto endPoints = resolver.resolve("localhost", "daytime");

	tcp::socket socket(io);
	// asio::connect(socket, endPoints);

	auto cmd = Cmder::NewFrom(move(socket));

	auto title = "Welcome to use Fan's cmd to control server(Press Up key can view history)";
	auto ui = CmdUI(title);
	ui.Init();
	ui.RegisterAction("\t", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
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
		auto opts = cmd.Complete(lastWord);
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
	ui.RegisterAction("\x0a", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		auto& cmd = *currentCmdLine;

		if (cmd.empty())
		{
			return;
		}

		if (cmd == "exit")
		{
			ui.TerminateOnNextRun(); // 改名，running 也改名
		}
		else
		{
			addToHistory(string(">> ") + cmd);
			currentCmdLine->clear();
			hintLine->clear();
			// upArrowCallback(addToHistory, currentCmdLine, hintLine, colOffsetPtr, startShowLineNumPtr);
			++*startShowLineNumPtr;

			return;
			// auto result = cmd.Run(*currentCmdLine);
			// addToHistory(result);
			// currentCmdLine->clear();
			// hintLine->clear();
		}
	});

	// Delete key on Mac
	ui.RegisterAction("\x7f", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
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
	ui.RegisterAction("\x1b\x5b\x44", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		--*colOffsetPtr;
	});
	// Right arrow
	ui.RegisterAction("\x1b\x5b\x43", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		++*colOffsetPtr;
	});
	// Up arrow
	ui.RegisterAction("\x1b\x5b\x41", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		auto& n = *startShowLineNumPtr;
		if (n > 0)
		{
			--n;
		}
	});
	// Down arrow
	ui.RegisterAction("\x1b\x5b\x42", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr, size_t* startShowLineNumPtr)
	{
		++*startShowLineNumPtr;
	});
	ui.Runloop();
	return 0;
}