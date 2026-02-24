{
  description = "femaster-lab-1";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...} @ inputs: let
    forAllSystems = nixpkgs.lib.genAttrs [
      "aarch64-linux"
      "x86_64-linux"
    ];
  in {
    devShells = forAllSystems (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        default = pkgs.mkShell {
          name = "femaster-lab-1";
          NIX_CONFIG = "experimental-features = nix-command flakes";
          shellHook = ''
          '';
          nativeBuildInputs = with pkgs; [
            lldb
            clang
            clang-tools
          ];
          buildInputs = with pkgs; [
            cmake
            pkg-config
            lld
            # libcxx
            gnumake
            bear
            eigen
          ];
        };
      }
    );
  };
}
