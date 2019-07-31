#pragma once
#pragma once
#include <functional> // for function
#include <variant>    // variant
#include <cstring>    // for memcpy
#include <exception>  // for runtime_error
#include <string>     // for to_string
#include <vector>     // for vector
#include <memory>     // for unique_ptr, shared_ptr
#include <iterator>
#include <utility>
#ifdef BTREE_DEBUG
#include "Utility.hpp"
#endif

namespace btree {
	using std::variant;
	using std::pair;
	using std::vector;
	using std::function;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::array;
	using std::memcpy;
	using std::runtime_error;
	using std::make_unique;
	using std::forward_iterator_tag;
	using std::make_pair;

#define ELEMENTS_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder, typename PtrType>

	/**
	 * First and second relationship is to public
	 */
	ELEMENTS_TEMPLATE
		class Elements {
		public:
			template <typename T>
			class ElementsIterator;
			using ValueForContent = variant<Value, unique_ptr<PtrType>>;
			using Content = pair<Key, ValueForContent>;
			using LessThan = function<bool(const Key&, const Key&)>;

			const bool           LeafFlag;
			shared_ptr<LessThan> LessThanPtr;

			template <typename Iter>
			Elements(Iter, Iter, shared_ptr<LessThan>);
			Elements(const Elements&);
			Elements(Elements&&) noexcept;

			bool             have(const Key&) const;
			uint16_t         count() const;
			bool             full()  const;

			// bool means max key changes
			bool             remove(const Key&);
			bool             remove(uint16_t); // TODO maybe will implement
			void             removeItemsFrom(bool, uint16_t);
			template <typename T>
			void             insert(pair<Key, T>);
			template <typename T>
			void             append(pair<Key, T>);
			template <typename T>
			pair<Key, T> exchangeMax(pair<Key, T>);
			template <typename T>
			pair<Key, T> exchangeMin(pair<Key, T>);
			// TODO maybe directly use PtrType without * this project every where

			ValueForContent& operator[](const Key&);
			Content&         operator[](uint16_t);
			const ValueForContent& operator[](const Key&) const;
			const Content&         operator[](uint16_t)   const;

			int32_t          indexOf(const PtrType*) const;
			int32_t          indexOf(const Key&)     const;
			uint16_t         suitablePosition(const Key&)     const;
			auto begin();
			auto end();
			auto begin() const;
			auto end() const;

			// use template to reduce the code below
			static Value&         value(ValueForContent&);
			static PtrType*       ptr(ValueForContent&);
			static const Value&   value(const ValueForContent&);
			static PtrType*       ptr(const ValueForContent&);

		private:
			uint16_t                   _count{ 0 };
			array<Content, BtreeOrder> _elements;

			void     adjustMemory(int16_t, Content *);
			uint16_t relatedIndex(const Key&);
			template <typename T>
			void     add(pair<Key, T>);

			static Content& assign(Content&, const pair<Key, Value>&);
			template <typename T>
			static Content& assign(Content&, pair<Key, unique_ptr<T>>&);
			static Content* moveElement(int16_t, Content*, Content*);
			static void initialInternalElements(array<Content, BtreeOrder>&, const array<Content, BtreeOrder>&, bool, uint16_t);
			static unique_ptr<PtrType>& uniquePtr(ValueForContent&);

			/*template <typename T>
			struct ContentRelatedType {
			};

			template <>
			struct ContentRelatedType<const Content> {
				using type = const Content;
			};

			template <>
			struct ContentRelatedType<Content> {
				using type = Content;
			};

		public:
			template <typename T>
			class ElementsIterator {
				using type = typename ContentRelatedType<T>::type;
				type* _ptr;

			public:
				using iterator_category = forward_iterator_tag;
				using value_type = type;
				using difference_type = ptrdiff_t;
				using pointer = type * ;
				using reference = type & ;

				explicit ElementsIterator(T* p)       : _ptr(p) {}

				type&             operator* () const { return *_ptr; }
				type*             operator->() const { return _ptr; }
				ElementsIterator& operator++()       { ++_ptr; return *this; }
				bool              operator==(const ElementsIterator& i) const
				{ return _ptr == i._ptr; }
				bool              operator!=(const ElementsIterator& i) const
				{ return _ptr != i._ptr; }
			};*/
	};
}

namespace btree {
#define ELE Elements<Key, Value, BtreeOrder, PtrType>

	ELEMENTS_TEMPLATE
		template <typename Iter>
	ELE::Elements(
		Iter begin,
		Iter end,
		shared_ptr<LessThan> lessThanPtr
	) :
		LeafFlag(std::is_same<typename std::decay<decltype(*begin)>::type, pair<Key, Value>>::value),
		LessThanPtr(lessThanPtr)
	{
		if (begin != end) {
			do {
				Elements::assign(_elements[_count], *begin);

				++_count;
				++begin;
			} while (begin != end);
		}
	}

