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

