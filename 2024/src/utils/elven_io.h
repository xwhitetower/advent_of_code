#ifndef ELVEN_IO_H
#define ELVEN_IO_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace ElvenIO {

    typedef std::vector<std::string> input_type;

    input_type inline read(const char *filename) {
        std::fstream file(filename);

        std::string line;
        input_type lines;
        while (std::getline(file, line)) {
            lines.emplace_back(line);
        }

        return std::move(lines);
    }
}
#endif //ELVEN_IO_H