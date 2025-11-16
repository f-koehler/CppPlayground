find_package(Doxygen REQUIRED)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR}/include/)

configure_file(conf.in.py ${CMAKE_CURRENT_SOURCE_DIR}/conf.py @ONLY)

add_custom_target(
    doc ALL
    DEPENDS doxygen
    COMMAND uv run sphinx-build -b html ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
