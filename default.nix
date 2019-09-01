{
  nixpkgs ? <nixpkgs>,
  pkgs ? import nixpkgs {}
}:
let
  myStdenv = with pkgs; overrideCC stdenv gcc9;
in myStdenv.mkDerivation {
  name = "attoparsecpp";
  buildInputs = with pkgs; [ catch2 gbenchmark ];
  nativeBuildInputs = with pkgs; [ cmake ];

  src = ./.;
}
