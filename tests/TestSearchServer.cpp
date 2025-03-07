#include <gtest/gtest.h>
#include "SearchServer.h"
#include "InvertedIndex.h"

using namespace std;

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };

    const vector<string> request = {"milk water", "sugar"};
    
    const vector<vector<RelativeIndex>> expected = {
        {
            {2, 1.0}, 
            {0, 0.7}, 
            {1, 0.3}
        },
        {}
    };

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    
    vector<vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        ASSERT_EQ(result[i].size(), expected[i].size());
        for (size_t j = 0; j < result[i].size(); ++j) {
            ASSERT_EQ(result[i][j].doc_id, expected[i][j].doc_id);
            ASSERT_FLOAT_EQ(result[i][j].rank, expected[i][j].rank);
        }
    }
}

TEST(TestCaseSearchServer, TestTop5) {
    const vector<string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo stockholm is the capital of sweden", 
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland",
    };

    const vector<string> request = {"moscow is the capital of russia"};

    const vector<vector<RelativeIndex>> expected = {
        {
            {7, 1.0}, 
            {14, 1.0},  
            {0, 0.666667}, 
            {1, 0.666667}, 
            {2, 0.666667}
        }
    };

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    
    vector<vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        ASSERT_EQ(result[i].size(), expected[i].size());
        for (size_t j = 0; j < result[i].size(); ++j) {
            ASSERT_EQ(result[i][j].doc_id, expected[i][j].doc_id);
            ASSERT_NEAR(result[i][j].rank, expected[i][j].rank, 0.0001);
        }
    }
}
