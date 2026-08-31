## 前提
- CSZAP 4th 情報の問題

## 目的
- [Iris](https://archive.ics.uci.edu/dataset/53/iris)データをもとにロジスティック回帰分析をしてみる
- 4つの変数をもとに学習させ、何か適当な値を入れた時にどの種類のアヤメなのか？を判定させてみる

## 出力結果
```terminaloutput
(.venv) ➜  iris git:(iris-logstic-regression) ✗ python3 analytics.py
データをダウンロード中...

正解率 (Accuracy): 1.00

--- 詳細な評価レポート ---
                 precision    recall  f1-score   support

    Iris-setosa       1.00      1.00      1.00        10
Iris-versicolor       1.00      1.00      1.00         9
 Iris-virginica       1.00      1.00      1.00        11

       accuracy                           1.00        30
      macro avg       1.00      1.00      1.00        30
   weighted avg       1.00      1.00      1.00        30


========================================
🔮 任意のデータでアヤメの種類を予測します
========================================
/Users/hiroki.horiguchi/WorkSpace/leetcode/.venv/lib/python3.13/site-packages/sklearn/utils/validation.py:2827: UserWarning: X does not have valid feature names, but StandardScaler was fitted with feature names
  warnings.warn(

【花サンプルの予測結果 1】
入力された数値: [5.1, 3.5, 1.4, 0.2]
👉 AIの予測: 「Iris-setosa」です！
📊 確率の内訳:
  - Setosa     : 97.97%
  - Versicolor : 2.03%
  - Virginica  : 0.00%

【花サンプルの予測結果 2】
入力された数値: [6.5, 3.0, 5.2, 2.0]
👉 AIの予測: 「Iris-virginica」です！
📊 確率の内訳:
  - Setosa     : 0.04%
  - Versicolor : 15.20%
  - Virginica  : 84.76%
```