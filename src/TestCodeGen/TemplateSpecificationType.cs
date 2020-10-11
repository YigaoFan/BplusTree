using System;
using System.Collections.Generic;
using System.Text;

namespace TestCodeGen
{
    public class TemplateSpecificationType : IType
    {
        private IType[] templateArgs;
        private Template template;

        public string Name
        {
            get
            {
                var builder = new StringBuilder(template.Name);
                builder.Append('<');
                foreach (var arg in templateArgs)
                {
                    builder.Append(arg.Name + ", ");
                }

                builder.Length -= 2;
                builder.Append('>');

                return builder.ToString();
            }
        }

        public IEnumerable<string> InitCode
        {
            get
            {
                yield return "{";
                yield return template.ComposeInitCode(templateArgs);
                yield return "}";
            }
        }

        public TemplateSpecificationType(Template template, IType[] templateArgs)
        {
            this.template = template;
            this.templateArgs = templateArgs;
        }
    }
}
