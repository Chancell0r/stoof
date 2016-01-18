package com.kohls.trinaryTree;

/**
 * Created by kohlsj on 1/17/16.
 */
public class TrinaryTreeNode extends Object implements Comparable<TrinaryTreeNode> {

    private Integer value;

    private TrinaryTreeNode leftNode;

    private TrinaryTreeNode middleNode;

    private TrinaryTreeNode rightNode;

    public TrinaryTreeNode(Integer value) {
        this.value = value;
    }

    public Integer getValue() {
        return value;
    }

    public void setValue(Integer value) {
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
        if (value == o.getValue()) {
            return 0;
        } else if (value > o.getValue()) {
            return 1;
        } else {
            return -1;
        }
    }
}
