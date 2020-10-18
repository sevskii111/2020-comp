#include "Library/lex.hpp"

void save_lex_resul_to_file(const std::string &path, const std::vector<std::pair<lex::types, std::string>> &result)
{
    std::ofstream file(path);
    file << "\"№\",\"type\",\"text\"" << std::endl;
    for (size_t i = 0; i < result.size(); i++)
    {
        file << "\"" << i << "\",\"" << lex::lex_types_text(result[i].first) << "\",\"" << result[i].second << "\"" << std::endl;
    }
    file.close();
}

int main()
{
    lex test_lex;
    test_lex.load_kwords("KEYWORDS.txt");
    test_lex.load_special("EQ.txt", lex::types::equal);
    test_lex.load_special("REL_OPS.txt", lex::types::relative_operators);
    test_lex.load_special("DELIMETER.txt", lex::types::delimeter);
    auto result = test_lex.parse_file("example.alg");
    save_lex_resul_to_file("result.csv", result);
}
