using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace TestCodeGen
{
    public class CodeSnippet_ByteConverter
    {
        private IType type;
        private static List<string> codes = new List<string>
        {
            "ByteConverter<T>::WriteDown(t, &writer);",
            //"static_assert(ByteConverter<T>::SizeStable);",
            "ByteConverter<T>::ReadOut(&reader);",// 这里不对，有的类型没有支持 == 符号
            //"ASSERT(t == ByteConverter<T>::ReadOut(&reader));",// 这里不对，有的类型没有支持 == 符号
            // 这个更多作为一个编译测试吧？
        };

        public CodeSnippet_ByteConverter(IType type)
        {
            this.type = type;
        }

        public string CodeWithIndent(int indentCount, string indentUnit)
        {
            var indent = new StringBuilder(indentUnit);
            for (var i = 0; i < (indentCount - 1); ++i)
            {
                indent.Append(indentUnit);
            }

            var builder = new StringBuilder();
            if (type is CompoundType)
            {
                var innerIndent = indent;
                var ct = type as CompoundType;
                foreach (var line in ct.Definition())
                {
                    builder.AppendLine(innerIndent + line);
                }
                --builder.Length;// remove newline char
                builder.AppendLine(";");
            }

            builder.AppendLine(indent + "using T = " + type.Name + ';');
            builder.AppendLine(indent + "T t = ");
            foreach (var code in type.InitCode)
            {
                var innerIndent = indent;
                builder.AppendLine(innerIndent + code);
            }
            --builder.Length;// remove newline char
            builder.AppendLine(";");
            

            foreach (var code in codes)
            {
                builder.AppendLine(indent + code);
            }

            return builder.ToString();
        }
    }
}
