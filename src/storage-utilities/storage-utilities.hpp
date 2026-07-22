#pragma once

#include "math-core/math-objects.hpp"

#include <unordered_map>
#include <type_traits>
#include <string_view>
#include <filesystem>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <utility>

// Forward declare sqlite3
struct sqlite3;

enum class MathObjType {
    GenericVector,
    GenericMatrix,
};

struct MathObjMap {
    std::string key_str;
    MathObjType type;
    size_t index;
};

// Helper for static_assert in template branches
template<class> inline constexpr bool always_false_v = false;

// Manage Loading & Storing the Sandbox registry
class SandboxSessionManager {
private:
    std::filesystem::path saved_data_dir;
    std::string active_filename;

    // Map string hash key to the obj_data
    template <typename T> struct ObjEntry {
        uint64_t hash_key;
        T obj_data;
    };

    // sandbox_registry for tracking name strings by hash keys
    // connected to MathObj stored in dedicated vectors
    // in memory and tracked with MathObjMap
    std::unordered_map<uint64_t, MathObjMap> sandbox_registry;
    std::vector<ObjEntry<GenericVector>> generic_vector_pool;
    std::vector<ObjEntry<GenericMatrix>> generic_matrix_pool;

    // Compile-time routing : return corresponding data pool
    template <typename T> auto& get_pool() {
        if constexpr (std::is_same_v<T, GenericVector>) return generic_vector_pool;
        else if constexpr (std::is_same_v<T, GenericMatrix>) return generic_matrix_pool;
        else static_assert(always_false_v<T>, "Unsupported math object type.");
    }
    // Compile-time routing : return corresponding math object type
    template <typename T> constexpr MathObjType get_type() {
        if constexpr (std::is_same_v<T, GenericVector>) return MathObjType::GenericVector;
        else if constexpr (std::is_same_v<T, GenericMatrix>) return MathObjType::GenericMatrix;
        else static_assert(always_false_v<T>, "Unsupported math object type.");
    }
    // Compile-time routing : helper to manage delete objects from their pools
    template <typename T> void swap_pop(std::vector<ObjEntry<T>>& pool, size_t selected_index) {
        size_t last_index = pool.size() - 1;
        // Swap iff selected_index != last_index
        if (selected_index != last_index) {
            // Swap selected with last, note pool[index] is ObjEntry which is movable
            pool[selected_index] = std::move(pool[last_index]);
            // Effectively update MathObjMap index with moved ObjEntry's hash_key
            sandbox_registry[pool[selected_index].hash_key].index = selected_index;
        }
        pool.pop_back();
    }

    // Internal key_str hashing for memory efficient mapping 
    inline uint64_t get_hash_key(std::string_view key_str) const {
        return std::hash<std::string_view>{}(key_str);
    }

    // Helper function to execute simple sqlite3
    static void execute_sql(sqlite3* db, const std::string& sql);
    
    // LOAD sandbox data from specified filename
    void load_sandbox();

public:
    // SandboxSessionManager Constructor
    explicit SandboxSessionManager(const std::filesystem::path& data_dir, const std::string& filename);

    // return the active sandbox filename string
    const std::string& get_active_filename() const {return active_filename;}

    // Trivial stuff
    size_t count() const {return sandbox_registry.size();}

    // Add an object to sandbox_registry and handle storage
    template <typename T> bool add(std::string_view key, T obj) {
        uint64_t hash = get_hash_key(key);

        // Reject adding objects with the same key_str
        if (sandbox_registry.find(hash) != sandbox_registry.end()) {return false;}

        // Get corresponding pool and type
        auto& pool = get_pool<T>();
        MathObjType type = get_type<T>();

        // Store at corresponding Obj pool 
        size_t index = pool.size;
        pool.push_back({hash, std::move(obj)});
        // Register to sandbox_registry duh, store original key
        sandbox_registry[hash] = {std::string(key), type, index};

        return true;
    }

    // Safely retrieve a POINTER to the requested type, allowing in-place edits
    // Returns nullptr if the key doesn't exist OR if you ask for the wrong type
    template<typename T> T* get(std::string_view key) {
        uint64_t hash = get_hash_key(key);
        // Get map registry of specified hash
        auto it = sandbox_registry.find(hash);

        // if specified key and its object doesn't exist
        if (it == sandbox_registry.end()) {return nullptr;}
        // if specified type doesn't match the object's type
        if (it->second.type != get_type<T>()) {return nullptr;}
        // return a reference to the object stored at its pool
        return& get_pool<T>()[it->second.index].obj_data;
    }

    // Remove an object from sandbox_registry and handle delete
    bool remove(std::string_view key);

    // Dictionary key rename without copying heavy vector data
    void rename(std::string_view old_key, std::string_view new_key);

    // STORE sandbox data written back to filename
    void save_sandbox() const;

    // Save then delete previous sandbox, switch and load new sandbox
    void switch_sandbox(const std::string& new_filename);
};