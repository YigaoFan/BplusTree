#pragma once
#include <string>
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
	ui.RegisterAction("\t", [&](auto addToHistory, string* currentCmdLine, string* hintLine)
	{
		auto lastWordPos = currentCmdLine->find_last_of(' ');
		string lastWord;
		if (lastWordPos == string::npos)
		{
			lastWord = *currentCmdLine;
		}
		else
		{
			lastWord = currentCmdLine->substr(lastWordPos + 1);
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
	ui.RegisterAction("\x0a", [&](auto addToHistory, string* currentCmdLine, string* hintLine)
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
	ui.RegisterAction("\x7f", [&](auto addToHistory, string* currentCmdLine, string* hintLine)
	{
		if (not currentCmdLine->empty())
		{
			currentCmdLine->pop_back();
		}
	});

	ui.Runloop();
	return 0;
}