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
    ids,
    broken_sequence,
    integer,
    real
};

const char *lex_types_text(lex_types number)
{
    switch (number)
    {
    case equal:
        return "equal";
        break;

    case delimeter:
        return "delimeter";
        break;

    case relative_operators:
        return "relative_operators";
        break;

    case keywords:
        return "keywords";
        break;

    case ids:
        return "ids";
        break;

    case broken_sequence:
        return "broken_sequence";
        break;

    case integer:
        return "integer";
        break;

    case real:
        return "real";
        break;

    default:
        return "not defined";
        break;
    }
}

class lex
{
    std::set<char> separator{' ', '\n'};
    std::set<char> non_id_chars;
    std::set<std::string> kwords;
    std::map<std::string, lex_types> symb;

public:
    void load_special(const std::string &path, lex_types type_of_s);
    void load_kwords(const std::string &path);
    std::vector<std::pair<lex_types, std::string>> parse_file(const std::string &path);
};

void lex::load_special(const std::string &path, lex_types type_of_s)
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
        symb[temp] = type_of_s;
    }
    file.close();
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
    file.get(temp);
    do
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
                file.get(temp);
            }
            break;

        case symbols:
            if (non_id_chars.count(temp))
            {
                lex_m += temp;
                file.get(temp);
            }
            else
            {
                state = start;
                if (symb.find(lex_m) != symb.end())
                {
                    result.push_back({symb[lex_m], lex_m});
                }
                else
                {
                    result.push_back({broken_sequence, lex_m});
                }
            }
            break;

        case text:
            if (!separator.count(temp) && !non_id_chars.count(temp))
            {
                lex_m += temp;
                file.get(temp);
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
        }
    } while (!file.eof());
    file.close();
    for (auto &i : result)
    {
        if (i.first == ids)
        {
            try
            {
                std::stoi(i.second);
                i.first = integer;
            }
            catch (std::invalid_argument)
            {
                try
                {
                    std::stof(i.second);
                    i.first = real;
                }
                catch (std::invalid_argument)
                {
                }
            }
            if (i.first == integer && i.second.find('.') != std::string::npos)
            {
                try
                {
                    std::stof(i.second);
                    i.first = real;
                }
                catch (std::invalid_argument)
                {
                }
            }
        }
    }
    return result;
}

void save_lex_resul_to_file(const std::string &path, const std::vector<std::pair<lex_types, std::string>> &result)
{
    std::ofstream file(path);
    file << "\"№\",\"type\",\"text\"" << std::endl;
    for (size_t i = 0; i < result.size(); i++)
    {
        file << "\"" << i << "\",\"" << lex_types_text(result[i].first) << "\",\"" << result[i].second << "\"" << std::endl;
    }
    file.close();
}

int main()
{
    lex test_lex;
    test_lex.load_kwords("KEYWORDS.txt");
    test_lex.load_special("EQ.txt", equal);
    test_lex.load_special("REL_OPS.txt", relative_operators);
    test_lex.load_special("DELIMETER.txt", delimeter);
    auto result = test_lex.parse_file("example.alg");
    save_lex_resul_to_file("result.csv", result);
}