	ELEMENTS_TEMPLATE
		ELE::Elements(const Elements& that) :
		LeafFlag(that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_count(that._count)
	{
		initialInternalElements(_elements, that._elements, that.LeafFlag, _count);
	}

	ELEMENTS_TEMPLATE
		ELE::Elements(Elements&& that) noexcept :
		LeafFlag(that.LeafFlag),
		LessThanPtr(that.LessThanPtr),
		_count(that._count),
		_elements(std::move(that._elements))
	{
		that._count = 0;
	}

	ELEMENTS_TEMPLATE
		bool
		ELE::have(const Key& key) const
	{
		auto& lessThan = *LessThanPtr;
		for (auto i = 0; i < _count; ++i) {
			auto& eleKey = _elements[i].first;
			if (lessThan(eleKey, key) == lessThan(key, eleKey)) {
				return true;
			}
		}
		return false;
	}

	ELEMENTS_TEMPLATE
		uint16_t
		ELE::count() const
	{
		return _count;
	}

	ELEMENTS_TEMPLATE
		bool
		ELE::Elements::full() const
	{
		return _count >= BtreeOrder;
	}

	ELEMENTS_TEMPLATE
		bool
		ELE::remove(const Key& key)
	{
		auto i = relatedIndex(key);
		auto boundChanged{ false };

		adjustMemory(-1, &_elements[i + 1]);
		if (i == (_count - 1)) {
			boundChanged = true;
		}
		--_count;
		return boundChanged;
	}

	ELEMENTS_TEMPLATE
		void
		ELE::removeItemsFrom(bool head, uint16_t count)
	{
		if (head) {
			adjustMemory(-count, &_elements[0]);
		}

		_count -= count;
	}

	ELEMENTS_TEMPLATE
		typename ELE::ValueForContent&
		ELE::operator[](const Key& key)
	{
		return const_cast<ValueForContent&>(
			(static_cast<const Elements&>(*this))[key]
			);
	}

	ELEMENTS_TEMPLATE
		const typename ELE::ValueForContent&
		ELE::operator[](const Key& key) const
	{
		auto i = indexOf(key);

		if (i != -1) {
			return _elements[i].second;
		}

		throw runtime_error("Can't get the Value correspond"
			+ key
			+ ","
			+ " Please check the key existence.");
	}

	ELEMENTS_TEMPLATE
		typename ELE::Content&
		ELE::operator[](uint16_t i)
	{
		return const_cast<Content&>(
			(static_cast<const Elements&>(*this))[i]
			);
	}

	ELEMENTS_TEMPLATE
		const typename ELE::Content&
		ELE::operator[](uint16_t i) const
	{
		return _elements[i];
	}

	ELEMENTS_TEMPLATE
		auto
		ELE::begin()
	{
		// 本来应该从 begin 的返回类型推导的，这样...
		return _elements.begin();
		//return ElementsIterator(_elements.begin());
	}

	ELEMENTS_TEMPLATE
		auto
		ELE::end()
	{
		return _elements.end();
		//return ElementsIterator(_elements.end());
	}

	ELEMENTS_TEMPLATE
		auto
		ELE::begin() const
	{
		return _elements.begin();
		//return ElementsIterator(_elements.begin());
	}

	ELEMENTS_TEMPLATE
		auto
		ELE::end() const
	{
		return _elements.end();
		//return ElementsIterator(_elements.end());
	}

	ELEMENTS_TEMPLATE
		int32_t
		ELE::indexOf(const PtrType* pointer) const
	{
#define PTR_OF_ELE ptr(_elements[i].second)

		for (auto i = 0; i < _count; ++i) {
			if (PTR_OF_ELE == pointer) {
				return i;
			}
		}

		return -1; // means not found
#undef PTR_OF_ELE
	}


	ELEMENTS_TEMPLATE
		int32_t
		ELE::indexOf(const Key& key) const
	{
#define KEY_OF_ELE _elements[i].first
		auto& lessThan = *LessThanPtr;

		for (auto i = 0; i < _count; ++i) {
			if ((!lessThan(key, KEY_OF_ELE)) && (!lessThan(KEY_OF_ELE, key))) {
				return i;
			}
		}

		return -1; // means not found
#undef KEY_OF_ELE
	}

	ELEMENTS_TEMPLATE
		uint16_t
		ELE::suitablePosition(const Key& key) const
	{
#define KEY_OF_ELE _elements[i].first
		auto& lessThan = *LessThanPtr;

		for (auto i = 0; i < _count; ++i) {
			if (lessThan(key, KEY_OF_ELE)) {
				return i;
			}
		}

		return _count + 1;
#undef KEY_OF_ELE
	}

	ELEMENTS_TEMPLATE
		template <typename T>
	void
		ELE::insert(pair<Key, T> p)
	{
		// maybe only leaf add logic use this Elements method
		// middle add logic is in middle Node self
		auto& k = p.first;
		auto& v = p.second;

		int16_t i = 0;
		for (; i < _count; ++i) {
			if ((*LessThanPtr)(k, _elements[i].first) == (*LessThanPtr)(_elements[i].first, k)) {
				throw runtime_error("The inserting key duplicates: " + k);
			}
			else if ((*LessThanPtr)(k, _elements[i].first)) {
				goto Insert;
			}
		}
		// when equal and bigger than the bound, throw the error
		throw runtime_error("\nWrong adding Key-Value: " + k + " " + v + ". "
			+ "Now the count of this Elements: " + std::to_string(_count) + ". "
			+ "And please check the max Key to ensure not beyond the bound.");

	Insert:
		adjustMemory(1, &_elements[i]);
		_elements[i] = std::move(p); //	Elements::assign(_elements[i], p);
		++_count;
	}

	ELEMENTS_TEMPLATE
		template <typename T>
	void
		ELE::append(pair<Key, T> p)
	{
		if (full()) {
			throw runtime_error("No free space to add");
		}

		_elements[_count] = std::move(p);
		++_count;
	}

#ifdef BTREE_DEBUG
#define BOUND_CHECK                                                                                                  \
	if (_count < BtreeOrder) {                                                                                       \
		throw runtime_error("Please invoke exchangeMax when the Elements is full, you can use full to check it");\
	} else if (have(p.first)) {                                                                                  \
		throw runtime_error("The Key: " + p.first + " has already existed");                                     \
	}
#endif

	ELEMENTS_TEMPLATE
		template <typename T>
	pair<Key, T>
		ELE::exchangeMax(pair<Key, T> p)
	{
#ifdef BTREE_DEBUG
		BOUND_CHECK
#endif
			auto& maxItem = _elements[_count - 1];
		auto key = maxItem.first;
		auto valueForContent = std::move(maxItem.second);

		--_count;
		add(p);

		if constexpr (std::is_same<T, Value>::value) {
			return make_pair<Key, T>(std::move(key), std::move(value(valueForContent)));
		}
		else {
			return make_pair<Key, T>(std::move(key), std::move(ptr(valueForContent)));
		}
	}

	ELEMENTS_TEMPLATE
		template <typename T>
	pair<Key, T>
		ELE::exchangeMin(pair<Key, T> p)
	{
#ifdef BTREE_DEBUG
		BOUND_CHECK
#endif
			auto& minItem = _elements[0];
		auto key = minItem.first;
		auto valueForContent = std::move(minItem.second);
		// pair<Key, Value> min{ minItem.first, value(minItem.second) };
		// move left
		adjustMemory(-1, &_elements[1]);
		--_count;

		add(p);

		if constexpr (std::is_same<T, Value>::value) {
			return make_pair<Key, T>(key, value(valueForContent));
		}
		else {
			return make_pair<Key, T>(key, ptr(valueForContent));
		}
	}

#ifdef BOUND_CHECK
#undef BOUND_CHECK
#endif

	ELEMENTS_TEMPLATE
		template <typename T>
	void
		ELE::add(pair<Key, T> p)
	{
		auto& lessThan = *LessThanPtr;

		if (lessThan(_elements[_count - 1].first, p.first)) {
			append(p);
		}
		else {
			insert(p);
		}
	}

	ELEMENTS_TEMPLATE
		void
		ELE::adjustMemory(int16_t direction, Content* begin)
	{
		// default Content* end
		if (_count == 0 || direction == 0) {
			return;
		}
		else if (_count >= BtreeOrder && direction > 0) {
			throw runtime_error("The Elements is full");
		}
		auto end = /*(_count >= BtreeOrder) ? _elements.end() : */&_elements[_count]; // end is exclude
		moveElement(direction, begin, end);
	}

	ELEMENTS_TEMPLATE
		uint16_t
		ELE::relatedIndex(const Key &key)
	{
		for (auto i = 0; i < _count; ++i) {
			if (_elements[i].first == key) {
				return i;
			}
		}

		throw runtime_error("Can't get the related element of the key: " + key);
	}
}

namespace btree {
	ELEMENTS_TEMPLATE
		typename ELE::Content*
		ELE::moveElement(int16_t direction, Content* begin, Content* end)
	{
		// depend on different direction, where to start copy is different
		// in the future, could use WORD_BIT to copy
		if (direction < 0) {
			return static_cast<Content *>(
				memcpy(begin + direction, begin, (end - begin) * sizeof(Content)));
		}
		else if (direction > 0) {
			for (auto current = end - 1; current >= begin; --current) {
				return static_cast<Content *>(
					memcpy(current + direction, current, sizeof(Content)));
			}
		}

		return begin;
	}

