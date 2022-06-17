sha1=$(git rev-parse --short HEAD)
if git diff --shortstat 2>/dev/null | read dummy; then sha1="${sha1}-dirty" ; fi
echo "Generating automatically the version header file with $sha1 version."
mkdir -p .build/sketch
cat > .build/sketch/version.h << EOF
#ifndef _VERSION_H
#define _VERSION_H
#define VERSION "${sha1}"
#endif //define _VERSION_H
EOF
