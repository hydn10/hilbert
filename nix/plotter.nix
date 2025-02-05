{
  config,
  lib,
  dream2nix,
  ...
}:
{
  imports = [
    dream2nix.modules.dream2nix.WIP-python-pdm
  ];

  pdm.lockfile = ../python/pdm.lock;
  pdm.pyproject = ../python/pyproject.toml;

  mkDerivation = {
    src = lib.cleanSource ../python;
    buildInputs = [
      config.deps.python3.pkgs.pdm-backend
    ];
  };

  buildPythonPackage = {
    pythonImportsCheck = [
      "plotter"
    ];
  };
}
