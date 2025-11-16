find_package(Doxygen REQUIRED)
set(DOXYGEN_GENERATE_HTML NO)
set(DOXYGEN_GENERATE_XML YES)
doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR}/include/)

configure_file(conf.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
file(COPY index.rst DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY _static DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY _templates DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(
    doc ALL
    DEPENDS doxygen conf.py index.rst _static _templates
    COMMAND uv run sphinx-build -b html ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
