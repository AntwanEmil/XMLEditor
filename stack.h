#ifndef STACK_H
#define STACK_H

#include <list>
#include <utility>

template<class T>
class Stack {
public:
    Stack(unsigned int max = 2) : m_max(max) {}

    void clear(bool doDel = true) {while (!m_list.empty()) pop(doDel);}

    void push(T *item, int textState);
    void pop(bool doDel = true);

    std::pair<T*, int> top() const {return m_list.back();}

    unsigned int size() const {return m_list.size();}
    bool empty() const {return m_list.empty();}
private:
    std::list<std::pair<T*, int>> m_list;
    unsigned int m_max;
};

template<class T>
void Stack<T>::pop(bool doDel) {
    if (!m_list.empty()) {
        if (doDel)
            delete m_list.back().first;
        m_list.pop_back();
    }
}

template<class T>
void Stack<T>::push(T *item, int textState) {
    m_list.push_back(std::pair<T*, int>(item, textState));
    while (m_list.size() > m_max) {
        delete m_list.front().first;
        m_list.pop_front();
    }
}

#endif // STACK_H
