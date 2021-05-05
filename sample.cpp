#include <iostream>

#include <SakuraAccessLogAnalyzer.hpp>

using namespace std;

int main(int argc, char *argv[])
{
#if 1
    // 参照元サイト
    // 「https://」以降を登録
    // 前方一致
    // サイト数は任意
    vector<string> s;

    s.push_back("www.google."); // comもco.jpもカウント
    s.push_back("www.bing.com");
    s.push_back("search.yahoo.co.jp");
    s.push_back("hirokio.jp"); // 自サイト登録により、サイト内の移動をカウント

    // ログファイル名は、テキストファイル、gzファイル(自動的に解凍)、フォルダ(配下のログを集計)のいずれも可
    vector<sala::page> vp = sala::logcount(argv[1], s);

    // CSV形式にて結果出力
    for (const sala::page &p : vp)
    {
        cout << p.url << ","
            << p.count << ","
            << p.siteCount.at(s[0]) << ","
            << p.siteCount.at(s[1]) << ","
            << p.siteCount.at(s[2]) << ","
            << p.siteCount.at(s[3]) << ","
            << p.others << endl;
    }
#else
    // 参照元サイトを登録しない場合、各ページのカウント数のみ集計
    vector<sala::page> vp = sala::logcount(argv[1]);

    // CSV形式にて結果出力
    for (const sala::page &p : vp)
    {
        cout << p.url << "," << p.count << endl;
    }
#endif
    return 0;
}
