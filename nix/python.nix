{
  pkgs,
  uv2nix,
  pyproject-nix,
  pyproject-build-systems,
}:
let
  python = pkgs.python3;
  pyproject = builtins.fromTOML (builtins.readFile ../python/pyproject.toml);
  distributionName = pyproject.project.name;
  applicationScript = builtins.head (builtins.attrNames pyproject.project.scripts);

  workspace = uv2nix.lib.workspace.loadWorkspace {
    workspaceRoot = ../python;
  };

  overlay = workspace.mkPyprojectOverlay {
    sourcePreference = "wheel";
  };

  hacks = pkgs.callPackage pyproject-nix.build.hacks { };
  inherit (pkgs.callPackages pyproject-nix.build.util { }) mkApplication;

  pythonSet = (pkgs.callPackage pyproject-nix.build.packages { inherit python; }).overrideScope (
    pkgs.lib.composeManyExtensions [
      pyproject-build-systems.overlays.wheel
      overlay
      (_final: prev: {
        matplotlib = hacks.nixpkgsPrebuilt {
          from = python.pkgs.matplotlib;
          prev = prev.matplotlib;
        };
        tkinter = hacks.nixpkgsPrebuilt {
          from = python.pkgs.tkinter;
        };
      })
    ]
  );

  applicationVenv = pythonSet.mkVirtualEnv "${distributionName}-env" (
    workspace.deps.default // { tkinter = [ ]; }
  );

  application = mkApplication {
    venv = applicationVenv;
    package = pythonSet.${distributionName};
  };
in
{
  inherit
    application
    applicationScript
    distributionName
    python
    pythonSet
    ;
}
