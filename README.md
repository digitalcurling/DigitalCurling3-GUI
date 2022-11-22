# DigitalCurling3-GUI

このリポジトリではDigitalCurling3に対応したGUIを提供します．

使用方法については [GUIマニュアル](https://github.com/digitalcurling/DigitalCurling3/wiki/GUI%E3%83%9E%E3%83%8B%E3%83%A5%E3%82%A2%E3%83%AB) を参照ください．

## 機能

- サーバーが出力した対戦ログの再生
- (未実装) 人間用の対戦クライアント
- (未実装) 対戦サーバーの対戦途中状態の表示

## ダウンロード

[リリースページ](https://github.com/digitalcurling/DigitalCurling3-GUI/releases)からダウンロードしてください．

## ビルド

QtCreatorで `CMakeLists.txt` を開いてビルドします．

## ライセンス

GPL-3.0

---

# 以下開発者向け情報

## リリース作業(Windows)

1. QtCreatorでReleaseビルドを行う
1. 作成されたビルドディレクトリをカレントディレクトリとして，`cmake --install .`を実行する．
   - インストールディレクトリはCMakeのコンフィギュレーションの `CMAKE_INSTALL_PREFIX` から変更可能．
1. 2022/11/01現在，デプロイツールにバグがあるらしく，CMakeで作成したインストール先ディレクトリ内の `bin/libstdc++-6.dll` を手動でQtインストールディレクトリ内の `libstdc++-6.dll` に差し替える必要があります．また，`libgcc_s_dw2-1.dll`をインストール先から削除し，`libgcc_s_seh-1.dll`と`libwinpthread-1.dll`をQtインストールディレクトリからコピーします．
1. インストールディレクトリ内の `bin/digitalcurling3_gui.exe` を実行できることを確認する．
1. インストールディレクトリを丸ごとアーカイブしてリリースする．

