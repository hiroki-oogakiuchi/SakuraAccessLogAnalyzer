#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>

#include <gzip/decompress.hpp>
#include <gzip/utils.hpp>

namespace sala
{
    // ページ毎のアクセス数
    struct page
    {
        // 各ページのURL
        std::string url;

        // アクセス数合計(A)
        // (A)=(B)+(C)
        int count = 0;

        // 参照元サイト毎のアクセス数(B)
        std::unordered_map<std::string, int> siteCount;

        // その他のアクセス数(C)
        int others = 0;
    };

    class SakuraAccessLogAnalyzer
    {
    private:
        // カウント対象ログの正規表現
        std::regex rePage{R"([\w\.-]+ - - \[([\w/: \+-]+)\] "GET ([^"]*(/|\.zip)) HTTP/1\.1" 200 [\d-]+ "https://([^"]*)\" "[^"]*")"};

        // 行の解析
        void getlineCommon(const std::string &line, const std::vector<std::string> &refersite, std::vector<page> &vpage)
        {
            std::smatch s;

            if (std::regex_match(line, s, rePage))
            {
                // 既にカウント済みのページを検索
                auto itp = std::find_if(vpage.begin(), vpage.end(), [&s](page &p){return p.url == s.str(2);});

                // 既にカウント済みのサイトを検索
                auto its = std::find_if(refersite.begin(), refersite.end(), [&s](const std::string &sitename){
                    return sitename.size() <= s.str(4).size() && std::equal(std::begin(sitename), std::end(sitename), std::begin(s.str(4)));});

                if (itp == vpage.end())
                {
                    // ページ該当無し
                    // 新規にベクターに追加
                    page p;
                    p.url = s.str(2);
                    p.count = 1;

                    for (std::size_t i = 0; i < refersite.size(); i++)
                    {
                        p.siteCount.emplace(refersite[i], 0);
                    }

                    if (its == refersite.end())
                    {
                        // サイト該当無し
                        p.others = 1;
                    }
                    else
                    {
                        // サイト該当有り
                        p.siteCount[its->c_str()] = 1;
                    }

                    vpage.push_back(p);
                }
                else
                {
                    // ページ該当有り
                    // カウントを増やす
                    itp->count++;

                    if (its == refersite.end())
                    {
                        itp->others++;
                    }
                    else
                    {
                        itp->siteCount[its->c_str()]++;
                    }
                }
            }
        }

        // ファイルオープン
        void open(const std::string &filename, const std::vector<std::string> &refersite, std::vector<page> &vpage)
        {
            std::ifstream ifs(filename, std::ios_base::binary);
            std::filesystem::path pa = filename;
            std::string line;

            if (pa.extension() == ".gz")
            {
                // gzipファイル
                std::size_t size = std::filesystem::file_size(filename);
                char *pointer = new char[size];
                ifs.read(pointer, size);

                if (gzip::is_compressed(pointer, size))
                {
                    // 解凍
                    std::string decompressed_data = gzip::decompress(pointer, size);

                    delete[] pointer;

                    std::istringstream iss(decompressed_data);

                    while (std::getline(iss, line))
                    {
                        getlineCommon(line, refersite, vpage);
                    }
                }
                else
                {
                    delete[] pointer;
                    ifs.close();
                    throw std::runtime_error(filename + " is not gzip file.");
                }
            }
            else
            {
                // テキストファイル
                while (std::getline(ifs, line))
                {
                    getlineCommon(line, refersite, vpage);
                }
            }

            ifs.close();
        }

    public:
        // ファイル読み込み
        void read(const std::string &filename, const std::vector<std::string> &refersite, std::vector<page> &vpage)
        {
            if (std::filesystem::is_regular_file(filename))
            {
                open(filename, refersite, vpage);
            }
            else if (std::filesystem::is_directory(filename))
            {
                // 引数のフォルダを再帰的に検索
                for (const std::filesystem::directory_entry &i : std::filesystem::recursive_directory_iterator(filename))
                {
                    // ファイルのみ
                    if (i.is_regular_file())
                    {
                        open(i.path().string(), refersite, vpage);
                    }
                }
            }
            else
            {
                throw std::runtime_error(filename + " error");
            }

            // 集計結果をソート
            std::sort(vpage.begin(), vpage.end(), [&refersite](page &a, page &b){

                    // アクセス数は降順
                    if (a.count != b.count)
                    {
                        return a.count > b.count;
                    }

                    // 参照元サイト毎のアクセス数は降順
                    // 優先順位は登録順
                    for (std::size_t i = 0; i < refersite.size(); i++)
                    {
                        if (a.siteCount[refersite[i]] != b.siteCount[refersite[i]])
                        {
                            return a.siteCount[refersite[i]] > b.siteCount[refersite[i]];
                        }
                    }

                    // その他のアクセス数は降順
                    if (a.others != b.others)
                    {
                        return a.others > b.others;
                    }

                    // URLは昇順
                    return a.url < b.url;
                });
        }
    };

    inline std::vector<page> logcount(const std::string &filename, const std::vector<std::string> &refersite = {})
    {
        SakuraAccessLogAnalyzer sakura;
        std::vector<page> vpage;
        sakura.read(filename, refersite, vpage);
        return vpage;
    }
}
