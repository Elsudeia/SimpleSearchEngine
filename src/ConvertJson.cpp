#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>
#include "ConvertJson.h"

using json = nlohmann::json;

void ConverterJSON::MakeSampleRequestsFile() {

    json requests_data;
    requests_data["requests"] = json::array();

    requests_data["requests"].push_back("unabtanium");
    requests_data["requests"].push_back("nonexistanium");
    requests_data["requests"].push_back("wild range");
    requests_data["requests"].push_back("big brother");
    requests_data["requests"].push_back("simple cup");
    
    try {
        std::ofstream requests_file(requests_path);
        if (!requests_file) {
            throw std::runtime_error("Error creating requests.json");
        }

        requests_file << requests_data.dump(4);
        requests_file.close();
        std::cout << "requests.json was successfully created." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing to requests.json: " << e.what() << std::endl;
    }
}

void ConverterJSON::MakeSampleConfigFile(){

    json config_data;
    config_data["config"] = {
        {"name", "My_First_Search_Engine"},
        {"version", "0.1"},
        {"max_responses", 5}
    };
    config_data["files"] = json::array();
    try {
        if (std::filesystem::exists(resources_folder) && std::filesystem::is_directory(resources_folder)) {
            for (const auto& entry : std::filesystem::directory_iterator(resources_folder)) {
                if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                    config_data["files"].push_back(entry.path().string());
                }
            }
        } else {
            std::cerr << "Missing resource folder!\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Can't access folder resources: " << e.what() << std::endl;
    }

    try {
        std::ofstream config_file(config_path);
        if (!config_file) {
            throw std::runtime_error("Error making config.json");
        }
        config_file << config_data.dump(4);
        config_file.close();
        std::cout << "config.json was successfully made." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing in config.json: " << e.what() << std::endl;
    }
};

void ConverterJSON::CheckForEssentialFiles(){
    if (!std::filesystem::exists(config_path)) {
        MakeSampleConfigFile();
    }
    if (!std::filesystem::exists(requests_path)) {
       MakeSampleRequestsFile();
    }
};

json OpenJsonFile (std::string &json_file_path){
    json containment;
    try {
        std::ifstream json_file(json_file_path);
        if (!json_file) {
            throw std::runtime_error("Can not open .json file");
        }
        json_file >> containment;
        json_file.close();
        if (containment.is_null()) {
            throw std::runtime_error(".json file is empty!");
        }
    } catch (const json::parse_error& e) {
        std::cerr << "Parsing error JSON: " << json_file_path << " " << e.what() << std::endl;
        return containment;
    } catch (const std::exception& e) {
        std::cerr << "Error while reading .json file: " << json_file_path << " " << e.what() << std::endl;
        return containment;
    }
    return containment;
}

std::vector<std::string> ConverterJSON::GetTextDocuments(){
    std::vector<std::string> files_data;
    std::string config_path = "../config.json";
    json config = OpenJsonFile(config_path);
    
    if (!config.contains("files") || !config["files"].is_array()) {
        std::cerr << "Config.json does not have 'files'" << std::endl;
        return files_data;
    }

    try {
        for (const auto& filename : config["files"]) {
            if (!filename.is_string()) continue;

            std::filesystem::path file_path = filename.get<std::string>();

            std::ifstream file(file_path);
            if (!file) {
                std::cerr << "Can't open file: " << file_path << std::endl;
                continue;
            }
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            files_data.push_back(content);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading files: " << e.what() << std::endl;
        return files_data;
    }
    return files_data;
}

int ConverterJSON::GetResponsesLimit(){
    int max_responses = -1;
    std::string config_path = "../config.json";
    json config = OpenJsonFile(config_path);

    if (config.contains("config") && config["config"].contains("max_responses")) {
        max_responses = config["config"]["max_responses"];
    } else {
        std::cerr << "Can't find 'max_responses'" << std::endl;
    }
    return max_responses;
}

std::vector<std::string> ConverterJSON::GetRequests(){
    std::vector<std::string> requests;
    std::string requests_path = "../requests.json";

    json config = OpenJsonFile(requests_path);
    if (config.is_null() || !config.contains("requests")) {  
        std::cerr << "Key 'requests' is missing" << std::endl;
        return requests;
    }
    try {
        for (const auto& item : config.at("requests")) {  
            if (item.is_string()) {  
                requests.push_back(item.get<std::string>());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing requests.json: " << e.what() << std::endl;
        return requests;
    }
    return requests;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    json output_json;

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string request_key = "request" + std::to_string(i + 1);

        if (answers[i].empty()) {
            output_json["answers"][request_key]["result"] = "false";
        } else {
            output_json["answers"][request_key]["result"] = "true";

            for (const auto& [docid, rank] : answers[i]) {
                output_json["answers"][request_key]["relevance"].push_back({{"docid", docid}, {"rank", rank}});
            }
        }
    }
    std::ofstream file("../answers.json");
    if (!file) {
        std::cerr << "Can't open answers.json for writing" << std::endl;
        return;
    }
    file << output_json.dump(4); 
    file.close();
}