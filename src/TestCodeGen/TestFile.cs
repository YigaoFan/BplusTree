using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace TestCodeGen
{
    public class TestFile
    {
        private string testFramePath;
        private string testFuncName;
        private TestCase[] testCases;

        public TestFile(string testFramePath, string testFuncName, params TestCase[] testCases)
        {
            this.testFramePath = testFramePath;
            this.testFuncName = testFuncName;
            this.testCases = testCases;
        }

        //public TestCase Add(TestCase testCase)
        //{
        //    testCases.Add(testCase);
        //    return testCase;
        //}

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendLine($"#include \"{testFramePath}\"");
            builder.AppendLine();

            foreach (var tc in testCases)
            {
                builder.Append(tc.ToString());
            }

            builder.AppendLine();
            builder.AppendLine($"DEF_TEST_FUNC({testFuncName})");
            return builder.ToString();
        }
    }
}
