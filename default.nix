{ lib, gcc14Stdenv, cmake, fftw }:

let
  vcpkgFile = builtins.fromJSON (builtins.readFile ./vcpkg.json);

  pname = vcpkgFile.name;
  version = vcpkgFile.version;
in
  gcc14Stdenv.mkDerivation
  {
    inherit pname;

    name = "${pname}-${version}";
    inherit version;

    src = lib.cleanSource ./.;

    buildInputs = [ fftw.dev ];

    nativeBuildInputs = [ cmake ];

    #cmakeFlags = [
    #  "-DCMAKE_PREFIX_PATH=${fftw.dev}/lib/cmake"
    #  "-DFFTW3_ROOT=${fftw.dev}"
    #];
  }
