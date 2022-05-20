#pragma once

#include <nrf_assert.h>

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace Pinetime {
  /**
   * @brief flat linked list for POD types, T should define these members:
   *
   * - std::uint8_t fllNextIdx;
   * - std::uint8_t fllPrevIdx;
   *
   * @tparam T
   * @tparam capacity
   */
  template <typename T, std::uint8_t maxSize> //
  class FlatLinkedList {
    static_assert(maxSize > 0, "FlatLinkedList maxSize should at least be 1");

  public:
    using value_type = T;
    using size_type = std::uint8_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    static constexpr size_type npos = std::numeric_limits<size_type>::max();

    static constexpr size_type capacity() {
      return maxSize;
    }

    template <bool isConst> //
    class Iterator {
    private:
      friend FlatLinkedList;
      using cond_reference = std::conditional_t<isConst, const_reference, reference>;
      using cond_pointer = std::conditional_t<isConst, const_pointer, pointer>;
      using cond_FlatLinkedList = std::conditional_t<isConst, const FlatLinkedList, FlatLinkedList>;

      constexpr Iterator(size_type pos, cond_FlatLinkedList& fll) : pos {pos}, fll {fll} {
      }

    public:
      constexpr Iterator& operator++() {
        pos = fll.arr[pos].fllNextIdx;
        return *this;
      }
      constexpr Iterator& operator--() {
        if (pos == npos) {
          pos = fll.lastIdx;
        } else {
          pos = fll.arr[pos].fllPrevIdx;
        }
        return *this;
      }
      constexpr cond_reference operator*() {
        return fll.arr[pos];
      }
      constexpr cond_pointer operator->() {
        return &fll.arr[pos];
      }
      constexpr explicit operator cond_pointer() const {
        return &fll.arr[pos];
      }

      friend constexpr bool operator==(Iterator const& lhs, Iterator const& rhs) {
        return lhs.pos == rhs.pos;
      }

      friend constexpr bool operator!=(Iterator const& lhs, Iterator const& rhs) {
        return !(lhs == rhs);
      }

    private:
      size_type pos;
      cond_FlatLinkedList& fll;
    };

    using iterator = Iterator</*isConst:*/ false>;
    using const_iterator = Iterator</*isConst:*/ true>;

    constexpr FlatLinkedList() {
      arr[firstFreeIdx].fllNextIdx = npos;
      arr[firstFreeIdx].fllPrevIdx = npos;
    }

    constexpr iterator emplace_back() {
      size_type newIdx = takeFreeElem();

      arr[newIdx].fllPrevIdx = lastIdx;
      if (!empty()) {
        arr[lastIdx].fllNextIdx = newIdx;
      } else {
        firstIdx = newIdx;
      }
      lastIdx = newIdx;
      ++sz;
      return iterator(lastIdx, *this);
    }

    /**
     * @brief Inserts a new element into the container directly before pos.
     *
     * @param pos
     * @return constexpr iterator
     */
    constexpr iterator emplace(iterator pos) {
      size_type newIdx = takeFreeElem();

      arr[newIdx].fllPrevIdx = lastIdx;
      if (!empty()) {
        arr[lastIdx].fllNextIdx = newIdx;
      } else {
        firstIdx = newIdx;
      }
      lastIdx = newIdx;
      ++sz;
      return iterator(lastIdx, *this);
    }

    constexpr reference front() {
      ASSERT(!empty());
      return arr[firstIdx];
    }
    constexpr const_reference front() const {
      ASSERT(!empty());
      return arr[firstIdx];
    }
    constexpr reference back() {
      ASSERT(!empty());
      return arr[lastIdx];
    }
    constexpr const_reference back() const {
      ASSERT(!empty());
      return arr[lastIdx];
    }

    constexpr iterator begin() {
      return iterator(firstIdx, *this);
    }
    constexpr const_iterator begin() const {
      return const_iterator(firstIdx, *this);
    }
    constexpr iterator end() {
      return iterator(npos, *this);
    }
    constexpr const_iterator end() const {
      return const_iterator(npos, *this);
    }

    constexpr size_type size() const {
      return sz;
    }

    constexpr bool empty() const {
      return sz == 0;
    }

    // std::array<value_type, capacity>& GetArray() {
    //   return arr;
    // }

  private:
    constexpr size_type takeFreeElem() {
      ASSERT(size() < capacity());

      // get a new free element
      const size_type newIdx = firstFreeIdx;

      if (arr[firstFreeIdx].fllNextIdx == npos) {
        // arr[firstFreeIdx..N] are all free
        ++firstFreeIdx;
        arr[firstFreeIdx].fllNextIdx = npos;
      } else {
        firstFreeIdx = arr[firstFreeIdx].fllNextIdx;
      }
      arr[newIdx].fllNextIdx = npos;
      return newIdx;
    }

    std::array<value_type, maxSize> arr;
    size_type sz {0};
    size_type firstIdx {npos};
    size_type lastIdx {npos};
    /**
     * @brief linked list of free elements
     * if arr[firstFreeIdx].fllNextIdx == npos then [firstFreeIdx..N] are all free
     */
    size_type firstFreeIdx {0};
  };
}