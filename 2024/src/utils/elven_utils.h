#ifndef ELVEN_UTILS_H
#define ELVEN_UTILS_H

#include <set>
#include <deque>

namespace ElvenUtils {
    typedef long X;
    typedef long Y;

    struct Point {
        X x;
        Y y;

        Point(const X x, const Y y) : x(x), y(y) {}
        [[nodiscard]] Y north() const { return y - 1; }
        [[nodiscard]] Y up() const { return north(); }
        [[nodiscard]] Y south() const { return y + 1; }
        [[nodiscard]] Y down() const { return south(); }
        [[nodiscard]] X east() const { return x + 1; }
        [[nodiscard]] X right() const { return east(); }
        [[nodiscard]] X west() const { return x - 1; }
        [[nodiscard]] X left() const { return west(); }
        [[nodiscard]] bool is(const X &x, const Y &y) const {
            return this->x == x && this->y == y;
        }
        [[nodiscard]] bool is_not(const X &x, const Y &y) const {
            return !is(x, y);
        }
        [[nodiscard]] std::size_t manhattan_distance(const Point &point) const {
            return std::max(point.x, x) - std::min(point.x, x) + std::max(point.y, y) - std::min(point.y, y);
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
        Point& operator+=(const Point& other) {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }
        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
        friend std::ostream& operator<<(std::ostream& os, const Point& point);
    };


    class Map {
    public:
        static constexpr char EMPTY = '.';
        static constexpr char BLOCKED = '#';
        std::deque<std::deque<char>> map;

        explicit Map(const std::size_t x_dimension, const std::size_t y_dimension) {
            for (int y = 0; y < y_dimension; ++y) {
                map.emplace_back(x_dimension, EMPTY);
            }
        }

        explicit Map(const std::vector<std::string> &map) {
            std::ranges::transform(
                map.begin(), map.end(),
                std::back_inserter(this->map),
                [](auto &line) {
                    std::deque<char> row;
                    std::copy(
                        line.begin(), line.end(),
                        std::back_inserter(row)
                    );
                    return std::move(row);
                }
            );
        }
        explicit Map(const std::vector<std::vector<char>> &map) {
            std::ranges::transform(
                map.begin(), map.end(),
                std::back_inserter(this->map),
                [](auto &line) {
                    std::deque<char> row;
                    std::copy(
                        line.begin(), line.end(),
                        std::back_inserter(row)
                    );
                    return std::move(row);
                }
            );
        }

        Map(const Map& other) = default;

        [[nodiscard]] Point find(const char element) const {
            for (long i = 0; i < map.size(); ++i) {
                if (auto it = std::ranges::find(map[i].begin(), map[i].end(), element); it != map[i].end()) {
                    return Point{it - map[i].begin(), i};
                }
            }
            throw std::runtime_error("No start posion found");
        }
        [[nodiscard]] std::vector<Point> find_all(const char element) const {
            std::vector<Point> found;
            for (auto y = 0 ; y < y_size(); ++y) {
                for (auto x = 0 ; x < x_size(); ++x) {
                    if (at(x, y) == element) { found.emplace_back(x, y); }
                }
            }
            return found;
        }

        [[nodiscard]] size_t x_size() const { return map.front().size(); }
        [[nodiscard]] size_t y_size() const { return map.size(); }

        [[nodiscard]] bool in_x_boundary(const X &x) const { return x < x_size(); }
        [[nodiscard]] bool in_y_boundary(const Y &y) const { return y < y_size(); }
        [[nodiscard]] bool in_boundary(const Point &point) const { return in_x_boundary(point.x) && in_y_boundary(point.y); }

        [[nodiscard]] char at(const X &x, const Y &y) const {  return map[y][x]; }
        [[nodiscard]] char at(const Point& point) const { return at(point.x, point.y);  }
        char operator[](const Point& point) const { return at(point.x, point.y);  }

        void set(const X &x, const Y &y, const char &value) { map[y][x] = value; }
        void set(const Point& point, const char &value) { set(point.x, point.y, value); }

        void expand_up() {
            map.emplace_front(x_size(), EMPTY);
        }
        void expand_down() {
            map.emplace_back(x_size(), EMPTY);
        }
        void expand_right() {
            for (auto& row : map) {
                row.emplace_back(EMPTY);
            }
        }
        void expand_left() {
            for (auto& row : map) {
                row.emplace_front(EMPTY);
            }
        }

        void paint_exterior(const char paint) {
            expand_up();
            expand_down();
            expand_right();
            expand_left();
            std::set<Point> visited_cells;
            std::deque<Point> explore_cells;
            explore_cells.emplace_back(0, 0);
            set(0, 0, paint);
            while (!explore_cells.empty()) {
                const auto point = explore_cells.front();
                explore_cells.pop_front();
                if (const auto north = Point(point.x, point.north());
                    in_y_boundary(north.y) && at(north) == EMPTY && !visited_cells.contains(north)
                ) {
                    explore_cells.push_back(north);
                    set(point.x, point.north(), paint);
                    visited_cells.insert(north);
                }
                if (const auto south = Point(point.x, point.south());
                    in_y_boundary(south.y) && at(south) == EMPTY && !visited_cells.contains(south)
                ) {
                    explore_cells.push_back(south);
                    set(point.x, point.south(), paint);
                    visited_cells.insert(south);
                }
                if (const auto east = Point(point.east(), point.y);
                    in_x_boundary(east.x) && at(east) == EMPTY && !visited_cells.contains(east)
                ) {
                    explore_cells.push_back(east);
                    set(point.east(), point.y, paint);
                    visited_cells.insert(east);
                }
                if (const auto west = Point(point.west(), point.y);
                    in_x_boundary(west.x) && at(west) == EMPTY && !visited_cells.contains(west)
                ) {
                    explore_cells.push_back(west);
                    set(point.west(), point.y, paint);
                    visited_cells.insert(west);
                }
            }
        }

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