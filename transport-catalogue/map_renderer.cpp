    #include "map_renderer.h"

    namespace MapRender{

    using namespace TransportCatalogue;


    void MapRenderer::SetSettings(const RenderOptions& settings){ settings_ = settings;}

    void MapRenderer::SetRoutes(const std::unordered_map<std::string_view, Bus* > &routes){ routes_ = routes;}

    void MapRenderer::SetStops(const std::unordered_map<std::string_view, Stop*> &stops){ stops_ = stops;}

    void MapRenderer::Render(std::ostream &output){
        std::vector<geo::Coordinates> coordinates;
        std::set<const Stop*, LexSort<Stop>> stops_to_render;
        for (const auto& [name, stop]: stops_) {
            if (!stop->routes.empty()) {
                coordinates.emplace_back(stop->coords);
                stops_to_render.emplace(stop);
            }
        }
        SphereProjector projector(coordinates.begin(), coordinates.end(),
                    settings_.width, settings_.height, settings_.padding);                     
        std::set<const Bus*, LexSort<Bus>> routes_to_render;
        for (const auto& route : routes_) {
            if (!route.second->route.empty()) { routes_to_render.emplace(route.second);}
        }
        RenderBusRoutesLinesLayer(projector, routes_to_render);
        RenderRoutesNamesLayer(projector, routes_to_render);
        RenderStopsLayer(projector, stops_to_render);
        RenderStopsNamesLayer(projector, stops_to_render);
        doc_.Render(output);
        doc_.Clear();
    }

        void MapRenderer::RenderBusRoutesLinesLayer(const SphereProjector& projector,
                            std::set<const Bus*, LexSort<Bus>>& routes_to_render){
            int index = 0;
            for (const auto& bus : routes_to_render){
                svg::Polyline line;
                for (const auto &stop : bus->route){ line.AddPoint(projector(stop->coords));}
            
                line.SetStrokeColor(settings_.color_palette[index % settings_.color_palette.size()])
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeWidth(settings_.line_width)
                    .SetFillColor("none");
                ++index;
                doc_.Add(line);
            }
        }

    void MapRenderer::RenderRoutesNamesLayer(const SphereProjector& projector,
                    std::set<const Bus*, LexSort<Bus>>& routes_to_render) {
        using namespace std::literals;
        int index = 0;

        for (const auto& bus : routes_to_render) {
            std::vector<Stop*> stops_for_labeling{bus->route.front()};
            if (!bus->is_round  && bus->route.front() != bus->route[bus->route.size()/2]) {
                stops_for_labeling.emplace_back(bus->route[bus->route.size()/2]);
                //stops_for_labeling.emplace(stops_for_labeling.begin(),bus->route[bus->route.size()/2])
                //std::cerr << bus->route.size();
            }

            for (const auto& stop : stops_for_labeling) {
                svg::Text underlying, text;
                underlying.SetFillColor(settings_.underlayer_color)
                        .SetStrokeColor(settings_.underlayer_color)
                        .SetStrokeWidth(settings_.underlayer_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                        .SetPosition(projector(stop->coords))
                        .SetOffset({settings_.bus_label_offset_1, settings_.bus_label_offset_2})
                        .SetFontSize(settings_.bus_label_font_size)
                        .SetFontFamily("Verdana"s)
                        .SetFontWeight("bold"s)
                        .SetData(bus->name);
                doc_.Add(underlying);

                text.SetFillColor(settings_.color_palette[index % settings_.color_palette.size()])
                        .SetPosition(projector(stop->coords))
                        .SetOffset({settings_.bus_label_offset_1, settings_.bus_label_offset_2})
                        .SetFontSize(settings_.bus_label_font_size)
                        .SetFontFamily("Verdana"s)
                        .SetFontWeight("bold"s)
                        .SetData(bus->name);
                doc_.Add(text);
            }
            index++;
        }
    }

        void MapRenderer::RenderStopsLayer(const SphereProjector& projector,
                std::set<const Stop*, LexSort<Stop>>& stops_to_render){
            using namespace std::literals;
            svg::Circle circle;
            circle.SetRadius(settings_.stop_radius)
                .SetFillColor("white"s);

            for (const auto& stop: stops_to_render) {
                circle.SetCenter(projector(stop->coords));
                doc_.Add(circle);
            }
        }

        void MapRenderer::RenderStopsNamesLayer(const SphereProjector& projector,
                            std::set<const Stop*, LexSort<Stop>>& stops_to_render){
        using namespace std::literals;
            for (const auto& stop: stops_to_render) {

                svg::Text underlying, text;
                underlying.SetFillColor(settings_.underlayer_color)
                        .SetStrokeColor(settings_.underlayer_color)
                        .SetStrokeWidth(settings_.underlayer_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                        .SetPosition(projector(stop->coords))
                        .SetOffset({settings_.stop_label_offset_1, settings_.stop_label_offset_2})
                        .SetFontSize(settings_.stop_label_font_size)
                        .SetFontFamily("Verdana"s)
                        //.SetFontWeight("bold"s)
                        .SetData(stop->name);
                doc_.Add(underlying);

                text.SetFillColor("black"s)
                        .SetPosition(projector(stop->coords))
                        .SetOffset({settings_.stop_label_offset_1, settings_.stop_label_offset_2})
                        .SetFontSize(settings_.stop_label_font_size)
                        .SetFontFamily("Verdana"s)
                        //.SetFontWeight("bold"s)
                        .SetData(stop->name);
                doc_.Add(text);
            }
        }

    }//namespace MapRender