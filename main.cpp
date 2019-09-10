#include <iostream>
#include <fstream>
#include <vector>

using std::istream;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

static const char* valid_key_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char* valid_whitespace = " \t";
static const char* programName;

static void error(const string& message) {
    cout << programName << ": " << message << endl;
    exit(-1);
}
static void error(const char* message) {
    error(string(message));
}
static void error(const char* message, const char* value) {
    error(string(message) + value);
}
static void error(const char* message, const string& value) {
    error(string(message) + value);
}

static void loadKeys(ifstream& in, vector<string>& keys) {
    string key;
    while (getline(in, key)) {
        if (key.length() < 1) {
            error("Empty key found.");
        } else if (key.find_first_not_of(valid_key_characters) != string::npos) {
            error("Invalid key found: ", key);
        } else {
            keys.push_back(key);
        }
    }
}

static bool keyIsValid(const vector<string>& validKeys, const string& key) {
    for (int i = 0; i < validKeys.size(); i++) {
        if (validKeys[i] == key) {
            return true;
        }
    }
    return false;
}

static void printSpaces(int count) {
    for (int i = 0; i < count; i++) {
        cout << " ";
    }
}

static void loadEnemies(istream& in, vector<string>& validKeys) {
    int maxKeyLength = 0;
    vector<string> keys;
    vector<string> values;
    bool firstEnemy = true;

    string line;
    while (getline(in, line)) {
        if (line.length() == 0) {
            // end of enemy
            if (!firstEnemy) {
                cout << endl;
            } else {
                firstEnemy = false;
            }

            for (int i = 0; i < keys.size(); i++) {
                cout << keys[i];
                printSpaces(maxKeyLength - keys[i].length() + 1);
                cout << values[i] << endl;
            }

            // clear current enemies lise
            keys.clear();
            values.clear();
            maxKeyLength = 0;
        } else {
            int firstNonSpaceNdx = line.find_first_not_of(valid_whitespace);
            if (firstNonSpaceNdx > 0) {
                // continuation
                if (values.empty()) {
                    error("Continuation line does not follow key/pair line.");
                } else {
                    values.back() += " ";
                    values.back() += line.substr(firstNonSpaceNdx);
                }
            } else {
                //new key/value line
                int firstNonAlphabetic = line.find_first_not_of(valid_key_characters);
                if (firstNonAlphabetic == string::npos) {
                    error("Line constains only key: ", line);
                } else {
                    string key = line.substr(0, firstNonAlphabetic);
                    if (!keyIsValid(validKeys, key)) {
                        error("Invalid key: ", key);
                    } else {
                        line = line.substr(firstNonAlphabetic);
                        firstNonSpaceNdx = line.find_first_not_of(valid_whitespace);
                        if (firstNonAlphabetic == 0) {
                            error("Invalid key character at: ", line);
                        } else if (firstNonSpaceNdx == string::npos) {
                            error("Line missing value after key");
                        } else {
                            keys.push_back(key);
                            values.push_back(line.substr(firstNonSpaceNdx));
                            if (maxKeyLength < key.length()) {
                                maxKeyLength = key.length();
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    vector<string> keys;

    programName = argv[0];

    if (argc < 2) {
        error("Missing key file argument");
    } else {
        string name(argv[1]);
        ifstream in(name, ifstream::in);
        if (!in.is_open()) {
            error("Could not open key file: ", argv[1]);
        } else {
            loadKeys(in, keys);
            in.close();

            if (argc > 2) {
                for (int i = 2; i < argc; i++) {
                    in.open(argv[i]);
                    if (!in.is_open()) {
                        error("Could not open data file: ", argv[i]);
                    } else {
                        loadEnemies(in, keys);
                        in.close();
                    }
                }
            } else {
                loadEnemies(cin, keys);
            }
        }

        return 0;
    }
}