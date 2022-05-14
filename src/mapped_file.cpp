#include "mapped_file.hpp"

#include <unistd.h>   // ::close()
#include <fcntl.h>    // ::open()
#include <sys/stat.h> // ::stat()
#include <sys/mman.h> // ::mmap()/::munmap()/::madvise()


MappedFile::MappedFile(MappedFile&& other) noexcept
  : m_data(other.m_data)
  , m_size(other.m_size)
{
  other.reset();
}

MappedFile& MappedFile::operator=(MappedFile&& other) noexcept {
  if (this != &other) {
    close();
    m_data = other.m_data;
    m_size = other.m_size;
    other.reset();
  }

  return *this;
}

MappedFile::~MappedFile() noexcept {
  close();
}

bool MappedFile::is_open() const noexcept {
  return m_data != nullptr && m_size != 0;
}

std::error_code MappedFile::open(const char* path) noexcept  {
  const int flags = 0;
  int fd = ::open(path, flags);
  if (fd < 0) {
    return std::error_code(errno, std::system_category());
  }

  struct stat stats;
  if (::fstat(fd, &stats) < 0) {
    ::close(fd);
    return std::error_code(errno, std::system_category());
  }

  void* ptr = ::mmap(nullptr, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
  ::close(fd);
  if (ptr == MAP_FAILED) {
    return std::error_code(errno, std::system_category());
  }

  m_data = reinterpret_cast<char*>(ptr);
  m_size = stats.st_size;
  return {};
}

std::error_code MappedFile::close() noexcept {
  std::error_code ec;
  if (m_data) {
    if (::munmap(const_cast<char*>(m_data), m_size) < 0) {
      ec = std::error_code(errno, std::system_category());
    }
  }

  reset();
  return ec;
}

std::error_code MappedFile::hint_sequential() const noexcept {
  if (!is_open()) {
    return std::error_code(EINVAL, std::system_category());
  }

  if (::madvise(const_cast<char*>(m_data), m_size, MADV_SEQUENTIAL) < 0) {
    return std::error_code(errno, std::system_category());
  }

  return {};
}
