using System;
using System.Collections.Generic;
using System.Text;

namespace TestCodeGen
{
    public class CompoundType : IType
    {
        public string Name { get; }

        public string Init => throw new NotImplementedException();

        private List<IType> memberTypes;

        public CompoundType(string name, List<IType> types)
        {
            Name = name;
            memberTypes = types;
        }

        public string Definition()
        {
            var builder = new StringBuilder();
            builder.AppendLine($"struct {Name}");
            builder.AppendLine("{");

            var i = 0;
            foreach (var t in memberTypes)
            {
                builder.AppendLine($"{t.Name} Member{i++};");
            }

            builder.AppendLine("}");
            return builder.ToString();
        }
    }
}
