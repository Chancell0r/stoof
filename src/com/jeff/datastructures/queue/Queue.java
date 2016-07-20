package com.jeff.datastructures.queue;

/**
 * Created by kohlsj on 7/19/16.
 */
public interface Queue<E> {

    boolean enqueue(E object);

    E dequeue();

    E peek();

}
