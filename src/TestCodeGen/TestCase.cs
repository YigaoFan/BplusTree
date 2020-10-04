using System.Collections.Generic;
using System.Text;

namespace TestCodeGen
{
    public class TestCase
    {
        private string name;
        private List<Section> sections;

        public TestCase(string name, params Section[] sections)
        {
            this.name = name;
            this.sections = new List<Section>();
            this.sections.AddRange(sections);
        }

        public Section Add(Section section)
        {
            sections.Add(section);
            return section;
        }

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendLine($"TESTCASE(\"{name}\")");
            builder.AppendLine("{");

            foreach (var s in sections)
            {
                builder.Append(s.ToString());
                builder.AppendLine();
            }

            builder.AppendLine("}");
            return builder.ToString();
        }
    }
}