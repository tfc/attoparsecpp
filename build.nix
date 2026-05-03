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
      ./CMakeLists.txt
      ./benchmark
      ./gtest
      ./include
      ./pkg-config.pc.cmake
      ./test
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
