package com.jeff.datastructures;

import com.jeff.datastructures.linkedlist.LinkedList;
import com.jeff.datastructures.linkedlist.List;

public class Main {

    public static void main(String[] args) {
        List<String> linkedList = new LinkedList<>();
        linkedList.linkFront("hello");
        linkedList.linkFront("hello2");
        linkedList.linkBack("meow");
        linkedList.insertAfter("meow2", "meow");
        linkedList.insertBefore("meow3", "meow");
        linkedList.remove("meow");
        linkedList.unlinkFirst();
        linkedList.unlinkLast();
        System.out.println(linkedList.contains("meow3"));
        System.out.println(linkedList.contains("blah"));
        System.out.println(linkedList.first());
        System.out.println(linkedList.last());
        System.out.println(linkedList.toString());
    }
}
