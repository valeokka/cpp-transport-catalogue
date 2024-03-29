#include "json_reader.h"

namespace TransportCatalogue{
using namespace std::literals;



void JSONReader::Read(TransportCatalogue& tc, MapRender::MapRenderer& render, Router::TransportRouter& router){
    std::unordered_map<std::string, Stop> stops;
    std::unordered_map<std::string, std::pair<bool, std::vector<std::string>>> buses;
    std::vector<std::tuple<std::string, std::string, int>> distance_stops; 

    if(requests_.count("base_requests")){
        auto base_requests = requests_.at("base_requests").AsArray();
        for (auto request_map : base_requests){
            json::Dict request = request_map.AsDict();
            std::string name = request.at("name").AsString();
            if(request.at("type") == "Bus"){
                buses[name] = std::move(ReadBus(request));
            } else if(request.at("type") == "Stop"){
                stops[name] = ReadStop(request);
                for(const auto [distance_to, distance] : request.at("road_distances").AsDict()){
                    distance_stops.push_back(std::make_tuple(name, std::move(distance_to), distance.AsInt()));
                }
            }
        }
        for (const auto& stop : stops)
        {tc.AddStop(stop.second.name, stop.second.coords);}
        for (const auto& bus : buses)
        {tc.AddBus(bus.first, bus.second.first, bus.second.second);}
        for(const auto& [name, distance_to, distance] : distance_stops)
        {tc.SetDistance(name,distance_to, distance);}
    }
    if(requests_.count("routing_settings")){
        auto routing_requests = requests_.at("routing_settings").AsDict();
        int bus_wait_time = routing_requests.at("bus_wait_time").AsInt();
        int bus_velocity = routing_requests.at("bus_velocity").AsInt();
        router.SetBusWaitTime(bus_wait_time);
        router.SetBusSpeed(bus_velocity);
    }
    if(requests_.count("render_settings")){
        auto render_requests = requests_.at("render_settings").AsDict();
        auto bus_label_offset = 
                render_requests.at("bus_label_offset").AsArray();
        auto stop_label_offset = 
                render_requests.at("stop_label_offset").AsArray();
        std::vector<svg::Color> color_palette;
        for (const auto color_node : render_requests.at("color_palette").AsArray()){
            color_palette.push_back(ProccedColor(color_node));
        }

        MapRender::RenderOptions settings{
            render_requests.at("width").AsDouble(),
            render_requests.at("height").AsDouble(),

            render_requests.at("padding").AsDouble(),

            render_requests.at("line_width").AsDouble(),
            render_requests.at("stop_radius").AsDouble(),

            render_requests.at("bus_label_font_size").AsInt(),
            bus_label_offset[0].AsDouble(),
            bus_label_offset[1].AsDouble(),

            render_requests.at("stop_label_font_size").AsInt(),
            stop_label_offset[0].AsDouble(),
            stop_label_offset[1].AsDouble(),

            ProccedColor(render_requests.at("underlayer_color")),
            render_requests.at("underlayer_width").AsDouble(),

            color_palette
        };
        render.SetSettings(settings);
        render.SetRoutes(tc.RequestBuses());
        render.SetStops(tc.RequestStops());
    }
    if(requests_.count("stat_requests")){
        json::Array results;
        auto stat_requests = requests_.at("stat_requests").AsArray();
        for (auto request_map : stat_requests){
            json::Dict request = request_map.AsDict();
            int id = request.at("id").AsInt();
            if(request.at("type") == "Bus"){
                results.push_back(json::Node(OutBusStat(tc, request.at("name").AsString(), id)));
            } else if(request.at("type") == "Stop"){
                results.push_back(json::Node(OutStopRoutes(tc, request.at("name").AsString(), id)));
            }else if(request.at("type") == "Map"){
                results.push_back(json::Node(OutMapRender(tc, render, id)));
            } else if(request.at("type") == "Route"){
                results.push_back(json::Node(OutRouteInfo(tc, router, request.at("from").AsString(), 
                                                                 request.at("to").AsString(), id)));
            }
       }
    //    json::Node node = ;
       json::Print(json::Document(std::move(std::move(results))), out_);
    }
}

std::string JSONReader::GetSerializationSettings() const {
    std::string result;
    if(requests_.count("serialization_settings")){
        result = requests_.at("serialization_settings").AsDict().at("file").AsString();
    }
    return result;
}

Stop JSONReader::ReadStop(const json::Dict& request){
    Stop stop;
    stop.name = request.at("name").AsString();
    stop.coords.lat = request.at("latitude").AsDouble();
    stop.coords.lng = request.at("longitude").AsDouble();
    return stop;
}

std::pair<bool, std::vector<std::string>> JSONReader::ReadBus(const json::Dict& request){
    std::vector<std::string> bus_stops;
    std::string name = request.at("name").AsString();
    for(const auto stop_name : request.at("stops").AsArray())
    {   bus_stops.push_back(stop_name.AsString());}
    bool is_round = request.at("is_roundtrip").AsBool();
    if(!request.at("is_roundtrip").AsBool()){
        auto i = bus_stops.size();
        bus_stops.reserve((i * 2) - 1);
        int k = 2;
        for (; i < bus_stops.capacity(); ++i){
            bus_stops.push_back(bus_stops[bus_stops.size() - k]);
            k += 2;
        }
        
    }
    return {is_round, std::move(bus_stops)};
}

json::Dict JSONReader::OutBusStat(const TransportCatalogue& tc, std::string_view bus_name, int id){
    auto res_out = tc.GetBusInfo(bus_name);
    json::Node info;
    if(res_out.has_value()){
        Utility::BusInfo res = res_out.value();
        int unique_count = res.unique_count;
        int all_count = res.all_count;
        info=
        json::Builder{}.StartDict()
        .Key("curvature").Value(res.curvation)
        .Key("request_id").Value(id)
        .Key("route_length").Value(res.length)
        .Key("stop_count").Value(all_count)
        .Key("unique_stop_count").Value(unique_count)
        .EndDict().Build();
    }else{
        info=
        json::Builder{}.StartDict()
        .Key("error_message").Value("not found")
        .Key("request_id").Value(id)
        .EndDict().Build();
    }
    return info.AsDict();

}

json::Dict JSONReader::OutStopRoutes(const TransportCatalogue& tc, std::string_view stop_name, int id){
    auto res_opt = tc.GetStopRoutes(stop_name);
    json::Node info;
    if(res_opt.has_value()){
        auto unordered_res = res_opt.value();
        json::Array buses;
        
        if(!unordered_res.size() == 0){
            std::set <std::string_view> res;
            for (auto route : unordered_res){ res.insert(std::move(route)); }
            for (auto route : res){
                buses.push_back(json::Node(std::string{route}));
            }
        }
        info=
        json::Builder{}.StartDict()
        .Key("buses").Value(buses)
        .Key("request_id").Value(id)
        .EndDict().Build();
        
    }else{
        info = 
        json::Builder{}.StartDict()
        .Key("request_id").Value(id)
        .Key("error_message").Value("not found")
        .EndDict().Build();
    }
    return info.AsDict();
}

svg::Color JSONReader::ProccedColor(const json::Node& request){
    svg::Color result;
    if(request.IsArray()){
        auto color_set = request.AsArray();
        int red = color_set[0].AsInt();
        int green = color_set[1].AsInt();
        int blue = color_set[2].AsInt();
        if(color_set.size() == 3){ 
            return svg::Rgb{red, green, blue};}
        else{ 
            double opacity = color_set[3].AsDouble();
            return svg::Rgba{red, green, blue, opacity};}
    }else{ return request.AsString();}
}

json::Dict JSONReader::OutMapRender(const TransportCatalogue& tc, MapRender::MapRenderer& render, int id){
    std::ostringstream render_to;
    render.SetRoutes(tc.RequestBuses());
    render.SetStops(tc.RequestStops());
    render.Render(render_to);

    std::string render_string = render_to.str();
    json::Node result_node =
    json::Builder{}.StartDict()
    .Key("map").Value(render_string)
    .Key("request_id").Value(id)
    .EndDict().Build();

    return result_node.AsDict();
}

json::Dict JSONReader::OutRouteInfo(const TransportCatalogue& tc, Router::TransportRouter& router,
                                            std::string_view from, std::string_view to, int id){

	if (router.GetVertexCount() == 0){// создается только если не был создан ранее, т.к. в таком случае GetVertexCount() == 0
        const std::unordered_map<std::string_view, Bus*>& info_bus = tc.RequestBuses();
        router.CreateGraph(info_bus, tc);
    } 
    auto info = std::move(router.ResultRoute(tc, tc.GetStop(from)->name, tc.GetStop(to)->name));
    json::Node result_node;
    if(!info.has_value()){ 
        result_node = json::Builder{}.StartDict()
        .Key("request_id").Value(id)
        .Key("error_message").Value("not found")
        .EndDict().Build();
    }else{
        std::vector<json::Node> items;

        json::Builder builder;
        builder.StartDict()
        .Key("request_id").Value(id)
        .Key("total_time").Value(info->total_time)
        .Key("items").StartArray();

        for (const auto& item : info->route) {
            builder.StartDict()
            .Key("stop_name"s).Value(item.stop->name)
			.Key("time"s).Value(item.wait_time)
			.Key("type"s).Value("Wait"s).EndDict()
			.StartDict().Key("bus"s).Value(item.bus->name)
			.Key("span_count"s).Value(item.count_stops)
			.Key("time"s).Value(item.run_time)
			.Key("type"s).Value("Bus"s).EndDict();
		}
        
        result_node = builder.EndArray().EndDict().Build();
    }
    return result_node.AsDict();
}
}//namespace TransportCatalogue