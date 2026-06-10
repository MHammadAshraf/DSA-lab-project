#pragma once
// ---------------------------------------------------------------------------
// DataStructures.h - ALL custom data structures, hand written from scratch.
// No STL containers (vector/map/list/priority_queue) are used here; these are
// our own implementations, as required by the lab.
//
// Contents:
//   DynamicArray<T>  - growable array (our std::vector replacement)
//   LinkedList<T>    - singly linked list with tail pointer
//   Stack<T>         - LIFO stack (used for undo)
//   MinHeap          - binary min-heap priority queue (scheduling + Dijkstra)
//   HashIntMap<V>    - hash table with separate chaining, int keys
//   Graph            - adjacency-list city map with Dijkstra shortest path
// ---------------------------------------------------------------------------
#include <string>

// =============================== DynamicArray ==============================
// Amortised O(1) push_back by doubling capacity. O(1) indexed access.
template <class T>
class DynamicArray {
    T*  data;
    int sz;
    int cap;
    void copyFrom(const DynamicArray& o) {
        sz = o.sz; cap = o.cap;
        data = cap ? new T[cap] : nullptr;
        for (int i = 0; i < sz; i++) data[i] = o.data[i];
    }
public:
    DynamicArray() : data(nullptr), sz(0), cap(0) {}
    DynamicArray(const DynamicArray& o) { copyFrom(o); }
    DynamicArray& operator=(const DynamicArray& o) {
        if (this != &o) { delete[] data; copyFrom(o); }
        return *this;
    }
    ~DynamicArray() { delete[] data; }

    void reserve(int n) {
        if (n <= cap) return;
        int nc = cap ? cap * 2 : 4;
        if (nc < n) nc = n;
        T* nd = new T[nc];
        for (int i = 0; i < sz; i++) nd[i] = data[i];
        delete[] data;
        data = nd; cap = nc;
    }
    void push_back(const T& v) { reserve(sz + 1); data[sz++] = v; }
    void pop_back() { if (sz) sz--; }
    int  size() const { return sz; }
    bool empty() const { return sz == 0; }
    void clear() { sz = 0; }
    T&       operator[](int i)       { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    T&       back()       { return data[sz - 1]; }
    const T& back() const { return data[sz - 1]; }
};

// ================================ LinkedList ===============================
// Singly linked list, O(1) push_back via tail pointer, O(1) pop_front.
template <class T>
class LinkedList {
    struct Node { T val; Node* next; Node(const T& v) : val(v), next(nullptr) {} };
    Node* head;
    Node* tail;
    int   sz;
public:
    LinkedList() : head(nullptr), tail(nullptr), sz(0) {}
    LinkedList(const LinkedList& o) : head(nullptr), tail(nullptr), sz(0) {
        for (Node* c = o.head; c; c = c->next) push_back(c->val);
    }
    LinkedList& operator=(const LinkedList& o) {
        if (this != &o) { clear(); for (Node* c = o.head; c; c = c->next) push_back(c->val); }
        return *this;
    }
    ~LinkedList() { clear(); }

    void clear() {
        Node* c = head;
        while (c) { Node* n = c->next; delete c; c = n; }
        head = tail = nullptr; sz = 0;
    }
    void push_back(const T& v) {
        Node* n = new Node(v);
        if (!head) head = tail = n; else { tail->next = n; tail = n; }
        sz++;
    }
    bool pop_front(T& out) {
        if (!head) return false;
        Node* n = head; out = n->val;
        head = head->next;
        if (!head) tail = nullptr;
        delete n; sz--;
        return true;
    }
    int  size() const { return sz; }
    bool empty() const { return sz == 0; }

