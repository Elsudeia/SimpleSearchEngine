#include "ConvertJson.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <iostream>

int main() {
    try {
        ConverterJSON jsonConverter;
        jsonConverter.CheckForEssentialFiles();

        std::vector<std::string> documents = jsonConverter.GetTextDocuments();
        if (documents.empty()) {
            std::cerr << "Can't get documents" << std::endl;
            return 1;
        }

        InvertedIndex index;
        index.UpdateDocumentBase(documents);

        SearchServer searchServer(index);

        std::vector<std::string> queries = jsonConverter.GetRequests();
        if (queries.empty()) {
            std::cerr << "Can't get requests" << std::endl;
            return 1;
        }

        std::vector<std::vector<RelativeIndex>> results = searchServer.search(queries);
        
        std::vector<std::vector<std::pair<int, float>>> formattedResults;
        for (const auto& result : results) {
            std::vector<std::pair<int, float>> formatted;
            for (const auto& entry : result) {
                formatted.emplace_back(entry.doc_id, entry.rank);
            }
            formattedResults.push_back(formatted);
        }
        
        jsonConverter.putAnswers(formattedResults);
        
        std::cout << "Search complete results in answers.json" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
