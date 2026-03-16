{
    description = "ncurses braille experiments";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    };

    outputs = {self, nixpkgs}: let
        system = "x86_64-linux";
        pkgs = import nixpkgs { inherit system; };
    in {
        devShells.${system}.default = pkgs.mkShell {
            packages = [
                pkgs.ncurses
                pkgs.gnumake
                pkgs.gcc
                pkgs.clang-tools
                pkgs.gdb
            ];
        };
    };
}
