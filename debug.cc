#include "searcher.hpp"
#include "index.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

const std::string input = "./data/raw_html/raw.txt";

int main()
{
    ns_searcher::Searcher* searcher = new ns_searcher::Searcher();
    searcher->InitSearcher(input);

    std::string query;
    std::string json_string;
    char buffer[1024];
    while(true) {
        std::cout << "Please Enter Your Search Query#:";
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strlen(buffer) - 1] = 0;
        query = buffer;
        std::cout << buffer << std::endl;
        searcher->Search(query, &json_string);
        std::cout << json_string << std::endl;
    }


    return 0;
}