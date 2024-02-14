// Copyright (c) 2022 Emilian Cioca
#pragma once
#if defined(LOUPE_ASSERTS) && (!defined(NDEBUG) || defined(_DEBUG))

#include <cassert>
#define LOUPE_ASSERT(exp, msg) do { assert((exp) && msg); } while(false)
#define LOUPE_ASSERTS_ENABLED

#else

#define LOUPE_ASSERT(exp, msg) ((void)0)

#endif
