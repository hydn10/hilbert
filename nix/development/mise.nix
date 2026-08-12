{ pkgs, adapter }:
let
  # Use the default version once nixpkgs catches up: 0.23.2 predates stable CPS.
  gersemiPackage = import ../packages/gersemi.nix { inherit pkgs; };

  # Expose only the command so Gersemi's private Python does not leak into the dev shell.
  gersemiCommand = pkgs.writeShellScriptBin "gersemi" ''
    exec ${gersemiPackage}/bin/gersemi "$@"
  '';

  mappings = {
    clang-format = _: pkgs.llvmPackages_22.clang-tools;
    clang-tools = _: pkgs.llvmPackages_22.clang-tools;
    cmake = _: pkgs.cmake;
    gersemi = _: gersemiCommand;
    just = _: pkgs.just;
    ninja = _: pkgs.ninja;
    actionlint = _: pkgs.actionlint;
    oxfmt = _: pkgs.oxfmt;
    precious = _: pkgs.precious;
    shellcheck = _: pkgs.shellcheck;
    uv = _: pkgs.uv;
  };
in
adapter.mapTools {
  miseToml = ../../mise.toml;
  miseLock = ../../mise.lock;
} mappings