	ELEMENTS_TEMPLATE
		void
		ELE::initialInternalElements(
			array<Content, BtreeOrder>& thisElements,
			const array<typename ELE::Content, BtreeOrder>& thatElements,
			bool isValue,
			uint16_t count
		)
	{
		auto src = &thatElements;
		auto des = &thisElements;

		memcpy(des, src, count * sizeof(Content)); // copy all without distinguish

		if (!isValue) {
			for (auto& e : thisElements) {
				auto& ptr = std::get<unique_ptr<PtrType>>(e.second);
				auto deepClone = ptr->clone();
				ptr.release();
				ptr.reset(deepClone.release());
			}
		}
	}

	ELEMENTS_TEMPLATE
		template <typename T>
	typename ELE::Content&
		ELE::assign(Content &ele, pair<Key, unique_ptr<T>>& ptrPair)
	{
		static_assert(std::is_base_of<PtrType, T>::value, "The type to be stored should be derived from PtrType");

		ele.first = ptrPair.first;
		ELE::uniquePtr(ele.second).reset(ptrPair.second.release());
		// ele.second = std::move(make_unique<PtrType>(ptrPair.second.release()));
		// new (&(ele.second)) ValueForContent(std::move(ptrPair.second));
		return ele;
	}

	ELEMENTS_TEMPLATE
		typename ELE::Content&
		ELE::assign(Content &ele, const pair<Key, Value>& pairPtr)
	{
		ele = std::move(pairPtr);
		return ele;
	}

	ELEMENTS_TEMPLATE
		Value&
		ELE::value(ValueForContent& v)
	{
		return std::get<Value>(v);
	}

	ELEMENTS_TEMPLATE
		const Value&
		ELE::value(const ValueForContent& v)
	{
		return std::get<const Value>(v);
	}

	ELEMENTS_TEMPLATE
		PtrType*
		ELE::ptr(ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v).get();
	}

	ELEMENTS_TEMPLATE
		PtrType*
		ELE::ptr(const ValueForContent& v)
	{
		// #error which to choose?
		// return std::get<unique_ptr<PtrType>>(v).get();
		return std::get<unique_ptr<PtrType>>(v).get();
		// return std::get<const unique_ptr<PtrType>>(v).get();
	}

	ELEMENTS_TEMPLATE
		unique_ptr<PtrType>&
		ELE::uniquePtr(ValueForContent& v)
	{
		return std::get<unique_ptr<PtrType>>(v);
	}

#undef ELE
#undef ELEMENTS_TEMPLATE
}

