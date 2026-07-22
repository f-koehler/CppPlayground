#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>

template <typename T> class Matrix {
public:
  Matrix(std::size_t rows, std::size_t cols)
      : m_rows(rows), m_cols(cols), m_data(rows * cols) {}

  T &operator()(std::size_t row, std::size_t col) noexcept {
    return m_data[(row * m_cols) + col];
  }
  const T &operator()(std::size_t row, std::size_t col) const noexcept {
    return m_data[(row * m_cols) + col];
  }

  [[nodiscard]] std::size_t rows() const noexcept { return m_rows; }
  [[nodiscard]] std::size_t cols() const noexcept { return m_cols; }
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

  MatrixView() = default; // empty matrix view
  MatrixView(T *data, std::size_t rows, std::size_t cols,
             std::size_t stride) noexcept
      : m_rows(rows), m_cols(cols), m_stride(stride), m_data(data) {
    assert(stride >= cols);
  }
  explicit MatrixView(Matrix<Scalar> &matrix) noexcept
      : MatrixView(matrix.data(), matrix.rows(), matrix.cols(), matrix.cols()) {
  }

  // operator MatrixView<const T>() const noexcept;
  T &operator()(std::size_t row, std::size_t col) const noexcept {
    assert(row < m_rows);
    assert(col < m_cols);
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
  MatrixView subview(std::size_t start_row, std::size_t start_column,
                     std::size_t num_rows, std::size_t num_cols) const {
    if ((start_row + num_rows > m_rows) || (start_column + num_cols > m_cols)) {
      throw std::out_of_range("Subview out-of-bounds");
    }
    return MatrixView(m_data + index(start_row, start_column), num_rows,
                      num_cols, m_stride);
  }
  void resize(std::size_t nrows, std::size_t ncols) {

  }
  void reshape(std::size_t nrows, std::size_t ncols);

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

int main() {}
