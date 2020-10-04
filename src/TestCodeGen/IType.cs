namespace TestCodeGen
{
    public interface IType
    {
        string Name { get; }
        string Init { get; }// 接下来要做这个，对于 CompoundType 有个树形结构，
                            // 对于模板类型，要调用模板定义的构造函数
    }
}
