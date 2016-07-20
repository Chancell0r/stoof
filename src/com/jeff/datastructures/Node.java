package com.jeff.datastructures;

/**
 * Created by kohlsj on 7/18/16.
 */
public class Node<V> {

    private V value;

    private Node<V> next;

    private Node<V> prev;

    public Node(V value, Node<V> next, Node<V> prev) {
        this.value = value;
        this.next = next;
        this.prev = prev;
    }

    public Node(V value, Node<V> next) {
        this(value, next, null);
    }

    public Node(V value) {
        this.value = value;
    }

    public V getValue() {
        return value;
    }

    public void setValue(V value) {
        this.value = value;
    }

    public Node<V> next() {
        return next;
    }

    public void setNext(Node<V> node) {
        this.next = node;
    }

    public Node<V> prev() {
        return prev;
    }

    public void setPrev(Node<V> prev) {
        this.prev = prev;
    }
}
