#include "domain.h"
namespace TransportCatalogue{

//-------Stop------

bool Stop::operator<(Stop other){ return name < other.name;}
bool Stop::operator==(Stop other){ return name == other.name;}

//-------Bus------

bool Bus::operator<(Bus other){ return name < other.name; }
bool Bus::operator==(Bus other){ return name == other.name; }

//-------Utility------

std::size_t Utility::StopDistanceHasher::operator()(const std::pair<const Stop*, const Stop*>& stops) const {
	int hash_one = stops.first->coords.lat * 37 * 37 * 37 + stops.first->coords.lng * 37 * 37 + stops.first->routes.size() * 37;
    int hash_two = stops.second->coords.lat * 12 * 12 * stops.second->coords.lng * 12 * 12;
    return hash_one * 1000 + hash_two;
}

}//namespace TransportCatalogue