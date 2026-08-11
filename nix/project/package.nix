{
  lib,
  stdenv,
  cmake,
  ninja,
  targetBuildInputs,
  hostBuildInputs,
  consumerBuildInputs,
  buildApps ? false,
  buildTests ? false,
}:
let
  projectRoot = ../../.;
  manifest = builtins.fromJSON (builtins.readFile (projectRoot + "/vcpkg.json"));
  cmakeBoolean = value: if value then "ON" else "OFF";
in
stdenv.mkDerivation {
  pname = manifest.name;
  inherit (manifest) version;

  src = lib.cleanSource projectRoot;
  strictDeps = true;

  nativeBuildInputs = [
    cmake
    ninja
  ]
  ++ hostBuildInputs;
  buildInputs = targetBuildInputs;
  propagatedBuildInputs = consumerBuildInputs;

  cmakeGenerator = "Ninja";
  cmakeFlags = [
    "-DHILBERT_BUILD_APPS=${cmakeBoolean buildApps}"
    "-DHILBERT_BUILD_TESTING=${cmakeBoolean buildTests}"
  ];

  doCheck = buildTests;

  meta = {
    description = "Hilbert-transform tools for automotive suspension analysis";
    license = lib.licenses.mit;
    platforms = lib.platforms.unix;
  };
}
