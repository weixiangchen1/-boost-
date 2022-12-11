#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "util.hpp"
#include "log.hpp"


// src_path路径保存的是所有待处理的html文件 
const std::string src_path = "data/input";
const std::string output = "data/raw_html/raw.txt";

typedef struct DocInfo
{
    std::string title;     // 网页标题
    std::string content;   // 网页内容摘要
    std::string url;       // 即将跳转的url
}DocInfo_t;

bool EnumFile(const std::string& src_path, std::vector<std::string>* files_list);
bool ParseHtml(const std::vector<std::string>& files_list, std::vector<DocInfo_t>* results);
bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output);
static bool ParseTitle(const std::string &file, std::string *title);
static bool ParseContent(const std::string &file, std::string *Content);
static bool ParseUrl(const std::string &file, std::string *url);

int main()
{
    // 1. 递归式的把每个html文件名带路径，保存到files_list，
    // 方便一个一个进行读取
    std::vector<std::string> files_list;
    if (!EnumFile(src_path, &files_list)) {
        // std::cerr << "enum file name error" << std::endl;
        LOG(FATAL, "enum file name error");
        return 1;
    }
    
    // 2. 按照files_list读取每个文件内容进行解析
    std::vector<DocInfo_t> results;
    if (!ParseHtml(files_list, &results)) {
        // std::cerr << "parser html error" << std::endl;
        LOG(FATAL, "parser html error");
        return 2;
    }

    // 3. 把解析完毕的文件内容写入output，\3作为每个文档的分隔符
    if (!SaveHtml(results, output)) {
        // std::cerr << "save html error" << std::endl;
        LOG(FATAL, "save html error");
        return 3;
    }

    return 0;
}

bool EnumFile(const std::string& src_path, std::vector<std::string>* files_list)
{
    namespace fs = boost::filesystem; // boost库命名空间
    fs::path root_path(src_path);     // 起始路径
    // 判断起始路径是否存在
    if (!fs::exists(root_path)) {
        // std::cerr << src_path << " not exits" << std::endl;
        LOG(FATAL, src_path + " not exits");
        return false;
    }
    
    // 定义一个空迭代器判断递归结束
    fs::recursive_directory_iterator end;
    for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++) {
        // 递归遍历所有文件
        // 判断文件是否是普通文件，html是普通文件
        if (!fs::is_regular_file(*iter)) {
            continue;
        }
        // 判断文件名后缀是否为html
        if (iter->path().extension() != ".html") {
            continue;
        }
        //std::cout << "debug: " << iter->path().string() << std::endl;
        files_list->push_back(iter->path().string());
    }

    return true;
}

void showDoc(DocInfo_t doc)
{
    std::cout << "title: " << doc.title << std::endl;
    std::cout << "content: " << doc.content << std::endl;
    std::cout << "url: " << doc.url << std::endl;
}

bool ParseHtml(const std::vector<std::string>& files_list, std::vector<DocInfo_t>* results)
{
    for (const std::string &file : files_list) {
        // 1. 读取文件，ReadFile()
        std::string result;   // 保存当前网页的信息
        if (!ns_util::FileUtil::ReadFile(file, &result)) {
            continue;
        }
        DocInfo_t doc;
        // 2. 解析文件，提取title
        if (!ParseTitle(result, &doc.title)) {
            continue;
        }
        // 3. 解析文件，提取content
        if (!ParseContent(result, &doc.content)) {
            continue;
        }
        // 4. 解析文件，提取url
        if (!ParseUrl(file, &doc.url)) {
            continue;
        }
        //showDoc(doc);
        results->push_back(std::move(doc));
        
    }
    return true;
}

static bool ParseTitle(const std::string &file, std::string *title)
{
    size_t begin = file.find("<title>");
    if (begin == std::string::npos) {
        return false;
    }
    size_t end = file.find("</title>");
    if (end == std::string::npos) {
        return false;
    }

    begin += std::string("<title>").size();
    if (begin > end) {
        return false;
    }
    *title = file.substr(begin, end - begin);
    return true;
}

static bool ParseContent(const std::string &file, std::string *Content)
{
    enum status{
        LABLE,
        CONTENT
    };

    enum status s = LABLE;
    for (char c:file) {
        switch(s) {
            case LABLE: 
                if (c == '>') s = CONTENT;
                break; 
            case CONTENT:
                if (c == '<') s = LABLE;
                else {
                    if (c == '\n') c = ' ';
                    Content->push_back(c);
                }
                break; 
            default: break;
        }
    }
    
    return true;
}

static bool ParseUrl(const std::string &file, std::string *url)
{
    std::string url_head = "https://www.boost.org/doc/libs/1_80_0/doc/html";
    std::string url_tail = file.substr(src_path.size()); // 由src_path结尾位置截取到结束
    *url = url_head + url_tail;
    return true;
}

bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output)
{
#define SEP '\3'
    // 以二进制的形式写入
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        // std::cerr << "open " << output << " failed" << std::endl;
        LOG(FATAL, "open " + output + " failed");
        return false;
    }
    for (auto& item:results) {
        std::string out_string;
        out_string = item.title;
        out_string += SEP;
        out_string += item.content;
        out_string += SEP;
        out_string += item.url;
        out_string += '\n';

        out.write(out_string.c_str(), out_string.size());
    }
    out.close();

    return true;
}
