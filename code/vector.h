#pragma once
#define MAXCAP 5
#include <string>

using namespace std;

template <class T>
class Vector {
public:
	Vector(){// creates an empty vector
		m_nSize = 0;
		m_nCapacity = MAXCAP;//默认的capacity=MAXCAP
		m_pElements = new T[m_nCapacity];
	}                      
	Vector(int size) {// creates a vector for holding 'size' elements
		m_nSize = 0;
		m_nCapacity = size;
		m_pElements = new T[m_nCapacity];
	}              
	Vector(const Vector& r) {// the copy ctor
		m_nSize = r.m_nSize;
		m_nCapacity = r.m_nCapacity;
		m_pElements = new T[m_nCapacity];
		int i;
		for (i = 0; i < m_nSize; i++) {
			m_pElements[i] = r.m_pElements[i];//将内容复制过来
		}
	}
	~Vector() {// destructs the vector 
		delete[] m_pElements;
	}                     
	T& operator[](int index) {// accesses the specified element without bounds checking
		return m_pElements[index];//without bounds checking
	}
	T& at(int index) {// accesses the specified element, throws an exception of type 'std::out_of_range' when index <0 or >=m_nSize
		if (index >= 0 && index < m_nSize) {
			return m_pElements[index];
		}
		else {
			throw out_of_range(to_string(index));//超出范围
		}
	}              
	int size() const { return m_nSize; }		 // return the size of the container
	void push_back(const T& x) {// adds an element to the end
		if (m_nSize == m_nCapacity) {
			inflate();//扩容
		}
		m_pElements[m_nSize++] = x;//先赋值，再++ 
		return;
	}     
	void clear() {// clears the contents
		delete[] m_pElements;
		m_nSize = 0;
		m_pElements = new T[m_nCapacity];//重新分配空间
		return;
	}                  
	bool empty() const { return m_nSize == 0; }            // checks whether the container is empty 
	void deletei(int index) {
		if (index >= 0 && index < m_nSize) {
			int i;
			for (i = index; i < m_nSize - 1; i++) {
				m_pElements[i] = m_pElements[i + 1];
			}
			m_nSize--;
		}//不考虑capacity的缩小
	}
private:
	void inflate() {// expand the storage of the container to a new capacity, e.g. 2*m_nCapacity
		T *oldm;
		oldm = m_pElements;//记录原地址
		m_pElements = new T[2 * m_nCapacity];//分配两倍的空间
		m_nCapacity *= 2;
		int i;
		for (i = 0; i < m_nSize; i++) {
			m_pElements[i] = oldm[i];//拷贝数据
		}
		delete[] oldm;//释放原空间
	}
	T *m_pElements;                // pointer to the dynamically allocated storage
	int m_nSize;                   // the number of elements in the container
	int m_nCapacity;               // the number of elements that can be held in currently allocated storage
};