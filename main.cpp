/* 
 * Implementation of algorithm to generate a list of independent increments 
 * required to morth from collection A to B by deletion, insertion, move and 
 * update the element. 
 *
 * 2016 (c) http://www.zavyalov.nl
  */


#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>

struct Item {
    int64_t key;
    int state;
};

using collection_t = std::vector<Item>;

enum class ChangeValue {
    Deletion = 0, Insertion, Move, Update
};

bool operator< (ChangeValue l, ChangeValue r)
{
    return static_cast<int32_t>(l) < static_cast<int32_t>(r);
}

const char* get_name(enum ChangeValue v)
{
    switch (v) {
        case ChangeValue::Deletion:     return "Deletion";
        case ChangeValue::Insertion:    return "Insertion";
        case ChangeValue::Update:       return "Update";
        case ChangeValue::Move:         return "Move";
    }
}

struct Change {
    enum ChangeValue which;
    uint64_t from;
    uint64_t to;
};

using changes_t = std::vector<Change>;

void
produce_increments(const collection_t &before, const collection_t &after, changes_t &changes)
{
    using indexmap = std::vector<uint64_t>;
    indexmap beforeCopy; beforeCopy.reserve(before.size());
    std::vector<int8_t> afterCopy(after.size(), 0);
    
    size_t countDel = 0, firstInBefore = before.size();
    for (size_t i = 0; i < before.size(); ++i) {
        auto itemBefore = before[i];
        auto it = std::find_if(std::begin(after), std::end(after),
                [itemBefore](const Item &item)->bool {
                    return item.key == itemBefore.key;
                });
        // In case not found, will store 'after.size()' in there.
        auto indexAfter = std::distance(std::begin(after), it);
        if (indexAfter == after.size()) {
            countDel++;
            changes.push_back({ChangeValue::Deletion, i, 0});
            beforeCopy.push_back(after.size());
        }
        else {
            if (firstInBefore == before.size()) {
                firstInBefore = i;
            }
            beforeCopy.push_back(i - countDel);
            afterCopy[indexAfter] = 1;
        }
    }
    // Insert
    if (after.size() > before.size() - countDel) {
        size_t countIns = 0;
        size_t currentIndex = firstInBefore; // First In Before After Deletions Executed :~) 
        // Worst Case: Upper Estimate: 2 * after.size() * before.size() iterations 
        for (size_t i = 0; i < after.size(); ++i) {
            auto itemAfter = after[i];
            auto it = std::find_if(std::begin(before), std::end(before),
                    [itemAfter](const Item &item)->bool{
                        return item.key == itemAfter.key;
                    });

            if (it == std::end(before)) {
                size_t k = currentIndex;
                while (k < before.size()) {
                
                    if (beforeCopy[k] == i) {
                        currentIndex = k;
                        break;
                    }
                    if (beforeCopy[k] != after.size() &&
                        currentIndex < k) {
                        beforeCopy[k] = beforeCopy[currentIndex] + 1;
                        currentIndex = k;
                    }
                    k++;
                }
                if (currentIndex < before.size() &&
                        beforeCopy[currentIndex] == i) {
                    // Increase index as prepend for a new item.
                    beforeCopy[currentIndex]++; 
                }
                changes.push_back({ChangeValue::Insertion, i, 0});
            }
        }
        // Update the indexes of beforeCopy's tail
        if (currentIndex < before.size()) {
            size_t k = currentIndex;
            do {
                if (beforeCopy[k] != after.size() &&
                    currentIndex < k) {
                    beforeCopy[k] = beforeCopy[currentIndex] + 1;
                    currentIndex = k;
                }

                k++;
            } while (k < before.size());
        }
    }
    // Moves
    for (size_t i = 0; i < afterCopy.size(); ++i) {
        if (afterCopy[i] == (int8_t)1) {
            // TODO: limit index on beforeCopy?
            for (size_t j = firstInBefore; j < beforeCopy.size(); ++j) {
                if (after[i].key == before[j].key) {
                    if (i != beforeCopy[j]) {
                        changes.push_back({ChangeValue::Move, beforeCopy[j], i});
                        beforeCopy[j] = after.size();
                        // Updating 'beforeCopy' indexes
                        // Need 'head' that is different from zero. 
                        for (size_t k = firstInBefore; k < j; ++k) {
                            if (beforeCopy[k] != after.size()) {
                                beforeCopy[k]++;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    // Updates
    for (size_t i = 0; i < afterCopy.size(); ++i) {
        if (afterCopy[i] == (int8_t)1) {
            if (after[i].state) {
                changes.push_back({ChangeValue::Update, i, 0});
            }
        }
    }

    { // Log the intermediate results
        std::cout << "== beforeCopy ==\n"; 
        for (auto &&i : beforeCopy) {
            std::cout << i << "\t";
        }
        std::cout << "== DONE ==\n";
    }
}

/*
 Making matrix [after X before] might be straight forward implementation 
 when no memory constraints. 
            0   1   2   3   4   5   X
     0  6   x   x   x   x   x   x   i
     1  7   x   x   x   x   x   x   i
     2  5   x   x   x   x   x   o   m/u
        Z   d   d   d   d   d   m/u   
*/
int main ()
{
    // test 101
    {
        collection_t before{ {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0} };
        collection_t after { {5,1}, {6,0}, {7,0} };

        changes_t increments; increments.reserve(before.size() + after.size());
        produce_increments(before, after, increments);
        
        for (auto &&v : increments) {
            std::cout << get_name(v.which) << ": " << v.from;
            if (v.which == ChangeValue::Move) {
                std::cout << " -> " << v.to;
            }
            std::cout << "\n";
        }
    }
    // test 102
    {
        collection_t before{ {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0} };
        collection_t after { {6,0}, {7,0}, {5,1}, {3,1}, {4,0} };

        changes_t increments; increments.reserve(before.size() + after.size());
        produce_increments(before, after, increments);
        
        for (auto &&v : increments) {
            std::cout << get_name(v.which) << ": " << v.from;
            if (v.which == ChangeValue::Move) {
                std::cout << " -> " << v.to;
            }
            std::cout << "\n";
        }
    }

    // test 103
    {
        collection_t before;
        collection_t after;

        std::srand(std::time(0)); // use current time as seed for random generator
        int random_variable = std::rand();
        std::cout << "Random value on [0 " << RAND_MAX << "]: " 
              << random_variable << '\n';

        size_t before_size = 500, after_size = 100;
        before.reserve(before_size);
        after.reserve(after_size);
        int rv = std::rand() % 7;
        for (size_t i = 0; i < before_size; ++i) {
            before.push_back({rv, 0});
            rv += std::rand() % 7 + 1; 
        }
        rv = std::rand() % 5  + 10;
        for (size_t i = 0; i < after_size; ++i) {
            after.push_back({rv, rv % 2});
            rv += std::rand() % 5 + 1; 
        }

        changes_t increments; increments.reserve(before.size() + after.size());
        produce_increments(before, after, increments);
        
        for (auto &&v : increments) {
            std::cout << get_name(v.which) << ": " << v.from;
            if (v.which == ChangeValue::Move) {
                std::cout << " -> " << v.to;
            }
            std::cout << "\n";
        }
    }
    
    return 0;
}

