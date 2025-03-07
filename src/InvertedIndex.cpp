#include "InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <set>
#include <condition_variable>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs); 

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) { max_threads = 2; }

    std::vector<std::thread> thread_pool;
    std::mutex queue_mutex;
    std::queue<size_t> task_queue;
    std::unordered_map<std::string, std::mutex> word_mutexes;

    std::mutex task_mutex;
    std::condition_variable task_cv;
    size_t remaining_tasks = docs.size();

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        task_queue.push(doc_id);
    }

    auto worker = [&]() {
        while (true) {
            size_t doc_id;
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (task_queue.empty()) return;
                doc_id = task_queue.front();
                task_queue.pop();
            }

            std::unordered_map<std::string, std::vector<Entry>> local_dict;
            std::unordered_map<std::string, size_t> word_count;
            std::istringstream stream(docs[doc_id]);
            std::string word;

            while (stream >> word) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c) {
                    return !std::isalnum(c) && c != '-';
                }), word.end());
                if (!word.empty()) {
                    ++word_count[word];
                }
            }

            for (const auto& [word, count] : word_count) {
                local_dict[word].push_back({ doc_id, count });
            }

            for (const auto& [word, entries] : local_dict) {
                std::lock_guard<std::mutex> lock(word_mutexes[word]);
                auto& word_entries = freq_dictionary[word];
                word_entries.insert(word_entries.end(), entries.begin(), entries.end());

                std::set<Entry> unique_entries(word_entries.begin(), word_entries.end());
                word_entries.assign(unique_entries.begin(), unique_entries.end());
            }

            {
                std::lock_guard<std::mutex> lock(task_mutex);
                if (--remaining_tasks == 0) {
                    task_cv.notify_one(); 
                }
            }
        }
    };

    for (unsigned int i = 0; i < max_threads; ++i) {
        thread_pool.emplace_back(worker);
    }

    {
        std::unique_lock<std::mutex> lock(task_mutex);
        task_cv.wait(lock, [&]() { return remaining_tasks == 0; });
    }

    for (auto& thread : thread_pool) {
        thread.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    if (freq_dictionary.find(word) != freq_dictionary.end()) {
        return freq_dictionary[word];
    }
    return {};
}
