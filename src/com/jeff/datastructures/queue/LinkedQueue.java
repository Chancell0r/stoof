package com.jeff.datastructures.queue;

import com.jeff.datastructures.Node;

/**
 * Created by kohlsj on 7/19/16.
 */
public class LinkedQueue<E> implements Queue<E> {

    private Node<E> front;

    private Node<E> back;

    @Override
    public boolean enqueue(E object) {
        Node<E> nodeToAdd = new Node<>(object);
        if (back == null) {
            back = nodeToAdd;
            front = nodeToAdd;
            return true;
        }
        Node<E> currentNode = head;
        head = nodeToAdd;
        nodeToAdd.setNext(currentNode);
        return true;
    }

    @Override
    public E dequeue() {
        if (front == null) {
            return null;
        }
        Node<E> currentNode = front;
        front = front.next();
        return currentNode.getValue();
    }

    @Override
    public E peek() {
        if (front == null) {
            return null;
        }
        return front.getValue();
    }
}
