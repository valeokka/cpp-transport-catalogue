#include "json_builder.h"

#include <iostream>
#include <variant>
namespace json{

    Builder::KeyItemContext Builder::Key(std::string key){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(!nodes_stack_.empty() && nodes_stack_.back()->IsDict()){
            nodes_stack_.push_back(std::make_unique<Node>(std::move(key)));

        }else{
            throw std::logic_error("There is no dict to add key or another key isn`t closed");
        }
        return {*this};
    }

    Builder& Builder::Value(Node::Value value_in){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        Node value(std::move(value_in));
        if(nodes_stack_.empty()){ //Не начат ни словарь, ни массив
            root_ = std::move(value);
        }else if(nodes_stack_.back()->IsString()){ //В nodes_stack ключ от Map
        
            std::string key = std::move(nodes_stack_.back()->AsString());
            nodes_stack_.pop_back();
            nodes_stack_.back()->ModifyDict()[std::move(key)] = std::move(value);

        }else if(nodes_stack_.back()->IsArray()){

            nodes_stack_.back()->ModifyArray().push_back(std::move(value));

        }else{
            throw std::logic_error("You may add only Dicts, Arrays and values");
        }
        return *this;
    }
    
    Builder::DictItemContext Builder::StartDict(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        nodes_stack_.emplace_back(std::make_unique<Node>(json::Dict{}));
        return {*this};
    }

    Builder::ArrayItemContext Builder::StartArray(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        nodes_stack_.emplace_back(std::make_unique<Node>(json::Array{}));
        return {*this};
    }

    Builder& Builder::EndDict(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(nodes_stack_.back()->IsDict()){
            auto dict_node = std::move(nodes_stack_.back());
            nodes_stack_.pop_back();
            json::Dict dict = dict_node->AsDict();     

            Value(std::move(dict));
        }else{
            throw std::logic_error("There is no open dicts or something else is open");
        }
        return *this;
    }

    Builder& Builder::EndArray(){
        if(root_!= nullptr){throw std::logic_error("Root is finished");}
        if(nodes_stack_.back()->IsArray()){
            auto array_node = std::move(nodes_stack_.back());
            nodes_stack_.pop_back();
            json::Array array = array_node->AsArray();
            
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

Builder::KeyItemContext Builder::DictItemContext::Key(std::string key){
    return {builder_.Key(std::move(key))};
}

Builder& Builder::DictItemContext::EndDict(){
    return builder_.EndDict();
}

Builder::ArrayItemContext Builder::ArrayItemContext::StartArray(){
    return {builder_.StartArray()};
}

Builder::DictItemContext Builder::ArrayItemContext::StartDict(){
    return {builder_.StartDict()};
}

Builder& Builder::ArrayItemContext::EndArray(){
    return {builder_.EndArray()};
}

Builder::KeyValueContext Builder::KeyItemContext::Value(Node::Value value_in){
    builder_.Value(std::move(value_in));
    return {builder_};
}

Builder::ArrayItemContext Builder::KeyItemContext::StartArray(){
    return {builder_.StartArray()};
}

Builder::DictItemContext Builder::KeyItemContext::StartDict(){
    return {builder_.StartDict()};
}

Builder::KeyItemContext Builder::KeyValueContext::Key(std::string key){
    return {builder_.Key(std::move(key))};
}

Builder& Builder::KeyValueContext::EndDict(){
    return {builder_.EndDict()};
}

}