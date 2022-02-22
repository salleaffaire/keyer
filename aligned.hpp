#ifndef _ALIGNED_HPP___
#define _ALIGNED_HPP___

#include <cstring>
#include <utility>

template <typename T>
class Aligned {
 private:
  size_t mSize;
  T *mData;
  T *mAlignedData;

 public:
  Aligned() : mSize(0), mData(0) {}

  Aligned(size_t size) : mSize(size) {
    mData = new T[size + 32];
    mAlignedData = (T *)(((uintptr_t)mData + 31) & ~(uintptr_t)0x1F);
  }

  virtual ~Aligned() {
    if (mData) {
      delete[] mData;
    }
  }

  Aligned(const Aligned<T> &that) : mSize(that.mSize) {
    mData = new T[that.mSize + 32];
    mAlignedData = (T *)(((uintptr_t)mData + 31) & ~(uintptr_t)0x1F);
    std::memcpy(mData, that.mData, mSize);
  }

  Aligned(Aligned<T> &&that) : mSize(that.mSize) {
    mData = that.mData;
    mAlignedData = that.mAlignedData;
    that.mData = 0;
  }

  Aligned<T> &operator=(Aligned<T> that) {
    std::swap(mData, that.mData);
    std::swap(mAlignedData, that.mAlignedData);
    return *this;
  }

  // Accessor
  T *geta() { return mAlignedData; }
};

#endif