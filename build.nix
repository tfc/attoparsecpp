{ stdenv
, catch2
, ninja
, gbenchmark
, gcovr
, cmake
, lib
, clang-tools
, doCheck ? true
}:

stdenv.mkDerivation {
  name = "attoparsecpp";

  src = lib.sourceByRegex ./. [
    "^include.*"
    "^test.*"
    "^benchmark.*"
    ".*CMakeLists\.txt$"
    "^pkg-config\.pc\.cmake$"
  ];

  checkInputs = [
    gbenchmark
    catch2
    gcovr
  ];
  inherit doCheck;

  enableParallelBuilding = true;

  nativeBuildInputs = [
    clang-tools
    cmake
    ninja
  ];
}
