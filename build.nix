{ stdenv
, lib
, catch2_3
, clang-tools
, cmake
, gbenchmark
, gcovr
, llvm
, ninja
, gtest
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
    catch2_3
    gbenchmark
    gtest
  ]
  ++ lib.optional stdenv.cc.isClang llvm; # for llvm-cov

  inherit doCheck;

  nativeBuildInputs = [
    clang-tools
    cmake
    gcovr
    ninja
  ];
}
