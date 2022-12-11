#include <string>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include <unordered_map>
#include "index.hpp"
#include "util.hpp"
#include "log.hpp"

namespace ns_searcher {
    struct InvertedElemPrint {
        uint64_t doc_id;
        int weight;
        std::vector<std::string> words;
        InvertedElemPrint() : doc_id(0), weight(0) {}
    };


    class Searcher {
    private:
        ns_index::Index* index;
    public:
        void InitSearcher(const std::string& input)
        {
            // 1. 创建Index单例对象
            index = ns_index::Index::GetInstance();
            // std::cout << "创建Index单例完成..." << std::endl;
            LOG(NORMAL, "创建Index单例完成...");
            // 2. 根据Index对象构建索引
            index->BuildIndex(input);
            // std::cout << "构建索引完成..." << std::endl;
            LOG(NORMAL, "构建索引完成...");
        } 

        std::string GetDesc(const std::string& html_content, const std::string& word)
        {
            // 寻找html_content文件中第一次出现word的位置，往前找50字节，往后找100字节
            const int prev_step = 50;
            const int next_step = 100;
            // 1. 寻找word首次出现位置
            auto iter = std::search(html_content.begin(), html_content.end(), word.begin(), word.end(), [](int x, int y){
                return std::tolower(x) == std::tolower(y);
            });
            if (iter == html_content.end()) {
                return "None";
            }
            int pos = std::distance(html_content.begin(), iter);
            // 2. 确定截取起始位置和截止位置，start、end
            int start = 0;
            int end = html_content.size() - 1;
            if (pos - prev_step > start) start = pos - prev_step;
            if (pos + next_step < end) end = pos + next_step;
            // 3. 截取子串返回
            if (start > end) {
                return "None";
            }
            
            return html_content.substr(start, end - start);
        }

        void Search(const std::string& query, std::string* json_string)
        {
            // 1. 分词：对搜索关键字query进行分词
            std::vector<std::string> words;
            ns_util::JiebaUtil::CutString(query, &words);
            // 2. 触发：根据分词结果在Index进行查找
            //ns_index::InvertedList inverted_list_all;
            std::vector<InvertedElemPrint> inverted_list_all;
            std::unordered_map<uint64_t, InvertedElemPrint> tokens_map;

            for (std::string word : words) {
                boost::to_lower(word);
                ns_index::InvertedList* inverted_list = index->GetInvertedList(word);
                if (nullptr == inverted_list) {
                    continue;
                }
                // inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
                // 去重
                for (const auto &elem : *inverted_list) {
                    auto &item = tokens_map[elem.doc_id]; // unordered_map[] 有则获取，无则新建
                    // item是doc_id相同的结点
                    item.doc_id = elem.doc_id;
                    item.weight += elem.weight;
                    item.words.push_back(elem.word); 
                }

            }
            for (const auto &item : tokens_map) {
                inverted_list_all.push_back(std::move(item.second));
            }
            // 3. 合并排序：汇总查找结果，根据相关性(weight)进行降序排序
            // std::sort(inverted_list_all.begin(), inverted_list_all.end(), [](const ns_index::InvertedElem& e1, const ns_index::InvertedElem& e2){
            //     return e1.weight > e2.weight;
            // });
            std::sort(inverted_list_all.begin(), inverted_list_all.end(), [](const InvertedElemPrint& e1, const InvertedElemPrint& e2){
                return e1.weight > e2.weight;
            });
            // 4. 构建：将排好序的结果，生成json串 -- jsoncpp库
            Json::Value root;
            for (auto& item : inverted_list_all) {
                ns_index::DocInfo* doc = index->GetForwardIndex(item.doc_id);
                if (nullptr == doc) {
                    continue;
                }
                Json::Value elem;
                elem["title"] = doc->title;
                elem["desc"] = GetDesc(doc->content, item.words[0]);
                elem["url"] = doc->url;
                elem["weight"] = item.weight;
                elem["id"] = (int)item.doc_id;

                root.append(elem);
            }

            Json::StyledWriter writer;
            *json_string = writer.write(root);
        }
    };
}