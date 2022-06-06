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
      using cond_reference = std::conditional_t<isConst, FlatLinkedList::const_reference, FlatLinkedList::reference>;
      using cond_pointer = std::conditional_t<isConst, FlatLinkedList::const_pointer, FlatLinkedList::pointer>;
      using cond_FlatLinkedList = std::conditional_t<isConst, const FlatLinkedList, FlatLinkedList>;

      constexpr Iterator(size_type pos, cond_FlatLinkedList& fll) : pos {pos}, fll {&fll} {
      }

    public:
      using iterator_category = std::bidirectional_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = FlatLinkedList::value_type;
      using pointer = cond_pointer;
      using reference = cond_reference;

      constexpr Iterator(Iterator<false> const& other) : pos {other.pos}, fll {other.fll} {
      }

      constexpr Iterator& operator++() {
        pos = fll->arr[pos].fllNextIdx;
        return *this;
      }
      constexpr Iterator operator++(int) const {
        auto tmp = *this;
        ++(*this);
        return tmp;
      }
      constexpr Iterator& operator--() {
        if (pos == npos) {
          pos = fll->lastIdx;
        } else {
          pos = fll->arr[pos].fllPrevIdx;
        }
        return *this;
      }
      constexpr Iterator operator--(int) const {
        auto tmp = *this;
        --(*this);
        return tmp;
      }

      constexpr reference operator*() const {
        return fll->arr[pos];
      }
      constexpr pointer operator->() const {
        return &fll->arr[pos];
      }

      constexpr explicit operator pointer() const {
        return &fll->arr[pos];
      }

      constexpr bool HasNext() const {
        return pos != npos && (*this)->fllNextIdx != npos;
      }

      constexpr bool HasPrev() const {
        if (pos == npos) {
          return fll->lastIdx != npos;
        } else {
          return (*this)->fllPrevIdx != npos;
        }
      }

      friend constexpr bool operator==(Iterator const& lhs, Iterator const& rhs) {
        return lhs.pos == rhs.pos;
      }

      friend constexpr bool operator!=(Iterator const& lhs, Iterator const& rhs) {
        return !(lhs == rhs);
      }

    private:
      size_type pos;
      cond_FlatLinkedList* fll;
    };

    using iterator = Iterator</*isConst:*/ false>;
    using const_iterator = Iterator</*isConst:*/ true>;

    constexpr FlatLinkedList() {
      arr[firstFreeIdx].fllNextIdx = npos;
      arr[firstFreeIdx].fllPrevIdx = npos;
    }

    constexpr iterator emplace_back() {
      return emplace(end());
    }

    /**
     * @brief Inserts a new element into the container directly before pos.
     *
     * @param pos
     * @return constexpr iterator
     */
    constexpr iterator emplace(const_iterator pos) {
      size_type newIdx = takeFreeElem();

      arr[newIdx].fllNextIdx = pos.pos;
      if (pos != end()) {
        arr[newIdx].fllPrevIdx = pos->fllPrevIdx;
        // pos->fllPrevIdx = newIdx; // can't on const_iterator
        arr[pos.pos].fllPrevIdx = newIdx;
      } else {
        arr[newIdx].fllPrevIdx = lastIdx;
        lastIdx = newIdx;
      }
      if (arr[newIdx].fllPrevIdx != npos) {
        arr[arr[newIdx].fllPrevIdx].fllNextIdx = newIdx;
      } else {
        firstIdx = newIdx;
      }
      return iterator(newIdx, *this);
    }

    constexpr iterator erase(const_iterator pos) {
      auto prevIdx = pos->fllPrevIdx;
      auto nextIdx = pos->fllNextIdx;
      if (prevIdx == npos) {
        firstIdx = nextIdx;
      } else {
        arr[prevIdx].fllNextIdx = nextIdx;
      }
      if (nextIdx == npos) {
        lastIdx = prevIdx;
      } else {
        arr[nextIdx].fllPrevIdx = prevIdx;
      }

      arr[pos.pos].fllNextIdx = firstFreeIdx;
      firstFreeIdx = pos.pos;

      --sz;
      return iterator(nextIdx, *this);
    }

    constexpr void pop_back() {
      erase(iterator {lastIdx, *this});
    }

    constexpr void pop_front() {
      erase(begin());
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
      ASSERT(firstFreeIdx < capacity());

      // get a new free element
      const size_type newIdx = firstFreeIdx;
      ++sz;

      if (arr[firstFreeIdx].fllNextIdx == npos) {
        // arr[firstFreeIdx..N] are all free
        ++firstFreeIdx;
        if (size() < capacity()) {
          ASSERT(firstFreeIdx < capacity());
          arr[firstFreeIdx].fllNextIdx = npos;
        } else {
          firstFreeIdx = npos;
        }
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