{ lib, stdenv, cmake, fftw }:

let
  vcpkgFile = builtins.fromJSON (builtins.readFile ./vcpkg.json);

  pname = vcpkgFile.name;
  version = vcpkgFile.version;
in
  stdenv.mkDerivation
  {
    inherit pname;

    name = "${pname}-${version}";
    inherit version;

    src = lib.cleanSource ./.;

    buildInputs = [ fftw ];

    nativeBuildInputs = [ cmake ];
  }
