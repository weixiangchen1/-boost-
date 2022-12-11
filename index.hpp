#pragma once 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include "util.hpp"
#include "log.hpp"

namespace ns_index
{
    struct DocInfo
    {
        std::string title;   // 文档的标题
        std::string content; // 文档的内容梗概
        std::string url;     // 文档的官方网址
        uint64_t doc_id;     // 文档id
    };

    struct InvertedElem
    {
        uint64_t doc_id;     // 文档id
        std::string word;    // 文档的关键词
        int weight;          // 权重
    };
               
    // 倒排拉链
    typedef std::vector<InvertedElem> InvertedList;

    class Index
    {
        private:
            static Index* instance;
            static std::mutex mtx;
        private:
            Index(){};
            Index(const Index& idx) = delete;
            Index& operator=(const Index& idx) = delete;
        public:
            static Index* GetInstance()
            {
                if (nullptr == instance) {
                    mtx.lock();
                    if (nullptr == instance) {
                        instance = new Index();
                    }
                    mtx.unlock();
                }

                return instance;
            }

            ~Index(){};
        public:
            // 根据文档id找到文档内容 -- 正排索引
            DocInfo* GetForwardIndex(uint64_t doc_id) {
                if (doc_id >= forward_index.size()) {
                    // std::cerr << "doc_id out range" << std::endl;
                    LOG(FATAL, "doc_id out range");
                    return nullptr;
                }
                return &forward_index[doc_id];
            }

            // 根据关键字找到倒排拉链 -- 倒排索引
            InvertedList* GetInvertedList(const std::string& word) {
                auto item = inverted_index.find(word);
                if (item == inverted_index.end()) {
                    // std::cerr << "get invertedlist error" << std::endl;
                    LOG(FATAL, "get invertedlist error");
                    return nullptr;
                }
                return &(item->second);
            }

            // 构建正排索引和倒排索引
            bool BuildIndex(const std::string& input) {
                // 读取data/raw.html/raw.txt 的内容
                std::ifstream in(input, std::ios::in | std::ios::binary);
                if (!in.is_open()) {
                    // std::cerr << "open " << input << " error" << std::endl;
                    LOG(FATAL, "open " + input + " error");
                    return false;
                }
                
                std::string line;
                int cnt = 0;
                while (std::getline(in, line)) {
                    // 构建正排索引
                    DocInfo* doc = BuildForwardIndex(line);
                    if (doc == nullptr) {
                        // std::cerr << "build " << line << "error" << std::endl;
                        LOG(FATAL, "bulid index error");
                        continue;
                    }

                    // 构建倒排索引
                    BuildInvertedIndex(*doc);
                    cnt++;
                    if (cnt % 50 == 0) {
                        // std::cout << "当前已经建立的索引文档: [" << cnt << "]" << std::endl;
                        LOG(NORMAL, "当前已经建立的索引文档: [" + std::to_string(cnt) + "]");
                    }
                }
                return true;
            }
        private:
            DocInfo* BuildForwardIndex(const std::string& line) {
                // 1. 解析line -> title/content/url 
                std::vector<std::string> results;   // 存放解析后的三段字符串
                const std::string sep = "\3";       // 字符串分隔符
                ns_util::StringTool::Split(line, &results, sep);

                // 2. 切分line 填充DocInfo
                DocInfo doc;
                doc.title = results[0];
                doc.content = results[1];
                doc.url = results[2];
                doc.doc_id = forward_index.size();

                // 3. 将DocInfo插入到forward_index正排索引
                forward_index.push_back(std::move(doc));

                return &forward_index.back();
            }
            
            bool BuildInvertedIndex(const DocInfo& doc) {
                struct word_cnt {
                    int title_cnt;
                    int content_cnt;

                    word_cnt(): title_cnt(0), content_cnt(0) {}
                };
                
                // 对文档标题进行分词
                std::vector<std::string> title_words;
                ns_util::JiebaUtil::CutString(doc.title, &title_words);
                // 对文档标题进行词频统计
                std::unordered_map<std::string, word_cnt> word_map;
                for (std::string s : title_words) {
                    boost::to_lower(s);
                    word_map[s].title_cnt++;
                }

                // 对文档内容进行分词
                std::vector<std::string> content_words;
                ns_util::JiebaUtil::CutString(doc.content, &content_words);
                // 对文档内容进行词频统计
                for (std::string s : content_words) {
                    boost::to_lower(s);
                    word_map[s].content_cnt++;
                }

    #define X 10
    #define Y 1
                for (auto & word_pair : word_map) {
                    InvertedElem item;
                    item.doc_id = doc.doc_id;
                    item.word = word_pair.first;
                    item.weight = X*word_pair.second.title_cnt + Y*word_pair.second.content_cnt;

                    InvertedList & inverted_list = inverted_index[word_pair.first];
                    inverted_list.push_back(std::move(item));
                }

                return true;
            }

        private:
            // 正排索引，通过数组下标代表文档id
            std::vector<DocInfo> forward_index;
            // 倒排索引，一个关键词与多个InvertedElem(倒排拉链)对应
            std::unordered_map<std::string, InvertedList> inverted_index;
    };
    Index* Index::instance = nullptr;
    std::mutex Index::mtx;

}
