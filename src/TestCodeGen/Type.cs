using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace TestCodeGen
{
    //public class Type : IType
    //{
    //    public string Name { get; private set; }
    //    public string InitCode { get; private set; }

    //    public Type(string name, string initCode) : this(name, 0)
    //    { }

    //    public Type(string name, int templateArgCount)
    //    {
    //        this.Name = name;
    //        this.templateArgCount = templateArgCount;
    //        typeArgs = new List<Type>();
    //    }

    //    public bool IsTemplateType()
    //    {
    //        return templateArgCount != 0;
    //    }

    //    public Type GenConcreteTypeWith(params Type[] types)
    //    {
    //        Debug.Assert(IsTemplateType(), "cannot invoke this method on non tempalte type");

    //        var t = new Type(name, 0);
    //        foreach (var ty in types)
    //        {
    //            Debug.Assert(!ty.IsTemplateType(), "type should not be template type");
    //            t.typeArgs.Add(ty);
    //        }

    //        return t;
    //    }

    //    public override string ToString()
    //    {
    //        Debug.Assert(!IsTemplateType());
    //        var builder = new StringBuilder(name);

    //        if (typeArgs.Count == 0)
    //        {
    //            return builder.ToString();
    //        }

    //        builder.Append('<');
    //        foreach (var arg in typeArgs)
    //        {
    //            builder.Append(arg.ToString() + ',');
    //        }
    //        builder.Length -= 1;
    //        builder.Append('>');

    //        return builder.ToString();
    //    }

    //    //public string GenInitCode()
    //    //{
    //    //    // TODO
    //    //}
    //}
}
