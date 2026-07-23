#include "storage-utilities/storage-utilities.hpp"

#include <functional>
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace fs = std::filesystem;

// Constructor Implementation
SandboxSessionManager::SandboxSessionManager(const fs::path& data_dir, const std::string& filename) :
    saved_data_dir(data_dir), active_filename(filename) {load_sandbox();}

// Private
void SandboxSessionManager::load_sandbox() {
    fs::path s_filepath = saved_data_dir / active_filename;
    if (!fs::exists(s_filepath)) {
        std::cout << "[SANDBOX] Creating new sandbox session targeting: " << s_filepath.filename() << "\n";
        return; // There is nothing to load, exit.
    }
    else {std::cout << "[SANDBOX] Loading existing sandbox session from: " << s_filepath.filename() << "\n";}

    // TODO

    std::cout << "[SANDBOX] Successfully loaded sandbox from: " << s_filepath.filename() << "\n";
}

// Public
bool SandboxSessionManager::remove(std::string_view key, std::string& err_buffer) {
    uint64_t hash = get_hash_key(key);
    // Get map registry of specified hash
    auto it = sandbox_registry.find(hash);
    // Specified key has no match aka doesn't exist
    if (it == sandbox_registry.end()) {
        err_buffer = std::string(key) + " doesn't exist.";
        return false;
    }

    MathObjMap math_obj_map = it->second;
    // Remove the object from its pool
    switch (math_obj_map.type) {
        case MathObjType::GenericVector:
            swap_pop(generic_vector_pool, math_obj_map.obj_index, math_obj_map.key_index);
            break;
        case MathObjType::GenericMatrix:
            swap_pop(generic_matrix_pool, math_obj_map.obj_index, math_obj_map.key_index);
            break;
    }

    // Finally remove the map registry
    sandbox_registry.erase(it);
    return true;
}

// Public
bool SandboxSessionManager::rename(std::string_view old_key, std::string_view new_key, std::string& err_buffer) {
    // Return if there is no change in key
    if (old_key == new_key) {return false;}
    // Check if old_key already exists 
    auto it = sandbox_registry.find(get_hash_key(old_key));
    if (it == sandbox_registry.end()) {
        err_buffer = std::string(old_key) + " doesn't exist.";
        return false;
    }
    // Check if new_key already exists
    uint64_t new_hash = get_hash_key(new_key);
    if (sandbox_registry.find(new_hash) != sandbox_registry.end()) {
        err_buffer = std::string(new_key) + " already exists (or hash collision).";
        return false;
    }
    // update key_str and registry hash
    MathObjMap map_data = it->second;
    key_str_pool[map_data.key_index] = std::string(new_key);
    sandbox_registry.erase(it);
    sandbox_registry.emplace(new_hash, map_data);
    return true;
}

// Public
void SandboxSessionManager::save_sandbox() const {
    fs::path s_filepath = saved_data_dir / active_filename;
    if (!fs::exists(s_filepath)) {
        std::cout << "[SANDBOX] Saving new sandbox session targeting: " << s_filepath.filename() << "\n";
        return; // There is nothing to load, exit.
    }
    else {std::cout << "[SANDBOX] Saving existing sandbox session from: " << s_filepath.filename() << "\n";}

    // TODO

    std::cout << "[SANDBOX] Successfully saved sandbox to: " << s_filepath.filename() << "\n";
}

// Public
void SandboxSessionManager::switch_sandbox(const std::string& new_filename) {
    // Store current data to disk
    save_sandbox();

    // Kill the memory spike by assigning an empty map forces the immediate 
    // destruction of all heavy variants/vectors AND drops the bucket allocation.
    sandbox_registry = boost::unordered_flat_map<uint64_t, MathObjMap>();

    // Re-target the path and read the new database
    active_filename = new_filename;
    load_sandbox();
}