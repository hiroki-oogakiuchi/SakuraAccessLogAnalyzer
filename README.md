# SakuraAccessLogAnalyzer header-only C++ library

　さくらインターネットのレンタルサーバーのアクセスログより、各ページのアクセス数を集計する、C++のヘッダーオンリーライブラリです。

　ライブラリの呼び出し方法は、sample.cppを参照してください。

## 集計対象ページ

- 管理画面の[サーバステータス]-[アクセスログ]-[アクセスログ設定]-[ホスト名の情報]は、「保存しない」
- URLの末尾は、「/」または「.zip」
- HTTP/1.1
- 参照元サイトは、https

## ビルド要件

- C++17以降
- [zlib](https://zlib.net/)
- [gzip-hpp](https://github.com/mapbox/gzip-hpp)

　zlib、gzip-hppの導入手順は、「[【C++】gzip形式にて、ファイルを圧縮・解凍する](https://hirokio.jp/programming/cpp-gzip/)」にて解説しています。

## 作者

[大垣内弘樹 (Hiroki Oogakiuchi)](https://hirokio.jp/)
