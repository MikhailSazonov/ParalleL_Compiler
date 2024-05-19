#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "funcdef.hpp"
#include "annotate.hpp"
#include "oop.hpp"

struct Type;

namespace Def {
    const std::string MAIN_NAME = "main";
    const std::string EXPR_SPEC_SYMB = "!()+-*/%<>=.\'\"";
    const std::string TYPE_DELIMETER = "->";
    const std::string EXPR_DELIMETER = " ";
    const char MANGLING_SYMBOL = '%';
    const char UNPACKING_SYMBOL = '#';
    const std::string GEN_NAME = "GEN";
    const char ANNOT_DELIM_SYMBOL = '_';
    const std::string FILE_EXT = ".pl";

    const std::string WHERE_KEYWORD = "where";
    const std::string CLASS_KEYWORD = "class";

    const std::string ANNOTATE_KEYWORD = "annotate";

    typedef std::unordered_map<std::string, std::shared_ptr<Type>> TypeTable;
    typedef std::unordered_map<std::string, std::vector<FuncDef>> FuncTable;
    typedef std::unordered_map<std::string, Annotation> AnnTable;
}