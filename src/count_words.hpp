#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>


class Word {
public:
  Word() noexcept;

  // precondition: all(is_alpha(c) for c in [begin, end))
  Word(const char* begin, const char* end) noexcept;
  Word(const Word&) noexcept = default;
  Word& operator=(const Word&) noexcept = default;
  ~Word() noexcept = default;

  // precondition: all(is_alpha(c) for c in [ptr, ptr+n))
  void set(const char* ptr, std::uint32_t n) noexcept;

  const char* data() const noexcept {
    return m_data;
  }

  std::uint32_t len() const noexcept {
    return static_cast<std::uint32_t>(m_lenhash & 0xffffffff);
  }

  std::uint32_t hash() const noexcept {
    return static_cast<std::uint32_t>(m_lenhash >> 32);
  }

  bool operator==(Word other) const noexcept;
  bool operator<(Word other) const noexcept;

private:
  const char* m_data;
  std::uint64_t m_lenhash;
};

std::vector<std::pair<std::size_t, Word>> count_words(const char* data, std::size_t size);