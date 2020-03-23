{
  nixpkgs ? <nixpkgs>,
  pkgs ? import nixpkgs {}
}:
let
  myStdenv = with pkgs; overrideCC stdenv gcc9;
in pkgs.stdenv.mkDerivation {
  name = "attoparsecpp";
  buildInputs = with pkgs; [ catch2 gbenchmark ];
  nativeBuildInputs = with pkgs; [ cmake ];

  src = ./.;
}
