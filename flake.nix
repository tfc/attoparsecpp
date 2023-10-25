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

      packages =
        let
          attoparsec = pkgs.callPackage ./build.nix { };
        in
        {
          default = config.packages.attoparsec;
          attoparsec =
            if pkgs.stdenv.hostPlatform.isDarwin
            then config.packages.attoparsec-clang
            else config.packages.attoparsec-gcc;

          attoparsec-gcc = attoparsec.override {
            stdenv = pkgs.gcc13Stdenv;
          };

          attoparsec-clang = attoparsec.override {
            stdenv = pkgs.clang16Stdenv;
            clang-tools = pkgs.clang-tools_16;
          };

          coverage = config.packages.attoparsec.overrideAttrs (_: {
            hardeningDisable = [ "all" ];
            cmakeBuildType = "Coverage";
            postCheck = ''
              cmake --build . --target process_coverage
            '';
            installPhase = ''
              cp -r coverage $out
            '';
          });
        };

      checks = {
        inherit (config.packages)
          coverage
          attoparsec-clang
          ;

        pre-commit-check = inputs.pre-commit-hooks.lib.${system}.run {
          src = ./.;
          hooks = {
            clang-format.enable = true;
            deadnix.enable = true;
            nixpkgs-fmt.enable = true;
            statix.enable = true;
          };
        };
      } // pkgs.lib.optionalAttrs (!pkgs.hostPlatform.isDarwin) (
        # Sanitizers turned out to be complicated on macOS for now.
        let
          sansStr = pkgs.lib.concatMapStringsSep " " (x: "-fsanitize=${x}");
          toSanitized = _: sans: config.packages.attoparsec.overrideAttrs (_: {
            hardeningDisable = [ "all" ];
            cmakeBuildType = "Debug";
            preConfigure = ''
              cmakeFlagsArray+=(
                "-DCMAKE_CXX_FLAGS='-fsanitize=undefined ${sansStr sans}'"
              )
            '';
          });
        in
        builtins.mapAttrs toSanitized {
          sanitizer-ub-address = [ "undefined" "address" ];
          sanitizer-leak = [ "leak" ];
        }
      ) // pkgs.lib.optionalAttrs (!pkgs.hostPlatform.isDarwin) {
        inherit (config.packages) attoparsec-gcc;
      };
    };
  };
}
