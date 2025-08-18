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