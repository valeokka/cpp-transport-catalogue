#include "stat_reader.h"

void SReader::ReadStat(TransportCatalogue::TransportCatalogue& tc){
    std::string request_count_string;
    std::getline(is_, request_count_string);
    int request_count = std::stoi(request_count_string);

    for(int i = 0; i < request_count; ++i){
        std::string text;
        std::getline(is_, text);
        std::string text_sv = text.substr(text.find_first_not_of(' '));
        std::string request_type = text_sv.substr(0, text. find(' '));
        text_sv = text_sv.substr(text_sv.find(' '));
        if(request_type == "Bus"){
            std::string name = text_sv.substr(1);
            if(tc.GetBus(name) != nullptr){
            OutBusStat(tc, name);
            }else{
                std::cout << "Bus " << name << ": not found" << std::endl;
            }
        }else if(request_type == "Stop"){
            std::string name = text_sv.substr(1);
            if(tc.GetStop(name) != nullptr){
                OutStopRoutes(tc, name);
            }else{
                std::cout << "Stop " << name << ": not found" << std::endl;
            }
        }
    }
}

void SReader::OutBusStat(TransportCatalogue::TransportCatalogue& tc, std::string_view bus_name){
    TransportCatalogue::Utility::BusInfo res = tc.GetBusInfo(bus_name);
    std::cout << "Bus " << bus_name << ": " << res.all_count << " stops on route, " <<
    res.unique_count << " unique stops, " << std::setprecision(6) << res.length << " route length, " << 
    res.curvation << " curvature"<< std::endl;
}

void SReader::OutStopRoutes(TransportCatalogue::TransportCatalogue& tc, std::string_view stop_name){

    std::unordered_set <std::string_view> unordered_res = tc.GetStopRoutes(stop_name);
    if(unordered_res.size() == 0){ std::cout << "Stop " << stop_name << ": no buses" << std::endl; } else{
        std::set <std::string_view> res;
//Проверь есть ли смысл тут в move
        for (auto route : unordered_res){ res.insert(std::move(route)); }
        std::cout << "Stop " << stop_name << ": buses ";
        for(auto route : res){ std::cout << route << " "; }
        std::cout <<std::endl;}
}
