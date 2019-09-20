################################################################################
#                                                                              #
# Copyright (C) 2019 Fondazione Istitito Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################
# @authors: Michele Colledanchise <michele.colledanchise@iit.it>
#           Alberto Cardellino <alberto.cardellino@iit.it>

# execute_process(COMMAND bash -c "readskill.native sklist.xml")                      # --> genera skill.ml
# execute_process(COMMAND bash -c "ocamlbuild -use-ocamlfind interpreter.native.o")   # --> genera interpreter.native.o che rinominiamo modcaml.o (o come ci piace)

set(SRCS BT_engine_unige.cpp)

set(OBJS objs/modcaml.o)                        # compiled object from OCaml
set(BINDING_SRCS  wrap.c                        # wrap c/c++
                  ExecSkill_wrap.cpp)           # bridge verso SmartSoft // probably not requireed here

set(OTHER_SRCS  tinyXML2/tinyxml2.cpp)


set_source_files_properties(${OBJS}
                            PROPERTIES
                            EXTERNAL_OBJECT true
                            GENERATED true
                            )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# create the executable using collected source-files
add_executable(BT_engine_unige ${SRCS} ${BINDING_SRCS} ${OTHER_SRCS} ${OBJS} )

# add_library(wrap SHARED ${OCAML_SRCS})

# findPackage(ocaml)  # --> cartella con le librerie /usr/lib/ocaml)
# link your external libraries here (if there are any)
target_link_libraries(BT_engine_unige   -L/usr/lib/ocaml
                                        asmrun
                                        unix
                                        dl
                                        m)

target_link_libraries(BT_engine_unige YARP::YARP_init bt_cpp_modules)
