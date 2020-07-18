#include <iostream>
#include <vector>
using namespace std;

template<class K>
struct KeyOfValue {
	const K& operator()(const K& key) {
		return key;
	}
};
//开散列: 指针 + 单链表
template<class V>
struct hashNode {
	V _value;
	hashNode* _next;
	hashNode(const V& val = V())
		:_value(val)
		, _next(nullptr)
	{}
};

template<class K,class V,class KeyOfValue,class HF>
class hashBucket;

template<class K,class V,class KeyOfValue,class HF>
struct hashIterator {
	typedef hashNode<V> Node;
	typedef hashIterator<K, V, KeyOfValue, HF> Self;
	typedef hashBucket<K, V, KeyOfValue, HF> HT;

	Node* _node;
	HT* _ht;

	hashIterator(Node* node,HT* ht)
		:_node(node)
		,_ht(ht)
	{}

	V& operator*() {
		return _node->_value;
	}

	V* operator->() {
		return &_node->_value;
	}

	bool operator!=(const Self& it) {
		return _node != it._node;
	}

	Self& operator++() {
		if (_node->_next)
			_node = _node->_next;
		else {
			//找到下一个非空链表的头结点
			// 1. 定位当前节点在哈希表中的位置
			//kov: 获取value的key
			KeyOfValue kov;
			//hf: 获取key转换之后的整数
			HF hf;
			size_t idx = hf(kov(_node->_value)) % _ht->_table.size();

			// 2. 从表中的下一个位置开始查找第一个非空链表的头结点
			++idx;
			Node* cur = _ht->_table[idx];

			for (; idx < _ht->_table.size(); ++idx)
			{
				if (_ht->_table[idx])
				{
					_node = _ht->_table[idx];
					break;
				}
			}
			// 3. 判断是否找到一个非空的头结点
			if (idx == _ht->_table.size())
				_node = nullptr;
		}
		return *this;
	}


};














template<class K,class V,class KeyOfValue,class HF>
class hashBucket {
public:
	//迭代器声明为友元类
	template <class K, class V, class KeyOfValue, class HF>
	friend struct HashIterator;

	typedef hashNode<V> Node;
	typedef hashIterator<K, V, KeyOfValue, HF> iterator;

	iterator begin() {
		for (int i = 0; i < _table.size(); i++) {
			Node* cur = _table[i];
			if (cur) {
				return iterator(cur, this);
			}
		}
		return iterator(nullptr, this);
	}

	iterator end() {
		return iterator(nullptr, this);
	}

	bool insert(const V& val) {
		//检查容量
		checkCapacity();

		//
		KeyOfValue kov;
		HF hf;
		int idx =hf(kov(val))% _table.size();

		Node* cur = _table[idx];
		while (cur) {
			if (cur->_value == val) {
				return false;
				cur = cur->_next;
			}
		}
		//创造节点,实现头插
		cur = new Node(val);
		cur->_next = _table[idx];
		_table[idx] = cur;
		_size++;
		return true;
	}
	//检查容量
	void checkCapacity() {
		if (_size == _table.size()) {
			//增加容量
			size_t newSize =_size == 0 ? 5 : 2 * _size;
			vector<Node*> newHt;
			newHt.resize(newSize);
			//搬移元素
			KeyOfValue kov;
			HF hf;
			for (int i = 0; i < _table.size(); i++) {
				Node* cur = _table[i];
				while (cur) {
					Node* next = cur->_next;
					//获取插入的位置

					int idx = hf(kov(cur->_value)) % newHt.size();
					//头插
					cur->_next = newHt[idx];
					newHt[idx] = cur;
					//更新cur
					cur = next;
				}
				_table[i] = nullptr;
			}
			//浅拷贝
			_table.swap(newHt);
		}
	}
	Node* find(const K& key) {
		if (_size == 0)
			return nullptr;
		HF hf;
		int idx = hf(key) % _table.size();
		KeyOfValue kov;
		Node* cur = _table[idx];
		while (cur) {
			if (kov(cur->_value) == key)
				return cur;
			cur = cur->_next;
		}
		return nullptr;
	}

	//删除
	bool erase(const K& key) {
		HF hf;
		int idx = hf(key) % _table.size();
		Node* cur = _table[idx];
		Node* prev = nullptr;

		KeyOfValue kov;
		while (cur) {
			if (kov(cur->_value) == key) {
				//删除的是头结点
				if (prev == nullptr) {
					_table[idx] = cur->_next;
				}
				else {
					prev->_next = cur->_next;
				}
				delete cur;
				_size--;
				return true;
			}
			else {
				prev = cur;
				cur = cur->_next;
			}
		}
		return false;
	}
private:
	vector<Node*> _table;
	size_t _size = 0;
};

//字符转换为整数
struct strToInt
{
	size_t operator()(const string& str)
	{
		size_t hash = 0;
		for (const auto& ch : str)
		{
			hash = hash * 131 + ch;
		}
		return hash;
	}
};

template <class K>
struct hashFun
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

//unordered_map实现:
template<class K,class V,class HF = hashFun<K>>
class unorderedmap {
	struct MapKeyOfValue {
		const K& operator()(const pair<K, V>& value) {
			return value.first;
		}
	};
public:
	bool insert(const pair<K, V>& val) {
		return ht.insert(val);
	}
private:
	hashBucket<K, V, MapKeyOfValue> ht;
};



//unordered_set实现
template<class K, class HF = hashFun<K>>
class unorderedset {
	struct SetKeyOfValue {
		const K& operator()(const K& value) {
			return value;
		}
	};
public:
	typedef typename HashTable<K, K, SetKeyOfValue, HF>::iterator iterator;

	iterator begin()
	{
		return _ht.begin();
	}

	iterator end()
	{
		return _ht.end();
	}
	bool insert(const K& val) {
		return ht.insert(val);
	}
private:
	hashBucket<K, SetKeyOfValue> ht;
};

 