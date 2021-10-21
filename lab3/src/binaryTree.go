package main

type TreeNode struct {
	Value int
	Left  *TreeNode
	Right *TreeNode
}

// binary search tree
type BST struct {
	root *TreeNode
}

func New() *BST {
	tree := &BST{}
	return tree
}

func (t *BST) Insert(v int) {
	t.root = insert(v, t.root)
}

func insert(v int, root *TreeNode) *TreeNode {
	if root == nil {
		return &TreeNode{v, nil, nil}
	}
	if v < root.Value {
		root.Left = insert(v, root.Left)
	} else {
		root.Right = insert(v, root.Right)
	}
	return root
}

func (t *BST) InOrderTraverse() []int {
	arr := []int{}

	inOrder(t.root, &arr)
	return arr

}

func inOrder(p *TreeNode, arr *[]int) {
	if p == nil {
		return
	}

	inOrder(p.Left, arr)
	*arr = append(*arr, p.Value)
	//fmt.Printf("%v ", p.Value)
	inOrder(p.Right, arr)
}

func (t *BST) Hash() int {
	hash := 1

	var inOrderHash func(*TreeNode)
	inOrderHash = func(root *TreeNode) {
		if root == nil {
			return
		}
		inOrderHash(root.Left)
		hash = (hash*(root.Value+2) + (root.Value + 2)) % 1000
		inOrderHash(root.Right)
	}

	inOrderHash(t.root)

	return hash
}
