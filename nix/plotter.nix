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

#  deps = {nixpkgs, ...}: {
#    pdm = nixpkgs.pdm;
#  };

  pdm.lockfile = ../python/pdm.lock;
  pdm.pyproject = ../python/pyproject.toml;
  #pdm.pythonInterpreter = config.deps.python3;

  mkDerivation = {
    src = lib.cleanSource ./..;
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