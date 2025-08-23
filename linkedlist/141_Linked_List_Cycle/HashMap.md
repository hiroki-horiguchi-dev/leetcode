### HashMap を簡易的に再実装してみよう
- [wiki](https://ja.wikipedia.org/wiki/%E3%83%8F%E3%83%83%E3%82%B7%E3%83%A5%E3%83%86%E3%83%BC%E3%83%96%E3%83%AB) と [HashMap](https://docs.oracle.com/javase/8/docs/api/java/util/HashMap.html)読んでざっと書いたのがここまで
- 
```java
// interface は以下の Method Summary を参照
// https://docs.oracle.com/javase/8/docs/api/java/util/HashMap.html
interface HashMap {
    void clear();

    V get(Object key);

    V getOrDefault(Object key, V defaultValue);

    V put(K key, V value);

    // remove なのに　Value を返すのはなぜ？ boolean の設計が一番良くない？削除できたのか、できなかったのかが知りたいのであって、削除された値を返す必要あるのか？
    V remove(Object key);

    boolean containsKey(Object key);

    boolean containsValue(Object value);

    boolean isEmpty();

    int size();
}

// こいつがどういう実装になっているのかは今はまあいいかな、あとで共通化すればいい
// 多分だけど、hashTable とかはこっちに移しちゃっていもいい気もする
// 継承って仕組みあんまり好きじゃないんだよな。変数が隠蔽されちゃって辿るんが難しいし、どこでいつ変更されたのかが把握しづらくて可読性悪いと思うんだよな
abstract class AbstractMap<K, V> {}

public class HashMapImpl<K, V>() extends AbstractMap<K,V> implements HashMap  {
    
    // 10 くらい？？？
    private final int HASH_TABLE_DEFAULT_CAPACITY = 10;

    // HashTable を用意する、サイズはどのくらい持っておけばいい？いつ拡張する？
    // 連鎖法で組もうとしているので、ハッシュ化された key, value, next ポインタを持つ構造体のリスト 
    private Node<K, V>[HASH_TABLE_DEFAULT_CAPACITY] hashTable;
    
    // HashTable の要素数をカウントする。hashTable.size() or hashTable.length でいい気もするけど。。
    // 計算回数 O(N) になっちゃうからもっといたほうがいいかも
    private int tableSize = 0;
    
    @Override
    public void clear() {
        // 参照切っちゃえばいいよね？
        hashTable = new Node<K,V>[HASH_TABLE_DEFAULT_CAPACITY];
        tableSize = 0;
    }

    
    @Override
    public V get(Object key) {
        int hashedCode = hashCode(key);
        if (hashTable[hashedCode] != null) {
            return hashTable[hashedCode];
        } else {
            return null;
        }
    }

    @Override
    public V getOrDefault(Object key, V defaultValue) {
        V temp = get(key);
        if (temp == null) {
            return defaultValue;
        } else {
            return temp;
        }
    }

    @Override
    public V put(K key, V value) {
        int hashedKey = hashCode(key);
        if (Node[hashedKey] == null) {
            Node[hashedKey] = new Node(hashedKey, value);
        } else {
            // 衝突回避処理を実行
            // 連鎖法: 衝突を起こした hash化した後の key 同士をポインタでつなぐ方式。
            // 開番地法: 衝突が発生したら、テーブルの中の空いている別の番地を探す方式。
        }
    }

    @Override
    public V remove(Object key) {
        int hashedCode = hashCode(key);
        if (Node[hashedCode] != null) {
            V temp Node[hashedCode];
            Node[hashedCode] = null;
            tableSize--;
            return temp;
        } else {
            return null;
        }
    }

    // うん？ key があるって何で判断すんの？
    @Override
    public boolean containsKey(Object key) {
        int hashedKey = hashCode(key);
        return Node[hashedKey] != null;
    }

    // こっちはこれで良さそう
    @Override
    public boolean containsValue(Object value) {
        int hashedKey = hashCode(key);
        return Node[hashedKey] != null;
    }

    @Override
    public boolean isEmpty() {
        return tableSize == 0;
    }

    @Override
    public int size() {
        return tableSize;
    }
}

// Value をラップした Node構造体
public class Node<K, V> {
    K key;
    V value;
    Node<K,V> next;
    Node(Object value) {
        this.value = value;
    }
}


```