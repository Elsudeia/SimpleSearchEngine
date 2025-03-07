#include <gtest/gtest.h>
#include "ConvertJson.h"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class ConverterJSONTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаём тестовые файлы
        CreateTestConfig();
        CreateTestRequests();
    }

    void TearDown() override {
        // Удаляем тестовые файлы после тестов
        std::filesystem::remove("../config.json");
        std::filesystem::remove("../requests.json");
    }

    void CreateTestConfig() {
        json config;
        config["config"] = {
            {"name", "Test_Search_Engine"},
            {"version", "0.1"},
            {"max_responses", 3}
        };
        config["files"] = {"../resources/file000.txt", "../resources/file001.txt"};
        std::ofstream file("../config.json");
        file << config.dump(4);
    }

    void CreateTestRequests() {
        json requests;
        requests["requests"] = {"test1", "test2"};
        std::ofstream file("../requests.json");
        file << requests.dump(4);
    }
};

TEST_F(ConverterJSONTest, GetResponsesLimit) {
    ConverterJSON converter;
    EXPECT_EQ(converter.GetResponsesLimit(), 3);
}

TEST_F(ConverterJSONTest, GetRequests) {
    ConverterJSON converter;
    auto requests = converter.GetRequests();
    ASSERT_EQ(requests.size(), 2);
    EXPECT_EQ(requests[0], "test1");
    EXPECT_EQ(requests[1], "test2");
}

TEST_F(ConverterJSONTest, GetTextDocuments) {
    ConverterJSON converter;
    auto docs = converter.GetTextDocuments();
    ASSERT_EQ(docs.size(), 2);
}

TEST_F(ConverterJSONTest, PutAnswers) {
    ConverterJSON converter;
    std::vector<std::vector<std::pair<int, float>>> answers = {
        {{1, 0.8}, {2, 0.5}},
        {{3, 1.0}}
    };
    converter.putAnswers(answers);
    
    std::ifstream file("../answers.json");
    ASSERT_TRUE(file.good());
}