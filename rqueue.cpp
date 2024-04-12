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
void RQueue::clear() {
    deleteSubtree(m_heap);
}

void RQueue::deleteSubtree(Node *subtree) {
    if (subtree == nullptr) return;

    deleteSubtree(subtree->m_left);
    deleteSubtree(subtree->m_right);

    delete subtree;
}

RQueue::RQueue(const RQueue& rhs) {

}

// getHeapType()
HEAPTYPE RQueue::getHeapType() const {
    return m_heapType;
}

// 
RQueue& RQueue::operator=(const RQueue& rhs) {
    return *this;
}

// mergeWithQueue(RQueue &rhs)
// Calls merge(Node*, Node*) function to recursively merge the two heaps
void RQueue::mergeWithQueue(RQueue& rhs) {
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

    // merging the two heaps
    p1->m_right = leftistMerge(p1->m_right, p2); 
    
    if (p1->m_left == nullptr) {
        p1->m_left = p1->m_right;
        p1->m_right = nullptr;
    }
    else {
        
        if (p1->m_left->m_npl < p1->m_right->m_npl) {
            Node *temp = p1->m_left;
            p1->m_left = p1->m_right;
            p1->m_right = temp;
        }

        if (p1->m_right) {
            p1->m_npl = p1->m_right->m_npl + 1;
        } 
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
    dump();
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
// 
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
// 
Node* RQueue::reInsert(Node *root, Node *curr) {
    Node *right = curr->m_left;
    Node *left = curr->m_right;

    if (left) {
        root = reInsert(root, left);
        curr->m_left = nullptr;
    }
    if (right) {
        root = reInsert(root, right);
        curr->m_right = nullptr;
    }

    if (m_structure == LEFTIST) {
        root = leftistMerge(root, curr);
    }
    else {
        root = skewMerge(root, curr);
    }
    cout << "\n";
    dump(root);
    return root;
}

// setStructure(STRUCTURE structure)
// Changes the structure of the heap, from skew
void RQueue::setStructure(STRUCTURE structure){
    m_structure = structure;

    Node *old_root = m_heap;
    m_heap = nullptr;

    m_heap = reInsert(m_heap, old_root);
}

// 
STRUCTURE RQueue::getStructure() const {
    return m_structure;
}

void RQueue::printStudentsQueue() const {

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