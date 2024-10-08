// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.h"

// RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
// Constructor
RQueue::RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}

// ~RQueue
// Destructor 
RQueue::~RQueue() {
    clear();
}

// clear()
void RQueue::clear() {
    deleteSubtree(m_heap);
}

// deleteSubtree(Node *subtree) helper function
// Recursively iterates through the tree and deletes every node is a post-order format
void RQueue::deleteSubtree(Node *subtree) {
    if (subtree == nullptr) return;

    deleteSubtree(subtree->m_left);
    deleteSubtree(subtree->m_right);

    delete subtree;
}

// RQueue(const RQueue& rhs)
RQueue::RQueue(const RQueue& rhs) {
    m_priorFunc = rhs.getPriorityFn();
    m_heapType = rhs.getHeapType();
    m_structure = rhs.getStructure();

    m_heap = deepCopy(rhs.m_heap);
}

// deepCopy(Node *temp)
// Recursively iterates through temp subtree and deep copies everything.
Node* RQueue::deepCopy(Node *temp) {
    if (temp == nullptr) return nullptr;

    Node *copyNode = new Node(temp->getStudent());

    copyNode->m_left = deepCopy(temp->m_left);
    copyNode->m_right = deepCopy(temp->m_right);

    return copyNode;
}

// getHeapType()
HEAPTYPE RQueue::getHeapType() const {
    return m_heapType;
}

// Overloaded assignment operator
RQueue& RQueue::operator=(const RQueue& rhs) {
    if (&rhs == this) return *this;

    clear();

    m_priorFunc = rhs.getPriorityFn();
    m_heapType = rhs.getHeapType();
    m_structure = rhs.getStructure();

    m_heap = deepCopy(rhs.m_heap);

    return *this;
}

// mergeWithQueue(RQueue &rhs)
// Calls merge(Node*, Node*) function to recursively merge the two heaps
void RQueue::mergeWithQueue(RQueue& rhs) {

    // protection against self-merging
    if (&rhs == this) return;

    if (rhs.getPriorityFn() != m_priorFunc || rhs.getHeapType() != m_heapType || rhs.getStructure() != m_structure) {
        throw domain_error("Cannot merge heaps with different structures or priority functions.\n");
    }

    Node *leftTree = m_heap;
    Node *rightTree = rhs.m_heap;

    if (m_structure == LEFTIST) {
        m_heap = leftistMerge(leftTree, rightTree);
    }
    else {
        m_heap = skewMerge(leftTree, rightTree);
    }
}

// merge(Node* p1, Node*p2)
// p1 has higher priority over p2
// p1 and p2 are both the roots of their trees
// returns the root of the one final tree
Node* RQueue::skewMerge(Node* p1, Node* p2) {

    // two verifications to check if either heaps dont exist
    if (p1 == nullptr) return p2;
    if (p2 == nullptr) return p1;

    // two verifications to check if the priority is in order
    if (m_heapType == MINHEAP && (m_priorFunc(p1->getStudent()) > m_priorFunc(p2->getStudent()))) return skewMerge(p2, p1);
    if (m_heapType == MAXHEAP && (m_priorFunc(p1->getStudent()) < m_priorFunc(p2->getStudent()))) return skewMerge(p2, p1);

    // merging the two heaps
    Node* tempHeap = p1->m_left;
    p1->m_right = skewMerge(p1->m_right, p2);
    p1->m_left = p1->m_right;
    p1->m_right = tempHeap;
    
    return p1;
}

// leftistMerge(Node *p1, Node *p2)
// p1 has higher priority over p2
Node* RQueue::leftistMerge(Node* p1, Node* p2) {

    // two verifications to check if either heaps dont exist
    if (p1 == nullptr) return p2;
    if (p2 == nullptr) return p1;

    // two verifications to check if the priority is in order
    if (m_heapType == MINHEAP && (m_priorFunc(p1->getStudent()) > m_priorFunc(p2->getStudent()))) return leftistMerge(p2, p1);
    if (m_heapType == MAXHEAP && (m_priorFunc(p1->getStudent()) < m_priorFunc(p2->getStudent()))) return leftistMerge(p2, p1);
    
    p1->m_right = leftistMerge(p1->m_right, p2); // recursively merges p1 right child with p2
    
    // if left child doesnt exist then we replace it with the right child
    if (p1->m_left == nullptr) {
        p1->m_left = p1->m_right;
        p1->m_right = nullptr;
    }
    // if right childs npl is higher than the left childs, it switched them
    else if (p1->m_left->m_npl < p1->m_right->m_npl) {
        Node* temp = p1->m_left;
        p1->m_left = p1->m_right;
        p1->m_right = temp;
    }
    
    // updating the npl of p1 based on the info on its right child
    if (p1->m_right) {
        p1->m_npl = p1->m_right->m_npl + 1;
    } 
    else {
        p1->m_npl = 0;
    }
    
    return p1;
}

// calculateNPL(Node*)
// Calculates the NPL of the current node
int RQueue::calculateNPL(Node * mynode) {
    if (mynode == nullptr){
        return -1;
    }

    int leftNPL = calculateNPL( mynode->m_left);
    int rightNPL = calculateNPL(mynode->m_right);

    mynode->m_npl = 1 + std::min(leftNPL, rightNPL);
    return mynode->m_npl;
}