#include <vector> // for vector

namespace btree {
	using std::make_pair;

	struct LeafFlag {};
	struct MiddleFlag {};

#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define BASE NodeBase<Key, Value, BtreeOrder>

	/*NODE_TEMPLATE
	class NodeBase;*/

	/*template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
	doAdd(BASE*, pair<Key, T>, vector<BASE*>&);*/

	NODE_TEMPLATE
		class NodeBase {
		template <typename Key, typename Value, typename BtreeOrder, typename T>
		friend void doAdd(BASE*, pair<Key, T>, vector<BASE*>&);
		public:
			using Ele = Elements<Key, Value, BtreeOrder, NodeBase>;
			using LessThan = typename Ele::LessThan;
			const bool Middle;
			template <typename Iter>
			NodeBase(LeafFlag, Iter, Iter, shared_ptr<LessThan>);
			template <typename Iter>
			NodeBase(MiddleFlag, Iter, Iter, shared_ptr<LessThan>);
			NodeBase(const NodeBase&);
			NodeBase(NodeBase&&) noexcept;
			virtual unique_ptr<NodeBase> clone() const = 0;
			virtual ~NodeBase() = default;

			inline Key         maxKey() const;
			uint16_t           childCount() const;
			inline vector<Key> allKey() const;
			inline bool        have(const Key&);
			Value*             search(const Key&);
			bool               add(pair<Key, Value>);
			bool               remove(const Key&);
			void               searchSiblingsIn(vector<NodeBase *> &, NodeBase*&, NodeBase*&) const;
			inline bool        full()  const;

		protected:
			Elements<Key, Value, BtreeOrder, NodeBase> elements_;
			inline bool empty() const;
			inline void   changeInSearchDownPath(const Key&, const Key&);
			inline Value* searchWithSaveTrack(const Key&, vector<NodeBase*>&);

			template <typename T>
			inline void reallocateSiblingElement(bool, NodeBase*, vector<NodeBase*>&, pair<Key, T>);
			inline void changeMaxKeyIn(vector<NodeBase*>&, const Key&) const;
			inline void replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<NodeBase*>&, const Key&, const Key&);
			template <typename T>
			inline void splitNode(pair<Key, T>, vector<NodeBase*>&);
			inline void insertLeafToUpper(NodeBase*, vector<NodeBase*>&) const;
	};
}

namespace btree {

