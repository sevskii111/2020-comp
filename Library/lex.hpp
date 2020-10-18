#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

class lex
{
public:
    enum types
    {
        equal,
        delimeter,
        relative_operators,
        keywords,
        ids,
        broken_sequence,
        integer,
        real
    };

private:
    std::set<char> separator{' ', '\n'};
    std::set<char> non_id_chars;
    std::set<std::string> kwords;
    std::map<std::string, types> symb;

public:
    static const char *lex_types_text(types number);
    void load_special(const std::string &path, types type_of_s);
    void load_kwords(const std::string &path);
    std::vector<std::pair<types, std::string>> parse_file(const std::string &path);
};

#endif