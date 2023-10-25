{ stdenv
, catch2
, ninja
, gbenchmark
, gcovr
, cmake
, lib
, clang-tools
, doCheck ? true
, doBenchmark ? true
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
  postCheck = lib.optionalString doBenchmark ''
    ./benchmark/attoparsecpp-benchmark
  '';

  enableParallelBuilding = true;

  nativeBuildInputs = [
    clang-tools
    cmake
    ninja
  ];
}
