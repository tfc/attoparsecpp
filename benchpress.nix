{ stdenv, fetchFromGitHub, cmake, icu }:

stdenv.mkDerivation rec {
  name = "benchpress-${version}";
  version = "1";

  src = fetchFromGitHub {
      owner = "cjgdev";
      repo = "benchpress";
      rev = "56d5687dbfb18ee038d4b59705e31fcab4faea85";
      sha256 = "0myzxwmwdq8f4qb2wfwb984m62kq1322zc8000yz7pb8h8kpix6s";
  };

  nativeBuildInputs = [ cmake icu ];

  meta = {
    description = "benchpress lel";
  };
}
