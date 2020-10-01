using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace TestCodeGen
{
    //enum TypeKind
    //{
    //    NormalType,
    //    NonType,
    //}

    public class Template
    {
        private string name;
        //private Type[] argsInfo;

        public Template(string name)
        {
            this.name = name;
            //this.argsInfo = argsInfo;
        }

        public TemplateSpecificationType GenerateTypeWith(IType[] types)
        {
            //CheckType(types);
            return new TemplateSpecificationType(name, types);
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
