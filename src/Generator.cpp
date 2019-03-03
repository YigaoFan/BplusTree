// Created by 范义高 on 2019-03-03.
// 这个文件主要是用来生成函数库的两个文件
#include <vector> // for vector
#include <string> // for string
#include <array> // for array
#include "Generator.hpp"

// 这些 using 应该在 namespace 里面还是外面？这个文件是 cpp 文件，所以本身没有那么大的影响
using std::vector;
using std::string;
using std::array;

namespace func_lib {
    vector<string>
    scanf_source(const string& src_dir)
    {
       vector<string> res;

       return res;
    }

    vector<string>
    compile(const vector<string>& sources, const string& save_object_dir)
    {
        vector<string> res;
        res.reserve(sources.size());

        return res;
    }

    void
    construct_LIB_file(const vector<string>& objs, const string& target_dir)
    {

        // open a file

        // add obj content into file
    }

    void
    construct_IDX_file(const vector<string>& objs, const string& target_dir)
    {
        // set the key-value to call btree
    }

    void
    generate_function_library()
    {
        // paths could be read from some configure file or other way
        const string& source_folder = "TODO";
        const string& obj_folder = "TODO";
        const string& target_save_folder = "TODO";

        // need to require a file just include a function or use program to scan
        // suggest to use program to scan to set different function into different obj
        auto sources = scanf_source(source_folder);
        auto objs = compile(sources, obj_folder);

        construct_LIB_file(objs, target_save_folder);
        construct_IDX_file(objs, target_save_folder);

    }
}

