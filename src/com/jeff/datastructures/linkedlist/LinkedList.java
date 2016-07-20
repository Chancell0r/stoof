package com.jeff.datastructures.linkedlist;

import com.jeff.datastructures.Node;

/**
 * Created by kohlsj on 7/18/16.
 */
public class LinkedList<V> implements List<V> {

    private Node<V> head;

    private Node<V> tail;

    public LinkedList() {
    }

    public LinkedList(V firstValue) {
        Node<V> node = new Node<>(firstValue, null);
        head = tail = node;
    }

    @Override
    public boolean linkFront(V object) {
        if (object == null) {
            return false;
        }
        Node<V> nodeToAdd = new Node<>(object, this.head);
        this.head = nodeToAdd;
        if (this.tail == null) {
            this.tail = nodeToAdd;
        }
        return true;
    }

    @Override
    public boolean linkBack(V object) {
        if (object == null) {
            return false;
        }
        Node<V> nodeToAdd = new Node<>(object);
        if (tail == null) {
            tail = nodeToAdd;
            head = nodeToAdd;
            return true;
        }
        Node<V> lastNode = this.tail;
        lastNode.setNext(nodeToAdd);
        tail = nodeToAdd;
        return true;
    }

    @Override
    public boolean contains(V object) {
        Node<V> currentNode = this.head;
        while (currentNode != null) {
            if (currentNode.getValue().equals(object)) {
                return true;
            } else {
                currentNode = currentNode.next();
            }
        }
        return false;
    }

    @Override
    public boolean remove(V object) {
        if (object == null || head == null) {
            return false;
        }
        if (head.getValue().equals(object) && head == tail) {
            head = null;
            tail = null;
            return true;
        }
        Node<V> currentNode = head;
        while (currentNode.next() != null) {
            if (currentNode.next().getValue().equals(object)) {
                Node<V> nodeToRemove = currentNode.next();
                currentNode.setNext(nodeToRemove.next());
                return true;
            } else {
                currentNode = currentNode.next();
            }
        }
        return false;
    }

    @Override
    public V first() {
        return head == null ? null : head.getValue();
    }

    @Override
    public V last() {
        return tail == null ? null : tail.getValue();
    }

    @Override
    public boolean unlinkFirst() {
        if (head == null) {
            return false;
        }
        if (head == tail) {
            head = null;
            tail = null;
            return true;
        }
        head = head.next();
        return true;
    }

    @Override
    public boolean unlinkLast() {
        if (tail == null) {
            return false;
        }
        if (tail == head) {
            tail = null;
            head = null;
            return true;
        }
        Node<V> currentNode = head;
        while (currentNode != null) {
            if (currentNode.next() == tail) {
                currentNode.setNext(null);
                tail = currentNode;
                return true;
            } else {
                currentNode = currentNode.next();
            }
        }
        return false;
    }

    @Override
    public boolean insertBefore(V objectToInsert, V objectToSearch) {
        Node<V> insertNode = new Node<>(objectToInsert);
        if (head == null) {
            head = insertNode;
            tail = insertNode;
            return true;
        }
        Node<V> currentNode = head;
        while (currentNode != null) {
            if (currentNode.next().getValue().equals(objectToSearch)) {
                Node<V> nodeToLink = currentNode.next();
                currentNode.setNext(insertNode);
                insertNode.setNext(nodeToLink);
                return true;
            } else {
                currentNode = currentNode.next();
            }
        }
        return false;
    }

    @Override
    public boolean insertAfter(V objectToInsert, V objectToSearch) {
        Node<V> insertNode = new Node<>(objectToInsert);
        if (head == null) {
            head = insertNode;
            tail = insertNode;
            return true;
        }
        Node<V> currentNode = head;
        while (currentNode != null) {
            if (currentNode.getValue().equals(objectToSearch)) {
                currentNode.setNext(insertNode);
                if (currentNode == tail) {
                    tail = insertNode;
                }
                return true;
            } else {
                currentNode = currentNode.next();
            }
        }
        return false;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        if (head != null) {
            Node<V> list = head;
            while (list != null) {
                builder.append(list.getValue().toString());
                if (list.next() != null) {
                    builder.append(" -> ");
                }
                list = list.next();
            }
        }
        return builder.toString();
    }
}
