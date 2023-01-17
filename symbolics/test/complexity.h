#pragma once

#include <memory.h>

namespace complexity
{

    template < typename Key, typename Value >
    class MHashTable
    {
    public:
        MHashTable() {};
        ~MHashTable() {};

        inline Value find(size_t hash, Key key) const
        {
            size_t bhash = boundHash(hash);
            if (bhash < m_hashmemory.size())
            {
                std::map< Key, Value > map = m_hashmemory[bhash]; 
                return map[key];
            }
            return Value();
        };

        void insert(Key key, Value value, size_t hash) 
        {
            size_t bhash = boundHash(hash);
            if (m_hashmemory.size() <= bhash)
            {
                for (size_t i=m_hashmemory.size();i<=bhash;++i)
                {
                    std::map< Key, Value > emptyvaluevec;
                    m_hashmemory.push_back(emptyvaluevec);
                }
            }
            (m_hashmemory[bhash])[key] = value;
        }


    protected:
        std::vector< std::map< Key, Value > > m_hashmemory;

        inline size_t boundHash(size_t &hash) const { return hash & 0xfffff;};
        
    };


    template < typename T >
    struct vector
    {
    public:
        vector(size_t Size=10): m_size(0),m_memsize(Size), m_values(NULL) { alloc(); };
        ~vector() { free(); }; 

        inline T &operator[](size_t index) 
        {
            if (index < m_size)
                return m_values[index];
            // ohne diese Exception ist es wesentlich schneller, so schnell wie wenn man einen Array durchlaeuft!
            throw std::exception();
        };

        void operator() (T &value)
        {
            // need more memory
            if (m_memsize <= m_size) alloc();
            // insert
            m_values[m_size] = value;
            // increase
            m_size++;
        }
    protected:
        T *m_values;
        size_t m_size;
        size_t m_memsize;

        void alloc()
        {
            if (m_values == NULL)
            {
                m_values = new T[m_memsize];
 	    	void * mvoid = (void*)m_values;
	    	size_t needsize = sizeof(T)*m_memsize; 
            	memset(mvoid,0,needsize);
                return;
            }
            // increase memsize
            m_memsize = static_cast<size_t>(m_memsize*1.7);
            // copy
            T *newvalues = new T[m_memsize];
	    void * mvoid = (void*)newvalues;
	    size_t needsize = sizeof(T)*m_memsize; 
            memset(mvoid,0,needsize);
            for (size_t i=0;i<m_size;i++)
                newvalues[i] = m_values[i];
            // free old
            delete[] m_values;
            m_values = newvalues;
        };
        void free()
        {
            if (m_values != NULL)
            {
                delete[] m_values;
                m_values = NULL;
                m_size = 0;
                m_memsize = 0;
            }
        };
    };
};
