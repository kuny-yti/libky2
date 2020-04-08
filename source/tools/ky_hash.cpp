#include "tools/ky_hash.h"

const _ky_hash_ _ky_hash_::shared_nul = {};

f32 _ky_hash_::sMaxUsageRate = .9f;        ///< 桶的最大使用率
f32 _ky_hash_::sGrowRate     = 1.7312543f; ///< 桶的增长率
