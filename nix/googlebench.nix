{ stdenv
, googlebenchSrc
, cmake
, gtest
}:

stdenv.mkDerivation rec {
  name = "googlebench";

  src = googlebenchSrc;

  cmakeFlags = [ "-DBENCHMARK_ENABLE_GTEST_TESTS=OFF" ];

  nativeBuildInputs = [ cmake ];

  meta = {
    description = "google benchmark";
  };
}
