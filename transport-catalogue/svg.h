#pragma once

#define _USE_MATH_DEFINES 
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <optional>
#include <variant>
namespace svg {

struct Rgb{
    	Rgb(int  red, int  green, int  blue)
			: red(red), green(green), blue(blue) {}
            
    Rgb() = default;
    int  red = 0;
    int  green = 0;
    int  blue = 0;
};

struct Rgba{
    Rgba() = default;
    		Rgba(int  red, int  green, int  blue, double opacity)
			: red(red), green(green), blue(blue), opacity(opacity) {}
    int  red = 0;
    int  green = 0;
    int  blue = 0;
    double opacity  = 1.0;
};

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;
inline const Color NoneColor{ "none" };
struct ColorOut{
 
    std::ostream& out;
    void operator()(std::monostate){
        using namespace std::literals;
        out << "none"sv;
    }
    void operator()(std::string text){
        using namespace std::literals;
        out << text;
    }
    void operator()(svg::Rgb rgb){
        using namespace std::literals;
        out << "rgb("sv << static_cast<int>(rgb.red) << ","sv << static_cast<int>(rgb.green) 
                                            << ","sv << static_cast<int>(rgb.blue) << ")"sv;
    }
    void operator()(svg::Rgba rgba){
        using namespace std::literals;
        out << "rgba("sv << static_cast<int>(rgba.red) << ","sv << static_cast<int>(rgba.green) 
                    << ","sv << static_cast<int>(rgba.blue) << ","sv << rgba.opacity << ")"sv;
    }

};

std::ostream& operator<<(std::ostream& os, svg::Color color);

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& os, const svg::StrokeLineCap& linecap);

std::ostream& operator<<(std::ostream& os, const svg::StrokeLineJoin& linejoin);

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

template <typename Owner>
class PathProps {
public:

    Owner& SetFillColor(Color color){ fill_color_ = color; return AsOwner();}
    Owner& SetStrokeColor(Color color){ stroke_color_ = color; return AsOwner();}
    Owner& SetStrokeWidth(double width){ width_ = width; return AsOwner();}
    Owner& SetStrokeLineCap(StrokeLineCap line_cap){ line_cap_ = line_cap; return AsOwner();}
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join){ line_join_ = line_join; return AsOwner();}

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_.has_value()) 
        { out << " fill=\""sv << *fill_color_ << "\""sv;}
        if (stroke_color_.has_value()) 
        { out << " stroke=\""sv << *stroke_color_ << "\""sv;}
        if(width_.has_value())
        { out << " stroke-width=\""sv << *width_ << "\""sv;}
        if(line_cap_.has_value())
        { out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;}
        if(line_join_.has_value())
        { out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;}
    }

private:
   Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;

};


/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle() = default;
    Circle(Point center, double radius);
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline>{
public:
    Polyline() = default;
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);


private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    Text() = default;
    Text(Point pos, Point offset, uint32_t size, std::string font_family, 
                                std::string font_weight, std::string data);
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    void RenderObject(const RenderContext& context) const override;
    std::string ProcessText(std::string_view text);

    Point pos_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_ = 1;
    std::string font_family_ = "";
    std::string font_weight_ = "";
    std::string data_ = "";
};

class ObjectContainer{
public:

    template <typename Obj>
    void Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    } 
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
protected:
    ~ObjectContainer() = default;
    std::vector<std::unique_ptr<Object>> objects_;
};

class Drawable{
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};


class Document : public ObjectContainer {
public:

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    void Clear();

};

namespace shapes {

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override;

private:
    svg::Point p1_, p2_, p3_;
};

class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays);
    void Draw(svg::ObjectContainer& container) const override;

private:
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);

    Polyline polyline_;
};

class Snowman : public svg::Drawable {
public:
    Snowman(Point pos, double rad);
    void Draw(svg::ObjectContainer& container) const override;

private:
    Point pos_;
    double rad_;
};

} // namespace shapes


}  // namespace svg