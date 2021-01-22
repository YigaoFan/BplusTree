#pragma once
#include <cstddef>
#include <fstream>
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"
#include "ObjectRelation/LabelNodeBase.hpp"

namespace FuncLib::Store
{
	using namespace FuncLib::Store::ObjectRelation;
	using Collections::CreateRefEnumerator;
	using ::std::fstream;
	using ::std::size_t;
	using ::std::vector;

	class WriteQueue;
	class AllocateSpaceQueue;
	class ResizeSpaceQueue;

	class ObjectBytes : private LabelNodeBase<ObjectBytes>
	{
	private:
		using Base = LabelNodeBase<ObjectBytes>;
		vector<char> _bytes;

	public:
		static constexpr char Blank = ' ';

		WriteQueue* ToWrites = nullptr;
		AllocateSpaceQueue* ToAllocates = nullptr;
		ResizeSpaceQueue* ToResizes = nullptr;

		ObjectBytes(pos_label label, WriteQueue* writeQueuen = nullptr, AllocateSpaceQueue* allocateQueue = nullptr, ResizeSpaceQueue* resizeQueue = nullptr);
		ObjectBytes(pos_label label, vector<char> bytes);
		
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