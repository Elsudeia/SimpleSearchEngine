#include "SearchServer.h"
#include "InvertedIndex.h"
#include "ConvertJson.h"
#include <sstream>
#include <algorithm>
#include <set>
#include <iostream>

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> results;
    int max_responses = ConverterJSON().GetResponsesLimit();

    for (const auto& query : queries_input) {
        std::istringstream iss(query);
        std::set<std::string> unique_words;
        std::string word;

        while (iss >> word) {
            unique_words.insert(word);
        }

        std::vector<std::pair<std::string, size_t>> sorted_words;
        for (const auto& word : unique_words) {
            size_t word_count = _index.GetWordCount(word).size(); 
            sorted_words.push_back({word, word_count});
        }
        std::sort(sorted_words.begin(), sorted_words.end(), 
                  [](const std::pair<std::string, size_t>& a, const std::pair<std::string, size_t>& b) {
                      return a.second < b.second;
                  });

        std::map<size_t, size_t> doc_relevance; 

        for (const auto& [word, _] : sorted_words) {
            auto word_entries = _index.GetWordCount(word);
            for (const auto& entry : word_entries) {
                doc_relevance[entry.doc_id] += entry.count;
            }
        }

        if (doc_relevance.empty()) {
            results.push_back({});
            continue;
        }

        size_t max_relevance = 0;
        for (const auto& [doc_id, relevance] : doc_relevance) {
            if (relevance > max_relevance) {
                max_relevance = relevance;
            }
        }

        std::vector<RelativeIndex> search_results;
        for (const auto& [doc_id, relevance] : doc_relevance) {
            search_results.push_back({doc_id, static_cast<float>(relevance) / max_relevance});
        }

        std::sort(search_results.begin(), search_results.end(), 
          [](const RelativeIndex& a, const RelativeIndex& b) {
              if (a.rank == b.rank) {
                  return a.doc_id < b.doc_id;
              }
              return a.rank > b.rank;
          });

        if (max_responses > 0 && search_results.size() > static_cast<size_t>(max_responses)) {
            search_results.resize(max_responses);
        }


        results.push_back(search_results);
    }
    
    return results;
}