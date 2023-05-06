with import <nixpkgs> { }; {
  md380toolsEnv = stdenv.mkDerivation {
    name = "md380toolsEnv";
    buildInputs = [
      curl
      gcc-arm-embedded-9
      gnumake
      libusb1
      perl
      python27
      (pkgs.python27Packages.buildPythonPackage rec {
        pname = "pyusb";
        version = "1.1.0"; # the last Python 2 version
        src = pkgs.python27Packages.fetchPypi {
          inherit pname version;
          sha256 = "sha256-1p7WS/8OIQLaEbP0lWclaGeFO4YReGiWcaFj0whlwpg=";
        };
        doCheck = false;
        propagatedBuildInputs = [
          pkgs.python27Packages.setuptools_scm
        ];
      })
      unzip
      wget
      which
    ];
    LD_LIBRARY_PATH = "${libusb1.out}/lib";
  };
}
