with import <nixpkgs> {}; {
  md380toolsEnv = stdenv.mkDerivation {
    name = "md380toolsEnv";
    buildInputs = [
      curl
      gcc-arm-embedded
      gnumake
      libusb1
      perl
      python27
      python27Packages.pyusb
      unzip
      wget
      which
    ];
    LD_LIBRARY_PATH="${libusb1.out}/lib";
  };
}
