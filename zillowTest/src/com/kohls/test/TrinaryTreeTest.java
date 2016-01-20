package com.kohls.test;

import com.kohls.trinaryTree.TrinaryTree;
import com.kohls.trinaryTree.TrinaryTreeNode;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

/**
 * Created by kohlsj on 1/17/16.
 */
public class TrinaryTreeTest {

    private TrinaryTree tree;

    /**
     * tree setup for testing
     * ---3
     * --2  5
     * -1  4 6
     * -1
     */
    @Before
    public void setUp() {
        tree = new TrinaryTree();
        tree.insert(3);
        tree.insert(2);
        tree.insert(5);
        tree.insert(4);
        tree.insert(6);
        tree.insert(1);
        tree.insert(1);
    }

    @Test
    public void testInsertMultipleNodes() {
        TrinaryTreeNode node = tree.getRoot();
        Assert.assertEquals(3, node.getValue());
        node = node.getLeftNode();
        //test left insert
        Assert.assertEquals(2, node.getValue());
        node = node.getLeftNode();
        //test middle and multiple level insert
        Assert.assertEquals(1, node.getMiddleNode().getValue());
        node = tree.getRoot().getRightNode();
        //test right insert
        Assert.assertEquals(5, node.getValue());
    }

    @Test
    public void testDeleteWithNodeWithLeftAndRightNode() {
        tree.delete(5);
        TrinaryTreeNode node = tree.getRoot();
        node = node.getRightNode();
        Assert.assertEquals(6, node.getValue());
        Assert.assertEquals(4, node.getLeftNode().getValue());
    }

    @Test
    public void testDeleteWithNodeWithLeftNode() {
        tree.delete(2);
        TrinaryTreeNode node = tree.getRoot();
        Assert.assertEquals(1, node.getLeftNode().getValue());
    }

    @Test
    public void testDeleteWithNodeWithMiddleNode() {
        tree.delete(1);
        TrinaryTreeNode node = tree.getRoot().getLeftNode();
        Assert.assertEquals(1, node.getLeftNode().getValue());
        Assert.assertNull(node.getLeftNode().getMiddleNode());
    }

    @Test
    public void testDeleteNodeWithARightNodeThatHasALeftNode() {
        tree.delete(3);
        TrinaryTreeNode node = tree.getRoot();
        Assert.assertEquals(5, node.getValue());
        Assert.assertEquals(6, node.getRightNode().getValue());
        Assert.assertEquals(4, node.getLeftNode().getValue());
    }

}