	NODE_TEMPLATE
		template <typename Iter>
	BASE::NodeBase(LeafFlag, Iter begin, Iter end, shared_ptr<LessThan> lessThanPtr)
		: Middle(false), elements_(begin, end, lessThanPtr)
	{ }
	NODE_TEMPLATE
		template <typename Iter>
	BASE::NodeBase(MiddleFlag, Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Middle(true), elements_(begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
		BASE::NodeBase(const NodeBase& that)
		: Middle(that.Middle), elements_(that.elements_)
	{ }

	NODE_TEMPLATE
		BASE::NodeBase(NodeBase&& that) noexcept
		: Middle(that.Middle), elements_(std::move(that.elements_))
	{ }

	NODE_TEMPLATE
		uint16_t
		BASE::childCount() const
	{
		return elements_.count();
	}

	NODE_TEMPLATE
		bool
		BASE::empty() const
	{
		return elements_.count() == 0;
	}

	NODE_TEMPLATE
		bool
		BASE::full() const
	{
		return elements_.full();
	}

	NODE_TEMPLATE
		Key
		BASE::maxKey() const
	{
		return elements_[elements_.count() - 1].first;
	}

	NODE_TEMPLATE
		bool
		BASE::have(const Key& key)
	{
		if (!Middle) {
			return elements_.have(key);
		}
		else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					Ele::ptr(e.second)->have(key);
				}
				else if (!(*elements_.LessThanPtr)(e.first, key)) {
					return true;
				}
			}

			return false;
		}
	}

	NODE_TEMPLATE
		Value*
		BASE::search(const Key& key)
	{
		auto maxValueForContent = [](Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		if (!Middle) {
			return elements_.have(key) ? &Ele::value(elements_[key]) : nullptr;
		}
		else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->search(key);
				}
				else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
						return &Ele::value(maxValueForContent(subNodePtr->elements_));
					}
					else {
						subNodePtr = Ele::ptr(maxValueForContent(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}

			return nullptr;
		}
	}

	NODE_TEMPLATE
		vector<Key>
		BASE::allKey() const
	{
		vector<Key> keys;
		keys.reserve(elements_.count());
		for (auto& e : elements_) {
			keys.emplace_back(e.first);
		}

		return keys;
	}

	template <typename Key, typename Value, uint16_t BtreeOrder>
	void
		collectDeepInfo(BASE*, const Key&, vector<BASE*>&);

	NODE_TEMPLATE
		bool
		BASE::add(pair<Key, Value> p)
	{
		vector<decltype(this)> passedNodeTrackStack;

		auto& key = p.first;
		collectDeepInfo(this, key, passedNodeTrackStack);
		// 模板参数匹配顺序
		doAdd<Key, Value, BtreeOrder, Value>(this, p, passedNodeTrackStack);

		return false; // TODO wait to modify
	}

	NODE_TEMPLATE
		bool
		BASE::remove(const Key& key)
	{
		auto maxValue = [](Ele& e) -> typename Ele::ValueForContent& {
			return e[e.count() - 1].second;
		};

		// TODO wrong
		// Keep internal node key count between w/2 and w
		if (!Middle) {
			elements_.remove(key);
			// check count
		}
		else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					// Recursive locate
				}
				else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					if (!subNodePtr->Middle) {
						subNodePtr->elements_.remove(key);
						// Check count
					}
					else {
						subNodePtr = Ele::ptr(maxValue(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}
		}

		return false; // TODO wait to modify
	}


	NODE_TEMPLATE
		void
		BASE::changeInSearchDownPath(const Key& oldKey, const Key& newKey)
	{
		vector<NodeBase*> trackStack{};

		searchWithSaveTrack(oldKey, trackStack);

		auto rend = trackStack.rend();
		for (auto rIter = trackStack.rbegin(); rIter != rend; ++rIter) {
			auto& node = *rIter;

			auto i = node.elements_.indexOf(oldKey);
			if (i != -1 && node->Middle) { // No need to change leaf
				node.elements_[i].first = newKey;
			}
		}

	}

	NODE_TEMPLATE
		Value*
		BASE::searchWithSaveTrack(const Key& key, vector<NodeBase*>& trackStack)
	{
		// TODO modify, or think of search
		// TODO maybe not need return value
		trackStack.push_back(this);

		auto maxValueForContent = [](Ele& e) {
			return e[e.count() - 1].second;
		};

		if (!Middle) {
			return elements_.have(key) ? &Ele::value(elements_[key]) : nullptr;
		}
		else {
			for (auto& e : elements_) {
				if ((*elements_.LessThanPtr)(key, e.first)) {
					return Ele::ptr(e.second)->searchWithSaveTrack(key, trackStack);
				}
				else if (!(*elements_.LessThanPtr)(e.first, key)) {
					auto subNodePtr = Ele::ptr(e.second);

				SearchInThisNode:
					trackStack.push_back(subNodePtr);

					if (!subNodePtr->Middle) {
						return &Ele::value(maxValueForContent(subNodePtr->elements_));
					}
					else {
						subNodePtr = Ele::ptr(maxValueForContent(subNodePtr->elements_));
						goto SearchInThisNode;
					}
				}
			}

			return nullptr;
		}
	}

	NODE_TEMPLATE
		void
		BASE::searchSiblingsIn(vector<NodeBase*>& passedNodeTrackStack, NodeBase*& previous, NodeBase*& next) const
	{

	}

	NODE_TEMPLATE
		template <typename T>
	void
		BASE::reallocateSiblingElement(bool isPrevious, NodeBase* sibling, vector<NodeBase*>& passedNodeTrackStack, pair<Key, T> p)
	{
		auto& stack = passedNodeTrackStack;

		if (isPrevious) {
			auto min = elements_.exchangeMin(p);
			auto previousOldMax = sibling->maxKey();
			sibling->elements_.append(min);
			auto newMaxKey = min.first;
			// previous max change
			// TODO could use this stack to get sibling stack, then change upper
			replacePreviousNodeMaxKeyInTreeBySearchUpIn(stack, previousOldMax, newMaxKey);
		}
		else {
			auto&& max = elements_.exchangeMax(p);
			// this max change
			changeMaxKeyIn(stack, maxKey());
			sibling->elements_.insert(p);
		}
	}

	NODE_TEMPLATE
		void
		BASE::changeMaxKeyIn(vector<NodeBase*>& passedNodeTrackStack, const Key& maxKey) const
	{
		auto& stack = passedNodeTrackStack;
		if (stack.size() < 2) {
			return;
		}

		auto  nodePtr = stack.back();
		stack.pop_back();
		auto& upperNode = *stack.back();
		// 以下这个修改 key 的部分可以复用
		auto  matchIndex = upperNode.elements_.indexOf(nodePtr);
		(upperNode.elements_)[matchIndex].first = maxKey;

		auto maxIndex = upperNode.childCount() - 1;
		if (matchIndex == maxIndex) {
			changeMaxKeyIn(stack, maxKey);
		}
	}

	// use stack to get root node(maybe not need root node), then use this early max key to search
	NODE_TEMPLATE
		void
		BASE::replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<NodeBase*>& passedNodeTrackStack, const Key& oldKey, const Key& newKey)
	{
		auto& stack = passedNodeTrackStack;
		auto emitUpperNode = [&] {
			auto last = stack.back();
			stack.pop_back();

			return last;
		};

		emitUpperNode(); // top pointer is leaf, it's useless


		for (NodeBase* node = emitUpperNode(); stack.size() != 0; node = emitUpperNode()) {
			// judge if the node is the same ancestor between this and previous node
			if (node->have(oldKey)) {
				node->changeInSearchDownPath(oldKey, newKey);

				// judge if need to change upper node
				auto i = node->elements_.indexOf(oldKey); // should create a method called this layer search
				auto maxIndex = childCount() - 1;
				if (i != -1 && i == maxIndex) {
					stack.push_back(node);
					// change upper node
					changeMaxKeyIn(stack, newKey);
					break;
				}
			}
		}
	}

	template <typename Key, typename Value, uint16_t BtreeOrder, typename T>
	void
		setSiblings(BASE*, BASE*);

	NODE_TEMPLATE
		template <typename T>
	void
		BASE::splitNode(pair<Key, T> p, vector<NodeBase*>& passedNodeTrackStack)
	{
		auto& key = p.first;
		auto& lessThan = *(elements_.LessThanPtr);
		auto& stack = passedNodeTrackStack;

		auto newPre = this->clone(); // TODO have a problem
		auto newPrePtr = newPre.get();
		// left is newPre, right is this
		setSiblings<Key, Value, BtreeOrder, T>(newPrePtr, this);

		auto i = elements_.suitablePosition(key);

		// [] not need reference if don't have last sentence?
		auto moveItems = [&](uint16_t preNodeRemoveCount) {
			newPrePtr->elements_.removeItemsFrom(false, preNodeRemoveCount);
			this->elements_.removeItemsFrom(true, BtreeOrder - preNodeRemoveCount);
		};

		auto addIn = [&](NodeBase* node, bool shouldAppend) {
			if (shouldAppend) {
				node->elements_.append(p);
			}
			else {
				node->elements_.insert(p);
			}
		};

#define HANDLE_ADD(leaf, maxBound) auto shouldAppend = (i == maxBound); do { addIn(leaf, shouldAppend); } while(0)

		constexpr bool odd = BtreeOrder % 2;
		constexpr auto middle = odd ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
		if (i <= middle) {
			constexpr auto removeCount = middle;
			moveItems(removeCount);

			HANDLE_ADD(newPrePtr, middle);
		}
		else {
			constexpr auto removeCount = BtreeOrder - middle;
			moveItems(removeCount);

			HANDLE_ADD(this, BtreeOrder);
			if (shouldAppend) {
				auto stackCopy = stack;
				changeMaxKeyIn(stackCopy, key);
			}
		}

#undef HANDLE_ADD

		insertLeafToUpper(newPrePtr, stack);
	}

	NODE_TEMPLATE
		void
		BASE::insertLeafToUpper(NodeBase* node, vector<NodeBase*>& passedNodeTrackStack) const
	{
#define EMIT_UPPER_NODE() stack.pop_back()

		// reduce useless node, prepare to upper level add, like start new leaf add
		auto& stack = passedNodeTrackStack;
		EMIT_UPPER_NODE();

		doAdd(this, make_pair<Key, NodeBase*>(node->maxKey(), node), stack);
		// 这部分甚至可能动 root，做好心理准备

#undef EMIT_UPPER_NODE
	}

