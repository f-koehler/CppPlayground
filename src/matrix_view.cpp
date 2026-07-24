#include <cassert>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T> class Matrix {
public:
  Matrix(std::size_t rows, std::size_t cols)
      : m_data(rows * cols), m_rows(rows), m_cols(cols) {}

  T &operator()(std::size_t row, std::size_t col) noexcept {
    return m_data[(row * m_cols) + col];
  }
  const T &operator()(std::size_t row, std::size_t col) const noexcept {
    return m_data[(row * m_cols) + col];
  }

  [[nodiscard]] constexpr std::size_t rows() const noexcept { return m_rows; }
  [[nodiscard]] constexpr std::size_t cols() const noexcept { return m_cols; }
  T *data() noexcept { return m_data.data(); }
  const T *data() const noexcept { return m_data.data(); }

private:
  std::vector<T> m_data;
  std::size_t m_rows;
  std::size_t m_cols;
};

template <class T> class MatrixView {
public:
  using Scalar = std::remove_const_t<T>;

  constexpr MatrixView() = default; // empty matrix view
  constexpr MatrixView(T *data, std::size_t rows, std::size_t cols,
                       std::size_t stride) noexcept
      : m_rows(rows), m_cols(cols), m_stride(stride), m_data(data) {
    assert(stride >= cols);
  }
  constexpr explicit MatrixView(Matrix<Scalar> &matrix) noexcept
      : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), matrix.cols()) {
  }
  constexpr explicit MatrixView(const Matrix<Scalar> &matrix) noexcept
    requires(std::is_const_v<T>)
      : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), matrix.cols()) {
  }

  explicit constexpr operator MatrixView<const Scalar>() const noexcept
    requires(!std::is_const_v<T>)
  {
    return MatrixView<const Scalar>(m_data, m_rows, m_cols, m_stride);
  }
  constexpr T &operator()(std::size_t row, std::size_t col) const noexcept {
    assert(row < m_rows);
    assert(col < m_cols);
    return m_data[index(row, col)];
  }

  [[nodiscard]] constexpr std::size_t rows() const noexcept { return m_rows; }
  [[nodiscard]] constexpr std::size_t cols() const noexcept { return m_cols; }
  [[nodiscard]] constexpr std::size_t stride() const noexcept {
    return m_stride;
  }
  [[nodiscard]] constexpr bool is_contiguous() const noexcept {
    return m_stride == m_cols;
  }
  [[nodiscard]] constexpr bool empty() const noexcept {
    return m_rows == 0 || m_cols == 0;
  }
  constexpr MatrixView subview(std::size_t start_row, std::size_t start_column,
                               std::size_t num_rows,
                               std::size_t num_cols) const {
    if ((start_row + num_rows > m_rows) || (start_column + num_cols > m_cols)) {
      throw std::out_of_range("Subview out-of-bounds");
    }
    return MatrixView(m_data + index(start_row, start_column), num_rows,
                      num_cols, m_stride);
  }
  constexpr void crop(std::size_t num_rows, std::size_t num_cols) {
    if ((num_rows > m_rows) || (num_cols > m_cols)) {
      throw std::length_error("Cannot grow a matrix view");
    }
    m_rows = num_rows;
    m_cols = num_cols;
  }
  constexpr void reshape(std::size_t num_rows, std::size_t num_cols) {
    if (!is_contiguous()) {
      throw std::logic_error("Can only reshape a continuous MatrixView");
    }
    if (num_rows * num_cols != m_rows * m_cols) {
      throw std::length_error(
          "MatrixView reshape must preserve number of elements");
    }
    m_rows = num_rows;
    m_cols = num_cols;
    m_stride = num_cols;
  }

  constexpr T *row_begin(std::size_t row) const noexcept {
    return m_data + index(row, 0);
  }
  constexpr T *row_end(std::size_t row) const noexcept {
    return row_begin(row) + m_cols;
  }

private:
  std::size_t m_rows = 0;
  std::size_t m_cols = 0;
  std::size_t m_stride = 0; // number of columns of underlying matrix (important
  // for subviews changing row)
  T *m_data = nullptr;

  constexpr std::size_t index(std::size_t row, std::size_t col) const noexcept {
    return (row * m_stride) + col;
  }
};

template <typename T> MatrixView(Matrix<T> &) -> MatrixView<T>;
template <typename T> MatrixView(const Matrix<T> &) -> MatrixView<const T>;

