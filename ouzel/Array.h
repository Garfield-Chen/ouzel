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
            for (T* element : elements)
            {
                element->retain();
            }
        }

        Array& operator=(const Array& other)
        {
            this->free();

            elements = other.elements;

            for (T* element : elements)
            {
                element->retain();
            }

            return *this;
        }

        Array(const Array&& other):
            elements(std::move(other.elements))
        {

        }

        Array& operator=(const Array&& other)
        {
            this->free();

            elements = std::move(other.elements);

            return *this;
        }


        void pushBack(T* element)
        {
            element->retain();

            elements.push_back(element);
        }

        void erase(T* element)
        {
            auto i = std::find(elements.begin(), elements.end(), element);

            if (i != elements.end())
            {
                (*i)->release();
                elements.erase(i);
            }
        }

        void clear()
        {
            for (T* element : elements)
            {
                element->release();
            }

            elements.clear();
        }

        bool hasElement(T* element)
        {
            auto elementIterator = std::find(elements.begin(), elements.end(), element);

            return elementIterator != elements.end();
        }

        bool empty() const { return elements.empty(); }
        size_t size() const { return elements.size(); }

        T* operator[](size_t index) { return elements[index]; }
        iterator begin() { return elements.begin(); }
        iterator end() { return elements.end(); }
        const_iterator begin() const { return elements.begin(); }
        const_iterator end() const { return elements.end(); }
        reverse_iterator rbegin() { return elements.rbegin(); }
        reverse_iterator rend() { return elements.rend(); }
        const_reverse_iterator rbegin() const { return elements.rbegin(); }
        const_reverse_iterator rend() const { return elements.rend(); }

    private:
        void free()
        {
            for (T* element : elements)
            {
                element->release();
            }

            elements.clear();
        }

        std::vector<T*> elements;
    };
}
