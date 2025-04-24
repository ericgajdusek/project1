#include <iostream>
#include <list>
#include <algorithm>
#include <sstream>

#define STATE_BLOCKED 0
#define STATE_READY 1

// ResourceList
struct Node {
    int index; 
    int amount;  
    Node* next;  

    // constructor
    Node(int p, int n) : index(p), amount(n), next(nullptr) {}
};
class ResourceList {
public:
    Node* head;

    // constructor
    ResourceList() : head(nullptr) {}

    // functoin to add a node to the back of the list
    void add_to_list(int p, int n) {
        // if already in list, add to amount (for resource list not waitlist)
        Node* temp = head;
        while (temp) {
            if (temp->index == p) {
                temp->amount += n;
                return;
            }
            temp = temp->next;
        }
        Node* newNode = new Node(p, n);  
        if (head == nullptr) {
            head = newNode; 
        } else {
            Node* current = head;
            while (current->next != nullptr) { 
                current = current->next;
            }
            current->next = newNode; 
        }
    }

    // function to remove a node from the front of the list
    void remove_from_head() {
        if (head == nullptr) {
            return;
        }
        Node* temp = head;  
        head = head->next;  
        delete temp; 
    }

    void remove_by_value(int value) {
        while (head != nullptr && head->index == value) {
            remove_from_head();
        }
        Node* current = head;
        while (current != nullptr && current->next != nullptr) {
            if (current->next->index == value) {
                Node* temp = current->next;
                current->next = temp->next;
                delete temp;
            } else {
                current = current->next;
            }
        }
    }

    Node *find(int i) {
        Node* current = head;
        while (current) { 
            if (current->index == i) {
                return current;
            }
            current = current->next;
        }  
        return nullptr;
    }

    // function to reset list
    void resetList() {
        // delete all nodes before resetting the head
        while (head != nullptr && head->next != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        // reset head
        head = nullptr; 
    }

    ~ResourceList() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// process struct
struct Process {
    int priority;
    int state;
    int parent;
    std::list<int> *children;
    ResourceList resources;

    Process(int prior, int par) {
        priority = prior;
        state = 0;
        parent = par;
        children = new std::list<int>();
    }
};

// resource struct
struct Resource {
    int inventory;
    int state;
    ResourceList waitlist;
};

// Ready list (array of linked lists)
std::list<int> rl[3];

// PCB
Process *pcb[16];

//RCB
Resource *rcb[4];

// current running proc
int running;

// Functions

int create(int priority);

void destroy(int proc);
int destroy_child(int proc);

void request(int resource, int num);

void release(int resource, int num);

void timeout();

void scheduler();

void init();