#pragma once
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>
#include "StaticConfig.hpp"
#include "LableRelationNode.hpp"
#include "../../Btree/Enumerator.hpp"

namespace FuncLib::Store
{
	using Collections::CreateEnumerator;
	using ::std::ofstream;
	using ::std::shared_ptr;
	using ::std::size_t;
	using ::std::vector;
	using ::std::filesystem::path;

	void WriteByte(path const& filename, pos_int start, char const* begin, size_t size);
	void DuplicateWriteByte(path const& filename, pos_int start, size_t count, char ch);

	class ObjectBytes
	{
	private:
		pos_lable _lable;
		vector<char> _bytes;
		vector<ObjectBytes*> _subObjectBytes;
	public:
		ObjectBytes(pos_lable lable);
		ObjectBytes(ObjectBytes const& that) = delete;
		ObjectBytes(ObjectBytes&& that) noexcept = default;
		
		void WriteIn(path const& filename, pos_int pos) const;

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