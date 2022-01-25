#pragma once
#include <vector>
#include <iterator>
#include <cmath>
#include <stdexcept>
#include <iostream>

template <typename T>
class Deque
{
    //Deque structure
    //[firstBlockInd...lastBlockInd]->[0...innerArraySize - 1]
    //[firstInd][BlockWithValues][lastInd]
private:
    static const int innerArraySize = 64;
    int pivotPosition = 0;
    int lastInd = (innerArraySize - 1) / 2;
    int firstInd = (innerArraySize - 1) / 2 - 1;
    int firstBlockInd = 0;
    int lastBlockInd = 0;
    uint64_t sz = 0;
    uint64_t cap = innerArraySize;
    std::vector<T*> arr;

    T* makeBlock()
    {
        T* elem = reinterpret_cast<T*>(new int8_t[innerArraySize * sizeof(T)]);
        return elem;
    }
    void resize()
    {
        uint64_t newSize = arr.size() * 2;
        std::vector<T*> newArr(newSize);
        uint64_t newPivot = newSize / 2 - 1;
        int offLeft = firstBlockInd - pivotPosition;
        int offRight = lastBlockInd - pivotPosition;
        pivotPosition = newPivot;
        uint64_t block = newPivot + offLeft;
        for (int i = firstBlockInd; i <= lastBlockInd; ++i, ++block)
        {
            newArr[block] = arr[i];
        }
        for(int i = 0; i < firstBlockInd; ++i)
        {
            delete arr[i];
        }
        for(int i = lastBlockInd + 1; i < static_cast<int>(arr.size()); ++i)
        {
            delete arr[i];
        }
        firstBlockInd = static_cast<int>(newPivot + offLeft);
        lastBlockInd = static_cast<int>(newPivot + offRight);
        try
        {
            for (auto& elem : newArr)
            {
                if (elem == nullptr)
                    elem = makeBlock();
            }
        } catch (...)
        {
            for(auto& elem : newArr)
            {
                bool same = false;
                for(auto& oldElem : arr)
                {
                    if(elem == oldElem)
                    {
                        same = true;
                        break;
                    }
                }
                if(same)
                {
                    continue;
                }
                delete elem;
            }
            throw;
        }
        arr = newArr;
        cap = arr.size() * innerArraySize;
    }
public:
    Deque()
    {
        arr.resize(2);
        cap = innerArraySize * 2;
        arr[0] = makeBlock();
        arr[1] = makeBlock();
    }

    Deque(int n, T elem = T())
    {
        int outerSize = ceil(static_cast<double>(n) / static_cast<double>(innerArraySize));
        int capacity = 2;
        int pow = 0;
        while(capacity < outerSize)
        {
            ++pow;
            capacity *= 2;
        }
        arr.resize(capacity);
        pivotPosition = floor(static_cast<double>(capacity) / 2.0);
        try
        {
            for (auto& elem : arr)
            {
                elem = makeBlock();
            }
        }
        catch (...)
        {
            for(auto& elem : arr)
            {
                delete elem;
            }
            throw;
        }
        firstBlockInd = pivotPosition;
        lastBlockInd = firstBlockInd;
        sz = n;
        cap = arr.size() * innerArraySize;
        firstBlockInd = arr.size() / 4;
        lastBlockInd = firstBlockInd;
        if(n >= innerArraySize)
        {
            firstInd = innerArraySize - 1;
        }
        else
        {
            firstInd = (innerArraySize - 1) / 2 - 1;
        }
        firstInd -= (n - (n / innerArraySize) * innerArraySize) / 2;
        lastInd = firstInd;
        try
        {
            for (int i = 0; i < n; ++i)
            {
                ++lastInd;
                if (lastInd >= innerArraySize)
                {
                    lastInd = 0;
                    ++lastBlockInd;
                }
                arr[lastBlockInd][lastInd] = elem;
            }
            ++lastInd;
        }
        catch (...)
        {
            for(auto& elem : arr)
            {
                delete elem;
            }
            throw;
        }
    }
    Deque(const Deque& another)
    {
        pivotPosition = another.pivotPosition;
        lastInd = another.lastInd;
        lastBlockInd = another.lastBlockInd;
        firstBlockInd = another.firstBlockInd;
        firstInd = another.firstInd;
        sz = another.sz;
        cap = another.cap;
        arr.resize(another.arr.size());
        try
        {
            for (size_t i = 0; i < arr.size(); ++i)
            {
                arr[i] = makeBlock();
                std::copy(another.arr[i], another.arr[i] + innerArraySize, arr[i]);
            }
        }
        catch (...)
        {
            for(auto& elem : arr)
            {
                delete elem;
            }
            throw;
        }
    }

