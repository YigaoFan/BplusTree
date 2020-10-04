using System;
using System.Collections.Generic;
using System.Text;

namespace TestCodeGen
{
    public class CodeSnippet_ByteConverter
    {
        private IType type;
        private static List<string> codes = new List<string>
        {
            "T t;",
            "ByteConverter<T>::WriteDown(t, &writer);",
            "static_assert(ByteConverter<T>::SizeStable);",
            "ASSERT(t == ByteConverter<T>::ReadOut(&reader));",
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
            }

            builder.AppendLine(indent + "using T = " + type.Name + ';');

            foreach (var code in codes)
            {
                builder.AppendLine(indent + code);
            }

            return builder.ToString();
        }
    }
}
