use std::env;
use std::fs::File;
use std::io::{Read, Write};
use std::error::Error;
use reqwest::blocking::Client;

struct Session {
    value: String
}

fn get_input(session: &Session, day: &str) -> Result<(), Box<dyn Error>> {
    let body = get_body(session, format!("https://adventofcode.com/2022/day/{}/input", day))?;

    let mut file = File::create(format!("data/day{}/input.txt", day))?;
    file.write_all(body.as_bytes())?;

    Ok(())
}

fn discover_test_data(session: &Session, day: &str, part: &str) -> Result<(), Box<dyn Error>> {
    let html_body = get_body(session, format!("https://adventofcode.com/2022/day/{}", day))?;
    if part == "1" {
        let test_input = discover_test_input(&html_body)?;
        let mut file = File::create(format!("data/day{}/test_input.txt", day))?;
        file.write_all(test_input.as_bytes())?;
    }
    let test_result = discover_test_result(&html_body)?;
    let mut file = File::create(format!("data/day{}/part{}/test_result.txt", day, part))?;
    file.write_all(test_result.as_bytes())?;
    Ok(())
}

fn discover_test_input(html_body: &str) -> Result<String, Box<dyn Error>> {
    let test_input_candidate = find_last_element(html_body, "pre","<code>")?;
    let test_input = remove_tag(&test_input_candidate, "code")?;
    Ok(test_input)
}

fn discover_test_result(html_body: &str) -> Result<String, Box<dyn Error>> {
    let test_result_candidate = find_last_element(html_body, "code","<em>")?;
    let test_result = remove_tag(&test_result_candidate, "em")?;
    Ok(test_result)
}

fn find_last_element(html_body: &str, tag: &str, filter: &str) -> Result<String, Box<dyn Error>> {
    let document = scraper::Html::parse_document(&html_body);
    let code_selector = scraper::Selector::parse(tag).unwrap();
    Ok(
        document.select(&code_selector)
        .filter(|x| x.inner_html().contains(filter))
        .last()
        .expect("html parse error")
        .inner_html()
    )
}

fn remove_tag(html_body: &str, tag: &str) -> Result<String, Box<dyn Error>> {
    let document = scraper::Html::parse_document(&html_body);
    let code_selector = scraper::Selector::parse(tag).unwrap();
    Ok(
        document.select(&code_selector)
        .last()
        .expect("html parse error")
        .inner_html()
    )
}

fn get_body(session: &Session, url: String) -> Result<String, Box<dyn Error>> {
    let mut res = Client::new()
        .get(url)
        .header("Cookie", format!("session={}", session.value))
        .send()?;
    let mut body = String::new();
    res.read_to_string(&mut body)?;
    Ok(body)
}

fn main() -> Result<(), Box<dyn Error>> {
    let session = Session { value: env::var("AOC_SESSION")? };
    let day = std::env::args().nth(1).expect("no day given");
    let part = std::env::args().nth(2).expect("no part given");
    get_input(&session, &day)?;
    discover_test_data(&session, &day, &part)?;
    Ok(())
}