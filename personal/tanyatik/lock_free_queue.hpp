#pragma once

#include <atomic>

namespace tanyatik {

template<typename T>
class LockFreeQueue {
private:
    struct ListNode {
        T data;
        std::atomic<ListNode *> next;

        ListNode(T data) :
            data(data),
            next(nullptr)
            {}
    };

    struct QueueSnapshot;

    typedef std::atomic<ListNode *> ListNodePtr;
    typedef std::atomic<QueueSnapshot *> QueueSnapshotPtr;

    struct QueueSnapshot {
        ListNodePtr push_queue;
        ListNodePtr pop_queue;
        ListNodePtr to_delete;
        QueueSnapshot *next_old_snapshot;    
    };

    QueueSnapshotPtr current_snapshot_;
    std::atomic<int> active_threads_;

    QueueSnapshotPtr old_snapshot_list_;

public: 
    LockFreeQueue() :
        current_snapshot_(new QueueSnapshot()),
        active_threads_(0),
        old_snapshot_list_(nullptr)
        {}

    ~LockFreeQueue() {
        QueueSnapshot *current = current_snapshot_.load();
        incActiveThreads();
        decActiveThreads();
        deleteList(current->push_queue);
        deleteList(current->pop_queue);
        delete current;
    }

    void push(T data) {
        ListNode *node(new ListNode(data));
        // make a copy
        QueueSnapshot *new_snapshot(new QueueSnapshot());

        incActiveThreads();
        
        while (true) {
            // try update current_snapshot_ with new_snapshot,
            // which is copied current_snapshot_ with new_node
            QueueSnapshot *current = current_snapshot_.load();

            new_snapshot->push_queue = node;
            node->next = current->push_queue.load();

            new_snapshot->pop_queue = current->pop_queue.load();

            // expect new_snapshot in current_snapshot_, if yes, put current into current_snapshot_
            if (current_snapshot_.compare_exchange_weak(current, new_snapshot)) {
                decActiveThreads();
                addOldSnapshot(current);
                break;
            }  
        }
    }

    bool tryPop(T *data) {
        incActiveThreads();        
        QueueSnapshot *new_snapshot = nullptr;

        while(true) {
            QueueSnapshot *current = current_snapshot_.load();

            if (current->pop_queue == nullptr && current->push_queue == nullptr) {
                if (new_snapshot) {
                    delete new_snapshot;
                }
                decActiveThreads();
                return false;
            }

            if (current->pop_queue != nullptr) {
                // can use first element from pop_queue as data
                auto pop_queue = current->pop_queue.load();

                // make new version of current_snapshot (without first element of pop_queue) 
                if (!new_snapshot) {
                    new_snapshot = new QueueSnapshot();
                }

                new_snapshot->push_queue = current->push_queue.load();
                new_snapshot->pop_queue.store(pop_queue->next);

                if (current_snapshot_.compare_exchange_weak(current, new_snapshot)) {
                    *data = pop_queue->data;
                    pop_queue->next = nullptr;                
    
                    addToDeleteList(current, pop_queue);                
                    addOldSnapshot(current);

                    decActiveThreads();

                    return true;
                } else {
                    continue;
                }
            }

            // make new version of snapshot with pop_queue as reversed push_queue
            // in next iteration of this cycle, we will take an element from pop_queue
            if (!new_snapshot) {
                new_snapshot = new QueueSnapshot();
            }
            
            new_snapshot->push_queue = nullptr; 

            ListNode *reversed_list(makeReversedList(current->push_queue)); 

            new_snapshot->pop_queue.store(reversed_list);
            if (current_snapshot_.compare_exchange_weak(current, new_snapshot)) {
                addToDeleteList(current, current->push_queue);
                current->push_queue = nullptr;
                addOldSnapshot(current);

                new_snapshot = nullptr;
            } else {
                // reversed list is no longer needed
                deleteList(new_snapshot->pop_queue);
                continue;
            }
        }
    }

    void debugPrint() const {
        std::cout << "Actual snapshot: " << std::endl;
        debugPrint(current_snapshot_.load());
        std::cout << "Former snapshot: " << std::endl;
        debugPrint(old_snapshot_list_.load());
    }

    bool empty() {
        incActiveThreads();
        auto snapshot = current_snapshot_.load();
        bool empty = snapshot->push_queue == nullptr && snapshot->pop_queue == nullptr;
        decActiveThreads();
        return empty;
    }

private:
    static void debugPrint(ListNode *list) {
        std::cout << "{ "; 
        while (list != nullptr) {
            std::cout << list << ":" << list->data << " ";
            list = list->next;
        }
        std::cout << " }";
    }

    static void debugPrint(QueueSnapshot *snapshot) {
        std::cout << "Snapshot: " << snapshot << std::endl;
        if (snapshot == nullptr) {
            return;
        }

        std::cout << "push queue: ";
        debugPrint(snapshot->push_queue);
        std::cout << std::endl;

        std::cout << "pop queue: ";
        debugPrint(snapshot->pop_queue);
        std::cout << std::endl;
        
        std::cout << "to delete: ";
        debugPrint(snapshot->to_delete);
        std::cout << std::endl;

        std::cout << "next old snapshot: ";
        debugPrint(snapshot->next_old_snapshot);
    }

    static ListNode *makeReversedList(ListNode *list) {
        ListNode *straight = list;
        ListNode *reversed = nullptr;

        while (straight != nullptr) {
            auto current_reversed = new ListNode(straight->data);
            current_reversed->next = reversed;

            reversed = current_reversed;
            straight = straight->next;
        }

        return reversed;
    }

    static void addToDeleteList(QueueSnapshot *snapshot, ListNode *element) {
        while (true) {
            auto to_delete = snapshot->to_delete.load();
        
            if (snapshot->to_delete.compare_exchange_weak(to_delete, element)) {
                break;
            }
        }
    }

    void addOldSnapshot(QueueSnapshot *current) {
        while(true) {
            current->next_old_snapshot = old_snapshot_list_.load();
    
            if (old_snapshot_list_.compare_exchange_weak(current->next_old_snapshot, current)) {
                break;    
            } 
        }
    }

    void tryDeleteOldSnapshots() {
        QueueSnapshot *current_old = old_snapshot_list_.load();

        if (!current_old) {
            return;
        }

        // if the only thread tries to work with queue
        if (active_threads_ == 1) {
            // expect nullptr, if yes, put current_old into old_snapshot_list_
            
            QueueSnapshot *desired = nullptr;
            if (old_snapshot_list_.compare_exchange_strong(current_old, desired)) {
                while (current_old) {
                    auto tmp = current_old->next_old_snapshot;
                    deleteList(current_old->to_delete);
                    delete current_old;
                    current_old = tmp;
                }
            }
        }
    }

    static void deleteList(ListNode *node_list) {
        ListNode *pointer = node_list;
        while (pointer) {
            auto deleted = pointer;
            pointer = pointer->next;
            delete deleted;
        }
    }

    void incActiveThreads() {
        ++active_threads_;
    }

    void decActiveThreads() {
        tryDeleteOldSnapshots();
        --active_threads_;
    }
};

} // namespace tanyatik
