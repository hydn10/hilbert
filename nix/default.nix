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
in
{
  inherit
    development
    project
    python
    ;
}
