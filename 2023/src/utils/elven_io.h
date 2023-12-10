#ifndef ELVEN_IO_H
#define ELVEN_IO_H

#include <iostream>
#include <fstream>
#include <string>

namespace ElvenIO {

    typedef std::vector<std::string> input_type;

    input_type inline read(const char *filename) {
        std::fstream file(filename);
        input_type lines;

        std::copy(
            std::istream_iterator<std::string>(file),
            std::istream_iterator<std::string>(),
            std::back_inserter(lines)
        );

        return std::move(lines);
    }
}
#endif //ELVEN_IO_H