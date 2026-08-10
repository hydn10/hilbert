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

  shellPackages = [ python ];
  shellEnv = {
    UV_PYTHON_DOWNLOADS = "never";
    UV_PYTHON = python.interpreter;
    PYTHONPATH = "${python.pkgs.tkinter}/${python.sitePackages}";
  }
  // pkgs.lib.optionalAttrs pkgs.stdenv.hostPlatform.isLinux {
    LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
      pkgs.stdenv.cc.cc.lib
      pkgs.zlib
      pkgs.libx11
      pkgs.wayland
    ];
  };
}
