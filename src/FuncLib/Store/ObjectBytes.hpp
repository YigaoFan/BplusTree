#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <vector>
#include "StaticConfig.hpp"
#include "LableRelationNode.hpp"
#include "../../Btree/Enumerator.hpp"
#include "ObjectBytesQueue.hpp"

namespace FuncLib::Store
{
	using Collections::CreateEnumerator;
	using ::std::ofstream;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	class WriteQueue;
	class AllocateSpaceQueue;

	class ObjectBytes
	{
	private:
		pos_lable _lable;
		vector<char> _bytes;
		vector<ObjectBytes*> _subObjectBytes;// 用 unique_ptr TODO
	public:
		WriteQueue* ToWrites;
		AllocateSpaceQueue* ToAllocates;
		ResizeSpaceQueue* ToResizes;

		// remove nullptr TODO
		ObjectBytes(pos_lable lable, WriteQueue* writeQueuen = nullptr, AllocateSpaceQueue* allocateQueue = nullptr, ResizeSpaceQueue* resizeQueue = nullptr);
		ObjectBytes(ObjectBytes const& that) = delete;
		ObjectBytes(ObjectBytes&& that) noexcept = default;
		
		void WriteIn(ofstream* fileStream, pos_int pos) const;

		void AddSub(ObjectBytes* subObjectByte);
		pos_lable Lable() const;
		auto GetLableSortedSubsEnumerator() const { return CreateEnumerator(_subObjectBytes); }

		size_t Size() const;

		void Add(char const* begin, size_t size);
		void AddBlank(size_t count);
	private:
		void DoWrite(ofstream* filename, pos_int& pos) const;
	};
}