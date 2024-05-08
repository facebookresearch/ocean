/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFrameInterpolatorBilinear.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

namespace {

using namespace cv;

constexpr int INTER_RESIZE_COEF_BITS = 11;
constexpr int INTER_RESIZE_COEF_SCALE = 1 << INTER_RESIZE_COEF_BITS;
constexpr int MAX_ESIZE = 16;

template <typename ST, typename DT>
struct Cast {
  using type1 = ST;
  using rtype = DT;

  DT operator()(ST S0, ST b0, ST S1, ST b1) const {
    return DT(S0 * b0 + S1 * b1);
  }
};

template <typename ST, typename DT, int bits>
struct FixedPtCast {
  using type1 = ST;
  using rtype = DT;

  DT operator()(ST S0, ST b0, ST S1, ST b1) const {
    return DT((((b0 * (S0 >> 4)) >> 16) + ((b1 * (S1 >> 4)) >> 16) + 2) >> 2);
  }
};

struct VResizeNoVec {
  int operator()(const uchar**, uchar*, const uchar*, int) const {
    return 0;
  }
};

#if defined(__ARM_NEON__) || defined(__ARM_NEON)

/* For unsigned char type
 * (((b0 * (S0 >> 4)) >> 16) + ((b1 * (S1 >> 4)) >> 16) + 2) >> 2
 */
struct VResizeLinearVec_8u {
  int operator()(const uchar** _src, uchar* dst, const uchar* _beta, int width)
      const {
    const int **src = (const int**)_src, *S0 = src[0], *S1 = src[1];
    const short* beta = (const short*)_beta;
    int x = 0;
    int16x8_t v_b0 = vdupq_n_s16(beta[0]), v_b1 = vdupq_n_s16(beta[1]),
              v_delta = vdupq_n_s16(2);

    for (; x <= width - 16; x += 16) {
      /*
       * Load 4 + 4 integers from two adjacent rows and right shift each
       * by 4. (S0 >> 4) and (S1 >> 4)
       */
      int32x4_t v_src00 = vshrq_n_s32(vld1q_s32(S0 + x), 4),
                v_src10 = vshrq_n_s32(vld1q_s32(S1 + x), 4);
      int32x4_t v_src01 = vshrq_n_s32(vld1q_s32(S0 + x + 4), 4),
                v_src11 = vshrq_n_s32(vld1q_s32(S1 + x + 4), 4);

      /*
       * 32 bit integers are converted to 16 bit and combined to 128 bit
       * vectors.
       */
      int16x8_t v_src0 = vcombine_s16(vmovn_s32(v_src00), vmovn_s32(v_src01));
      int16x8_t v_src1 = vcombine_s16(vmovn_s32(v_src10), vmovn_s32(v_src11));

      /* Compute
       * (((b0 * (S0 >> 4)) >> 16) + ((b1 * (S1 >> 4)) >> 16) + 2) >> 2
       */
      int16x8_t v_dst0 = vaddq_s16(
          vshrq_n_s16(vqdmulhq_s16(v_src0, v_b0), 1),
          vshrq_n_s16(vqdmulhq_s16(v_src1, v_b1), 1));
      v_dst0 = vshrq_n_s16(vaddq_s16(v_dst0, v_delta), 2);

      /*
       * Load the next 8 elements for both the rows and perform the
       * interpolation in the y direction same as above
       */
      v_src00 = vshrq_n_s32(vld1q_s32(S0 + x + 8), 4);
      v_src10 = vshrq_n_s32(vld1q_s32(S1 + x + 8), 4);
      v_src01 = vshrq_n_s32(vld1q_s32(S0 + x + 12), 4);
      v_src11 = vshrq_n_s32(vld1q_s32(S1 + x + 12), 4);

      v_src0 = vcombine_s16(vmovn_s32(v_src00), vmovn_s32(v_src01));
      v_src1 = vcombine_s16(vmovn_s32(v_src10), vmovn_s32(v_src11));

      int16x8_t v_dst1 = vaddq_s16(
          vshrq_n_s16(vqdmulhq_s16(v_src0, v_b0), 1),
          vshrq_n_s16(vqdmulhq_s16(v_src1, v_b1), 1));
      v_dst1 = vshrq_n_s16(vaddq_s16(v_dst1, v_delta), 2);

      /*
       * Store the result of the operation on the total 16 elements at dst + x
       * by first saturating the integers from signed to unsigned and then
       * joining two uint8x8_t into a single 128 bit vector.
       */
      vst1q_u8(dst + x, vcombine_u8(vqmovun_s16(v_dst0), vqmovun_s16(v_dst1)));
    }

    return x;
  }
};

/* For floating point S0 * b0 + S1 * b1 */
struct VResizeLinearVec_32f {
  int operator()(const uchar** _src, uchar* _dst, const uchar* _beta, int width)
      const {
    const float** src = (const float**)_src;
    const float* beta = (const float*)_beta;
    const float *S0 = src[0], *S1 = src[1];
    float* dst = (float*)_dst;
    int x = 0;

    float32x4_t v_b0 = vdupq_n_f32(beta[0]), v_b1 = vdupq_n_f32(beta[1]);

    for (; x <= width - 8; x += 8) {
      /* Load consecutive rows in v_src00 and v_src10 (also v_src01 and v_src11)
       */
      float32x4_t v_src00 = vld1q_f32(S0 + x), v_src01 = vld1q_f32(S0 + x + 4);
      float32x4_t v_src10 = vld1q_f32(S1 + x), v_src11 = vld1q_f32(S1 + x + 4);

      /* Compute
       * v_src00 * v_b0 + v_src10 * v_b1 and store it in dst + x
       */
      vst1q_f32(dst + x, vmlaq_f32(vmulq_f32(v_src00, v_b0), v_src10, v_b1));
      vst1q_f32(
          dst + x + 4, vmlaq_f32(vmulq_f32(v_src01, v_b0), v_src11, v_b1));
    }

    return x;
  }
};

#elif defined(__SSSE3__)

/*
 * For unsigned char type
 * (((b0 * (S0 >> 4)) >> 16) + ((b1 * (S1 >> 4)) >> 16) + 2) >> 2
 */
struct VResizeLinearVec_8u {
  int operator()(const uchar** _src, uchar* dst, const uchar* _beta, int width)
      const {
    const int** src = (const int**)_src;
    const short* beta = (const short*)_beta;
    const int *S0 = src[0], *S1 = src[1];
    int x = 0;
    __m128i b0 = _mm_set1_epi16(beta[0]), b1 = _mm_set1_epi16(beta[1]);
    __m128i delta = _mm_set1_epi16(2);

    /*
     * Similar logic as ARM NEON implementation. If the data is aligned use
     * _mm_load_si128 otherwise use unaligned load.
     */
    if ((((size_t)S0 | (size_t)S1) & 15) == 0) {
      for (; x <= width - 16; x += 16) {
        __m128i x0, x1, x2, y0, y1, y2;
        x0 = _mm_load_si128((const __m128i*)(S0 + x));
        x1 = _mm_load_si128((const __m128i*)(S0 + x + 4));
        y0 = _mm_load_si128((const __m128i*)(S1 + x));
        y1 = _mm_load_si128((const __m128i*)(S1 + x + 4));
        x0 = _mm_packs_epi32(_mm_srai_epi32(x0, 4), _mm_srai_epi32(x1, 4));
        y0 = _mm_packs_epi32(_mm_srai_epi32(y0, 4), _mm_srai_epi32(y1, 4));

        x1 = _mm_load_si128((const __m128i*)(S0 + x + 8));
        x2 = _mm_load_si128((const __m128i*)(S0 + x + 12));
        y1 = _mm_load_si128((const __m128i*)(S1 + x + 8));
        y2 = _mm_load_si128((const __m128i*)(S1 + x + 12));
        x1 = _mm_packs_epi32(_mm_srai_epi32(x1, 4), _mm_srai_epi32(x2, 4));
        y1 = _mm_packs_epi32(_mm_srai_epi32(y1, 4), _mm_srai_epi32(y2, 4));

        x0 = _mm_adds_epi16(_mm_mulhi_epi16(x0, b0), _mm_mulhi_epi16(y0, b1));
        x1 = _mm_adds_epi16(_mm_mulhi_epi16(x1, b0), _mm_mulhi_epi16(y1, b1));

        x0 = _mm_srai_epi16(_mm_adds_epi16(x0, delta), 2);
        x1 = _mm_srai_epi16(_mm_adds_epi16(x1, delta), 2);
        _mm_storeu_si128((__m128i*)(dst + x), _mm_packus_epi16(x0, x1));
      }
    } else {
      for (; x <= width - 16; x += 16) {
        __m128i x0, x1, x2, y0, y1, y2;
        x0 = _mm_loadu_si128((const __m128i*)(S0 + x));
        x1 = _mm_loadu_si128((const __m128i*)(S0 + x + 4));
        y0 = _mm_loadu_si128((const __m128i*)(S1 + x));
        y1 = _mm_loadu_si128((const __m128i*)(S1 + x + 4));
        x0 = _mm_packs_epi32(_mm_srai_epi32(x0, 4), _mm_srai_epi32(x1, 4));
        y0 = _mm_packs_epi32(_mm_srai_epi32(y0, 4), _mm_srai_epi32(y1, 4));

        x1 = _mm_loadu_si128((const __m128i*)(S0 + x + 8));
        x2 = _mm_loadu_si128((const __m128i*)(S0 + x + 12));
        y1 = _mm_loadu_si128((const __m128i*)(S1 + x + 8));
        y2 = _mm_loadu_si128((const __m128i*)(S1 + x + 12));
        x1 = _mm_packs_epi32(_mm_srai_epi32(x1, 4), _mm_srai_epi32(x2, 4));
        y1 = _mm_packs_epi32(_mm_srai_epi32(y1, 4), _mm_srai_epi32(y2, 4));

        x0 = _mm_adds_epi16(_mm_mulhi_epi16(x0, b0), _mm_mulhi_epi16(y0, b1));
        x1 = _mm_adds_epi16(_mm_mulhi_epi16(x1, b0), _mm_mulhi_epi16(y1, b1));

        x0 = _mm_srai_epi16(_mm_adds_epi16(x0, delta), 2);
        x1 = _mm_srai_epi16(_mm_adds_epi16(x1, delta), 2);
        _mm_storeu_si128((__m128i*)(dst + x), _mm_packus_epi16(x0, x1));
      }
    }

    for (; x < width - 4; x += 4) {
      __m128i x0, y0;
      x0 = _mm_srai_epi32(_mm_loadu_si128((const __m128i*)(S0 + x)), 4);
      y0 = _mm_srai_epi32(_mm_loadu_si128((const __m128i*)(S1 + x)), 4);
      x0 = _mm_packs_epi32(x0, x0);
      y0 = _mm_packs_epi32(y0, y0);
      x0 = _mm_adds_epi16(_mm_mulhi_epi16(x0, b0), _mm_mulhi_epi16(y0, b1));
      x0 = _mm_srai_epi16(_mm_adds_epi16(x0, delta), 2);
      x0 = _mm_packus_epi16(x0, x0);
      *(int*)(dst + x) = _mm_cvtsi128_si32(x0);
    }

    return x;
  }
};

/* For floating point S0 * b0 + S1 * b1 */
struct VResizeLinearVec_32f {
  int operator()(const uchar** _src, uchar* _dst, const uchar* _beta, int width)
      const {
    const float** src = (const float**)_src;
    const float* beta = (const float*)_beta;
    const float *S0 = src[0], *S1 = src[1];
    float* dst = (float*)_dst;
    int x = 0;

    __m128 b0 = _mm_set1_ps(beta[0]), b1 = _mm_set1_ps(beta[1]);

    if ((((size_t)S0 | (size_t)S1) & 15) == 0) {
      for (; x <= width - 8; x += 8) {
        __m128 x0, x1, y0, y1;
        x0 = _mm_load_ps(S0 + x);
        x1 = _mm_load_ps(S0 + x + 4);
        y0 = _mm_load_ps(S1 + x);
        y1 = _mm_load_ps(S1 + x + 4);

        x0 = _mm_add_ps(_mm_mul_ps(x0, b0), _mm_mul_ps(y0, b1));
        x1 = _mm_add_ps(_mm_mul_ps(x1, b0), _mm_mul_ps(y1, b1));

        _mm_storeu_ps(dst + x, x0);
        _mm_storeu_ps(dst + x + 4, x1);
      }
    } else {
      for (; x <= width - 8; x += 8) {
        __m128 x0, x1, y0, y1;
        x0 = _mm_loadu_ps(S0 + x);
        x1 = _mm_loadu_ps(S0 + x + 4);
        y0 = _mm_loadu_ps(S1 + x);
        y1 = _mm_loadu_ps(S1 + x + 4);

        x0 = _mm_add_ps(_mm_mul_ps(x0, b0), _mm_mul_ps(y0, b1));
        x1 = _mm_add_ps(_mm_mul_ps(x1, b0), _mm_mul_ps(y1, b1));

        _mm_storeu_ps(dst + x, x0);
        _mm_storeu_ps(dst + x + 4, x1);
      }
    }

    return x;
  }
};

#else
using VResizeLinearVec_8u = VResizeNoVec;
using VResizeLinearVec_32f = VResizeNoVec;
#endif

template <typename T, typename WT, typename AT, int ONE>
struct HResizeLinear {
  using value_type = T;
  using buf_type = WT;
  using alpha_type = AT;

