#!/bin/sh

rm -rf $(find ./ -name "debug" -or -name "release" -or -name "Debug" -or -name "Release" -or -name ".svn" -or -name "ipch" -and -type d)
rm -rf $(find ./ -name "*~" -or -name "*.suo" -or -name "*.ncb" -or -name "*.user" -or -name "*.idb" -or -name "*.sdf" -or -name "*.vcproj" -or -name "*.filters")
# 以下src以下でのみ
rm -rf $(find src -name "*.lib")
rm -rf $(find src -name "*.pdb")
