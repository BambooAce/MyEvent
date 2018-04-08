package main
import "fmt"
//结构体声明 不能var const修饰
type Node struct {
	name string
	age int
	next *Node
	prev *Node
}

type Header struct{
	header *Node
	tail *Node
	len uint32
}
//函数参数不支持const
func createNode(_name string , _age int) *Node{
		n := new(Node)
		if n != nil { 
			n.name = _name
			n.age = _age
			n.next = nil
			n.prev = nil
		}
		return n
}

func insertNode(head *Header, node *Node) bool{
	if head == nil || node == nil{
		return false
	}
	if head.len == 0{
		head.header = node
		head.tail = node
	}else{
		head.tail.next = node
		node.prev = head.tail
		head.tail = node
	}
	head.len++
	return true
}

func  getNode(head *Header, _name string) *Node{
	if head == nil{
		return nil
	}
	for n := head.header;  n != nil; n = n.next {
		if n.name == _name {
			return n
		}
	}
	return nil
}


func initList() *Header{
	n := new(Header)
	if n != nil {
		n.header = nil
		n.tail = nil
	}
	return n
}

func main() {
	//数组定义 []type{}
	nodearr :=  [...]Node{Node{name:"test1", age:12}, Node{name:"test2", age:13}}
	node := createNode("miaoge", 23)
	node2 := createNode("helloheloo", 33)
	header :=initList()
	//数组
	for _, one := range nodearr{
		insertNode(header, &one)
	}
	insertNode(header, node)
	insertNode(header, node2)   	 
	n :=getNode(header, "test2")
	fmt.Printf("%s, %d\n", n.name, n.age);
}
