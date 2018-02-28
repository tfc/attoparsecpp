{
    pkgs   ? import <nixpkgs> {},
    stdenv ? pkgs.stdenv
}:
rec {
  myProject = stdenv.mkDerivation {
    name = "attoparsecpp";
    version = "0.1";

    src = ./.;
    checkPhase = ''
      make -C test check
      make -C benchmark check
    '';

    installPhase = ''
      mkdir -p $out/include
      cp -r include/attoparsecpp $out/include/
    '';

    buildInputs = with pkgs; [
      (callPackage ./catch.nix { })
      (callPackage ./googlebench.nix { stdenv = stdenv; })
    ];
  };
}