    // Visit every element in order (functor receives const T&).
    template <class F> void forEach(F f) const {
        for (Node* c = head; c; c = c->next) f(c->val);
    }
    // Remove first element matching predicate; returns it via 'removed'.
    template <class Pred> bool removeIf(Pred p, T& removed) {
        Node* prev = nullptr; Node* c = head;
        while (c) {
            if (p(c->val)) {
                removed = c->val;
                if (prev) prev->next = c->next; else head = c->next;
                if (c == tail) tail = prev;
                delete c; sz--;
                return true;
            }
            prev = c; c = c->next;
        }
        return false;
    }
};

// ================================== Stack ==================================
template <class T>
class Stack {
    DynamicArray<T> a;
public:
    void push(const T& v) { a.push_back(v); }
    bool pop(T& out) {
        if (a.empty()) return false;
        out = a.back(); a.pop_back();
        return true;
    }
    bool top(T& out) const {
        if (a.empty()) return false;
        out = a.back();
        return true;
    }
    bool empty() const { return a.empty(); }
    int  size()  const { return a.size(); }
};

// ================================= MinHeap =================================
// Binary min-heap. Stores (score, id) pairs; smallest score is popped first.
// Used for order scheduling (score = urgency) and inside Dijkstra.
struct HeapNode { long long score; int id; };

class MinHeap {
    DynamicArray<HeapNode> h;
    void swapN(int a, int b) { HeapNode t = h[a]; h[a] = h[b]; h[b] = t; }
    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (h[p].score <= h[i].score) break;
            swapN(i, p); i = p;
        }
    }
    void siftDown(int i) {
        int n = h.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, s = i;
            if (l < n && h[l].score < h[s].score) s = l;
            if (r < n && h[r].score < h[s].score) s = r;
            if (s == i) break;
            swapN(i, s); i = s;
        }
    }
public:
    void push(long long score, int id) {
        HeapNode n; n.score = score; n.id = id;
        h.push_back(n); siftUp(h.size() - 1);
    }
    bool peek(HeapNode& out) const {
        if (h.empty()) return false;
        out = h[0]; return true;
    }
    bool pop(HeapNode& out) {
        if (h.empty()) return false;
        out = h[0];
        h[0] = h.back(); h.pop_back();
        if (!h.empty()) siftDown(0);
        return true;
    }
    bool empty() const { return h.empty(); }
    int  size()  const { return h.size(); }
    void clear() { h.clear(); }
};

// =============================== HashIntMap ================================
// Hash table with separate chaining. Integer keys, value type V.
// O(1) average insert/lookup/delete. Used by the Search & Retrieval engine
// to map id -> index for orders/customers/restaurants/riders.
template <class V>
class HashIntMap {
    struct Entry { int key; V val; Entry* next; };
    Entry** buckets;
    int     nb;
    int     count;
    int bucketOf(int k) const {
        unsigned long long h = (unsigned long long)(unsigned)k * 2654435761ULL;
        return (int)(h % (unsigned long long)nb);
    }
public:
    HashIntMap(int b = 1024) : nb(b), count(0) {
        buckets = new Entry*[nb];
        for (int i = 0; i < nb; i++) buckets[i] = nullptr;
    }
    ~HashIntMap() { clearAll(); delete[] buckets; }
    HashIntMap(const HashIntMap&) = delete;            // non-copyable (keeps it simple)
    HashIntMap& operator=(const HashIntMap&) = delete;

    void clearAll() {
        for (int i = 0; i < nb; i++) {
            Entry* c = buckets[i];
            while (c) { Entry* n = c->next; delete c; c = n; }
            buckets[i] = nullptr;
        }
        count = 0;
    }
    void put(int k, const V& v) {
        int i = bucketOf(k);
        for (Entry* c = buckets[i]; c; c = c->next)
            if (c->key == k) { c->val = v; return; }
        Entry* e = new Entry{ k, v, buckets[i] };
        buckets[i] = e; count++;
    }
    bool get(int k, V& out) const {
        int i = bucketOf(k);
        for (Entry* c = buckets[i]; c; c = c->next)
            if (c->key == k) { out = c->val; return true; }
        return false;
    }
    bool contains(int k) const {
        int i = bucketOf(k);
        for (Entry* c = buckets[i]; c; c = c->next)
            if (c->key == k) return true;
        return false;
    }
    bool remove(int k) {
        int i = bucketOf(k);
        Entry* prev = nullptr; Entry* c = buckets[i];
        while (c) {
            if (c->key == k) {
                if (prev) prev->next = c->next; else buckets[i] = c->next;
                delete c; count--; return true;
            }
            prev = c; c = c->next;
        }
        return false;
    }
    int size() const { return count; }
};

