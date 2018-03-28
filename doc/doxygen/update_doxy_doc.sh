# - Run script to update local doxygen doc in html directory

DOXYFILE=Doxyfile

# reformat comments
kaliveda -b -q

export KALIVEDA_VERSION=$(kaliveda-config --version)
cp kaliveda.git/README.md kaliveda.doxygen/
export INPUT_DIR=kaliveda.doxygen && doxygen $DOXYFILE
