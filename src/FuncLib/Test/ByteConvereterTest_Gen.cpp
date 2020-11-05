#include <string>
#include <memory>
#include "../../TestFrame/FlyTest.hpp"
#include "../Persistence/ByteConverter.hpp"
#include "../Store/ObjectBytes.hpp"
#include "../Store/FileReader.hpp"
#include "Util.hpp"

using namespace FuncLib;
using namespace FuncLib::Store;
using namespace FuncLib::Test;
using namespace FuncLib::Persistence;
using namespace std;

TESTCASE("Byte converter test")
{
	auto filename = "byteConverterTest";
	auto path = MakeFilePath(filename);
	auto writer = ObjectBytes(0);// TODO 生成器要加下这个 pos_lable 0
	auto reader = FileReader(path, 0);
	SECTION("section0")
	{
		using T = int;
		T t =
			0;
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section1")
	{
		using T = bool;
		T t =
			true;
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section2")
	{
		using T = float;
		T t =
			1.23F;
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section3")
	{
		using T = double;
		T t =
			2.34;
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section4")
	{
		using T = char;
		T t =
			'a';
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section5")
	{
		using T = string;
		T t =
			"Hello World";
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section6")
	{
		using T = OwnerLessDiskPtr<int>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section7")
	{
		using T = OwnerLessDiskPtr<bool>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section8")
	{
		using T = OwnerLessDiskPtr<float>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section9")
	{
		using T = OwnerLessDiskPtr<double>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section10")
	{
		using T = OwnerLessDiskPtr<char>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section11")
	{
		using T = OwnerLessDiskPtr<string>;
		T t =
			{
				{nullptr, 0}, nullptr};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section12")
	{
		using T = DiskPos<int>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section13")
	{
		using T = DiskPos<bool>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section14")
	{
		using T = DiskPos<float>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section15")
	{
		using T = DiskPos<double>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section16")
	{
		using T = DiskPos<char>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section17")
	{
		using T = DiskPos<string>;
		T t =
			{
				nullptr, 0};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section18")
	{
		using T = LiteVector<int, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section19")
	{
		using T = LiteVector<bool, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section20")
	{
		using T = LiteVector<float, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section21")
	{
		using T = LiteVector<double, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section22")
	{
		using T = LiteVector<char, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section23")
	{
		using T = LiteVector<string, size_t, 10>;
		T t =
			{

			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section24")
	{
		struct CompoundType_0
		{
			int Member0;
			int Member1;
		};
		using T = CompoundType_0;
		T t =
			{
				0,
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section25")
	{
		struct CompoundType_1
		{
			bool Member0;
			int Member1;
		};
		using T = CompoundType_1;
		T t =
			{
				true,
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section26")
	{
		struct CompoundType_2
		{
			float Member0;
			int Member1;
		};
		using T = CompoundType_2;
		T t =
			{
				1.23F,
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section27")
	{
		struct CompoundType_3
		{
			double Member0;
			int Member1;
		};
		using T = CompoundType_3;
		T t =
			{
				2.34,
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section28")
	{
		struct CompoundType_4
		{
			char Member0;
			int Member1;
		};
		using T = CompoundType_4;
		T t =
			{
				'a',
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section29")
	{
		struct CompoundType_5
		{
			string Member0;
			int Member1;
		};
		using T = CompoundType_5;
		T t =
			{
				"Hello World",
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section30")
	{
		struct CompoundType_6
		{
			OwnerLessDiskPtr<int> Member0;
			int Member1;
		};
		using T = CompoundType_6;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section31")
	{
		struct CompoundType_7
		{
			OwnerLessDiskPtr<bool> Member0;
			int Member1;
		};
		using T = CompoundType_7;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section32")
	{
		struct CompoundType_8
		{
			OwnerLessDiskPtr<float> Member0;
			int Member1;
		};
		using T = CompoundType_8;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section33")
	{
		struct CompoundType_9
		{
			OwnerLessDiskPtr<double> Member0;
			int Member1;
		};
		using T = CompoundType_9;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section34")
	{
		struct CompoundType_10
		{
			OwnerLessDiskPtr<char> Member0;
			int Member1;
		};
		using T = CompoundType_10;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section35")
	{
		struct CompoundType_11
		{
			OwnerLessDiskPtr<string> Member0;
			int Member1;
		};
		using T = CompoundType_11;
		T t =
			{
				{{nullptr, 0}, nullptr},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section36")
	{
		struct CompoundType_12
		{
			DiskPos<int> Member0;
			int Member1;
		};
		using T = CompoundType_12;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section37")
	{
		struct CompoundType_13
		{
			DiskPos<bool> Member0;
			int Member1;
		};
		using T = CompoundType_13;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section38")
	{
		struct CompoundType_14
		{
			DiskPos<float> Member0;
			int Member1;
		};
		using T = CompoundType_14;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section39")
	{
		struct CompoundType_15
		{
			DiskPos<double> Member0;
			int Member1;
		};
		using T = CompoundType_15;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section40")
	{
		struct CompoundType_16
		{
			DiskPos<char> Member0;
			int Member1;
		};
		using T = CompoundType_16;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section41")
	{
		struct CompoundType_17
		{
			DiskPos<string> Member0;
			int Member1;
		};
		using T = CompoundType_17;
		T t =
			{
				{nullptr, 0},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section42")
	{
		struct CompoundType_18
		{
			LiteVector<int, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_18;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section43")
	{
		struct CompoundType_19
		{
			LiteVector<bool, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_19;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section44")
	{
		struct CompoundType_20
		{
			LiteVector<float, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_20;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section45")
	{
		struct CompoundType_21
		{
			LiteVector<double, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_21;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section46")
	{
		struct CompoundType_22
		{
			LiteVector<char, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_22;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section47")
	{
		struct CompoundType_23
		{
			LiteVector<string, size_t, 10> Member0;
			int Member1;
		};
		using T = CompoundType_23;
		T t =
			{
				{

				},
				0,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section48")
	{
		struct CompoundType_24
		{
			int Member0;
			bool Member1;
		};
		using T = CompoundType_24;
		T t =
			{
				0,
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section49")
	{
		struct CompoundType_25
		{
			bool Member0;
			bool Member1;
		};
		using T = CompoundType_25;
		T t =
			{
				true,
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section50")
	{
		struct CompoundType_26
		{
			float Member0;
			bool Member1;
		};
		using T = CompoundType_26;
		T t =
			{
				1.23F,
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section51")
	{
		struct CompoundType_27
		{
			double Member0;
			bool Member1;
		};
		using T = CompoundType_27;
		T t =
			{
				2.34,
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section52")
	{
		struct CompoundType_28
		{
			char Member0;
			bool Member1;
		};
		using T = CompoundType_28;
		T t =
			{
				'a',
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section53")
	{
		struct CompoundType_29
		{
			string Member0;
			bool Member1;
		};
		using T = CompoundType_29;
		T t =
			{
				"Hello World",
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section54")
	{
		struct CompoundType_30
		{
			OwnerLessDiskPtr<int> Member0;
			bool Member1;
		};
		using T = CompoundType_30;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section55")
	{
		struct CompoundType_31
		{
			OwnerLessDiskPtr<bool> Member0;
			bool Member1;
		};
		using T = CompoundType_31;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section56")
	{
		struct CompoundType_32
		{
			OwnerLessDiskPtr<float> Member0;
			bool Member1;
		};
		using T = CompoundType_32;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section57")
	{
		struct CompoundType_33
		{
			OwnerLessDiskPtr<double> Member0;
			bool Member1;
		};
		using T = CompoundType_33;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section58")
	{
		struct CompoundType_34
		{
			OwnerLessDiskPtr<char> Member0;
			bool Member1;
		};
		using T = CompoundType_34;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section59")
	{
		struct CompoundType_35
		{
			OwnerLessDiskPtr<string> Member0;
			bool Member1;
		};
		using T = CompoundType_35;
		T t =
			{
				{{nullptr, 0}, nullptr},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section60")
	{
		struct CompoundType_36
		{
			DiskPos<int> Member0;
			bool Member1;
		};
		using T = CompoundType_36;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section61")
	{
		struct CompoundType_37
		{
			DiskPos<bool> Member0;
			bool Member1;
		};
		using T = CompoundType_37;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section62")
	{
		struct CompoundType_38
		{
			DiskPos<float> Member0;
			bool Member1;
		};
		using T = CompoundType_38;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section63")
	{
		struct CompoundType_39
		{
			DiskPos<double> Member0;
			bool Member1;
		};
		using T = CompoundType_39;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section64")
	{
		struct CompoundType_40
		{
			DiskPos<char> Member0;
			bool Member1;
		};
		using T = CompoundType_40;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section65")
	{
		struct CompoundType_41
		{
			DiskPos<string> Member0;
			bool Member1;
		};
		using T = CompoundType_41;
		T t =
			{
				{nullptr, 0},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section66")
	{
		struct CompoundType_42
		{
			LiteVector<int, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_42;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section67")
	{
		struct CompoundType_43
		{
			LiteVector<bool, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_43;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section68")
	{
		struct CompoundType_44
		{
			LiteVector<float, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_44;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section69")
	{
		struct CompoundType_45
		{
			LiteVector<double, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_45;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section70")
	{
		struct CompoundType_46
		{
			LiteVector<char, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_46;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section71")
	{
		struct CompoundType_47
		{
			LiteVector<string, size_t, 10> Member0;
			bool Member1;
		};
		using T = CompoundType_47;
		T t =
			{
				{

				},
				true,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section72")
	{
		struct CompoundType_48
		{
			int Member0;
			float Member1;
		};
		using T = CompoundType_48;
		T t =
			{
				0,
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section73")
	{
		struct CompoundType_49
		{
			bool Member0;
			float Member1;
		};
		using T = CompoundType_49;
		T t =
			{
				true,
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section74")
	{
		struct CompoundType_50
		{
			float Member0;
			float Member1;
		};
		using T = CompoundType_50;
		T t =
			{
				1.23F,
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section75")
	{
		struct CompoundType_51
		{
			double Member0;
			float Member1;
		};
		using T = CompoundType_51;
		T t =
			{
				2.34,
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section76")
	{
		struct CompoundType_52
		{
			char Member0;
			float Member1;
		};
		using T = CompoundType_52;
		T t =
			{
				'a',
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section77")
	{
		struct CompoundType_53
		{
			string Member0;
			float Member1;
		};
		using T = CompoundType_53;
		T t =
			{
				"Hello World",
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section78")
	{
		struct CompoundType_54
		{
			OwnerLessDiskPtr<int> Member0;
			float Member1;
		};
		using T = CompoundType_54;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section79")
	{
		struct CompoundType_55
		{
			OwnerLessDiskPtr<bool> Member0;
			float Member1;
		};
		using T = CompoundType_55;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section80")
	{
		struct CompoundType_56
		{
			OwnerLessDiskPtr<float> Member0;
			float Member1;
		};
		using T = CompoundType_56;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section81")
	{
		struct CompoundType_57
		{
			OwnerLessDiskPtr<double> Member0;
			float Member1;
		};
		using T = CompoundType_57;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section82")
	{
		struct CompoundType_58
		{
			OwnerLessDiskPtr<char> Member0;
			float Member1;
		};
		using T = CompoundType_58;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section83")
	{
		struct CompoundType_59
		{
			OwnerLessDiskPtr<string> Member0;
			float Member1;
		};
		using T = CompoundType_59;
		T t =
			{
				{{nullptr, 0}, nullptr},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section84")
	{
		struct CompoundType_60
		{
			DiskPos<int> Member0;
			float Member1;
		};
		using T = CompoundType_60;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section85")
	{
		struct CompoundType_61
		{
			DiskPos<bool> Member0;
			float Member1;
		};
		using T = CompoundType_61;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section86")
	{
		struct CompoundType_62
		{
			DiskPos<float> Member0;
			float Member1;
		};
		using T = CompoundType_62;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section87")
	{
		struct CompoundType_63
		{
			DiskPos<double> Member0;
			float Member1;
		};
		using T = CompoundType_63;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section88")
	{
		struct CompoundType_64
		{
			DiskPos<char> Member0;
			float Member1;
		};
		using T = CompoundType_64;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section89")
	{
		struct CompoundType_65
		{
			DiskPos<string> Member0;
			float Member1;
		};
		using T = CompoundType_65;
		T t =
			{
				{nullptr, 0},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section90")
	{
		struct CompoundType_66
		{
			LiteVector<int, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_66;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section91")
	{
		struct CompoundType_67
		{
			LiteVector<bool, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_67;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section92")
	{
		struct CompoundType_68
		{
			LiteVector<float, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_68;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section93")
	{
		struct CompoundType_69
		{
			LiteVector<double, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_69;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section94")
	{
		struct CompoundType_70
		{
			LiteVector<char, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_70;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section95")
	{
		struct CompoundType_71
		{
			LiteVector<string, size_t, 10> Member0;
			float Member1;
		};
		using T = CompoundType_71;
		T t =
			{
				{

				},
				1.23F,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section96")
	{
		struct CompoundType_72
		{
			int Member0;
			double Member1;
		};
		using T = CompoundType_72;
		T t =
			{
				0,
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section97")
	{
		struct CompoundType_73
		{
			bool Member0;
			double Member1;
		};
		using T = CompoundType_73;
		T t =
			{
				true,
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section98")
	{
		struct CompoundType_74
		{
			float Member0;
			double Member1;
		};
		using T = CompoundType_74;
		T t =
			{
				1.23F,
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section99")
	{
		struct CompoundType_75
		{
			double Member0;
			double Member1;
		};
		using T = CompoundType_75;
		T t =
			{
				2.34,
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section100")
	{
		struct CompoundType_76
		{
			char Member0;
			double Member1;
		};
		using T = CompoundType_76;
		T t =
			{
				'a',
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section101")
	{
		struct CompoundType_77
		{
			string Member0;
			double Member1;
		};
		using T = CompoundType_77;
		T t =
			{
				"Hello World",
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section102")
	{
		struct CompoundType_78
		{
			OwnerLessDiskPtr<int> Member0;
			double Member1;
		};
		using T = CompoundType_78;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section103")
	{
		struct CompoundType_79
		{
			OwnerLessDiskPtr<bool> Member0;
			double Member1;
		};
		using T = CompoundType_79;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section104")
	{
		struct CompoundType_80
		{
			OwnerLessDiskPtr<float> Member0;
			double Member1;
		};
		using T = CompoundType_80;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section105")
	{
		struct CompoundType_81
		{
			OwnerLessDiskPtr<double> Member0;
			double Member1;
		};
		using T = CompoundType_81;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section106")
	{
		struct CompoundType_82
		{
			OwnerLessDiskPtr<char> Member0;
			double Member1;
		};
		using T = CompoundType_82;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section107")
	{
		struct CompoundType_83
		{
			OwnerLessDiskPtr<string> Member0;
			double Member1;
		};
		using T = CompoundType_83;
		T t =
			{
				{{nullptr, 0}, nullptr},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section108")
	{
		struct CompoundType_84
		{
			DiskPos<int> Member0;
			double Member1;
		};
		using T = CompoundType_84;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section109")
	{
		struct CompoundType_85
		{
			DiskPos<bool> Member0;
			double Member1;
		};
		using T = CompoundType_85;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section110")
	{
		struct CompoundType_86
		{
			DiskPos<float> Member0;
			double Member1;
		};
		using T = CompoundType_86;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section111")
	{
		struct CompoundType_87
		{
			DiskPos<double> Member0;
			double Member1;
		};
		using T = CompoundType_87;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section112")
	{
		struct CompoundType_88
		{
			DiskPos<char> Member0;
			double Member1;
		};
		using T = CompoundType_88;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section113")
	{
		struct CompoundType_89
		{
			DiskPos<string> Member0;
			double Member1;
		};
		using T = CompoundType_89;
		T t =
			{
				{nullptr, 0},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section114")
	{
		struct CompoundType_90
		{
			LiteVector<int, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_90;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section115")
	{
		struct CompoundType_91
		{
			LiteVector<bool, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_91;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section116")
	{
		struct CompoundType_92
		{
			LiteVector<float, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_92;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section117")
	{
		struct CompoundType_93
		{
			LiteVector<double, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_93;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section118")
	{
		struct CompoundType_94
		{
			LiteVector<char, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_94;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section119")
	{
		struct CompoundType_95
		{
			LiteVector<string, size_t, 10> Member0;
			double Member1;
		};
		using T = CompoundType_95;
		T t =
			{
				{

				},
				2.34,
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section120")
	{
		struct CompoundType_96
		{
			int Member0;
			char Member1;
		};
		using T = CompoundType_96;
		T t =
			{
				0,
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section121")
	{
		struct CompoundType_97
		{
			bool Member0;
			char Member1;
		};
		using T = CompoundType_97;
		T t =
			{
				true,
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section122")
	{
		struct CompoundType_98
		{
			float Member0;
			char Member1;
		};
		using T = CompoundType_98;
		T t =
			{
				1.23F,
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section123")
	{
		struct CompoundType_99
		{
			double Member0;
			char Member1;
		};
		using T = CompoundType_99;
		T t =
			{
				2.34,
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section124")
	{
		struct CompoundType_100
		{
			char Member0;
			char Member1;
		};
		using T = CompoundType_100;
		T t =
			{
				'a',
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section125")
	{
		struct CompoundType_101
		{
			string Member0;
			char Member1;
		};
		using T = CompoundType_101;
		T t =
			{
				"Hello World",
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section126")
	{
		struct CompoundType_102
		{
			OwnerLessDiskPtr<int> Member0;
			char Member1;
		};
		using T = CompoundType_102;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section127")
	{
		struct CompoundType_103
		{
			OwnerLessDiskPtr<bool> Member0;
			char Member1;
		};
		using T = CompoundType_103;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section128")
	{
		struct CompoundType_104
		{
			OwnerLessDiskPtr<float> Member0;
			char Member1;
		};
		using T = CompoundType_104;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section129")
	{
		struct CompoundType_105
		{
			OwnerLessDiskPtr<double> Member0;
			char Member1;
		};
		using T = CompoundType_105;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section130")
	{
		struct CompoundType_106
		{
			OwnerLessDiskPtr<char> Member0;
			char Member1;
		};
		using T = CompoundType_106;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section131")
	{
		struct CompoundType_107
		{
			OwnerLessDiskPtr<string> Member0;
			char Member1;
		};
		using T = CompoundType_107;
		T t =
			{
				{{nullptr, 0}, nullptr},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section132")
	{
		struct CompoundType_108
		{
			DiskPos<int> Member0;
			char Member1;
		};
		using T = CompoundType_108;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section133")
	{
		struct CompoundType_109
		{
			DiskPos<bool> Member0;
			char Member1;
		};
		using T = CompoundType_109;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section134")
	{
		struct CompoundType_110
		{
			DiskPos<float> Member0;
			char Member1;
		};
		using T = CompoundType_110;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section135")
	{
		struct CompoundType_111
		{
			DiskPos<double> Member0;
			char Member1;
		};
		using T = CompoundType_111;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section136")
	{
		struct CompoundType_112
		{
			DiskPos<char> Member0;
			char Member1;
		};
		using T = CompoundType_112;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section137")
	{
		struct CompoundType_113
		{
			DiskPos<string> Member0;
			char Member1;
		};
		using T = CompoundType_113;
		T t =
			{
				{nullptr, 0},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section138")
	{
		struct CompoundType_114
		{
			LiteVector<int, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_114;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section139")
	{
		struct CompoundType_115
		{
			LiteVector<bool, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_115;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section140")
	{
		struct CompoundType_116
		{
			LiteVector<float, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_116;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section141")
	{
		struct CompoundType_117
		{
			LiteVector<double, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_117;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section142")
	{
		struct CompoundType_118
		{
			LiteVector<char, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_118;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section143")
	{
		struct CompoundType_119
		{
			LiteVector<string, size_t, 10> Member0;
			char Member1;
		};
		using T = CompoundType_119;
		T t =
			{
				{

				},
				'a',
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section144")
	{
		struct CompoundType_120
		{
			int Member0;
			string Member1;
		};
		using T = CompoundType_120;
		T t =
			{
				0,
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section145")
	{
		struct CompoundType_121
		{
			bool Member0;
			string Member1;
		};
		using T = CompoundType_121;
		T t =
			{
				true,
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section146")
	{
		struct CompoundType_122
		{
			float Member0;
			string Member1;
		};
		using T = CompoundType_122;
		T t =
			{
				1.23F,
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section147")
	{
		struct CompoundType_123
		{
			double Member0;
			string Member1;
		};
		using T = CompoundType_123;
		T t =
			{
				2.34,
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section148")
	{
		struct CompoundType_124
		{
			char Member0;
			string Member1;
		};
		using T = CompoundType_124;
		T t =
			{
				'a',
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section149")
	{
		struct CompoundType_125
		{
			string Member0;
			string Member1;
		};
		using T = CompoundType_125;
		T t =
			{
				"Hello World",
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section150")
	{
		struct CompoundType_126
		{
			OwnerLessDiskPtr<int> Member0;
			string Member1;
		};
		using T = CompoundType_126;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section151")
	{
		struct CompoundType_127
		{
			OwnerLessDiskPtr<bool> Member0;
			string Member1;
		};
		using T = CompoundType_127;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section152")
	{
		struct CompoundType_128
		{
			OwnerLessDiskPtr<float> Member0;
			string Member1;
		};
		using T = CompoundType_128;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section153")
	{
		struct CompoundType_129
		{
			OwnerLessDiskPtr<double> Member0;
			string Member1;
		};
		using T = CompoundType_129;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section154")
	{
		struct CompoundType_130
		{
			OwnerLessDiskPtr<char> Member0;
			string Member1;
		};
		using T = CompoundType_130;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section155")
	{
		struct CompoundType_131
		{
			OwnerLessDiskPtr<string> Member0;
			string Member1;
		};
		using T = CompoundType_131;
		T t =
			{
				{{nullptr, 0}, nullptr},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section156")
	{
		struct CompoundType_132
		{
			DiskPos<int> Member0;
			string Member1;
		};
		using T = CompoundType_132;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section157")
	{
		struct CompoundType_133
		{
			DiskPos<bool> Member0;
			string Member1;
		};
		using T = CompoundType_133;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section158")
	{
		struct CompoundType_134
		{
			DiskPos<float> Member0;
			string Member1;
		};
		using T = CompoundType_134;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section159")
	{
		struct CompoundType_135
		{
			DiskPos<double> Member0;
			string Member1;
		};
		using T = CompoundType_135;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section160")
	{
		struct CompoundType_136
		{
			DiskPos<char> Member0;
			string Member1;
		};
		using T = CompoundType_136;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section161")
	{
		struct CompoundType_137
		{
			DiskPos<string> Member0;
			string Member1;
		};
		using T = CompoundType_137;
		T t =
			{
				{nullptr, 0},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section162")
	{
		struct CompoundType_138
		{
			LiteVector<int, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_138;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section163")
	{
		struct CompoundType_139
		{
			LiteVector<bool, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_139;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section164")
	{
		struct CompoundType_140
		{
			LiteVector<float, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_140;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section165")
	{
		struct CompoundType_141
		{
			LiteVector<double, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_141;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section166")
	{
		struct CompoundType_142
		{
			LiteVector<char, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_142;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section167")
	{
		struct CompoundType_143
		{
			LiteVector<string, size_t, 10> Member0;
			string Member1;
		};
		using T = CompoundType_143;
		T t =
			{
				{

				},
				"Hello World",
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section168")
	{
		struct CompoundType_144
		{
			int Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_144;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section169")
	{
		struct CompoundType_145
		{
			bool Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_145;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section170")
	{
		struct CompoundType_146
		{
			float Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_146;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section171")
	{
		struct CompoundType_147
		{
			double Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_147;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section172")
	{
		struct CompoundType_148
		{
			char Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_148;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section173")
	{
		struct CompoundType_149
		{
			string Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_149;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section174")
	{
		struct CompoundType_150
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_150;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section175")
	{
		struct CompoundType_151
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_151;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section176")
	{
		struct CompoundType_152
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_152;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section177")
	{
		struct CompoundType_153
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_153;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section178")
	{
		struct CompoundType_154
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_154;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section179")
	{
		struct CompoundType_155
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_155;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section180")
	{
		struct CompoundType_156
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_156;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section181")
	{
		struct CompoundType_157
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_157;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section182")
	{
		struct CompoundType_158
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_158;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section183")
	{
		struct CompoundType_159
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_159;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section184")
	{
		struct CompoundType_160
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_160;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section185")
	{
		struct CompoundType_161
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_161;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section186")
	{
		struct CompoundType_162
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_162;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section187")
	{
		struct CompoundType_163
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_163;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section188")
	{
		struct CompoundType_164
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_164;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section189")
	{
		struct CompoundType_165
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_165;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section190")
	{
		struct CompoundType_166
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_166;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section191")
	{
		struct CompoundType_167
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<int> Member1;
		};
		using T = CompoundType_167;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section192")
	{
		struct CompoundType_168
		{
			int Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_168;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section193")
	{
		struct CompoundType_169
		{
			bool Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_169;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section194")
	{
		struct CompoundType_170
		{
			float Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_170;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section195")
	{
		struct CompoundType_171
		{
			double Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_171;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section196")
	{
		struct CompoundType_172
		{
			char Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_172;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section197")
	{
		struct CompoundType_173
		{
			string Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_173;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section198")
	{
		struct CompoundType_174
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_174;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section199")
	{
		struct CompoundType_175
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_175;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section200")
	{
		struct CompoundType_176
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_176;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section201")
	{
		struct CompoundType_177
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_177;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section202")
	{
		struct CompoundType_178
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_178;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section203")
	{
		struct CompoundType_179
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_179;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section204")
	{
		struct CompoundType_180
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_180;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section205")
	{
		struct CompoundType_181
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_181;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section206")
	{
		struct CompoundType_182
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_182;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section207")
	{
		struct CompoundType_183
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_183;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section208")
	{
		struct CompoundType_184
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_184;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section209")
	{
		struct CompoundType_185
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_185;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section210")
	{
		struct CompoundType_186
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_186;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section211")
	{
		struct CompoundType_187
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_187;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section212")
	{
		struct CompoundType_188
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_188;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section213")
	{
		struct CompoundType_189
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_189;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section214")
	{
		struct CompoundType_190
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_190;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section215")
	{
		struct CompoundType_191
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<bool> Member1;
		};
		using T = CompoundType_191;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section216")
	{
		struct CompoundType_192
		{
			int Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_192;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section217")
	{
		struct CompoundType_193
		{
			bool Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_193;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section218")
	{
		struct CompoundType_194
		{
			float Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_194;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section219")
	{
		struct CompoundType_195
		{
			double Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_195;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section220")
	{
		struct CompoundType_196
		{
			char Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_196;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section221")
	{
		struct CompoundType_197
		{
			string Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_197;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section222")
	{
		struct CompoundType_198
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_198;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section223")
	{
		struct CompoundType_199
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_199;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section224")
	{
		struct CompoundType_200
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_200;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section225")
	{
		struct CompoundType_201
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_201;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section226")
	{
		struct CompoundType_202
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_202;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section227")
	{
		struct CompoundType_203
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_203;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section228")
	{
		struct CompoundType_204
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_204;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section229")
	{
		struct CompoundType_205
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_205;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section230")
	{
		struct CompoundType_206
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_206;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section231")
	{
		struct CompoundType_207
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_207;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section232")
	{
		struct CompoundType_208
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_208;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section233")
	{
		struct CompoundType_209
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_209;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section234")
	{
		struct CompoundType_210
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_210;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section235")
	{
		struct CompoundType_211
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_211;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section236")
	{
		struct CompoundType_212
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_212;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section237")
	{
		struct CompoundType_213
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_213;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section238")
	{
		struct CompoundType_214
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_214;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section239")
	{
		struct CompoundType_215
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<float> Member1;
		};
		using T = CompoundType_215;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section240")
	{
		struct CompoundType_216
		{
			int Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_216;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section241")
	{
		struct CompoundType_217
		{
			bool Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_217;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section242")
	{
		struct CompoundType_218
		{
			float Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_218;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section243")
	{
		struct CompoundType_219
		{
			double Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_219;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section244")
	{
		struct CompoundType_220
		{
			char Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_220;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section245")
	{
		struct CompoundType_221
		{
			string Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_221;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section246")
	{
		struct CompoundType_222
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_222;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section247")
	{
		struct CompoundType_223
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_223;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section248")
	{
		struct CompoundType_224
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_224;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section249")
	{
		struct CompoundType_225
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_225;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section250")
	{
		struct CompoundType_226
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_226;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section251")
	{
		struct CompoundType_227
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_227;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section252")
	{
		struct CompoundType_228
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_228;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section253")
	{
		struct CompoundType_229
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_229;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section254")
	{
		struct CompoundType_230
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_230;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section255")
	{
		struct CompoundType_231
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_231;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section256")
	{
		struct CompoundType_232
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_232;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section257")
	{
		struct CompoundType_233
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_233;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section258")
	{
		struct CompoundType_234
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_234;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section259")
	{
		struct CompoundType_235
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_235;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section260")
	{
		struct CompoundType_236
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_236;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section261")
	{
		struct CompoundType_237
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_237;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section262")
	{
		struct CompoundType_238
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_238;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section263")
	{
		struct CompoundType_239
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<double> Member1;
		};
		using T = CompoundType_239;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section264")
	{
		struct CompoundType_240
		{
			int Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_240;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section265")
	{
		struct CompoundType_241
		{
			bool Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_241;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section266")
	{
		struct CompoundType_242
		{
			float Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_242;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section267")
	{
		struct CompoundType_243
		{
			double Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_243;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section268")
	{
		struct CompoundType_244
		{
			char Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_244;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section269")
	{
		struct CompoundType_245
		{
			string Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_245;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section270")
	{
		struct CompoundType_246
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_246;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section271")
	{
		struct CompoundType_247
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_247;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section272")
	{
		struct CompoundType_248
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_248;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section273")
	{
		struct CompoundType_249
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_249;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section274")
	{
		struct CompoundType_250
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_250;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section275")
	{
		struct CompoundType_251
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_251;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section276")
	{
		struct CompoundType_252
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_252;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section277")
	{
		struct CompoundType_253
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_253;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section278")
	{
		struct CompoundType_254
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_254;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section279")
	{
		struct CompoundType_255
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_255;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section280")
	{
		struct CompoundType_256
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_256;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section281")
	{
		struct CompoundType_257
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_257;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section282")
	{
		struct CompoundType_258
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_258;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section283")
	{
		struct CompoundType_259
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_259;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section284")
	{
		struct CompoundType_260
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_260;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section285")
	{
		struct CompoundType_261
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_261;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section286")
	{
		struct CompoundType_262
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_262;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section287")
	{
		struct CompoundType_263
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<char> Member1;
		};
		using T = CompoundType_263;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section288")
	{
		struct CompoundType_264
		{
			int Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_264;
		T t =
			{
				0,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section289")
	{
		struct CompoundType_265
		{
			bool Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_265;
		T t =
			{
				true,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section290")
	{
		struct CompoundType_266
		{
			float Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_266;
		T t =
			{
				1.23F,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section291")
	{
		struct CompoundType_267
		{
			double Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_267;
		T t =
			{
				2.34,
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section292")
	{
		struct CompoundType_268
		{
			char Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_268;
		T t =
			{
				'a',
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section293")
	{
		struct CompoundType_269
		{
			string Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_269;
		T t =
			{
				"Hello World",
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section294")
	{
		struct CompoundType_270
		{
			OwnerLessDiskPtr<int> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_270;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section295")
	{
		struct CompoundType_271
		{
			OwnerLessDiskPtr<bool> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_271;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section296")
	{
		struct CompoundType_272
		{
			OwnerLessDiskPtr<float> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_272;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section297")
	{
		struct CompoundType_273
		{
			OwnerLessDiskPtr<double> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_273;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section298")
	{
		struct CompoundType_274
		{
			OwnerLessDiskPtr<char> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_274;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section299")
	{
		struct CompoundType_275
		{
			OwnerLessDiskPtr<string> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_275;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section300")
	{
		struct CompoundType_276
		{
			DiskPos<int> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_276;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section301")
	{
		struct CompoundType_277
		{
			DiskPos<bool> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_277;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section302")
	{
		struct CompoundType_278
		{
			DiskPos<float> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_278;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section303")
	{
		struct CompoundType_279
		{
			DiskPos<double> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_279;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section304")
	{
		struct CompoundType_280
		{
			DiskPos<char> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_280;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section305")
	{
		struct CompoundType_281
		{
			DiskPos<string> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_281;
		T t =
			{
				{nullptr, 0},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section306")
	{
		struct CompoundType_282
		{
			LiteVector<int, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_282;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section307")
	{
		struct CompoundType_283
		{
			LiteVector<bool, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_283;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section308")
	{
		struct CompoundType_284
		{
			LiteVector<float, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_284;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section309")
	{
		struct CompoundType_285
		{
			LiteVector<double, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_285;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section310")
	{
		struct CompoundType_286
		{
			LiteVector<char, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_286;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section311")
	{
		struct CompoundType_287
		{
			LiteVector<string, size_t, 10> Member0;
			OwnerLessDiskPtr<string> Member1;
		};
		using T = CompoundType_287;
		T t =
			{
				{

				},
				{{nullptr, 0}, nullptr},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section312")
	{
		struct CompoundType_288
		{
			int Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_288;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section313")
	{
		struct CompoundType_289
		{
			bool Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_289;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section314")
	{
		struct CompoundType_290
		{
			float Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_290;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section315")
	{
		struct CompoundType_291
		{
			double Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_291;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section316")
	{
		struct CompoundType_292
		{
			char Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_292;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section317")
	{
		struct CompoundType_293
		{
			string Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_293;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section318")
	{
		struct CompoundType_294
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_294;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section319")
	{
		struct CompoundType_295
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_295;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section320")
	{
		struct CompoundType_296
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_296;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section321")
	{
		struct CompoundType_297
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_297;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section322")
	{
		struct CompoundType_298
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_298;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section323")
	{
		struct CompoundType_299
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_299;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section324")
	{
		struct CompoundType_300
		{
			DiskPos<int> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_300;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section325")
	{
		struct CompoundType_301
		{
			DiskPos<bool> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_301;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section326")
	{
		struct CompoundType_302
		{
			DiskPos<float> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_302;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section327")
	{
		struct CompoundType_303
		{
			DiskPos<double> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_303;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section328")
	{
		struct CompoundType_304
		{
			DiskPos<char> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_304;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section329")
	{
		struct CompoundType_305
		{
			DiskPos<string> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_305;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section330")
	{
		struct CompoundType_306
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_306;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section331")
	{
		struct CompoundType_307
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_307;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section332")
	{
		struct CompoundType_308
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_308;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section333")
	{
		struct CompoundType_309
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_309;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section334")
	{
		struct CompoundType_310
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_310;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section335")
	{
		struct CompoundType_311
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<int> Member1;
		};
		using T = CompoundType_311;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section336")
	{
		struct CompoundType_312
		{
			int Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_312;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section337")
	{
		struct CompoundType_313
		{
			bool Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_313;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section338")
	{
		struct CompoundType_314
		{
			float Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_314;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section339")
	{
		struct CompoundType_315
		{
			double Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_315;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section340")
	{
		struct CompoundType_316
		{
			char Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_316;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section341")
	{
		struct CompoundType_317
		{
			string Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_317;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section342")
	{
		struct CompoundType_318
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_318;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section343")
	{
		struct CompoundType_319
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_319;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section344")
	{
		struct CompoundType_320
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_320;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section345")
	{
		struct CompoundType_321
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_321;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section346")
	{
		struct CompoundType_322
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_322;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section347")
	{
		struct CompoundType_323
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_323;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section348")
	{
		struct CompoundType_324
		{
			DiskPos<int> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_324;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section349")
	{
		struct CompoundType_325
		{
			DiskPos<bool> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_325;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section350")
	{
		struct CompoundType_326
		{
			DiskPos<float> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_326;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section351")
	{
		struct CompoundType_327
		{
			DiskPos<double> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_327;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section352")
	{
		struct CompoundType_328
		{
			DiskPos<char> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_328;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section353")
	{
		struct CompoundType_329
		{
			DiskPos<string> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_329;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section354")
	{
		struct CompoundType_330
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_330;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section355")
	{
		struct CompoundType_331
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_331;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section356")
	{
		struct CompoundType_332
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_332;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section357")
	{
		struct CompoundType_333
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_333;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section358")
	{
		struct CompoundType_334
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_334;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section359")
	{
		struct CompoundType_335
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<bool> Member1;
		};
		using T = CompoundType_335;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section360")
	{
		struct CompoundType_336
		{
			int Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_336;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section361")
	{
		struct CompoundType_337
		{
			bool Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_337;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section362")
	{
		struct CompoundType_338
		{
			float Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_338;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section363")
	{
		struct CompoundType_339
		{
			double Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_339;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section364")
	{
		struct CompoundType_340
		{
			char Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_340;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section365")
	{
		struct CompoundType_341
		{
			string Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_341;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section366")
	{
		struct CompoundType_342
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_342;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section367")
	{
		struct CompoundType_343
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_343;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section368")
	{
		struct CompoundType_344
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_344;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section369")
	{
		struct CompoundType_345
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_345;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section370")
	{
		struct CompoundType_346
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_346;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section371")
	{
		struct CompoundType_347
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_347;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section372")
	{
		struct CompoundType_348
		{
			DiskPos<int> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_348;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section373")
	{
		struct CompoundType_349
		{
			DiskPos<bool> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_349;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section374")
	{
		struct CompoundType_350
		{
			DiskPos<float> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_350;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section375")
	{
		struct CompoundType_351
		{
			DiskPos<double> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_351;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section376")
	{
		struct CompoundType_352
		{
			DiskPos<char> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_352;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section377")
	{
		struct CompoundType_353
		{
			DiskPos<string> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_353;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section378")
	{
		struct CompoundType_354
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_354;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section379")
	{
		struct CompoundType_355
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_355;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section380")
	{
		struct CompoundType_356
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_356;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section381")
	{
		struct CompoundType_357
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_357;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section382")
	{
		struct CompoundType_358
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_358;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section383")
	{
		struct CompoundType_359
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<float> Member1;
		};
		using T = CompoundType_359;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section384")
	{
		struct CompoundType_360
		{
			int Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_360;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section385")
	{
		struct CompoundType_361
		{
			bool Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_361;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section386")
	{
		struct CompoundType_362
		{
			float Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_362;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section387")
	{
		struct CompoundType_363
		{
			double Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_363;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section388")
	{
		struct CompoundType_364
		{
			char Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_364;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section389")
	{
		struct CompoundType_365
		{
			string Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_365;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section390")
	{
		struct CompoundType_366
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_366;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section391")
	{
		struct CompoundType_367
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_367;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section392")
	{
		struct CompoundType_368
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_368;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section393")
	{
		struct CompoundType_369
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_369;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section394")
	{
		struct CompoundType_370
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_370;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section395")
	{
		struct CompoundType_371
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_371;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section396")
	{
		struct CompoundType_372
		{
			DiskPos<int> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_372;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section397")
	{
		struct CompoundType_373
		{
			DiskPos<bool> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_373;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section398")
	{
		struct CompoundType_374
		{
			DiskPos<float> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_374;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section399")
	{
		struct CompoundType_375
		{
			DiskPos<double> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_375;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section400")
	{
		struct CompoundType_376
		{
			DiskPos<char> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_376;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section401")
	{
		struct CompoundType_377
		{
			DiskPos<string> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_377;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section402")
	{
		struct CompoundType_378
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_378;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section403")
	{
		struct CompoundType_379
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_379;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section404")
	{
		struct CompoundType_380
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_380;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section405")
	{
		struct CompoundType_381
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_381;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section406")
	{
		struct CompoundType_382
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_382;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section407")
	{
		struct CompoundType_383
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<double> Member1;
		};
		using T = CompoundType_383;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section408")
	{
		struct CompoundType_384
		{
			int Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_384;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section409")
	{
		struct CompoundType_385
		{
			bool Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_385;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section410")
	{
		struct CompoundType_386
		{
			float Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_386;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section411")
	{
		struct CompoundType_387
		{
			double Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_387;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section412")
	{
		struct CompoundType_388
		{
			char Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_388;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section413")
	{
		struct CompoundType_389
		{
			string Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_389;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section414")
	{
		struct CompoundType_390
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_390;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section415")
	{
		struct CompoundType_391
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_391;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section416")
	{
		struct CompoundType_392
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_392;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section417")
	{
		struct CompoundType_393
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_393;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section418")
	{
		struct CompoundType_394
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_394;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section419")
	{
		struct CompoundType_395
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_395;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section420")
	{
		struct CompoundType_396
		{
			DiskPos<int> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_396;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section421")
	{
		struct CompoundType_397
		{
			DiskPos<bool> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_397;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section422")
	{
		struct CompoundType_398
		{
			DiskPos<float> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_398;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section423")
	{
		struct CompoundType_399
		{
			DiskPos<double> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_399;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section424")
	{
		struct CompoundType_400
		{
			DiskPos<char> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_400;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section425")
	{
		struct CompoundType_401
		{
			DiskPos<string> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_401;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section426")
	{
		struct CompoundType_402
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_402;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section427")
	{
		struct CompoundType_403
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_403;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section428")
	{
		struct CompoundType_404
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_404;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section429")
	{
		struct CompoundType_405
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_405;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section430")
	{
		struct CompoundType_406
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_406;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section431")
	{
		struct CompoundType_407
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<char> Member1;
		};
		using T = CompoundType_407;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section432")
	{
		struct CompoundType_408
		{
			int Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_408;
		T t =
			{
				0,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section433")
	{
		struct CompoundType_409
		{
			bool Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_409;
		T t =
			{
				true,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section434")
	{
		struct CompoundType_410
		{
			float Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_410;
		T t =
			{
				1.23F,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section435")
	{
		struct CompoundType_411
		{
			double Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_411;
		T t =
			{
				2.34,
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section436")
	{
		struct CompoundType_412
		{
			char Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_412;
		T t =
			{
				'a',
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section437")
	{
		struct CompoundType_413
		{
			string Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_413;
		T t =
			{
				"Hello World",
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section438")
	{
		struct CompoundType_414
		{
			OwnerLessDiskPtr<int> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_414;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section439")
	{
		struct CompoundType_415
		{
			OwnerLessDiskPtr<bool> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_415;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section440")
	{
		struct CompoundType_416
		{
			OwnerLessDiskPtr<float> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_416;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section441")
	{
		struct CompoundType_417
		{
			OwnerLessDiskPtr<double> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_417;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section442")
	{
		struct CompoundType_418
		{
			OwnerLessDiskPtr<char> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_418;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section443")
	{
		struct CompoundType_419
		{
			OwnerLessDiskPtr<string> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_419;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section444")
	{
		struct CompoundType_420
		{
			DiskPos<int> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_420;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section445")
	{
		struct CompoundType_421
		{
			DiskPos<bool> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_421;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section446")
	{
		struct CompoundType_422
		{
			DiskPos<float> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_422;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section447")
	{
		struct CompoundType_423
		{
			DiskPos<double> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_423;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section448")
	{
		struct CompoundType_424
		{
			DiskPos<char> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_424;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section449")
	{
		struct CompoundType_425
		{
			DiskPos<string> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_425;
		T t =
			{
				{nullptr, 0},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section450")
	{
		struct CompoundType_426
		{
			LiteVector<int, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_426;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section451")
	{
		struct CompoundType_427
		{
			LiteVector<bool, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_427;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section452")
	{
		struct CompoundType_428
		{
			LiteVector<float, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_428;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section453")
	{
		struct CompoundType_429
		{
			LiteVector<double, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_429;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section454")
	{
		struct CompoundType_430
		{
			LiteVector<char, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_430;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section455")
	{
		struct CompoundType_431
		{
			LiteVector<string, size_t, 10> Member0;
			DiskPos<string> Member1;
		};
		using T = CompoundType_431;
		T t =
			{
				{

				},
				{nullptr, 0},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section456")
	{
		struct CompoundType_432
		{
			int Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_432;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section457")
	{
		struct CompoundType_433
		{
			bool Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_433;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section458")
	{
		struct CompoundType_434
		{
			float Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_434;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section459")
	{
		struct CompoundType_435
		{
			double Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_435;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section460")
	{
		struct CompoundType_436
		{
			char Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_436;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section461")
	{
		struct CompoundType_437
		{
			string Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_437;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section462")
	{
		struct CompoundType_438
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_438;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section463")
	{
		struct CompoundType_439
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_439;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section464")
	{
		struct CompoundType_440
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_440;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section465")
	{
		struct CompoundType_441
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_441;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section466")
	{
		struct CompoundType_442
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_442;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section467")
	{
		struct CompoundType_443
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_443;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section468")
	{
		struct CompoundType_444
		{
			DiskPos<int> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_444;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section469")
	{
		struct CompoundType_445
		{
			DiskPos<bool> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_445;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section470")
	{
		struct CompoundType_446
		{
			DiskPos<float> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_446;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section471")
	{
		struct CompoundType_447
		{
			DiskPos<double> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_447;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section472")
	{
		struct CompoundType_448
		{
			DiskPos<char> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_448;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section473")
	{
		struct CompoundType_449
		{
			DiskPos<string> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_449;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section474")
	{
		struct CompoundType_450
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_450;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section475")
	{
		struct CompoundType_451
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_451;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section476")
	{
		struct CompoundType_452
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_452;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section477")
	{
		struct CompoundType_453
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_453;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section478")
	{
		struct CompoundType_454
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_454;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section479")
	{
		struct CompoundType_455
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<int, size_t, 10> Member1;
		};
		using T = CompoundType_455;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section480")
	{
		struct CompoundType_456
		{
			int Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_456;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section481")
	{
		struct CompoundType_457
		{
			bool Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_457;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section482")
	{
		struct CompoundType_458
		{
			float Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_458;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section483")
	{
		struct CompoundType_459
		{
			double Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_459;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section484")
	{
		struct CompoundType_460
		{
			char Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_460;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section485")
	{
		struct CompoundType_461
		{
			string Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_461;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section486")
	{
		struct CompoundType_462
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_462;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section487")
	{
		struct CompoundType_463
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_463;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section488")
	{
		struct CompoundType_464
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_464;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section489")
	{
		struct CompoundType_465
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_465;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section490")
	{
		struct CompoundType_466
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_466;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section491")
	{
		struct CompoundType_467
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_467;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section492")
	{
		struct CompoundType_468
		{
			DiskPos<int> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_468;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section493")
	{
		struct CompoundType_469
		{
			DiskPos<bool> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_469;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section494")
	{
		struct CompoundType_470
		{
			DiskPos<float> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_470;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section495")
	{
		struct CompoundType_471
		{
			DiskPos<double> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_471;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section496")
	{
		struct CompoundType_472
		{
			DiskPos<char> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_472;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section497")
	{
		struct CompoundType_473
		{
			DiskPos<string> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_473;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section498")
	{
		struct CompoundType_474
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_474;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section499")
	{
		struct CompoundType_475
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_475;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section500")
	{
		struct CompoundType_476
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_476;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section501")
	{
		struct CompoundType_477
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_477;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section502")
	{
		struct CompoundType_478
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_478;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section503")
	{
		struct CompoundType_479
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<bool, size_t, 10> Member1;
		};
		using T = CompoundType_479;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section504")
	{
		struct CompoundType_480
		{
			int Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_480;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section505")
	{
		struct CompoundType_481
		{
			bool Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_481;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section506")
	{
		struct CompoundType_482
		{
			float Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_482;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section507")
	{
		struct CompoundType_483
		{
			double Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_483;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section508")
	{
		struct CompoundType_484
		{
			char Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_484;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section509")
	{
		struct CompoundType_485
		{
			string Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_485;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section510")
	{
		struct CompoundType_486
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_486;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section511")
	{
		struct CompoundType_487
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_487;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section512")
	{
		struct CompoundType_488
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_488;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section513")
	{
		struct CompoundType_489
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_489;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section514")
	{
		struct CompoundType_490
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_490;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section515")
	{
		struct CompoundType_491
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_491;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section516")
	{
		struct CompoundType_492
		{
			DiskPos<int> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_492;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section517")
	{
		struct CompoundType_493
		{
			DiskPos<bool> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_493;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section518")
	{
		struct CompoundType_494
		{
			DiskPos<float> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_494;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section519")
	{
		struct CompoundType_495
		{
			DiskPos<double> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_495;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section520")
	{
		struct CompoundType_496
		{
			DiskPos<char> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_496;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section521")
	{
		struct CompoundType_497
		{
			DiskPos<string> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_497;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section522")
	{
		struct CompoundType_498
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_498;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section523")
	{
		struct CompoundType_499
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_499;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section524")
	{
		struct CompoundType_500
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_500;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section525")
	{
		struct CompoundType_501
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_501;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section526")
	{
		struct CompoundType_502
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_502;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section527")
	{
		struct CompoundType_503
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<float, size_t, 10> Member1;
		};
		using T = CompoundType_503;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section528")
	{
		struct CompoundType_504
		{
			int Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_504;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section529")
	{
		struct CompoundType_505
		{
			bool Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_505;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section530")
	{
		struct CompoundType_506
		{
			float Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_506;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section531")
	{
		struct CompoundType_507
		{
			double Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_507;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section532")
	{
		struct CompoundType_508
		{
			char Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_508;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section533")
	{
		struct CompoundType_509
		{
			string Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_509;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section534")
	{
		struct CompoundType_510
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_510;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section535")
	{
		struct CompoundType_511
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_511;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section536")
	{
		struct CompoundType_512
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_512;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section537")
	{
		struct CompoundType_513
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_513;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section538")
	{
		struct CompoundType_514
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_514;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section539")
	{
		struct CompoundType_515
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_515;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section540")
	{
		struct CompoundType_516
		{
			DiskPos<int> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_516;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section541")
	{
		struct CompoundType_517
		{
			DiskPos<bool> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_517;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section542")
	{
		struct CompoundType_518
		{
			DiskPos<float> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_518;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section543")
	{
		struct CompoundType_519
		{
			DiskPos<double> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_519;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section544")
	{
		struct CompoundType_520
		{
			DiskPos<char> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_520;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section545")
	{
		struct CompoundType_521
		{
			DiskPos<string> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_521;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section546")
	{
		struct CompoundType_522
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_522;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section547")
	{
		struct CompoundType_523
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_523;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section548")
	{
		struct CompoundType_524
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_524;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section549")
	{
		struct CompoundType_525
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_525;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section550")
	{
		struct CompoundType_526
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_526;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section551")
	{
		struct CompoundType_527
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<double, size_t, 10> Member1;
		};
		using T = CompoundType_527;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section552")
	{
		struct CompoundType_528
		{
			int Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_528;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section553")
	{
		struct CompoundType_529
		{
			bool Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_529;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section554")
	{
		struct CompoundType_530
		{
			float Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_530;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section555")
	{
		struct CompoundType_531
		{
			double Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_531;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section556")
	{
		struct CompoundType_532
		{
			char Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_532;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section557")
	{
		struct CompoundType_533
		{
			string Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_533;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section558")
	{
		struct CompoundType_534
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_534;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section559")
	{
		struct CompoundType_535
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_535;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section560")
	{
		struct CompoundType_536
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_536;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section561")
	{
		struct CompoundType_537
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_537;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section562")
	{
		struct CompoundType_538
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_538;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section563")
	{
		struct CompoundType_539
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_539;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section564")
	{
		struct CompoundType_540
		{
			DiskPos<int> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_540;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section565")
	{
		struct CompoundType_541
		{
			DiskPos<bool> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_541;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section566")
	{
		struct CompoundType_542
		{
			DiskPos<float> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_542;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section567")
	{
		struct CompoundType_543
		{
			DiskPos<double> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_543;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section568")
	{
		struct CompoundType_544
		{
			DiskPos<char> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_544;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section569")
	{
		struct CompoundType_545
		{
			DiskPos<string> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_545;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section570")
	{
		struct CompoundType_546
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_546;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section571")
	{
		struct CompoundType_547
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_547;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section572")
	{
		struct CompoundType_548
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_548;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section573")
	{
		struct CompoundType_549
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_549;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section574")
	{
		struct CompoundType_550
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_550;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section575")
	{
		struct CompoundType_551
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<char, size_t, 10> Member1;
		};
		using T = CompoundType_551;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section576")
	{
		struct CompoundType_552
		{
			int Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_552;
		T t =
			{
				0,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section577")
	{
		struct CompoundType_553
		{
			bool Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_553;
		T t =
			{
				true,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section578")
	{
		struct CompoundType_554
		{
			float Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_554;
		T t =
			{
				1.23F,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section579")
	{
		struct CompoundType_555
		{
			double Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_555;
		T t =
			{
				2.34,
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section580")
	{
		struct CompoundType_556
		{
			char Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_556;
		T t =
			{
				'a',
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section581")
	{
		struct CompoundType_557
		{
			string Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_557;
		T t =
			{
				"Hello World",
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section582")
	{
		struct CompoundType_558
		{
			OwnerLessDiskPtr<int> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_558;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section583")
	{
		struct CompoundType_559
		{
			OwnerLessDiskPtr<bool> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_559;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section584")
	{
		struct CompoundType_560
		{
			OwnerLessDiskPtr<float> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_560;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section585")
	{
		struct CompoundType_561
		{
			OwnerLessDiskPtr<double> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_561;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section586")
	{
		struct CompoundType_562
		{
			OwnerLessDiskPtr<char> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_562;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section587")
	{
		struct CompoundType_563
		{
			OwnerLessDiskPtr<string> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_563;
		T t =
			{
				{{nullptr, 0}, nullptr},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section588")
	{
		struct CompoundType_564
		{
			DiskPos<int> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_564;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section589")
	{
		struct CompoundType_565
		{
			DiskPos<bool> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_565;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section590")
	{
		struct CompoundType_566
		{
			DiskPos<float> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_566;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section591")
	{
		struct CompoundType_567
		{
			DiskPos<double> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_567;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section592")
	{
		struct CompoundType_568
		{
			DiskPos<char> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_568;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section593")
	{
		struct CompoundType_569
		{
			DiskPos<string> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_569;
		T t =
			{
				{nullptr, 0},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section594")
	{
		struct CompoundType_570
		{
			LiteVector<int, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_570;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section595")
	{
		struct CompoundType_571
		{
			LiteVector<bool, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_571;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section596")
	{
		struct CompoundType_572
		{
			LiteVector<float, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_572;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section597")
	{
		struct CompoundType_573
		{
			LiteVector<double, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_573;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section598")
	{
		struct CompoundType_574
		{
			LiteVector<char, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_574;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}

	SECTION("section599")
	{
		struct CompoundType_575
		{
			LiteVector<string, size_t, 10> Member0;
			LiteVector<string, size_t, 10> Member1;
		};
		using T = CompoundType_575;
		T t =
			{
				{

				},
				{

				},
			};
		ByteConverter<T>::WriteDown(t, &writer);
		ByteConverter<T>::ReadOut(&reader);
	}
}

DEF_TEST_FUNC(byteConverterTest_gen)
