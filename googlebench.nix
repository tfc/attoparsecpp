{ stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "googlebench-${version}";
  version = "1.3.0";

  src = fetchFromGitHub {
      owner = "google";
      repo = "benchmark";
      rev = "v${version}";
      sha256 = "1qx2dp7y0haj6wfbbfw8hx8sxb8ww0igdfrmmaaxfl0vhckylrxh";
  };

  nativeBuildInputs = [ cmake ];

  meta = {
    description = "google benchmark";
  };
}
