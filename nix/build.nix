{ stdenv
, catch2
, googlebench
, cmake
, lib
}:

stdenv.mkDerivation {
  name = "attoparsecpp";

  src = lib.sourceByRegex ./.. [
    "^include$"
    "^include/attoparsecpp$"
    "^test$"
    "^benchmark$"
    ".*CMakeLists\.txt$"
    ".*\.hpp$"
    ".*\.cpp$"
    "^pkg-config\.pc\.cmake$"
  ];

  checkInputs = [ catch2 googlebench ];
  doCheck = true;
  postCheck = ''
    ./benchmark/attoparsecpp-benchmark
  '';

  enableParallelBuilding = true;

  nativeBuildInputs = [ cmake ];
}
