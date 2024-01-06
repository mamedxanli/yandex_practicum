#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
} //correct
 
int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}  //correct

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
} //correct

set<string> ParseStopWords(const string& text) {
    set<string> stop_words;
    for (const string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    } 
    return stop_words;
} //correct

const int MAX_RESULT_DOCUMENT_COUNT = 5;

struct Document {
    int id;
    int relevance;
}; //correct

bool HasDocumentGreaterRelevance(const Document& lhs, const Document& rhs) {
    return lhs.relevance > rhs.relevance;
} //correct

class SearchServer {
    public:
        struct DocumentContent {
            int id;
            vector <string> words;
        }; //correct
    
        void AddDocument(int document_id, const string& document) {
            const vector<string> words = SplitIntoWordsNoStop(document);
            documents_.push_back({document_id, words});    
        } //correct
    
        void SetStopWords(const string& text) {
            for (const string& word : SplitIntoWords(text)) {
                stop_words_.insert(word);
            }
        } //correct
    
        vector<Document> FindTopDocuments(const string& raw_query) {
        const set<string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(), HasDocumentGreaterRelevance);
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    } //?????????
    
    void PrintAllDocuments() const {
        for (const auto& document : documents_) {
            cout << "Document ID: " << document.id << endl;
            for (const auto& word : document.words) {
                cout << word << " ";
            }
            cout << endl;
        }
    }
    
    vector<DocumentContent> GetDocuments() const {
        return documents_;
    }

    private:
        vector <DocumentContent> documents_;
        set <string> stop_words_;
    
        vector<string> SplitIntoWordsNoStop(const string& document) {
        vector<string> words;
        for (const string& word : SplitIntoWords(document)) {
            if (stop_words_.count(word) == 0) {
                words.push_back(word);
            }
        }
        return words;
    } // correct
    
        vector<Document> FindAllDocuments(const set<string>& query_words) {
        vector<Document> matched_documents;
        for (const auto& document : documents_) {
            const int relevance = MatchDocument(document, query_words);
            if (relevance > 0) {
                matched_documents.push_back({document.id, relevance});
            }
        }
    return matched_documents;
    } // correct
    
        static int MatchDocument(const DocumentContent& content, const set<string>& query_words) {
            if (query_words.empty()) {
                return 0;
    }
        set<string> matched_words;
        for (const string& word : content.words) {
            if (matched_words.count(word) != 0) {
                continue;
            }
            if (query_words.count(word) != 0) {
                matched_words.insert(word);
            }
        }
        return static_cast<int>(matched_words.size());
            } // ??????????
    
        set<string> ParseQuery(const string& raw_query) {
            set<string> query_words;
            for (const string& word : SplitIntoWordsNoStop(raw_query)) {
                query_words.insert(word);
            }
            return query_words;
        } // correct
    
};

SearchServer CreateSearchServer() {
        SearchServer search_server;
        search_server.SetStopWords(ReadLine());
        const int document_count = ReadLineWithNumber();        
        for (int x = 0; x < document_count; ++x) {
            search_server.AddDocument(x, ReadLine());
        } return search_server;
        } // correct



int main() {
    // Create the SearchServer instance with the input data
    SearchServer search_server = CreateSearchServer();
    const auto& documents = search_server.GetDocuments();
   
    for (const auto& doc : documents) {
        string query = "";
        for (const auto& word : doc.words) {
            query += word + " ";
        }
        auto top_documents = search_server.FindTopDocuments(query);
        for (const auto& docs : top_documents) {
            cout << "Document ID: " << docs.id << ", Relevance: " << docs.relevance << endl;
        }

}
    return 0;
}