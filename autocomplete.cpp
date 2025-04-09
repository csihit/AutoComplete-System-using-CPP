#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    unordered_map<string, int> frequency;
    unordered_map<string, int> context;
    vector<string> suggestions;
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() { root = new TrieNode(); }

    void insert(const string& phrase, const string& contextWord) {
        TrieNode* current = root;
        string prefix = "";
        for (char c : phrase) {
            prefix += c;
            if (!current->children.count(c)) {
                current->children[c] = new TrieNode();
            }
            current = current->children[c];

            if (find(current->suggestions.begin(), current->suggestions.end(), phrase) == current->suggestions.end()) {
                if (current->suggestions.size() < 5) {
                    current->suggestions.push_back(phrase);
                } else {
                    string leastFrequent = *min_element(
                        current->suggestions.begin(),
                        current->suggestions.end(),
                        [&](const string& a, const string& b) {
                            return current->frequency[a] < current->frequency[b];
                        }
                    );
                    if (current->frequency[phrase] > current->frequency[leastFrequent]) {
                        replace(current->suggestions.begin(), current->suggestions.end(), leastFrequent, phrase);
                    }
                }
            }

            current->frequency[phrase]++;
            current->context[contextWord]++;
        }
        current->isEndOfWord = true;
    }

    vector<string> getSuggestions(const string& prefix, const string& contextWord) {
        TrieNode* current = root;
        for (char c : prefix) {
            if (!current->children.count(c)) {
                return {};
            }
            current = current->children[c];
        }

        sort(current->suggestions.begin(), current->suggestions.end(),
             [&](const string& a, const string& b) {
                 if (current->context[a] != current->context[b])
                     return current->context[a] > current->context[b];
                 return current->frequency[a] > current->frequency[b];
             });

        return vector<string>(current->suggestions.begin(), 
                              current->suggestions.begin() + min(5, (int)current->suggestions.size()));
    }

    void learnFromUserInput(const string& phrase) {
        insert(phrase, "USER_DEFINED");
    }
};

int main() {
    Trie trie;

    vector<string> phrases = {
        "I am feeling very happy today",
        "I am feeling very tired after work",
        "I am feeling very excited about the weekend",
        "I am feeling very productive",
        "I am feeling very proud of myself",
        "I am feeling very strong today",
        "I am feeling very cold right now"
    };

    for (const string& phrase : phrases) {
        trie.insert(phrase, "GENERAL");
    }

    string input;
    while (true) {
        cout << "\nType a phrase: ";
        getline(cin, input);

        vector<string> suggestions = trie.getSuggestions(input, "GENERAL");

        if (!suggestions.empty()) {
            cout << "Suggestions:\n";
            for (const string& suggestion : suggestions) {
                cout << " - " << suggestion << endl;
            }
        } else {
            cout << "No suggestions found. Add to learning? (Y/N): ";
            char choice;
            cin >> choice;
            cin.ignore();  
            if (choice == 'Y' || choice == 'y') {
                trie.learnFromUserInput(input);
                cout << "Phrase added to learning.\n";
            }
        }
    }

    return 0;
}