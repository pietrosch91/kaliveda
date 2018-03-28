# Set up directory in order to update doxygen class documentation
# Run this script in an empty directory like so:
#
#    [path to source directory]/doc/doxygen prepare_doxy_update.sh
#
KV_SOURCE=$(kaliveda-config --srcdir)

ln -s $KV_SOURCE kaliveda.git
git clone --single-branch -b gh-pages git@github.com:kaliveda-dev/kaliveda.git html

# remove history from gh-pages
cd html
git reset --hard $(git rev-list --max-parents=0 --abbrev-commit HEAD)
cd ..

# copy scripts etc.
list=$(ls $KV_SOURCE/doc/doxygen)
for f in $list; do
   ln -s $KV_SOURCE/doc/doxygen/$f
done
