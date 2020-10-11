using System.Collections.Generic;
using System.Text;

namespace TestCodeGen
{
    public class TestCase
    {
        private string name;
        private IEnumerable<string> codes;
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

        public void AddCode(IEnumerable<string> codes)
        {
            this.codes = codes;
        }

        public override string ToString()
        {
            var builder = new StringBuilder();
            builder.AppendLine($"TESTCASE(\"{name}\")");
            builder.AppendLine("{");

            if (codes != null)
            {
                var tab = "\t";
                foreach (var c in codes)
                {
                    builder.AppendLine(tab + c);
                }
            }

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