  void operator()(
      const T** src,
      WT** dst,
      int count,
      const int* xofs,
      const AT* alpha,
      int dwidth,
      int cn,
      int xmax) const {
    int dx, k;

    for (k = 0; k <= count - 2; k++) {
      const T *S0 = src[k], *S1 = src[k + 1];
      WT *D0 = dst[k], *D1 = dst[k + 1];
      for (dx = 0; dx < xmax; dx++) {
        int sx = xofs[dx];
        WT a0 = alpha[dx * 2], a1 = alpha[dx * 2 + 1];
        WT t0 = S0[sx] * a0 + S0[sx + cn] * a1;
        WT t1 = S1[sx] * a0 + S1[sx + cn] * a1;
        D0[dx] = t0;
        D1[dx] = t1;
      }

      for (; dx < dwidth; dx++) {
        int sx = xofs[dx];
        D0[dx] = WT(S0[sx] * ONE);
        D1[dx] = WT(S1[sx] * ONE);
      }
    }

    for (; k < count; k++) {
      const T* S = src[k];
      WT* D = dst[k];
      for (dx = 0; dx < xmax; dx++) {
        int sx = xofs[dx];
        D[dx] = S[sx] * alpha[dx * 2] + S[sx + cn] * alpha[dx * 2 + 1];
      }

      for (; dx < dwidth; dx++) {
        D[dx] = WT(S[xofs[dx]] * ONE);
      }
    }
  }
};

template <typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeLinear {
  using value_type = T;
  using buf_type = WT;
  using alpha_type = AT;

  void operator()(const WT** src, T* dst, const AT* beta, int width) const {
    WT b0 = beta[0], b1 = beta[1];
    const WT *S0 = src[0], *S1 = src[1];
    CastOp castOp;
    VecOp vecOp;

    int x = vecOp((const uchar**)src, (uchar*)dst, (const uchar*)beta, width);
    for (; x < width; x++) {
      dst[x] = castOp(S0[x], b0, S1[x], b1);
    }
  }
};

template <typename HResize, typename VResize>
class resizeGeneric_Invoker : public ParallelLoopBody {
 public:
  using T = typename HResize::value_type;
  using WT = typename HResize::buf_type;
  using AT = typename HResize::alpha_type;

