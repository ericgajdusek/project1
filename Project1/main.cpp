#include "main.h"

int create(int priority) {
    if (priority < 0 || priority > 2) {
        std::cout << "error" << std::endl;
        return -1;
    }
    for (int i = 0 ; i < 16 ; i++) {
        if (pcb[i] == nullptr) {
            // create new process
            pcb[i] = new Process(priority, running);
            // add process to parent's child list
            if (running >= 0) pcb[running]->children->push_back(i);
            
            // add to ready list
            rl[priority].push_back(i);

            std::cout << "process " << i << " created" << std::endl;
            if (running >= 0 && pcb[i]->priority > pcb[running]->priority) {
                scheduler();
            }
            return 0;
        }
    }
    return -1;
}

void destroy(int proc) {
    if (!pcb[proc]) {
        std::cout << "error" << std::endl;
        return;
    }
    int procs_destroyed = 0;
    // destroy all children
    while (!pcb[proc]->children->empty()) {
        int child = pcb[proc]->children->front();
        pcb[proc]->children->remove(child);
        procs_destroyed += destroy_child(child);
    }

    //remove from parent's children list
    pcb[pcb[proc]->parent]->children->remove(proc);

    // remove from ready list
    rl[pcb[proc]->priority].remove(proc);

    Node *curr = pcb[proc]->resources.head;
    int i = 0;
    while (curr) {
        // remove from resource waitlists
        rcb[i]->waitlist.remove_by_value(proc);
        // release all resources
        int temp = running;
        running = proc;
        release(curr->index, curr->amount);
        running = temp;
        curr = curr->next;
        i++;
    }

    // destroy process
    delete pcb[proc]->children;
    delete pcb[proc];
    pcb[proc] = nullptr;
    procs_destroyed++;

    std::cout << procs_destroyed << " processes destroyed" << std::endl;
    scheduler();
}

int destroy_child(int proc) {
    int procs_destroyed = 0;
    // destroy all children
    while (!pcb[proc]->children->empty()) {
        int child = pcb[proc]->children->front();
        pcb[proc]->children->remove(child);
        destroy_child(child);
        procs_destroyed++;
    }

    //remove from parent's children list
    pcb[pcb[proc]->parent]->children->remove(proc);

    // remove from ready list
    rl[pcb[proc]->priority].remove(proc);

    Node *curr = pcb[proc]->resources.head;
    int i = 0;
    while (curr) {
        // remove from resource waitlists
        rcb[i]->waitlist.remove_by_value(proc);
        // release all resources
        int temp = running;
        running = proc;
        release(curr->index, curr->amount);
        running = temp;
        curr = curr->next;
        i++;
    }

    // destroy process
    delete pcb[proc]->children;
    delete pcb[proc];
    pcb[proc] = nullptr;
    procs_destroyed++;
    return procs_destroyed;
}

void request(int resource, int num) {
    if (resource < 0 || resource > 3) {
        std::cout << "error" << std::endl;
        return;
    }
    if (rcb[resource]->state >= num) {
        rcb[resource]->state -= num;
        // add resource to process resources
        pcb[running]->resources.add_to_list(resource, num);
    } else {
        // remove process from ready list and add to resource waitlist
        rl[pcb[running]->priority].remove(running);
        rcb[resource]->waitlist.add_to_list(running, num);

        // change process state to blocked
        pcb[running]->state = STATE_BLOCKED;
        std::cout << "process " << running << " blocked" << std::endl;
        scheduler();
    }
}

void release(int resource, int num) {
    
    Node *curr;
    // check that process has resource
    if ((curr = pcb[running]->resources.find(resource)) == nullptr) {
        std::cout << "error" << std::endl;
        return;
    }
    if (curr->amount > num) {
        // remove num units of resource from process
        curr->amount -= num;
    } else if (curr->amount == num) {
        // remove from resource list of current process
        pcb[running]->resources.remove_by_value(resource);
        
    } else {
        // trying to release too many units
        std::cout << "error" << std::endl;
        return;
    }

    // if no processes on waitlist
    if (rcb[resource]->waitlist.head == nullptr) {
        rcb[resource]->state += num;
    } else {
        while (true) {
            int next_proc = rcb[resource]->waitlist.head->index;
            int requested = rcb[resource]->waitlist.head->amount;

            if (rcb[resource]->state + num < requested) {
                break;
            }

            // add resource to next process resource list
            pcb[next_proc]->resources.add_to_list(resource, requested);
            // decrement requested resource from waitlist head
            rcb[resource]->waitlist.head->amount -= requested;
            // remove next process from waitlist
            rcb[resource]->waitlist.remove_from_head();

            // add next process to ready list
            rl[pcb[next_proc]->priority].push_back(next_proc);
            
            // next proc ready
            pcb[next_proc]->state = STATE_READY;
            break;
        }
    }
    std::cout << "resource " << resource << " released" << std::endl;
    scheduler();
}

void timeout() {
    int p = pcb[running]->priority;
    // remove process from ready list
    rl[p].remove(running);

    // add process to back of ready list
    rl[p].push_back(running);
    scheduler();
}

void scheduler() {
    for (int i = 2 ; i >= 0 ; i--) {
        if (!rl[i].empty()) {
            running = rl[i].front();
            break;
        }
    }
    std::cout << "process " << running << " running" << std::endl;
}

void init() {
    // reset pcb
    for (int i = 0; i < 16; ++i) {
        if (pcb[i] != nullptr) {
            delete pcb[i];
            pcb[i] = nullptr;
        }
    }
    

    // reset rcb
    for (int i = 0 ; i < 4 ; i++) {
        rcb[i] = new Resource();
        rcb[i]->inventory = (i < 2) ? 1 : (i == 2 ? 2 : 3);
        rcb[i]->state = rcb[i]->inventory;
        rcb[i]->waitlist.resetList();
    }

    

    // reset ready list
    for (int i = 0 ; i < 3 ; i++) {
        rl[i].clear();
    }
    running = -1;

    create(0);
    scheduler();
}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);

        std::string command;
        int arg1, arg2;

        iss >> command;


        if (command == "in") {
            
            init();
        } else if (command == "cr") {
            if (iss >> arg1) {
                create(arg1);
            } else {
                std::cout << "Usage: create <int>\n";
            }
        } else if (command == "de") {
            if (iss >> arg1) {
                destroy(arg1);
            } else {
                std::cout << "Usage: destroy <int>\n";
            }
        } else if (command == "rq") {
            if (iss >> arg1 >> arg2) {
                request(arg1, arg2);
            } else {
                std::cout << "Usage: request <int> <int>\n";
            }
        } else if (command == "rl") {
            if (iss >> arg1 >> arg2) {
                release(arg1, arg2);
            } else {
                std::cout << "Usage: release <int> <int>\n";
            }
        } else if (command == "to") {
            timeout();
        } else {
            std::cout << "Unknown command\n";
        }
    }
        
    return 0;
}