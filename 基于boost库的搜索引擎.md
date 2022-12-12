@[TOC](文章目录)
# 项目背景
在互联网时代，搜索引擎是每一个人不可或缺的工具。常见的搜索引擎有百度、搜狗和360搜索等等。
![在这里插入图片描述](https://img-blog.csdnimg.cn/ead7687509a541d4b3c6bee50ccdc246.png)
**在百度中搜索，每一个结果都包含三部分，网页标题，网页内容摘要和即将跳转的url。**



Boost库是C++编程中非常常用的一个库，但是可惜的是Boost库官网并没有内置站内搜索。实现一个基于boost库的站内搜索引擎，做到和常见搜索引擎一样，显示相关内容的搜索结果，包括**网页标题，网页内容摘要和即将跳转的url。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/1a58fb332d21470ba359ed6a2b5674a8.png)

---
# 搜索引擎宏观体系
![在这里插入图片描述](https://img-blog.csdnimg.cn/1911a88f48df4779ba93154aaccdf88f.png)
**客户端通过浏览器向服务器发送请求，服务器通过搜索引擎检索关键字所对应的文件，内存中的文件来源于磁盘文件中的html文件进行去标签和数据清洗后建立索引。**

---

# 技术栈和项目环境
- **技术栈**：```C/C++，STL，Boost库，Jsoncpp，cppjieba，cpp-httplib，html5，css，js，jQuery，Ajax```
- **项目环境**：```CentOS7服务器，vim/gcc/Makefile，vs2019，vscode```

---

# 正排索引和倒排索引
**正排索引：由文档id找到文档内容**

例如：
|文档id|文档内容|
|--|--|
|1| 腾讯今天发布了新游戏 |
| 2 |  小米今天发布了新手机|

文档1：腾讯今天发布了新游戏 --- 腾讯/今天/发布/新游戏
文档2：小米今天发布了新手机 --- 小米/今天/发布/新手机

我们使用搜索引擎进行搜索时，是通过关键词进行搜索的，所以我们就必须对文档进行分词，方便我们进行建立倒排索引。

**停止词(暂停词)：了、的、吗、a、the…一般我们在分词的时候可以不考虑。因为这些词的词频很高，如果我们把这些词保留下来，那他们在搜索时区分唯一性的价值也不大。**


**倒排索引：将文档分词，通过关键词建立和文档id的索引。**

| 关键词（具有唯一性） | 文档id（weight）|
|--|--|
| 腾讯 | 文档1 |
| 小米 |  文档1|
|  今天| 文档1，文档2 |
| 发布 | 文档1，文档2 |
| 新游戏 | 文档1 |
| 新手机 | 文档2 |


**模拟一次用户搜索的过程：**
用户检索: 腾讯 → 倒排索引 → 文档id → 正排索引 → 提取文档内容 → 构建网页标题，网页梗概和url → 构建响应结果
**如果检索 "发布" 关键词，我们发现文档1和文档2都存在，服务器会通过关键字的weight权重进行抉择。**


---

# 编写数据去标签与数据清洗的模块Parser

**下载Boost库数据源：**
>Boost官网：```https://www.boost.org/```

![在这里插入图片描述](https://img-blog.csdnimg.cn/0b6b621600824593a6e978f3bc97f4b0.png)

![在这里插入图片描述](https://img-blog.csdnimg.cn/c3f239affbc14782b782660f467e72e8.png)

下载完毕之后，需要上传到云服务器进行数据处理： ```rz -E```


![在这里插入图片描述](https://img-blog.csdnimg.cn/1bc547b6620e404f8236c5cf307c21b0.png)

**解压Boost库：**```tar xzf boost_1_80_0.tar.gz ```

![在这里插入图片描述](https://img-blog.csdnimg.cn/f1b94f0e887d4421a80423d0263daeac.png)

**我们在网上搜索的内容，实际上是html文件，所以我们搭建搜索引擎，只需要Boost库中的/doc/html中的html文件来建立索引，提取html文件**：```mkdir  -p data/input```
![在这里插入图片描述](https://img-blog.csdnimg.cn/2e7b856612e1434eac5fbd12def35dca.png)

```cp -r boost_1_80_0/doc/html/* data/input ```

![在这里插入图片描述](https://img-blog.csdnimg.cn/f2d6f64a7ffd4c2385f0dab5216b0c34.png)

**去标签**：```touch parser.cc```
编写parser文件对网页信息进行去标签动作，做数据处理数据清洗。
![在这里插入图片描述](https://img-blog.csdnimg.cn/23d92a3826cd4d1ba2326dd1763c0cb7.png)

**什么是标签：**

```css
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html> 
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Chapter 45. Boost.YAP</title>
<link rel="stylesheet" href="../../doc/src/boostbook.css" type="text/css">
<meta name="generator" content="DocBook XSL Stylesheets V1.79.1">
<link rel="home" href="index.html" title="The Boost C++ Libraries BoostBook Documentation Subset">
<link rel="up" href="libraries.html" title="Part I. The Boost C++ Libraries (BoostBook Subset)">
<link rel="prev" href="xpressive/appendices.html" title="Appendices">
<link rel="next" href="boost_yap/manual.html" title="Manual">
</head>
```
**如上述代码，\<html\>\<link\>都是标签，标签会被浏览器渲染成不同的样子，但是对我们来说是没有价值的，所以需要去掉。**

```parser.cc```的工作是将html文件进行数据清洗，将文件中的标签去掉后放置到```raw_html```目录下的```raw.txt```文件下：

![在这里插入图片描述](https://img-blog.csdnimg.cn/c942a3b07c23406fbd52784b5b6b211d.png)


**目标是将每个html文件都进行数据清洗，写入到同一个文件中，每个文档的内容不换行，用特殊字符'\3' 分割。**
![在这里插入图片描述](https://img-blog.csdnimg.cn/7dfbba68479e4cfc939a5e84e3053c8f.png)
**ASCII表中有的字符属于控制字符，是不可显示的。我们获得的文档内容基本都属于打印字符。3对应^C，是不可显示的，所以也就不会污染我们形成的新文档。**

---


## parser代码基本结构
![在这里插入图片描述](https://img-blog.csdnimg.cn/695bd60bbc52414cbe9e4c2272523583.png)


在编写```parser.cc```代码之前，我们首先要**获取待处理数据的路径和保存处理后数据的路径。**

```cpp
// src_path路径保存的是所有待处理的html文件 
// output路径保存处理后数据的文档的路径
const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";
```
**第一步，使用EnumFile函数通过待处理的html文件的路径src_path，递归式的罗列每个html文件+路径保存到files_list中，以便于后序一个一个读取文件。
第二步，解析出了每个html文件和路径，就可以一个一个读取文件，对html文件进行数据清洗和去标签，提取出我们想要的内容，放置到DocInfo结构体数组中保存。
第三步，将数据清洗完和去标签完的数据写入output路径下的raw.txt文件中。**


```parser.cc```代码结构：

```cpp
#include <iostream>
#include <string>
#include <vector>

// src_path路径保存的是所有待处理的html文件 
const std::string src_path = "data/input/";
const std::string output = "data/raw_html/raw.txt";

typedef struct DocInfo
{
    std::string title;     // 网页标题
    std::string content;   // 网页内容摘要
    std::string url;       // 即将跳转的url
}DocInfo_t;

bool EnumFile(const std::string& src_path, std::vector<std::string>* files_list);
bool parserHtml(const std::vector<std::string>& files_list, std::vector<DocInfo_t>* results);
bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output);

int main()
{
    // 1. 递归式的把每个html文件名带路径，保存到files_list，方便一个一个进行读取
    std::vector<std::string> files_list;
    if (!EnumFile(src_path, &files_list)) {
        std::cerr << "enum file name error" << std::endl;
        return 1;
    }
    
    // 2. 按照files_list读取每个文件内容进行解析
    std::vector<DocInfo_t> results;
    if (!parserHtml(files_list, &results)) {
        std::cerr << "parser html error" << std::endl;
        return 2;
    }

    // 3. 把解析完毕的文件内容写入output，\3作为每个文档的分隔符
    if (!SaveHtml(results, output)) {
        std::cerr << "save html error" << std::endl;
        return 3;
    }

    return 0;
}

```

---
## 使用boost编写枚举文件名模块 -- EnumFile()
C++算法和STL对于文件系统的支持不是特别好，所以要完成枚举文件名模块，就需要利用到boost库来处理这个模块。

**boost开发库的安装**：```sudo yum install -y boost-devel```

![在这里插入图片描述](https://img-blog.csdnimg.cn/9c69b261768c45e4bc3c0a2b3a544ea4.png)

**boost库手册查看：**
进入```www.boost.org```官网 → 点击右下角Document  → 选择版本1.53.0 → 选择Filesystem → 选择Tutorial 即可查看boost库Filesystem文件系统的手册教程。
![在这里插入图片描述](https://img-blog.csdnimg.cn/c42b0271dd9c4e0297ad3ed93ec8d5e1.png)

**枚举文件名模块思路**：首先创建起始路径，也就是要递归扫描html文件的路径，第一步先判断起始路径是否存在，若存在，则第二步定义```recursive_directory_iterator```空迭代器作为递归遍历的的结束条件，第三步进行递归遍历筛选文件如果不是普通文件或者文件后缀不是‘.html’ 则```continue```循环继续，第四步将筛选之后的以html为后缀的普通文件```push_back```到存储文件名+路径的files_list容器中。



```cpp
bool EnumFile(const std::string& src_path, std::vector<std::string>* files_list)
{
    namespace fs = boost::filesystem; // boost库命名空间
    fs::path root_path(src_path);     // 起始路径
    // 判断起始路径是否存在
    if (!fs::exists(root_path)) {
        std::cerr << src_path << " not exits" << std::endl;
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
        std::cout << "debug: " << iter->path().string() << std::endl;
        files_list->push_back(iter->path().string());
    }

    return true;
}
```
**Makefile文件：**
因为我们使用了boost库，所以必须链接**boost库和boost文件库**。

```c
cc=g++
parser:parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11
.PHONY:clean
clean:
	rm -rf parser
```

**运行结果：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/8bc2f91437684464bbca4f986b238e77.png)

**利用wc指令比对结果是否正确：**

 - ```boost_searcher/data/input```目录下：```ls -Rl | grep -E '.html' | wc -l```
![在这里插入图片描述](https://img-blog.csdnimg.cn/8ca7dae92e404236b5efd5d8a5886084.png)

 - ```boost_searcher```目录下：```./parser | wc -l```
![在这里插入图片描述](https://img-blog.csdnimg.cn/8690c31c68e74263b688588e97f676aa.png)


---

## 解析html代码结构编写 --- ParseHtml()
当我们提取了每个html文件的文件名和路径后，要做的就是解析html文件内容。

**分为四个步骤**：
 1. 通过循环遍历文件名和路径，一个一个读取文件；
 2. 解析文件，提取title；
 3. 解析文件，提取content；
 4. 解析文件，提取url；


**定义一个工具函数集Util.hpp，用于存放工具函数比如ReadFile()读取文件函数。**

**Util.hpp:**
```cpp
#pragma once
#include <iostream>
#include <string>
#include <fstream>

namespace ns_util{
    class FileUtil{
        public:
            static bool ReadFile(const std::string &file_path, std::string *out)
            {
                std::ifstream in(file_path, std::ios::in);
                if (!in.is_open()) {
                    std::cerr << "open file " << file_path << " error" << std::endl;
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
}

```
**ParseHtml()代码结构**：

```cpp
bool ParseHtml(const std::vector<std::string>& files_list, std::vector<DocInfo_t>* results)
{
    for (const std::string &file : files_list) {
        // 1. 读取文件，ReadFile()
        std::string result;   // 保存当前网页的信息
        if (!ns_util::FileUtil::ReadFile(file, result)) {
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
        if (!ParseUrl(result, &doc.url)) {
            continue;
        }
        results.push_back(doc);
    }
    
    return true;
}
```

---

### 提取title -- ParseTitle()
**html文件的标题为```<title></title>```之间的内容。找到```<title>```的位置和```</title>```，再利用substr()函数切分字符串即可得到标题。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/2dc5cb5ad58641f29e0e97b9640fdb60.png)

**ParseTitle()代码**：

```cpp
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

```

---

### 提取content -- ParseContent()
![在这里插入图片描述](https://img-blog.csdnimg.cn/07dc9d261f224919ad21fd24a04a9ca6.png)
**提取content的本质是去标签，只保留正文部分。当我们遍历html文件时，当遇到```<```时，表示标签开始，遇到```>```时，表示正文开始，直到遇到下一个```<```，表示新的标签开始。**

**ParseContent()代码：**

```cpp
static bool ParseContent(const std::string &file, std::string *Content)
{
	// 定义一个状态机，表示当前的状态
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
```

---

### 提取url -- ParseUrl()

**提取url的本质是拼接url。boost库的官方文档和我们做项目下载下来的文档，是有路径的对应关系的。**

**官网的url：** 
```https://www.boost.org/doc/libs/1_80_0/doc/html/accumulators.html```
```https://www.boost.org/doc/libs/1_80_0/doc/html/any.html```

**项目中html文件路径：**
```data/input/accumulators.html```
```data/input/any.html```

通过观察官网的url我们可以发现url的前面部分是一样的，
即```url_head = https://www.boost.org/doc/libs/1_80_0/doc/html```
而后半部分可以通过提取HTML文件的路径来获取，
即```url_tail = [data/input](删除)    /accumulators.html```
即```url = url_head + url_tail```

**ParseUrl()代码：**

```cpp
static bool ParseUrl(const std::string &file, std::string *url)
{
    std::string url_head = "https://www.boost.org/doc/libs/1_80_0/doc/html";
    std::string url_tail = file.substr(src_path.size()); // 由src_path结尾位置截取到结束
    *url = url_head + url_tail;
    return true;
}
```


---

## 编写写入文件代码 --- SaveHtml()

解析完html文件之后，需要将数据清洗完和去标签完的数据写入output路径下的raw.txt文件中。

**写入格式：**
```title\3content\3url \n title\3content\3url \n title\3content\3 url...```

**SaveHtml()代码：**

```cpp
bool SaveHtml(const std::vector<DocInfo_t>& results, const std::string& output)
{
#define SEP '\3'
    // 以二进制的形式写入
    std::ofstream out(output, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "open " << output << " failed" << std::endl;
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
```
**运行结果：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/c17f23f5502c432a8fb9369f9607acca.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/eba470d1f0b74e89a672a39694803afe.png)


---
# 编写建立索引模块 -- index

##  搭建索引代码结构

处理完html文件的数据后，下一步是构建正排索引和倒排索引。
**在用户搜索关键词后，通过关键词获得倒排拉链，获取到倒排拉链有关的文档，通过文档id获取文档内容。**



**Index.hpp:**

```cpp
#pragma once 
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

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

    class Index
    {
        // 倒排拉链
        typedef std::vector<InvertedElem> InvertedList;
        private:
            // 正排索引，通过数组下标代表文档id
            std::vector<DocInfo> forward_index;
            // 倒排索引，一个关键词与多个InvertedElem(倒排拉链)对应
            std::unordered_map<std::string, InvertedList> inverted_inedx;
        public:
            Index(){};
            ~Index(){};
        public:
            // 根据文档id找到文档内容 -- 正排索引
            DocInfo* GetForwardIndex(uint64_t doc_id) {}
            // 根据关键字找到倒排拉链 -- 倒排索引
            InvertedList* GetInvertedList(const std::string& word) {}
            // 构建正排索引和倒排索引
            bool BuildIndex(const std::string& input) {}
    };
}
```

---

## 获取正排索引 --- GetForwardIndex()
正排索引，通过数组下标代表文档id，获取正排索引就是通过数组下标获取到文档内容。

```cpp
DocInfo* GetForwardIndex(uint64_t doc_id) {
	if (doc_id >= forward_index.size()) {
		std::cerr << "doc_id out range" << std::endl;
        return nullptr;
    }
    return &forward_index[doc_id];
}
```


---

## 获取倒排索引 --- GetInvertedList()
根据关键词，在unordered_map中查找倒排拉链，一个关键词对应多个文档及其文档id和内容即倒排拉链。

```cpp
InvertedList *GetInvertedList(const std::string &word) {
    auto item = inverted_inedx.find(word);
    if (item == inverted_inedx.end()) {
        std::cerr << "get invertedlist error" << std::endl;
        return nullptr;
    }
    return &(item->second);
}
```

---

## 构建正排索引和倒排索引 --- BuildIndex()

思路: 遍历文档的每一行，每一行代表一个html文件内容，为文档构建正排索引再构建倒排索引。



```cpp
// 构建正排索引和倒排索引
bool BuildIndex(const std::string& input) {
	// 读取data/raw.html/raw.txt 的内容
	std::ifstream in(input, std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		std::cerr << "open " << input << " error" << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(in, line)) {
		// 构建正排索引
		DocInfo* doc = BuildForwardIndex(line);
		if (doc == nullptr) {
			std::cerr << "build " << line << "error" << std::endl;
			continue;
		}

		// 构建倒排索引
		BuildInvertedIndex(*doc);

	}
	return true;
}
```

---

### 构建正排索引 -- BuildForwardIndex()
构建正排索引要分为三步：

 1. 解析文档的每一行，利用字符串切分算法切分出title、content和url。
 2. 将切分后的内容填入DocInfo对象。
 3. 将DocInfo插入正排索引forward_index中。

**BuildForwardIndex()代码：**

```cpp
DocInfo* BuildForwardIndex(const std::string& line) {
	// 1. 解析line -> title/content/url 
	std::vector<std::string> results;   // 存放解析后的三段字符串
	const std::string sep = "\3";       // 字符串分隔符
	ns_util::StringTool::CutString(line, &results, sep);

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
```

**CutString函数被放在util.hpp工具集中：**

CutString中运用到了boost库中的切分字符串的函数
引用的头文件 <boost/algorithm/string.hpp>

**boost::split()函数用于切割string字符串，将切割之后的字符串放到一个std::vector\<std::string\> 之中；**

有4个参数：

以boost::split(type, select_list, boost::is_any_of(","), boost::token_compress_on);

(1)、type类型是std::vector<std::string>，用于存放切割之后的字符串

(2)、select_list：传入的字符串，可以为空。

(3)、boost::is_any_of(",")：设定切割符为,(逗号)

(4)、 boost::token_compress_on：将连续多个分隔符当一个，默认为boost::token_compress_off。



```cpp
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace ns_util{
    class StringTool
    {
        public:
            static void CutString(const std::string& line, std::vector<std::string>* out, const std::string& sep)
            {
                boost::split(*out, line, boost::is_any_of(sep), boost::token_compress_on);
            }
    };
}
```

---


### 倒排索引思路

思路：

 1. **得到构建正排索引后的DocInfo.**

```
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

// 倒排索引，一个关键词与多个InvertedElem(倒排拉链)对应
std::unordered_map<std::string, InvertedList> inverted_inedx;

例如：
title：黑化肥
content：黑化肥会发灰，发灰会挥发
url：https://xxx.com
doc_id：12
```
 2. **对title && content进行分词。**
- title：黑化肥/黑/化肥   →（title_word）
- content：黑化肥/会/发灰/发灰/会/挥发   →（content_word）

3. **词频统计。**

构建关键词和文档的相关性，规定出现在标题的词比出现在文档的词相关性更高。

伪代码：
```
// 关键词在文档或者标题出现的频率
struct word_cnt {
	title_cnt;
	content_cnt;
};

// key : value = 关键词 ：词频统计
unorder_map<string, word_cnt> WordCnt;

// 遍历title_word，统计词频
for (auto& word : title_word)
{
	WordCnt[word].title_cnt++;
}

// 遍历content_word，统计词频
for (auto& word : content_word)
{
	WordCnt[word].content_cnt++;
}
```


 4. **根据DocInfo的内容形成一个或多个InvertedElem(倒排拉链)。**

伪代码：
```
for (auto& word : WordCnt)
{
	// 构建关键词 和 文档id的映射关系
	struct InvertedElem Elem;
	Elem.doc_id = 12;
	Elem.word = word.first;

	// 自定义词频统计，设置权重 
	Elem.weight = 10 * word.second.title.cnt + word.second.content.cnt;

	// 关键词构建倒排拉链 -- 一个关键词对应多个文档
	inverted_index[word.first].push_back(Elem);
}
```
---
 
### 使用cppjieba库进行分词
**下载cppjieba库：**```git clone https://github.com/yanyiwu/cppjieba```

![在这里插入图片描述](https://img-blog.csdnimg.cn/1105876c81154d199693e4bd2256e7d4.png)
**下载cppjieba库后，需要将```cppjieba/deps/limonp/ ```的文件拷贝到```cppjieba/include/cppjieba/ ```目录下，否则会编译报错。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/0617d24cf81549048df01879c23ff6e8.png)

**测试cppjieba分词：**

**添加软链接：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/623dc9ccc5524b2aa0d956a1ead07160.png)

**demo.cpp:**

```cpp
#include "inc/cppjieba/Jieba.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

int main(int argc, char** argv) {
  cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
  vector<string> words;
  vector<cppjieba::Word> jiebawords;
  string s;
  string result;

  s = "小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
  cout << s << endl;
  cout << "[demo] CutForSearch" << endl;
  jieba.CutForSearch(s, words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

 
  return EXIT_SUCCESS;
}
```

**运行结果：**
	
![在这里插入图片描述](https://img-blog.csdnimg.cn/c64a226ae89f449fa9707bed1af33f68.png)

---
### 引入cppjieba库到项目

**cppjieba库放在third_part目录下：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/e6a7775959104374969542ae8f628f44.png)


**为boost_searcher目录建立软链接：**


![在这里插入图片描述](https://img-blog.csdnimg.cn/6f9a3b220b6c45ab9dc54af3f92970aa.png)
**util.hpp:**

```cpp
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "cppjieba/Jieba.hpp"

namespace ns_util{

    const char* const DICT_PATH = "./dict/jieba.dict.utf8";
    const char* const HMM_PATH = "./dict/hmm_model.utf8";
    const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
    const char* const IDF_PATH = "./dict/idf.utf8";
    const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";
    class JiebaUtil
    {
    private:
        static cppjieba::Jieba jieba;
    public:
        static void CutString(const std::string& src, std::vector<std::string>* out)
        {
            jieba.CutForSearch(src, *out);
        }
    };
    cppjieba::Jieba JiebaUtil::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
}

```


---

### 构建倒排索引 --- BuildInvertedIndex()
构建倒排索引分为三步：

1. 将文档标题和内容分词。
2. 将分词后结果转化为小写并进行词频统计。
3. 根据文档形成倒排拉链。

**BuildInvertedIndex()代码：**

```cpp
bool BuildInvertedIndex(const DocInfo& doc) {
	struct word_cnt {
		int title_cnt;
		int content_cnt;

		word_cnt() : title_cnt(0), content_cnt(0) {}
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

		InvertedList & inverted_list = inverted_list[word_pair.first];
		inverted_list.push_back(std::move(item));
	}

	return true;
}
```

---
## 构造Index单例
当我们在获取索引时，如果每次都从头开始创建索引，就会造成资源的浪费。所以我们可以将```Index```类改成单例，保证只有一份```Index```对象。

```cpp
class Index
{
private:
	static Index* instance;
	static std::mutex mtx;

	Index();
	Index(const Index&) = delete;
	Index& operator=(const Index&) = delete;

public:
	Index* Getinstance()
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
};

Index* Index::instance = nullptr;
std::mutex Index::mtx;
```


----

# 编写搜索引擎模块 -- searcher

## 搭建搜索引擎代码结构
**searcher.hpp:**

```cpp
#include "index.hpp"

namespace ns_searcher {
    class Searcher{
    private:
        ns_index::Index* index;
    public:
        void InitSearcher(const std::string& input)
        {
            // 1. 创建Index单例对象
            // 2. 根据Index对象构建索引
        } 

        void Search(const std::string& query, std::string* json_string)
        {
            // 1. 分词：对搜索关键字query进行分词
            // 2. 触发：根据分词结果在Index进行查找
            // 3. 合并排序：汇总查找结果，根据相关性(weight)进行降序排序
            // 4. 构建：将排好序的结果，生成json串 -- jsoncpp库
        }
    };
}
```


---
## 初始化搜索引擎模块searcher

**初始化分为两个步骤：**

1. 创建```Index```单例对象，保证只有一份```Index```对象
2. 根据```Index```对象构建索引

```cpp
void InitSearcher(const std::string& input)
{
    // 1. 创建Index单例对象
    index = ns_index::Index::GetInstance();
    std::cout << "创建Index单例完成..." << std::endl;
    
    // 2. 根据Index对象构建索引
    index->BuildIndex(input);
    std::cout << "构建索引完成..." << std::endl;
} 
```

---

## 查找关键词模块 -- Search()
**查找模块分为四步：**

1. **分词**：对搜索关键字```query```进行分词，存放在```vector```数组中。
2. **触发**：根据分词结果在Index进行查找，首先定义一个```InvertedList```倒排拉链```inverted_list_all```，用于存放关键词分词后所有词对应的文档信息```InvertedElem```，第二步遍历关键词分词后的结果，对每一个词进行倒排索引，找到该词对应的倒排拉链```inverted_list```，最后将```inverted_list```倒排拉链中的所有```InvertedElem```插入到```inverted_list_all```中。
3. **合并排序**：汇总查找结果，根据相关性(```weight```)进行降序排序
4. **构建**：将排好序的结果，使用```jsoncpp```库生成```json```串，由于```content```内容过多，在搜索结果中不好全部展示，所以定义了函数```GetDesc()```对```content```进行概要切分。

**Search()代码：**

```cpp
void Search(const std::string& query, std::string* json_string)
{
	// 1. 分词：对搜索关键字query进行分词
	std::vector<std::string> words;
	ns_util::JiebaUtil::CutString(query, &words);
	// 2. 触发：根据分词结果在Index进行查找
	ns_index::InvertedList inverted_list_all;
	for (std::string word : words) {
		boost::to_lower(word);
		ns_index::InvertedList* inverted_list = index->GetInvertedList(word);
		if (nullptr == inverted_list) {
			continue;
		}
		inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end());
	}
	// 3. 合并排序：汇总查找结果，根据相关性(weight)进行降序排序
	std::sort(inverted_list_all.begin(), inverted_list_all.end(), [](const ns_index::InvertedElem& e1, const ns_index::InvertedElem& e2){
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
		elem["desc"] = GetDesc(doc->content, item.word);
		elem["url"] = doc->url;
		// elem["weight"] = item.weight;
		// elem["id"] = (int)item.doc_id;

		root.append(elem);
	}

	Json::StyledWriter writer;
	*json_string = writer.write(root);
}
```

---

**下载jsoncpp库：**```sudo yum install -y jsoncpp-devel```

![在这里插入图片描述](https://img-blog.csdnimg.cn/b8b58f736e7a46edaee479986921fc9c.png)

**使用jsoncpp库必须包含头文件：```#include <jsoncpp/json/json.h>```
并且在编译的时候必须包含选项：```-ljsoncpp```**

---

**GetDesc()函数：**
思路：

1. 寻找```html_content```文件中第一次出现```word```的位置，并记录该位置为```pos```。
2. 确定截取的起始位置```start```和结束位置```end```。
3. 使用```substr()```截取子串返回。


```cpp
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
```


---


# 编写http调用模块 -- http_server

## 准备工作 -- 安装cpp-httplib库

cpp-httplib 在使用的时候需使用较新的gcc，否则会编译出错。
centos 7 默认的gcc版本是4.8.5，是比较老的版本，我们需要升级gcc。

![在这里插入图片描述](https://img-blog.csdnimg.cn/c4f68f1a7c3447018bc3099f454d7b56.png)

**安装scl： ```sudo yum install centos-release-scl scl-utils-build```**


**安装devtoolset-7：```sudo yum install -y devtoolset-7-gcc-c++```**

**启动devtoolset：```scl enable devtoolset-7 bash```**

**启动后服务器的gcc版本就变成了7.3.1了，但是此次版本变化只在本次会话有效，可以在需要使用```cpp-httplib```手动升级，但是如果想永久有效，就可以在文件 ```~/.bash_profile```中添加语句```scl enable devtoolset-7 bash```**


![在这里插入图片描述](https://img-blog.csdnimg.cn/073d42c28b224afeac18ce76c93fb49a.png)

**建立软链接：```ln -s ~/third_part/cpp-httplib-0.7.15 cpp-httplib```**

![在这里插入图片描述](https://img-blog.csdnimg.cn/7106320c6bee4666b240a9f11900121a.png)

---
## http调用模块代码结构

```cpp
#include "searcher.hpp"
#include "cpp-httplib/httplib.h"

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
        std::cout << "用户搜索词: " << word << std::endl;
        std::string json_string;
        search.Search(word, &json_string);
        resp.set_content(json_string, "application/json");
    });
    server.listen("0.0.0.0", 8081);
    
    return 0;
}

```


---

# 编写前端模块

**wwwroot目录下index.html：**

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="http://code.jquery.com/jquery-2.1.1.min.js"></script>

    <title>boost 搜索引擎</title>
    <style>
        /* 去掉网页中的所有的默认内外边距，html的盒子模型 */
        * {
            /* 设置外边距 */
            margin: 0;
            /* 设置内边距 */
            padding: 0;
        }
        /* 将我们的body内的内容100%和html的呈现吻合 */
        html,
        body {
            height: 100%;
        }
        /* 类选择器.container */
        .container {
            /* 设置div的宽度 */
            width: 800px;
            /* 通过设置外边距达到居中对齐的目的 */
            margin: 0px auto;
            /* 设置外边距的上边距，保持元素和网页的上部距离 */
            margin-top: 15px;
        }
        /* 复合选择器，选中container 下的 search */
        .container .search {
            /* 宽度与父标签保持一致 */
            width: 100%;
            /* 高度设置为52px */
            height: 52px;
        }
        /* 先选中input标签， 直接设置标签的属性，先要选中， input：标签选择器*/
        /* input在进行高度设置的时候，没有考虑边框的问题 */
        .container .search input {
            /* 设置left浮动 */
            float: left;
            width: 600px;
            height: 50px;
            /* 设置边框属性：边框的宽度，样式，颜色 */
            border: 1px solid black;
            /* 去掉input输入框的有边框 */
            border-right: none;
            /* 设置内边距，默认文字不要和左侧边框紧挨着 */
            padding-left: 10px;
            /* 设置input内部的字体的颜色和样式 */
            color: #CCC;
            font-size: 14px;
        }
        /* 先选中button标签， 直接设置标签的属性，先要选中， button：标签选择器*/
        .container .search button {
            /* 设置left浮动 */
            float: left;
            width: 150px;
            height: 52px;
            /* 设置button的背景颜色，#4e6ef2 */
            background-color: #4e6ef2;
            /* 设置button中的字体颜色 */
            color: #FFF;
            /* 设置字体的大小 */
            font-size: 19px;
            font-family:Georgia, 'Times New Roman', Times, serif;
        }
        .container .result {
            width: 100%;
        }
        .container .result .item {
            margin-top: 15px;
        }

        .container .result .item a {
            /* 设置为块级元素，单独站一行 */
            display: block;
            /* a标签的下划线去掉 */
            text-decoration: none;
            /* 设置a标签中的文字的字体大小 */
            font-size: 20px;
            /* 设置字体的颜色 */
            color: #4e6ef2;
        }
        .container .result .item a:hover {
            text-decoration: underline;
        }
        .container .result .item p {
            margin-top: 5px;
            font-size: 16px;
            font-family:'Lucida Sans', 'Lucida Sans Regular', 'Lucida Grande', 'Lucida Sans Unicode', Geneva, Verdana, sans-serif;
        }

        .container .result .item i{
            /* 设置为块级元素，单独站一行 */
            display: block;
            /* 取消斜体风格 */
            font-style: normal;
            color: green;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="search">
            <input type="text" value="请输入搜索关键字">
            <button onclick="Search()">搜索一下</button>
        </div>
        <div class="result">
            <!-- 动态生成网页内容 -->
        </div>
    </div>
    <script>
        function Search(){
            // 是浏览器的一个弹出框
            // alert("hello js!");
            // 1. 提取数据, $可以理解成就是JQuery的别称
            let query = $(".container .search input").val();
            console.log("query = " + query); //console是浏览器的对话框，可以用来进行查看js数据

            //2. 发起http请求,ajax: 属于一个和后端进行数据交互的函数，JQuery中的
            $.ajax({
                type: "GET",
                url: "/s?word=" + query,
                success: function(data){
                    console.log(data);
                    BuildHtml(data);
                }
            });
        }

        function BuildHtml(data){
            // 获取html中的result标签
            let result_lable = $(".container .result");
            // 清空历史搜索结果
            result_lable.empty();

            for( let elem of data){
                // console.log(elem.title);
                // console.log(elem.url);
                let a_lable = $("<a>", {
                    text: elem.title,
                    href: elem.url,
                    // 跳转到新的页面
                    target: "_blank"
                });
                let p_lable = $("<p>", {
                    text: elem.desc
                });
                let i_lable = $("<i>", {
                    text: elem.url
                });
                let div_lable = $("<div>", {
                    class: "item"
                });
                a_lable.appendTo(div_lable);
                p_lable.appendTo(div_lable);
                i_lable.appendTo(div_lable);
                div_lable.appendTo(result_lable);
            }
        }
    </script>
</body>
</html>

```


---


# 整体测试
**项目框架：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/6a64bcbae171455d99822125b2cbe19c.png)
**makefile文件：**

```c
PARSER = parser
DUG = debug
HTTP_SERVER = http_server
.PHONY:all
all: $(PARSER) $(DUG) $(HTTP_SERVER)

cc=g++
$(PARSER):parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11
$(DUG):debug.cc
	$(cc) -o $@ $^ -ljsoncpp -std=c++11
$(HTTP_SERVER):http_server.cc
	$(cc) -o $@ $^ -ljsoncpp -lpthread -std=c++11
.PHONY:clean
clean:
	rm -rf $(PARSER) $(DUG) $(HTTP_SERVER)
```

**测试：**

**先执行```./parser```进行数据清洗和去标签操作。
再执行```./http_server```启动服务。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/8d03100c1c0848f687106c6c23a713a2.png)


---

# 解决搜索重复文件问题
当我们在建立倒排索引时，用户输入的词被分成多个词，这些词有可能同时索引到同一个文档id，这样会造成搜索结果重复的现象。

**测试：
我们新建test.html文件放在data/input目录下：**

```html
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
  <head>
  <!-- Copyright (C) 2002 Douglas Gregor <doug.gregor -at- gmail.com>

      Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
      http://www.boost.org/LICENSE_1_0.txt) -->
    <title>测试文件</title>
    <meta http-equiv="refresh" content="0; URL=http://www.boost.org/doc/libs/master/doc/html/signals.html">
  </head>
  <body>
    这是一个测试用例
    <a href="http://www.boost.org/doc/libs/master/doc/html/signals.html">http://www.boost.org/doc/libs/master/doc/html/signals.html</a>
  </body>
</html>

```
**重新编译：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/82b1535d63ea4d729b53db5c23331b82.png)

**改进后：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/a75668bec1474c6f81b9d62c50b8e1b1.png)


---

# 编写添加日志模块 -- log
**log.hpp:**

```html
#pragma once 
#include <iostream>
#include <string>
#include <ctime>

#define NORMAL  1
#define WARNING 2
#define DEBUG   3
#define FATAL   4

#define LOG(LEVEL, MESSAGE) log(#LEVEL, MESSAGE, __FILE__, __LINE__);

void log (std::string level, std::string message, std::string file, int line) 
{
    std::cout << "[" << level << "][" << time(nullptr) << "][" << message << "][" << file <<  ": " << line << "]" << std::endl;  
}

```

---
# 去掉暂停词
**当我们在对文档进行分词的时候，会将一些 "the", "is" 等等的词也分到，在搜索的时候，也会展示这些暂停词的搜索结果，所以在构建索引的时候可以把暂停词对应的索引删除，达到去掉暂停词的效果。**

**在dict词库中有暂停词词库：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/b3b4bfffa38c4a40a6489c724156d74b.png)
**可以在分词后，再读取该暂停词库，把暂停词对应的索引删除：**

**util.hpp:**

```cpp
namespace ns_util
{
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
```


---

# 在Linux部署服务
在后台启动服务，并将日志信息写入log目录下的log.txt:

```nohup  ./http_server &>log/log.txt 2>&1```
![在这里插入图片描述](https://img-blog.csdnimg.cn/e5f35679797a42859b5832502beaa938.png)

```netstat -nltp```查看端口状态：
![在这里插入图片描述](https://img-blog.csdnimg.cn/0b543960376f43929a6a3824d852a459.png)
可以看到项目已经部署上去。


---

# 项目代码

项目代码仓库：[boost搜索引擎](https://github.com/weixiangchen1/boost_searcher)

---
