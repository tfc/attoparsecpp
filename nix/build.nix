{ stdenv
, catch2
, googlebench
, cmake
}:

stdenv.mkDerivation {
  name = "attoparsecpp";

  src = ./..;

  checkInputs = [ catch2 googlebench ];
  doCheck = true;
  postCheck = ''
    ./benchmark/attoparsecpp-benchmark
  '';

  enableParallelBuilding = true;

  nativeBuildInputs = [ cmake ];
}
