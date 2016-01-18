package com.kohls;

import com.kohls.test.StringToLongTest;
import com.kohls.test.TrinaryTreeTest;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;

public class Main {

    public static void main(String[] args) {
        JUnitCore junit = new JUnitCore();
        Result result = junit.run(StringToLongTest.class, TrinaryTreeTest.class);

        if (result.wasSuccessful()) {
            System.out.println("All tests were successful");
        } else {
            System.out.println(result.getFailures());
        }
    }
}
