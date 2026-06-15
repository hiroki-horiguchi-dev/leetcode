from ucimlrepo import fetch_ucirepo
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, classification_report

# 1. UCIからIrisデータセットを直接取得
print("データをダウンロード中...")
iris = fetch_ucirepo(id=53)

# 2. 特徴量(X)とターゲット(y)を抽出（すでにPandasのDataFrame型になっています）
X = iris.data.features
y = iris.data.targets

# ※ y は1列のデータフレームなので、機械学習モデルが扱いやすいように1次元配列に変換します
y = y.values.ravel()

# 3. データを学習用8割、テスト用2割に分割
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 4. データの標準化
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# 5. ロジスティック回帰モデルの作成と学習
model = LogisticRegression(solver='lbfgs')
model.fit(X_train_scaled, y_train)

# 6. 予測の実行
y_pred = model.predict(X_test_scaled)

# 7. 結果の評価
accuracy = accuracy_score(y_test, y_pred)
print(f"\n正解率 (Accuracy): {accuracy:.2f}")
print("\n--- 詳細な評価レポート ---")
print(classification_report(y_test, y_pred))


print("\n" + "="*40)
print("🔮 任意のデータでアヤメの種類を予測します")
print("="*40)

# 1. 予測したいアヤメの数値を任意に入力（例として2つの花をセット）
# データの並び順: [sepal_length, sepal_width, petal_length, petal_width]
unknown_iris_data = [
    [5.1, 3.5, 1.4, 0.2],  # おそらく Setosa っぽい小ぶりな花
    [6.5, 3.0, 5.2, 2.0]   # おそらく Virginica っぽい大ぶりな花
]

# 2. 【超重要】入力データも、学習時と同じScalerで「標準化」する
# これをやらないと、AIが数値を正しく解釈できません
unknown_scaled = scaler.transform(unknown_iris_data)

# 3. ズバリ「どの種類か」を予測させる
predictions = model.predict(unknown_scaled)

# 4. 「それぞれの種類である確率」を計算させる
probabilities = model.predict_proba(unknown_scaled)

# 5. 結果を分かりやすく表示する
# ※ ucimlrepoから取得した本来の名前（Setosa等）のリスト
class_names = ['Iris-setosa', 'Iris-versicolor', 'Iris-virginica']

for i, (pred, prob) in enumerate(zip(predictions, probabilities)):
    print(f"\n【花サンプルの予測結果 {i+1}】")
    print(f"入力された数値: {unknown_iris_data[i]}")

    # 予測された種類（predには 0, 1, 2 のいずれかが入っているか、文字列がそのまま入っています）
    # ucimlrepoのターゲット形式に合わせて、名前を表示します
    if isinstance(pred, int):
        predicted_name = class_names[pred]
    else:
        predicted_name = pred  # すでに文字列の場合

    print(f"👉 AIの予測: 「{predicted_name}」です！")

    # 各確率の内訳を表示（各クラスの確率が小数で入っているので%に直す）
    print("📊 確率の内訳:")
    print(f"  - Setosa     : {prob[0]*100:.2f}%")
    print(f"  - Versicolor : {prob[1]*100:.2f}%")
    print(f"  - Virginica  : {prob[2]*100:.2f}%")