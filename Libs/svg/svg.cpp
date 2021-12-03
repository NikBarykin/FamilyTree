#include "svg.h"

#include <sstream>

using namespace std;


namespace Svg {
    ostream& operator <<(ostream& out, const Color& col) {
        if (holds_alternative<Rgb>(col)) {
            Rgb rgb = get<Rgb>(col);
            out << "rgb(" << rgb.red << "," << rgb.green << "," << rgb.blue << ")";
        }
        else if (holds_alternative<Rgba>(col)) {
            Rgba rgba = get<Rgba>(col);
            out << "rgba(" << rgba.red << "," 
                           << rgba.green << "," 
                           << rgba.blue << ","
                           << rgba.alpha << ")";
        }
        else if (holds_alternative<string>(col)) {
            out << get<string>(col);
        }
        else if (holds_alternative<monostate>(col)) {
            out << "none";
        }
        else {
            throw runtime_error("o_0");
        }
        return out;
    }

    void GraphicalObject::RenderFeatures(ostream& out) const {
        out << "fill=\"" << fill_color_ << "\" ";
        out << "stroke=\"" << stroke_color_ << "\" ";
        out << "stroke-width=\"" << stroke_width_ << "\" ";
        if (stroke_line_cap_)
            out << "stroke-linecap=\"" << *stroke_line_cap_ << "\" ";
        if (stroke_line_join_)
            out << " " << "stroke-linejoin=\"" << *stroke_line_join_ << "\" ";
    }

    void Circle::Render(ostream& out) const {
        out << "<circle ";
        RenderFeatures(out);
        out << "cx=\"" << cx_ << "\" ";
        out << " cy=\"" << cy_ << "\" ";
        out << " r=\"" << radius_ << "\" ";
        out << "/>";
    }

    void Polyline::Render(ostream& out) const {
        out << "<polyline ";
        RenderFeatures(out);
        out << "points=\"";
        for (const Point& p : points_) {
            out << p.x << "," << p.y << " ";
        }
        out << "\" ";
        out << "/>";
    }

    void Text::Render(ostream& out) const {
        out << "<text ";
        RenderFeatures(out);
        out << "x=\"" << x_ << "\" ";
        out << "y=\"" << y_ << "\" ";
        out << "dx=\"" << dx_ << "\" ";
        out << "dy=\"" << dy_ << "\" ";
        out << "font-size=\"" << font_size_ << "\" ";
        if (font_family_)
            out << "font-family=\"" << *font_family_ << "\" ";
        if (font_weight_)
            out << "font-weight=\"" << *font_weight_ << "\" ";
        out << ">";
        out << data_;
        out << "</text>";
    }

    void Rect::Render(ostream &out) const {
        out << "<rect ";
        RenderFeatures(out);
        out << "x=\"" << x_ << "\" ";
        out << "y=\"" << y_ << "\" ";
        out << "width=\"" << width_ << "\" ";
        out << "height=\"" << height_ << "\" ";
        out << "/>";
    }

    void Document::Merge(Document&& other) {
        move(begin(other.objects_), end(other.objects_), back_inserter(objects_));
    }

    void Document::Render(ostream& out) const {
        out << R"(<?xml version="1.0" encoding="UTF-8" ?>)";
        out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)";
        for (const auto& ptr : objects_) {
            ptr->Render(out);
        }
        out << R"(</svg>)";
    }

    string Document::AsString() const {
        stringstream ss;
        Render(ss);
        return ss.str();
    }
}
