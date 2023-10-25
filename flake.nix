{
  description = "C++ Project Template";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:nixos/nixpkgs";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
  };

  outputs = inputs: inputs.flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    perSystem = { config, self', inputs', pkgs, system, ... }: {
      devShells.default = pkgs.mkShell {
        inherit (config.checks.pre-commit-check) shellHook;
        inputsFrom = [ config.packages.attoparsec ];
      };

      packages.default = config.packages.attoparsec;
      packages.attoparsec = pkgs.callPackage ./build.nix { };

      checks = {
        gcc = config.packages.attoparsec.override { stdenv = pkgs.gccStdenv; };
        clang = config.packages.attoparsec.override { stdenv = pkgs.clangStdenv; };

        pre-commit-check = inputs.pre-commit-hooks.lib.${system}.run {
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
