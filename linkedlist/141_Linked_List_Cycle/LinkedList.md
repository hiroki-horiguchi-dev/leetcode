### LinkedList(単方向)
```java
public class ListNode {
    int val;
    ListNode next;
    
    ListNode(int val, ListNode node) {
        this.val = val;
        this.next = node;
    }
}
```

### LinkedList(双方向)
```java
public class ListNode {
    int val;
    ListNode previous;
    ListNode next;
    
    ListNode(int val, ListNode next, ListNode previous) {
        this.val = val;
        this.previous = previous;
        this.next = next;
    }
}
```

### LinkedList (基本的なのを詰め込んだやつ)

```java

interface LinkedList {
    // 末尾追加
    boolean add(ListNode addNode);
    // 先頭追加
    void addFirst(ListNode node);
    // 任意の index 箇所に追加
    void add(int index, ListNode node); 
    // 任意の index 箇所を削除
    void remove(int index);
}

public class LinkedListImpl implements LinkedList {
    
    // 先頭 head を保持する
    ListNode head;
    // LinkedList size
    int size = 0;
    
    @Override
    public boolean add(ListNode addNode) {
        if (head == null) {
            this.head = addNode;
            size++;
            return true;
        } 
        ListNode node = head;
        while (node.next != null) {
            node = node.next;
        }
        node.next = addNode;
        size++;
        return true;
    }

    @Override
    public void addFirst(ListNode node) {
        if (head != null) {
            node.next = head;
        }
        this.head = node;
        size++;
    }

    @Override
    public void add(int index, ListNode addNode) {
        if (head == null) {
            this.head = addNode;
            size++;
        } else if (index == 0) {
            addFirst(addNode);
        } else {
            int i = 0;
            ListNode node = head;
            while (node.next != null) {
                if (i == (index - 1)) {
                    // ポインタ張り替えして return
                    addNode.next = node.next;
                    node.next = addNode;
                    size++;
                    return;
                }
                node = node.next;
                i++;
            }
            throw IndexOutOfBoundsException("index: " + index + "is out of range.")
        }
    }

    @Override
    public void remove(int index) {
        if (head == null) return;

        // 先頭削除
        if (index == 0) {
            head = head.next;
            size--;
            return;
        }
        
        int i = 0;
        ListNode node = head;
        while (node.next != null) {
            if (i == (index - 1)) {
                node.next = node.next.next;
                size--;
                return;
            }
            i++;
            node = node.next;
        }
    }

    LinkedListImpl(ListNode node) {
        this.head = node;
    }
}

public class ListNode {
    int val;
    ListNode next;

    ListNode(int val, ListNode node) {
        this.val = val;
        this.next = node;
    }
}

```