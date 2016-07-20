package com.jeff.datastructures.stack;

import com.jeff.datastructures.Node;

/**
 * Created by kohlsj on 7/19/16.
 */
public class LinkedStack<E> implements Stack<E> {

    private Node<E> head;

    @Override
    public E pop() {
        if (head == null) {
            return null;
        }
        Node<E> currentNode = head;
        head = head.next();
        return currentNode.getValue();
    }

    @Override
    public boolean push(E object) {
        Node<E> nodeToAdd = new Node<>(object);
        if (head == null) {
            head = nodeToAdd;
        }
        Node<E> currentNode = head;
        nodeToAdd return false;
    }

    @Override
    public E peek() {
        if (head == null) {
            return null;
        }
        return head.getValue();
    }
}
