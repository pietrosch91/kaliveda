#---WRITE_EXAMPLES_INDEX(subproject)

set(html_filename ${CMAKE_BINARY_DIR}/${subproject}/index.html)
file(WRITE ${html_filename} "<h2>Examples of use of ${subproject} classes</h2>\n")
file(APPEND ${html_filename} "<ol>\n")
