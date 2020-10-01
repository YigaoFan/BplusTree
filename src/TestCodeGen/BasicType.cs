using System;
namespace TestCodeGen
{
    public class BasicType : IType
    {
        public string Name { get; }

        public string Init { get; }

        public BasicType(string name, string init)
        {
            Name = name;
            Init = init;
        }
    }
}
