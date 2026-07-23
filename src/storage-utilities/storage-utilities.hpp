#pragma once

#include "math-core/math-objects.hpp"

#include <boost/unordered/unordered_flat_map.hpp>
#include <type_traits>
#include <string_view>
#include <filesystem>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <utility>

struct MathObjMap {
    MathObjType type;
    uint32_t obj_index;
    uint32_t key_index;
};

// Helper for static_assert in template branches
template<class> inline constexpr bool always_false_v = false;

// Manage Loading & Storing the Sandbox registry
class SandboxSessionManager {
private:
    std::filesystem::path saved_data_dir;
    std::string active_filename;

    // Encapsulate obj_data and hash_key to sync with registry
    template <typename T> struct ObjEntry {
        uint64_t hash_key;
        T obj_data;
    };

    // sandbox_registry for tracking name strings by hash keys
    // connected to MathObj stored in dedicated vectors
    // in memory and tracked with MathObjMap
    boost::unordered_flat_map<uint64_t, MathObjMap> sandbox_registry;
    std::vector<std::string> key_str_pool; // display for user
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
    template <typename T> inline void swap_pop(std::vector<ObjEntry<T>>& obj_pool, uint32_t selected_obj_idx, uint32_t selected_key_idx) {
        size_t last_obj_idx = obj_pool.size() - 1;
        size_t last_key_idx = key_str_pool.size() - 1;
        // Swap iff selected_index != last_index
        if (selected_obj_idx != last_obj_idx) {
            // Swap selected with last, note pool[index] is ObjEntry which is movable
            obj_pool[selected_obj_idx] = std::move(obj_pool[last_obj_idx]);
            // Swap selected with last
            key_str_pool[selected_key_idx] = std::move(key_str_pool[last_key_idx]);
            // Effectively update MathObjMap index with moved ObjEntry's hash_key
            sandbox_registry[obj_pool[selected_obj_idx].hash_key].obj_index = selected_obj_idx;
        }
        obj_pool.pop_back();
        key_str_pool.pop_back();
    }

    // Internal key_str hashing for memory efficient mapping 
    inline uint64_t get_hash_key(std::string_view key_str) const {
        return std::hash<std::string_view>{}(key_str);
    }
    
    // LOAD sandbox data from specified filename
    void load_sandbox();

public:
    // SandboxSessionManager Constructor
    explicit SandboxSessionManager(const std::filesystem::path& data_dir, const std::string& filename);

    // return the active sandbox filename string
    const std::string& get_active_filename() const {return active_filename;}
    // return the vector string of keys for display to the user
    const std::vector<std::string>& get_key_str() const {return key_str_pool;}
    // return the amount of objects present in the registry
    size_t count() const {return sandbox_registry.size();}


    // Add an object to sandbox_registry and handle storage
    template <typename T> bool add(std::string_view key, T obj, std::string& err_buffer) {
        uint64_t hash = get_hash_key(key);

        // Reject adding objects with the same key_str
        if (sandbox_registry.find(hash) != sandbox_registry.end()) {
            err_buffer = std::string(key) + " already exists.";
            return false;
        }

        // Get corresponding pool and type
        auto& obj_pool = get_pool<T>();
        MathObjType type = get_type<T>();

        // get the index at the obj_pool
        uint32_t obj_index = obj_pool.size();
        // get the index at the key_str_pool
        uint32_t key_index = key_str_pool.size();
        // Store obj at corresponding obj_pool
        obj_pool.push_back({hash, std::move(obj)});
        // Store key at corresponding key_pool
        key_str_pool.push_back(std::string(key));

        // Register to sandbox_registry
        sandbox_registry[hash] = {type, obj_index, key_index};

        return true;
    }


    // Safely retrieve a POINTER to the requested type, allowing in-place edits
    // Returns nullptr if the key doesn't exist OR if you ask for the wrong type
    template<typename T> T* get(std::string_view key, std::string& err_buffer) {
        uint64_t hash = get_hash_key(key);
        // Get map registry of specified hash
        auto it = sandbox_registry.find(hash);

        // if specified key and its object doesn't exist
        if (it == sandbox_registry.end()) {
            err_buffer = std::string(key) + " doesn't exist.";
            return nullptr;
        }
        // if specified type doesn't match the object's type
        if (it->second.type != get_type<T>()) {
            err_buffer = std::string(key) + " object type mismatch.";
            return nullptr;
        }
        // return a reference to the object stored at its pool
        return& get_pool<T>()[it->second.obj_index].obj_data;
    }

    // Remove an object from sandbox_registry and handle delete
    bool remove(std::string_view key, std::string& err_buffer);

    // Dictionary key rename without copying heavy vector data
    bool rename(std::string_view old_key, std::string_view new_key, std::string& err_buffer);

    // STORE sandbox data written back to filename
    void save_sandbox() const;

    // Save then delete previous sandbox, switch and load new sandbox
    void switch_sandbox(const std::string& new_filename);
};