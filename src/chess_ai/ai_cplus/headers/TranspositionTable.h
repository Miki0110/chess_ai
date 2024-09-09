#include <unordered_map>
#include <memory>
#include <mutex>

class TranspositionTable {
public:
    static std::unordered_map<uint64_t, int>& getInstance() {
        static std::unordered_map<uint64_t, int> instance;  // The shared transposition table
        return instance;
    }

    // You can add mutex handling here if needed for thread-safe access
    static std::mutex& getMutex() {
        static std::mutex mutex;
        return mutex;
    }

    // Disable copying and assignment
    TranspositionTable(const TranspositionTable&) = delete;
    TranspositionTable& operator=(const TranspositionTable&) = delete;

private:
    TranspositionTable() {}  // Private constructor to prevent instantiation
};
