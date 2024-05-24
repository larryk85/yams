#pragma once

#include <cctype>
#include <cstdint>
#include <algorithm>
#include <string_view>

namespace yams {
   template<std::size_t N>
   struct ct_str {
      char _data[N] {};

      consteval ct_str(const char (&str)[N]) noexcept { std::copy_n(str, N, _data); }

      consteval ct_str(std::string_view str) noexcept { std::copy_n(str.begin(), N, _data); }

      consteval ct_str(const ct_str<N>& str) noexcept { std::copy_n(str._data, N, _data); }

      consteval ct_str<N>& operator=(const ct_str<N>& str) noexcept {
         std::copy_n(str._data, N, _data);
         return *this;
      }

      consteval ct_str<N>& operator=(const char (&str) [N]) noexcept {
         std::copy_n(str, N, _data);
         return *this;
      }


      consteval bool operator==(const ct_str<N> str) const noexcept { return std::equal(str._data, str._data+N, _data); }

      template<std::size_t N2>
      consteval bool operator==(const ct_str<N2> s) const noexcept { return false; }

      template<std::size_t N2>
      consteval ct_str<N+N2-1> operator+(const ct_str<N2> str) const noexcept {
         char newchar[N+N2-1] {};
         std::copy_n(_data, N-1, newchar);
         std::copy_n(str._data, N2, newchar+N-1);
         return newchar;
      }

      constexpr char operator[](std::size_t n) const noexcept { return _data[n]; }
      template <std::size_t I>
      consteval char at() const noexcept { return _data[I]; }
      consteval auto data() const noexcept { return _data; }
      constexpr auto data() noexcept { return _data; }
      consteval std::size_t size() const noexcept { return N-1; }
   };

   template<std::size_t S1, std::size_t S2>
   consteval auto operator+(ct_str<S1> fs, const char (&str) [S2]) noexcept {
      return fs + ct_str<S2>(str);
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator+(const char (&str) [S2], ct_str<S1> fs) noexcept {
      return ct_str<S2>(str) + fs;
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator==(ct_str<S1> fs, const char (&str) [S2]) noexcept {
      return fs == ct_str<S2>(str);
   }

   template<std::size_t S1, std::size_t S2>
   consteval auto operator==(const char (&str) [S2], ct_str<S1> fs) noexcept {
      return ct_str<S2>(str) == fs;
   }
} // namespace yams