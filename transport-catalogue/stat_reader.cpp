#include "stat_reader.h"
namespace TransportCatalogue{
    
void SReader::ReadStat(const TransportCatalogue& tc){
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
            OutBusStat(tc, name);
        }else if(request_type == "Stop"){
            std::string name = text_sv.substr(1);
            OutStopRoutes(tc, name);
        }
    }
}

void SReader::OutBusStat(const TransportCatalogue& tc, std::string_view bus_name){
    auto res_opt = tc.GetBusInfo(bus_name);
    if(res_opt.has_value()){
    Utility::BusInfo res = res_opt.value();
    std::cout << "Bus " << bus_name << ": " << res.all_count << " stops on route, " <<
        res.unique_count << " unique stops, " << std::setprecision(6) << res.length << " route length, " << 
        res.curvation << " curvature"<< std::endl;
    }else{
        std::cout << "Bus " << bus_name << ": not found" << std::endl;
    }

}

void SReader::OutStopRoutes(const TransportCatalogue& tc, std::string_view stop_name){

    auto res_opt = tc.GetStopRoutes(stop_name);
    if(res_opt.has_value()){
    std::unordered_set <std::string_view> unordered_res = res_opt.value();
    if(unordered_res.size() == 0){ std::cout << "Stop " << stop_name << ": no buses" << std::endl; } else{
        std::set <std::string_view> res;
//Проверь есть ли смысл тут в move
        for (auto route : unordered_res){ res.insert(std::move(route)); }
        std::cout << "Stop " << stop_name << ": buses ";
        for(auto route : res){ std::cout << route << " "; }
        std::cout <<std::endl;}
    }else{
        std::cout << "Stop " << stop_name << ": not found" << std::endl;
    }
}
}