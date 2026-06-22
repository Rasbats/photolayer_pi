#if (defined(OCPN_GHC_FILESYSTEM) ||                           \
     (defined(__clang_major__) && (__clang_major__ < 15)) ||    \
     (defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
      __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ < 101500))
// MacOS 1.13, or deployment target below 10.15 where std::filesystem
// is unavailable regardless of the clang version used.
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#else
#include <filesystem>
#include <utility>
namespace fs = std::filesystem;
#endif
