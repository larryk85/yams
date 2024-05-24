#pragma once

#include <cctype>
#include <cstdint>
#include <algorithm>
#include <string_view>

namespace yams {
   template<std::size_t N>
   struct ct_str {
      char _data[N] {};

      consteval ct_str(const char (&str)[N]) { std::copy_n(str, N, _data); }

      consteval ct_str(std::string_view str) { std::copy_n(str.begin(), N, _data); }

      consteval ct_str(const ct_str<N>& str) { std::copy_n(str._data, N, _data); }

      consteval ct_str<N>& operator=(const ct_str<N>& str) {
         std::copy_n(str._data, N, _data);
         return *this;
      }

      consteval ct_str<N>& operator=(const char (&str) [N]) {
         std::copy_n(str, N, _data);
         return *this;
      }


      consteval bool operator==(const ct_str<N> str) const { return std::equal(str._data, str._data+N, _data); }

      template<std::size_t N2>
      consteval bool operator==(const ct_str<N2> s) const { return false; }

      template<std::size_t N2>
      consteval ct_str<N+N2-1> operator+(const ct_str<N2> str) const {
         char newchar[N+N2-1] {};
         std::copy_n(_data, N-1, newchar);
         std::copy_n(str._data, N2, newchar+N-1);
         return newchar;
      }

      constexpr char operator[](std::size_t n) const { return _data[n]; }
      template <std::size_t I>
      consteval char at() const { return _data[I]; }
      consteval auto data() const { return _data; }
      consteval std::size_t size() const { return N-1; }
   };

   template<std::size_t S1, std::size_t S2>
   consteval auto operator+(ct_str<S1> fs, const char (&str) [S2]) {
      return fs + ct_str<S2>(str);
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator+(const char (&str) [S2], ct_str<S1> fs) {
      return ct_str<S2>(str) + fs;
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator==(ct_str<S1> fs, const char (&str) [S2]) {
      return fs == ct_str<S2>(str);
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator==(const char (&str) [S2], ct_str<S1> fs) {
      return ct_str<S2>(str) == fs;
   }
} // namespace yams