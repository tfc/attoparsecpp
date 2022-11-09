{ stdenv
, catch2
, gbenchmark
, cmake
, lib
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
  ];
  inherit doCheck;
  postCheck = lib.optionalString doBenchmark ''
    ./benchmark/attoparsecpp-benchmark
  '';

  enableParallelBuilding = true;

  nativeBuildInputs = [ cmake ];
}
