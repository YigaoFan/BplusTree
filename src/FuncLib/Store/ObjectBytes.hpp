#pragma once
#include <cstddef>
#include <fstream>
#include <vector>
#include "StaticConfig.hpp"
#include "../../Btree/Enumerator.hpp"
#include "ObjectBytesQueue.hpp"
#include "LabelNodeBase.hpp"

namespace FuncLib::Store
{
	using Collections::CreateRefEnumerator;
	using ::std::ofstream;
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

		using Base::AddSub;
		using Base::CreateSortedSubNodeEnumerator;
		using Base::Label;

		void WriteIn(ofstream* fileStream, pos_int pos) const;
		size_t Size() const;
		void Add(char const* begin, size_t size);
		void AddBlank(size_t count);
	private:
		void DoWrite(ofstream* filename, pos_int& pos) const;
	};
}