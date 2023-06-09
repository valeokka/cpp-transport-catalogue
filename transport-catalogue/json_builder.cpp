#include "json_builder.h"

#include <iostream>

namespace json{

    KeyItemContext Builder::Key(std::string key){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(!nodes_stack_.empty() && nodes_stack_.back()->IsDict()){
            Node* node = new Node(std::move(key));
            nodes_stack_.push_back(node);

        }else{
            throw std::logic_error("There is no dict to add key or another key isn`t closed");
        }
        return {*this};
    }

    Builder& Builder::Value(Node::Value value_in){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        Node value(value_in);
        if(nodes_stack_.empty()){ //Не начат ни словарь, ни массив
            root_ = std::move(value);
        }else if(nodes_stack_.back()->IsString()){ //В nodes_stack ключ от Map
            Node* key_node = nodes_stack_.back();
            nodes_stack_.pop_back();
            Node* dict_node = nodes_stack_.back();
            nodes_stack_.pop_back();

            json::Dict dict = std::move(dict_node->AsDict());
            std::string key = std::move(key_node->AsString());
            dict[std::move(key)] = std::move(value);

            delete key_node;
            delete dict_node;

            Node* node = new Node(std::move(dict));

            nodes_stack_.push_back(node);

        }else if(nodes_stack_.back()->IsArray()){
            Node* array_node = nodes_stack_.back();
            nodes_stack_.pop_back();

            json::Array array = std::move(array_node->AsArray());
            array.push_back(std::move(value));

            delete array_node;

            Node* node = new Node(std::move(array));

            nodes_stack_.push_back(node);
        }else{
            throw std::logic_error("You may add only Dicts, Arrays and values");
        }
        return *this;
    }
    
    DictItemContext Builder::StartDict(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        nodes_stack_.emplace_back(new Node(json::Dict{}));
        return {*this};
    }

    ArrayItemContext Builder::StartArray(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        nodes_stack_.emplace_back(new Node(json::Array{}));
        return {*this};
    }

    Builder& Builder::EndDict(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(nodes_stack_.back()->IsDict()){
            Node* dict_node = nodes_stack_.back();
            nodes_stack_.pop_back();
            json::Dict dict = dict_node->AsDict();     

            delete dict_node;

            Value(std::move(dict));
        }else{
            throw std::logic_error("There is no open dicts or something else is open");
        }
        return *this;
    }

    Builder& Builder::EndArray(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(nodes_stack_.back()->IsArray()){
            Node* array_node = nodes_stack_.back();
            nodes_stack_.pop_back();
            json::Array array = array_node->AsArray();

            delete array_node;
            
            Value(std::move(array));
        }else{
            throw std::logic_error("There is no open array or something else is open");
        }
        return *this;

    }

    json::Node Builder::Build(){
        if(root_ == nullptr || !nodes_stack_.empty()){
            throw std::logic_error("There is something opened here or nothing to build");
        }else{
            return root_;
        }
    }



KeyItemContext DictItemContext::Key(std::string key){
    return {builder_.Key(key)};
}

Builder& DictItemContext::EndDict(){
    return builder_.EndDict();
}

KeyItemContext DictValueContext::Key(std::string key){
    return {builder_.Key(key)};
}

Builder& DictValueContext::EndDict(){
    return builder_.EndDict();
}
    
ArrayValueContext ArrayItemContext::Value(Node::Value value_in){
    builder_.Value(value_in);
    return {builder_};
}

ArrayItemContext ArrayItemContext::StartArray(){
    return {builder_.StartArray()};
}

DictItemContext ArrayItemContext::StartDict(){
    return {builder_.StartDict()};
}

Builder& ArrayItemContext::EndArray(){
    return {builder_.EndArray()};
}

ArrayValueContext ArrayValueContext::Value(Node::Value value_in){
    builder_.Value(value_in);
    return {builder_};
}

ArrayItemContext ArrayValueContext::StartArray(){
    return {builder_.StartArray()};
}

DictItemContext ArrayValueContext::StartDict(){
    return {builder_.StartDict()};
}

Builder& ArrayValueContext::EndArray(){
    return {builder_.EndArray()};
}

KeyValueContext KeyItemContext::Value(Node::Value value_in){
    builder_.Value(value_in);
    return {builder_};
}

ArrayItemContext KeyItemContext::StartArray(){
    return {builder_.StartArray()};
}

DictItemContext KeyItemContext::StartDict(){
    return {builder_.StartDict()};
}

KeyValueContext KeyValueContext::Value(Node::Value value_in){
    builder_.Value(value_in);
    return {builder_};
}

KeyItemContext KeyValueContext::Key(std::string key){
    return {builder_.Key(key)};
}

Builder& KeyValueContext::EndDict(){
    return {builder_.EndDict()};
}



}