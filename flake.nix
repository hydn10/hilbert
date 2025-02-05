{
  description = "";

  inputs = {
    dream2nix.url = "github:nix-community/dream2nix";
    nixpkgs.follows = "dream2nix/nixpkgs";
  };

  outputs = { self, nixpkgs, dream2nix }:
    let
      pkgs-lin64 = import nixpkgs { system = "x86_64-linux"; };
      packageDrv-lin64 = pkgs-lin64.callPackage ./default.nix {};

      pkgName = packageDrv-lin64.pname;

      plotter = dream2nix.lib.evalModules {
        packageSets.nixpkgs = dream2nix.inputs.nixpkgs.legacyPackages.x86_64-linux;
        modules = [
          nix/plotter.nix
          ({lib, ...}: {
            paths.projectRoot = "python";
            paths.projectRootFile = "pyproject.toml";
            paths.package = ./.;
            mkDerivation = lib.mkForce {
              src = "python";
            };
          })
        ];
      };
    in
    {
      apps.x86_64-linux.${pkgName} = 
      {
        type = "app";
        program = "${packageDrv-lin64}/bin/${pkgName}";
      };

      apps.x86_64-linux.plotter =
      {
        type = "app";
        program = "${plotter}";
      };

      apps.x86_64-linux.default = self.apps.x86_64-linux.${pkgName};

      overlays.default = final: prev: { ${pkgName} = self.packages.x86_64-linux.${pkgName}; };

      packages.x86_64-linux.${pkgName} = packageDrv-lin64;

      packages.x86_64-linux.plotter = plotter;

      packages.x86_64-linux.default = self.packages.x86_64-linux.${pkgName};

      devShells.x86_64-linux.default = import ./shell.nix {
        pkgs = pkgs-lin64;
        pkg = packageDrv-lin64;
      };
    };
}
