package com.kohls.trinaryTree;

/**
 * Created by kohlsj on 1/17/16.
 * <p/>
 * Denotes the structure of each node for a TrinaryTree.
 */
public class TrinaryTreeNode implements Comparable<TrinaryTreeNode> {

    private int value;

    private TrinaryTreeNode leftNode;

    private TrinaryTreeNode middleNode;

    private TrinaryTreeNode rightNode;

    public TrinaryTreeNode(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        this.value = value;
    }

    public TrinaryTreeNode getLeftNode() {
        return leftNode;
    }

    public void setLeftNode(TrinaryTreeNode leftNode) {
        this.leftNode = leftNode;
    }

    public TrinaryTreeNode getMiddleNode() {
        return middleNode;
    }

    public void setMiddleNode(TrinaryTreeNode middleNode) {
        this.middleNode = middleNode;
    }

    public TrinaryTreeNode getRightNode() {
        return rightNode;
    }

    public void setRightNode(TrinaryTreeNode rightNode) {
        this.rightNode = rightNode;
    }

    @Override
    public int compareTo(TrinaryTreeNode o) {
        if (o == null) {
            return 1;
        }
        return value - o.getValue();
    }
}
