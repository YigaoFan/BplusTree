#pragma once
#include <cstddef>
#include <fstream>
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"
#include "ObjectBytesQueue.hpp"
#include "ObjectRelation/LabelNodeBase.hpp"

namespace FuncLib::Store
{
	using namespace FuncLib::Store::ObjectRelation;
	using Collections::CreateRefEnumerator;
	using ::std::fstream;
	using ::std::size_t;
	using ::std::vector;

	class ObjectBytes : private LabelNodeBase<ObjectBytes>
	{
	private:
		using Base = LabelNodeBase<ObjectBytes>;
		vector<char> _bytes;

	public:
		static constexpr char Blank = ' ';

		WriteQueue* ToWrites;
		AllocateSpaceQueue* ToAllocates;
		ResizeSpaceQueue* ToResizes;

		ObjectBytes(pos_label label, WriteQueue* writeQueuen = nullptr, AllocateSpaceQueue* allocateQueue = nullptr, ResizeSpaceQueue* resizeQueue = nullptr);
		
		ObjectBytes* ConstructSub(pos_label label);
		using Base::CreateSortedSubNodeEnumerator;
		using Base::Label;

		bool Written() const;
		void WriteIn(fstream* fileStream, pos_int pos) const;
		void WriteIn(auto const& writer) const
		{
			writer(&_bytes);
		}
		size_t Size() const;
		void Add(char const* begin, size_t size);
		void AddBlank(size_t count);
	};
}