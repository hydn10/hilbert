{
  pkgs,
  vcpkgAdapter,
  miseAdapter,
  uv2nix,
  pyproject-nix,
  pyproject-build-systems,
}:
let
  project = import ./project {
    inherit pkgs;
    adapter = vcpkgAdapter;
  };

  python = import ./python.nix {
    inherit
      pkgs
      uv2nix
      pyproject-nix
      pyproject-build-systems
      ;
  };

  development.mise = import ./development/mise.nix {
    inherit pkgs;
    adapter = miseAdapter;
  };

  dataContractCheck =
    pkgs.runCommand "hilbert-data-contract"
      {
        nativeBuildInputs = [
          project.packageWithApps
          python.application
        ];
      }
      ''
        hilbert-cli --duration 1 --output simulation.csv
        plotter validate simulation.csv
        touch "$out"
      '';
in
{
  inherit
    dataContractCheck
    development
    project
    python
    ;
}
