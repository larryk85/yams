#pragma once

#include <Windows.h>
#include <string_view>
#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <limits>
#include <filesystem>
#include <array>

namespace fast {
   void PrintLastError() {
      DWORD msg_id = ::GetLastError();
      if (msg_id == 0) {
         std::cout << "No error occurred." << std::endl;
         return;
      }

      LPSTR msg_buf = nullptr;

      size_t size = FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, msg_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         (LPSTR)&msg_buf, 0, NULL);

      std::string_view msg = {msg_buf, size};

      std::cout << "Error: " << msg << std::endl;

      LocalFree(msg_buf);
   }

   namespace detail {
      struct win_handle {
         HANDLE handle;
         win_handle(HANDLE h) : handle(h) {}
         ~win_handle() {
            if (handle && handle != INVALID_HANDLE_VALUE) {
               CloseHandle(handle);
            }
         }
          inline constexpr win_handle(const win_handle& other) = delete;
          inline constexpr win_handle& operator=(const win_handle& other) = delete;
          inline constexpr win_handle(win_handle&& other) noexcept : handle{other.handle} {
             other.handle = nullptr;
          }

          inline constexpr win_handle& operator=(win_handle&& other) noexcept {
             if (this != &other) {
                if (handle && handle != INVALID_HANDLE_VALUE) {
                   CloseHandle(handle);
                }
                handle = other.handle;
                other.handle = nullptr;
             }
             return *this;
          }
         
         inline constexpr operator bool() const {
            return handle != nullptr && handle != INVALID_HANDLE_VALUE;
         }

         inline constexpr auto operator*() const {
            return handle;
         }
      };
   } // namespace detail

   class loader_impl {
      public:
         struct loader_impl_error : std::runtime_error {
            loader_impl_error(std::string_view msg) : std::runtime_error(msg.data()) {}
         };

         inline loader_impl() : file_handle(nullptr), mapping(nullptr), mapping_view(nullptr) {}

         inline ~loader_impl() {
            std::cout << "Unloading" << std::endl;
            unload();
         }
         inline auto get_size() const {
            PLARGE_INTEGER size = 0;
            if (GetFileSizeEx(*file_handle, size) == 0) {
               throw loader_impl_error("Failed to get file size");
            }
            return size->QuadPart;
         }

         constexpr inline auto get_view() const {
            return mapping_view;
         }

         constexpr inline auto get_mapping() const {
            return *mapping;
         }

         constexpr inline auto get_file() const {
            return *file_handle;
         }

         inline std::wstring get_file_name() {
            DWORD sz = GetFinalPathNameByHandleW(*file_handle, nullptr, 0, FILE_NAME_OPENED);
            std::wstring fn(sz+1, '\0');
            if (GetFinalPathNameByHandleW(*file_handle, fn.data(), sz+1, FILE_NAME_OPENED) == 0) {
               PrintLastError();
               throw std::runtime_error("Failed to get file name from handle");
            }
            return fn;
         }

         inline void load(std::string_view fn) {
            auto mode = std::filesystem::exists(fn) ? OPEN_EXISTING : CREATE_NEW;
            file_handle = {CreateFileA(fn.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, mode, FILE_ATTRIBUTE_NORMAL, nullptr)};
            std::wcout << "MN: " << get_file_name() << std::endl;

            if (*file_handle == INVALID_HANDLE_VALUE) {
               PrintLastError();
               throw loader_impl_error("File Creation Failed or File Not Found");
            }

            mapping = CreateFileMappingA(*file_handle, nullptr, PAGE_READWRITE, 0, INT_MAX, nullptr);
            std::cout << "mapping: " << mapping << std::endl;
            PrintLastError();
            if (!mapping) {
               PrintLastError();
               throw loader_impl_error("Initial Mapping Failed");
            }

            mapping_view = MapViewOfFile(*mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (mapping_view == nullptr) {
               throw loader_impl_error("Mapping View Failed");
            }
         }

         inline void unload() {
            if (mapping_view) {
               UnmapViewOfFile(mapping_view);
               mapping_view = nullptr;
            }
            if (mapping) {
               CloseHandle(*mapping);
               mapping = nullptr;
            }
         }

      private:
         detail::win_handle file_handle;
         detail::win_handle mapping;
         void* mapping_view;
   };
} // namespace fast