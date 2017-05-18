#include <vector>
#include <algorithm>
#include <cmemory>

#define N 5

template<typename KeyType>
class Node {
public:
    Node() : _size(0) { };
    virtual ~Node();

    Node(const Node& node) {
        memcpy(_keys, node->_keys, sizeof(node->_keys));
    }

    virtual void* Find(KeyType key);
    virtual bool Update(KeyType key, void *value);

    // 若已满，则返回分裂后新增加节点的指针
    virtual Node<KeyType>* Insert(KeyType key, void *value);
    virtual bool Delete(KeyType key);

    bool isFull() { return _size == N - 1; }
    bool isHalfFull() { return _size > (N - 1) / 2; }

    int getSearchKey() { return _keys[0]; }

protected:
    KeyType _keys[N - 1];
    size_t _size;

    // 找到满足key >= _keys[index]的最小index
    int _BinarySearch(KeyType key) {
        int i = 0,
            j = _keys.size() - 1;

        if (key < _keys[0])
            return -1;

        while (i < j) {
            int mid = i + ((j - i) >> 1);
            if (_keys[mid] == key)
                return mid;

            if (_keys[mid] < key)
                i = mid + 1;
            else
                j = mid;
        }
        return i;
    }
}

template<typename KeyType>
class InnerNode : public Node<KeyType> {
public :
    InnerNode();
    ~InnerNode() {
        for (auto& p : _pointer)
            delete p;
    }

    InnerNode(const InnerNode &node) {
        memcpy(_pointer, node->pointer, sizeof(node->pointer));
    }

    void* Find(KeyType key) override {
        // 对于_pointer来说，会比_keys多一个
        // 其中需要_pointer[0]存的是小于所有的搜索键的指针，故index均要+1
        int index = _BinarySearch(key) + 1;
        return _pointer[index]->Find(key);
    }

    bool Update(KeyType key, void *value) override {
        int index = _BinarySearch(key) + 1;
        return _pointer[index]->Update(key, value);
    }

    Node<KeyType>* Insert(KeyType key, void *value) override {
        int index = _BinarySearch(key) + 1;

        Node<KeyType> *ret_node = _pointer[index]->Insert(key, value);
        if (ret_node == nullptr)
            return nullptr;
        else { // 子节点已满
            if (!isFull()) {
                _Insert(ret_node->getSearchKey() , ret_node);
                return nullptr;
            } else { // 该节点也已满，继续分裂节点
                // 结点已满，需要分裂
                Node<KeyType> *new_node = new InnerNode<KeyType>(*this);

                // 保留该节点的前半段，后半段数据给新节点
                if (key < _keys[ceil(_size / 2)]) {
                    // 插入key应在前半段
                    _size = ceil(_size / 2) - 1;
                    _Insert(ret_node->getSearchKey(), ret_node);
                    new_node->Remove(_size);

                }
                else {
                    // 插入key应在后半段
                    _size = ceil(_size / 2);
                    new_node->Remove(_size);
                    new_node->Insert(ret_node->getSearchKey(), ret_node);
                }

                return new_node;
            }
        }
    }

    void Remove(size_t length) {
        _size = _size - length;

        for (size_t i = 0; i < _size; i++) {
            _keys[i] = _keys[i + length];
            _pointer[i] = _pointer[i + length];
        }
    }

private:
    Node<KeyType> *_pointer[N];

    void _Insert(KeyType key, void *ptr) {
        int i = _size - 1;
        while (key > _keys[i] && i >= 0) {
            _keys[i + 1] = _keys[i];
            _pointer[i + 2] = _pointer[i + 1];
            i--;
        }

        _keys[i + 1] = key;
        _pointer[i + 2] = ptr;
        _size++;
    }
}

template<typename KeyType>
class LeafNode : public Node<KeyType> {
public:
    LeafNode() : _next(nullptr) { }
    ~LeafNode();

    void* Find(KeyType key) override {
        int index = _BinarySearch(key);
        if (_keys[index] == key)
            return _pointer[index];
        else
            return nullptr;
    }

    bool Update(KeyType key, void *value) override {
        int index = _BinarySearch(key);
        if (_keys[index] == key) {
            _pointer[index + 1] = value;
            return true;
        } else
            return false;
    }

    Node<KeyType>* Insert(KeyType key, void *value) override {
        if (!isFull()) {
            _Insert(key, value);
            return nullptr;
        }
        else {
            // 结点已满，需要分裂
            LeafNode<KeyType> *new_node = new InnerNode<KeyType>(*this);
            new_node->_next = node->_next;
            node->next = new_node->_data;

            // 保留该节点的前半段，后半段数据给新节点
            if (key < _keys[ceil(_size / 2)]) {
                // 插入key应在前半段
                _size = ceil(_size / 2) - 1;
                _Insert(key, value);
                new_node->Remove(_size);

            }
            else {
                // 插入key应在后半段
                _size = ceil(_size / 2);
                new_node->Remove(_size);
                new_node->Insert(key, value);
            }

            return new_node;
        }

    }

    void Remove(size_t length) {
        _size = _size - length;

        for (size_t i = 0; i < _size; i++) {
            _keys[i] = _keys[i + length];
            _data[i] = _data[i + length];
        }
    }

private:
    void *_data[N - 1];
    LeafNode<KeyType> *_next;

    void _Insert(KeyType key, void *value) {
        int i = _size - 1;
        while (key > _keys[i] && i >= 0) {
            _keys[i + 1] = _keys[i];
            _data[i + 1] = _data[i];
            i--;
        }

        _keys[i + 1] = key;
        _data[i + 1] = value;
        _size++;
    }

}


template<typename KeyType, typename DataType>
class BPTree {
public:
    BPTree(int block_size = 4096)
        : block_size(block_size),
    {

    }
    ~BPTree() {
        delete _root;
    }

    bool Insert(KeyType key, DataType *value) {
        if (_root == nullptr) {
            _root = new Node<KeyType>();
        }
        _root->Insert(key, static_cast<void *>(value));
    }
    bool Delete(KeyType key);
    DataType* Find(KeyType key) {
        if (_root == nullptr)
            return _root;
        else
            return static_cast<DataType *>(_root->Find(key));
    }
    bool Update(KeyType key, DataType *value) {
        if (_root == nullptr)
            return false;
        else
            return _root->Update(key, static_cast<void *>(value));
    }
    void Clear();

private:
    Node<KeyType> *_root;
    int block_size;
}