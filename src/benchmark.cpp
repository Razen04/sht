// Only for benchmarking the speed per item and not the actual entry point.
// Ignore this file. Create by Gemini.

#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/Renderer.hpp"
#include "../include/Utils.hpp"
#include "../include/json.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::chrono;
using json = nlohmann::json;

// Copy of your processing logic
string processHtml(const string &raw) {
  if (raw.empty())
    return "";
  string sanitizedRaw = sanitizeInput(raw);
  Lexer lexer(sanitizedRaw);
  vector<Token> tokens;
  Token token = lexer.getNextToken();
  while (token.type != TokenType::EOF_TOKEN) {
    tokens.push_back(token);
    token = lexer.getNextToken();
  }
  Parser p(tokens);
  auto root = p.parse();
  Renderer r;
  return r.render(*root);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: ./benchmark <input_file.json>" << endl;
    return 1;
  }

  // 1. PRE-LOAD DATA (Not Timed)
  ifstream inputFile(argv[1]);
  json j;
  inputFile >> j;
  inputFile.close();

  vector<string> questions;
  for (const auto &item : j) {
    if (item.contains("question") && item["question"].is_string()) {
      questions.push_back(item["question"]);
    }
  }

  cout << "Benchmarking " << questions.size() << " items..." << endl;

  // 2. START TIMER
  auto start = high_resolution_clock::now();

  // 3. RUN PROCESSING
  volatile size_t totalBytes = 0; // Prevent compiler optimization
  for (const auto &q : questions) {
    string res = processHtml(q);
    totalBytes += res.size();
  }

  // 4. STOP TIMER
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  cout << "Total CPU Time: " << duration.count() / 1000.0 << " ms" << endl;
  cout << "Average per item: " << (duration.count() / 1000.0) / questions.size()
       << " ms" << endl;

  return 0;
}
