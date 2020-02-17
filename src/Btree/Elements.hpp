#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <utility>
#include "Basic.hpp"
#include "IEnumerator.hpp"
#include "../Basic/Exception.hpp"
#include "Exception.hpp"
#include "LiteVector.hpp"

namespace Collections
{
	using ::std::pair;
	using ::std::vector;
	using ::std::function;
	using ::std::shared_ptr;
	using ::std::array;
	using ::std::move;
	using ::Basic::KeyNotFoundException;
	
	// TODO when BtreeOrder is big, use binary search in iterate process
	template <typename Key, typename Value, order_int BtreeOrder, typename LessThan>
	class Elements : public LiteVector<pair<Key, Value>, order_int, BtreeOrder>
	{
	public:
		using Item = pair<Key, Value>;
		using Base = LiteVector<Item, order_int, BtreeOrder>;
		shared_ptr<LessThan> LessThanPtr;

	public:
		Elements(shared_ptr<LessThan> lessThanPtr) : Base(), LessThanPtr(lessThanPtr)
		{ }

		Elements(IEnumerator<Item&>& enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base(), LessThanPtr(lessThanPtr)
		{
			while (enumerator.MoveNext())
			{
				Base::Add(move(enumerator.Current()));
			}
		}

		// TODO how to solve && and & in up and below method
		Elements(IEnumerator<Item>&& enumerator, shared_ptr<LessThan> lessThanPtr)
			: Base(), LessThanPtr(lessThanPtr)
		{ 
			while (enumerator.MoveNext())
			{
				Base::Add(move(enumerator.Current()));
			}
		}

		Elements(Elements const& that) : Base(that), LessThanPtr(that.LessThanPtr)
		{ }

		Elements(Elements&& that) noexcept : Base(move(static_cast<Base&>(that))), LessThanPtr(move(that.LessThanPtr))
		{ }

		bool ContainsKey(Key const& key) const
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

		/// Remove the item corresponding to the key.
		/// Invoker should ensure key exists in this Elements.
		/// \param key
		void RemoveKey(Key const& key)
		{
			return Base::RemoveAt(IndexKeyOf(key));
		}

		void Add(Item p, function<void()> changeMinCallback = [](){})
		{
			if (this->Emplace())
			{
				return Append(move(p));
			}

			if ((*LessThanPtr)(this->operator[](this->_count - 1).first, p.first))
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

		void AppendItems(vector<Item> items)
		{
			for (auto& i : items)
			{
				this->Append(move(i));
			}
		}

		Item ExchangeMax(Item p)
		{
			auto max = this->PopOut();
			Add(move(p));
			return move(max);
		}

		Item ExchangeMin(Item p)
		{
			auto min = this->FrontPopOut();
			Add(move(p));
			return move(min);
		}
		
		// TODO why should use below code to compile code in MiddleNode
		using Base::operator[];
		Value const& GetValue(Key const& key) const
		{
			return const_cast<Elements*>(this)->GetValue(key);
		}

		Value& GetValue(Key const& key)
		{
			return this->operator[](IndexKeyOf(key)).second;
		}

		order_int IndexKeyOf(Key const& key) const
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
		order_int SelectBranch(T const& key) const
		{
			for (decltype(this->_count) i = 1; i < this->_count; ++i)
			{
				if ((*LessThanPtr)(key, this->operator[](i).first))
				{
					return i - 1;
				}
			}

			return this->_count - 1;
		}

		auto GetEnumerator() { return CreateEnumerator(*this); }
		auto GetEnumerator() const { return CreateEnumerator(*this); }

	private:
		// TODO check template args
		template <bool WithCheck=true>
		void Insert(Item p, function<void()> changeMinCallback)
		{
			for (decltype(this->_count) i = 0; i < this->_count; ++i)
			{
				auto& lessThan = *LessThanPtr;
				if (lessThan(p.first, this->operator[](i).first))
				{
					Base::Emplace(i, move(p));
					if (i == 0)
					{
						changeMinCallback();
					}
				}

				if constexpr (WithCheck)
				{
					if (!lessThan(this->operator[](i).first, p.first))
					{
						throw DuplicateKeyException(move(p.first));
					}
				}
			}
		}
	};
}