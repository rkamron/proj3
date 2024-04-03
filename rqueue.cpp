// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.h"
RQueue::RQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}
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

    m_heap = merge(leftTree, rightTree);
}

// merge(Node* p1, Node*p2)
// p1 has higher priority over p2
Node* RQueue::merge(Node* p1, Node* p2) {

    // two verifications to check if either heaps dont exist
    if (p1 == nullptr) return p2;
    if (p2 == nullptr) return p1;

    // two verifications to check if the priority is in order
    if (m_heapType == MINHEAP && (m_priorFunc(p1->getStudent()) > m_priorFunc(p2->getStudent()))) return merge(p2, p1);
    if (m_heapType == MAXHEAP && (m_priorFunc(p1->getStudent()) < m_priorFunc(p2->getStudent()))) return merge(p2, p1);

    // merging the two heaps
    Node* tempHeap = p1->m_right; 
    p1->m_right = p1->m_left; // swap children of p1
    p1->m_left = merge(p2, tempHeap); // merging p2 with the left child of p1
    
    // updating npl
    if (p1->m_left == nullptr) {
        // left child empty so move the right to the left
        p1->m_left = p1->m_right;
        p1->m_right = nullptr;
        p1->setNPL(p1->m_left->m_npl + 1);
    } 
    else if (p1->m_left != nullptr && p1->m_right != nullptr) {
        if (p1->m_left->m_npl < p1->m_right->m_npl) {
            // if the right side npl is higher than left side npl we swap
            Node *temp = p1->m_left;
            p1->m_left = p1->m_right;
            p1->m_right = temp;
        }
        p1->setNPL(p1->m_right->m_npl + 1);
    }

    return p1;
}

// insertStudent(const Student&)
// Creates a new node with the student information and merges it with the existing tree
void RQueue::insertStudent(const Student& student) {
    Node *newNode = new Node(student); // makes a new node 
    m_heap = merge(m_heap, newNode);
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

Student RQueue::getNextStudent() {

}

void RQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    m_priorFunc = priFn;
    m_heapType = heapType;
}

void RQueue::setStructure(STRUCTURE structure){
    m_structure = structure;
}

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