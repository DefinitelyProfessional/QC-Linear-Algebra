#pragma once
#include <vector>
#include <complex>
#include <string>
#include <stdexcept>
#include <utility>

class Matrix {
protected: // Protected so the Vector subclass can access them
    std::vector<std::complex<double>> m_data;
    size_t m_rows;
    size_t m_cols;
    
    std::string m_id;
    
    // cached metadata
    bool is_unitary;
    bool is_hermitian;
    bool is_normalized;

public:
    // Dimension-First Constructor (Allocates empty/zero matrix)
    Matrix(size_t rows, size_t cols, std::string id = "unnamed_matrix")
        : m_rows(rows), m_cols(cols), m_id(std::move(id)), 
          m_data(rows * cols, {0.0, 0.0}), // Zero-initialize contiguous memory
          is_unitary(false), is_hermitian(false), is_normalized(false) {}

    // Population Constructor (Receives dimensions and a flat input array)
    Matrix(size_t rows, size_t cols, const std::vector<std::complex<double>>& input_data, std::string id = "unnamed_matrix")
        : m_rows(rows), m_cols(cols), m_id(std::move(id)),
          is_unitary(false), is_hermitian(false), is_normalized(false) {
        
        // Strict boundary checking before memory assignment
        if (input_data.size() != rows * cols) {
            throw std::invalid_argument("Input array dimensions do not match the provided rows and columns.");
        }
        
        // C++ std::vector assignment safely copies the continuous block of memory
        m_data = input_data; 
    }

    // --- Accessors & Mutators ---
    size_t rows() const { return m_rows; }
    size_t cols() const { return m_cols; }
    const std::string& get_id() const { return m_id; }
    void set_id(const std::string& new_id) { m_id = new_id; }

    // Overloading the () operator for mathematical row-major indexing access
    inline std::complex<double>& operator()(size_t i, size_t j) {
        return m_data[i * m_cols + j];
    }
    
    inline const std::complex<double>& operator()(size_t i, size_t j) const {
        return m_data[i * m_cols + j];
    }

    // Zero-copy raw pointer exposure for the UI buffer
    const std::complex<double>* raw_buffer() const { return m_data.data(); }
};

class Vector : public Matrix {
public:
    // Dimension-First Constructor
    Vector(size_t dim, std::string id = "unnamed_vector")
        : Matrix(dim, 1, std::move(id)) {}

    // Population Constructor
    Vector(size_t dim, const std::vector<std::complex<double>>& input_data, std::string id = "unnamed_vector")
        : Matrix(dim, 1, input_data, std::move(id)) {}

    // Vector-specific dimension accessor
    size_t dim() const { return m_rows; }

    // 1D Indexing accessor specifically for vectors
    inline std::complex<double>& operator()(size_t i) {
        return m_data[i]; // Bypasses the row-major math since cols == 1
    }
    
    inline const std::complex<double>& operator()(size_t i) const {
        return m_data[i];
    }
};