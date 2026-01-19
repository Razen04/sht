#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/Renderer.hpp"
#include "../include/Utils.hpp"
#include "../include/json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

using json = nlohmann::json;

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

int main(int args, char *argv[]) {
  if (args < 3) {
    cerr << "Usage: " << argv[0] << "<input_file.json> <output_file.json>"
         << endl;
    return 1;
  }

  string inputPath = argv[1];
  string outputPath = argv[2];

  // load the json
  ifstream inputFile(inputPath);

  json j;
  try {
    inputFile >> j;
  } catch (json::parse_error &e) {
    cerr << "JSON Parse Error" << e.what() << endl;
    return 1;
  }
  inputFile.close();

  cout << "Loaded JSON. Processing " << j.size() << " items..." << endl;

  int count = 0;
  for (auto &item : j) {
    count++;

    string currentId = "Unknown";
    if (item.contains("id"))
      currentId = item["id"];

    try {
      if (item.contains("question") && item["question"].is_string()) {
        string raw = item["question"];
        item["question"] = processHtml(raw);
      }

      if (item.contains("options") && item["options"].is_array()) {
        for (auto &option : item["options"]) {
          if (option.is_string()) {
            string raw = option;
            option = processHtml(raw);
          }
        }
      }

      if (count % 100 == 0)
        cout << "Processed " << count << " items..." << endl;
    } catch (const std::exception &e) {
      cerr << "[ERROR] Skipped Item " << count << " (ID: " << currentId
           << "): " << e.what() << endl;
    } catch (...) {
      cerr << "[CRITICAL] Unknown crash at item " << count
           << " (ID: " << currentId << "): " << endl;
    }
  }

  ofstream outputFile(outputPath);
  if (!outputFile.is_open()) {
    cerr << "Count not create output file" << endl;
    return 1;
  }

  outputFile << j.dump(4);
  outputFile.close();

  cout << "Done." << endl;

  return 0;
}
