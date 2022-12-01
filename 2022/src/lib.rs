use std::fs::File;
use std::error::Error;
use std::io::{self, BufRead};

pub trait Challenge {
    fn solve(lines: Vec<String>) -> String;
}

pub fn read_lines(file_path: &str) -> Result<Vec<String>, Box<dyn Error>> {
    let file = File::open(file_path)?;
    let lines = io::BufReader::new(file).lines().collect::<Result<_, _>>()?;
    Ok(lines)
}