#pragma once

#include <vector>

#include "json.h"

namespace json{

class ItemContext;
class DictItemContext;
class DictValueContext;
class ArrayItemContext;
class ArrayValueContext;
class KeyItemContext;
class KeyValueContext;

class Builder{
public:
    KeyItemContext Key(std::string key);
    Builder& Value(Node::Value value_in);
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    json::Node Build();

private:
    Node root_; 
    std::vector<Node*> nodes_stack_;

};




class ItemContext{
public:
    ItemContext(Builder& builder) : builder_(builder){}
protected:
    Builder& builder_;

};

class DictItemContext : public ItemContext{
public:
    DictItemContext(Builder& builder) : ItemContext(builder) {}
    KeyItemContext Key(std::string key);
    Builder& EndDict();
};

class DictValueContext : public ItemContext{
public:
    DictValueContext(Builder& builder) : ItemContext(builder) {}
    KeyItemContext Key(std::string key);
    Builder& EndDict();
    
};

class ArrayItemContext : public ItemContext{
public:
    ArrayItemContext(Builder& builder) : ItemContext(builder) {}
    ArrayValueContext Value(Node::Value value_in);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
    Builder& EndArray();

};
class ArrayValueContext : public ItemContext{
public:
    ArrayValueContext(Builder& builder) : ItemContext(builder) {}
    ArrayValueContext Value(Node::Value value_in);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
    Builder& EndArray();
};

class KeyItemContext : public ItemContext{
public:
    KeyItemContext(Builder& builder) : ItemContext(builder) {}
    KeyValueContext Value(Node::Value value_in);
    ArrayItemContext StartArray();
    DictItemContext StartDict();
};

class KeyValueContext : public ItemContext{
public:
    KeyValueContext(Builder& builder) : ItemContext(builder) {}
    KeyValueContext Value(Node::Value value_in);
    KeyItemContext Key(std::string key);
    Builder& EndDict();
    
};

}