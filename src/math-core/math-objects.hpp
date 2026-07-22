#pragma once

#include <complex>
#include <cstddef>
#include <vector>

// Versatile simple Vector. GenericVector(dim, data)
struct GenericVector {
    size_t v_dim{0};
    std::vector<std::complex<double>> v_data;

    // Constructors
    // GenericVector() = default; // Trivial default constructor Inlined
    explicit GenericVector(size_t dim);
    GenericVector(size_t dim, const std::vector<std::complex<double>>& input_data);

    // Accessors inlined directly in header for zero function-call overhead
    inline std::complex<double>& operator()(size_t i) { return v_data[i]; }
    inline const std::complex<double>& operator()(size_t i) const { return v_data[i]; }

    // Zero-copy raw pointer exposure for the UI buffer
    inline const std::complex<double>* raw_buffer() const { return v_data.data(); }
};

// Versatile simple Matrix. GenericMatrix(rows, cols, data)
struct GenericMatrix {
    size_t m_rows{0};
    size_t m_cols{0};
    std::vector<std::complex<double>> m_data;

    // Constructors
    // GenericMatrix() = default; // Trivial default constructor (Inlined)
    GenericMatrix(size_t rows, size_t cols);
    GenericMatrix(size_t rows, size_t cols, const std::vector<std::complex<double>>& input_data);

    // Accessors inlined directly in header for zero function-call overhead
    inline std::complex<double>& operator()(size_t i, size_t j) { return m_data[i * m_cols + j]; }
    inline const std::complex<double>& operator()(size_t i, size_t j) const { return m_data[i * m_cols + j]; }

    // Zero-copy raw pointer exposure for the UI buffer
    inline const std::complex<double>* raw_buffer() const { return m_data.data(); }
};