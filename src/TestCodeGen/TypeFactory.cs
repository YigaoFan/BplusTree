using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace TestCodeGen
{
    public static class TypeFactory
    {
        public static List<BasicType> GenBasicType()
        {
            var names = new KeyValuePair<string, string>[]
            {
                new KeyValuePair<string, string>("int", "0"),
                new KeyValuePair<string, string>("bool", "true"),
                new KeyValuePair<string, string>("float", "1.23F"),
                new KeyValuePair<string, string>("double", "2.34"),
                new KeyValuePair<string, string>("char", "'a'"),
                new KeyValuePair<string, string>("string", "\"Hello World\""),
            };
            var types = new List<BasicType>();

            foreach (var pair in names)
            {
                types.Add(new BasicType(pair.Key, pair.Value));
            }

            return types;
        }

        public static List<TemplateSpecificationType> GenSpecializationType(this List<IType> typeArgs)
        {
            // 这里需要 env 的东西，在 template 去 eval pattern
            var numType = new NonType<int>(10);
            var types = new List<TemplateSpecificationType>();

            {
                var ptr = new Template("OwnerLessDiskPtr", "{ nullptr, 0 }, nullptr");
                foreach (var t in typeArgs)
                {
                    types.Add(ptr.GenerateTypeWith(new[] { t, }));
                }
            }

            {
                var pos = new Template("DiskPos", "nullptr, 0");
                foreach (var t in typeArgs)
                {
                    types.Add(pos.GenerateTypeWith(new[] { t, }));
                }
            }

            {
                var elements = new Template("LiteVector", null);
                Debug.Assert(typeArgs.Count > 1, "Elements need more than 2 type args");
                foreach (var t in typeArgs)
                {
                    var t1 = t;
                    var t2 = new BasicType("size_t", null);
                    types.Add(elements.GenerateTypeWith(new[] { t1, t2, numType, }));
                }
            }

            // 下面这几种之后肯定会测，主要的部分也涉及上面这些，所以这次不测应该是没问题（再想想）
            //{
            //    var middleNode = new Template("MiddleNode", null);
            //    Debug.Assert(typeArgs.Count > 1, "Elements need more than 2 type args");
            //    for (var i = 0; i < typeArgs.Count - 1; ++i)
            //    {
            //        var t1 = typeArgs[i];
            //        var t2 = typeArgs[i + 1];
            //        types.Add(middleNode.GenerateTypeWith(new[] { t1, t2, numType }));
            //    }
            //}

            //{
            //    var leafNode = new Template("LeafNode", null);
            //    Debug.Assert(typeArgs.Count > 1, "Elements need more than 2 type args");
            //    for (var i = 0; i < typeArgs.Count - 1; ++i)
            //    {
            //        var t1 = typeArgs[i];
            //        var t2 = typeArgs[i + 1];
            //        types.Add(leafNode.GenerateTypeWith(new[] { t1, t2, numType, }));
            //    }
            //}

            //{
            //    var btree = new Template("Btree", null);
            //    Debug.Assert(typeArgs.Count > 1, "Elements need more than 2 type args");
            //    for (var i = 0; i < typeArgs.Count - 1; ++i)
            //    {
            //        var t1 = typeArgs[i];
            //        var t2 = typeArgs[i + 1];
            //        types.Add(btree.GenerateTypeWith(new[] { numType, t1, t2, }));
            //    }
            //}

            return types;
        }

        private static IEnumerable<List<IType>> SelectTypesFrom(this List<IType> types, int num)
        {
            if (num == 0)
            {
                yield return new List<IType>();
                yield break;
            }

            foreach (var t in types)
            {
                foreach (var sub in SelectTypesFrom(types, num - 1))
                {
                    sub.Add(t);
                    yield return sub;
                }
            }
        }

        public static IEnumerable<CompoundType> GenCompoundType(this List<IType> types, int memberCount)
        {
            if (memberCount == 0)
            {
                yield return null;
                yield break;
            }

            var i = 0;
            foreach (var ts in types.SelectTypesFrom(memberCount))
            {
                yield return new CompoundType("CompoundType_" + (i++), ts);
            }
        }
    }
}
