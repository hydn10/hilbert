{
  description = "Hilbert suspension analysis and visualization";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";

    vcpkg-nix-adapter = {
      url = "github:hydn10/vcpkg-nix-adapter/v0.2.0";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    mise-nix-adapter = {
      url = "github:hydn10/mise-nix-adapter/v0.1.0";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    pyproject-nix = {
      url = "github:pyproject-nix/pyproject.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    uv2nix = {
      url = "github:pyproject-nix/uv2nix";
      inputs.pyproject-nix.follows = "pyproject-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    pyproject-build-systems = {
      url = "github:pyproject-nix/build-system-pkgs";
      inputs.pyproject-nix.follows = "pyproject-nix";
      inputs.uv2nix.follows = "uv2nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      vcpkg-nix-adapter,
      mise-nix-adapter,
      uv2nix,
      pyproject-nix,
      pyproject-build-systems,
    }:
    let
      supportedSystems = [ "x86_64-linux" ];

      mkSystemOutputs =
        system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          workspace = import ./nix {
            inherit
              pkgs
              uv2nix
              pyproject-nix
              pyproject-build-systems
              ;
            vcpkgAdapter = vcpkg-nix-adapter.lib;
            miseAdapter = mise-nix-adapter.lib;
          };

          projectName = workspace.project.package.pname;
          nativeAppName = "${projectName}-cli";
          pythonAppName = workspace.python.applicationScript;
        in
        {
          packages = {
            default = self.packages.${system}.${projectName};
            "${projectName}" = workspace.project.package;
            "${nativeAppName}" = workspace.project.packageWithApps;
            "${pythonAppName}" = workspace.python.application;
          };

          apps = {
            default = self.apps.${system}.${nativeAppName};

            "${nativeAppName}" = {
              type = "app";
              program = "${workspace.project.packageWithApps}/bin/${nativeAppName}";
              meta.description = "Run the Hilbert suspension simulation.";
            };

            "${pythonAppName}" = {
              type = "app";
              program = "${workspace.python.application}/bin/${pythonAppName}";
              meta.description = "Validate or plot Hilbert simulation CSV data.";
            };
          };

          devShells.default = pkgs.mkShell {
            inputsFrom = [ workspace.project.package ];
            packages = workspace.development.mise.packages ++ workspace.python.shellPackages;
            env = workspace.python.shellEnv;
          };

          formatter = pkgs.nixfmt-tree;

          checks = {
            cpp-quality = workspace.project.qualityCheck;
            python-plotter = workspace.python.application;
            data-contract = workspace.dataContractCheck;
            nix-format = self.formatter.${system}.check self;
          };
        };

      perSystem = nixpkgs.lib.genAttrs supportedSystems mkSystemOutputs;
    in
    {
      packages = nixpkgs.lib.mapAttrs (_: outputs: outputs.packages) perSystem;
      apps = nixpkgs.lib.mapAttrs (_: outputs: outputs.apps) perSystem;
      devShells = nixpkgs.lib.mapAttrs (_: outputs: outputs.devShells) perSystem;
      formatter = nixpkgs.lib.mapAttrs (_: outputs: outputs.formatter) perSystem;
      checks = nixpkgs.lib.mapAttrs (_: outputs: outputs.checks) perSystem;
    };
}
