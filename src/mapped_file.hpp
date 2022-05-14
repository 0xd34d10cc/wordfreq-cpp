#pragma once

#include <cstddef>
#include <system_error>


class MappedFile {
public:
  MappedFile() = default;
  MappedFile(const MappedFile& other) = delete;
  MappedFile(MappedFile&& other) noexcept;
  MappedFile& operator=(const MappedFile&) = delete;
  MappedFile& operator=(MappedFile&& other) noexcept;
  ~MappedFile() noexcept;

  bool is_open() const noexcept;

  std::error_code open(const char* path) noexcept;
  std::error_code close() noexcept;
  std::error_code hint_sequential() const noexcept;

  const char* data() const noexcept {
    return m_data;
  }

  std::size_t size() const noexcept {
    return m_size;
  }

private:
  void reset() noexcept {
    m_data = nullptr;
    m_size = 0;
  }

  const char* m_data{ nullptr };
  std::size_t m_size{ 0 };
};
