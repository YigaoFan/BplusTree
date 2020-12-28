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

	auto title = "Welcome to use Fan's cmd to control server";
	auto ui = CmdUI(title);
	ui.Init();
	ui.RegisterAction("\t", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr)
	{
		// 只需要看光标之前的部分
		auto searchRange = string_view(*currentCmdLine).substr(0, currentCmdLine->length() + *colOffsetPtr);
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

		auto opts = cmd.Complete(lastWord);
		hintLine->clear();

		for (auto& op : opts)
		{
			hintLine->append(move(op));
			hintLine->push_back(' ');
		}
	});

	// Enter key on Mac
	ui.RegisterAction("\x0a", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr)
	{
		if (*currentCmdLine == "exit")
		{
			ui.TerminateOnNextRun(); // 改名，running 也改名
		}
		else
		{
			return;
			auto result = cmd.Run(*currentCmdLine);
			addToHistory(result);
			currentCmdLine->clear();
			hintLine->clear();
		}
	});

	// Delete key on Mac
	ui.RegisterAction("\x7f", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr)
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
	ui.RegisterAction("\x1b\x5b\x44", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr)
	{
		--*colOffsetPtr;
	});
	// Right arrow
	ui.RegisterAction("\x1b\x5b\x43", [&](auto addToHistory, string* currentCmdLine, string* hintLine, int* colOffsetPtr)
	{
		++*colOffsetPtr;
	});
	ui.Runloop();
	return 0;
}