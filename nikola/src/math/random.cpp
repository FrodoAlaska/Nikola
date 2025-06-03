#include "nikola/nikola_base.h"
#include "nikola/nikola_math.h"

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

/// ----------------------------------------------------------------------
/// *** Math random ***

/// ----------------------------------------------------------------------
/// Globals
static std::random_device rand_device;
static std::default_random_engine engine(rand_device());
/// Globals
/// ----------------------------------------------------------------------

/// ----------------------------------------------------------------------
/// Math random functions

const f32 random_f32() {
  std::uniform_real_distribution<f32> uni_dist;
  return uni_dist(engine);
}

const f32 random_f32(const f32 min, const f32 max) {
  std::uniform_real_distribution<f32> uni_dist(min, max);
  return uni_dist(engine);
}

const f64 random_f64() {
  std::uniform_real_distribution<f64> uni_dist;
  return uni_dist(engine);
}

const f64 random_f64(const f64 min, const f64 max) {
  std::uniform_real_distribution<f64> uni_dist(min, max);
  return uni_dist(engine);
}

const i32 random_i32() {
  std::uniform_int_distribution<i32> uni_dist;
  return uni_dist(engine);
}

const i32 random_i32(const i32 min, const i32 max) {
  std::uniform_int_distribution<i32> uni_dist(min, max);
  return uni_dist(engine);
}

const i64 random_i64() {
  std::uniform_int_distribution<i64> uni_dist;
  return uni_dist(engine);
}

const i64 random_i64(const i64 min, const i64 max) {
  std::uniform_int_distribution<i64> uni_dist(min, max);
  return uni_dist(engine);
}

const u32 random_u32() { 
  std::uniform_int_distribution<u32> uni_dist;
  return uni_dist(engine);
}

const u32 random_u32(const u32 min, const u32 max) {
  std::uniform_int_distribution<u32> uni_dist(min, max);
  return uni_dist(engine);
}

const u64 random_u64() {
  std::uniform_int_distribution<u64> uni_dist;
  return uni_dist(engine);
}

const u64 random_u64(const u64 min, const u64 max) {
  std::uniform_int_distribution<u64> uni_dist(min, max);
  return uni_dist(engine);
}

/// Math random functions
/// ----------------------------------------------------------------------

/// *** Math random ***
/// ----------------------------------------------------------------------

} // End of nikola

//////////////////////////////////////////////////////////////////////////
