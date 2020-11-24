#pragma once
#include <string>
#include <vector>

namespace FuncLib::Compile
{
	using ::std::move;
	using ::std::string;
	using ::std::vector;

	// 包含包名、函数名、返回值类型、参数类型（参数类型和名字，这个信息可以放到 summary 里面去）
	class FuncType
	{
	private:
		vector<string> _packageHierarchy;
		string _returnType;
		string _funcName;
		vector<string> _argTypes;
	public:
		FuncType(string returnType, string functionName, vector<string> argTypes)
			: _returnType(move(returnType)), _funcName(move(functionName)), _argTypes(move(argTypes))
		{ }

		void PackageHierarchy(vector<string> packageHierarchy)
		{
			_packageHierarchy = move(packageHierarchy);
		}

		void FuncName(string funcName)
		{
			_funcName = move(funcName);
		}

		// 可以像 TiKV 那样对 Key 对 package name 做一些优化存储 TODO
		string ToKey() const
		{
			string const DefalutPackage = "Global";
			string s;

			// 我希望排序的时候优先比较 pack，所以把包名放在了前面
			if (_packageHierarchy.empty())
			{
				s.append(DefalutPackage);
			}
			else
			{
				for (auto& p : _packageHierarchy)
				{
					s.append(p + ':');
				}

				s.pop_back();
			}
			s.append(" ");
			
			s.append(_returnType + ' ');
			s.append(_funcName + ' ');

			for (auto& a : _argTypes)
			{
				s.append(a + ',');
			}
			s.pop_back();

			return s;
		}
	};
}