    Deque& operator=(const Deque& another)
    {
        if(this == &another)
            return *this;
        for(auto& elem : arr)
        {
            delete elem;
        }
        pivotPosition = another.pivotPosition;
        lastInd = another.lastInd;
        lastBlockInd = another.lastBlockInd;
        firstBlockInd = another.firstBlockInd;
        firstInd = another.firstInd;
        sz = another.sz;
        cap = another.cap;
        arr.resize(another.arr.size());
        try
        {
            for (size_t i = 0; i < arr.size(); ++i)
            {
                arr[i] = makeBlock();
                std::copy(another.arr[i], another.arr[i] + innerArraySize, arr[i]);
            }
        }
        catch (...)
        {
            for(auto& elem : arr)
            {
                delete elem;
            }
            throw;
        }
        return *this;
    }

    ~Deque()
    {
        for(auto& elem : arr)
        {
            delete elem;
        }
    }

    uint64_t size() const noexcept
    {
        return sz;
    }

    T& operator[](size_t ind)
    {
        //std::cerr << "Access by index: " << ind << "\n";
        ++ind;
        long long off = 0;
        if(firstInd + ind >= innerArraySize)
        {
            ++off;
            ind -= innerArraySize - firstInd;
        }
        else
        {
            ind += firstInd;
        }
        off += ind / innerArraySize;
        ind %= innerArraySize;
        return arr[firstBlockInd + off][ind];
    }

    const T& operator[](size_t ind) const
    {
        //std::cerr << "Access by index: " << ind << "\n";
        ++ind;
        long long off = 0;
        if(firstInd + ind >= innerArraySize)
        {
            ++off;
            ind -= innerArraySize - firstInd;
        }
        else
        {
            ind += firstInd;
        }
        off += ind / innerArraySize;
        ind %= innerArraySize;
        return arr[firstBlockInd + off][ind];
    }

    T& at(size_t ind)
    {
        if(ind >= sz)
        {
            throw std::out_of_range("Index out of range");
        }
        return (*this)[ind];
    }

    const T& at(size_t ind) const
    {
        if(ind >= sz)
        {
            throw std::out_of_range("Index out of range");
        }
        return (*this)[ind];
    }

    void push_back(const T& elem)
    {
        arr[lastBlockInd][lastInd] = elem;
        ++sz;
        ++lastInd;
        if (lastInd >= innerArraySize)
        {
            lastInd = 0;
            if (lastBlockInd + 1 >= static_cast<int>(arr.size()))
            {
                resize();
            }
            ++lastBlockInd;
        }
    }

    void push_front(const T& elem)
    {
        arr[firstBlockInd][firstInd] = elem;
        ++sz;
        --firstInd;
        if (firstInd < 0)
        {
            firstInd = innerArraySize - 1;
            if (firstBlockInd - 1 < 0)
            {
                resize();
            }
            --firstBlockInd;
        }
    
    }

    void pop_back()
    {
        if(sz != 0)
        {
            --lastInd;
            if (lastInd < 0)
            {
                lastInd = innerArraySize - 1;
                --lastBlockInd;
            }
            --sz;
        }
    }

    void pop_front()
    {
        if(sz != 0)
        {
            ++firstInd;
            if(firstInd >= innerArraySize)
            {
                firstInd = 0;
                ++firstBlockInd;
            }
            --sz;
        }
    }

    template<bool isConst>
    struct common_iterator
    {
        friend Deque;
    private:
        const int* pivot = nullptr;
        const std::vector<T*>* array = nullptr;
        int offset = 0;
        int ind = 0;
        common_iterator(const int* p, const std::vector<T*>* ptr, int blockInd, int pos) :
        pivot(p),
        array(ptr),
        offset(blockInd - *p),
        ind(pos)
        {
            if(pos >= innerArraySize)
            {
                pos -= innerArraySize;
                ++offset;
            }
            else if(pos < 0)
            {
                pos += innerArraySize;
                --offset;
            }
        }
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = int;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        common_iterator() = default;
        common_iterator(const common_iterator& other) = default;

        common_iterator& operator=(const common_iterator& other)
        {
            if(this == &other)
                return *this;
            pivot = other.pivot;
            array = other.array;
            offset = other.offset;
            ind = other.ind;
            return *this;
        }

        common_iterator& operator+=(int n)
        {
            ind += n;
            if(ind >= innerArraySize)
            {
                offset += ind / innerArraySize;
                ind %= innerArraySize;
            }
            return *this;
        }

        common_iterator& operator-=(int n)
        {
            ind -= n;
            if(ind < 0)
            {
                offset -= abs(ind) / innerArraySize + 1;
                ind += (abs(ind) / innerArraySize + 1) * innerArraySize;
            }
            return *this;
        }

