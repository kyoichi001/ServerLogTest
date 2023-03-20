# 説明

コードの詳細な仕様と、コードテストの説明について記載致します。

## 定義
- 故障期間について
    - 開始が最初にpingがタイムアウトしたとき、終了が次にpingの応答が返るまでのことを指します。
    - タイムアウトしたままログが終了していた場合、故障期間の終了日時は-1としたものを出力します。
- 過負荷状態について
    - 直近m回の平均応答時間がtミリ秒を超えた場合、最初にその状態になってから次のpingで解消されるまでのことを指します。
    - 直近m回のpingの中にタイムアウトしたものがある場合、故障状態であり、過負荷状態でもあるとみなし、平均応答時間の計算時は閾値であるtミリ秒として扱います。
    - タイムアウトしたままログが終了していた場合、過負荷状態の期間の終了日時は-1としたものを出力します。
- IPアドレスについて
    - 一定間隔でpingを複数サーバーに送信しているため、サーバーのアドレスもリクエストに対して一定の順番であると仮定します。そのため、入力データに登場するアドレスは順番が固定とします。

## 設計
想定される入力は、設問毎に以下の通りとなっております。

### 設問1
```
設問番号Q
データ
```
### 設問2
```
設問番号Q
閾値N
データ
```
### 設問3, 4
```
設問番号Q
閾値N
直近回数m 平均応答時間t
データ
```

また、出力の仕様は以下の通りです。
### 設問1
```
故障状態のサーバーアドレス1 故障期間1開始 故障期間1終了
故障状態のサーバーアドレス2 故障期間2開始 故障期間2終了
...
```
### 設問2
```
故障状態のサーバーアドレス1 故障期間1開始 故障期間1終了
故障状態のサーバーアドレス2 故障期間2開始 故障期間2終了
...
```
### 設問3
```
故障状態のサーバーアドレス1 故障期間1開始 故障期間1終了
故障状態のサーバーアドレス2 故障期間2開始 故障期間2終了
...
===
サーバー1のアドレス 過負荷時間1開始 過負荷時間1終了
サーバー2のアドレス 過負荷時間1開始 過負荷時間1終了
...
```
### 設問4
```
故障状態のサーバーアドレス1 故障期間1開始 故障期間1終了
故障状態のサーバーアドレス2 故障期間2開始 故障期間2終了
...
===
サーバー1のアドレス 過負荷時間1開始 過負荷時間1終了
サーバー2のアドレス 過負荷時間1開始 過負荷時間1終了
...
```

### 具体例1
設問1の入力の場合、以下のような入力例が考えられます。
```
1
20201019133124,10.20.30.1/16,2
20201019133125,10.20.30.1/16,2
20201019133126,10.20.30.1/16,-
20201019133127,10.20.30.1/16,-
20201019133128,10.20.30.1/16,2
```
10.20.30.1/16について、タイムアウトが20201019133126から発生し、20201019133128で次の応答が帰ったため、故障期間を求めると以下の通りとなります。
```
10.20.30.1/16 20201019133126 20201019133128
```

### 具体例2
設問2の入力の場合、以下のような入力例が考えられます。
```
2
3
20201019133124,10.20.30.1/16,2
20201019133125,10.20.30.1/16,2
20201019133126,10.20.30.1/16,-
20201019133127,10.20.30.1/16,-
20201019133128,10.20.30.1/16,-
20201019133129,10.20.30.1/16,2
```
Nは3であり、10.20.30.1/16は20201019133126から20201019133128まで3回連続してタイムアウトしています。そのため、故障とみなし、出力は以下の通りとなります。
```
10.20.30.1/16 20201019133126 20201019133129
```

## コードの構成

コードは以下の要素で構成されています。
- 基本的な機能のための関数・クラス
    - IPアドレスを表す `Address` 構造体
    - サーバーログを表す `Log` 構造体
    - 日付の文字列 → `timestamp` への変換を行う `DateToTimestamp`
    - `timestamp` → 日付の文字列への変換を行う `TimestampToDate`
- 各設問を実行する関数
    - `GetBrokenServers1`
    - `GetBrokenServers2`
    - `GetBrokenServers3`
    - `GetBrokenServers4` (時間内に完了できず、未完成となっております。大変申し訳ございません。)
- テストデータを読み込み実行する関数 CodeTest
- main関数

## テストデータについて

テストデータとその意図について、一覧でまとめます。

|設問|ファイル名|意図|
|--|--|--|
|設問1|`in/test_1_0_in.txt`|1つのサーバーで故障しなかった場合|
|設問1|`in/test_1_1_in.txt`|1つのサーバーで故障した場合|
|設問1|`in/test_1_2_in.txt`|2つのサーバーで故障した場合|
|設問1|`in/test_1_3_in.txt`|2つのサーバーで、一つ復活し、また故障した場合|
|設問1|`in/test_1_4_in.txt`|2つのサーバーで、故障したままログが終了した場合|
|設問2|`in/test_2_0_in.txt`|`in/test_1_0_in.txt`と同じ（N=1）|
|設問2|`in/test_2_1_in.txt`|`in/test_1_1_in.txt`と同じ（N=1）|
|設問2|`in/test_2_2_in.txt`|`in/test_1_2_in.txt`と同じ（N=1）|
|設問2|`in/test_2_3_in.txt`|`in/test_1_3_in.txt`と同じ（N=1）|
|設問2|`in/test_2_4_in.txt`|`in/test_1_4_in.txt`と同じ（N=1）|
|設問2|`in/test_2_5_in.txt`|2回まで連続して応答なしの場合（N=3）|
|設問2|`in/test_2_6_in.txt`|3回まで連続して応答なしの場合（N=3）|
|設問3|`in/test_3_0_in.txt`|`in/test_2_0_in.txt`と同じ（N=1, m=1, t=3）|
|設問3|`in/test_3_1_in.txt`|`in/test_2_1_in.txt`と同じ（N=1, m=1, t=3）|
|設問3|`in/test_3_2_in.txt`|`in/test_2_2_in.txt`と同じ（N=1, m=1, t=3）|
|設問3|`in/test_3_3_in.txt`|`in/test_2_3_in.txt`と同じ（N=1, m=1, t=3）|
|設問3|`in/test_3_4_in.txt`|`in/test_2_4_in.txt`と同じ（N=3, m=1, t=3）|
|設問3|`in/test_3_5_in.txt`|`in/test_2_5_in.txt`と同じ（N=3, m=1, t=3）|
|設問3|`in/test_3_6_in.txt`|`in/test_2_6_in.txt`と同じ（N=3, m=1, t=3）|
|設問3|`in/test_3_7_in.txt`|1つのサーバーで障害なし、過負荷が発生した場合。（N=1, m=1, t=5）|
|設問3|`in/test_3_8_in.txt`|1つのサーバーで障害なし、過負荷が発生した場合。（N=1, m=3, t=5）|
|設問3|`in/test_3_9_in.txt`|2つのサーバーで障害あり、過負荷が発生した場合。（N=1, m=1, t=5）|

## プログラムの実行方法

このソースコードはC++14で書かれています。プログラムを実行するためには、g++などのC++用コンパイラを用いる必要があります。g++を仕様したコンパイル実行例を以下に記載します。
```
g++ -o out main.cpp
```
そして、生成されたファイルを以下のように実行すれば、テストデータに対して出力結果が表示されます。
```
./out
```