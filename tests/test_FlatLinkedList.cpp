#include <FlatLinkedList.h>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <algorithm>
#include <iterator>
#include <vector>
#include <list>
#include <random>

template <typename T, std::uint8_t N> //
std::ostream& operator<<(std::ostream& out, Pinetime::FlatLinkedList<T, N> const& fll) {
  out << "[";
  if (!fll.empty()) {
    std::copy(std::begin(fll), std::prev(std::end(fll)), std::ostream_iterator<T>(out, ", "));
    out << fll.back();
  }
  out << "]";
  return out;
}

template <typename T> //
std::ostream& operator<<(std::ostream& out, std::list<T> const& list) {
  out << "[";
  if (!list.empty()) {
    std::copy(std::begin(list), std::prev(std::end(list)), std::ostream_iterator<T>(out, ", "));
    out << list.back();
  }
  out << "]";
  return out;
}

template <typename T> //
std::ostream& operator<<(std::ostream& out, std::vector<T> const& vector) {
  out << "[";
  if (!vector.empty()) {
    std::copy(std::begin(vector), std::prev(std::end(vector)), std::ostream_iterator<T>(out, ", "));
    out << vector.back();
  }
  out << "]";
  return out;
}

struct PODTest {
  int i;

  std::uint8_t fllNextIdx;
  std::uint8_t fllPrevIdx;

  friend constexpr bool operator==(PODTest const& lhs, PODTest const& rhs) noexcept {
    return lhs.i == rhs.i;
  }

  friend constexpr bool operator!=(PODTest const& lhs, PODTest const& rhs) noexcept {
    return !(lhs == rhs);
  }

  friend constexpr bool operator<(PODTest const& lhs, PODTest const& rhs) noexcept {
    return lhs.i < rhs.i;
  }
};

std::ostream& operator<<(std::ostream& out, PODTest const& pod) {
  return out << "{i: " << pod.i << ", n:" << static_cast<unsigned>(pod.fllNextIdx) << ", p:" << static_cast<unsigned>(pod.fllPrevIdx)
             << "}";
}

std::mt19937 gen(std::random_device {}());

TEST_CASE("FlatLinkedList", "[FlatLinkedList]") {
  constexpr std::uint8_t FLL_CAPACITY = 10;
  Pinetime::FlatLinkedList<PODTest, FLL_CAPACITY> fll;

  SECTION("Empty FlatLinkedList") {
    REQUIRE(fll.capacity() == FLL_CAPACITY);
    REQUIRE(fll.size() == 0);
    REQUIRE(fll.empty());
    REQUIRE(fll.begin() == fll.end());
  }

  SECTION("emplace_back") {
    auto it = fll.emplace_back();
    CAPTURE(fll);
    REQUIRE(fll.begin() == it);
    REQUIRE(!fll.empty());
    REQUIRE(fll.size() == 1);
    REQUIRE(fll.begin() != fll.end());
    REQUIRE(!it.HasNext());
    REQUIRE(!it.HasPrev());
    it->i = 8469;
    REQUIRE(*it == fll.back());
    REQUIRE(*it == fll.front());
    REQUIRE(it->i == fll.front().i);
    REQUIRE(fll.back() == fll.front());
    REQUIRE(&fll.back() == &fll.front());
  }
  // - fill
  // - clear it one by one
  // - fill it again
  SECTION("fill with emplace_back") {
    std::vector<PODTest> vec;
    vec.reserve(fll.capacity());
    for (std::size_t i = 0; i < fll.capacity(); ++i) {
      REQUIRE(fll.capacity() == FLL_CAPACITY);
      REQUIRE(fll.size() == i);

      auto it = fll.emplace_back();
      it->i = i;
      vec.emplace_back(PODTest {i});
      CAPTURE(vec, fll);

      REQUIRE(!it.HasNext());
      if (i != 0) {
        REQUIRE(it.HasPrev());
      } else {
        REQUIRE(!it.HasPrev());
      }
      REQUIRE(std::equal(std::begin(fll), std::end(fll), std::begin(vec), std::end(vec)));
      REQUIRE(std::equal(
        std::make_reverse_iterator(std::end(fll)), std::make_reverse_iterator(std::begin(fll)), std::rbegin(vec), std::rend(vec)));
      REQUIRE(fll.front() == vec.front());
      REQUIRE(fll.back() == vec.back());
      REQUIRE(!fll.empty());
    }

    CAPTURE(vec, fll);
    REQUIRE(fll.size() == fll.capacity());
  }
  SECTION("fill with emplace(begin())") {
    std::list<PODTest> list;

    for (std::size_t i = 0; i < fll.capacity(); ++i) {
      REQUIRE(fll.capacity() == FLL_CAPACITY);
      REQUIRE(fll.size() == i);

      auto it = fll.emplace(fll.begin());
      it->i = i;
      list.emplace_front(PODTest {i});
      CAPTURE(list, fll);

      REQUIRE(!it.HasPrev());
      if (i != 0) {
        REQUIRE(it.HasNext());
      } else {
        REQUIRE(!it.HasNext());
      }

      REQUIRE(std::equal(std::begin(fll), std::end(fll), std::begin(list), std::end(list)));
      REQUIRE(std::equal(
        std::make_reverse_iterator(std::end(fll)), std::make_reverse_iterator(std::begin(fll)), std::rbegin(list), std::rend(list)));
      REQUIRE(fll.front() == list.front());
      REQUIRE(fll.back() == list.back());
      REQUIRE(!fll.empty());
    }
    CAPTURE(list, fll);
    REQUIRE(fll.size() == fll.capacity());
  }
  // - test emplace in sorted one to keep it sorted
  SECTION("random fill with emplace() to keep it sorted") {
    std::list<PODTest> list;
    std::uniform_int_distribution<int> dist(0, 99);

    auto emplaceSorted = [](auto& l, int e) {
      auto it = l.emplace(std::find_if(std::begin(l), std::end(l), [e](PODTest const& pod) {
        return pod.i > e;
      }));
      it->i = e;
    };

    for (std::size_t i = 0; i < fll.capacity(); ++i) {
      REQUIRE(fll.capacity() == FLL_CAPACITY);
      REQUIRE(fll.size() == i);

      int elem = dist(gen);
      emplaceSorted(fll, elem);
      emplaceSorted(list, elem);

      CAPTURE(fll, list);
      REQUIRE(std::equal(std::begin(fll), std::end(fll), std::begin(list), std::end(list)));
      REQUIRE(std::equal(
        std::make_reverse_iterator(std::end(fll)), std::make_reverse_iterator(std::begin(fll)), std::rbegin(list), std::rend(list)));
      REQUIRE(fll.front() == list.front());
      REQUIRE(fll.back() == list.back());
      REQUIRE(!fll.empty());
    }

    CAPTURE(list, fll);
    REQUIRE(fll.size() == fll.capacity());
    REQUIRE(std::is_sorted(std::begin(fll), std::end(fll)));
  }
  // - test random emplace/remove and compare with std::list
}