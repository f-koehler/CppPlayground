#ifndef CPPPLAYGROUND_CONTAINERS_LRU_CACHE_HPP
#define CPPPLAYGROUND_CONTAINERS_LRU_CACHE_HPP

#include <cstddef>
#include <list>
#include <optional>
#include <unordered_map>
#include <utility>

namespace CppPlayground {

template <typename Key, typename T> class LRUCache {
public:
  using key_type = Key;
  using mapped_type = T;

  explicit LRUCache(std::size_t capacity)
      : m_lookup(capacity), m_capacity(capacity) {}

  [[nodiscard]] std::size_t size() const noexcept { return m_elements.size(); }

  template <typename K, typename V> T &insert(K &&key, V &&value) {
    auto pos = m_lookup.find(key);
    if (pos == m_lookup.end()) {
      // new key
      m_elements.emplace_front(std::forward<K>(key), std::forward<V>(value));
      m_lookup.emplace(m_elements.front().key, m_elements.begin());
      shrink_to_fit();
      return m_elements.front().value;
    }

    // existing key
    auto list_pos = pos->second;
    list_pos->value = std::forward<V>(value);
    promote_element(list_pos);
    return list_pos->value;
  }

  std::optional<T> get(const Key &key) {
    auto pos = m_lookup.find(key);
    if (pos == m_lookup.end()) {
      return {};
    }
    auto list_pos = pos->second;
    promote_element(list_pos);
    return list_pos->value;
  }

private:
  struct ListEntry {
    Key key;
    T value;
  };
  using ListIterator = typename std::list<ListEntry>::iterator;

  std::list<ListEntry> m_elements;
  std::unordered_map<Key, ListIterator> m_lookup;
  std::size_t m_capacity;

  void shrink_to_fit() {
    while (m_elements.size() > m_capacity) {
      m_lookup.erase(m_elements.back().key);
      m_elements.pop_back();
    }
  }
  void promote_element(ListIterator pos) {
    m_elements.splice(m_elements.begin(), m_elements, pos);
  }
};

} // namespace CppPlayground

#endif
