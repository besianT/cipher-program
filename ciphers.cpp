#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Function declarations go at the top of the file so we can call them
// anywhere in our program, such as in main or in other functions.
// Most other function declarations are in the included header
// files.

vector<char> hillClimb(const QuadgramScorer& scorer, const string& ciphertext) {
  vector<char> guess = genRandomSubstCipher();
  vector<char> newkey;
  newkey = guess;
  int count = 0;
  double oldscore = 0;
  double newscore = 0;
  string test;
  string cleanedciphertext = clean(ciphertext);
  test = cleanedciphertext;
  oldscore = scoreString(scorer, cleanedciphertext);
  int rand1;
  int rand2;
  while (count < 1000) {
    test = cleanedciphertext;
    rand1 = Random::randInt(25);
    rand2 = Random::randInt(25);

    while (rand1 == rand2) {
      rand2 = Random::randInt(25);
    }
    swap(guess.at(rand1), guess.at(rand2));
    test = applySubstCipher(guess, test);

    newscore = scoreString(scorer, test);

    if (newscore > oldscore) {
      oldscore = newscore;
      count = 0;
      newkey = guess;

    } else {
      count++;
      guess = newkey;
    }
  }
  return newkey;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  double newscore;
  vector<char> cipher;
  vector<char> goodcipher;
  string cleaned = clean(ciphertext);
  string test = cleaned;
  double oldscore = scoreString(scorer, cleaned);
  for (int i = 0; i < 26; ++i) {
    test = cleaned;
    cipher = hillClimb(scorer, test);

    test = applySubstCipher(cipher, test);

    newscore = scoreString(scorer, test);

    if (newscore > oldscore) {
      oldscore = newscore;
      goodcipher = cipher;
    }
  }

  return goodcipher;
}

// When you add a new helper function, make sure to declare it up here!

/**
 * Print instructions for using the program.
 */
void printMenu();

int main() {
  Random::seed(time(NULL));
  string command;
  string word;
  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;
  ifstream f;
  f.open("dictionary.txt");
  vector<string> dict;
  while (f >> word) {
    dict.push_back(word);
  }

  ifstream quad("english_quadgrams.txt");
  vector<string> quadgrams;
  vector<int> occurances;
  string line;
  string number;

  while (getline(quad, line)) {
    quadgrams.push_back(line.substr(0, 4));
    number = line.substr(5);
    occurances.push_back(stoi(number));
  }
  QuadgramScorer scorer(quadgrams, occurances);
  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }
    if (command == "C" || command == "c") {
      caesarEncryptCommand();
    }
    if (command == "d" || command == "D") {
      caesarDecryptCommand(dict);
    }
    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }
    if (command == "e" || command == "E") {
      computeEnglishnessCommand(scorer);
    }
    if (command == "s" || command == "S") {
      decryptSubstCipherCommand(scorer);
    }
    if (command == "f" || command == "F") {
      ifstream inputfile;
      ofstream outputfile;
      string inputfilename;
      string outputfilename;
      cin >> inputfilename;
      cin >> outputfilename;
      inputfile.open(inputfilename);
      string in;
      string todecrypt;
      while (getline(inputfile, in)) {
        todecrypt += in;
        todecrypt += '\n';
      }
      vector<char> ciph;

      ciph = decryptSubstCipher(scorer, todecrypt);
      outputfile.open(outputfilename);

      todecrypt = applySubstCipher(ciph, todecrypt);

      outputfile << todecrypt;
    }
    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  int position = ALPHABET.find(c);
  position += amount;
  position = position % 26;

  return ALPHABET.at(position);
}

string rot(const string& line, int amount) {
  string newString;
  for (int i = 0; i < line.length(); i++) {
    if (isalpha(line.at(i))) {
      newString += rot(toupper(line.at(i)), amount);
    } else if (isspace(line.at(i))) {
      newString += line.at(i);
    }
  }
  return newString;
}

void caesarEncryptCommand() {
  cout << "Enter the text to encrypt:";
  string toenc;
  getline(cin, toenc);
  int amount;
  cout << "Enter the number of characters to rotate by:";
  cin >> amount;
  toenc = rot(toenc, amount);
  cout << toenc << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  for (int i = 0; i < strings.size(); ++i) {
    strings[i] = rot(strings[i], amount);
  }
}

string clean(const string& s) {
  string cleaned;
  for (int i = 0; i < s.length(); ++i) {
    if (isalpha(s[i])) {
      cleaned += toupper(s[i]);
    }
  }
  return cleaned;
}

