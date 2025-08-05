{
  pkgs,
  lib,
  ...
}:
let
  clangPackage = pkgs.llvmPackages_20.clangUseLLVM;
in

{
  env = {
    VCPKG_ROOT = "${pkgs.vcpkg}/share/vcpkg";
  };
  enterShell = ''
    export CC="${lib.getExe' clangPackage "clang"}";
    export CXX="${lib.getExe' clangPackage "clang++"}";
  '';
  packages = [
    pkgs.git
    pkgs.vcpkg
    pkgs.cmake
    clangPackage
  ];

  git-hooks.hooks = {
    clang-format = {
      enable = true;
      types_or = [
        "c++"
        "c"
      ];
    };
    clang-tidy.enable = true;
    cppcheck = {
      enable = true;
      types = [ "c++" ];
      entry = ''
        ${lib.getExe' pkgs.cppcheck "cppcheck"} \
            --check-level=exhaustive \
            --enable=performance \
            --enable=portability \
            --enable=style \
            --enable=warning \
            --library=qt \
            --error-exitcode=1 \
            --inline-suppr \
            --suppress=unusedStructMember
      '';
    };
    flawfinder = {
      enable = true;
      pass_filenames = false;
      entry = "${lib.getExe pkgs.flawfinder} --error-level=0 ./src";
    };

    cmake-format = {
      enable = true;
      types = [ "cmake" ];
      entry = "${lib.getExe' pkgs.cmake-format "cmake-format"} -c .cmake.yaml -i";
    };
    cmake-lint = {
      enable = true;
      types = [ "cmake" ];
      entry = "${lib.getExe' pkgs.cmake-format "cmake-lint"} -c .cmake.yaml";
    };

    deadnix.enable = true;
    flake-checker.enable = true;
    nil.enable = true;
    nixfmt-rfc-style.enable = true;
    statix.enable = true;

    prettier.enable = true;
  };
}
