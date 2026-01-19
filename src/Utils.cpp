#include "string"
using namespace std;

string sanitizeInput(string c) {
  int n = c.size();
  string res;

  for (int i = 0; i < n; i++) {
    if (i + 1 < n && i + 2 < n && i + 3 < n && c[i] == '<' && c[i + 1] == 'b' &&
        c[i + 2] == 'r' && c[i + 3] == '>') {
      res += '\n';
      i += 3;
    } else {
      res += c[i];
    }
  }

  return res;
}
