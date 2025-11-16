configure_file(Doxyfile.in Doxyfile @ONLY)

find_package(Doxygen REQUIRED)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR}/include/CppPlayground)
