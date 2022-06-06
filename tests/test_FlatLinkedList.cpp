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

constexpr std::uint8_t FLL_CAPACITY = 10;
namespace Pinetime {
  // template <typename T, std::uint8_t N> //
  std::ostream& operator<<(std::ostream& out, typename Pinetime::FlatLinkedList<PODTest, FLL_CAPACITY>::const_iterator it) {
    return out << "&(" << *it << ")";
  }

  // template <typename T, std::uint8_t N> //
  std::ostream& operator<<(std::ostream& out, typename Pinetime::FlatLinkedList<PODTest, FLL_CAPACITY>::iterator it) {
    return out << (typename Pinetime::FlatLinkedList<PODTest, FLL_CAPACITY>::const_iterator {it});
  }
}

std::mt19937 gen(std::random_device {}());

int uniform_rand(int min, int max) {
  std::uniform_int_distribution<int> dist(min, max);
  return dist(gen);
}

template <typename T, std::uint8_t N, typename Rng> //
void require_equal(Pinetime::FlatLinkedList<T, N> const& fll, Rng const& rng) {
  REQUIRE(fll.size() == rng.size());
  REQUIRE(std::equal(std::begin(fll), std::end(fll), std::begin(rng), std::end(rng)));
  REQUIRE(
    std::equal(std::make_reverse_iterator(std::end(fll)), std::make_reverse_iterator(std::begin(fll)), std::rbegin(rng), std::rend(rng)));
  REQUIRE(fll.empty() == rng.empty());
  if (!fll.empty()) {
    REQUIRE(fll.front() == rng.front());
    REQUIRE(fll.back() == rng.back());
  }
}

template <std::uint8_t N> //
void test_erase(Pinetime::FlatLinkedList<PODTest, N>& fll);

template <bool do_test_erase, std::uint8_t N> //
void test_random_fill_sorted(Pinetime::FlatLinkedList<PODTest, N>& fll) {
  SECTION("random fill with emplace() to keep it sorted") {
    std::list<PODTest> list;
    std::uniform_int_distribution<int> dist(0, 99);

    auto emplaceSorted = [](auto& l, int e) {
      auto it = l.emplace(std::find_if(std::begin(l), std::end(l), [e](PODTest const& pod) {
        return pod.i > e;
      }));
      it->i = e;
    };

    for (int i = 0; i < fll.capacity(); ++i) {
      REQUIRE(fll.capacity() == FLL_CAPACITY);
      REQUIRE(fll.size() == i);

      int elem = dist(gen);
      emplaceSorted(fll, elem);
      emplaceSorted(list, elem);

      CAPTURE(fll, list);
      require_equal(fll, list);
      REQUIRE(!fll.empty());
      REQUIRE(std::is_sorted(std::begin(fll), std::end(fll)));
    }

    CAPTURE(list, fll);
    REQUIRE(fll.size() == fll.capacity());
    if /* constexpr */ (do_test_erase) {
      test_erase(fll);
    }
  }
}

template <std::uint8_t N> //
void test_erase(Pinetime::FlatLinkedList<PODTest, N>& fll) {
  std::list<PODTest> list(std::begin(fll), std::end(fll));
  require_equal(fll, list);

  SECTION("erase from the end") {
    REQUIRE(fll.size() == N);
    for (int i = fll.size() - 1; i >= 0; --i) {
      REQUIRE(!fll.empty());
      auto it = fll.erase(std::prev(fll.end()));
      // list.erase(std::prev(list.end()));
      list.pop_back();
      CAPTURE(list, fll);
      REQUIRE(it == fll.end());
      require_equal(fll, list);
    }
    REQUIRE(fll.empty());
    test_random_fill_sorted<false>(fll);
  }

  SECTION("erase from the begining") {
    REQUIRE(fll.size() == N);
    for (int i = fll.size() - 1; i >= 0; --i) {
      REQUIRE(!fll.empty());
      auto it = fll.erase(fll.begin());
      // list.erase(list.begin());
      list.pop_front();
      CAPTURE(list, fll);
      REQUIRE(it == fll.begin());
      require_equal(fll, list);
    }
    REQUIRE(fll.empty());
    test_random_fill_sorted<false>(fll);
  }

  // - test random emplace/remove and compare with std::list
  SECTION("erase random positions") {
    REQUIRE(fll.size() == N);
    for (int i = fll.size() - 1; i >= 0; --i) {
      REQUIRE(!fll.empty());
      auto pos_to_erase = uniform_rand(0, list.size() - 1);
      auto it_to_erase = std::next(fll.begin(), pos_to_erase);
      auto it_after_erased = std::next(it_to_erase);
      auto it = fll.erase(it_to_erase);
      list.erase(std::next(list.begin(), pos_to_erase));
      CAPTURE(list, fll);
      REQUIRE(it == it_after_erased);
      require_equal(fll, list);
    }
    REQUIRE(fll.empty());
    test_random_fill_sorted<false>(fll);
  }
}

TEST_CASE("FlatLinkedList", "[FlatLinkedList]") {
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
    for (int i = 0; i < fll.capacity(); ++i) {
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
      require_equal(fll, vec);
      REQUIRE(!fll.empty());
    }

    CAPTURE(vec, fll);
    REQUIRE(fll.size() == fll.capacity());

    test_erase(fll);
  }
  SECTION("fill with emplace(begin())") {
    std::list<PODTest> list;

    for (int i = 0; i < fll.capacity(); ++i) {
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

      require_equal(fll, list);
      REQUIRE(!fll.empty());
    }
    CAPTURE(list, fll);
    REQUIRE(fll.size() == fll.capacity());

    test_erase(fll);
  }
  // - test emplace in sorted one to keep it sorted
  test_random_fill_sorted<true>(fll);
}
