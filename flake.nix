{
  description = "Raylib development environment";

  inputs = { nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05"; };

  outputs = { self, nixpkgs, ... }:
    let system = "x86_64-linux";
    in {
      devShells."${system}".default =
        let pkgs = import nixpkgs { inherit system; };
        in pkgs.mkShell {
          packages = [
            pkgs.libGL

            # X11 dependencies
            pkgs.xorg.libX11
            pkgs.xorg.libX11.dev
            pkgs.xorg.libXcursor
            pkgs.xorg.libXi
            pkgs.xorg.libXinerama
            pkgs.xorg.libXrandr

            pkgs.just
            pkgs.raylib
            # pkgs.emscripten # websupport for raylib

            pkgs.clang-tools
          ];

          # Audio dependencies
          LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [ pkgs.alsa-lib ];

          inputsFrom = [
            pkgs.raylib
          ];
        };
    };
}
