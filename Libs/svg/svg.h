#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <ostream>
#include <cinttypes>
#include <optional>
#include <iostream>


namespace Svg {
    struct Point {
        double x, y;
    };

    struct Rgb {
        int red, green, blue;
    };

    struct Rgba {
        int red, green, blue;
        double alpha;
    };

    using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;

    std::ostream& operator <<(std::ostream& out, const Color& col);

    const Color NoneColor = Color();

    class GraphicalObject {
    protected:
        Color fill_color_ = NoneColor;
        Color stroke_color_ = NoneColor;
        double stroke_width_ = 1.0;
        std::optional<std::string> stroke_line_cap_, stroke_line_join_;
    public:
        virtual ~GraphicalObject() = default;
        virtual void Render(std::ostream& out) const = 0;

        void RenderFeatures(std::ostream& out) const;
    };


    template<typename ObjectType>
    class GraphicalObjectSetters : public GraphicalObject {
    public:
        ObjectType& SetFillColor(const Color& col) {
            fill_color_ = col;
            return dynamic_cast<ObjectType&>(*this);
        }
        ObjectType& SetStrokeColor(const Color& col) {
            stroke_color_ = col;
            return dynamic_cast<ObjectType&>(*this);
        }
        ObjectType& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return dynamic_cast<ObjectType&>(*this);
        }
        ObjectType& SetStrokeLineCap(const std::string& line_cap) {
            stroke_line_cap_ = line_cap;
            return dynamic_cast<ObjectType&>(*this);
        }
        ObjectType& SetStrokeLineJoin(const std::string& line_join) {
            stroke_line_join_ = line_join;
            return dynamic_cast<ObjectType&>(*this);
        }
    };

    class Circle : public GraphicalObjectSetters<Circle> {
    private:
        double cx_ = 0.0, cy_ = 0.0;
        double radius_ = 1.0;
    public:
        ~Circle() = default;
        Circle& SetCenter(Point p) {
            cx_ = p.x;
            cy_ = p.y;
            return *this;
        }
        Circle& SetRadius(double new_radius) {
            radius_ = new_radius;
            return *this;
        }

        void Render(std::ostream& out) const override;
    };

    class Polyline : public GraphicalObjectSetters<Polyline> {
    private:
        std::vector<Point> points_;
    public:
        ~Polyline() = default;
        Polyline& AddPoint(Point new_p) {
            points_.push_back(new_p);
            return *this;
        }

        void Render(std::ostream& out) const override;
    };

    class Text : public GraphicalObjectSetters<Text> {
    private:
        double x_ = 0.0, y_ = 0.0;
        double dx_ = 0.0, dy_ = 0.0;
        uint32_t font_size_ = 1;
        std::optional<std::string> font_family_, font_weight_;
        std::string data_;
    public:
        ~Text() = default;

        Text& SetPoint(Point p) {
            x_ = p.x;
            y_ = p.y;
            return *this;
        }

        Text& SetOffset(Point delta) {
            dx_ = delta.x;
            dy_ = delta.y;
            return *this;
        }

        Text& SetFontSize(uint32_t new_font_size) {
            font_size_ = new_font_size;
            return *this;
        }

        Text& SetFontFamily(const std::string &new_font_family) {
            font_family_ = new_font_family;
            return *this;
        }

        Text& SetFontWeight(const std::string& new_font_weight) {
            font_weight_ = new_font_weight;
            return *this;
        }

        Text& SetData(const std::string &new_data) {
            data_ = new_data;
            return *this;
        }

        void Render(std::ostream& out) const override;
    };

    class Rect : public GraphicalObjectSetters<Rect> {
    private:
        double x_, y_;
        double width_, height_;
    public:
        ~Rect() = default;

        Rect& SetPoint(Point p) {
            x_ = p.x;
            y_ = p.y;
            return *this;
        }

        Rect& SetSizes(double width, double height) {
            width_ = width;
            height_ = height;
            return *this;
        }

        void Render(std::ostream& out) const override;
    };

    class Document {
    private:
        std::vector<std::unique_ptr<GraphicalObject>> objects_;
    public:
        template<typename GraphObject>
        void Add(GraphObject object) {
            auto ptr = std::make_unique<GraphObject>(std::move(object));
            objects_.push_back(std::move(ptr));
        }
        void Merge(Document&& other);
        void Render(std::ostream& out) const;
        std::string AsString() const;
    };
}
