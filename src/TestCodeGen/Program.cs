using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace TestCodeGen
{
    class MainClass
    {
        public static void Main(string[] args)
        {
            //var testFile =
            //    new TestFile("../../TestFrame/FlyTest.hpp", "byteConverterTest",
            //        new TestCase("Byte converter test",
            //            new Section("int"),
            //            new Section("float")));

            //Console.Write(testFile.ToString());

            var types = TypeFactory.GenBasicType().ConvertAll(basic => (IType)basic);
            var specialization = types.GenSpecializationType();
            specialization.ForEach(e => Console.WriteLine(e.Name));
            foreach (var t in types.GenCompoundType(2))
            {
                Console.WriteLine(t.Name);
            }
        }
    }
}
