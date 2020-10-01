using System;
namespace TestCodeGen
{
    public class NonType<T> : IType
    {
        private T t;

        public string Name { get => t.ToString(); }
        public string Init { get => throw new InvalidOperationException("cannot init non-type"); }

        public NonType(T t)
        {
            this.t = t;
        }
    }
}
