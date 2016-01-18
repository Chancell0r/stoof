package com.kohls.trinaryTree;

/**
 * Created by kohlsj on 1/17/16.
 * <p/>
 * Class used to denote a TrinaryTree.
 * <p/>
 * If a node is greater than parent node, it goes into right leaf.
 * If a node is less than parent node, it goes into left leaf.
 * If a node is equal to the parent node, it goes into middle leaf.
 * <p/>
 * Insertions/Deletions do not balance the tree after.
 * <p/>
 * Deletions will preserve values when deleting nodes.
 */
public class TrinaryTree {

    private TrinaryTreeNode root;

    public TrinaryTree() {
        this.root = null;
    }

    public TrinaryTree(int value) {
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

        root = deleteNode(currentNode, new TrinaryTreeNode(value));
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
     * recursively find the node to remove from tree.
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
            return handleNodeDeletion(currentNode, nodeToDelete);
        }
        return currentNode;

    }

    /**
     * Handle the process of actually deleting the node
     *
     * @param currentNode
     * @param nodeToDelete
     * @return
     */
    private TrinaryTreeNode handleNodeDeletion(TrinaryTreeNode currentNode, TrinaryTreeNode nodeToDelete) {
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
            //if the right node has a left node, make sure to preserve it.
            if (rightNode.getLeftNode() != null) {
                if (rightNode.getRightNode() != null) {
                    rightNode.getRightNode().setLeftNode(rightNode.getLeftNode());
                }
            }
            rightNode.setLeftNode(leftNode);
            return rightNode;

        }
        //right node doesn't exist and replace current node with the current node's left node.
        else if (currentNode.getLeftNode() != null) {
            return currentNode.getLeftNode();
        }

        if (currentNode == root) {
            return null;
        }
        return null;
    }
}
