let
  sources = import ./nix/sources.nix {};
  pkgs = import sources.nixpkgs {};

  googlebench = pkgs.callPackage ./nix/googlebench.nix {
    googlebenchSrc = sources.benchmark;
  };

in {
  attoparsecpp = pkgs.callPackage ./nix/build.nix {
    stdenv = pkgs.gcc11Stdenv;
    inherit googlebench;
  };
}
