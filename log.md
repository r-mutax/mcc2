# 2024/03/03
- preprocessorのステージを追加した

# 2024/03/04
- #includeを読み取ってTK_INCLUDEとするようにした。TK_HASHとTK_HASH_HASHも副産物として追加
- `defined`をどうしようか？プリプロセッサの世界では予約後だけど、そうじゃないときはそうじゃない。まあこれはtokenizerではTK_DEFINEDとかにしておいて、プリプロセッサから抜けるときにTK_IDENTに変換しようかな
- tokeeprocessも一緒にかけるようにした。9cc.hもこうしていた。
- 早いとこ`#include`くらいはやっときたいな。あとは`#if`ができればセルフホストのための最低限のプリプロセッサが動きそう。（セルフホスト用のデバッグスイッチが動けばいいので）
- じゃあコンパイラに事前定義マクロ渡したいね～。

# 2024/03/05
- preprocessの中でTK_INCLUDEが来たら、tokenize(filepath)して置き換えるようにした。今のところinludepathは、コンパイルターゲットに限定している
- ちょっと反則的だけど、IR_COMMENTの第1引数にわたすようにREG_TOKENタイプを追加した。これはアセンブリファイルに出す文字列が文字列+ファイル名でやっているので、そのせい。
- error_atの引数をTokenに変更した。

# 2024/03/09
- これは昨日だけど、型指定子を読めるようにした。constだけ対応している。Type.is_constを追加して、is_constへのassignをエラーにした。
- 日記書く習慣がなかなかないから書き忘れてしまう。
- error_tok()とerror_at_src()を用意して、tokenize中はerror_at_src()でエラーを出すようにした。従来のTokenでerror出す関数はerror_tok()にした。
- long型を追加した。

# 2024/08/13
- 明示CASTの実装中。mccでは命令が扱うレジスタは基本64bit指定ってことにしよう。楽だから。つまりサイズが増える方向のキャストの場合は、何もいらなくなる。楽。
- というか今はレジスタをアクティベートすると64bitで操作する仕様だからしょうがない。
- サイズが下がる場合は切り詰めをしないといけない。
- unsignedは使えない仕様だった。unsignedの分も頑張ってテーブル作ったんだが…次はunsigndを実装するかな…
- というかpointerのテスト動かしてなかったんですけど～

# 2024/08/16
- ポインタの試験用関数が呼ばれていなかったので呼ぶようにした。
- DREFへの書き込みサイズがおかしかったので修正。IR_ASSIGNの左オペランドは書き込み先のアドレスなんだけど、ポインタの中身をロードしてるときにおかしかった。ポインタ変数のサイズは8だけど、例えばintのポインタだったら書き込む先は1じゃないといけないので。

# 2024/08/17
- 符号なし整数に対応した。結構色々変えた。直値を今までint固定で扱っていたのを、unsigned longで扱うようにした。マイナスの値はunary()で読み取るので、直値は全部正の数でOK
- 変数からのロードで固定で符号拡張していたのを、unsignedなら零拡張するようにした。
- cmp命令に渡す直値が32bit幅を超えるとNGなので、左辺値として（レジスタに直値を読み込んで）比較するようにした。

# 2025/03/06
- デバッグ情報がうまくかけなくなるので、一旦cfi*ディレクティブは出さないようにする。
- ラインデバッグでうまく止まってくれないので、.fileディレクティブにファイルをパスで書くようにする。

# 2025/04/29
- 構造体の初期化式のチャレンジを始めた。まずは手始めに`struct STRUCT s = {};`で0クリアできるように対応した。
    - 初期化式のnodeをInitialize.init_nodeに変更して、base_type + pointerとstruct, union, arrrayの場合で処理を共通化した。
    - Relocationは、init_nodeのタイプで処理を分けるようにした。
    - ついでにテストのメイン関数でスタックを汚しておくようにした。

# 2025/05/04 - 2025/05/06
- 関数内部で宣言するstatic変数にチャレンジ
    - 関数スコープに、その内部で使うstatic変数のリストをもたせる必要がある。ポインタ連結リストをライブラリとして実装する。
        - せっかくなのでライブラリは外部にも公開できるようにライブラリとして用意してみる。
        - #indludeした結果、読み込んだトークン列がTK_EOFのみになっちゃうことがあったので、不具合修正。
    - 関数のIdentに変数のポインタリストをもたせるようにした。
        - static変数には宣言ごとにidを割り当てて、`.Lvarname.static_id`という名前で扱うようにした。
        - static_idはユニークにすることで、異なる関数/異なるスコープで宣言されたstatic変数を別で扱うことが可能になる。

# 2025/05/11
- 多次元配列に対応した。（まだしてなかったんだ…）
- エスケープシーケンスの読み込みができてなかったので直した。
- #pragma onceがファイル名とファイルパスで比較して聞いていなかったので、ファイルパスを登録してファイルパスで比較するようにした。

# 2025/07/04
- 長さのある配列の初期化に対応した。
    - 指定の数よりも短い場合、長い場合に対応しないといけない
    - グローバル変数の初期化にも対応する必要あり