  resizeGeneric_Invoker(
      const Mat& _src,
      Mat& _dst,
      const int* _xofs,
      const int* _yofs,
      const AT* _alpha,
      const AT* __beta,
      const Size& _ssize,
      const Size& _dsize,
      int _ksize,
      int _xmin,
      int _xmax)
      : ParallelLoopBody(),
        src(_src),
        dst(_dst),
        xofs(_xofs),
        yofs(_yofs),
        alpha(_alpha),
        _beta(__beta),
        ssize(_ssize),
        dsize(_dsize),
        ksize(_ksize),
        xmin(_xmin),
        xmax(_xmax) {
    CV_Assert(ksize <= MAX_ESIZE);
  }

  void operator()(const Range& range) const override {
    int dy, cn = src.channels();
    HResize hresize;
    VResize vresize;

    int bufstep = (int)alignSize(dsize.width, 16);
    AutoBuffer<WT> _buffer(bufstep * ksize);
    const T* srows[MAX_ESIZE] = {0};
    WT* rows[MAX_ESIZE] = {0};
    int prev_sy[MAX_ESIZE];

    for (int k = 0; k < ksize; k++) {
      prev_sy[k] = -1;
      rows[k] = (WT*)_buffer + bufstep * k;
    }

    const AT* beta = _beta + ksize * range.start;

    for (dy = range.start; dy < range.end; dy++, beta += ksize) {
      int sy0 = yofs[dy], k0 = ksize, k1 = 0, ksize2 = ksize / 2;

      for (int k = 0; k < ksize; k++) {
        int s = sy0 - ksize2 + 1 + k;
        int sy = s >= 0 ? (s < ssize.height ? s : ssize.height - 1) : 0;
        for (k1 = std::max(k1, k); k1 < ksize; k1++) {
          if (k1 < MAX_ESIZE && sy == prev_sy[k1]) {
            if (k1 > k) {
              memcpy(rows[k], rows[k1], bufstep * sizeof(rows[0][0]));
            }
            break;
          }
        }
        if (k1 == ksize) {
          k0 = std::min(k0, k);
        }
        srows[k] = src.template ptr<T>(sy);
        prev_sy[k] = sy;
      }

      if (k0 < ksize) {
        hresize(
            (const T**)(srows + k0),
            (WT**)(rows + k0),
            ksize - k0,
            xofs,
            (const AT*)(alpha),
            dsize.width,
            cn,
            xmax);
      }
      vresize(
          (const WT**)rows, (T*)(dst.data + dst.step * dy), beta, dsize.width);
    }
  }

