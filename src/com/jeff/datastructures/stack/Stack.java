package com.jeff.datastructures.stack;

/**
 * Created by kohlsj on 7/19/16.
 */
public interface Stack<E> {

    E pop();

    boolean push(E object);

    E peek();

}
