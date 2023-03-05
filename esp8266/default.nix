{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = [ pkgs.arduino-cli pkgs.python311 pkgs.python311Packages.pyserial ];
}
