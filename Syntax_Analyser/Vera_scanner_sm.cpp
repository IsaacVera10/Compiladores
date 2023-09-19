#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <unordered_map>

using namespace std;
/*
 Alumno 1: Isaac Vera Romero - 202010528
 Grupo de 1
 */


class Token {
public:
    enum Type { CADENA, ERR, END, LABEL, ID, KEYWORD, NUM, EOL };
    enum KeywordType { NOTHING, PUSH, JMPEQ, JMPGT, JMPGE, JMPLT, JMPLE, GOTO,
        SKIP, POP, DUP, SWAP, ADD, SUB, MUL, DIV, STORE, LOAD };

    Type type;
    string lexema;
    KeywordType ktype;
    string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string source);
    Token(Type type, const string& source, int first, int last);
    Token(Type type, const string source, KeywordType ktype);

    static const char* token_names[8];
    static const char* keyword_names[18];
};
unordered_map<string, Token::KeywordType> reservedWords = {
        {"push", Token::PUSH},
        {"jmpeq", Token::JMPEQ},
        {"jmpgt", Token::JMPGT},
        {"jmpge", Token::JMPGE},
        {"jmplt", Token::JMPLT},
        {"jmple", Token::JMPLE},
        {"goto", Token::GOTO},
        {"skip", Token::SKIP},
        {"pop", Token::POP},
        {"dup", Token::DUP},
        {"swap", Token::SWAP},
        {"add", Token::ADD},
        {"sub", Token::SUB},
        {"mult", Token::MUL},
        {"div", Token::DIV},
        {"store", Token::STORE},
        {"load", Token::LOAD},
};
const char* Token::token_names[8] = { "CADENA", "ERR", "END", "LABEL", "ID", "KEYWORD", "NUM", "EOL" };
const char* Token::keyword_names[18] = { "NOTHING", "PUSH", "JMPEQ", "JMPGT", "JMPGE", "JMPLT", "JMPLE", "GOTO",
                                         "SKIP", "POP", "DUP", "SWAP", "ADD", "SUB", "MUL", "DIV", "STORE", "LOAD" };

Token::Token(Type type): type(type), ktype(NOTHING) { lexema = ""; }

Token::Token(Type type, char c): type(type), ktype(NOTHING) {
    lexema = c;
}

Token::Token(Type type, const string source): type(type), ktype(NOTHING) {
    lexema = source;
}

Token::Token(Type type, const string& source, int first, int last): type(type), ktype(NOTHING) {
    text = source.substr(first, last);
}

Token::Token(Type type, const string source, KeywordType ktype): type(type), ktype(ktype) { lexema = source; }

class Scanner {
public:
    Scanner(const string& input);
    Token* nextToken();
    ~Scanner();


    friend ostream& operator<<(std::ostream& os, const Token& token);
private:
    string input;
    int first=0, current=0;
    char nextChar();
    void rollBack();
    void startLexema();
    string getLexema();
    bool isReservedWord(const string& lexema);
    void incrStartLexema();
};

Scanner::Scanner(const string& input): input(input), current(0) { }

Scanner::~Scanner() { }

char Scanner::nextChar() {
    char c = input[current];
    if (c != '\0') {
        current++;
    }
    return c;
}
void Scanner::rollBack() { // retract
    if (input[current] != '\0')
        current--;
}

void Scanner::startLexema() {
    first = current-1;
}

string Scanner::getLexema() {
    return input.substr(first,current-first);
}

bool Scanner::isReservedWord(const string& lexema) {
    return reservedWords.find(lexema) != reservedWords.end();
}

void Scanner::incrStartLexema() {
    first++;
}

Token* Scanner::nextToken() {
    Token* token;
    char c;
    c = nextChar();//c es el caracter
    while (c == ' ' || c == '\t') c = nextChar();

    if (c == '\0') {
        return new Token(Token::END);
    } else if (isdigit(c)) {
        string num;
        while (isdigit(c)) {
            num += c;
            c = nextChar();
        }
        rollBack();
        return new Token(Token::NUM, num);
    } else if (isalpha(c) || c == '_') {
            startLexema();
        string lexema;
        while (isalnum(c) || c == '_') {
            lexema += c;
            c = nextChar();
        }
        if (c == ':') { // Check for the colon after an identifier
            c = nextChar();
            rollBack();
            return new Token(Token::LABEL, lexema);
        }
        rollBack();
        if (isReservedWord(lexema)) {
            return new Token(Token::KEYWORD, lexema, reservedWords[lexema]);
        } else {
            return new Token(Token::ID, lexema);
        }
    } else if (c == '\n') {
        return new Token(Token::EOL, "\n");
    } else {
        return new Token(Token::ERR, string(1, c));
    }
}


std::ostream& operator<<(std::ostream& os, const Token& token) {
    if (token.type == Token::LABEL) {
        os << "LABEL(" << token.lexema << ")";
    } else if (token.type == Token::KEYWORD) {
        // Verifica que el KeywordType sea válido
        if (token.ktype >= Token::NOTHING && token.ktype <= Token::LOAD) {
            os << "KEYWORD "<<Token::keyword_names[token.ktype]<<" (" << token.lexema<<")";
        } else {
            os << "KEYWORD(" << token.lexema << ", UNKNOWN)";
        }
    } else if (token.type == Token::EOL) {
        os << Token::token_names[static_cast<int>(token.type)];
    }
    else if (!token.lexema.empty()) {
        os << Token::token_names[static_cast<int>(token.type)] << "(" << token.lexema << ")";
    } else {
        os << Token::token_names[static_cast<int>(token.type)] << "()"; // Agrega paréntesis vacíos para otros tokens sin lexema
    }
    return os;
}



int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Incorrect number of arguments" << endl;
        exit(1);
    }

    cout << "Reading program from file " << argv[1] << endl;
    ifstream t(argv[1]);
    stringstream buffer;
    buffer << t.rdbuf();

    Scanner scanner(buffer.str());

    Token* tk = scanner.nextToken();

    while (tk->type != Token::END) {
        cout << "next token " << *tk << endl;
        delete tk;
        tk = scanner.nextToken();
    }
    cout << "last token " << *tk << endl;
    delete tk;
}
