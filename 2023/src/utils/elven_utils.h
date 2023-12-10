#ifndef ELVEN_UTILS_H
#define ELVEN_UTILS_H

namespace ElvenUtils {
    typedef size_t X;
    typedef size_t Y;

    struct Point {
        X x;
        Y y;

        Point(const X x, const Y y) : x(x), y(y) {}
        [[nodiscard]] Y north() const { return y - 1; }
        [[nodiscard]] Y south() const { return y + 1; }
        [[nodiscard]] X east() const { return x + 1; }
        [[nodiscard]] X west() const { return x - 1; }
        [[nodiscard]] bool is(const X &x, const Y &y) const {
            return this->x == x && this->y == y;
        }
        [[nodiscard]] bool is_not(const X &x, const Y &y) const {
            return !is(x, y);
        }


        bool operator<(const Point& other) const {
            return x < other.x || (x == other.x && y < other.y) ;
        }
        bool operator==(const Point& other) const {
            return std::tie(x, y) == std::tie(other.x, other.y);
        }
        bool operator!=(const Point& other) const {
            return !(*this == other);
        }
        friend std::ostream& operator<<(std::ostream& os, const Point& point);
    };


    class Map {
    public:
        std::vector<std::vector<char>> map;

        explicit Map(const std::vector<std::string> &map) {
            this->map.reserve(map.size());
            std::ranges::transform(
                map.begin(), map.end(),
                std::back_inserter(this->map),
                [](auto &line) {
                    std::vector<char> row;
                    row.reserve(line.length());
                    std::copy(
                        line.begin(), line.end(),
                        std::back_inserter(row)
                    );
                    return std::move(row);
                }
            );
        }
        explicit Map(const std::vector<std::vector<char>> &map) : map(map) {}
        Map(const Map& other) = default;

        [[nodiscard]] Point find(const char element) const {
            for (size_t i = 0; i < map.size(); ++i) {
                if (auto it = std::ranges::find(map[i].begin(), map[i].end(), element); it != map[i].end()) {
                    return Point{static_cast<X>(it - map[i].begin()), i};
                }
            }
            throw std::runtime_error("No start posion found");
        }

        [[nodiscard]] size_t x_size() const { return map.front().size(); }
        [[nodiscard]] size_t y_size() const { return map.size(); }

        [[nodiscard]] size_t in_x_boundary(const X &x) const { return x < x_size(); }
        [[nodiscard]] size_t in_y_boundary(const Y &y) const { return y < y_size(); }

        [[nodiscard]] char at(const X &x, const Y &y) const {  return map[y][x]; }
        [[nodiscard]] char at(const Point& point) const { return at(point.x, point.y);  }
        char operator[](const Point& point) const { return at(point.x, point.y);  }

        void set(const X &x, const Y &y, const char &value) { map[y][x] = value; }
        void set(const Point& point, const char &value) { set(point.x, point.y, value); }

        void print() const {
            std::cout << *this << std::endl;
        }

        friend std::ostream& operator<<(std::ostream& os, const Map& map);
    };

    inline std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "(" << point.x << "," << point.y << ")";
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Map& map) {
        for (const auto &row: map.map) {
            for (const auto &cell: row) { os << cell; }
            os << std::endl;
        }
        return os;
    }
}

#endif //ELVEN_UTILS_H