#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;


class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node : Value {
public:
using Value::variant;
//-----------Consructors-----------
    // Node();
    Node(Value value);

//-----------IsSmtg-----------------

    bool IsInt() const;
    bool IsDouble() const; 
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;
    
//-----------AsSmtg----------------

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const;
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    const Value& GetValue() const;

//--------Operators---------------
bool operator==(const Node& other) const {
    return GetValue() == other.GetValue();
}

bool operator!=(const Node& other) const {
    return !(*this == other);
}

// private:
//     Value value_;
};

class Document {
public:
    Document(Node root);
    const Node& GetRoot() const;
    void Clear();

private:
    Node root_;
};

Document Load(std::istream& input);
void Print(const Document& doc, std::ostream& output);

inline bool operator==(const Document& lhs, const Document& rhs) { return lhs.GetRoot() == rhs.GetRoot();}
inline bool operator!=(const Document& lhs, const Document& rhs) { return !(lhs == rhs);}

}  // namespace json