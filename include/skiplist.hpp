#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <cassert>

template <typename Comparable>
class SkipList {
private:
    struct Node {
        Comparable key;
        std::vector<std::shared_ptr<Node>> forward;

        Node(int k, int level) : key(k), forward(level, nullptr) {}
    };

    int maxLevel;
    float probability;
    int level;
    std::shared_ptr<Node> head;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> distribution;

    int randomLevel() {
        int lvl = 1;
        while (distribution(gen) < probability && lvl < maxLevel) {
            ++lvl;
        }
        return lvl;
    }

public:
    SkipList(int maxLvl = 16, float prob = 0.5)
        : maxLevel(maxLvl), probability(prob), level(1), 
          head(std::make_shared<Node>(-1, maxLvl)),
          gen(rd()), distribution(0.0, 1.0) {}

    void insert(Comparable key) {
        std::vector<std::shared_ptr<Node>> update(maxLevel, nullptr);
        auto current = head;

        for (int i = level - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; ++i) {
                update[i] = head;
            }
            level = newLevel;
        }

        auto newNode = std::make_shared<Node>(key, newLevel);
        for (int i = 0; i < newLevel; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    bool search(Comparable key) const {
        auto current = head;
        for (int i = level - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        return current && current->key == key;
    }

    void erase(int key) {
        std::vector<std::shared_ptr<Node>> update(maxLevel, nullptr);
        auto current = head;

        for (int i = level - 1; i >= 0; --i) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->key == key) {
            for (int i = 0; i < level; ++i) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            while (level > 1 && !head->forward[level - 1]) {
                --level;
            }
        }
    }

    void print() const {
        for (int i = 0; i < level; ++i) {
            auto node = head->forward[i];
            std::cout << "Level " << i + 1 << ": ";
            while (node) {
                std::cout << node->key << " ";
                node = node->forward[i];
            }
            std::cout << "\n";
        }
    }
};

