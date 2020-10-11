using System.Collections.Generic;

namespace TestCodeGen
{
    public class CompoundType : IType
    {
        private List<IType> memberTypes;
        public string Name { get; }

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

        public IEnumerable<string> InitCode
        {
            get
            {
                yield return "{";

                foreach (var t in memberTypes)
                {
                    foreach (var code in t.InitCode)
                    {
                        yield return code;
                    }

                    yield return ",";
                }

                yield return "}";
            }
        }
    }
}
