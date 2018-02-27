{ pkgs ? import <nixpkgs> { config.allowBroken = true; } }:
let
  stdenv = pkgs.stdenv;
in rec {
  myProject = stdenv.mkDerivation {
    name = "attoparsecpp";
    version = "dev-0.1";
    buildInputs = with pkgs; [
      (callPackage ./catch.nix {})
      (callPackage ./benchpress.nix {})
      gcc
      gnumake
      gbenchmark
      gtest
    ];
  };
}
