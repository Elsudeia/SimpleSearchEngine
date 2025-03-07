#pragma once
#include <vector>
#include <string>

class ConverterJSON {
public:
ConverterJSON() = default;

void CheckForEssentialFiles();

std::vector<std::string> GetTextDocuments();

int GetResponsesLimit();

std::vector<std::string> GetRequests();

void putAnswers(const std::vector<std::vector<std::pair<int, float>>>
&answers);
private:
    void MakeSampleRequestsFile();
    void MakeSampleConfigFile();
    std::string config_path = "../config.json";
    std::string resources_folder = "../resources";
    std::string requests_path = "../requests.json";
};