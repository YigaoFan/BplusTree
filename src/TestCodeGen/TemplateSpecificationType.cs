using System;
using System.Text;

namespace TestCodeGen
{
    public class TemplateSpecificationType : IType
    {
        private string templateName;
        private IType[] templateArgs;

        public string Name
        {
            get
            {
                var builder = new StringBuilder(templateName);
                builder.Append('<');
                foreach (var arg in templateArgs)
                {
                    builder.Append(arg.Name + ',');
                }

                builder.Length -= 1;
                builder.Append('>');

                return builder.ToString();
            }
        }

        public string Init => throw new NotImplementedException();// 这个依赖哪个template

        public TemplateSpecificationType(string templateName, IType[] templateArgs)
        {
            this.templateName = templateName;
            this.templateArgs = templateArgs;
        }


    }
}