#undef BASE
#undef NODE_TEMPLATE
}


namespace btree {
	template<typename Derived, typename Key, typename Value, uint16_t BtreeOrder>
	class NodeBase_CRTP : public NodeBase<Key, Value, BtreeOrder> {
	protected:
		using Base = NodeBase<Key, Value, BtreeOrder>;

	public:
		// provide same constructor of NodeBase
		using Base::Base;

		// could return Derived type?, I think
		virtual unique_ptr<Base> clone() const
		{
			// how to reduce the const mark
			return make_unique<Derived>(static_cast<Derived const &>(*this));
			// return unique_ptr<Derived>(new Derived(*this));
		}
	};
}

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define MIDDLE MiddleNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
		class MiddleNode : public NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder> {
		private:
			using          Base = NodeBase<Key, Value, BtreeOrder>;
			using          Base_CRTP = NodeBase_CRTP<MIDDLE, Key, Value, BtreeOrder>;
			using typename Base::LessThan;

		public:
			template <typename Iter>
			MiddleNode(Iter, Iter, shared_ptr<LessThan>);
			MiddleNode(const MiddleNode&);
			MiddleNode(MiddleNode&&) noexcept;
			~MiddleNode() override;

			// TODO Base is not past Base, maybe occur some problem
			Base*            minSon();
			void             collectAddInfo(const Key&, vector<Base*>&);
			Base*            operator[](const Key&);
			pair<Key, Base*> operator[](uint16_t);

		private:
			// void addBeyondMax(pair<Key, Value>);
	};
}

namespace btree {
	NODE_TEMPLATE
		template <typename Iter>
	MIDDLE::MiddleNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(MiddleFlag(), begin, end, funcPtr)
	{ }

	NODE_TEMPLATE
		MIDDLE::MiddleNode(const MiddleNode& that)
		: Base_CRTP(that) // TODO wrong work
	{
		// for (auto& e : Base::elements_) {
		// 	// Set to corresponding this elements_ position
		// 	Base::Ele::ptr(e.second)->clone();
		// }
	}

	NODE_TEMPLATE
		MIDDLE::MiddleNode(MiddleNode&& that) noexcept
		: Base_CRTP(std::move(that))
	{ }

	NODE_TEMPLATE
		MIDDLE::~MiddleNode() = default;

	NODE_TEMPLATE
		typename MIDDLE::Base*
		MIDDLE::minSon()
	{
		auto& es = Base::elements_;

		return Base::Ele::ptr(es[0].second);
	}

	NODE_TEMPLATE
		typename MIDDLE::Base*
		MIDDLE::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::ptr(e);
	}

	NODE_TEMPLATE
		pair<Key, typename MIDDLE::Base*>
		MIDDLE::operator[](uint16_t i)
	{
		auto& e = this->elements_[i];
		// TODO because this is pass by value, can't be used by directly
		return make_pair(e.first, Base::Ele::ptr(e.second));
	}

	NODE_TEMPLATE
		void
		MIDDLE::collectAddInfo(const Key& key, vector<Base*>& passedNodeTrackStack)
	{
		//using Base::elements_;
		// auto& pointer  = Base::elements_.ptr;
		//using Base::Ele::ptr;
		auto& elements = Base::elements_;
		auto& lessThan = *(elements.LessThanPtr);
		auto& stack = passedNodeTrackStack;

		stack.push_back(this);

#define KEY_OF_ELE e.first
#define VALUE_OF_ELE e.second
#define LAST_ONE (count - 1)
#define PTR(E) Base::Ele::ptr(E)

		auto count = elements.count();
		for (auto i = 0; i < count; ++i) {
			auto& e = elements[i];

			if (lessThan(key, KEY_OF_ELE) || i == LAST_ONE) {
				auto subNodePtr = PTR(VALUE_OF_ELE);

				if (subNodePtr->Middle) {
					static_cast<MiddleNode*>(subNodePtr)->collectAddInfo(key, stack);
				}
				else {
					stack.push_back(subNodePtr);
				}

				return;
			}
		}

#undef PTR
#undef LAST_ONE
#undef VALUE_OF_ELE
#undef KEY_OF_ELE
	}

	// TODO have problem, think about this method
	// Name maybe not very accurate, because not must beyond
	// NODE_TEMPLATE
	// void
	// MIDDLE::addBeyondMax(pair<Key, Value> p)
	// {
	// 	using Base::elements_;
	// 	using Base::Ele::ptr;
	// 	// Ptr append will some thing different
	// 	if (!Base::full()) {
	// 		elements_.append(p);
	// 		// update bound related info
	// 	} else if (/*sibling space free*/) {
	// 		// move some element to sibling
	// 	} else {
	// 		// split node into two
	// 	}
	// 	// Think about this way: should add new right most node to expand max bound
	//
	// }

