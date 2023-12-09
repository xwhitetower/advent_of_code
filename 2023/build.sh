export CC=clang-17
export CXX=clang++-17

cmake -Wno-dev -B"build" \
-DCMAKE_BUILD_TYPE=Release \
.

cmake --build "build" --target install