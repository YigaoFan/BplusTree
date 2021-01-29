#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <filesystem>
#include "../FuncLib/Store/File.hpp"
#include "../FuncLib/Persistence/ByteConverter.hpp"

namespace Server
{
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::filesystem::path;

	class AccountManager
	{
	private:
		using AccountsInfo = map<string, pair<string, bool>>;
		shared_ptr<File> _file;
		shared_ptr<AccountsInfo> _accountsInfo;

		AccountManager(shared_ptr<File> file, shared_ptr<AccountsInfo> accountsInfo)
			: _file(file), _accountsInfo(accountsInfo)
		{ }

	public:
		AccountManager(AccountManager&& that) noexcept = default;
		AccountManager(AccountManager const& that) = delete;
		static AccountManager GetFrom(path const& dirPath)
		{
			auto path = dirPath / "accounts_info";
			auto firstSetup = not exists(path);
			auto file = File::GetFile(path);

			shared_ptr<AccountsInfo> accounts;

			constexpr pos_label accountsLabel = 10;
			if (firstSetup)
			{
				auto [l, a] = file->New(accountsLabel, AccountsInfo());
				accounts = move(a);
			}
			else
			{
				accounts = file->Read<AccountsInfo>(accountsLabel);
			}

			return AccountManager(move(file), move(accounts));
		}

		void Add(bool isAdmin, string username, string password)
		{
			_accountsInfo->insert(pair(move(username), pair(move(password), isAdmin)));
		}

		bool IsRegistered(string const& username, string const& password)
		{
			return _accountsInfo->contains(username) 
				and (*_accountsInfo)[username].first == password;
		}

		bool IsAdmin(string const& username, string const& password) const
		{
			return _accountsInfo->contains(username)
				and (*_accountsInfo)[username].first == password
				and (*_accountsInfo)[username].second;
		}
	};
}