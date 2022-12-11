#include "searcher.hpp"
#include "cpp-httplib/httplib.h"
#include "log.hpp"

const std::string root_path = "./wwwroot";
const std::string input = "./data/raw_html/raw.txt";

int main()
{
    // 初始化搜索模块
    ns_searcher::Searcher search;
    search.InitSearcher(input);

    httplib::Server server;
    server.set_base_dir(root_path.c_str());
    server.Get("/s", [&search](const httplib::Request& req, httplib::Response& resp){
        // 如果用户没有提交搜索词
        if (!req.has_param("word")) {
            resp.set_content("请输入文本或关键词!", "text/plain; charset=UTF-8");
            return;
        }
        std::string word = req.get_param_value("word");
        // std::cout << "用户搜索词: " << word << std::endl;
        LOG(NORMAL, "用户搜索关键词: " + word);
        std::string json_string;
        search.Search(word, &json_string);
        resp.set_content(json_string, "application/json");
    });
    server.listen("0.0.0.0", 8081);
    
    return 0;
}
