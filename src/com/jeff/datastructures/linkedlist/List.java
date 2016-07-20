package com.jeff.datastructures.linkedlist;

/**
 * Created by kohlsj on 7/18/16.
 */
public interface List<V> {

    boolean linkFront(V object);

    boolean linkBack(V object);

    boolean contains(V object);

    boolean remove(V object);

    V first();

    V last();

    boolean unlinkFirst();

    boolean unlinkLast();

    boolean insertBefore(V objectToInsert, V objectToSearch);

    boolean insertAfter(V objectToInsert, V objectToSearch);

}