        common_iterator& operator++()
        {
            *this += 1;
            return *this;
        }
        common_iterator& operator--()
        {
            *this -= 1;
            return *this;
        }
        common_iterator operator++(int)
        {
            common_iterator ci = *this;
            ci += 1;
            return ci;
        }
        common_iterator operator--(int)
        {
            common_iterator ci = *this;
            ci -= 1;
            return ci;
        }

        friend common_iterator operator+(const common_iterator& it, int n)
        {
            common_iterator ci = it;
            ci += n;
            return ci;
        }

        friend common_iterator operator-(const common_iterator& it, int n)
        {
            common_iterator ci = it;
            ci -= n;
            return ci;
        }

        friend int operator-(const common_iterator& it1, const common_iterator& it2)
        {
            if(it1.offset < it2.offset)
            {
                int diff = (innerArraySize - it1.ind) + it2.ind;
                diff += (it2.offset - it1.offset - 1) * innerArraySize;
                return -diff;
            }
            else if(it1.offset > it2.offset)
            {
                int diff = (innerArraySize - it2.ind) + it1.ind;
                diff += (it1.offset - it2.offset - 1) * innerArraySize;
                return diff;
            }
            else
            {
                return it1.ind - it2.ind;
            }
        }

        friend bool operator<(const common_iterator& it1, const common_iterator& it2)
        {
            return it1.array == it2.array && it1 - it2 < 0;
        }
        friend bool operator==(const common_iterator& it1, const common_iterator& it2)
        {
            return !(it1 < it2) && !(it2 < it1);
        }
        friend bool operator>(const common_iterator& it1, common_iterator& it2)
        {
            return !(it1 < it2) && !(it1 == it2);
        }
        friend bool operator<=(const common_iterator& it1, const common_iterator& it2)
        {
            return !(it1 > it2);
        }
        friend bool operator>=(const common_iterator& it1, const common_iterator& it2)
        {
            return !(it1 < it2);
        }
        friend bool operator!=(const common_iterator& it1, const common_iterator& it2)
        {
            return !(it1 == it2);
        }

        std::conditional_t<isConst, const T&, T&> operator*() const
        {
            return (*array)[*pivot + offset][ind];
        }

        std::conditional_t<isConst, const T*, T*> operator->() const
        {
            return (*array)[*pivot + offset] + ind;
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()
    {
        iterator it(&pivotPosition, &arr, firstBlockInd, firstInd + 1);
        return it;
    }
    iterator end()
    {
        iterator it(&pivotPosition, &arr, lastBlockInd, lastInd);
        return it;
    }
    const_iterator begin() const
    {
        const_iterator it(&pivotPosition, &arr, firstBlockInd, firstInd + 1);
        return it;
    }
    const_iterator end() const
    {
        const_iterator it(&pivotPosition, &arr, lastBlockInd, lastInd);
        return it;
    }
    const_iterator cbegin() const
    {
        const_iterator it(&pivotPosition, &arr, firstBlockInd, firstInd + 1);
        return it;
    }
    const_iterator cend() const
    {
        const_iterator it(&pivotPosition, &arr, lastBlockInd, lastInd);
        return it;
    }
    reverse_iterator rbegin()
    {
        iterator it(&pivotPosition, &arr, lastBlockInd, lastInd - 1);
        return reverse_iterator(it);
    }
    reverse_iterator rend()
    {
        iterator it(&pivotPosition, &arr, firstBlockInd, firstInd);
        return reverse_iterator(it);
    }
    const_reverse_iterator rbegin() const
    {
        const_iterator it(&pivotPosition, &arr, lastBlockInd, lastInd - 1);
        return const_reverse_iterator(it);
    }
    const_reverse_iterator rend() const
    {
        const_iterator it(&pivotPosition, &arr, firstBlockInd, firstInd);
        return const_reverse_iterator(it);
    }
    const_reverse_iterator crbegin() const
    {
        const_iterator it(&pivotPosition, &arr, lastBlockInd, lastInd - 1);
        return const_reverse_iterator(it);
    }
    const_reverse_iterator crend() const
    {
        const_iterator it(&pivotPosition, &arr, firstBlockInd, firstInd);
        return const_reverse_iterator(it);
    }

    void insert(iterator it, const T& elem)
    {
        if(it == this->end())
        {
            this->push_back(elem);
            return;
        }
        T tmp = *it;
        *it = elem;
        for(iterator iter = it + 1; iter < this->end(); ++iter)
        {
            T buff = *iter;
            *iter = tmp;
            tmp = buff;
        }
        this->push_back(tmp);
    }

    void erase(iterator it)
    {
        for(iterator iter = it + 1; iter < this->end(); ++iter)
        {
            *(iter - 1) = *iter;
        }
        this->pop_back();
    }

    void printStructure()
    {
        for(auto& elem : arr)
        {
            for(int i = 0; i < innerArraySize; ++i)
                std::cout << elem[i] << " ";
            std::cout << std::endl;
        }
    }
};
