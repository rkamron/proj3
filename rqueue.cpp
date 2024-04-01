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

}
void RQueue::clear() {

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
// 
void RQueue::mergeWithQueue(RQueue& rhs) {
    Node *leftTree = m_heap;
    Node *rightTree = rhs.m_heap;

    m_heap = merge(leftTree, rightTree);

    /*
    // if either rhs or m_heap are empty 
    if (leftTree == nullptr) {
        m_heap = rhs.m_heap;
        return;
    }
    else if (rightTree == nullptr) return;

    // if neither m_heap or rhs are empty, we calculate their priority then merge them
    if (m_heapType == MINHEAP) {
        if (m_priorFunc(leftTree->getStudent()) <= m_priorFunc(rightTree->getStudent())) {
            m_heap = merge(leftTree, rightTree);
        }
        else {
            m_heap = merge(rightTree, leftTree);
        }
    }
    else if (m_heapType == MAXHEAP) {
        if (m_priorFunc(leftTree->getStudent()) >= m_priorFunc(rightTree->getStudent())) {
            m_heap = merge(leftTree, rightTree);
        } 
        else {
            m_heap = merge(rightTree, leftTree);
        }
    }
    */
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
    p1->m_right = p1->m_left;
    p1->m_left = merge(p2, tempHeap);
    
    return p1;
}

// insertStudent(const Student&)
// Creates a new node with the student information and merges it with the existing tree
void RQueue::insertStudent(const Student& student) {
    Node *newNode = new Node(student); // makes a new node 
    m_heap = merge(m_heap, newNode);
    m_size++;
}

int RQueue::numStudents() const {

}

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