#undef MIDDLE
#undef NODE_TEMPLATE
}

#pragma once
#include <utility>
#include "NodeBaseCrtp.hpp"

namespace btree {
#define NODE_TEMPLATE template <typename Key, typename Value, uint16_t BtreeOrder>
#define LEAF LeafNode<Key, Value, BtreeOrder>

	NODE_TEMPLATE
		class LeafNode : public NodeBase_CRTP<LEAF, Key, Value, BtreeOrder> {
		using Base = NodeBase<Key, Value, BtreeOrder>;
		using Base_CRTP = NodeBase_CRTP<LEAF, Key, Value, BtreeOrder>;
		using typename Base::LessThan;

		public:
			template <typename Iterator>
			LeafNode(Iterator, Iterator, shared_ptr<LessThan>);
			LeafNode(const LeafNode&, LeafNode* previous = nullptr, LeafNode* next = nullptr);
			LeafNode(LeafNode&&) noexcept;
			~LeafNode() override;

			const Value&     operator[](const Key&);
			pair<Key, Value> operator[](uint16_t);
			void             remove(const Key&);
			inline LeafNode* nextLeaf() const;
			inline void      nextLeaf(LeafNode*);
			inline LeafNode* previousLeaf() const;
			inline void      previousLeaf(LeafNode*);

		private:
			// TODO remember to initialize two property below
			LeafNode* _next{ nullptr };
			LeafNode* _previous{ nullptr };

			// template default is inline?
			// inline void insertLeafToUpper(LeafNode*, vector<Base*>) const;
	};
}

namespace btree {
	NODE_TEMPLATE
		template <typename Iter>
	LEAF::LeafNode(Iter begin, Iter end, shared_ptr<LessThan> funcPtr)
		: Base_CRTP(LeafFlag(), begin, end, funcPtr)
	{}

	NODE_TEMPLATE
		LeafNode<Key, Value, BtreeOrder>::LeafNode(const LeafNode& that, LeafNode* previous, LeafNode* next)
		: Base_CRTP(that), _previous(previous), _next(next)
	{}

	NODE_TEMPLATE
		LeafNode<Key, Value, BtreeOrder>::LeafNode(LeafNode&& that) noexcept
		: Base_CRTP(std::move(that)), _next(that._next)
	{}

	NODE_TEMPLATE
		LeafNode<Key, Value, BtreeOrder >::~LeafNode() = default;

	NODE_TEMPLATE
		const Value&
		LEAF::operator[](const Key& key)
	{
		auto& e = this->elements_[key];
		return Base::Ele::value(e);
	}

	NODE_TEMPLATE
		pair<Key, Value>
		LEAF::operator[](uint16_t i)
	{
		auto& e = Base::elements_[i];
		return make_pair(e.first, Base::Ele::value(e.second));
	}

	// NODE_TEMPLATE
	// bool
	// LEAF::add(pair<Key, Value> p, vector<Base*>& passedNodeTrackStack)
	// {
	// 	using Base::full;
	// 	using Base::elements_;
	// 	auto& k = p.first;
	// 	auto& lessThan = *(elements_.LessThanPtr);
	// 	auto& stack = passedNodeTrackStack;
	// 	stack.push_back(this);

	// 	if (!full()) {
	// 		if (lessThan(k, Base::maxKey())) {
	// 			elements_.insert(p);
	// 		} else {
	// 			elements_.append(p);
	// 			changeMaxKeyIn(stack, k);
	// 		}
	// 		// why here not Base::spaceFreeIn()?
	// 	} else if (spaceFreeIn(_previous)) {
	// 		siblingElementReallocate(true, stack, p);
	// 	} else if (spaceFreeIn(_next)) {
	// 		siblingElementReallocate(false, stack, p);
	// 	} else {
	// 		splitNode(p, stack);
	// 	}
	// }

	NODE_TEMPLATE
		void
		LEAF::remove(const Key& key)
	{
		if (this->elements_.remove(key)) {
			//			btree_->change_bound_upwards(this,)
						// TODO
		}
	}

	NODE_TEMPLATE
		LEAF*
		LEAF::nextLeaf() const
	{
		return _next;
	}

	NODE_TEMPLATE
		void
		LEAF::nextLeaf(LeafNode* next)
	{
		_next = next;
	}

	NODE_TEMPLATE
		void
		LEAF::previousLeaf(LeafNode* previous)
	{
		_previous = previous;
	}

	NODE_TEMPLATE
		LEAF*
		LEAF::previousLeaf() const
	{
		return _previous;
	}

	// TODO If you want to fine all the node, you could leave a gap "fineAllocate" to fine global allocate

	// NODE_TEMPLATE
	// void
	// LEAF::siblingElementReallocate(bool isPrevious, vector<Base*>& passedNodeTrackStack, pair<Key, Value> p)
	// {
	// 	auto& stack = passedNodeTrackStack;

