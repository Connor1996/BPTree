#include <vector>

template<typename KeyType>
class Node {
public:
    Node();
    virtual ~Node();

    virtual void* Find(KeyType key);
protected:
    std::vector<KeyType> _keys;

    // find the lower_bound
    int _BinarySearch(KeyType key) {
        int i = 0,
            j = _keys.size() - 1;

        if (key < _keys[0])
            return 0;

        while (i < j) {
            int mid = i + ((j - i) >> 1);
            if (_keys[mid] == key)
                return mid;

            if (_keys[mid] < key)
                i = mid + 1;
            else
                j = mid;
        }

        return i + 1;
    }
}

template<typename KeyType>
class InnerNode : public Node {
public :
    InnerNode();
    ~InnerNode();

    void* Find(KeyType key) override {
        int index = _BinarySearch(key);
        return _pointer[index]->Find(key);
    }

private:
    std::vector<Node<KeyType> *> _pointer;
}

template<typename KeyType>
class LeafNode : public Node {
public:
    LeafNode();
    ~LeafNode();

    void* Find(KeyType key) override {
        int index = _BinarySearch(key);
        return _data[index];
    }

private:
    std::vector<void *> _data;
}


template<typename KeyType, typename DataType>
class BPTree {
public:
    BPTree(int block_size = 4096)
        : block_size(block_size),
    {

    }
    ~BPTree();

    bool Insert(KeyType key, DataType *value);
    bool Delete(KeyType key);
    DataType* Find(KeyType key) {
        if (_root == nullptr)
            return _root;
        else
            return static_cast<DataType *>(_root->Find(key));
    }
    bool Update(KeyType key, DataType *value);
    void Clear();

private:
    Node<KeyType> *_root;
    int block_size;

}