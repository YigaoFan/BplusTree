#pragma once
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include <utility>
#include <filesystem>
#include <shared_mutex>
#include "../Basic/Exception.hpp"
#include "../FuncLib/Store/File.hpp"
#include "../FuncLib/Persistence/ByteConverter.hpp"
#include "../Network/Request.hpp"
#include "Awaiter.hpp"
#include "ThreadPool.hpp"

namespace Server
{
	using Basic::InvalidOperationException;
	using FuncLib::Store::File;
	using FuncLib::Store::pos_label;
	using Network::Request;
	using ::std::make_shared;
	using ::std::map;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_lock;
	using ::std::shared_mutex;
	using ::std::shared_ptr;
	using ::std::string;
	using ::std::unique_lock;
	using ::std::filesystem::path;

	class AccountData
	{
	private:
		static constexpr pos_label AccountsLabel = 10;
		/// key: username value: password, IsAdmin
		using AccountsInfo = map<string, pair<string, bool>>;
		shared_ptr<File> _file;
		shared_ptr<AccountsInfo> _accountsInfo;

		AccountData(shared_ptr<File> file, shared_ptr<AccountsInfo> accountsInfo)
			: _file(move(file)), _accountsInfo(accountsInfo)
		{
		}

	public:
		static AccountData GetFrom(path const& dirPath)
		{
			auto path = dirPath / "accounts_info";
			auto firstSetup = not exists(path);
			auto file = File::GetFile(path);

			shared_ptr<AccountsInfo> accounts;

			if (firstSetup)
			{
				auto [l, a] = file->New(AccountsLabel, AccountsInfo());
				accounts = move(a);
			}
			else
			{
				accounts = file->Read<AccountsInfo>(AccountsLabel);
			}

			return AccountData(move(file), move(accounts));
		}

		AccountData(AccountData&& that) noexcept = default;
		AccountData(AccountData const& that) = delete;
		~AccountData()
		{
			if (_file != nullptr and _accountsInfo != nullptr)
			{
				_file->Store(AccountsLabel, _accountsInfo);
			}
		}

		bool IsRegistered(string const& username, string const& password) const
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

		void Add(bool isAdmin, string username, string password)
		{
			if (_accountsInfo->contains(username))
			{
				throw InvalidOperationException("add failed: " + username + " already exist");
			}
			_accountsInfo->insert(pair(move(username), pair(move(password), isAdmin)));
		}

		void Remove(bool isAdmin, string const &username)
		{
			if (_accountsInfo->contains(username))
			{
				if ((*_accountsInfo)[username].second == isAdmin)
				{
					_accountsInfo->erase(username);
					return;
				}
				throw InvalidOperationException("remove failed: " + username + " account type(is admin or not) not match");
			}
			throw InvalidOperationException("remove failed: " + username + " not found");
		}
	};

	class AccountManager
	{
	private:
		mutable shared_mutex _dataMutex;
		AccountData _data;
		ThreadPool* _threadPool;

		auto GenerateWriteTask(auto requestPtr, auto task)
		{
			/// 对 task 提供一个安全访问 request 和 funcLib 的环境
			return [this, request = move(requestPtr), task = move(task)]
			{
				unique_lock<mutex> lock(request->Mutex);
				struct Guard
				{
					decltype(request) Request;
					decltype(lock) *Lock;

					~Guard()
					{
						Request->Done = true;
						if (std::current_exception() != nullptr)
						{
							Request->CondVar.wait(*Lock, [=]
							{
								return (bool)Request->Fail;
							});
							Request->Fail();
						}

						// 这里对 fail 的情况要区分下应该 TODO 和 log 里的信息能不能统一起来
					}
				};

				Guard g{ request, &lock };
				unique_lock<shared_mutex> dataGuard(this->_dataMutex);
				task(request);

				request->CondVar.wait(lock, [=]
				{
					return (bool)request->Success;
				});
				request->Success();
			};
		}

	public:
		AccountManager(AccountData data) : _data(move(data))
		{ }

		AccountManager(AccountManager&& that) noexcept
			: _dataMutex(), _data(move(that._data)), _threadPool(that._threadPool)
		{ }

		void SetThreadPool(ThreadPool* threadPool)
		{
			_threadPool = threadPool;
		}

		bool IsRegistered(string const& username, string const& password) const
		{
			shared_lock<shared_mutex> lock(_dataMutex);
			return _data.IsRegistered(username, password);
		}

		bool IsAdmin(string const& username, string const& password) const
		{
			shared_lock<shared_mutex> lock(_dataMutex);
			return _data.IsAdmin(username, password);
		}

		Awaiter<AddClientAccountRequest> AddClientAccount(AddClientAccountRequest::Content paras)
		{
			auto requestPtr = make_shared<AddClientAccountRequest>(AddClientAccountRequest{ Request(), move(paras) });
			_threadPool->Execute(GenerateWriteTask(requestPtr, [this](auto request)
			{
				_data.Add(false, move(request->Paras.Username), move(request->Paras.Password));
			}));

			return { requestPtr };
		}

		Awaiter<AddAdminAccountRequest> AddAdminAccount(AddAdminAccountRequest::Content paras)
		{
			// 为什么下面这个 make_shared 要这么构造，里面的那个 AddAdminAccountRequest{} 不能省吗
			// 可能 make_shared 内部不是用 {} 这个构造的
			auto requestPtr = make_shared<AddAdminAccountRequest>(AddAdminAccountRequest{ Request(), move(paras) });
			_threadPool->Execute(GenerateWriteTask(requestPtr, [this](auto request)
			{
				_data.Add(true, move(request->Paras.Username), move(request->Paras.Password));
			}));

			return { requestPtr };
		}

		Awaiter<RemoveClientAccountRequest> RemoveClientAccount(RemoveClientAccountRequest::Content paras)
		{
			auto requestPtr = make_shared<RemoveClientAccountRequest>(RemoveClientAccountRequest{ Request(), move(paras) });
			_threadPool->Execute(GenerateWriteTask(requestPtr, [this](auto request)
			{
				_data.Remove(false, request->Paras.Username);
			}));

			return { requestPtr };
		}

		Awaiter<RemoveAdminAccountRequest> RemoveAdminAccount(RemoveAdminAccountRequest::Content paras)
		{
			auto requestPtr = make_shared<RemoveAdminAccountRequest>(RemoveAdminAccountRequest{ Request(), move(paras) });
			_threadPool->Execute(GenerateWriteTask(requestPtr, [this](auto request)
			{
				_data.Remove(true, request->Paras.Username);
			}));

			return { requestPtr };
		}
	};
}