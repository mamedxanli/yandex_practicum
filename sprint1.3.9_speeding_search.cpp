#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    int relevance;
};

struct Query {
    set <string> minus_words_;
    set <string> plus_words_;
};


class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        for (const auto& word : words) {
            if (document.find(word) != string::npos) {
                word_to_documents_[word].insert(document_id);
            }
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    } //+



private:
    map<string, set<int>> word_to_documents_;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    bool IsInDoc(const string& word) const {
        return word_to_documents_.count(word) > 0;
    }

    Query ParseQuery(const string& text) const {
        Query query;
        vector <string> words = SplitIntoWordsNoStop(text);
        for (const string& word : words) {
            if (word[0] == '-' && !IsStopWord(word.substr(1)) ) {
                query.minus_words_.insert(word.substr(1));
            } else {query.plus_words_.insert(word);}
        } return query;
    } //+

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, int> document_to_relevance; // first int is id of document found, second int is relevancy = count of plus_words
        if (!query_words.plus_words_.empty()) {
            for (const auto& word : query_words.plus_words_) {
                if (IsInDoc(word)) {
                    for (int id : word_to_documents_.at(word)) {
                        document_to_relevance[id] += 1;
                    }
                } else { continue; 
                }
            }
            }
            else { return matched_documents; }
            
        if (!query_words.minus_words_.empty()) {
            for (const auto& word : query_words.minus_words_) {
                if (IsInDoc(word)) {
                    for (int id : word_to_documents_.at(word)) {
                        document_to_relevance.erase(id);
                    }
                }
                else { continue; }
            }
        }

        if (document_to_relevance.size() > 0) {
            for (auto& [id, relevance] : document_to_relevance) {
                matched_documents.push_back({id, relevance});
            }
        }

        return matched_documents;

    }
};


SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = " << document_id << ", " << "relevance = " << relevance << " }" << endl;
    }
}