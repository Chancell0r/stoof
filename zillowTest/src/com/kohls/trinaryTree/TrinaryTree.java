package com.kohls.trinaryTree;

/**
 * Created by kohlsj on 1/17/16.
 */
public class TrinaryTree {

    private TrinaryTreeNode root;

    public TrinaryTree() {
        this.root = null;
    }

    public TrinaryTree(Integer value) {
        this.root = new TrinaryTreeNode(value);
    }

    public TrinaryTreeNode getRoot() {
        return root;
    }

    /**
     * Insert a new node based on Integer value.
     *
     * @param value
     */
    public void insert(Integer value) {
        if (value == null) {
            return;
        }

        TrinaryTreeNode currentNode = root;
        TrinaryTreeNode nodeToInsert = new TrinaryTreeNode(value);

        insertNode(nodeToInsert, currentNode);
    }

    /**
     * delete a node based on Integer value.
     *
     * @param value
     */
    public void delete(Integer value) {
        if (value == null) {
            return;
        }

        TrinaryTreeNode currentNode = root;

        deleteNode(currentNode, new TrinaryTreeNode(value));
    }

    /**
     * recursively add new node into tree.
     *
     * @param nodeToInsert
     * @param currentNode
     */
    private TrinaryTreeNode insertNode(TrinaryTreeNode nodeToInsert, TrinaryTreeNode currentNode) {
        if (root == null && currentNode == root) {
            root = nodeToInsert;
            return null;
        }
        if (currentNode == null) {
            return nodeToInsert;
        }

        int comparison = nodeToInsert.compareTo(currentNode);
        if (comparison < 0) {
            currentNode.setLeftNode(insertNode(nodeToInsert, currentNode.getLeftNode()));
        } else if (comparison == 0) {
            currentNode.setMiddleNode(insertNode(nodeToInsert, currentNode.getMiddleNode()));
        } else {
            currentNode.setRightNode(insertNode(nodeToInsert, currentNode.getRightNode()));
        }
        return currentNode;
    }

    /**
     * recursively remove node from tree.
     *
     * @param currentNode
     * @param nodeToDelete
     * @return
     */
    private TrinaryTreeNode deleteNode(TrinaryTreeNode currentNode, TrinaryTreeNode nodeToDelete) {
        if (currentNode == null) {
            return null;
        }

        int comparison = nodeToDelete.compareTo(currentNode);

        if (comparison < 0) {
            currentNode.setLeftNode(deleteNode(currentNode.getLeftNode(), nodeToDelete));
        } else if (comparison > 0) {
            currentNode.setRightNode(deleteNode(currentNode.getRightNode(), nodeToDelete));
        } else {
            if (currentNode == root) {
                return null;
            }
            //traverse all middle nodes first
            if (currentNode.getMiddleNode() != null) {
                currentNode.setMiddleNode(deleteNode(currentNode.getMiddleNode(), nodeToDelete));
                return currentNode;
            }
            //if right node exists, replace current node with right node value
            //and assign left node of new node with the current nodes left node.
            if (currentNode.getRightNode() != null) {
                TrinaryTreeNode rightNode = currentNode.getRightNode();
                TrinaryTreeNode leftNode = currentNode.getLeftNode();
                rightNode.setLeftNode(leftNode);
                return rightNode;

            }
            //right node doesn't exist and replace current node with the current node's left node.
            else if (currentNode.getLeftNode() != null) {
                return currentNode.getLeftNode();
            }
            return null;
        }
        return currentNode;

    }
}
