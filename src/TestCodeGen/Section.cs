using System.Text;

namespace TestCodeGen
{
    public class Section
    {
        private string name;

        public Section(string name)
        {
            this.name = name;
        }

        public override string ToString()
        {
            var builder = new StringBuilder();

            var tab = "\t";
            builder.AppendLine(tab + $"SECTION(\"{name}\")");
            builder.AppendLine(tab + "{");
            builder.AppendLine(tab + "}");

            return builder.ToString();
        }
    }
}