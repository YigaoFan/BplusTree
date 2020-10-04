using System.Text;

namespace TestCodeGen
{
    public class Section
    {
        private string name;
        private CodeSnippet_ByteConverter snippet;

        public Section(string name, CodeSnippet_ByteConverter snippet)
        {
            this.name = name;
            this.snippet = snippet;
        }

        public override string ToString()
        {
            var builder = new StringBuilder();

            var tab = "\t";
            builder.AppendLine(tab + $"SECTION(\"{name}\")");
            builder.AppendLine(tab + '{');
            builder.Append(snippet.CodeWithIndent(2, tab));
            builder.AppendLine(tab + '}');

            return builder.ToString();
        }
    }
}