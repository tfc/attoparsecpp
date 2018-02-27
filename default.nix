{
    pkgs   ? import <nixpkgs> {},
    stdenv ? pkgs.stdenv
}:
rec {
  myProject = stdenv.mkDerivation {
    name = "attoparsecpp";
    version = "dev-0.1";
    buildInputs = with pkgs; [
      (callPackage ./catch.nix {})
      (callPackage ./googlebench.nix {})
      gnumake
      gtest
    ];
  };
}
