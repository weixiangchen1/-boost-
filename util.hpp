#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "cppjieba/Jieba.hpp"
#include "log.hpp"

namespace ns_util{
    class FileUtil{
        public:
            static bool ReadFile(const std::string &file_path, std::string *out)
            {
                std::ifstream in(file_path, std::ios::in);
                if (!in.is_open()) {
                    // std::cerr << "open file " << file_path << " error" << std::endl;
                    LOG(FATAL, "open file " + file_path + " error");
                    return false;
                }
                
                std::string line;
                while (std::getline(in, line)) {
                    *out += line;
                }

                in.close();
                return true;
            }
    };

    class StringTool
    {
        public:
            static void Split(const std::string& line, std::vector<std::string>* out, const std::string& sep)
            {
                boost::split(*out, line, boost::is_any_of(sep), boost::token_compress_on);
            }
    };

    const char* const DICT_PATH = "./dict/jieba.dict.utf8";
    const char* const HMM_PATH = "./dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
    const char* const IDF_PATH = "./dict/idf.utf8";
    const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";
    class JiebaUtil
    {
    private:
        // static cppjieba::Jieba jieba;
        cppjieba::Jieba jieba;
        std::unordered_map<std::string, bool> stop_words;

    private:
        JiebaUtil() : jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH) 
        {}
        JiebaUtil(const JiebaUtil&) = delete;
        JiebaUtil& operator=(const JiebaUtil&) = delete;

    public:
        static JiebaUtil* instance;
        static JiebaUtil* GetInstance() 
        {
            static std::mutex mtx;
            if (nullptr == instance) {
                mtx.lock();
                if (nullptr == instance) {
                    instance = new JiebaUtil();
                    instance->InitJiebaUtil();
                }
                mtx.unlock();
            }

            return instance;
        }

    public:
        void InitJiebaUtil()
        {
            std::ifstream in(STOP_WORD_PATH);
            if (!in.is_open()) {
                LOG(FATAL, "open stop words file failed");
                return;
            }
            std::string line;
            while (getline(in, line)) {
                stop_words.insert({line, true});
            }

            in.close();
        }

        void CutStringHelper(const std::string& src, std::vector<std::string>* out)
        {
            jieba.CutForSearch(src, *out);
            for (auto iter = out->begin(); iter != out->end(); ) {
                auto it = stop_words.find(*iter);
                if (it != stop_words.end()) {
                    iter = out->erase(iter);
                }
                else {
                    iter++;
                }
            }
        }

    public:
        static void CutString(const std::string& src, std::vector<std::string>* out)
        {
            // jieba.CutForSearch(src, *out);
            ns_util::JiebaUtil::GetInstance()->CutStringHelper(src, out);
        }
    };
    JiebaUtil* JiebaUtil::instance = nullptr;
}
