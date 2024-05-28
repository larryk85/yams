#pragma once

#include <cctype>
#include <cstdint>
#include <cstring>

#include <array>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


#include "compile_time_string.hpp"

/// @brief Namespace for the YAMS library base64 encoding and decoding functions
/**
 * @namespace yams::base64
 * @brief Namespace for base64 encoding and decoding functions.
 */
namespace yams::base64{

   /**
    * @brief The base64 character set used for encoding and decoding.
    */
   constexpr static inline auto charset = ct_str<64>("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

   namespace detail {
      /**
       * @brief Get the index of a character in the base64 character set.
       * @param c The character to find the index of.
       * @return The index of the character in the character set, or -1 if not found.
       */
      constexpr std::size_t get_index(char c) { 
         for (std::size_t i=0; i < 256; i++) {
            if (charset[i] == c) {
               return i;
            }
         }
         return -1;
      }

      /**
       * @brief Generate an array of indices for each character in the base64 character set.
       * @tparam Is The indices.
       * @param sequence The sequence of indices.
       * @return The array of indices.
       */
      template <std::size_t... Is>
      constexpr std::array<std::size_t, 256> generate_indices(std::index_sequence<Is...>) {
         return { detail::get_index(charset[Is])... };
      }

      constexpr static inline int8_t get_padding(std::span<const uint8_t> input) {
         std::size_t sz = input.size();

         constexpr std::array<uint8_t, 2> data = {'=', '='};

         if (sz > 0) {
            if (input[sz-1] == data[0]) {
               return 1;
            }
            if (sz > 1) {
               if (input[sz-2] == data[0] && input[sz-1] == data[1]) {
                  return 2;
               }
            }
         }
         switch (std::size_t sz = input.size(); sz) {
            case 0:
               return 0;
            case 1:
               return input[sz-1] == '=' ? 1 : 0;
            default:
               return input[sz-1] == '=' ? 1 : 0 + (input[sz-2] == '=' ? 1 : 0);
         }
      }
   } // namespace detail

   /**
    * @brief The array of indices for each character in the base64 character set.
    */
   constexpr static inline std::array<std::size_t, 256> indices = detail::generate_indices(std::make_index_sequence<64>{});

   /**
    * @brief Check if a character is a valid base64 character.
    * @param c The character to check.
    * @return True if the character is a valid base64 character, false otherwise.
    */
   constexpr static inline bool is_base64(unsigned char c) {
      return (c >= 'A' && c <= 'Z') ||
             (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') ||
             (c == '+') || (c == '/');
   }

   /**
    * @brief Alias for a binary data type.
    */
   using binary_t = std::vector<uint8_t>;
   
   /**
    * @brief Encode a string into base64 format.
    * @param input The input string to encode.
    * @return The encoded binary data.
    */
   constexpr inline static binary_t encode(std::string_view input) {
      binary_t output;
      std::array<uint8_t, 3> buffer_3;

      output.reserve(((input.size() + 2) / 3) * 4);

      const auto& update = [&](std::size_t& index, std::size_t bound) {
         std::array<uint8_t, 4> buffer_4;
         buffer_4[0] = (buffer_3[0] & 0xFC) >> 2;
         buffer_4[1] = ((buffer_3[0] & 0x03) << 4) + ((buffer_3[1] & 0xF0) >> 4);
         buffer_4[2] = ((buffer_3[1] & 0x0F) << 2) + ((buffer_3[2] & 0xC0) >> 6);
         buffer_4[3] = (buffer_3[2] & 0x3F);

         for (index=0; index < bound; index++) {
            output.push_back(charset[buffer_4[index]]);
         }
      };

      std::size_t index = 0;

      std::size_t i = 0;
      for (std::size_t x=0, l=input.size(); l || x < l; x++, l--) {
         buffer_3[i++] = static_cast<uint8_t>(input[x]);

         if (i == 3) {
            update(i, 4);
            i = 0;
         }
      }

      std::size_t j = 0;
      for (j=i; j < 3; j++) {
         buffer_3[j] = '\0';
      }
      update(j, i+1);
      switch(i) {
         default: return output;
         case 1:
            output.push_back('=');
            output.push_back('=');
            return output;
         case 2:
            output.push_back('=');
            return output;
      }
   }

   /**
    * @brief Decode base64 encoded binary data into a string.
    * @param input The input binary data to decode.
    * @return The decoded string.
    */
   inline static std::string decode(const binary_t& input) {
      if (input.empty()) { return {}; }
      std::size_t el = input.size() - detail::get_padding(input);
      std::size_t dl = (el * 3) / 4;

      std::string output;
      output.reserve(dl);

      std::size_t n = 0;

      for (std::size_t i = 0, x = 0; i < input.size(); i++) {
         if (std::isspace(static_cast<uint8_t>(input[i]))) {
            // skip newlines
            continue;
         }

         std::size_t d = indices[input[i]];
         std::cout << "D: " << d << '\n';
         std::cout << "i = " << i << '\n';
         std::cout << "x = " << x << '\n';
         std::cout << "input[i] = " << input[i] << '\n';
         if (d == -1) {
            throw std::runtime_error("Invalid base64 character");
         }

         n = (n << 6) | d;
         if (x % 4 == 3) {
            output.push_back((n >> 16) & 0xFF);
            if (input[i] != '=') {
               if (i > 0) {
                  output.push_back((n >> 8) & 0xFF);
               }
               output.push_back(n & 0xFF);
            }
         }
         x++;
      }

      return output;
   }


} // namespace yams