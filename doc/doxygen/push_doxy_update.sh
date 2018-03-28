# - Commit and push updated doc to github server

# commit modifications to source code
cd kaliveda.git
git commit -a

# commit doc
cd ../html
git add .
git commit -m "Doxygen update $(kaliveda-config --gitinfos)"
git push -f origin gh-pages
cd ..
