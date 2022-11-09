{
  description = "C++ Project Template";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:nixos/nixpkgs";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
  };

  outputs =
    { self
    , flake-parts
    , nixpkgs
    , pre-commit-hooks
    }:
    flake-parts.lib.mkFlake { inherit self; } {
      systems = [ "x86_64-linux" "aarch64-linux" ];
      perSystem = { config, self', inputs', pkgs, system, ... }:
        let
          attoparsec = pkgs.callPackage ./build.nix { };
        in
        {
          devShells.default = pkgs.mkShell {
            shellHook = ''
              ${config.checks.pre-commit-check.shellHook}
            '';
            nativeBuildInputs = with pkgs; [
            ];
            inputsFrom = [ attoparsec ];
          };

          packages.default = attoparsec;

          checks = {
            gcc = attoparsec.override { stdenv = pkgs.gccStdenv; };
            clang = attoparsec.override { stdenv = pkgs.clangStdenv; };

            pre-commit-check = pre-commit-hooks.lib.${system}.run {
              src = ./.;
              hooks = {
                nixpkgs-fmt.enable = true;
                statix.enable = true;
              };
            };
          };
        };
    };
}
