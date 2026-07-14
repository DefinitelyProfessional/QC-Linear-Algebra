#pragma once
#include <vector>
#include <complex>
#include <string>
#include <stdexcept>
#include <utility>

// ============================================================================
// Versatile Matrix Data Structure for both classical and quantum
// ============================================================================
class Matrix {
protected:
    // Prefixed with m_ to completely avoid naming conflicts with accessor methods
    std::vector<std::complex<double>> m_data;
    size_t m_rows;
    size_t m_cols;
    std::string m_id;
    
    // Cached quantum metadata properties
    bool m_is_unitary;
    bool m_is_hermitian;
    bool m_is_normalized;

public:
    // Dimension-First Constructor (Allocates empty/zero matrix)
    Matrix(size_t rows, size_t cols, std::string id = "unnamed_matrix")
        : m_rows(rows), m_cols(cols), m_id(std::move(id)), 
          m_data(rows * cols, {0.0, 0.0}), 
          m_is_unitary(false), m_is_hermitian(false), m_is_normalized(false) {}

    // Population Constructor (Receives dimensions and a flat input array)
    Matrix(size_t rows, size_t cols, const std::vector<std::complex<double>>& input_data, std::string id = "unnamed_matrix")
        : m_rows(rows), m_cols(cols), m_id(std::move(id)),
          m_is_unitary(false), m_is_hermitian(false), m_is_normalized(false) {
        
        // Strict boundary checking before memory assignment
        if (input_data.size() != rows * cols) {
            throw std::invalid_argument("Input array dimensions do not match the provided rows and columns.");
        }
        
        m_data = input_data; 
    }

    // Virtual destructor guarantees safe cleanup if handled via base class pointers
    virtual ~Matrix() = default;

    // Accessors & Mutators - Now completely conflict-free!
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

// ============================================================================
// Column Vector (Inherits from Matrix)
// ============================================================================
class ClassicVector : public Matrix {
public:
    // Dimension-First Constructor (Forces columns parameter to 1 automatically)
    ClassicVector(size_t dim, std::string id = "unnamed_vector")
        : Matrix(dim, 1, std::move(id)) {}

    // Population Constructor (Forces columns parameter to 1 automatically)
    ClassicVector(size_t dim, const std::vector<std::complex<double>>& input_data, std::string id = "unnamed_vector")
        : Matrix(dim, 1, input_data, std::move(id)) {}

    // Vector-specific dimension accessor (Simply references our underlying rows variable)
    size_t dim() const { return m_rows; }

    // Convenient 1D Indexing accessor specifically for linear vectors
    inline std::complex<double>& operator()(size_t i) {
        return m_data[i]; 
    }
    
    inline const std::complex<double>& operator()(size_t i) const {
        return m_data[i];
    }
};