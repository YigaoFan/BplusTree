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

        public IEnumerable<string> Definition()
        {
            yield return $"struct {Name}";
            yield return "{";

            var i = 0;
            foreach (var t in memberTypes)
            {
                yield return $"{t.Name} Member{i++};";
            }

            yield return "}";
        }
    }
}
