@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass "& {& '%~dp0scripts\compile.ps1' %*}"