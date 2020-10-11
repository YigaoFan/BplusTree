namespace TestCodeGen
{
    public class Template
    {
        public string Name { get; }
        private string initPattern;
        //private Type[] argsInfo;

        public Template(string name, string initPattern)
        {
            this.Name = name;
            this.initPattern = initPattern;
        }

        public TemplateSpecificationType GenerateTypeWith(IType[] types)
        {
            //CheckType(types);
            return new TemplateSpecificationType(this, types);
        }

        public string ComposeInitCode(IType[] types)
        {
            return initPattern;
        }

        //private void CheckType(IType[] types)
        //{
        //    Debug.Assert(types.Length == argsInfo.Length);

        //    for (var i = 0; i < types.Length; ++i)
        //    {
        //        var item = types[i];
        //        var info = argsInfo[i];
        //        Debug.Assert(item.GetType() == info);// 这里的类型比较可能会有问题，因为有的是 IType，有的是 NonType，所以这里的比较要支持继承
        //    }
        //}
    }
}
