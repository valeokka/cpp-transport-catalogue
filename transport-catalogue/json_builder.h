#pragma once

#include <vector>
#include <memory>

#include "json.h"

namespace json{

class Builder{

public:

    class ItemContext;
    class DictItemContext;
    class ArrayItemContext;
    class KeyItemContext;
    class KeyValueContext;

    KeyItemContext Key(std::string key);
    Builder& Value(Node::Value value_in);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    json::Node Build();
private:
    Node root_; 
    std::vector<std::unique_ptr<Node>> nodes_stack_;

};

class Builder::ItemContext{
public:
    ItemContext(Builder& builder) : builder_(builder){}
protected:
    Builder& builder_;

};

class Builder::DictItemContext : public Builder::ItemContext{
public:
    DictItemContext(Builder& builder) : ItemContext(builder) {}
    KeyItemContext Key(std::string key);
    Builder& EndDict();
};

class Builder::ArrayItemContext : public Builder::ItemContext{
public:
    ArrayItemContext(Builder& builder) : ItemContext(builder) {}
    ArrayItemContext Value(Node::Value value_in);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
    Builder& EndArray();

};

class Builder::KeyItemContext : public Builder::ItemContext{
public:
    KeyItemContext(Builder& builder) : ItemContext(builder) {}
    KeyValueContext Value(Node::Value value_in);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
};

class Builder::KeyValueContext : public Builder::ItemContext{
public:
    KeyValueContext(Builder& builder) : ItemContext(builder) {}
    KeyItemContext Key(std::string key);
    Builder& EndDict();
    
};

}