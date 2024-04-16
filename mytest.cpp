// UMBC - CMSC 341 - Spring 2024 - Proj3
#include "rqueue.cpp"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <ctime>     //used to get the current time
#include <string> // used for formatting output
using namespace std;
// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

    string getRandString(int size){
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i=0;i<size;i++){
            output = output + (char)getRandNum();
        }
        return output;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
// Priority functions compute an integer priority for a student.  Internal
// computations may be floating point, but must return an integer.
int priorityFn1(const Student &student);
int priorityFn2(const Student &student);
// a name database for testing purposes
// we can use this DB to generate student names randomly instead of generating
// random strings as names (from Random class)
const int NUMNAMES = 20;
string nameDB[NUMNAMES] = {
    "Ismail Carter", "Lorraine Peters", "Marco Shaffer", "Rebecca Moss",
    "Lachlan Solomon", "Grace Mclaughlin", "Tyrese Pruitt", "Aiza Green", 
    "Addie Greer", "Tatiana Buckley", "Tyler Dunn", "Aliyah Strong", 
    "Alastair Connolly", "Beatrix Acosta", "Camilla Mayo", "Fletcher Beck",
    "Erika Drake", "Libby Russo", "Liam Taylor", "Sofia Stewart"
};

class Tester{
    // implement your test functions in this class
    public:

    // testLeftistHeap(RQueue &tree)
    // Uses heap root to call checkNPL()
    bool testLeftistHeap(RQueue &tree) {
        bool result = checkNPL(tree.m_heap);
        return result;
    }

    // checkNPL(Node* curr)
    // iterates through the curr subtree and checks to make sure that the NPL is correct
    bool checkNPL(Node *curr) {
        // if node exists
        if (curr != nullptr) {
            
            // no children
            if (curr->m_left == nullptr && curr->m_right == nullptr) {
                if (curr->getNPL() != 0) return false; // checking curr NPL
                else return true;
            }
            // one child
            else if (curr->m_left == nullptr || curr->m_right == nullptr) {
                if (curr->getNPL() != 0) return false; // checking curr NPL

                Node *next = (curr->m_left != nullptr) ? curr->m_left : curr->m_right;
                return checkNPL(next);
            }
            // two children
            else if (curr->m_left && curr->m_right) {
                
                // if the right child has a higher NPL than the left child
                if (curr->m_left->getNPL() < curr->m_right->getNPL()) return false;

                // if curr node is +1 the min NPL of its nodes
                Node* childNPL = (curr->m_right->m_npl < curr->m_left->m_npl) ? curr->m_right : curr->m_left;

                if (curr->m_npl != childNPL->m_npl + 1) return false;

                // recusively check both childrens NPL
                bool result = (checkNPL(curr->m_left) && checkNPL(curr->m_right)) ? true : false;
                return result;
            }
        }

        // if curr == nullptr
        return true;
    }

    bool testSkewHeapInserts(RQueue & tree) {
        bool result = checkPriority(tree.m_heap, tree.m_heapType, tree.m_priorFunc);
        return result;
    }

    // checkPriority(Node*, HEAPTYPE, prifn_t)
    // iterates through the curr subtree and checks if the parent has a higher priority
    bool checkPriority(Node *curr, HEAPTYPE type, prifn_t priFn) {
        if (curr == nullptr) return true;

        // if there is a left child then check the priority between curr and curr->m_left
        if (curr->m_left) {
            if (type == MINHEAP && (priFn(curr->getStudent()) > priFn(curr->m_left->getStudent()))) {
                return false;
            }
            else if (type == MAXHEAP && (priFn(curr->getStudent()) < priFn(curr->m_left->getStudent()))) {
                return false;
            }

            return checkPriority(curr->m_left, type, priFn);
        }
        
        // if there is a right child then check the priority between curr and curr->m_right
        if (curr->m_right) {

            // if in a MINHEAP, the parent node priority is lower than its right node
            if (type == MINHEAP && (priFn(curr->getStudent()) > priFn(curr->m_right->getStudent()))) {
                return false;
            }
            else if (type == MAXHEAP && (priFn(curr->getStudent()) < priFn(curr->m_right->getStudent()))) {
                return false;
            }

            return checkPriority(curr->m_right, type, priFn);
        }

        return true;
    }

