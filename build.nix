{ stdenv
, catch2
, ninja
, gbenchmark
, gcovr
, llvm
, cmake
, lib
, clang-tools
, doCheck ? true
}:

stdenv.mkDerivation {
  name = "attoparsecpp";

  src = lib.fileset.toSource {
    root = ./.;
    fileset = lib.fileset.unions [
      ./include
      ./test
      ./benchmark
      ./CMakeLists.txt
      ./pkg-config.pc.cmake
    ];
  };

  checkInputs = [
    catch2
    gbenchmark
  ]
  ++ lib.optional stdenv.cc.isClang llvm; # for llvm-cov

  inherit doCheck;

  enableParallelBuilding = true;

  nativeBuildInputs = [
    clang-tools
    cmake
    gcovr
    ninja
  ];
}