vector<string> splitBySpaces(const string& s) {
  vector<string> split;  // after class I saw this was the easy way to do this
  string z;
  istringstream in(s);
  while (in >> z) {
    split.push_back(z);
  }
  // string n;
  // bool currentword = false;
  // for (int i = 0; i < s.length(); ++i) {
  //   if (isalpha(s[i])) {
  //     n += s[i];
  //     currentword = true;
  //   } else if (isspace(s[i]) && currentword) {
  //     split.push_back(n);
  //     currentword = false;
  //     n.clear();
  //   }
  // }
  // if (currentword) {
  //   split.push_back(n);
  // }
  return split;
}

string joinWithSpaces(const vector<string>& words) {
  string join;
  if (words.size() > 0) {
    for (int i = 0; i < words.size() - 1; ++i) {
      join += words[i];
      join += " ";
    }
    join += words[words.size() - 1];
  }
  return join;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int count = 0;
  for (int i = 0; i < words.size(); ++i) {
    for (int j = 0; j < dict.size(); ++j) {
      if (words[i] == dict[j]) {
        ++count;
      }
    }
  }
  return count;
}

void caesarDecryptCommand(const vector<string>& dict) {
  cout << "Enter the text to decrypt:";
  string todecrypt;
  vector<string> mystery;
  vector<string> inp;
  getline(cin, todecrypt);
  int nummatches = 0;
  bool anydecryptions = false;

  mystery = splitBySpaces(todecrypt);
  for (int i = 0; i < mystery.size(); ++i) {
    mystery[i] = clean(mystery[i]);
  }
  inp = mystery;
  rot(mystery, 0);

  nummatches = numWordsIn(mystery, dict);
  if (nummatches > (mystery.size() / 2)) {
    todecrypt = joinWithSpaces(mystery);
    cout << todecrypt << endl;
  }
  for (int i = 0; i < 26; ++i) {
    rot(mystery, 1);

    nummatches = numWordsIn(mystery, dict);
    if (nummatches > (mystery.size() / 2)) {
      todecrypt = joinWithSpaces(mystery);
      cout << todecrypt << endl;
      anydecryptions = true;
    }
  }
  if (!anydecryptions) {
    cout << "No good decryptions found";
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string encrypted;
  for (int i = 0; i < s.length(); ++i) {
    if (isalpha(s[i])) {
      encrypted += cipher[toupper(s[i]) - 'A'];
    } else {
      encrypted += s[i];
    }
  }
  return encrypted;
}

void applyRandSubstCipherCommand() {
  cout << "Enter text to be encrypted:";
  string enc;
  getline(cin, enc);
  cout << applySubstCipher(genRandomSubstCipher(), enc);
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  double score = 0;
  int length = s.length();
  for (int i = 0; i < length - 3; ++i) {
    score += scorer.getScore(s.substr(i, 4));
  }
  return score;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  cout << "Enter a string for scoring:";
  string s;
  getline(cin, s);
  s = clean(s);
  cout << s;
  double score = scoreString(scorer, s);
  cout << score;
}

// vector<char> hillClimb(const QuadgramScorer& scorer, const string&
// ciphertext) {
//   vector<char> guess = genRandomSubstCipher();
//   vector<char> newkey;
//   newkey = guess;
//   int count = 0;
//   double oldscore = 0;
//   double newscore = 0;
//   string test;
//   string cleanedciphertext = clean(ciphertext);
//   test = cleanedciphertext;
//   oldscore = scoreString(scorer, cleanedciphertext);
//   int rand1;
//   int rand2;
//   while (count < 1000) {
//     test = cleanedciphertext;
//     rand1 = Random::randInt(25);
//     rand2 = Random::randInt(25);

//     while (rand1 == rand2) {
//       rand2 = Random::randInt(25);
//     }
//     swap(guess.at(rand1), guess.at(rand2));
//     test = applySubstCipher(guess, test);

//     newscore = scoreString(scorer, test);

//     if (newscore > oldscore) {
//       oldscore = newscore;
//       count = 0;
//       newkey = guess;

//     } else {
//       count++;
//       guess = newkey;
//     }
//   }
//   return newkey;
// }

// vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
//                                 const string& ciphertext) {
//   double newscore;
//   vector<char> cipher;
//   vector<char> goodcipher;
//   string cleaned = clean(ciphertext);
//   string test = cleaned;
//   double oldscore = scoreString(scorer, cleaned);
//   for (int i = 0; i < 26; ++i) {
//     test = cleaned;
//     cipher = hillClimb(scorer, test);

//     test = applySubstCipher(cipher, test);

//     newscore = scoreString(scorer, test);

//     if (newscore > oldscore) {
//       oldscore = newscore;
//       goodcipher = cipher;
//     }
//   }

//   return goodcipher;
// }

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string s;
  getline(cin, s);

  vector<char> cipher = decryptSubstCipher(scorer, s);
  s = applySubstCipher(cipher, s);
  cout << s;
}

#pragma endregion SubstDec
