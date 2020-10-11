using System;
using System.Collections.Generic;

namespace TestCodeGen
{
    public class BasicType : IType
    {
        public string Name { get; }

        public IEnumerable<string> InitCode { get; }

        public BasicType(string name, string init)
        {
            Name = name;
            InitCode = new List<string>
            {
                init,
            };
        }
    }
}
