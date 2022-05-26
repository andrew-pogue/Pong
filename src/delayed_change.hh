#pragma once
#include <list>
#include <iterator>

template <typename T>
struct DelayedChange {
    T &var;
    T val;
    unsigned delay = 18; // in frames
};

// The elements of the list are applied and then erased if their delay is 0.
// Otherwise, their delay is reduced by 1.
template <typename T>
void process_delayed_changes(std::list<DelayedChange<T>> &delayed_change_list) {
    std::list<typename std::list<DelayedChange<T>>::iterator> queue_for_erase{};
    
    for (auto it = delayed_change_list.begin();
        it != delayed_change_list.end();
        it++)
    {
        if (it->delay < 1) {
            it->var = it->val;
            queue_for_erase.push_front(it);
        } else {
            it->delay--;
        }
    }
    
    while(!queue_for_erase.empty()) {
        delayed_change_list.erase(queue_for_erase.front());
        queue_for_erase.pop_front();
    }
}