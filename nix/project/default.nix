{ pkgs, adapter }:
let
  fftw = import ../packages/fftw.nix { inherit pkgs; };

  vcpkgDependencies =
    adapter.mapDependencies
      {
        vcpkgJson = ../../vcpkg.json;
      }
      {
        fftw3 = _: fftw;
      };

  package = pkgs.callPackage ./package.nix {
    targetBuildInputs = vcpkgDependencies.root.targetPackages;
    hostBuildInputs = vcpkgDependencies.root.hostPackages;
    consumerBuildInputs = [ fftw ];
  };

  packageWithApps = package.override {
    buildApps = true;
  };

  qualityCheck =
    (package.override {
      buildApps = true;
      buildTests = true;
    }).overrideAttrs
      (oldAttrs: {
        nativeBuildInputs = (oldAttrs.nativeBuildInputs or [ ]) ++ [ pkgs.llvmPackages_22.clang-tools ];
        cmakeFlags = (oldAttrs.cmakeFlags or [ ]) ++ [
          "-DCMAKE_CXX_CLANG_TIDY=clang-tidy;--warnings-as-errors=*"
          "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
        ];
        doCheck = true;
        preCheck = (oldAttrs.preCheck or "") + ''
          cmake --build . --target all_verify_interface_header_sets
        '';
      });

  installedConsumerCheck = pkgs.stdenv.mkDerivation {
    pname = "hilbert-installed-consumer";
    inherit (package) version;

    src = ../../tests/consumer-cmake;
    strictDeps = true;

    nativeBuildInputs = [
      pkgs.cmake
      pkgs.ninja
    ];
    buildInputs = [ package ];

    cmakeGenerator = "Ninja";
    cmakeFlags = [
      "-DHILBERT_EXPECT_FFTW_TARGET=ON"
      "-DHILBERT_PACKAGE_VERSION=${package.version}"
    ];

    doCheck = true;

    installPhase = ''
      runHook preInstall
      touch "$out"
      runHook postInstall
    '';
  };

  fftwPackageCheck = pkgs.stdenv.mkDerivation {
    pname = "fftw-cmake-package-consumer";
    inherit (fftw) version;

    src = ../../tests/fftw-consumer-cmake;
    strictDeps = true;

    nativeBuildInputs = [
      pkgs.cmake
      pkgs.ninja
    ];
    buildInputs = [ fftw ];

    cmakeGenerator = "Ninja";
    doCheck = true;

    installPhase = ''
      runHook preInstall
      touch "$out"
      runHook postInstall
    '';
  };
in
{
  inherit
    fftw
    fftwPackageCheck
    installedConsumerCheck
    package
    packageWithApps
    qualityCheck
    vcpkgDependencies
    ;
}
