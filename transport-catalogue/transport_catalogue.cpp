#include "transport_catalogue.h"

namespace TransportCatalogue{

using namespace geo;

void TransportCatalogue::AddStop(std::string name_in, Coordinates coord_in){
    if(stops_by_name_.count(name_in)) return;
    
    stop_list_.push_back({std::move(name_in),std::move(coord_in),{}});
    Stop* stop = &stop_list_.back();
    // stop->vertex_id = vertex_count_++;
    stops_by_name_[stop->name] = stop; 
}

void TransportCatalogue::AddBus(std::string name_in, bool is_round, const std::vector<std::string>& stops_in){
    std::vector<Stop*> stops;  
    for(std::string_view stop : stops_in){
    if(stops_by_name_.count(stop) == 0) { AddStop(std::string{stop}, {0,0});}
        stops.push_back(stops_by_name_.at(stop));
    }
    bus_list_.push_back({name_in, std::move(stops), is_round});
    Bus* bus = &bus_list_.back();
    bus_by_name_[bus->name] = std::move(bus);
    auto bus_tc = bus_by_name_[name_in];
    for(auto stop : bus_tc->route){
        stop->routes.insert(std::string_view{bus_tc->name});
    }
}

const Stop* TransportCatalogue::GetStop(std::string_view stop) const{
		const Stop* res = nullptr;
		if (stops_by_name_.count(stop)) { return static_cast<const Stop*>(stops_by_name_.at(stop)); }
		else { return res; }
}

const Bus* TransportCatalogue::GetBus(std::string_view bus) const{
		const Bus* res = nullptr;
		if (bus_by_name_.count(bus)) { return static_cast<const Bus*>(bus_by_name_.at(bus)); }
		else { return res; }
}

const std::unordered_map<std::string_view, Stop*> TransportCatalogue::RequestStops() const{ 
    return stops_by_name_;
}

const std::unordered_map<std::string_view, Bus*> TransportCatalogue::RequestBuses () const{ 
    return bus_by_name_;
}

std::optional <Utility::BusInfo>  TransportCatalogue::GetBusInfo(std::string_view bus_name) const {
    if(bus_by_name_.count(bus_name) == 0){ return {};}
    std::unordered_set<std::string_view> stops_set;
    Bus* bus = bus_by_name_.at(bus_name);
    for(const auto& stop : bus->route){
        stops_set.insert(stop->name);
    }

    double coord_lenght = 0.0;
    int real_length = 0;
    for(size_t i = 0; i < bus->route.size()-1; ++i){
        Stop* left = bus->route[i];
        Stop* right = bus->route[i+1];
        int distance;
        std::pair<Stop*, Stop*> seq_pair = std::make_pair(left,right);
        if(distances_.count(seq_pair)){
            distance = distances_.at(seq_pair);
        }else{
            std::pair<Stop*, Stop*> rev_pair = std::make_pair(right,left);
            if(distances_.count(rev_pair)){ distance = distances_.at(rev_pair);}
            else{ distance =0;}
        }
        real_length += distance;
        coord_lenght += ComputeDistance(left->coords, right->coords);
    }

    double curvation = real_length / coord_lenght;
    return{{bus->route.size(), stops_set.size(),real_length, curvation}};
}

std::optional <std::unordered_set<std::string_view>> TransportCatalogue::GetStopRoutes(std::string_view stop_in) const {
    if(stops_by_name_.count(stop_in) == 0) { return {};}
    else{ return stops_by_name_.at(stop_in)->routes;}
}

void TransportCatalogue::SetDistance(std::string_view left, std::string_view right, int distance){
    std::pair<Stop*, Stop*> key = std::make_pair(stops_by_name_[left],stops_by_name_[right]);
    distances_[std::move(key)] = distance;
}
int TransportCatalogue::GetDistance(const Stop* lhs, const Stop* rhs) const{
    std::pair<Stop*, Stop*> s_pair = std::make_pair(stops_by_name_.at(lhs->name), stops_by_name_.at(rhs->name));
    std::pair<Stop*, Stop*> r_pair = std::make_pair(stops_by_name_.at(rhs->name), stops_by_name_.at(lhs->name));
	
    if (distances_.count(s_pair)){ return distances_.at(s_pair);}
	return distances_.at(r_pair);
}

size_t TransportCatalogue::GetStopCount() const{ return stop_list_.size();}  
size_t TransportCatalogue::GetBusCount() const{ return bus_list_.size();}

}//namespace TransportCatalogue