#include "gtest/gtest.h"

#include <iasr/buffer/buffer.hpp>
#include <iasr/buffer/mutable_buffer.hpp>
#include <iasr/buffer/traits.hpp>

using namespace iasr;

TEST(buffer, ctor) {
  buffer buf;
  ASSERT_EQ(buf.size(), 0);
  ASSERT_EQ(buf.data(), nullptr);

  buffer buf2{1024};
  ASSERT_EQ(buf2.size(), 1024);
  ASSERT_NE(buf2.data(), nullptr);
}

TEST(buffer, fill_ctor) {
  buffer buf{1024, 0x33};
  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(buf.data()[i], 0x33);
  }
}

TEST(buffer, range_ctor) {
  std::string s{"hello, there"};
  buffer buf{s.begin(), s.end()};
  ASSERT_EQ(buf.size(), 12);
  ASSERT_EQ(buf.data()[4], 'o');
}

TEST(buffer, moving) {
  buffer buf_orig{1024};
  byte *real_data = buf_orig.data();
  size_t real_size = buf_orig.size();
  auto buf_moved = move(buf_orig);
  byte *moved_real_data = buf_moved.data();
  size_t moved_real_size = buf_moved.size();
  ASSERT_EQ(real_data, moved_real_data);
  ASSERT_EQ(real_size, moved_real_size);
}

TEST(buffer, copying) {
  buffer buf_orig{1024, 0x33};
  buffer buf_copied{buf_orig.begin(), buf_orig.end()};
  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(buf_copied.data()[i], 0x33);
  }
  ASSERT_NE(buf_orig.data(), buf_copied.data());
}

TEST(mutable_buffer, from_buffer_ctor) {
  buffer buf_orig{1024, 0x33};
  mutable_buffer bv{buf_orig};
  for (int i = 0; i < 1024; i++) {
    ASSERT_EQ(bv.data()[i], 0x33);
  }
  ASSERT_EQ(bv.size(), 1024);
}

TEST(mutable_buffer, from_span_ctor) {
  unsigned long longs[5];
  span<unsigned long> sp{longs};
  mutable_buffer bv{sp};
  ASSERT_EQ(bv.size(), 5 * sizeof(unsigned long));
}

TEST(traits, buffer_is_buffer_like) { ASSERT_TRUE(is_buffer_like_v<buffer>); }

TEST(traits, mutable_buffer_is_buffer_like) {
  ASSERT_TRUE(is_buffer_like_v<mutable_buffer>);
}

TEST(traits, span_byte_is_buffer_like) {
  ASSERT_TRUE(is_buffer_like_v<span<byte>>);
}

TEST(traits, span_char_is_buffer_like) {
  ASSERT_TRUE(is_buffer_like_v<span<char>>);
}

TEST(traits, string_is_buffer_like) { ASSERT_TRUE(is_buffer_like_v<string>); }

TEST(traits, vector_of_buffer_is_buffer_seq) {
  ASSERT_TRUE(is_buffer_seq_v<vector<buffer>>);
}

TEST(traits, vector_of_mutable_buffer_is_buffer_seq) {
  ASSERT_TRUE(is_buffer_seq_v<vector<mutable_buffer>>);
}

TEST(traits, span_of_mutable_buffer_is_buffer_seq) {
  ASSERT_TRUE(is_buffer_seq_v<span<mutable_buffer>>);
}

TEST(traits, vector_of_string_is_buffer_seq) {
  ASSERT_TRUE(is_buffer_seq_v<vector<string>>);
}

TEST(traits, array_of_string_is_buffer_seq) {
  ASSERT_TRUE((is_buffer_seq_v<array<string, 10>>));
}

TEST(traits, buffer_is_not_buffer_seq) {
  ASSERT_FALSE(is_buffer_seq_v<buffer>);
}

TEST(traits, mutable_buffer_is_not_buffer_seq) {
  ASSERT_FALSE(is_buffer_seq_v<mutable_buffer>);
}
