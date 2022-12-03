# Advent of Code

https://adventofcode.com/2022

This year we will save Christmas while learning [rust](https://www.rust-lang.org/)

## How to use

Project conventions:
* each day is its own binary named `day{day_number}-{part}` 
and can be run by `cargo run --bin name_of_bin`.
* Input data is read from its file `data/day{day_number}/input.txt`

This project contains a fetcher that can discover and download the test and normal problem inputs.
Usage `cargo run --bin fetcher <day_number> <part>`
It requires the Advent of Code browser session stored in the `AOC_SESSION` ENV variable