 private:
  Mat src;
  Mat dst;
  const int *xofs, *yofs;
  const AT *alpha, *_beta;
  Size ssize, dsize;
  const int ksize, xmin, xmax;

  resizeGeneric_Invoker& operator=(const resizeGeneric_Invoker&);
};

template <class HResize, class VResize>
static void resizeGeneric_(
    const Mat& src,
    Mat& dst,
    const int* xofs,
    const void* _alpha,
    const int* yofs,
    const void* _beta,
    int xmin,
    int xmax,
    int ksize) {
  using AT = typename HResize::alpha_type;

  const AT* beta = (const AT*)_beta;
  Size ssize = src.size(), dsize = dst.size();
  int cn = src.channels();
  ssize.width *= cn;
  dsize.width *= cn;
  xmin *= cn;
  xmax *= cn;

  Range range(0, dsize.height);
  resizeGeneric_Invoker<HResize, VResize> invoker(
      src,
      dst,
      xofs,
      yofs,
      (const AT*)_alpha,
      beta,
      ssize,
      dsize,
      ksize,
      xmin,
      xmax);
  parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

typedef void (*ResizeFunc)(
    const Mat& src,
    Mat& dst,
    const int* xofs,
    const void* alpha,
    const int* yofs,
    const void* beta,
    int xmin,
    int xmax,
    int ksize);

void resize_(
    int src_type,
    const uchar* src_data,
    size_t src_step,
    int src_width,
    int src_height,
    uchar* dst_data,
    size_t dst_step,
    int dst_width,
    int dst_height,
    double inv_scale_x,
    double inv_scale_y,
    int interpolation) {
  CV_Assert(
      (dst_width * dst_height > 0) || (inv_scale_x > 0 && inv_scale_y > 0));
  if (inv_scale_x < DBL_EPSILON || inv_scale_y < DBL_EPSILON) {
    inv_scale_x = static_cast<double>(dst_width) / src_width;
    inv_scale_y = static_cast<double>(dst_height) / src_height;
  }

  int depth = CV_MAT_DEPTH(src_type), cn = CV_MAT_CN(src_type);
  Size dsize = Size(
      saturate_cast<int>(src_width * inv_scale_x),
      saturate_cast<int>(src_height * inv_scale_y));
  CV_Assert(dsize.area() > 0);

  static ResizeFunc linear_tab[] = {
      resizeGeneric_<
          HResizeLinear<uchar, int, short, INTER_RESIZE_COEF_SCALE>,
          VResizeLinear<
              uchar,
              int,
              short,
              FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS * 2>,
              VResizeLinearVec_8u>>,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      resizeGeneric_<
          HResizeLinear<float, float, float, 1>,
          VResizeLinear<
              float,
              float,
              float,
              Cast<float, float>,
              VResizeLinearVec_32f>>};

  double scale_x = 1. / inv_scale_x, scale_y = 1. / inv_scale_y;

  Mat src(
      Size(src_width, src_height),
      src_type,
      const_cast<uchar*>(src_data),
      src_step);
  Mat dst(dsize, src_type, dst_data, dst_step);

  int k, sx, sy, dx, dy;

  int xmin = 0, xmax = dsize.width, width = dsize.width * cn;
  bool fixpt = depth == CV_8U;
  float fx, fy;
  ResizeFunc func = nullptr;
  int ksize = 0, ksize2;
  if (interpolation == 1) {
    ksize = 2, func = linear_tab[depth];
  } else {
    CV_Error(cv::Error::StsBadArg, "Unknown interpolation method");
  }
  ksize2 = ksize / 2;

  CV_Assert(func != 0);

  AutoBuffer<uchar> _buffer(
      (width + dsize.height) * (sizeof(int) + sizeof(float) * ksize));
  int* xofs = (int*)(uchar*)_buffer;
  int* yofs = xofs + width;
  float* alpha = (float*)(yofs + dsize.height);
  short* ialpha = (short*)alpha;
  float* beta = alpha + width * ksize;
  short* ibeta = ialpha + width * ksize;
  float cbuf[MAX_ESIZE];

  for (dx = 0; dx < dsize.width; dx++) {
    fx = (float)((dx + 0.5) * scale_x - 0.5);
    sx = cvFloor(fx);
    fx -= sx;

    if (sx < ksize2 - 1) {
      xmin = dx + 1;
      if (sx < 0) {
        fx = 0, sx = 0;
      }
    }

    if (sx + ksize2 >= src_width) {
      xmax = std::min(xmax, dx);
      if (sx >= src_width - 1) {
        fx = 0, sx = src_width - 1;
      }
    }

    for (k = 0, sx *= cn; k < cn; k++) {
      xofs[dx * cn + k] = sx + k;
    }

    cbuf[0] = 1.f - fx;
    cbuf[1] = fx;
    if (fixpt) {
      for (k = 0; k < ksize; k++) {
        ialpha[dx * cn * ksize + k] =
            saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
      }
      for (; k < cn * ksize; k++) {
        ialpha[dx * cn * ksize + k] = ialpha[dx * cn * ksize + k - ksize];
      }
    } else {
      for (k = 0; k < ksize; k++) {
        alpha[dx * cn * ksize + k] = cbuf[k];
      }
      for (; k < cn * ksize; k++) {
        alpha[dx * cn * ksize + k] = alpha[dx * cn * ksize + k - ksize];
      }
    }
  }

  for (dy = 0; dy < dsize.height; dy++) {
    fy = (float)((dy + 0.5) * scale_y - 0.5);
    sy = cvFloor(fy);
    fy -= sy;

    yofs[dy] = sy;
    cbuf[0] = 1.f - fy;
    cbuf[1] = fy;

    if (fixpt) {
      for (k = 0; k < ksize; k++) {
        ibeta[dy * ksize + k] =
            saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
      }
    } else {
      for (k = 0; k < ksize; k++) {
        beta[dy * ksize + k] = cbuf[k];
      }
    }
  }

  func(
      src,
      dst,
      xofs,
      fixpt ? (void*)ialpha : (void*)alpha,
      yofs,
      fixpt ? (void*)ibeta : (void*)beta,
      xmin,
      xmax,
      ksize);
}

} // namespace

void TestFrameInterpolatorBilinear::amlFacetrackerResize(const cv::Mat& _src, cv::Mat& _dst, cv::Size dsize, double inv_scale_x, double inv_scale_y, int interpolation)
{
  Size ssize = _src.size();

  CV_Assert(ssize.width > 0 && ssize.height > 0);
  CV_Assert(dsize.area() > 0 || (inv_scale_x > 0 && inv_scale_y > 0));
  if (dsize.area() == 0) {
    dsize = Size(
        saturate_cast<int>(ssize.width * inv_scale_x),
        saturate_cast<int>(ssize.height * inv_scale_y));
    CV_Assert(dsize.area() > 0);
  } else {
    inv_scale_x = (double)dsize.width / ssize.width;
    inv_scale_y = (double)dsize.height / ssize.height;
  }

  Mat src = _src;
  _dst.create(dsize, src.type());
  Mat dst = _dst;

  if (dsize == ssize) {
    // Source and destination are of same size. Return the source image.
    src.copyTo(dst);
    return;
  }

  resize_(
      src.type(),
      src.data,
      src.step,
      src.cols,
      src.rows,
      dst.data,
      dst.step,
      dst.cols,
      dst.rows,
      inv_scale_x,
      inv_scale_y,
      interpolation);
}

} // namespace TestOpenCV

} // mamespace TestCV

} // namespace Test

} // namespace Ocean
