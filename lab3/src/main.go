package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func main() {
	hashWorkers := flag.Int("hash-workers", 1, "number of hash workers to hash BSTs")
	dataWorkers := flag.Int("data-workers", 1, "number of workers to update the map")
	compWorkers := flag.Int("comp-workers", 1, "number of workers to do the comparisons")
	input := flag.String("input", "./", "input file path")

	flag.Parse()

	fmt.Println("hash workers: ", *hashWorkers)
	fmt.Println("data workers: ", *dataWorkers)
	fmt.Println("compWorkers: ", *compWorkers)
	fmt.Println(*input)

	bsts := []*BST{}

	file, err := os.Open(*input)
	if err != nil {
		fmt.Println("File open failed")
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		nums := strings.Split(line, " ")
		//fmt.Println(line)
		tree := &BST{}

		for _, num := range nums {
			val, _ := strconv.Atoi(num)
			tree.Insert(val)
		}
		bsts = append(bsts, tree)
	}

	hashMap := map[int][]int{}

	for id, tree := range bsts {
		hash := tree.Hash()
		hashMap[hash] = append(hashMap[hash], id)
	}

	for h, val := range hashMap {
		fmt.Printf("%v: ", h)
		for _, v := range val {
			fmt.Printf("%v ", v)
		}
		fmt.Println()
	}

	for id, tree := range bsts {
		arr := tree.InOrderTraverse()

		fmt.Print(id)
		for _, v := range arr {
			fmt.Printf(" %v ", v)
		}
		fmt.Println()
	}

}
