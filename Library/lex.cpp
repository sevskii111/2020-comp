#include "lex.hpp"

const char *lex::lex_types_text(lex::types number)
{
    static const std::map<types, std::string> text_types{{equal, "equal"},
                                                         {delimeter, "delimeter"},
                                                         {relative_operators, "relative_operators"},
                                                         {keywords, "keywords"},
                                                         {ids, "ids"},
                                                         {broken_sequence, "broken_sequence"},
                                                         {integer, "integer"},
                                                         {real, "real"}};
    if (text_types.find(number) != text_types.end())
    {
        return text_types.find(number)->second.c_str();
    }
    else
    {
        return "not defined";
    }
}

void lex::load_special(const std::string &path, lex::types type_of_s)
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

std::vector<std::pair<lex::types, std::string>> lex::parse_file(const std::string &path)
{
    std::vector<std::pair<types, std::string>> result;
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