// insertStudent(const Student&)
// Creates a new node with the student information and merges it with the existing tree
void RQueue::insertStudent(const Student& student) {
    Node *newNode = new Node(student); // makes a new node 
    if (m_structure == LEFTIST) {
        m_heap = leftistMerge(m_heap, newNode);
    }
    else {
        m_heap = skewMerge(m_heap, newNode);
    }

    m_size++;
}

// numStudents()
// returns the number of students
// calls count(Node*) helper function to get the total number of trees.
int RQueue::numStudents() const {
    return count(m_heap);
}

// count(Node *curr)
// returns the number of students in the curr subtree
int RQueue::count(Node *curr) const {
    int numNodes = 0;

    if (curr == nullptr) return 0; // if curr Node doesnt exist

    numNodes = count(curr->m_left);
    numNodes += count(curr->m_right);

    return numNodes;
}

// getPriorityFn() const
prifn_t RQueue::getPriorityFn() const {
    return m_priorFunc;
}

// getNextStudent()
// pops the root node
Student RQueue::getNextStudent() {
    if (m_size == 0) throw out_of_range("Error: unable to getNextStudent because heap is empty.\n");
    
    Student priorityStudent = m_heap->getStudent();
    delete m_heap;
    m_heap = skewMerge(m_heap->m_left, m_heap->m_right);

    return priorityStudent;
}

// setPriorityFn(prifn_t priFn, HEAPTYPE heapType);
// Convers the heap from MIN to MAX and vice versa
// Iterates through the heap in post-order and rebuilds the heap with the same nodes
void RQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    m_priorFunc = priFn;
    m_heapType = heapType;

    Node *old_root = m_heap;
    m_heap = nullptr;

    m_heap = reInsert(m_heap, old_root);
}

// reInsert(Node* root, Node* curr);
// root is the root Node of the new tree
// recusively takes every node in post-order
Node* RQueue::reInsert(Node *root, Node *curr) {
    Node *right = curr->m_left; // saves left child
    Node *left = curr->m_right; // saves right child
    
    // isolates curr from its children
    curr->m_left = nullptr; 
    curr->m_right = nullptr;

    // reinsert nodes in left subtree
    if (left) {
        root = reInsert(root, left);
    }
    // reinsert nodes in right subtree
    if (right) {
        root = reInsert(root, right);
    }

    // checks for the type of structure before re-inserting leaf node curr
    if (m_structure == LEFTIST) {
        root = leftistMerge(root, curr);
    }
    else {
        root = skewMerge(root, curr);
    }

    return root;
}

// setStructure(STRUCTURE structure)
// Changes the structure of the heap, from skew
void RQueue::setStructure(STRUCTURE structure){
    m_structure = structure;

    Node *old_root = m_heap; // save old heap
    m_heap = nullptr;

    m_heap = reInsert(m_heap, old_root); 
}

// getStructure() const
// m_structure getter
STRUCTURE RQueue::getStructure() const {
    return m_structure;
}

// printStudentsQueue() const
// Prints all nodes post-order
void RQueue::printStudentsQueue() const {
    preOrderPrint(m_heap);
}

// preOrderPrint(Node *curr) const Helper function
// Recusrsively iterates through the tree and prints every Node
void RQueue::preOrderPrint(Node *curr) const {
    if (curr == nullptr) return;

    cout << "[" << m_priorFunc(curr->getStudent()) << "] " << curr->getStudent() << "\n";

    preOrderPrint(curr->m_left);
    preOrderPrint(curr->m_right);
}

void RQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
    dump(m_heap);
  }
  cout << endl;
}
void RQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(pos->m_student) << ":" << pos->m_student.m_name;
    else
        cout << m_priorFunc(pos->m_student) << ":" << pos->m_student.m_name << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Student& student) {
  sout << "Student name: " << student.m_name 
        << ", Major: " << student.getMajorStr()
        << ", Gender: " << student.getGenderStr()
        << ", Level: " << student.getLevelStr();
  return sout;
}
ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.m_student;
  return sout;
}


// 
/*
// Updating NPL
    if (p1->m_left == nullptr) {
        // left child empty so move the right to the left
        p1->m_left = p1->m_right;
        p1->m_right = nullptr;
    } 
    else if (p1->m_left != nullptr && p1->m_right != nullptr) {
        if (p1->m_left->m_npl < p1->m_right->m_npl) {
            // if the right side npl is higher than left side npl we swap
            Node *temp = p1->m_left;
            p1->m_left = p1->m_right;
            p1->m_right = temp;
        }
        
        //cout << "Current node before updating its NPL: \n" << *p1 << "\n";
        int minNPL = (p1->m_right->m_npl <= p1->m_left->m_npl) ? p1->m_right->m_npl : p1->m_left->m_npl;
        
        if (p1->m_npl == minNPL + 1) return p1;

        p1->setNPL(p1->m_right->m_npl + 1);

    }
*/

/*
    if (p1->m_left) p1->m_left->m_npl = calculateNPL(p1->m_left);
    if (p1->m_right) p1->m_right->m_npl = calculateNPL(p1->m_right);
    
    // swap if right npl higher than left npl

    if (calculateNPL(p1->m_left) < calculateNPL(p1->m_right)) {
        Node *temp = p1->m_right;
        p1->m_right = p1->m_left;
        p1->m_left = temp;
    }
    
    p1->m_npl = 1 + calculateNPL(p1);
    */