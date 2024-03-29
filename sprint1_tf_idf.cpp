#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

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
    double relevance;
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
        map <string, int> word_count;
        for (const auto& word : words) {
            ++word_count[word];
        }
        for (const auto& [word, count] : word_count) {
            double tf = static_cast <double> (count)/words.size();
            word_to_document_freqs_[word][document_id] = tf;
            }
        ++document_count_;
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
    }



private:
    map<string, map<int, double>> word_to_document_freqs_;
    int document_count_ = 0;
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
        return word_to_document_freqs_.count(word) > 0;
    }

    Query ParseQuery(const string& text) const {
        Query query;
        vector <string> words = SplitIntoWordsNoStop(text);
        for (const string& word : words) {
            if (word[0] == '-' && !IsStopWord(word.substr(1)) ) {
                query.minus_words_.insert(word.substr(1));
            } else {query.plus_words_.insert(word);}
        } return query;
    }

    double InverseDocumentFrequency(int documentCount, double documentFrequency) const {
        //return log(static_cast<double>(documentCount) / (1 + documentFrequency));
        return log(static_cast<double>(documentCount) / documentFrequency);
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance;
        if (!query_words.plus_words_.empty()) {
            for (const auto& word : query_words.plus_words_) {
                if (IsInDoc(word)) {
                    double idf = InverseDocumentFrequency(document_count_, word_to_document_freqs_.at(word).size());
                    for (const auto& [id, tf] : word_to_document_freqs_.at(word)) {
                        document_to_relevance[id] += tf * idf;
                        }
                    } 
                }
            }

        if (!query_words.minus_words_.empty()) {
            for (const auto& word : query_words.minus_words_) {
                if (IsInDoc(word)) {
                    for (const auto& [id, freq] : word_to_document_freqs_.at(word)) {
                        document_to_relevance.erase(id);
                    }
                }
            }
        }
        
        if (document_to_relevance.size() > 0) {
            for (const auto& [id, relevance] : document_to_relevance) {
                matched_documents.push_back({id, static_cast<double>(relevance)});
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