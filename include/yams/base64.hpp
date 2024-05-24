#pragma once

#include <cctype>
#include <cstdint>
#include <cstring>

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include "compile_time_string.hpp"

namespace yams::base64{
   consteval static auto encoding_table() {
      return ct_str<64>("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
   }

   constexpr static inline bool is_base64(unsigned char c) {
      return (c >= 'A' && c <= 'Z') ||
             (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') ||
             (c == '+') || (c == '/');
   }
   
   consteval static auto decoding_table() {
      return ct_str<256>(
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\62\255"
         "\255\255\63\52\53\54\55\56\57\58\59\60\61\255\255"
         "\255\0\255\255\255\0\1\2\3\4\5\6\7\\8\\9"
         "\10\11\12\13\14\15\16\17\18\19\20\21\22\23\24"
         "\25\255\255\255\255\255\255\26\27\28\29\30\31\32\33"
         "\34\35\36\37\38\39\40\41\42\43\44\45\46\47\48"
         "\49\50\51\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
         "\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255"
      );
   }

   constexpr inline static std::vector<uint8_t> encode(std::string_view input) {
      constexpr auto table = encoding_table();

      std::vector<uint8_t> output;
      std::size_t i = 0;
      std::size_t j = 0;

      const std::size_t output_size = ((input.size() + 2) / 3) * 4;
      std::size_t input_size = input.size();

      std::array<uint8_t, 3> buffer_3;
      std::array<uint8_t, 4> buffer_4;

      output.reserve(output_size);

      const auto& update = [&](std::size_t& index, std::size_t bound) {
         buffer_4[0] = (buffer_3[0] & 0xFC) >> 2;
         buffer_4[1] = ((buffer_3[0] & 0x03) << 4) + ((buffer_3[1] & 0xF0) >> 4);
         buffer_4[2] = ((buffer_3[1] & 0x0F) << 2) + ((buffer_3[2] & 0xC0) >> 6);
         buffer_4[3] = (buffer_3[2] & 0x3F);

         for (index=0; index < bound; index++) {
            output.push_back(table[buffer_4[index]]);
         }
      };

      std::size_t index = 0;

      while (input_size--) {
         buffer_3[i++] = static_cast<uint8_t>(input[index++]);

         if (i == 3) {
            update(i, 4);
            i = 0;
         }
      }

      if (i) {
         for (j=i; j < 3; j++) {
            buffer_3[j] = '\0';
         }
         update(j, i+1);

         while (i++ < 3) {
            output.push_back('=');
         }
      }
      return output;
   }

} // namespace yams