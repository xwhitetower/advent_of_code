use std::collections::HashMap;
use std::error::Error;

use advent_of_code::read_lines;

struct FileNode {
    path: String,
    size: u32
}

static SEP: &str = "/";

// dirty but I do not have time to fight rust to create a proper tree that can navigate to the parent.
fn solve(lines: Vec<String>) -> u32 {
    let files = files(&lines);
    count_size(files)
}

fn count_size(files: Vec<FileNode>) -> u32 {
    let mut prefix_sizes: HashMap<String, u32> = HashMap::new();
    for file in files {
        let folders: Vec<&str> = file.path.split(SEP).collect();
        for i in 1..folders.len() {
            prefix_sizes.entry(folders[1..i].join(SEP))
                .and_modify(|v| *v += file.size)
                .or_insert(file.size);
        }
    }
    prefix_sizes.iter()
        .filter(|x| x.1 <= &100_000 )
        .map(|x| x.1)
        .sum()
}

fn files(lines: &Vec<String>) -> Vec<FileNode> {
    let mut files = Vec::new();
    let mut path = Vec::new();
    for line in lines {
        let mut words = line.split(' ');
        if line.starts_with("$ cd") {
            let dir_name = words.last().unwrap();
            if dir_name == ".." {
                path.pop();
            } else {
                path.push(dir_name);
            }
        } else if !line.starts_with("$") && !line.starts_with("dir"){
            let size = words.next().unwrap().parse::<u32>().unwrap();
            let file_name = words.last().unwrap();
            files.push(FileNode {
                path: (path.join(SEP) + SEP + file_name)[1..].to_string(),
                size
            })
        }
    }
    files
}

fn main() -> Result<(), Box<dyn Error>> {
    println!("{}", solve(read_lines("./data/day7/input.txt")?));
    Ok(())
}

#[test]
fn test() {
    let lines = read_lines("./data/day7/test_input.txt").unwrap();
    let expected_lines = read_lines("./data/day7/part1/test_result.txt").unwrap();
    let expect = expected_lines.first().unwrap().parse::<u32>().unwrap();
    assert_eq!(solve(lines), expect);
}
