{
  description = "C++ Project Template";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:nixos/nixpkgs";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
  };

  outputs = inputs: inputs.flake-parts.lib.mkFlake { inherit inputs; } {
    systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    perSystem = { config, pkgs, system, ... }: {
      devShells.default = pkgs.mkShell {
        inherit (config.checks.pre-commit-check) shellHook;
        inputsFrom = [ config.packages.attoparsec ];
        hardeningDisable = [ "all" ];
      };

      packages.default = config.packages.attoparsec;
      packages.attoparsec = pkgs.callPackage ./build.nix { };

      packages.coverage = config.packages.attoparsec.overrideAttrs (_: {
        hardeningDisable = [ "all" ];
        cmakeBuildType = "Coverage";
        postCheck = ''
          cmake --build . --target process_coverage
        '';
        installPhase = ''
          cp -r coverage $out
        '';
      });

      checks = {
        inherit (config.packages)
          coverage
          ;

        clang = config.packages.attoparsec.override {
          stdenv = pkgs.clangStdenv;
        };

        pre-commit-check = inputs.pre-commit-hooks.lib.${system}.run {
          src = ./.;
          hooks = {
            clang-format.enable = true;
            deadnix.enable = true;
            nixpkgs-fmt.enable = true;
            statix.enable = true;
          };
        };
      } // pkgs.lib.optionalAttrs (!pkgs.hostPlatform.isDarwin) {
        gcc = config.packages.attoparsec.override {
          stdenv = pkgs.gccStdenv;
        };
      };
    };
  };
}