// ================================== Graph ==================================
// Undirected weighted graph stored as an adjacency list. Roads can be blocked
// at runtime (rerouting). Shortest paths via Dijkstra using our own MinHeap.
const long long INF_DIST = (1LL << 60);

struct Edge { int to; int w; bool blocked; };

class Graph {
    int n;
    DynamicArray<DynamicArray<Edge> > adj;
    DynamicArray<std::string>         names;
public:
    Graph() : n(0) {}

    void init(int nodes) {
        n = nodes;
        adj.clear(); names.clear();
        for (int i = 0; i < n; i++) { adj.push_back(DynamicArray<Edge>()); names.push_back(std::string()); }
    }
    int  nodes() const { return n; }
    void setName(int i, const std::string& s) { if (i >= 0 && i < n) names[i] = s; }
    std::string name(int i) const { return (i >= 0 && i < n) ? names[i] : std::string("?"); }

    void addEdge(int u, int v, int w) {
        if (u < 0 || v < 0 || u >= n || v >= n) return;
        Edge a; a.to = v; a.w = w; a.blocked = false; adj[u].push_back(a);
        Edge b; b.to = u; b.w = w; b.blocked = false; adj[v].push_back(b);
    }
    // Toggle/-set the blocked flag on the edge in both directions.
    bool setBlocked(int u, int v, bool blocked) {
        bool found = false;
        if (u >= 0 && u < n)
            for (int i = 0; i < adj[u].size(); i++)
                if (adj[u][i].to == v) { adj[u][i].blocked = blocked; found = true; }
        if (v >= 0 && v < n)
            for (int i = 0; i < adj[v].size(); i++)
                if (adj[v][i].to == u) { adj[v][i].blocked = blocked; found = true; }
        return found;
    }

    // Dijkstra from src. Fills dist[] and prev[] (prev[src] = -1).
    void dijkstra(int src, DynamicArray<long long>& dist, DynamicArray<int>& prev) const {
        dist.clear(); prev.clear();
        for (int i = 0; i < n; i++) { dist.push_back(INF_DIST); prev.push_back(-1); }
        if (src < 0 || src >= n) return;
        dist[src] = 0;
        MinHeap pq; pq.push(0, src);
        DynamicArray<bool> done;
        for (int i = 0; i < n; i++) done.push_back(false);
        HeapNode cur;
        while (pq.pop(cur)) {
            int u = cur.id;
            if (done[u]) continue;
            done[u] = true;
            for (int j = 0; j < adj[u].size(); j++) {
                const Edge& e = adj[u][j];
                if (e.blocked) continue;
                if (dist[u] != INF_DIST && dist[u] + e.w < dist[e.to]) {
                    dist[e.to] = dist[u] + e.w;
                    prev[e.to] = u;
                    pq.push(dist[e.to], e.to);
                }
            }
        }
    }

    // Convenience: shortest distance only (INF_DIST if unreachable).
    long long shortestDist(int src, int dst) const {
        DynamicArray<long long> dist; DynamicArray<int> prev;
        dijkstra(src, dist, prev);
        if (dst < 0 || dst >= n) return INF_DIST;
        return dist[dst];
    }
    // Shortest path node sequence src..dst (empty if unreachable).
    DynamicArray<int> shortestPath(int src, int dst) const {
        DynamicArray<long long> dist; DynamicArray<int> prev;
        dijkstra(src, dist, prev);
        DynamicArray<int> path;
        if (dst < 0 || dst >= n || dist[dst] == INF_DIST) return path;
        DynamicArray<int> rev;
        for (int at = dst; at != -1; at = prev[at]) rev.push_back(at);
        for (int i = rev.size() - 1; i >= 0; i--) path.push_back(rev[i]);
        return path;
    }
};
