{
  description = "m20 build flake";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      formatter.${system} = pkgs.nixfmt-tree;
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gnumake
          openocd
          gcc-arm-embedded
        ];
      };
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        name = "m20";
        version = "unstable";

        src = ./.;

        nativeBuildInputs = [ pkgs.gcc-arm-embedded ];
        enableParallelBuilding = true;

        installPhase = ''
          mkdir -p $out/
          cp -r build $out/
          cp -r openocd $out/
          cat > $out/flash <<EOF
            #!/usr/bin/env bash
            set -e
            ${pkgs.openocd}/bin/openocd -f $out/openocd/stlink.cfg -f $out/openocd/stm32l0.cfg -c "program $out/build/m20.elf verify reset exit"
          EOF

            chmod +x $out/flash
        '';
      };
    };
}
