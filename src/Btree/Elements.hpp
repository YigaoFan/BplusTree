#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <utility>
#include <type_traits>
#include "Basic.hpp"
#include "IEnumerator.hpp"
#include "../Basic/Exception.hpp"
#include "CollectionException.hpp"
#include "LiteVector.hpp"
#include "../FuncLib/Persistence/FriendFuncLibDeclare.hpp"

namespace Collections
{
	using ::Basic::InvalidOperationException;
	using ::Basic::KeyNotFoundException;
	using ::std::array;
	using ::std::function;
	using ::std::is_convertible;
	using ::std::move;
	using ::std::pair;
	using ::std::shared_ptr;
	using ::std::vector;

	template <typename LessThan>
	struct TraitArgType;

	template <typename T>
	struct TraitArgType<LessThan<T>>
	{
		using Result = T;
	};
	
	template <typename T, typename LessThan>
	concept MatchLessThanArgType = is_convertible<T, typename TraitArgType<LessThan>::Result>::value;

	// TODO when BtreeOrder is big, use binary search in iterate process
	template <typename Key, typename Value, order_int BtreeOrder, typename LessThan = LessThan<Key>>
	class Elements : public LiteVector<pair<Key, Value>, order_int, BtreeOrder>
	{
	public:
		friend struct FuncLib::Persistence::ByteConverter<Elements, false>;
		template <typename, FuncLib::Persistence::OwnerState>
		friend struct FuncLib::Persistence::TypeConverter;
		using Item = pair<Key, Value>;
		using Base = LiteVector<Item, order_int, BtreeOrder>;
		shared_ptr<LessThan> LessThanPtr;

	public:
		// Items in constructor is sorted, will be moved
		Elements(shared_ptr<LessThan> lessThanPtr)
			: Base(), LessThanPtr(lessThanPtr)
		{ }

		Elements(IEnumerator<Item&> auto enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base(), LessThanPtr(lessThanPtr)
		{
			while (enumerator.MoveNext())
			{
				Base::Add(move(enumerator.Current()));
			}
		}

		Elements(IEnumerator<Item> auto enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base(), LessThanPtr(lessThanPtr)
		{ 
			while (enumerator.MoveNext())
			{
				Base::Add(move(enumerator.Current()));
			}
		}

		Elements(Elements const& that) : Base(that), LessThanPtr(that.LessThanPtr)
		{ }

		Elements(Elements&& that) noexcept
			: Base(move(that)), LessThanPtr(move(that.LessThanPtr))
		{ }

		template <typename T>
		requires MatchLessThanArgType<T, LessThan>
		bool ContainsKey(T key) const
		{
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				auto& e = enumerator.Current();
				auto& lessThan = *LessThanPtr;
				if (auto less = lessThan(key, e.first), notLess = lessThan(e.first, key); less == notLess)
				{
					return true;
				}
			}

			return false;
		}
		
		vector<Key> Keys() const
		{
			vector<Key> keys;
			keys.reserve(this->Count());
			auto enumerator = GetEnumerator();
			while (enumerator.MoveNext())
			{
				keys.push_back(enumerator.Current().first);
			}

			return keys;
		}

		void Add(Item p, function<void()> changeMinCallback = [](){})
		{
			if (this->Empty())
			{
				return Append(move(p));
			}

			if ((*LessThanPtr)(this->operator[](this->Count() - 1).first, p.first))
			{
				return Append(move(p));
			}
			else
			{
				return Insert(move(p), move(changeMinCallback));
			}
		}

		void Append(Item p) 
		{
			Base::Add(move(p));
		}

		/// p is from user, so need check duplicate
		Item ExchangeMax(Item p)
		{
			auto& lessThan = *LessThanPtr;

			if (lessThan(p.first, this->LastOne().first))
			{
				auto max = this->PopOut();
				Add(move(p));
				return move(max);
			}
			else if (!lessThan(this->LastOne().first, p.first))
			{
				throw DuplicateKeyException(move(p.first));
			}

			return move(p);
		}

		/// p is from user, so need check duplicate
		Item ExchangeMin(Item p)
		{
			auto& lessThan = *LessThanPtr;

			if (lessThan(this->FirstOne().first, p.first))
			{
				auto min = this->FrontPopOut();
				Add(move(p));
				return move(min);
			}
			else if (!lessThan(p.first, this->FirstOne().first))
			{
				throw DuplicateKeyException(move(p.first));
			}

			return move(p);
		}

		// TODO why should use below code to compile code in MiddleNode
		using Base::operator[];

		template <typename T> 
		requires MatchLessThanArgType<T, LessThan>
		Value const& GetValue(T key) const
		{
			return const_cast<Elements*>(this)->GetValue(key);
		}
		template <typename T>
		requires MatchLessThanArgType<T, LessThan>
		Value& GetValue(T key)
		{
			return this->operator[](IndexKeyOf(key)).second;
		}

		template <typename T>
		requires MatchLessThanArgType<T, LessThan>
		order_int IndexKeyOf(T key) const
		{
			auto enumerator = GetEnumerator();
			auto& lessThan = *LessThanPtr;
			while (enumerator.MoveNext())
			{
				auto& item = enumerator.Current();
				if (lessThan(key, item.first) == lessThan(item.first, key))
				{
					return (order_int)enumerator.CurrentIndex();
				}
			}

			throw KeyNotFoundException();
		}

		template <typename T>
		requires MatchLessThanArgType<T, LessThan>
		order_int SelectBranch(T key) const
		{
			for (decltype(this->Count()) i = 1; i < this->Count(); ++i)
			{
				if ((*LessThanPtr)(key, this->operator[](i).first))
				{
					return i - 1;
				}
			}

			return this->Count() - 1;
		}

		auto GetEnumerator() { return CreateRefEnumerator(*this); }
		auto GetEnumerator() const { return CreateRefEnumerator(*this); }

	private:
		Elements(LiteVector<pair<Key, Value>, order_int, BtreeOrder> base) : Base(move(base))
		{ }

		Elements() : Base() { }

		void LessThanPredicate(shared_ptr<LessThan> lessThanPtr)
		{
			LessThanPtr = lessThanPtr;
		}
		// TODO check template args
		template <bool WithCheck=true>
		void Insert(Item p, function<void()> changeMinCallback)
		{
			for (decltype(this->Count()) i = 0; i < this->Count(); ++i)
			{
				auto& lessThan = *LessThanPtr;
				if (lessThan(p.first, this->operator[](i).first))
				{
					Base::Emplace(i, move(p));
					if (i == 0)
					{
						changeMinCallback();
					}

					return;
				}
				else if constexpr (WithCheck)
				{
					if (!lessThan(this->operator[](i).first, p.first))
					{
						throw DuplicateKeyException(move(p.first));
					}
				}
			}

			throw InvalidOperationException("Cannot find suitable room for key, program has bug");
		}
	};
}