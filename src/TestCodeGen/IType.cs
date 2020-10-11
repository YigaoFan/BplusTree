using System.Collections.Generic;

namespace TestCodeGen
{
    public interface IType
    {
        string Name { get; }
        IEnumerable<string> InitCode { get; }
    }
}
