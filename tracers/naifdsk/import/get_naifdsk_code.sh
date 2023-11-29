#/bin/sh
set -e

# get_naifdsk_code.sh
# Author: Kris J. Becker 2023-11-29 - University of Arizona

# This script extracts the NAIF DSK implementation from the UA/ISIS code while
# preserving commit history and preps it for initial import into PSMRTS.

# First, ensure git_filter_repo is installed on your system. I simply
# downloaded from https://github.com/newren/git-filter-repo/tree/main and
# put it in ~/bin.  Be sure to make it executable using the command
# chmod 755 ~/bin/git_filter_repo. This is a Python script so ensure
# Python 3 is in your path. Also, it requires git >= 2.24.

# In directory "ImportFromIsis/IsisTransfers"
git clone http://orgit:7990/scm/isis/usgsisis36.git naifpsmrts

# Using the tar fil rather than the clone option
# bin/rm -rf usgsisis36 naifpsmrts
# tar zxvf usgsisis36.tar.gz
# mv usgsisis36 naifpsmrts

cd naifpsmrts

## FILTER FROM UA/ISIS

# Extract the desired directories
git filter-repo --path isis/src/base/objs/NaifDskPlateModel/ \
                --path isis/src/base/objs/NaifDskShape/

# Remove similar files and unneeded files before consolidation
git filter-repo --invert-paths --use-base-name \
                --path Makefile \
                --path unitTest.cpp \
                --path-glob '*.lbl' \
                --path-glob '*.truth'

# Put all the files in a single directory
git filter-repo --path-rename isis/src/base/objs/NaifDskPlateModel/:tracers/naifdsk/src/ \
                --path-rename isis/src/base/objs/NaifDskShape/:tracers/naifdsk/src/

# Remove all extraneous branches to clean it up
git branch | grep -v main | xargs git branch -D

# Reset the origin
git remote add origin http://orgit:7990/scm/isis/psmrts.git

## IMPORT INTO PSMRTS

# Now to import NAIF DSK into PSMRTS, assume we have a NAIF DKS filtered code
# in directory ./ImportFromIsis/IsisTransfers/naifpsmrts
git checkout -b naifdsk-filter-from-uaisis

# Now create directory to clone psmrts
cd ../..     # default dir now ./ImportFromIsis
git clone http://orgit:7990/scm/isis/psmrts.git
cd psmrts
git checkout -b import-naifdsk-from-uaisis
git remote add naifdsk-repo-source ../IsisTransfers/naifpsmrts
git fetch naifdsk-repo-source
git branch naifdsk-import-source remotes/naifdsk-repo-source/naifdsk-filter-from-uaisis
git merge naifdsk-import-source --allow-unrelated-histories

# Publish the branch to the PSMRTS repo
git push -u origin import-naifdsk-from-uaisis