	// 	if (isPrevious) {
	// 		auto&& min = Base::elements_.exchangeMin(p); // exchange need to provide key, PtrType version
	// 		auto&& previousOldMax = _previous->maxKey();
	// 		_previous->elements_.append(min);
	// 		// previous max change
	// 		replacePreviousNodeMaxKeyInTreeBySearchUpIn(stack, _previous, min);
	// 	} else {
	// 		auto&& max = Base::elements_.exchangeMax(p);
	// 		// this max change
	// 		changeMaxKeyIn(stack, Base::maxKey());
	// 		_next->elements_.insert(p);
	// 	}
	// }

// 	NODE_TEMPLATE
// 	void
// 	LEAF::splitNode(pair<Key, Value> p, vector<Base*>& passedNodeTrackStack)
// 	{
// 		auto& key = p.first;
// 		auto& lessThan = *(Base::Ele::LessThanPtr);
// 		auto& stack = passedNodeTrackStack;

// 		// construct a new one, left is newPre, right is this
// 		auto newPre = make_unique<LeafNode>(*this, _previous, this);
// 		auto newPrePtr = newPre.get();
// 		// update previous
// 		this->previousLeaf(newPrePtr);

// 		auto i = Base::elements_.suitablePosition(key);

// 		// [] not need reference if don't have last sentence?
// 		auto moveItems = [] (uint16_t preNodeRemoveCount) {
// 			newPrePtr->elements_.removeItemsFrom(false, preNodeRemoveCount);
// 			Base::     elements_.removeItemsFrom(true,  BtreeOrder - preNodeRemoveCount);
// 		};

// 		auto addIn = [&] (LeafNode* leaf, bool shouldAppend) {
// 			if (shouldAppend) {
// 				leaf->elements_.append(p);
// 			} else {
// 				leaf->elements_.insert(p);
// 			}
// 		};

// #define HANDLE_ADD(leaf, maxBound) auto shouldAppend = (i == maxBound); do { addIn(leaf, shouldAppend); } while(0)

// 		constexpr bool odd = BtreeOrder % 2;
// 		constexpr auto middle = odd ? (BtreeOrder / 2 + 1) : (BtreeOrder / 2);
// 		if (i <= middle) {
// 			constexpr auto removeCount = middle;
// 			moveItems(removeCount);

// 			HANDLE_ADD(newPrePtr, middle);
// 		} else {
// 			constexpr auto removeCount = BtreeOrder - middle;
// 			moveItems(removeCount);

// 			HANDLE_ADD(this, BtreeOrder);
// 			if (shouldAppend) {
// 				changeMaxKeyIn(stack, key);
// 			}
// 		}

// #undef HANDLE_ADD

// 		insertLeafToUpper(newPrePtr, stack); // stack is copied
// 	}

	// NODE_TEMPLATE
	// void
	// LEAF::changeMaxKeyIn(vector<Base*>& passedNodeTrackStack, const Key& maxKey) const
	// {
	// 	auto& stack = passedNodeTrackStack;
	// 	if (stack.size() < 2) {
	// 		return;
	// 	}

	// 	auto  nodePtr = stack.pop_back();
	// 	auto& upperNode = *(stack[stack.size() - 1]);
	// 	// 以下这个修改 key 的部分可以复用
	// 	auto  matchIndex = upperNode.elements_.indexOf(nodePtr);
	// 	upperNode[matchIndex].first = maxKey;

	// 	auto maxIndex = upperNode->childCount() - 1;
	// 	if (matchIndex == maxIndex) {
	// 		changeMaxKeyIn(stack, maxKey);
	// 	}
	// }

// 	// use stack to get root node(maybe not need root node), then use this early max key to search
// 	NODE_TEMPLATE
// 	void
// 	LEAF::replacePreviousNodeMaxKeyInTreeBySearchUpIn(vector<Base*>& passedNodeTrackStack, const Key& oldKey, const Key& newKey)
// 	{
// 		auto& stack = passedNodeTrackStack;
// #define EMIT_UPPER_NODE() stack.pop_back()

// 		EMIT_UPPER_NODE(); // top pointer is leaf, it's useless

// 		for (Base* node = EMIT_UPPER_NODE(); stack.size() != 0; node = EMIT_UPPER_NODE()) {
// 			// judge if the node is the same ancestor between this and previous node
// 			if (node->have(oldKey)) {
// 				node->changeInSearchDownPath(oldKey, newKey);

// 				// judge if need to change upper node
// 				auto i = node->elements_.indexOf(oldKey); // should create a method called this layer search
// 				auto maxIndex = Base::childCount() - 1;
// 				if (i != -1 && i == maxIndex) {
// 					stack.push_back(node);
// 					// change upper node
// 					changeMaxKeyIn(stack, newKey);
// 					break;
// 				}
// 			}
// 		}
// 	}

// 	NODE_TEMPLATE
// 	void
// 	LEAF::insertLeafToUpper(LeafNode* leaf, vector<Base*> passedNodeTrackStack) const
// 	{
// #define EMIT_UPPER_NODE() stack.pop_back()
//
// 		auto& stack = passedNodeTrackStack;
// 		// reduce the useless node, prepare to upper level add, just like start new leaf add
// 		EMIT_UPPER_NODE(); // top pointer is leaf, it's useless
//
// 		doAdd(make_pair<Key, Base*>(leaf->maxKey(), leaf), stack);
// 		// 这部分甚至可能动 root，做好心理准备
//
// #undef EMIT_UPPER_NODE
// 	}

#undef LEAF
#undef NODE_TEMPLATE
}
