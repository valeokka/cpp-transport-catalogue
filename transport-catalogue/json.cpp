#include "json.h"

using namespace std;

namespace json {
namespace {

using namespace std::literals;

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) { out.put(' '); }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent}; 
    }
};

Node LoadNode(std::istream& input);
Node LoadString(std::istream& input);
void PrintNode(const Node& node, const PrintContext& ctx);

Node LoadNumber(std::istream& input) {

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {throw ParsingError("Failed to read number from stream"s);}
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) { throw ParsingError("A digit is expected"s); }
        while (std::isdigit(input.peek())) { read_char(); }
    };

    if (input.peek() == '-') { read_char();}
    // Парсим целую часть числа
    if (input.peek() == '0') { read_char();
    // После 0 в JSON не могут идти другие цифры
    } else { read_digits();}

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node(std::stoi(parsed_num));
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node(std::stod(parsed_num));
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

Node LoadArray(std::istream& input) {
    Array result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') { input.putback(c); }
        result.push_back(LoadNode(input));
    }

    if (!input) { throw ParsingError("Load Array error");}

    return {std::move(result)};
}

Node LoadDict(std::istream& input) {
    Dict dict;
    auto dict_parse_error = "Dictionary parsing error"s;
    char c;

    while (input >> c && c != '}') {
        if (c == '"') {
        auto key = LoadString(input).AsString();

        if (input >> c && c == ':') {
            if (dict.find(key) != dict.end()) {
                throw ParsingError(dict_parse_error);
            }
            dict.emplace(std::move(key), LoadNode(input));
        } else {
            throw ParsingError(dict_parse_error);
        }
        } else if (c != ',') {
            throw ParsingError(dict_parse_error);
        }
    }
    if (!input) { throw ParsingError(dict_parse_error);}
    return {std::move(dict)};
}

Node LoadString(std::istream& input) {
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n': s.push_back('\n'); break;
                case 't': s.push_back('\t'); break;
                case 'r': s.push_back('\r'); break;
                case '"': s.push_back('"'); break;
                case '\\': s.push_back('\\'); break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node(s);
}

void MakeStr(std::istream &input, std::string &result) {
    char c;
    while (std::isalpha(input.peek())) {
    input >> c;
    result += c;
    }
}

Node LoadBool(std::istream& input){
    string boolean;
    MakeStr(input, boolean);
    if (boolean == "true"sv){ return Node{true};
    }else if (boolean == "false"sv){return Node{false};
    }else{ throw ParsingError("Expected bool"s);}
}

Node LoadNull(istream& input){
    string null;
    MakeStr(input, null);
    if(null == "null"sv){
        return Node(nullptr);
    }else{
        throw ParsingError("Expected null"s);
    }
}

Node LoadNode(std::istream& input) {
    char c;
    if (!(input >> c)) { throw ParsingError("node is empty"s);}
    
    switch (c) {
        case '[': return LoadArray(input);
        case '{': return LoadDict(input);
        case '"': return LoadString(input);
        case 't': [[fallthrough]]; // true и false обрабатываются одинаково
        case 'f': input.putback(c); return LoadBool(input);
        case 'n': input.putback(c); return LoadNull(input);
        default: input.putback(c); return LoadNumber(input);
    }
}

void PrintString(const std::string &str, const PrintContext& ctx) {
    ctx.out << '"';
    for (const char c : str) {
        switch (c) {
            case '\r': ctx.out << R"(\r)"; break;
            case '\n': ctx.out << R"(\n)"; break;
            case '"': [[fallthrough]]; // (") и (\) обрабатываются одинаково
            case '\\': ctx.out << '\\'; [[fallthrough]];
            default: ctx.out << c; break;
        }
    }
    ctx.out << '"';
}

template <typename T>
void PrintValue(const T &value, const PrintContext& ctx) {
    //ctx.PrintIndent();
    ctx.out << value;
}

void PrintValue(const std::string& value, const PrintContext& ctx) {
    //ctx.PrintIndent();
    PrintString(value, ctx);
}

void PrintValue(const std::nullptr_t&, const PrintContext& ctx) {
    //ctx.PrintIndent();
    ctx.out << "null"sv;
}

void PrintValue(const bool &value, const PrintContext& ctx) {
    //ctx.PrintIndent();
    ctx.out << std::boolalpha << value;
}

void PrintValue(const Array& nodes, const PrintContext& ctx) {

    ctx.out << "[\n"sv;
    bool first = true;
        auto ctxInd = ctx.Indented();
    for (const Node &node : nodes) {
        if (first) { first = false;} 
        else { ctx.out << ",\n"sv;}
        ctxInd.PrintIndent();
        PrintNode(node, ctxInd);
    }
    ctx.out << "\n";
    ctx.PrintIndent();
    ctx.out << "]"sv;
}

void PrintValue(const Dict& nodes, const PrintContext& ctx) {
    ctx.out << "{\n"sv;
    bool first = true;
        auto ctxInd = ctx.Indented();
    for (const auto& [key, node] : nodes) {
        if (first) { first = false;} 
        else { ctx.out << ",\n"sv;}
        ctxInd.PrintIndent();
        PrintValue(key, ctx);
        ctxInd.out << ": "sv;
        PrintNode(node, ctx);
    }
    ctx.out << "\n";
    ctx.PrintIndent();
    ctx.out << "}"sv;
}

void PrintNode(const Node& node, const PrintContext& ctx) {
  std::visit([&ctx](const auto& value) {
    PrintValue(value, ctx);
  }, node.GetValue());
}

}  // namespace


//---------------Constructors-------------

Node::Node(Value value) : Value(std::move(value)) {}

//---------------IsSmtg------------------

bool Node::IsInt() const { return std::holds_alternative<int>(*this);}
bool Node::IsPureDouble() const { return std::holds_alternative<double>(*this);}
bool Node::IsDouble() const { return IsInt() || IsPureDouble();}
bool Node::IsBool() const { return std::holds_alternative<bool>(*this);}
bool Node::IsString() const { return std::holds_alternative<std::string>(*this);}
bool Node::IsNull() const { return std::holds_alternative<std::nullptr_t>(*this);}
bool Node::IsArray() const { return std::holds_alternative<Array>(*this);}
bool Node::IsMap() const { return std::holds_alternative<Dict>(*this);}

//-----------AsSmtg----------------

const Array& Node::AsArray() const {
    return IsArray() ? std::get<Array>(*this) : throw std::logic_error("Is not Array"s);}

const Dict& Node::AsMap() const {
    return IsMap() ? std::get<Dict>(*this) : throw std::logic_error("Is not Dict"s);}

bool Node::AsBool() const {
    return IsBool() ? std::get<bool>(*this) : throw std::logic_error("Is not bool"s);}

int Node::AsInt() const {
    return IsInt() ? std::get<int>(*this) : throw std::logic_error("Is not int"s);}

const std::string &Node::AsString() const {
    return IsString() ? std::get<std::string>(*this) : throw std::logic_error("Is not string"s);}  

double Node::AsDouble() const {
    if (IsInt()) { return std::get<int>(*this);}
    if (IsPureDouble()) { return std::get<double>(*this); }
    throw std::logic_error("Is not double"s);}

const Value& Node::GetValue() const{ return *this;}

//----------Document------------

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

void Document::Clear(){
    root_ = {};
}

Document Load(std::istream& input) { return Document{ LoadNode(input) };}
void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), PrintContext{ output });}

}  // namespace json