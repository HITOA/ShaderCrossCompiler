#ifndef SHADERCROSSCOMPILER_TOKENIZER_H
#define SHADERCROSSCOMPILER_TOKENIZER_H

#include <vector>
#include <string>

namespace ShaderCC {
    enum class TokenType {
        TOKEN_IDENTIFIER,
        TOKEN_STRING,
        TOKEN_NUMBER,
        TOKEN_EQUAL,
        TOKEN_NEWLINE,
        TOKEN_WHITESPACE,
        TOKEN_BLOCKSTART,
        TOKEN_BLOCKEND,
        TOKEN_EOF
    };

    class Tokenizer {
    public:
        std::vector<std::pair<std::string, TokenType>> TokenizeAll(std::string_view data);

        std::pair<std::string, TokenType> Tokenize(std::string_view data);
    };
}
#endif //SHADERCROSSCOMPILER_TOKENIZER_H
