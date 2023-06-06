#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- ostream ------------------

std::ostream& operator<<(std::ostream& os, const svg::StrokeLineCap& linecap){
    switch (linecap) {
        case StrokeLineCap::BUTT:
            os << "butt";
            break;
        case StrokeLineCap::ROUND:
            os << "round";
            break;
        case StrokeLineCap::SQUARE:
            os << "square";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const svg::StrokeLineJoin& linejoin){
    switch (linejoin) {
        case StrokeLineJoin::ARCS:
            os << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            os << "bevel";
            break;
        case StrokeLineJoin::MITER:
            os << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            os << "miter-clip";
            break; 
        case StrokeLineJoin::ROUND:
            os << "round";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, svg::Color color){
    std::visit(ColorOut{os}, color);
    return os;
}

// ---------- Circle ------------------
Circle::Circle(Point center, double radius) : center_(center), radius_(radius)
{}

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Polyline ----------------

Polyline& Polyline::AddPoint(Point point){
    points_.push_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    auto size = points_.size();
    for(size_t i = 0; i < points_.size(); ++i){
        out << points_[i].x << "," << points_[i].y;
        if(--size){
            out << " "sv;
        }
    }
    out << "\""sv;
    RenderAttrs(out);
    out << " />"sv;
}

//   <polyline fill="none" stroke="blue" stroke-width="10" 
//             points="50,375
//                     150,375 150,325 250,325 250,375
//                     350,375 350,250 450,250 450,375
//                     550,375 550,175 650,175 650,375
//                     750,375 750,100 850,100 850,375
//                     950,375 950,25 1050,25 1050,375
//                     1150,375" />
// ---------- Text --------------------

Text::Text(Point pos, Point offset, uint32_t size, std::string font_family, 
                                std::string font_weight, std::string data):
    pos_(pos), offset_(offset), size_(size), font_family_(font_family), 
                    font_weight_(font_weight), data_(ProcessText(data))
    {}

Text& Text::SetPosition(Point pos){
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset){
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size){
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family){
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data){
    data_ = ProcessText(data);
    return *this;
}

std::string Text::ProcessText(std::string_view text){
    std::string result;

    for(const char& c : text){
        switch (c) {
            case '&':
                result += "&amp;";
                break;
            case '<':
                result += "&lt;";
                break;
            case '>':
                result += "&gt;";
                break;
            case '"':
                result += "&quot;";
                break;
            case '\'':
                result += "&apos;";
                break;
            default:
                result += c;
                break;
        }

    }

    return result;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out <<"<text"sv;
    RenderAttrs(out);
    out << " x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\""sv; 
    out <<" dx=\""sv << offset_.x <<"\" dy=\""sv << offset_.y << "\""sv;
    out <<" font-size=\""sv << size_ << "\""sv;
    if(!font_family_.empty())
    out <<" font-family=\""sv << font_family_  << "\""sv;
    if(!font_weight_.empty())
    out <<" font-weight=\""sv << font_weight_ << "\""sv;

    out <<">"sv << data_;
    out <<"</text>"sv;

}
//<text x="20" y="35" dx="1" dy="1" font-size="1" font-family="1" font-weight="1">data</text>

// ---------- Document ----------------

void Document::AddPtr(std::unique_ptr<Object>&& obj){
    objects_.push_back(std::move(obj));
}

void Document::Clear(){
    objects_.clear();
}

void Document::Render(std::ostream& out) const{
    RenderContext context(out);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
    for(const std::unique_ptr<Object>& obj : objects_){
        obj->Render(context);
    }
    out << "</svg>"sv;
}

// <?xml version="1.0" encoding="UTF-8" ?>
// <svg xmlns="http://www.w3.org/2000/svg" version="1.1">
//   <circle cx="20" cy="20" r="10" />
//   <text x="35" y="20" dx="0" dy="6" font-size="12" font-family="Verdana" font-weight="bold">Hello C++</text>
//   <polyline points="20,40 22.9389,45.9549 29.5106,46.9098 24.7553,51.5451 25.8779,58.0902 20,55 14.1221,58.0902 15.2447,51.5451 10.4894,46.9098 17.0611,45.9549 20,40" />
// </svg> 

namespace shapes{

// ---------- Triangle ----------------

void shapes::Triangle::Draw(svg::ObjectContainer& container) const  {
    container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
}

// ---------- Star --------------------

Star::Star(svg::Point center, double outer_rad, double inner_rad, int num_rays) :
polyline_(CreateStar(center,outer_rad,inner_rad,num_rays))
{}

void Star::Draw(svg::ObjectContainer& container) const{  
    container.Add(polyline_);
}

svg::Polyline Star::CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    Polyline polyline;
    polyline.SetFillColor("red").SetStrokeColor("black");
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}

// ---------- Snowman -----------------
    
Snowman::Snowman(Point pos, double rad) :
pos_(pos), rad_(rad) {}

void Snowman::Draw(svg::ObjectContainer& container) const{

    Point pos_down = pos_;
    pos_down.y+= rad_*5;

    Point pos_middle = pos_;
    pos_middle.y += rad_*2;

    Circle down(pos_down, rad_*2);
    down.SetFillColor("rgb(240,240,240)").SetStrokeColor("black");
    Circle middle(pos_middle, rad_*1.5);
    middle.SetFillColor("rgb(240,240,240)").SetStrokeColor("black");
    Circle up(pos_, rad_);
    up.SetFillColor("rgb(240,240,240)").SetStrokeColor("black");
    
    container.Add(down);
    container.Add(middle);
    container.Add(up);
}



}  // namespace shapes
}  // namespace svg