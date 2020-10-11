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

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendLine($"#include \"{testFramePath}\"");
            builder.AppendLine("#include \"Util.hpp\"");
            builder.AppendLine("#include \"../ByteConverter.hpp\"");
            builder.AppendLine("#include \"../Store/FileWriter.hpp\"");
            builder.AppendLine("#include \"../Store/FileReader.hpp\"");
            builder.AppendLine("#include <memory>");
            builder.AppendLine();
            builder.AppendLine("using namespace FuncLib;");
            builder.AppendLine("using namespace FuncLib::Store;");
            builder.AppendLine("using namespace FuncLib::Test;");
            builder.AppendLine("using namespace std;");
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
