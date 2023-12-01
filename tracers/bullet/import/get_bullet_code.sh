#/bin/sh
set -e

# get_bullet_code.sh
# Author: Kris J. Becker 2023-11-29 - University of Arizona

# This script extracts the Bullet implementation from the UA/ISIS code while
# preserving commit history and preps it for initial import into PSMRTS.

# First, ensure git_filter_repo is installed on your system. I simply
# downloaded from https://github.com/newren/git-filter-repo/tree/main and
# put it in ~/bin.  Be sure to make it executable using the command
# chmod 755 ~/bin/git_filter_repo. This is a Python script so ensure
# Python 3 is in your path. Also, it requires git >= 2.24.

# In directory "ImportFromIsis/IsisTransfers"
git clone http://orgit:7990/scm/isis/usgsisis36.git bulletpsmrts

# Using a tar file backup rather than the clone option
# /bin/rm -rf usgsisis36 bulletpsmrts
# tar zxvf usgsisis36.tar.gz
# mv usgsisis36 bulletpsmrts

cd bulletpsmrts

## FILTER FROM UA/ISIS

# Extract the desired directories
git filter-repo --path isis/src/base/objs/BulletDskShape/ \
                --path isis/src/base/objs/BulletMeshMapper/ \
                --path isis/src/base/objs/BulletObjShape/ \
                --path isis/src/base/objs/BulletPrioritizedShapes/ \
                --path isis/src/base/objs/BulletShapeFactor/ \
                --path isis/src/base/objs/BulletShapeModel/ \
                --path isis/src/base/objs/BulletTargetShape/ \
                --path isis/src/base/objs/BulletWorldManager/

# Remove similar files and unneeded files before consolidation
git filter-repo --invert-paths --use-base-name \
                --path Makefile \
                --path tiny_obj_loader.h \
                --path assets/ \
                --path unitTest.cpp \
                --path-glob '*.obj' \
                --path-glob '*.lbl' \
                --path-glob '*.truth'

# Put all the files in a single directory
git filter-repo --path-rename isis/src/base/objs/BulletDskShape/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletMeshMapper/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletObjShape/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletPrioritizedShapes/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletShapeFactor/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletShapeModel/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletTargetShape/:tracers/bullet/src/ \
                --path-rename isis/src/base/objs/BulletWorldManager/:tracers/bullet/src/


# Remove all extraneous branches to clean it up
git branch | grep -v main | xargs git branch -D

# Reset the origin
git remote add origin http://orgit:7990/scm/isis/psmrts.git

## IMPORT INTO PSMRTS

# Now to import Bullet into PSMRTS, assume we have a Bullet filtered code in
# directory ./ImportFromIsis/IsisTransfers/bulletpsmrts
git checkout -b bullet-filter-from-uaisis

# Now create directory to clone psmrts
cd ../..     # default dir now ./ImportFromIsis
git clone http://orgit:7990/scm/isis/psmrts.git
cd psmrts
git checkout -b import-bullet-from-uaisis
git remote add bullet-repo-source ../IsisTransfers/bulletpsmrts
git fetch bullet-repo-source
git branch bullet-import-source remotes/bullet-repo-source/bullet-filter-from-uaisis
git merge bullet-import-source --allow-unrelated-histories

# Publish the branch to the PSMRTS repo
git push -u origin import-bullet-from-uaisis