    // testDeepCopy(RQueue &tree1, RQueue &tree2)
    // accepts two trees and uses their root nodes to call deepCopyCheck()
    bool testDeepCopy(RQueue &tree1, RQueue &tree2) {
        bool result = deepCopyCheck(tree1.m_heap, tree2.m_heap);
        return result;
    }

    // deepCopyCheck(Node *heap1, Node *heap2)
    // recursively in order checks the addresses and student info of both
    bool deepCopyCheck(Node *heap1, Node *heap2) {
        
        // checks if both heap1 and heap2 are nullptr
        if (heap1 == nullptr) {
            return (heap2 == nullptr) ? true : false;
        }

        // checks if addresses are different and if student value is the same
        bool result = (&heap1 != &heap2 && heap1->getStudent() == heap2->getStudent()) ? true : false; 

        // recursively goes throught both heaps to check for address and value
        result = deepCopyCheck(heap1->m_left, heap2->m_left);
        result = deepCopyCheck(heap1->m_right, heap2->m_right);

        return result;
    }

    // testEmpty(RQueue &tree)
    bool testEmpty(RQueue &tree) {
        return (tree.m_heap == nullptr) ? true : false;
    }
    
};

int main(){
    Random nameGen(97,122);Random levelGen(0,3);
    Random majorGen(0,4);Random groupGen(0,3);
    Random raceGen(0,2);Random genderGen(0,2);
    Random incomeGen(0,4);Random highschoolGen(0,2);
    Tester test;
    
    cout << "\n=======INSERT TESTS=======\n";

    {
        RQueue leftistQueue(priorityFn2, MINHEAP, LEFTIST);

        // creates and inserts random students
        for (int i = 0; i < 300; i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            leftistQueue.insertStudent(student);
        }

        string result = (test.testLeftistHeap(leftistQueue)) ? "PASSED" : "FAILED";
        cout << "Leftist Min Heap insertions:\t" << result << "\n";

    }

    {
        RQueue skewQueue(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i = 0; i < 300; i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            skewQueue.insertStudent(student);
        }

        string result = (test.testSkewHeapInserts(skewQueue)) ? "PASSED" : "FAILED";
        cout << "Skew Min Heap insertions:\t" << result << "\n";

    }

    {
        RQueue aQueue(priorityFn1, MAXHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<10;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            aQueue.insertStudent(student);
        }

        string result = (test.testSkewHeapInserts(aQueue)) ? "PASSED" : "FAILED";
        cout << "Skew Max Heap insertions:\t" << result << "\n";

    }
    
    cout << "\n=======CONVERSION TESTS=======\n";
    {
        RQueue skewQueue(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<3;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            skewQueue.insertStudent(student);
        }

        skewQueue.setPriorityFn(priorityFn1, MAXHEAP);

        string result = (test.testSkewHeapInserts(skewQueue)) ? "PASSED" : "FAILED";
        cout << "Skew Min Heap to Skew Max Heap conversion:\t" << result << "\n";
    }

    {
        RQueue skewQueue(priorityFn1, MAXHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<3;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            skewQueue.insertStudent(student);
        }

        skewQueue.setPriorityFn(priorityFn2, MINHEAP);

        string result = (test.testSkewHeapInserts(skewQueue)) ? "PASSED" : "FAILED";
        cout << "Skew Max Heap to Skew Min Heap conversion:\t" << result << "\n";
    }

    {
        RQueue changeQueue(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            changeQueue.insertStudent(student);
        }

        changeQueue.setStructure(LEFTIST);
        
        string result = (test.testLeftistHeap(changeQueue)) ? "PASSED" : "FAILED";
        cout << "Skew Min Heap to Leftist Min Heap conversion:\t" << result << "\n";
    }

    {
        RQueue changeQueue(priorityFn2, MINHEAP, LEFTIST);

        // creates and inserts random students
        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            changeQueue.insertStudent(student);
        }

        changeQueue.setStructure(SKEW);
        
        string result = (test.testSkewHeapInserts(changeQueue)) ? "PASSED" : "FAILED";
        cout << "Leftist Min Heap to Skew Min Heap conversion:\t" << result << "\n";
    }

    cout << "\n=======COPY CONSTRUCTOR AND OVERLOADED ASSIGNMENT=======\n";
    {
        RQueue queue1(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue1.insertStudent(student);
        }

        RQueue queue2(queue1);
        
        string result = (test.testDeepCopy(queue1, queue2)) ? "PASSED" : "FAILED";
        cout << "Copy Constructor test:\t" << result << "\n";
    }

    {
        RQueue emptyQueue(priorityFn2, MINHEAP, SKEW); //empty Queue

        RQueue queue(emptyQueue);
        
        string result = (test.testEmpty(queue)) ? "PASSED" : "FAILED";
        cout << "Copy Constructor edge case:\t" << result << "\n";
    }
    
    {
        RQueue queue1(priorityFn2, MINHEAP, SKEW);
        RQueue queue2(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue1.insertStudent(student);
        }

        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue2.insertStudent(student);
        }

        queue1 = queue2;
        
        string result = (test.testDeepCopy(queue1, queue2)) ? "PASSED" : "FAILED";
        cout << "Overloaded assignment operator test:\t" << result << "\n";
    }

    {
        RQueue queue(priorityFn2, MINHEAP, SKEW);
        RQueue emptyQueue(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i=0;i<5;i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue.insertStudent(student);
        }

        queue = emptyQueue;
        
        string result = (test.testEmpty(queue)) ? "PASSED" : "FAILED";
        cout << "Overloaded assignment operator edge case:\t" << result << "\n";
    }

    cout << "\n=======MERGE TESTS=======\n";
    {
        RQueue queue1(priorityFn2, MINHEAP, SKEW);
        RQueue queue2(priorityFn2, MINHEAP, SKEW);

        // creates and inserts random students
        for (int i = 0; i < 5; i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue1.insertStudent(student);
        }

        queue1.mergeWithQueue(queue2);
        
        string result = (test.testSkewHeapInserts(queue1)) ? "PASSED" : "FAILED";
        cout << "Merge edge case (one heap empty):\t" << result << "\n";
    }

    {
        RQueue queue1(priorityFn2, MINHEAP, SKEW);
        RQueue queue2(priorityFn1, MAXHEAP, SKEW);

        // creates and inserts random students
        for (int i = 0; i < 5; i++){
            Student student(nameGen.getRandString(5), levelGen.getRandNum(),
                        majorGen.getRandNum(), groupGen.getRandNum(),
                        raceGen.getRandNum(), genderGen.getRandNum(),
                        incomeGen.getRandNum(), highschoolGen.getRandNum());
            queue1.insertStudent(student);
        }

        string result;

        try {
            queue1.mergeWithQueue(queue2);
            result = "FALED";
        }
        catch (domain_error &e) {
            result = "PASSED";
        }

        cout << "Merge error case (different priority functions):\t" << result << "\n";
    }

    cout << "\n=======REMOVAL TESTS=======\n";
    {
        RQueue emptyQueue(priorityFn2, MINHEAP, LEFTIST);

        string result;

        try {
            emptyQueue.getNextStudent();
            result = "FAILED"; // if getNextStudent() didnt throw a error case, then failed
        }
        catch (out_of_range &e) {
            result = "PASSED";
        }

        cout << "Removal error case (heap empty):\t" << result << "\n";

    }

    return 0;
}


int priorityFn1(const Student & student) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-10]
    //level + major + group
    //the highest priority would be 3+4+3 = 10
    //the lowest priority would be 0+0+0 = 0
    //the larger value means the higher priority
    int priority = student.getLevel() + student.getMajor() + student.getGroup();
    return priority;
}

int priorityFn2(const Student & student) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-10]
    //race + gender + income + highschool
    //the highest priority would be 0+0+0+0 = 0
    //the lowest priority would be 2+2+4+2 = 10
    //the smaller value means the higher priority
    int priority = student.getRace() + student.getGender() + student.getIncome() + student.getHighschool();
    return priority;
}