template <typename T> class StridedMatrixView {
public:
  using Scalar = std::remove_const_t<T>;

  constexpr StridedMatrixView() = default;
  constexpr StridedMatrixView(T *data, std::size_t rows, std::size_t cols,
                              std::size_t row_stride, std::size_t col_stride)
      : m_data(data), m_rows(rows), m_cols(cols), m_row_stride(row_stride),
        m_col_stride(col_stride) {}
  constexpr explicit StridedMatrixView(Matrix<Scalar> &matrix) noexcept
      : StridedMatrixView(matrix.data(), matrix.rows(), matrix.cols(),
                          matrix.cols(), 1UL) {}
  constexpr explicit StridedMatrixView(const Matrix<Scalar> &matrix) noexcept
    requires(std::is_const_v<T>)
      : StridedMatrixView(matrix.data(), matrix.rows(), matrix.cols(),
                          matrix.cols(), 1UL) {}
  constexpr explicit operator StridedMatrixView<const Scalar>() const noexcept
    requires(!std::is_const_v<T>)
  {
    return StridedMatrixView<const Scalar>(m_data, m_rows, m_cols, m_row_stride,
                                           m_col_stride);
  }

  constexpr T &operator()(std::size_t row, std::size_t col) const noexcept {
    assert(row < m_rows);
    assert(col < m_cols);
    return m_data[index(row, col)];
  }
  [[nodiscard]] constexpr std::size_t rows() const noexcept { return m_rows; }
  [[nodiscard]] constexpr std::size_t cols() const noexcept { return m_cols; }
  [[nodiscard]] constexpr std::size_t row_stride() const noexcept {
    return m_row_stride;
  }
  [[nodiscard]] constexpr std::size_t col_stride() const noexcept {
    return m_col_stride;
  }
  [[nodiscard]] constexpr StridedMatrixView subview(std::size_t start_row,
                                          std::size_t start_col,
                                          std::size_t num_rows,
                                          std::size_t num_cols) const {
    if ((start_row + num_rows > m_rows) || (start_col + num_cols > m_cols)) {
      throw std::out_of_range("Subview out-of-bounds");
    }
    return StridedMatrixView(m_data + index(start_row, start_col), num_rows,
                             num_cols, m_row_stride, m_col_stride);
  }
  [[nodiscard]] constexpr StridedMatrixView transpose() {
    return StridedMatrixView(m_data, m_cols, m_rows, m_col_stride,
                             m_row_stride);
  }
  [[nodiscard]] constexpr StridedMatrixView stride(std::size_t row_step, std::size_t col_step) {
      if(row_step ==0 || col_step ==0) {
          throw std::invalid_argument("Row & col step must be positive");
      }
      auto new_rows = (m_rows + row_step - 1) / row_step; // ceil div
      auto new_cols = (m_cols + col_step - 1) / col_step; // ceil div
      return StridedMatrixView(m_data, new_rows, new_cols, m_row_stride * row_step, m_col_stride * col_step);
  }

private:
  T *m_data;
  std::size_t m_rows = 0;
  std::size_t m_cols = 0;
  std::size_t m_row_stride = 0;
  std::size_t m_col_stride = 0;

  constexpr std::size_t index(std::size_t row, std::size_t col) const noexcept {
    return (row * m_row_stride) + (col * m_col_stride);
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &strm, const Matrix<T> &matrix) {
  for (std::size_t row = 0; row < matrix.rows(); ++row) {
    for (std::size_t col = 0; col < matrix.cols(); ++col) {
      strm << matrix(row, col) << ' ';
    }
    strm << '\n';
  }
  return strm;
}

template <typename T>
std::ostream &operator<<(std::ostream &strm, const MatrixView<T> &matrix) {
  for (std::size_t row = 0; row < matrix.rows(); ++row) {
    for (std::size_t col = 0; col < matrix.cols(); ++col) {
      strm << matrix(row, col) << ' ';
    }
    strm << '\n';
  }
  return strm;
}

int main() {
  Matrix<double> matrix(3, 3);
  const Matrix<double> const_matrix(3, 3);
  double value = 1.0;
  for (std::size_t row = 0; row < matrix.rows(); ++row) {
    for (std::size_t col = 0; col < matrix.cols(); ++col) {
      matrix(row, col) = value;
      value += 1.0;
    }
  }
  std::cout << matrix << '\n';

  auto view = MatrixView(matrix).subview(1, 1, 2, 2);
  std::cout << view << '\n';

  return 0;
}
