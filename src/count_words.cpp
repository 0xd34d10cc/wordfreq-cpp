#include "count_words.hpp"

#include <unordered_map>
#include <algorithm>

// 3rd-party libraries
#include "robin_hood.hpp"

#define LIKELY(expr) __builtin_expect((expr), 1)
#define UNLIKELY(expr) __builtin_expect((expr), 0)

static const std::uint32_t FNV32_INIT = 0x811c9dc5u;

static bool is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

// precondition: is_alpha(c)
static char lower(char c) {
  return c | (1 << 5);
}

// precondition: all(is_alpha(c) for c in [ptr, ptr+n))
static std::uint32_t hash_word(const char* ptr, std::uint32_t n) {
  std::uint32_t hash = FNV32_INIT;
  // Make compiler generate specialized version for short words, which are much more common
  if (LIKELY(n < 16)) {
    while (n) {
      hash = (hash * 0x01000193u) ^ static_cast<std::uint32_t>(lower(*ptr));
      ++ptr;
      --n;
    }
  } else {
    while (n) {
      hash = (hash * 0x01000193u) ^ static_cast<std::uint32_t>(lower(*ptr));
      ++ptr;
      --n;
    }
  }
  return hash;
}

Word::Word() noexcept
  : m_data(nullptr)
  , m_lenhash(static_cast<std::uint64_t>(FNV32_INIT) << 32)
{}

Word::Word(const char* begin, const char* end) noexcept {
  set(begin, static_cast<std::uint32_t>(end - begin));
}

void Word::set(const char* ptr, std::uint32_t n) noexcept {
  std::uint32_t hash = hash_word(ptr, n);
  m_data = ptr;
  m_lenhash = (static_cast<std::uint64_t>(hash) << 32) | static_cast<std::uint64_t>(n);
}

bool Word::operator==(Word other) const noexcept {
  if (m_lenhash != other.m_lenhash) {
    return false;
  }

  const std::uint32_t length = len();
  if (length < 6) {
    // there are no fnv32 hash collisions for [a-z] strings with equal len < 6
    return true;
  }

  // Make compiler generate specialized version for small words, which are much more common
  if (LIKELY(length < 16)) {
    for (std::uint32_t i = 0; i < length; ++i) {
      // if hashes are equal it is unlikely that words aren't
      if (UNLIKELY(lower(m_data[i]) != lower(other.m_data[i]))) {
        return false;
      }
    }
  } else {
    for (std::uint32_t i = 0; i < length; ++i) {
      // if hashes are equal it is unlikely that words aren't
      if (UNLIKELY(lower(m_data[i]) != lower(other.m_data[i]))) {
        return false;
      }
    }
  }

  return true;
}

bool Word::operator<(Word other) const noexcept {
  const std::uint32_t length = len() < other.len() ? len() : other.len();

  // Make compiler generate specialized version for small words, which are much more common
  if (LIKELY(length < 16)) {
    for (std::uint32_t i = 0; i < length; ++i) {
      char left = lower(m_data[i]);
      char right = lower(other.m_data[i]);

      if (left < right) {
        return true;
      } else if (left > right) {
        return false;
      }
    }
  } else {
    for (std::uint32_t i = 0; i < length; ++i) {
      char left = lower(m_data[i]);
      char right = lower(other.m_data[i]);

      if (left < right) {
        return true;
      } else if (left > right) {
        return false;
      }
    }
  }

  return len() < other.len();
}

struct WordHasher {
  std::size_t operator()(Word w) const noexcept {
    const auto h = static_cast<std::size_t>(w.hash());
    return (h << 32) | h;
  }
};

using FreqMap = robin_hood::unordered_map<Word, std::size_t, WordHasher>;

std::vector<std::pair<std::size_t, Word>> count_words(const char* data, std::size_t size) {
  FreqMap words;
  words.reserve(4096);

  const char* start = data;
  const char* eof = data + size;

  while (start != eof && !is_alpha(*start)) {
    ++start;
  }

  const char* end = start;
  while (start != eof) {
    while (end != eof && is_alpha(*end)) {
      ++end;
    }

    ++words[Word(start, end)];

    while (end != eof && !is_alpha(*end)) {
      ++end;
    }

    start = end;
  }

  std::vector<std::pair<std::size_t, Word>> sorted_words;
  sorted_words.reserve(words.size());
  for (auto [word, freq]: words) {
    sorted_words.push_back(std::make_pair(freq, word));
  }

  std::sort(sorted_words.begin(), sorted_words.end(), [](const auto& left, const auto& right) {
    return std::make_pair(-static_cast<std::ptrdiff_t>(left.first), left.second) <
           std::make_pair(-static_cast<std::ptrdiff_t>(right.first), right.second);
  });
  return sorted_words;
}
