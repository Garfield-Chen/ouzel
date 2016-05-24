// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <cstdint>

namespace ouzel
{
    template<class T>
    class Array
    {
    public:
        typedef typename std::vector<T*>::iterator iterator;
        typedef typename std::vector<T*>::const_iterator const_iterator;
        typedef typename std::vector<T*>::reverse_iterator reverse_iterator;
        typedef typename std::vector<T*>::const_reverse_iterator const_reverse_iterator;

        Array()
        {
        }

        virtual ~Array()
        {
            this->free();
        }

        Array(const Array& other):
            elements(other.elements)
        {
            for (T* element : addList)
            {
                elements.push_back(element);
            }

            for (T* element : removeList)
            {
                auto i = std::find(elements.begin(), elements.end(), element);

                if (i != elements.end())
                {
                    elements.erase(i);
                }
            }

            for (T* element : elements)
            {
                element->retain();
            }
        }

        Array& operator=(const Array& other)
        {
            this->free();

            elements = other.elements;

            for (T* element : addList)
            {
                elements.push_back(element);
            }

            for (T* element : removeList)
            {
                auto i = std::find(elements.begin(), elements.end(), element);

                if (i != elements.end())
                {
                    elements.erase(i);
                }
            }

            for (T* element : elements)
            {
                element->retain();
            }

            return *this;
        }

        Array(const Array&& other):
            elements(std::move(other.elements)),
            addList(std::move(other.addList)),
            removeList(std::move(other.removeList)),
            locked(other.locked)
        {
            other.locked = 0;
        }

        Array& operator=(const Array&& other)
        {
            this->free();

            elements = std::move(other.elements);
            addList = std::move(other.addList);
            removeList = std::move(other.removeList);
            locked = other.locked;

            other.locked = 0;

            return *this;
        }


        void pushBack(T* element)
        {
            element->retain();

            if (locked == 0)
            {
                forcePushBack(element);
            }
            else
            {
                addList.insert(element);

                auto i = removeList.find(element);

                if (i != removeList.end())
                {
                    removeList.erase(i);
                    (*i)->release();
                }
            }
        }

        void erase(T* element)
        {
            if (locked == 0)
            {
                forceErase(element);
            }
            else
            {
                removeList.insert(element);

                auto i = addList.find(element);

                if (i != addList.end())
                {
                    addList.erase(i);
                    (*i)->release();
                }
            }
        }

        void clear()
        {
            if (locked == 0)
            {
                for (T* element : elements)
                {
                    element->release();
                }

                elements.clear();
            }
            else
            {
                for (T* element : elements)
                {
                    erase(element);
                }
            }
        }

        void forcePushBack(T* element)
        {
            element->retain();
            elements.push_back(element);
        }

        void forceErase(T* element)
        {
            auto i = std::find(elements.begin(), elements.end(), element);

            if (i != elements.end())
            {
                (*i)->release();
                elements.erase(i);
            }
        }

        bool hasElement(T* element)
        {
            auto elementIterator = std::find(elements.begin(), elements.end(), element);
            auto addListIterator = addList.find(element);
            auto removeListiterator = removeList.find(element);

            return (elementIterator != elements.end() || addListIterator != addList.end()) && removeListiterator == removeList.end();
        }

        bool empty() const { return elements.empty() && addList.empty(); }

        T* operator[](size_t index) { return elements[index]; }
        iterator begin() { return elements.begin(); }
        iterator end() { return elements.end(); }
        const_iterator begin() const { return elements.begin(); }
        const_iterator end() const { return elements.end(); }
        reverse_iterator rbegin() { return elements.rbegin(); }
        reverse_iterator rend() { return elements.rend(); }
        const_reverse_iterator rbegin() const { return elements.rbegin(); }
        const_reverse_iterator rend() const { return elements.rend(); }

        void lock()
        {
            ++locked;
        }

        void unlock()
        {
            if (--locked == 0)
            {
                if (!addList.empty())
                {
                    for (T* element : addList)
                    {
                        forcePushBack(element);
                    }
                    addList.clear();
                }

                if (!removeList.empty())
                {
                    for (T* element : removeList)
                    {
                        forceErase(element);
                    }
                    removeList.clear();
                }
            }
        }

    private:
        void free()
        {
            for (T* element : elements)
            {
                element->release();
            }

            elements.clear();

            for (T* element : addList)
            {
                element->release();
            }

            elements.clear();

            for (T* element : removeList)
            {
                element->release();
            }

            elements.clear();
        }

        std::vector<T*> elements;
        int32_t locked = 0;
        std::set<T*> addList;
        std::set<T*> removeList;
    };
}
