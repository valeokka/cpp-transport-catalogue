#include "input_reader.h"
namespace TransportCatalogue{

void IReader::ReadInput(TransportCatalogue &tc)
{

    std::unordered_map<std::string, Stop> stops;
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>> buses;
    std::vector<std::tuple<std::string, std::string, int>> distance_stops; 
    std::string request_count_string;
    std::getline(is_, request_count_string);
    int request_count = std::stoi(request_count_string);
    for (int i = 0; i < request_count; ++i){

        std::string text;
        std::getline(is_, text);
        std::string_view text_sv = text;
        std::string_view query_type = text_sv.substr(0, text.find(' '));
        text_sv.remove_prefix(text.find(' ') + 1);
        
        if (query_type == "Stop"){
            ReadStop(text_sv, stops,distance_stops);
        }else if (query_type == "Bus"){
            ReadBus(text_sv, stops,  buses);
            }
    }
    for (const auto stop : stops)
    {tc.AddStop(stop.second.name, stop.second.coords);}
    for (const auto bus : buses)
    {tc.AddBus(bus.second.first, bus.second.second);}
    for(const auto& [name, distance_to, distance] : distance_stops)
    {tc.SetDistance(name,distance_to, distance);}
}

void IReader::ReadBus(std::string_view text_sv, std::unordered_map<std::string, Stop>& stops, 
std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>& buses){

    text_sv = text_sv.substr(text_sv.find_first_not_of(' ')); // Bus !750: Tolstopaltsevo - Marushkino - Rasskazovka!
    size_t pos = text_sv.find(':');
    std::string_view name = text_sv.substr(0, pos); // Bus !750!: Tolstopaltsevo - Marushkino - Rasskazovka

    text_sv = text_sv.substr(text_sv.find(':') + 1);          // Bus 750:! Tolstopaltsevo - Marushkino - Rasskazovka!
    text_sv = text_sv.substr(text_sv.find_first_not_of(' ')); // Bus 750: !Tolstopaltsevo - Marushkino - Rasskazovka!

    bool circle = false;
    char type;
    if (text_sv.find('-') != text_sv.npos) { type = '-'; circle = true;} 
    else { type = '>';}

    size_t pos_begin = text_sv.find_first_not_of(' ');
    pos = text_sv.find(type);
    std::vector<std::string> bus_stops;
    do{
        std::string stop_name = std::string{text_sv.substr(pos_begin, pos)};
        if(stop_name.find_last_of(' ') == stop_name.size()-1){
            stop_name = stop_name.substr(0, stop_name.find_last_not_of(' ')+1);
        }
                
        text_sv = text_sv.substr(pos + 1);
        text_sv = text_sv.substr(text_sv.find_first_not_of(' '));

        pos_begin = text_sv.find_first_not_of(' ');
        pos = text_sv.find_first_of(type);

        if (!stops.count(stop_name)){
            stops[std::string{stop_name}] = {std::string{stop_name}, {0, 0},{}};
        }

        bus_stops.push_back((stops.at(stop_name)).name);

    } while (pos != text_sv.npos);

    if (text_sv.size() != 0){
        if(text_sv.find_last_of(' ') == text_sv.size()-1)
            { text_sv = text_sv.substr(0, text_sv.find_last_not_of(' ')+1); }
        if (!stops.count(std::string{text_sv}))
            { stops[std::string{text_sv}] = {std::string{text_sv}, {0, 0},{}};}
        bus_stops.push_back((stops.at(std::string{text_sv})).name);
    }

    if (circle){
        int i = bus_stops.size();
        bus_stops.reserve((i * 2) - 1);
        int k = 2;
        for (; i < bus_stops.capacity(); ++i){
            bus_stops.push_back(bus_stops[bus_stops.size() - k]);
            k += 2;
        }
    }
            
    buses[std::string{name}] = {std::string{name}, std::move(bus_stops)};
}

void IReader::ReadStop(std::string_view text_sv, std::unordered_map<std::string, Stop>& stops, 
                std::vector<std::tuple<std::string, std::string, int>>& distance_stops){

    text_sv = text_sv.substr(text_sv.find_first_not_of(' ')); // Stop !Tolstopaltsevo: 55.611087, 37.208290!, 3900m to Marushkino
    auto pos = text_sv.find(':');
    std::string name = std::string{text_sv.substr(0, pos)}; // Stop !Tolstopaltsevo!: 55.611087, 37.208290, 3900m to Marushkino

    text_sv = text_sv.substr(pos + 1);                        // Stop Tolstopaltsevo:! 55.611087, 37.208290!, 3900m to Marushkino
    text_sv = text_sv.substr(text_sv.find_first_not_of(' ')); // Stop Tolstopaltsevo: !55.611087, 37.208290!, 3900m to Marushkino
    pos = text_sv.find(',');
    std::string_view lat = text_sv.substr(0, pos); // Stop Tolstopaltsevo: !55.611087!, 37.208290, 3900m to Marushkino

    text_sv = text_sv.substr(pos + 1);                        // Stop Tolstopaltsevo: 55.611087,! 37.208290!, 3900m to Marushkino
    text_sv = text_sv.substr(text_sv.find_first_not_of(' ')); // Stop Tolstopaltsevo: 55.611087, !37.208290, 3900m to Marushkino!
        
    std::string_view lng = text_sv.substr(0, std::min(text_sv.size(),text_sv.find(',')));
    stops[name] = {name, {std::stod(std::string(lat)), std::stod(std::string(lng))},{}};  

    while(text_sv.find(',') != text_sv.npos){
                
        text_sv = text_sv.substr(text_sv.find(',')+1); //Stop Tolstopaltsevo: 55.611087, 37.20829,! 3900m to Marushkino!
        std::string distance = std::string{text_sv.substr(text_sv.find_first_not_of(' '))}; //Stop Tolstopaltsevo: 55.611087, 37.20829, !3900m to Marushkino!
        distance = distance.substr(0,distance.find('m')); //Stop Tolstopaltsevo: 55.611087, 37.20829, !3900!m to Marushkino

        text_sv = text_sv.substr(text_sv.find("o")+2); //Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to !Marushkino!
        std::string distance_to = std::string{text_sv.substr(0,std::min(text_sv.size(),text_sv.find(',')))};//Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to !Marushkino!

        distance_stops.push_back(std::make_tuple(name, std::move(distance_to), std::stoi(distance)));
    }        
}
}