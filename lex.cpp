#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

enum lex_types
{
    equal,
    delimeter,
    relative_operators,
    keywords,
    ids
};

class lex
{
    std::set<char> separator{' ', '\n'};
    std::set<char> non_id_chars;
    std::set<std::string> eq, rel_ops, delimeters, kwords;
    void load_special(std::set<std::string> &pos, const std::string &path);

public:
    void load_eq(const std::string &path);
    void load_rel_ops(const std::string &path);
    void load_delimeters(const std::string &path);
    void load_kwords(const std::string &path);
    std::vector<std::pair<lex_types, std::string>> parse_file(const std::string &path);
};

void lex::load_special(std::set<std::string> &pos, const std::string &path)
{
    std::ifstream file(path);
    while (!file.eof())
    {
        std::string temp;
        getline(file, temp);
        for (auto &i : temp)
        {
            non_id_chars.insert(i);
        }
        pos.insert(temp);
    }
    file.close();
}

void lex::load_eq(const std::string &path)
{
    load_special(eq, path);
}

void lex::load_rel_ops(const std::string &path)
{
    load_special(rel_ops, path);
}

void lex::load_delimeters(const std::string &path)
{
    load_special(delimeters, path);
}

void lex::load_kwords(const std::string &path)
{
    std::ifstream file(path);
    while (!file.eof())
    {
        std::string temp;
        getline(file, temp);
        kwords.insert(temp);
    }
    file.close();
}

std::vector<std::pair<lex_types, std::string>> lex::parse_file(const std::string &path)
{
    std::vector<std::pair<lex_types, std::string>> result;
    std::ifstream file(path);

    enum states
    {
        start,
        symbols,
        text
    };

    char temp;
    std::string lex_m{""};
    states state = start;
    file.get(&temp, 1);
    while (!file.eof())
    {
        switch (state)
        {
        case start:
            lex_m = "";
            if (!separator.count(temp))
            {
                if (non_id_chars.count(temp))
                {
                    state = symbols;
                }
                else
                {
                    state = text;
                }
            }
            else
            {
                file.get(&temp, 1);
            }
            break;

        case symbols:
            if (non_id_chars.count(temp))
            {
                lex_m += temp;
                file.get(&temp, 1);
            }
            else
            {
                state = start;
                if (eq.count(lex_m))
                {
                    result.push_back({equal, lex_m});
                }
                else
                {
                    if (rel_ops.count(lex_m))
                    {
                        result.push_back({relative_operators, lex_m});
                    }
                    else
                    {
                        if (delimeters.count(lex_m))
                        {
                            result.push_back({delimeter, lex_m});
                        }
                        else
                        {
                            throw "Not found combination of symbols";
                        }
                    }
                }
            }
            break;

        case text:
            if (!separator.count(temp) && !non_id_chars.count(temp))
            {
                lex_m += temp;
                file.get(&temp, 1);
            }
            else
            {
                state = start;
                if (kwords.count(lex_m))
                {
                    result.push_back({keywords, lex_m});
                }
                else
                {
                    result.push_back({ids, lex_m});
                }
            }
            break;

        default:
            break;
        }
    }
}